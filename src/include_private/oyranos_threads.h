/** @file oyranos_threads.h
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

#ifndef OYRANOS_THREADS_H
#define OYRANOS_THREADS_H

#include <oyStruct_s.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(WIN32) && !defined(__GNU__)
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

typedef void (*oyJobCallback_f)      ( double              progress_zero_till_one,
                                       char              * status_text,
                                       int                 thread_id_,
                                       int                 job_id );
/*  A job is user allocated and added to the single job queue with oyJob_Add().
 *  The job queue is worked on by a internal maintained thead.
 */
typedef struct oyJob_s oyJob_s;
struct oyJob_s {
  oyStruct_s * context;                /**< the workload */
  int (*work) (oyJob_s * job);         /**< the working thread callback */
  int status_work_return;              /**< return value of the work() function */
  int (*finish) (oyJob_s * job);       /**< the observating thread callback */
  oyJobCallback_f cb_progress;         /**< the progress callback, called by observating thread */
  oyStruct_s * cb_progress_context;    /**< the progress callback context */
  /* Oyranos maintained fields */
  int id_;                             /**< Oyranos provided work ID, keep read only */
  int thread_id_;                      /**< Oyranos provided ID; keep read only */
  int status_done_;                    /**< Oyranos internal variable */
};
int                oyJob_Add         ( oyJob_s           * job,
                                       int                 finished );
int                oyJob_Get         ( oyJob_s          ** job,
                                       int                 finished );
int                oyMsg_Add         ( oyJob_s           * job,
                                       double              progress_zero_till_one,
                                       char              * status_text );
void *             oyJobWorker       ( void              * data );
void               oyJobResult       ( void );
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_THREADS_H */
