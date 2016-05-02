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

static int oy_job_initialised = 0;
/**
 *  @brief   Initialise the oyJob_s APIs
 *  @extends oyStruct_s
 *  @memberof oyJob_s
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
void               oyJobHandlingSet  ( oyJob_Add_f         jadd,
                                       oyJob_Get_f         jget,
                                       oyMsg_Add_f         madd,
                                       oyJobResult_f       result )
{
  oyJob_Add = jadd;
  oyJob_Get = jget;
  oyMsg_Add = madd;
  oyJobResult = result;
}

int                oyJob_AddInit     ( oyJob_s           * job,
                                       int                 finished );
int                oyJobInitialise_  ( void )
{
  oyOptions_s * opts = 0,
              * result_opts = 0;

  oy_job_initialised = 1;
  opts = oyOptions_New(0);
  int error = oyOptions_Handle( "//"OY_TYPE_STD"/threads_handler",
                                opts,"threads_handler",
                                &result_opts );
  if(error || oyJob_Add == oyJob_AddInit)
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_
                     " can't properly call \"threads_handler\"",OY_DBG_ARGS_);
  oyOptions_Release( &opts );
  return error;
}

int                oyJob_AddInit     ( oyJob_s           * job,
                                       int                 finished )
{ if(oyJobInitialise_() == 0)
    return oyJob_Add( job, finished );
  else
    return 1;
}
int                oyJob_GetInit     ( oyJob_s          ** job,
                                       int                 finished )
{ if(oyJobInitialise_() == 0)
    return oyJob_GetInit( job, finished );
  else
    return 1;
}
int                oyMsg_AddInit     ( oyJob_s           * job,
                                       double              progress_zero_till_one,
                                       char             ** status_text )
{ if(oyJobInitialise_() == 0)
    return oyMsg_AddInit( job, progress_zero_till_one, status_text );
  else
    return 1;
}
void               oyJobResultInit   ( void )
{ if(oyJobInitialise_() == 0)
    oyJobResultInit();
}


/**
 *  @brief   Add one unique oyJob_s to the job qeue
 *  @extends oyStruct_s
 *  @memberof oyJob_s
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyJob_Add_f oyJob_Add = oyJob_AddInit;
/**
 *  @brief   Get one unique oyJob_s from the job qeue
 *  @extends oyStruct_s
 *  @memberof oyJob_s
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyJob_Get_f oyJob_Get = oyJob_GetInit;
/**
 *  @brief   Add one unique message from the message qeue
 *  @memberof oyJob_s
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyMsg_Add_f oyMsg_Add = oyMsg_AddInit;
/**
 *  @brief   Poll for new Jobs
 *  @memberof oyJob_s
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
oyJobResult_f oyJobResult = oyJobResultInit;


static void jobPrintfCallback        ( double              progress_zero_till_one,
                                       char              * status_text,
                                       int                 thread_id_,
                                       int                 job_id,
                                       oyStruct_s        * cb_progress_context )
{ printf("%s():%d %02f %s %d/%d\n",__func__,__LINE__,progress_zero_till_one,
         status_text?status_text:"",thread_id_,job_id);
}

/** @fn      oyJob_New
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
oyJob_s *         oyJob_New          ( oyObject_s          object )
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

/** @fn      oyJob_Release
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
