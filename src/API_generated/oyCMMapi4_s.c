/** @file oyCMMapi4_s.c

   [Template file inheritance graph]
   +-> oyCMMapi4_s.template.c
   |
   +-> oyCMMapiFilter_s.template.c
   |
   +-> oyCMMapi_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyCMMapi4_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi4_s_.h"
  



/** Function oyCMMapi4_New
 *  @memberof oyCMMapi4_s
 *  @brief   allocate a new CMMapi4 object
 */
OYAPI oyCMMapi4_s * OYEXPORT
  oyCMMapi4_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi4_s_ * cmmapi4 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi4 = oyCMMapi4_New_( s );

  return (oyCMMapi4_s*) cmmapi4;
}

/** Function  oyCMMapi4_Copy
 *  @memberof oyCMMapi4_s
 *  @brief    Copy or Reference a CMMapi4 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi4                 CMMapi4 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi4_s* OYEXPORT
  oyCMMapi4_Copy( oyCMMapi4_s *cmmapi4, oyObject_s object )
{
  oyCMMapi4_s_ * s = (oyCMMapi4_s_*) cmmapi4;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_API4_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapi4_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapi4_s" );

  return (oyCMMapi4_s*) s;
}
 
/** Function oyCMMapi4_Release
 *  @memberof oyCMMapi4_s
 *  @brief   release and possibly deallocate a oyCMMapi4_s object
 *
 *  @param[in,out] cmmapi4                 CMMapi4 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi4_Release( oyCMMapi4_s **cmmapi4 )
{
  oyCMMapi4_s_ * s = 0;

  if(!cmmapi4 || !*cmmapi4)
    return 0;

  s = (oyCMMapi4_s_*) *cmmapi4;

  oyCheckType__m( oyOBJECT_CMM_API4_S, return 1 )

  *cmmapi4 = 0;

  return oyCMMapi4_Release_( &s );
}

/**
 *  @memberof oyCMMapi4_s
 *  @brief   set filter type specific runtime data
 *
 *  Runtime data can be used as context by a backend during execution. The data
 *  is typical set during backend load.
 *
 *  That data is apart from a filter object, which can have lifetime data
 *  associated through a oyFilterNode_GetContext(). A filter connector
 *  can have its processing data associated through oyFilterNode_SetData().
 *
 *  @param[in,out] api                 api object
 *  @param[in]     ptr                 the data needed to run the filter type
 *  @return                            error
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/19
 *  @since   2013/12/19 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
           oyCMMapi4_SetBackendContext ( oyCMMapi4_s       * api,
                                       oyPointer_s       * ptr )
{
  oyCMMapi4_s_ * s = (oyCMMapi4_s_*)api;
  int error = 0;

  if(!s)
    return -1;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API4_S, oyOBJECT_CMM_API_MAX, return 1 )

  {
    if(s->runtime_context)
      oyPointer_Release( &s->runtime_context );
    s->runtime_context = oyPointer_Copy( ptr, NULL );
  }   

  return error;
}

/**
 *  @memberof oyCMMapi4_s
 *  @brief   get filter type specific runtime data
 *
 *  Runtime data can be used as context by a backend during execution.
 *
 *  That data is apart from a filter object, which can have lifetime data
 *  associated through a oyFilterNode_GetContext(). A filter connector
 *  can have its processing data associated through oyFilterNode_SetData().
 *
 *  @param[in]     api                 api object
 *  @return                            the context needed to run the filter type
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/19
 *  @since   2013/12/19 (Oyranos: 0.9.5)
 */
OYAPI oyPointer_s * OYEXPORT
           oyCMMapi4_GetBackendContext ( oyCMMapi4_s       * api )
{
  oyCMMapi4_s_ * s = (oyCMMapi4_s_*)api;
  oyPointer_s * ptr = NULL;

  if(!s)
    return ptr;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API4_S, oyOBJECT_CMM_API_MAX, return NULL )

  oyPointer_Copy( s->runtime_context, NULL );
  return s->runtime_context;
}



/* Include "CMMapi4.public_methods_definitions.c" { */
/** Function    oyCMMapi4_Create
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 constructor
 *
 *  @param         init                custom initialisation
 *  @param         reset               custom deinitialisation
 *  @param         msg_set             message function setter
 *  @param         registration        the modules @ref registration string,
 *  @param         version             module version
 *  - 0: major - should be stable for the live time of a filter
 *  - 1: minor - mark new features
 *  - 2: patch version - correct errors
 *  @param         module_api          tell compatibility with Oyranos API
 *  - 0: last major Oyranos version during development time, e.g. 0
 *  - 1: last minor Oyranos version during development time, e.g. 9
 *  - 2: last Oyranos patch version during development time, e.g. 5
 *  @param         context_type        tell the context type, if any
 *  the data type of the context returned by oyCMMapi4_s::oyCMMFilterNode_ContextToMem_f, mandatory in case of a set oyCMMapi4_s::oyCMMFilterNode_ContextToMem
 *  e.g. ::oyCOLOR_ICC_DEVICE_LINK / "oyDL"
 *  @param         contextToMem 
 *  only mandatory for context producing filters, e.g. "//color/icc"
 *  @param         getText             optionally set a 
 *                                     oyCMMFilterNode_ContextToMem,
 *  used to override a Oyranos side hash creation
 *  @param         ui                  a UI description
 *  provide a oyCMMapi4_s::ui->getText( select, type ) call. The "select"
 *  argument shall cover at least "name" and "help"
 *  @param         object              a optional object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/03
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 */
OYAPI oyCMMapi4_s*  OYEXPORT
                   oyCMMapi4_Create  ( oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       const char        * context_type,
                                       oyCMMFilterNode_ContextToMem_f contextToMem,
                                       oyCMMFilterNode_GetText_f getText,
                                       oyCMMui_s         * ui,
                                       oyObject_s          object )
{
  oyCMMapi4_s_ * api4 = (oyCMMapi4_s_*) oyCMMapi4_New( object ); 
  if(!api4)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  oyCMMapi_Set( (oyCMMapi_s*) api4, init, reset, msg_set, registration,
                version, module_api );

  if(context_type)
    memcpy( api4->context_type, context_type, 8 );
  api4->oyCMMFilterNode_ContextToMem = contextToMem;
  api4->oyCMMFilterNode_GetText = getText;
  api4->ui = (oyCMMui_s_*) oyCMMui_Copy( ui, object );
  if(api4->ui)
  {
    api4->ui->parent = (oyCMMapiFilter_s*) oyCMMapi4_Copy( (oyCMMapi4_s*) api4, NULL );
  }

  return (oyCMMapi4_s*) api4;
}

/** Function    oyCMMapi4_GetUi
 *  @memberof   oyCMMapi4_s
 *  @brief      Get ui object from module
 *
 *  @param         obj                 the api4 module
 *  @return                            a ui object
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/06/10 (Oyranos: 0.9.4)
 *  @date    2014/06/10
 */
OYAPI oyCMMui_s*  OYEXPORT
                   oyCMMapi4_GetUi   ( oyCMMapi4_s       * obj )
{
  oyCMMapi4_s_ * api4 = (oyCMMapi4_s_*) obj; 
  if(!api4)
    return NULL;

  return (oyCMMui_s*) api4->ui;
}


/* } Include "CMMapi4.public_methods_definitions.c" */

