/** @file oyCMMobjectType_s.c

   [Template file inheritance graph]
   +-> oyCMMobjectType_s.template.c
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


  
#include "oyCMMobjectType_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMobjectType_s_.h"
  


/** Function oyCMMobjectType_New
 *  @memberof oyCMMobjectType_s
 *  @brief   allocate a new CMMobjectType object
 */
OYAPI oyCMMobjectType_s * OYEXPORT
  oyCMMobjectType_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMobjectType_s_ * cmmobjecttype = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmobjecttype = oyCMMobjectType_New_( s );

  return (oyCMMobjectType_s*) cmmobjecttype;
}

/** Function  oyCMMobjectType_Copy
 *  @memberof oyCMMobjectType_s
 *  @brief    Copy or Reference a CMMobjectType object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmobjecttype                 CMMobjectType struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMobjectType_s* OYEXPORT
  oyCMMobjectType_Copy( oyCMMobjectType_s *cmmobjecttype, oyObject_s object )
{
  oyCMMobjectType_s_ * s = (oyCMMobjectType_s_*) cmmobjecttype;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_OBJECT_TYPE_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMobjectType_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMobjectType_s" );

  return (oyCMMobjectType_s*) s;
}
 
/** Function oyCMMobjectType_Release
 *  @memberof oyCMMobjectType_s
 *  @brief   release and possibly deallocate a oyCMMobjectType_s object
 *
 *  @param[in,out] cmmobjecttype                 CMMobjectType struct object
 */
OYAPI int OYEXPORT
  oyCMMobjectType_Release( oyCMMobjectType_s **cmmobjecttype )
{
  oyCMMobjectType_s_ * s = 0;

  if(!cmmobjecttype || !*cmmobjecttype)
    return 0;

  s = (oyCMMobjectType_s_*) *cmmobjecttype;

  oyCheckType__m( oyOBJECT_CMM_OBJECT_TYPE_S, return 1 )

  *cmmobjecttype = 0;

  return oyCMMobjectType_Release_( &s );
}



/* Include "CMMobjectType.public_methods_definitions.c" { */
oyOBJECT_e         oyCMMobjectType_GetId
                                     ( oyCMMobjectType_s * obj )
{
  return ((oyCMMobjectType_s_*) obj)->id;
}
const char *       oyCMMobjectType_GetPaths
                                     ( oyCMMobjectType_s * obj )
{
  return ((oyCMMobjectType_s_*) obj)->paths;
}
oyCMMobjectGetText_f
                   oyCMMobjectType_GetTextF
                                     ( oyCMMobjectType_s * obj )
{
  return ((oyCMMobjectType_s_*) obj)->oyCMMobjectGetText;
}

/* } Include "CMMobjectType.public_methods_definitions.c" */

