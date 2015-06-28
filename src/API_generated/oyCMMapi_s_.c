/** @file oyCMMapi_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2015 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMapi_s.h"
#include "oyCMMapi_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  


/* Include "CMMapi.private_custom_definitions.c" { */
/** Function    oyCMMapi_Release__Members
 *  @memberof   oyCMMapi_s
 *  @brief      Custom CMMapi destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi  the CMMapi object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi_Release__Members( oyCMMapi_s_ * cmmapi )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi->member );
   */

  if(cmmapi->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi->member );
     */
  }
}

/** Function    oyCMMapi_Init__Members
 *  @memberof   oyCMMapi_s
 *  @brief      Custom CMMapi constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi  the CMMapi object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi_Init__Members( oyCMMapi_s_ * cmmapi )
{
  return 0;
}

/** Function    oyCMMapi_Copy__Members
 *  @memberof   oyCMMapi_s
 *  @brief      Custom CMMapi copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi_s_ input object
 *  @param[out]  dst  the output oyCMMapi_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi_Copy__Members( oyCMMapi_s_ * dst, oyCMMapi_s_ * src)
{
#if 0
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

#if 0
  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */

  return 0;
}

/* } Include "CMMapi.private_custom_definitions.c" */



/** @internal
 *  Function oyCMMapi_New_
 *  @memberof oyCMMapi_s_
 *  @brief   allocate a new oyCMMapi_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi_s_ * oyCMMapi_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapi_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapi_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMapi_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapi_Copy;
  s->release = (oyStruct_Release_f) oyCMMapi_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( s );
  /* ---- end of custom CMMapi constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyCMMapi_Copy__
 *  @memberof oyCMMapi_s_
 *  @brief   real copy a CMMapi object
 *
 *  @param[in]     cmmapi                 CMMapi struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi_s_ * oyCMMapi_Copy__ ( oyCMMapi_s_ *cmmapi, oyObject_s object )
{
  oyCMMapi_s_ *s = 0;
  int error = 0;

  if(!cmmapi || !object)
    return s;

  s = (oyCMMapi_s_*) oyCMMapi_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( s, cmmapi );
    /* ---- end of custom CMMapi copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyCMMapi_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi_Copy_
 *  @memberof oyCMMapi_s_
 *  @brief   copy or reference a CMMapi object
 *
 *  @param[in]     cmmapi                 CMMapi struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi_s_ * oyCMMapi_Copy_ ( oyCMMapi_s_ *cmmapi, oyObject_s object )
{
  oyCMMapi_s_ *s = cmmapi;

  if(!cmmapi)
    return 0;

  if(cmmapi && !object)
  {
    s = cmmapi;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapi_Copy__( cmmapi, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi_Release_
 *  @memberof oyCMMapi_s_
 *  @brief   release and possibly deallocate a CMMapi object
 *
 *  @param[in,out] cmmapi                 CMMapi struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapi_Release_( oyCMMapi_s_ **cmmapi )
{
  /* ---- start of common object destructor ----- */
  oyCMMapi_s_ *s = 0;

  if(!cmmapi || !*cmmapi)
    return 0;

  s = *cmmapi;

  *cmmapi = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( s );
  /* ---- end of custom CMMapi destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapi.private_methods_definitions.c" { */
#include "oyCMMapi3_s_.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi5_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyCMMapi10_s_.h"

/** @internal
 *  @memberof oyCMMapi_s_
 *  @brief   check for completeness
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/05/16
 *  @since   2007/12/06 (Oyranos: 0.1.8)
 */
oyOBJECT_e       oyCMMapi_Check_     ( oyCMMapi_s         * api_ )
{
  int error = !api_;
  oyOBJECT_e type = oyOBJECT_NONE;
  int module_api;
  oyCMMapi_s_ * api = (oyCMMapi_s_*) api_;

  if(error)
    return type;

  module_api = api->module_api[0]*10000 + api->module_api[1]*100
                    + api->module_api[2];

  if(api->type_ <= oyOBJECT_CMM_API_S ||
     api->type_ >= oyOBJECT_CMM_API_MAX)
    error = 1;
  else
  {
    if(module_api < 906 ||  /* last API break */
       OYRANOS_VERSION < module_api)
    {
      error = 1;
      DBG_MEM3_S("Wrong API for: %s %s %d", oyStructTypeToText(api->type_),
                 oyNoEmptyString_m_(api->registration), module_api);
      return type;
    }
  }

  if(error <= 0)
  switch(api->type_)
  {
    case oyOBJECT_CMM_API1_S:
    {
      /* This module type is obsolete and should be replaced by oyCMMapi4_s. */
      error = 1;
    } break;
    case oyOBJECT_CMM_API2_S:
    {
      /* This module type is obsolete and should be replaced by oyCMMapi8_s. */
      error = 1;
    } break;
    case oyOBJECT_CMM_API3_S:
    {
      oyCMMapi3_s_ * s = (oyCMMapi3_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyCMMProfileTag_GetValues &&
           /*s-> &&*/
           s->oyCMMProfileTag_Create ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API4_S:
    {
      oyCMMapi4_s_ * s = (oyCMMapi4_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2])))
      {
        error = 1;
        WARNc2_S("Incomplete module header: %s %s",
                 oyStructTypeToText(api->type_),
                 oyNoEmptyString_m_(api->registration));
      }
      if(!(s->oyCMMInit && s->ui))
      {
        int ui_module_api = s->ui->module_api[0]*10000
                            + s->ui->module_api[1]*100
                            + s->ui->module_api[2];

        if(ui_module_api < 906 ||  /* last API break */
          OYRANOS_VERSION < ui_module_api)
        {
          error = 1;
          WARNc2_S("Wrong UI API for: %s %s", oyStructTypeToText(api->type_),
                   oyNoEmptyString_m_(api->registration));
          return type;
        }
      }
      if(s->ui && s->ui->oyCMMFilter_ValidateOptions &&
         !s->ui->oyWidget_Event)
      {
        error = 1;
        WARNc2_S("Incomplete module UI function set: %s %s",
                 oyStructTypeToText(api->type_),
                 oyNoEmptyString_m_(api->registration));
      }
      if(s->context_type && s->context_type[0] &&
         !s->oyCMMFilterNode_ContextToMem)
      {
        error = 1;
        WARNc2_S("context_type provided but no oyCMMFilterNode_ContextToMem: %s %s",
                 oyStructTypeToText(api->type_),
                 oyNoEmptyString_m_(api->registration));
      }
      if(!(s->ui && s->ui->texts &&
           s->ui->getText && s->ui->getText("name", oyNAME_NAME, (oyStruct_s*)
                                            s->ui)))
      {
        error = 1;
        WARNc2_S("Missed module name: %s %s",
                 oyStructTypeToText(api->type_),
                 oyNoEmptyString_m_(api->registration));
      }
      if(!(s->ui && s->ui->category && s->ui->category[0]))
      {
        error = 1;
        WARNc2_S("Missed module category: %s %s",
                 oyStructTypeToText(api->type_),
                 oyNoEmptyString_m_(api->registration));
      }
      if(s->ui && s->ui->options && s->ui->options[0] && !s->ui->oyCMMuiGet)
      {
        error = 1;
        WARNc2_S("options provided without oyCMMuiGet: %s %s",
                 oyStructTypeToText(api->type_),
                 oyNoEmptyString_m_(api->registration));
      }
    } break;
    case oyOBJECT_CMM_API5_S:
    {
      oyCMMapi5_s_ * s = (oyCMMapi5_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->oyCMMFilterLoad &&
           s->oyCMMFilterScan
            ) )
      {
        int module_api = 10000*s->module_api[0] + 100*s->module_api[1] + 1*s->module_api[2];
        if(module_api < 906) /* last API break */
          error = 1;
      }
    } break;
    case oyOBJECT_CMM_API6_S:
    {
      oyCMMapi6_s_ * s = (oyCMMapi6_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->data_type_in && s->data_type_in[0] &&
           s->data_type_out && s->data_type_out[0] &&
           s->oyModuleData_Convert
            ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API7_S:
    {
      oyCMMapi7_s_ * s = (oyCMMapi7_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->oyCMMFilterPlug_Run &&
           ((s->plugs && s->plugs_n) || (s->sockets && s->sockets_n))
            ) )
      {
        int module_api = 10000*s->module_api[0] + 100*s->module_api[1] + 1*s->module_api[2];
        if(module_api < 906) /* last API break */
          error = 1;
      }
    } break;
    case oyOBJECT_CMM_API8_S:
    {
      oyCMMapi8_s_ * s = (oyCMMapi8_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->oyConfigs_FromPattern &&
           s->oyConfigs_Modify &&
           s->oyConfig_Rank &&
           s->rank_map
            ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API9_S:
    {
      oyCMMapi9_s_ * s = (oyCMMapi9_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           (!s->options ||
            (s->options && s->oyCMMFilter_ValidateOptions &&
             s->oyCMMuiGet && s->oyWidget_Event))&&
           (!s->texts ||
            ((s->texts || s->getText)
              && s->texts[0] && s->texts[0][0] && s->getText)) &&
           s->pattern && s->pattern[0]
            ) )
      {
        int module_api = 10000*s->module_api[0] + 100*s->module_api[1] + 1*s->module_api[2];
        if(module_api < 906) /* last API break */
          error = 1;
      }
    } break;
    case oyOBJECT_CMM_API10_S:
    {
      oyCMMapi10_s_ * s = (oyCMMapi10_s_*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           (!s->texts ||
            ((s->texts || s->getText)
              && s->texts[0] && s->texts[0][0] && s->getText)) &&
           s->oyMOptions_Handle
            ) )
        error = 1;
    } break;
    default: break;
  }

  if(error <= 0)
    type = api->type_;
  else
    WARNc2_S("Found problems with: %s %s", oyStructTypeToText(api->type_),
              oyNoEmptyString_m_(api->registration));


  return type;
}

/** @internal
 *  Function oyCMMapi_CheckWrap_
 *  @memberof oyCMMapi_s_
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/16 (Oyranos: 0.1.9)
 *  @date    2009/09/02
 */
oyOBJECT_e   oyCMMapi_CheckWrap_     ( oyCMMinfo_s       * cmm_info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank )
{
  oyOBJECT_e type = oyCMMapi_Check_( api );
  if(rank)
  {
    if(type)
      *rank = 1;
    else
      *rank = 0;
  }
  return type;
}

/** @internal
 *  Function oyCMMapi_selectFilter_
 *  @brief   filter the desired api
 *  @memberof oyCMMapi_s_
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyOBJECT_e   oyCMMapi_selectFilter_  ( oyCMMinfo_s_      * info,
                                       oyCMMapi_s_       * api,
                                       oyPointer           data,
                                       uint32_t          * rank )
{
  oyOBJECT_e type = oyOBJECT_NONE;
  oyCMMapiFilter_s_ * cmm_api = (oyCMMapiFilter_s_*) api;
  int error = !data || !api;
  oyRegistrationData_s * reg_filter;
  int found = 0;

  if(error <= 0)
    reg_filter = (oyRegistrationData_s*) data;

  if(error <= 0 &&
     api->type_ == reg_filter->type)
  {
    if(reg_filter->registration)
    {
      if(oyFilterRegistrationMatch( cmm_api->registration,
                                    reg_filter->registration, api->type_ ))
        found = 1;
    } else
      found = 1;

    if( found )
    {
      type = api->type_;
      if(rank)
        *rank = 1;
      else
        *rank = 0;
    }
  }

  return type;
}

/* } Include "CMMapi.private_methods_definitions.c" */

