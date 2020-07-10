/** @file oyranos_threads.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2014-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    thread methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
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

typedef void (*oyJobCallback_f)      ( double              progress_zero_till_one,
                                       char              * status_text,
                                       int                 thread_id_,
                                       int                 job_id,
                                       oyStruct_s        * cb_progress_context );

typedef struct oyJob_s oyJob_s;
/** @struct  oyJob_s
 *  @ingroup threads
 *  @extends oyStruct_s
 *  @brief   Asynchron job ticket
 *
 *  A generic job is added to the single job queue with oyJob_Add().
 *  The job queue is worked on by an internal maintained thead.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/01
 *  @since   2014/01/27 (Oyranos: 0.9.5)
 */
struct oyJob_s {
  oyOBJECT_e type_;                    /**< @brief oyOBJECT_JOB_S */
  oyStruct_Copy_f      copy_not;       /**< @brief keep to zero; as it is dangerous in a threaded context */
  oyStruct_Release_f   release;        /**< @brief Release function; optional */
  oyObject_s           oy_;            /**< @brief Oyranos internal object; not needed, keep to zero */
  
  oyStruct_s * context;                /**< @brief the workload data; optional */
  /** Main job function. Here the job does its processing. Data from 
   *  oyJob_s::context can be used for this. */
  int (*work) (oyJob_s * job);         /**< @brief the working thread callback */
  int status_work_return;              /**< @brief return value of the work() function */
  int (*finish) (oyJob_s * job);       /**< @brief the managing thread callback; optional @see oyJobResult() */
  /**
   *  oyJob_s::cb_progress_context may contain a GUI context, like a widget
   *  object, to be used to display the progress information from within
   *  the main/GUI thread. The callback is optional.
   */
  oyJobCallback_f cb_progress;         /**< @brief the progress callback, called inside the observating thread; optional */
  /** optional data */
  oyStruct_s * cb_progress_context;    /**< @brief the progress callback context */

  /* Oyranos or thread plug-in maintained fields */
  int id_;                             /**< thread plug-in provided work ID, keep read only */
  int thread_id_;                      /**< thread plug-in provided ID; keep read only */
  int status_done_;                    /**< Oyranos internal variable */
  int flags_;                          /**< oyJob_Add() provided flags; keep read only */
};
oyJob_s *         oyJob_New          ( oyObject_s          object );
void              oyJob_Release      ( oyJob_s          ** job );

#define oyJOB_ADD_PERSISTENT_JOB 0x01
typedef int      (*oyJob_Add_f)      ( oyJob_s          ** job,
                                       int                 finished,
                                       int                 flags );
typedef int      (*oyJob_Get_f)      ( oyJob_s          ** job,
                                       int                 finished );
typedef int      (*oyMsg_Add_f)      ( oyJob_s           * job,
                                       double              progress_zero_till_one,
                                       char             ** status_text );
typedef void     (*oyJobResult_f)    ( void );

extern oyJob_Add_f oyJob_Add;
extern oyJob_Get_f oyJob_Get;
extern oyMsg_Add_f oyMsg_Add;
extern oyJobResult_f oyJobResult;

void               oyJobHandlingSet  ( oyJob_Add_f         jadd,
                                       oyJob_Get_f         jget,
                                       oyMsg_Add_f         madd,
                                       oyJobResult_f       result,
                                       const char        * nick );
void               oyJobHandlingReset( );

typedef int      (*oyThreadId_f)     ( void );
extern oyThreadId_f oyThreadId;
void               oyThreadIdSet     ( oyThreadId_f        tid,
                                       const char        * nick );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_THREADS_H */
