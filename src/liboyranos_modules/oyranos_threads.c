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
                                       char              * status_text )
{ if(oyJobInitialise_() == 0)
    return oyMsg_AddInit( job, progress_zero_till_one, status_text );
  else
    return 1;
}
void               oyJobResultInit   ( void )
{ if(oyJobInitialise_() == 0)
    oyJobResultInit();
}


oyJob_Add_f oyJob_Add = oyJob_AddInit;
oyJob_Get_f oyJob_Get = oyJob_GetInit;
oyMsg_Add_f oyMsg_Add = oyMsg_AddInit;
oyJobResult_f oyJobResult = oyJobResultInit;
