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

/** @typedef oyJobCallback_f
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @memberof oyJob_s
 *  @brief   Progress callback for parallel job processing
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/02
 *  @since   2014/01/27 (Oyranos: 0.9.5)
 */
typedef void (*oyJobCallback_f)      ( double              progress_zero_till_one,
                                       char              * status_text,
                                       int                 thread_id_,
                                       int                 job_id,
                                       oyStruct_s        * cb_progress_context );

typedef struct oyJob_s oyJob_s;
/** @struct  oyJob_s
 *  @ingroup module_api
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
oyJob_s *         oyJob_New          ( oyObject_s          object );
void              oyJob_Release      ( oyJob_s          ** job );

typedef int      (*oyJob_Add_f)      ( oyJob_s           * job,
                                       int                 finished );
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

void              oyJobHandlingSet   ( oyJob_Add_f         jadd,
                                       oyJob_Get_f         jget,
                                       oyMsg_Add_f         madd,
                                       oyJobResult_f       result );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_THREADS_H */
