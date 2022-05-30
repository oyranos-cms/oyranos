/** @file oyCMMapi6_s.c

   [Template file inheritance graph]
   +-> oyCMMapi6_s.template.c
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


  
#include "oyCMMapi6_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi6_s_.h"
  



/** Function oyCMMapi6_New
 *  @memberof oyCMMapi6_s
 *  @brief   allocate a new CMMapi6 object
 */
OYAPI oyCMMapi6_s * OYEXPORT
  oyCMMapi6_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi6_s_ * cmmapi6 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi6 = oyCMMapi6_New_( s );

  return (oyCMMapi6_s*) cmmapi6;
}

/** Function  oyCMMapi6_Copy
 *  @memberof oyCMMapi6_s
 *  @brief    Copy or Reference a CMMapi6 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi6                 CMMapi6 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi6_s* OYEXPORT
  oyCMMapi6_Copy( oyCMMapi6_s *cmmapi6, oyObject_s object )
{
  oyCMMapi6_s_ * s = (oyCMMapi6_s_*) cmmapi6;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_API6_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapi6_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapi6_s" );

  return (oyCMMapi6_s*) s;
}
 
/** Function oyCMMapi6_Release
 *  @memberof oyCMMapi6_s
 *  @brief   release and possibly deallocate a oyCMMapi6_s object
 *
 *  @param[in,out] cmmapi6                 CMMapi6 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi6_Release( oyCMMapi6_s **cmmapi6 )
{
  oyCMMapi6_s_ * s = 0;

  if(!cmmapi6 || !*cmmapi6)
    return 0;

  s = (oyCMMapi6_s_*) *cmmapi6;

  oyCheckType__m( oyOBJECT_CMM_API6_S, return 1 )

  *cmmapi6 = 0;

  return oyCMMapi6_Release_( &s );
}

/**
 *  @memberof oyCMMapi6_s
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
           oyCMMapi6_SetBackendContext ( oyCMMapi6_s       * api,
                                       oyPointer_s       * ptr )
{
  oyCMMapi6_s_ * s = (oyCMMapi6_s_*)api;
  int error = 0;

  if(!s)
    return -1;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API6_S, oyOBJECT_CMM_API_MAX, return 1 )

  {
    if(s->runtime_context)
      oyPointer_Release( &s->runtime_context );
    s->runtime_context = oyPointer_Copy( ptr, NULL );
  }   

  return error;
}

/**
 *  @memberof oyCMMapi6_s
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
           oyCMMapi6_GetBackendContext ( oyCMMapi6_s       * api )
{
  oyCMMapi6_s_ * s = (oyCMMapi6_s_*)api;
  oyPointer_s * ptr = NULL;

  if(!s)
    return ptr;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API6_S, oyOBJECT_CMM_API_MAX, return NULL )

  oyPointer_Copy( s->runtime_context, NULL );
  return s->runtime_context;
}



/* Include "CMMapi6.public_methods_definitions.c" { */
/**
 *  @memberof   oyCMMapi6_s
 *  @brief      Get data type identifier
 *
 *  @param         api                 object
 *  @param         out_direction       
 *                                     - 0 - input data type
 *                                     - 1 - output data type
 *  @return                            the identifier string
 *
 *  @version Oyranos: 0.9.5
 *  @since   2014/01/10 (Oyranos: 0.9.5)
 *  @date    2014/01/10
 */
const char *       oyCMMapi6_GetDataType
                                     ( oyCMMapi6_s       * api,
                                       int                 out_direction )
{
  if(out_direction)
    return ((oyCMMapi6_s_*)api)->data_type_out;
  else
    return ((oyCMMapi6_s_*)api)->data_type_in;
}

/**
 *  @memberof   oyCMMapi6_s
 *  @brief      Get data conversion function
 *
 *  @param         api                 object
 *  @return                            the function
 *
 *  @version Oyranos: 0.9.5
 *  @since   2014/01/10 (Oyranos: 0.9.5)
 *  @date    2014/01/10
 */
oyModuleData_Convert_f
                   oyCMMapi6_GetConvertF
                                     ( oyCMMapi6_s       * api )
{
  return ((oyCMMapi6_s_*)api)->oyModuleData_Convert;
}

/* } Include "CMMapi6.public_methods_definitions.c" */

