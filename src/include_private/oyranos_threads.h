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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(WIN32) && !defined(__GNU__)
# include <process.h>
typedef CRITICAL_SECTION oyMutex_t;
# define oyMutexInit_m(m,a) InitializeCriticalSection(m)
# define oyMutexLock_m(m) EnterCriticalSection(m)
# define oyMutexUnLock_m(m) LeaveCriticalSection(m)
# define oyMutexDestroy_m(m) DeleteCriticalSection(m)
typedef unsigned long oyThread_t;
#else
# include <pthread.h>
typedef pthread_mutex_t oyMutex_t;
# define oyMutexInit_m(m,a) pthread_mutex_init(m,a)
# define oyMutexLock_m(m) pthread_mutex_lock(m)
# define oyMutexUnLock_m(m) pthread_mutex_unlock(m)
# define oyMutexDestroy_m(m) pthread_mutex_destroy(m)
typedef pthread_t oyThread_t;
int oyThreadCreate                   ( void             *(*func) (void * data),
                                       void              * data,
                                       oyThread_t        * thread );
#endif 


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_THREADS_H */
