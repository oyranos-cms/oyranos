/** @file oyCMMapiFilter_s.c

   [Template file inheritance graph]
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


  
#include "oyCMMapiFilter_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapiFilter_s_.h"
  



/** Function oyCMMapiFilter_New
 *  @memberof oyCMMapiFilter_s
 *  @brief   allocate a new CMMapiFilter object
 */
OYAPI oyCMMapiFilter_s * OYEXPORT
  oyCMMapiFilter_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapiFilter_s_ * cmmapifilter = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapifilter = oyCMMapiFilter_New_( s );

  return (oyCMMapiFilter_s*) cmmapifilter;
}

/** Function  oyCMMapiFilter_Copy
 *  @memberof oyCMMapiFilter_s
 *  @brief    Copy or Reference a CMMapiFilter object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapifilter                 CMMapiFilter struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapiFilter_s* OYEXPORT
  oyCMMapiFilter_Copy( oyCMMapiFilter_s *cmmapifilter, oyObject_s object )
{
  oyCMMapiFilter_s_ * s = (oyCMMapiFilter_s_*) cmmapifilter;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_API_FILTER_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapiFilter_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapiFilter_s" );

  return (oyCMMapiFilter_s*) s;
}
 
/** Function oyCMMapiFilter_Release
 *  @memberof oyCMMapiFilter_s
 *  @brief   release and possibly deallocate a oyCMMapiFilter_s object
 *
 *  @param[in,out] cmmapifilter                 CMMapiFilter struct object
 */
OYAPI int OYEXPORT
  oyCMMapiFilter_Release( oyCMMapiFilter_s **cmmapifilter )
{
  oyCMMapiFilter_s_ * s = 0;

  if(!cmmapifilter || !*cmmapifilter)
    return 0;

  s = (oyCMMapiFilter_s_*) *cmmapifilter;

  oyCheckType__m( oyOBJECT_CMM_API_FILTER_S, return 1 )

  *cmmapifilter = 0;

  return oyCMMapiFilter_Release_( &s );
}

/**
 *  @memberof oyCMMapiFilter_s
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
           oyCMMapiFilter_SetBackendContext ( oyCMMapiFilter_s       * api,
                                       oyPointer_s       * ptr )
{
  oyCMMapiFilter_s_ * s = (oyCMMapiFilter_s_*)api;
  int error = 0;

  if(!s)
    return -1;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API_FILTER_S, oyOBJECT_CMM_API_MAX, return 1 )

  {
    if(s->runtime_context)
      oyPointer_Release( &s->runtime_context );
    s->runtime_context = oyPointer_Copy( ptr, NULL );
  }   

  return error;
}

/**
 *  @memberof oyCMMapiFilter_s
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
           oyCMMapiFilter_GetBackendContext ( oyCMMapiFilter_s       * api )
{
  oyCMMapiFilter_s_ * s = (oyCMMapiFilter_s_*)api;
  oyPointer_s * ptr = NULL;

  if(!s)
    return ptr;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API_FILTER_S, oyOBJECT_CMM_API_MAX, return NULL )

  oyPointer_Copy( s->runtime_context, NULL );
  return s->runtime_context;
}



/* Include "CMMapiFilter.public_methods_definitions.c" { */

/* } Include "CMMapiFilter.public_methods_definitions.c" */

