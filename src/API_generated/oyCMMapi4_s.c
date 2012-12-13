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

/** Function oyCMMapi4_Copy
 *  @memberof oyCMMapi4_s
 *  @brief   copy or reference a CMMapi4 object
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
    oyCheckType__m( oyOBJECT_CMM_API4_S, return 0 )

  s = oyCMMapi4_Copy_( s, object );

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



/* Include "CMMapi4.public_methods_definitions.c" { */

/* } Include "CMMapi4.public_methods_definitions.c" */

