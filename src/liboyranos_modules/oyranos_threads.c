/** @file oyranos_threads.c
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

#include "oyranos_threads.h"

#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"
#include "oyranos_string.h"

/** @addtogroup misc
 *
 *  @{ *//* misc */

/** @addtogroup threads Threading
 *  @brief      Asynchron processing support
 *
 *  @section basics Concept
 *  Threading support in Oyranos is kept intentionally simple. Object level locking is available.
 *  Eigther avoid Oyranos locking by encapsulating it into a own locking model.
 *  Or provide locking functions in oyThreadLockingSet(), which can 
 *  detect recursive lock conditions. Recursive locks might not be a problem
 *  with POSIX systems and pthreads PTHREAD_MUTEX_RECURSIVE.
 *
 *  Job handling is more complex. A version is provided in the "trds" module
 *  and will be initialised by default during first use of the APIs. This 
 *  version used a threading model as is available during compilation. 
 *  In case you want a own threading model you can provide it through 
 *  oyJobHandlingSet().
 *
 *  @subsection why_threads Why asynchronous processing?
 *    Some expensive workload is good to load off to a background job
 *    and continue in the foreground for non interupted user interaction.
 *    E.g. the user should be able to continue interacting with the 
 *    image/movie, even while changed options need computing of the current
 *    DAG contexts.
 *
 *  @subsection why_threads_inside Why threading inside Oyranos?
 *    The background jobs tend to be related to tasks inside the Oyranos
 *    DAG and can not easily be handled outside the DAG. For instance it is 
 *    not easy to replace a expensive DAG while performing a expensive 
 *    option change - big image or movie + switching on/off proofing/effects.
 *
 *  @subsection why_threads_modules Why a modular approach?
 *    Threading models can very easily conflict and linking
 *    can become a night mare. Thus threading from "trds" plug-in must be 
 *    replaceable on the descretion of users.
 *
 *  @section init Initialisation
 *  Call oyThreadLockingSet() in order to use own thread locking functions.
 *  Call oyJobHandlingSet() to replace by own Job handling functions.
 *  The functions must be used before any call to Oyranos.
 *
 *  @section api_usage API usage
 *  @subsection adding_jobs Adding Jobs
 *  A oyJob_New() creates a new oyJob_s object which can be added to the job
 *  qeue by oyJob_Add(). Job creation and Job adding belongs to the managing
 *  thread. Calling oyJob_Add() for the first time in the process
 *  initialises the API's and starts the worker threads.
 *  oyJob_s::work() is called from inside the worker thread.
 *
 *  @subsection working Processing Jobs
 *  oyJob_s::work() does the asynchron oyJob_s::context processing. It can
 *  add messages with oyMsg_Add() to the message qeue.
 *  After returning from oyJob_s::work() the job object is placed back into
 *  the job qeue and marked as done (finished == 1) by the thread handling
 *  plug-in.
 *
 *  @subsection getting_results Getting Results
 *  Calling oyJobResult() in managing thread, usually the main GUI thread,
 *  allows to obtain progress status from the Oyranos worker threads.
 *  jobPrintfCallback() is the default message callback and just does printing
 *  a status information, which were previously add inside the oyJob_s::work()
 *  inside the worker thread with oyMsg_Add().
 *  The oyJob_s object, including the processed data, is placed into the 
 *  job qeue after oyJob_s::work() returned. To access the oyJob_s use the
 *  oyJob_s::finish() callback. oyJobResult() is as well responsible to call 
 *  oyJob_s::finish() from the managing/UI thread for that.
 *
 *  @subsection threads_debugging Debugging
 *  See @ref environment .
 *
 *  @{ *//* threads */

static const char * oy_thread_api_nick = "dumy";

/**
 *  @brief   Initialise the oyJob_s APIs
 *  @extends oyStruct_s
 *  @memberof oyJob_s
 *
 *  Be careful to set the APIs only before any call to the oyJob_s functions.
 *  Otherwise the behaviour is undefined.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
void               oyJobHandlingSet  ( oyJob_Add_f         jadd,
                                       oyJob_Get_f         jget,
                                       oyMsg_Add_f         madd,
                                       oyJobResult_f       result,
                                       const char        * nick )
{
  oyJob_Add = jadd;
  oyJob_Get = jget;
  oyMsg_Add = madd;
  oyJobResult = result;
  oy_thread_api_nick = nick;
}

int                oyJob_AddInit     ( oyJob_s          ** job,
                                       int                 finished,
                                       int                 flags );
extern oyThreadId_f oyObjectGetThreadId;
int                oyJobInitialise_  ( void )
{
  oyOptions_s * opts = 0,
              * result_opts = 0;

  opts = oyOptions_New(0);
  int error = oyOptions_Handle( "//"OY_TYPE_STD"/threads_handler",
                                opts,"threads_handler",
                                &result_opts );
  if(error || oyJob_Add == oyJob_AddInit)
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_
                     " can't properly call \"threads_handler\"",OY_DBG_ARGS_);
  oyOptions_Release( &opts );

  /* initialise for lower level libOyranosObject as well */
  oyObjectGetThreadId = oyThreadId;

  return error;
}

int                oyJob_AddInit     ( oyJob_s          ** job,
                                       int                 finished,
                                       int                 flags )
{ if(oyJobInitialise_() == 0)
    return oyJob_Add( job, finished, flags );
  else
    return 1;
}
int                oyJob_GetInit     ( oyJob_s          ** job,
                                       int                 finished )
{ if(oyJobInitialise_() == 0)
    return oyJob_Get( job, finished );
  else
    return 1;
}
int                oyMsg_AddInit     ( oyJob_s           * job,
                                       double              progress_zero_till_one,
                                       char             ** status_text )
{ if(oyJobInitialise_() == 0)
    return oyMsg_Add( job, progress_zero_till_one, status_text );
  else
    return 1;
}
void               oyJobResultInit   ( void )
{ if(oyJobInitialise_() == 0)
    oyJobResult();
}
int                oyThreadIdInit    ( void )
{ if(oyJobInitialise_() == 0)
    return oyThreadId();
  else
    return 1;
}


/** @typedef oyJob_Add_f
 *  @brief   Add one unique oyJob_s to the job qeue
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/09/22
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
/**
 *  @brief   Add one unique oyJob_s to the job qeue
 *  @memberof oyJob_s
 *  @see     oyJob_Add_f
 *
 *  @param   job                       the job object
 *  @param   finished                  mark the job in the queue
 *                                     - 0 - to be processed;
 *                                           work funtion was not yet called
 *                                     - 1 - finished processing;
 *                                           work function was called inside
 *                                           the worker thread;
 *                                           ready for getting with oyJob_Get()
 *  @param   flags                     hint on how to process the job
 *                                     - oyJOB_ADD_PERSISTENT_JOB - suggest to 
 *                                     add a new thread as the job may run 
 *                                     over the whole process live time;
 *                                     typical for a asynchron observer
 *  @return                            the job ID
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/09/22
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
oyJob_Add_f oyJob_Add = oyJob_AddInit;
/** @typedef oyJob_Get_f
 *  @brief   Get one unique oyJob_s from the job qeue
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/01
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
/**
 *  @brief   Get one unique oyJob_s from the job qeue
 *  @memberof oyJob_s
 *  @see     oyJob_Get_f
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyJob_Get_f oyJob_Get = oyJob_GetInit;
/** @typedef oyMsg_Add_f
 *  @brief   Add one unique message from the message qeue
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/01
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
/**
 *  @brief   Add one unique message from the message qeue
 *  @memberof oyJob_s
 *  @see     oyMsg_Add_f
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyMsg_Add_f oyMsg_Add = oyMsg_AddInit;
/** @typedef oyJobResult_f
 *  @brief   Poll for new Jobs
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/01
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
/**
 *  @brief   Poll for messages and finished jobs 
 *  @memberof oyJob_s
 *  @see     oyJobResult_f
 *
 *  The function does two things. It calls oyJob_s::cb_progress() from a 
 *  filled message qeue.
 *  And it takes jobs out of the job qeue, which are marked as finalised.
 *  oyJob_s::finish() is optionally called. After the oyJob_s::finish()
 *  callback the job is released.
 *
 *  The function is usually called from the managing-and main UI thread.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyJobResult_f oyJobResult = oyJobResultInit;
/** @typedef oyThreadId_f
 *  @brief   Get one unique thread ID
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/22
 *  @since   2019/09/22 (Oyranos: 0.9.7)
 */
/**
 *  @brief   Get one unique thread Id
 *  @see     oyThreadId_f
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/22
 *  @since   2019/09/22 (Oyranos: 0.9.7)
 */
oyThreadId_f oyThreadId = oyThreadIdInit;
void               oyThreadIdSet     ( oyThreadId_f        tid,
                                       const char        * nick )
{
  if(!oy_thread_api_nick || (oy_thread_api_nick && nick && strcmp(oy_thread_api_nick,nick) != 0))
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_
                     " previous and new thread API nick differ %s - %s.",OY_DBG_ARGS_,
                     oyNoEmptyString_m_(oy_thread_api_nick), oyNoEmptyString_m_(nick));
  oyThreadId = tid;
}


/** @typedef oyJobCallback_f
 *  @brief   Progress callback for parallel job processing
 *
 *  @param   progress_zero_till_one    will be called by the work thread with 0.0 before start of oyJob_s::work() and after with 1.0
 *                                     between oyMsg_Add() can be used inside oyJob_s::work() to add more fine grained status info
 *  @param   status_text               textual description of the work progress
 *  @param   thread_id_                work thread info
 *  @param   job_id                    id of the job
    @param   cb_progress_context       GUI context; e.g. gauge/progress bar ... (unused)
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/02
 *  @since   2014/01/27 (Oyranos: 0.9.5)
 */

static void jobPrintfCallback        ( double              progress_zero_till_one,
                                       char              * status_text,
                                       int                 thread_id_,
                                       int                 job_id,
                                       oyStruct_s        * cb_progress_context OY_UNUSED )
{ printf("%s():%d %02f %s %d/%d\n",__func__,__LINE__,progress_zero_till_one,
         status_text?status_text:"",thread_id_,job_id);
}

/** Function oyJob_New
 *  @brief   Allocate a new oyJob_s object
 *  @extends oyStruct_s
 *  @memberof oyJob_s
 *
 *  A object argument is ignored.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/02
 *  @since   2016/05/02 (Oyranos: 0.9.6)
 */
oyJob_s *         oyJob_New          ( oyObject_s          object OY_UNUSED )
{
  oyJob_s * job = (oyJob_s*) oyAllocateFunc_(sizeof(oyJob_s));

  if(job)
  {
    memset(job, 0, sizeof(oyJob_s));

    job->type_ = oyOBJECT_JOB_S;
    job->cb_progress = jobPrintfCallback;
  }

  return job;
}

/** Function oyJob_Release
 *  @brief   Deallocate a oyJob_s object
 *  @extends oyStruct_s
 *  @memberof oyJob_s
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/02
 *  @since   2016/05/02 (Oyranos: 0.9.6)
 */
void              oyJob_Release      ( oyJob_s          ** job )
{
  if(job && *job)
  {
    oyJob_s * s = *job;
    if(s->type_ != oyOBJECT_JOB_S)
      oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)s, OY_DBG_FORMAT_
                       "unexpected object", OY_DBG_ARGS_);

    if(s->context && s->context->release)
      s->context->release( &s->context );
    s->context = NULL;

    if(s->cb_progress_context && s->cb_progress_context->release)
      s->cb_progress_context->release( &s->cb_progress_context );
    s->cb_progress_context = NULL;

    s->type_ = oyOBJECT_NONE;
    oyDeAllocateFunc_( s );
    *job = NULL;
  }
}

/** @} *//* threads */
/** @} *//* misc */
