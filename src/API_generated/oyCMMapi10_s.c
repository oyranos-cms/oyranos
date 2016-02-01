/** @file oyCMMapi10_s.c

   [Template file inheritance graph]
   +-> oyCMMapi10_s.template.c
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
 *            2004-2016 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyCMMapi10_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi10_s_.h"
  



/** Function oyCMMapi10_New
 *  @memberof oyCMMapi10_s
 *  @brief   allocate a new CMMapi10 object
 */
OYAPI oyCMMapi10_s * OYEXPORT
  oyCMMapi10_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi10_s_ * cmmapi10 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi10 = oyCMMapi10_New_( s );

  return (oyCMMapi10_s*) cmmapi10;
}

/** Function oyCMMapi10_Copy
 *  @memberof oyCMMapi10_s
 *  @brief   copy or reference a CMMapi10 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi10                 CMMapi10 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi10_s* OYEXPORT
  oyCMMapi10_Copy( oyCMMapi10_s *cmmapi10, oyObject_s object )
{
  oyCMMapi10_s_ * s = (oyCMMapi10_s_*) cmmapi10;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_API10_S, return 0 )

  s = oyCMMapi10_Copy_( s, object );

  return (oyCMMapi10_s*) s;
}
 
/** Function oyCMMapi10_Release
 *  @memberof oyCMMapi10_s
 *  @brief   release and possibly deallocate a oyCMMapi10_s object
 *
 *  @param[in,out] cmmapi10                 CMMapi10 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi10_Release( oyCMMapi10_s **cmmapi10 )
{
  oyCMMapi10_s_ * s = 0;

  if(!cmmapi10 || !*cmmapi10)
    return 0;

  s = (oyCMMapi10_s_*) *cmmapi10;

  oyCheckType__m( oyOBJECT_CMM_API10_S, return 1 )

  *cmmapi10 = 0;

  return oyCMMapi10_Release_( &s );
}

/**
 *  @memberof oyCMMapi10_s
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
           oyCMMapi10_SetBackendContext ( oyCMMapi10_s       * api,
                                       oyPointer_s       * ptr )
{
  oyCMMapi10_s_ * s = (oyCMMapi10_s_*)api;
  int error = 0;

  if(!s)
    return -1;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API10_S, oyOBJECT_CMM_API_MAX, return 1 )

  {
    if(s->runtime_context)
      oyPointer_Release( &s->runtime_context );
    s->runtime_context = oyPointer_Copy( ptr, NULL );
  }   

  return error;
}

/**
 *  @memberof oyCMMapi10_s
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
           oyCMMapi10_GetBackendContext ( oyCMMapi10_s       * api )
{
  oyCMMapi10_s_ * s = (oyCMMapi10_s_*)api;
  oyPointer_s * ptr = NULL;

  if(!s)
    return ptr;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_CMM_API10_S, oyOBJECT_CMM_API_MAX, return NULL )

  return oyPointer_Copy( s->runtime_context, NULL );
}



/* Include "CMMapi10.public_methods_definitions.c" { */

/* } Include "CMMapi10.public_methods_definitions.c" */

