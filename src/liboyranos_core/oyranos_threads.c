/** @file oyranos_threads.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    thread methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2014/01/24
 *
 *  This file contains functions for thread handling.
 */

#include "oyranos_threads.h"

#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"
#include "oyranos_string.h"

#include <unistd.h> /* usleep() */
#ifdef _OPENMP
#define USE_OPENMP 1
#include <omp.h> /* omp_get_num_procs() */
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

oyThread_t * oy_threads_ = NULL;
int oy_thread_count_ = 0;

/**
 *  @brief   start a thread with a given function
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/01/25
 *  @since   2014/01/25 (Oyranos: 0.9.5)
 */
int oyThreadCreate                   ( void             *(*func) (void * ptr),
                                       void              * data,
                                       oyThread_t        * thread )
{
  int error = !thread || !func;

  if(!error)
  {
#if defined(WIN32) && !defined(__GNU__)
    *thread = (oyThread_t) _beginthread( func, 0, data );
    if(!(*thread))
      error = 1;
#else
    pthread_attr_t attr;

    error = pthread_attr_init( &attr );
    error = pthread_create( thread, &attr, func, data );
    error = pthread_attr_destroy( &attr );
#endif
  }

  return error;
}

typedef struct {
  oyMutex_t m;
  oyThread_t t;
  int ref;
} oyMutex_s;

/* define our own mutex based locking */
oyPointer  oyStruct_LockCreate_        ( oyStruct_s      * obj )
{
  oyMutex_s * ms = (oyMutex_s*) calloc(sizeof(oyMutex_s),1);

#if defined(WIN32) && !defined(__GNU__)
  int mattr = 0;
#else
  pthread_mutexattr_t mattr_local;
  pthread_mutexattr_t * mattr = &mattr_local;
  pthread_mutexattr_init( mattr );
  pthread_mutexattr_settype( mattr, PTHREAD_MUTEX_RECURSIVE );
#endif
  oyMutexInit_m( &ms->m, mattr );
  ms->t = oyThreadSelf();
  ms->ref = 0;
  return ms;
}
void       oyLockRelease_              ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )
{
  oyMutex_s * ms = (oyMutex_s*)lock;
  if(ms->ref != 0)
    WARNc3_S("%s %d ref counter=%d", marker, line, ms->ref);
    
  oyMutexDestroy_m( &ms->m );
  /* paranoid */
  ms->ref = -10000;
  free(ms);
}

int oyGetThreadID( oyThread_t t )
{
  int i;
  for(i = 0; i < oy_thread_count_; ++i)
    if      (oyThreadEqual( oy_threads_[i], t ))
      break;
  return i;
}

void       oyLock_                     ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )
{ 
  oyMutex_s * ms = (oyMutex_s*)lock;
  oyThread_t t = oyThreadSelf();

  {
    DBG_PROG4_S("%s %d thread[%d] ref:%d", marker, line, oyGetThreadID( t ), ms->ref );
    /* Get the resource, and probably wait for it. */
    oyMutexLock_m( &ms->m );
    /* As soon as we are able to lock, the mutex is owned by us. */
    ms->t = t;
    /* This is the first lock call to the resource, mark the oyMutex_s accordingly. */
    ms->ref++;

  }
}
void       oyUnLock_                   ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )
{
  oyMutex_s * ms = (oyMutex_s*)lock;

  {
    DBG_PROG4_S("%s %d thread[%d] ref:%d", marker, line, oyGetThreadID( oyThreadSelf()), ms->ref );
    /* Lessen the reference counter one level. */
    ms->ref--;
    oyMutexUnLock_m( &ms->m );
  }
  /* Discussion:
   * Several conditions are thinkable and would indicate errors in the system.
   * 1. a different thread unlocks this resource - strikes mutex definition.
   * 2. ms->ref < 0
   */
}


/* forward declaration from src/API_generated/oyStruct_s.c */

//oyStructList_s * oy_thread_cache_ = NULL;
oyStructList_s * oy_job_list_ = NULL;
oyStructList_s * oy_job_message_list_ = NULL;
void oyThreadsInit_(void)
{
  static int * thread_ids;
  int i;

  /* initialise threadsafe job and message queues */
  if(!oy_job_list_)
  {
    /* check threading */
    if(!oyThreadLockingReady())
      /* initialise our locking */
      oyThreadLockingSet( oyStruct_LockCreate_, oyLockRelease_,
                          oyLock_, oyUnLock_ );

    oy_job_list_ = oyStructList_Create( oyOBJECT_NONE, "oy_job_list_", NULL );
    oy_job_message_list_ = oyStructList_Create( oyOBJECT_NONE,
                                                "oy_job_message_list_", NULL );

    /* setup mutexes */
    oyObject_Lock( oy_job_list_->oy_, __func__, __LINE__ );
    oyObject_UnLock( oy_job_list_->oy_, __func__, __LINE__ );
    oyObject_Lock( oy_job_message_list_->oy_, __func__, __LINE__ );
    oyObject_UnLock( oy_job_message_list_->oy_, __func__, __LINE__ );

    if((omp_get_num_procs() - 1) >= 1)
      oy_thread_count_ = omp_get_num_procs() - 1;
    else
      oy_thread_count_ = 1;

    oy_threads_ = (oyThread_t*)calloc(sizeof(oyThread_t),oy_thread_count_+1);
    thread_ids = (int*)calloc(sizeof(int),oy_thread_count_+1);

    oy_threads_[0] = oyThreadSelf();

    for(i = 0; i < oy_thread_count_; ++i)
    {
      oyThread_t background_thread;

      thread_ids[i+1] = i+1;

      oyThreadCreate( oyJobWorker, &thread_ids[i+1], &background_thread );
      printf("thread created [%ld]\n", background_thread);

      oy_threads_[i+1] = background_thread;
    }
  }
}

/**
 *  @brief   Add and run a job
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/01/27
 *  @since   2014/01/27 (Oyranos: 0.9.5)
 */
int                oyJob_Add         ( oyJob_s           * job,
                                       int                 finished )
{
  oyBlob_s * blob;
  static int job_count = 0;
  int job_id = 0;
  int error = 0;

  oyThreadsInit_();

  /* set status */
  if(finished)
    job->status_done_ = 1;
  else
  {
    job->id_ = ++job_count;
    job->status_done_ = 0;
  }

  job_id = job->id_;

  /* setup container */
  blob = oyBlob_New(NULL);
  oyBlob_SetFromStatic( blob, job, 0, "oyJob_s" );

  /* add container to queue threadsafe */
  error = oyStructList_MoveIn( oy_job_list_, (oyStruct_s**) &blob, -1, 0 );
  if(error)
    WARNc2_S("error=%d %d", error, finished);

#if defined(WIN32) && !defined(__GNU__)
#else
  if(finished == 0)
  {
    oyMutex_t * m = (oyMutex_t*) oy_job_list_->oy_->lock_;
    oyObject_Lock( oy_job_list_->oy_, __func__, __LINE__ );
    pthread_cond_signal( &m->cond );
    oyObject_UnLock( oy_job_list_->oy_, __func__, __LINE__ );
  }
#endif

  return job_id;
}
int                oyJob_Get         ( oyJob_s          ** job,
                                       int                 finished )
{
  /* FIFO */
  *job = 0;
  if(!oy_job_list_) return -1;

  /* queue manipulation needs mutex */
  if(oy_debug >= 2)
  {
    char * t = NULL;
    oyStringAddPrintf_(&t, oyAllocateFunc_, oyDeAllocateFunc_, "%s() finished:%d", __func__, finished);
    oyObject_Lock( oy_job_list_->oy_, t, __LINE__ );
    oyFree_m_(t);
  } else
    oyObject_Lock( oy_job_list_->oy_, __func__, __LINE__ );

  int n = oyStructList_Count( oy_job_list_ );
  if(n)
  {
    oyBlob_s * blob = (oyBlob_s*) oyStructList_GetRefType( oy_job_list_, n-1, oyOBJECT_BLOB_S );
    oyJob_s * j = (oyJob_s*) oyBlob_GetPointer( blob );
    if((j->status_done_ > 0 && finished > 0) ||
       (j->status_done_ == 0 && finished == 0))
    {
      oyStructList_ReleaseAt( oy_job_list_, n-1 );
      *job = j;
    }
    oyBlob_Release( &blob );
  }
#if defined(WIN32) && !defined(__GNU__)
#else
  else if(finished == 0)
  {
    oyMutex_t * m = (oyMutex_t*) oy_job_list_->oy_->lock_;
    pthread_cond_wait( &m->cond, &m->mutex );
  }
#endif

  if(oy_debug >= 2)
  {
    char * t = NULL;
    oyStringAddPrintf_(&t, oyAllocateFunc_, oyDeAllocateFunc_, "%s() finished:%d", __func__, finished);
    oyObject_UnLock( oy_job_list_->oy_, t, __LINE__ );
    oyFree_m_(t);
  } else
    oyObject_UnLock( oy_job_list_->oy_, __func__, __LINE__ );

  return 0;
}

typedef struct {
  oyJobCallback_f     cb_progress;
  oyStruct_s        * cb_progress_context;
  double              progress_zero_till_one;
  char              * status_text;
  int                 job_id;
  int                 thread_id_;
} oyMsg_s;
int                oyMsg_Add         ( oyJob_s           * job,
                                       double              progress_zero_till_one,
                                       char              * status_text )
{
  oyMsg_s * m = (oyMsg_s*) calloc(sizeof(oyMsg_s),1);
  oyBlob_s * blob;
  int error;

  m->cb_progress = job->cb_progress;
  m->cb_progress_context = job->cb_progress_context;
  m->progress_zero_till_one = progress_zero_till_one;
  m->status_text = status_text;
  m->thread_id_ = job->thread_id_;
  m->job_id = job->id_;
  blob = oyBlob_New(NULL);
  oyBlob_SetFromStatic( blob, m, 0, "oyJob_s" );
  error = oyStructList_MoveIn( oy_job_message_list_, (oyStruct_s**) &blob, -1, 0 );
  if(error)
    WARNc2_S("error=%d %g", error, progress_zero_till_one);

  return 0;
}
int                oyMsg_Get         ( oyMsg_s          ** msg )
{
  /* FIFO */
  *msg = 0;
  if(!oy_job_message_list_) return -1;
  oyObject_Lock( oy_job_message_list_->oy_, __FILE__, __LINE__ );
  int n = oyStructList_Count( oy_job_message_list_ );
  if(n)
  {
    oyBlob_s * blob = (oyBlob_s*) oyStructList_GetRefType( oy_job_message_list_, 0, oyOBJECT_BLOB_S );
    oyMsg_s * m = (oyMsg_s*) oyBlob_GetPointer( blob );
    oyStructList_ReleaseAt( oy_job_message_list_, 0 );
    *msg = m;
    oyBlob_Release( &blob );
  }
  oyObject_UnLock( oy_job_message_list_->oy_, __FILE__, __LINE__ );
  return 0;
}

void               oySleep           ( double              seconds )
{
  usleep((useconds_t)(seconds*(double)1000));
}

void *             oyJobWorker       ( void              * data )
{
  int thread_id = *((int*)data);

  while(1)
  {
    oyJob_s * job = NULL;
    oyJob_Get( &job, 0 );
    if(job)
    {
      int finished = 1;
      job->thread_id_ = thread_id;
      if(job->cb_progress)
        oyMsg_Add(job, 0.0, strdup("start"));
      job->status_work_return = job->work(job);
      if(job->cb_progress)
        oyMsg_Add(job, 1.0, strdup("done"));
      oyJob_Add( job, finished );
    }
    oySleep(0.02);
  }
  return NULL;
}
void               oyJobResult     ( void )
{ 
  oyMsg_s * msg = NULL;
  oyJob_s * job = NULL;
  while(!oyMsg_Get( &msg ) && msg != NULL)
  {
    if(msg->cb_progress)
      msg->cb_progress( msg->progress_zero_till_one, msg->status_text, msg->thread_id_, msg->job_id );

    if(msg->status_text)
      free(msg->status_text); msg->status_text = NULL;
    free(msg); msg = NULL;
  }

  oyJob_Get( &job, 1 );
  if(job)
    if(job->finish)
      job->finish(job);
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

