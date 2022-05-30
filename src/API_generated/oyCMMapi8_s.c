/** @file oyCMMapi8_s.c

   [Template file inheritance graph]
   +-> oyCMMapi8_s.template.c
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


  
#include "oyCMMapi8_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi8_s_.h"
  



/** Function oyCMMapi8_New
 *  @memberof oyCMMapi8_s
 *  @brief   allocate a new CMMapi8 object
 */
OYAPI oyCMMapi8_s * OYEXPORT
  oyCMMapi8_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi8_s_ * cmmapi8 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi8 = oyCMMapi8_New_( s );

  return (oyCMMapi8_s*) cmmapi8;
}

/** Function  oyCMMapi8_Copy
 *  @memberof oyCMMapi8_s
 *  @brief    Copy or Reference a CMMapi8 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi8                 CMMapi8 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi8_s* OYEXPORT
  oyCMMapi8_Copy( oyCMMapi8_s *cmmapi8, oyObject_s object )
{
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*) cmmapi8;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_API8_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapi8_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapi8_s" );

  return (oyCMMapi8_s*) s;
}
 
/** Function oyCMMapi8_Release
 *  @memberof oyCMMapi8_s
 *  @brief   release and possibly deallocate a oyCMMapi8_s object
 *
 *  @param[in,out] cmmapi8                 CMMapi8 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi8_Release( oyCMMapi8_s **cmmapi8 )
{
  oyCMMapi8_s_ * s = 0;

  if(!cmmapi8 || !*cmmapi8)
    return 0;

  s = (oyCMMapi8_s_*) *cmmapi8;

  oyCheckType__m( oyOBJECT_CMM_API8_S, return 1 )

  *cmmapi8 = 0;

  return oyCMMapi8_Release_( &s );
}

/**
 *  @memberof oyCMMapi8_s
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
           oyCMMapi8_SetBackendContext ( oyCMMapi8_s       * api,
                                       oyPointer_s       * ptr )
{
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*)api;
  int error = 0;

  if(!s)
    return -1;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API8_S, oyOBJECT_CMM_API_MAX, return 1 )

  {
    if(s->runtime_context)
      oyPointer_Release( &s->runtime_context );
    s->runtime_context = oyPointer_Copy( ptr, NULL );
  }   

  return error;
}

/**
 *  @memberof oyCMMapi8_s
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
           oyCMMapi8_GetBackendContext ( oyCMMapi8_s       * api )
{
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*)api;
  oyPointer_s * ptr = NULL;

  if(!s)
    return ptr;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API8_S, oyOBJECT_CMM_API_MAX, return NULL )

  oyPointer_Copy( s->runtime_context, NULL );
  return s->runtime_context;
}



/* Include "CMMapi8.public_methods_definitions.c" { */
oyCMMui_s *        oyCMMapi8_GetUi   ( oyCMMapi8_s       * api )
{
  return ((oyCMMapi8_s_*)api)->ui;
}

/* } Include "CMMapi8.public_methods_definitions.c" */

