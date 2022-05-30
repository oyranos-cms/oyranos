/** @file oyCMMapi9_s.c

   [Template file inheritance graph]
   +-> oyCMMapi9_s.template.c
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


  
#include "oyCMMapi9_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi9_s_.h"
  



/** Function oyCMMapi9_New
 *  @memberof oyCMMapi9_s
 *  @brief   allocate a new CMMapi9 object
 */
OYAPI oyCMMapi9_s * OYEXPORT
  oyCMMapi9_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi9_s_ * cmmapi9 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi9 = oyCMMapi9_New_( s );

  return (oyCMMapi9_s*) cmmapi9;
}

/** Function  oyCMMapi9_Copy
 *  @memberof oyCMMapi9_s
 *  @brief    Copy or Reference a CMMapi9 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi9                 CMMapi9 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi9_s* OYEXPORT
  oyCMMapi9_Copy( oyCMMapi9_s *cmmapi9, oyObject_s object )
{
  oyCMMapi9_s_ * s = (oyCMMapi9_s_*) cmmapi9;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_API9_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapi9_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapi9_s" );

  return (oyCMMapi9_s*) s;
}
 
/** Function oyCMMapi9_Release
 *  @memberof oyCMMapi9_s
 *  @brief   release and possibly deallocate a oyCMMapi9_s object
 *
 *  @param[in,out] cmmapi9                 CMMapi9 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi9_Release( oyCMMapi9_s **cmmapi9 )
{
  oyCMMapi9_s_ * s = 0;

  if(!cmmapi9 || !*cmmapi9)
    return 0;

  s = (oyCMMapi9_s_*) *cmmapi9;

  oyCheckType__m( oyOBJECT_CMM_API9_S, return 1 )

  *cmmapi9 = 0;

  return oyCMMapi9_Release_( &s );
}

/**
 *  @memberof oyCMMapi9_s
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
           oyCMMapi9_SetBackendContext ( oyCMMapi9_s       * api,
                                       oyPointer_s       * ptr )
{
  oyCMMapi9_s_ * s = (oyCMMapi9_s_*)api;
  int error = 0;

  if(!s)
    return -1;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API9_S, oyOBJECT_CMM_API_MAX, return 1 )

  {
    if(s->runtime_context)
      oyPointer_Release( &s->runtime_context );
    s->runtime_context = oyPointer_Copy( ptr, NULL );
  }   

  return error;
}

/**
 *  @memberof oyCMMapi9_s
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
           oyCMMapi9_GetBackendContext ( oyCMMapi9_s       * api )
{
  oyCMMapi9_s_ * s = (oyCMMapi9_s_*)api;
  oyPointer_s * ptr = NULL;

  if(!s)
    return ptr;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API9_S, oyOBJECT_CMM_API_MAX, return NULL )

  oyPointer_Copy( s->runtime_context, NULL );
  return s->runtime_context;
}



/* Include "CMMapi9.public_methods_definitions.c" { */
oyCMMobjectType_s **
                   oyCMMapi9_GetObjectTypes
                                     ( oyCMMapi9_s       * api )
{
  return ((oyCMMapi9_s_*)api)->object_types;
}


/* } Include "CMMapi9.public_methods_definitions.c" */

