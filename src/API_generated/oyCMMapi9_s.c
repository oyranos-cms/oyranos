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

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/12/13
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

/** Function oyCMMapi9_Copy
 *  @memberof oyCMMapi9_s
 *  @brief   copy or reference a CMMapi9 object
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
    oyCheckType__m( oyOBJECT_CMM_API9_S, return 0 )

  s = oyCMMapi9_Copy_( s, object );

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



/* Include "CMMapi9.public_methods_definitions.c" { */
oyCMMobjectType_s **
                   oyCMMapi9_GetObjectTypes
                                     ( oyCMMapi9_s       * api )
{
  return ((oyCMMapi9_s_*)api)->object_types;
}


/* } Include "CMMapi9.public_methods_definitions.c" */

