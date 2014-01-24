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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

