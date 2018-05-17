/** @file oyranos_cmm_trds.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2014-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    threads module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2014/01/24
 */

/**
 *
 *  The main goal of this module is to provide a means to replace 
 *  threading / job functionality and switch to pthreads/windows/whatever
 *  model as the main application needs it. The implementation provides a
 *  reasonable set of functions for objects of type oyJob_s.
 */

//      * move some of the Why questions above to the doxygen docu.

#include "oyCMM_s.h"
#include "oyCMMapi10_s_.h"

#include "oyranos_cmm.h"         /* the API's this CMM implements */
#include "oyranos_i18n.h"
#include "oyranos_string.h"
#include "oyranos_threads.h"

/*
oyCMM_s         trds_cmm_module;
oyCMMapi10_s    trds_api10_cmm;
*/


/* --- internal definitions --- */

#define CMM_NICK "trds"

#define CMM_VERSION {0,1,0}

#if defined(_WIN32) && !defined(__GNU__)
# include <process.h>
typedef unsigned long oyThread_t;
# define oyThreadSelf  GetCurrentThreadId
# define oyThreadEqual(a,b) ((a) == (b))
typedef CRITICAL_SECTION oyMutex_t;
# define oyMutexInit_m(m,a) InitializeCriticalSection(m)
# define oyMutexLock_m(m) EnterCriticalSection(m)
# define oyMutexUnLock_m(m) LeaveCriticalSection(m)
# define oyMutexDestroy_m(m) DeleteCriticalSection(m)
#else
# include <pthread.h>
typedef pthread_t oyThread_t;
# define oyThreadSelf  pthread_self
# define oyThreadEqual(a,b) pthread_equal((a),(b))
typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} oyMutex_t;
# define oyMutexInit_m(m,a) { pthread_mutex_init(m.mutex, a); pthread_cond_init(m.cond, NULL); }
# define oyMutexLock_m(m) pthread_mutex_lock(m.mutex)
# define oyMutexUnLock_m(m) pthread_mutex_unlock(m.mutex)
# define oyMutexDestroy_m(m) { pthread_mutex_destroy(m.mutex); pthread_cond_destroy(m.cond); }
#endif 

int oyThreadCreate                   ( void             *(*func) (void * data),
                                       void              * data,
                                       oyThread_t        * thread );

void *             oyJobWorker       ( void              * data );

int                oyJob_Add_        ( oyJob_s          ** job,
                                       int                 finished,
                                       int                 flags );
int                oyJob_Get_        ( oyJob_s          ** job,
                                       int                 finished );
int                oyMsg_Add_        ( oyJob_s           * job,
                                       double              progress_zero_till_one,
                                       char             ** status_text );
void               oyJobResult_      ( void );

oyMessage_f trds_msg = oyMessageFunc;

int            trdsCMMMessageFuncSet ( oyMessage_f         trds_msg_func );
int                trdsCMMInit       ( );


#include <unistd.h> /* usleep() */
#ifdef _OPENMP
#define USE_OPENMP 1
#include <omp.h> /* omp_get_num_procs() */
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


oyStructList_s * oy_threads_ = NULL;

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
#if defined(_WIN32) && !defined(__GNU__)
    *thread = (oyThread_t) _beginthread( (void(*)(void*)) func, 0, data );
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
oyPointer  oyStruct_LockCreate_        ( oyStruct_s      * obj  OY_UNUSED)
{
  oyMutex_s * ms = (oyMutex_s*) calloc(sizeof(oyMutex_s),1);

#if defined(_WIN32) && !defined(__GNU__)
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
  int i, count = oyStructList_Count(oy_threads_);
  for(i = 0; i < count; ++i)
  {
    oyBlob_s * blob = (oyBlob_s*) oyStructList_GetRefType( oy_threads_, i, oyOBJECT_BLOB_S );
    oyThread_t bt = (oyThread_t) oyBlob_GetPointer( blob );
    oyBlob_Release( &blob );
    if      (oyThreadEqual( bt, t ))
      break;
  }
  return i;
}

void       oyLock_                     ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )
{ 
  oyMutex_s * ms = (oyMutex_s*)lock;
  oyThread_t t = oyThreadSelf();

  {
    if(oy_debug > 5)
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
    if(oy_debug > 5)
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
void oyThreadAdd()
{
  oyThread_t background_thread;
  int error;
  oyOption_s * o = oyOption_FromRegistration( "///id", NULL );
  int count = oyStructList_Count( oy_threads_ );
  oyBlob_s * blob;

  oyOption_SetFromInt( o, count, 0, 0 );

  oyThreadCreate( oyJobWorker, (oyPointer)o, &background_thread );
  if(oy_debug)
    trds_msg( oyMSG_DBG, 0, "thread created [%ld]\n", background_thread);

  blob = oyBlob_New(0);
  oyBlob_SetFromStatic( blob, (oyPointer)background_thread, 0, "oyThread_t" );
  oyObject_Lock( oy_threads_->oy_, __FILE__, __LINE__ );
  error = oyStructList_MoveIn( oy_threads_, (oyStruct_s**) &blob, -1, 0 );
  oyObject_UnLock( oy_threads_->oy_, __FILE__, __LINE__ );
  if(error)
    WARNc1_S("error=%d", error);
}

//oyStructList_s * oy_thread_cache_ = NULL;
oyStructList_s * oy_job_list_ = NULL;
oyStructList_s * oy_job_message_list_ = NULL;
void oyThreadsInit_( int flags )
{
  int i, count, error;

  /* initialise threadsafe job and message queues */
  if(!oy_job_list_)
  {
    oyBlob_s * blob;
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

#if defined(_OPENMP) && defined(USE_OPENMP)
    if((omp_get_num_procs() - 1) >= 1)
      count = omp_get_num_procs() - 1;
    else
#endif
      count = 2;

    oy_threads_ = oyStructList_Create( oyOBJECT_NONE, "oy_threads_", NULL );

    blob = oyBlob_New(0);
    oyBlob_SetFromStatic( blob, (oyPointer)oyThreadSelf(), 0, "oyThread_t" );
    oyObject_Lock( oy_threads_->oy_, __FILE__, __LINE__ );
    error = oyStructList_MoveIn( oy_threads_, (oyStruct_s**) &blob, -1, 0 );
    oyObject_UnLock( oy_threads_->oy_, __FILE__, __LINE__ );
    if(error)
       WARNc1_S("error=%d", error);

    for(i = 0; i < count; ++i)
    {
      oyThread_t background_thread;
      int error;
      oyOption_s * o = oyOption_FromRegistration( "///id", NULL );
      oyOption_SetFromInt( o, i+1, 0, 0 );

      oyThreadCreate( oyJobWorker, (oyPointer)o, &background_thread );
      if(oy_debug)
        trds_msg( oyMSG_DBG, 0, "thread created [%ld]\n", background_thread);

      blob = oyBlob_New(0);
      oyBlob_SetFromStatic( blob, (oyPointer)background_thread, 0, "oyThread_t" );
      oyObject_Lock( oy_threads_->oy_, __FILE__, __LINE__ );
      error = oyStructList_MoveIn( oy_threads_, (oyStruct_s**) &blob, -1, 0 );
      oyObject_UnLock( oy_threads_->oy_, __FILE__, __LINE__ );
      if(error)
         WARNc1_S("error=%d", error);
    }
  }

  if(flags & oyJOB_ADD_PERSISTENT_JOB)
  {
    oyThreadAdd();
  }
}

/**
 *  @brief   Add and run a job
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/09/22
 *  @since   2014/01/27 (Oyranos: 0.9.5)
 */
int                oyJob_Add_        ( oyJob_s          ** job_,
                                       int                 finished,
                                       int                 flags )
{
  oyBlob_s * blob;
  static int job_count = 0;
  int job_id = 0;
  int error = 0;
  oyJob_s * job = *job_;

  *job_ = NULL;

  oyThreadsInit_( flags );

  /* set status */
  if(finished)
    job->status_done_ = 1;
  else
  {
    job->id_ = ++job_count;
    job->status_done_ = 0;
    job->flags_ = flags;
  }

  job_id = job->id_;

  /* setup container */
  blob = oyBlob_New(NULL);
  oyBlob_SetFromStatic( blob, job, 0, "oyJob_s" );

  /* add container to queue threadsafe */
  error = oyStructList_MoveIn( oy_job_list_, (oyStruct_s**) &blob, -1, 0 );
  if(error)
    WARNc2_S("error=%d %d", error, finished);

#if defined(_WIN32) && !defined(__GNU__)
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
int                oyJob_Get_        ( oyJob_s          ** job,
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
#if defined(_WIN32) && !defined(__GNU__)
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
int                oyMsg_Add_        ( oyJob_s           * job,
                                       double              progress_zero_till_one,
                                       char             ** status_text )
{
  oyMsg_s * m = (oyMsg_s*) calloc(sizeof(oyMsg_s),1);
  oyBlob_s * blob;
  int error;

  m->cb_progress = job->cb_progress;
  if(job->cb_progress_context && job->cb_progress_context->copy)
    m->cb_progress_context = job->cb_progress_context->copy(job->cb_progress_context, 0);
  else
    m->cb_progress_context = job->cb_progress_context;
  m->progress_zero_till_one = progress_zero_till_one;
  m->status_text = *status_text;
  *status_text = NULL;
  m->thread_id_ = job->thread_id_;
  m->job_id = job->id_;
  blob = oyBlob_New(NULL);
  oyBlob_SetFromStatic( blob, m, 0, "oyMsg_s" );
  oyObject_Lock( oy_job_message_list_->oy_, __FILE__, __LINE__ );
  error = oyStructList_MoveIn( oy_job_message_list_, (oyStruct_s**) &blob, -1, 0 );
  oyObject_UnLock( oy_job_message_list_->oy_, __FILE__, __LINE__ );
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
  usleep((useconds_t)(seconds*(double)1000000));
}

void *             oyJobWorker       ( void              * data )
{
  oyOption_s * o = (oyOption_s*)data;
  int thread_id = oyOption_GetValueInt(o,0);
  oyOption_Release( &o );

  while(1)
  {
    int flags = 0;
    oyJob_s * job = NULL;
    oyJob_Get_( &job, 0 );
    if(job)
    {
      int finished = 1;
      char * t;
      job->thread_id_ = thread_id;
      if(job->cb_progress)
      {
        t = strdup("start");
        oyMsg_Add_(job, 0.0, &t);
      }
      job->status_work_return = job->work(job);
      if(job->cb_progress)
      {
        t = strdup("done");
        oyMsg_Add_(job, 1.0, &t);
      }
      flags = job->flags_;
      oyJob_Add_( &job, finished, 0 );
    }
    oySleep(0.02);

    if(flags & oyJOB_ADD_PERSISTENT_JOB)
      break;
  }
  return NULL;
}
void               oyJobResult_      ( void )
{ 
  oyMsg_s * msg = NULL;
  oyJob_s * job = NULL;
  while(!oyMsg_Get( &msg ) && msg != NULL)
  {
    if(msg->cb_progress)
    {
      if(msg->cb_progress_context && msg->cb_progress_context->copy)
        msg->cb_progress_context = msg->cb_progress_context->copy(msg->cb_progress_context, 0);
      msg->cb_progress( msg->progress_zero_till_one, msg->status_text, msg->thread_id_, msg->job_id, msg->cb_progress_context );
    }

    if(msg->status_text)
    { free(msg->status_text); msg->status_text = NULL; }
    free(msg); msg = NULL;
  }

  oyJob_Get_( &job, 1 );
  if(job)
  {
    if(job->finish)
      job->finish(job);
    oyJob_Release( &job );
  }
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


/** Function trdsCMMInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/29
 *  @since   2016/04/29 (Oyranos: 0.9.6)
 */
int                trdsCMMInit       ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}



/** Function trdsCMMMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
int            trdsCMMMessageFuncSet ( oyMessage_f         message_func )
{
  trds_msg = message_func;
  return 0;
}


/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
int          trdsMOptions_Handle     ( oyOptions_s       * options OY_UNUSED,
                                       const char        * command,
                                       oyOptions_s      ** result OY_UNUSED )
{
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    trds_msg( oyMSG_DBG, 0, "called %s()::can_handle", __func__ );
    return error;
  }
  else if(oyFilterRegistrationMatch(command,"threads_handler", 0))
  {
    oyJobHandlingSet( oyJob_Add_,
                      oyJob_Get_,
                      oyMsg_Add_,
                      oyJobResult_,
                      CMM_NICK );
    trds_msg( oyMSG_DBG, 0, "called %s()::threads_handler", __func__ );
  }

  return 0;
}

const char *trds_texts_profile_create[4] = {"can_handle","threads_handler","help",0};

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
const char * trdsInfoGetTextThreadsC ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "threads_handler")==0)
  {
         if(type == oyNAME_NICK)
      return "create_profile";
    else if(type == oyNAME_NAME)
      return _("Initialise the oyJob_s APIs.");
    else
      return _("The Oyranos \"threads_handler\" command will initialise the oyJob_s APIs with a available threading model.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Initialise the oyJob_s APIs.");
    else
      return _("The Oyranos \"threads_handler\" command will initialise the oyJob_s APIs with a available threading model.");
  }
  return 0;
}

/** @brief    trdsead oyCMMapi10_s implementation
 *
 *  handlers for threading
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyCMMapi10_s_    trds_api10_cmm = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) NULL,

  trdsCMMInit,
  trdsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "threads_handler._" CMM_NICK,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  trdsInfoGetTextThreadsC,             /**< getText */
  (char**)trds_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  trdsMOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};




/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
const char * trdsInfoGetText         ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Oyranos Threads");
    else
      return _("Oyranos Threads default Handling");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "BSD-3-Clause";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2014-2016 Kai-Uwe Behrmann; new BSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter provides a default oyJob_s handling mechanism for asynchronous processing.");
    else
      return _("The filter provides a default oyJob_s handling mechanism for asynchronous processing." );
  }
  return 0;
}
const char *trds_texts[5] = {"name","copyright","manufacturer","help",0};
oyIcon_s trds_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @brief    trds module infos
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyCMM_s trds_cmm_module = {

  oyOBJECT_CMM_INFO_S,                 /**< type, struct type */
  0,0,0,                               /**< ,dynamic object functions */
  CMM_NICK,                            /**< cmm, ICC signature */
  "0.6",                               /**< backend_version */
  trdsInfoGetText,                     /**< getText */
  (char**)trds_texts,                  /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,                     /**< oy_compatibility */

  (oyCMMapi_s*) & trds_api10_cmm,       /**< api */

  &trds_icon, /**< icon */
  trdsCMMInit                          /**< oyCMMinfoInit_f */
};

