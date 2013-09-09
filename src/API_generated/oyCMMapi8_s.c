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
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/12/13
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

/** Function oyCMMapi8_Copy
 *  @memberof oyCMMapi8_s
 *  @brief   copy or reference a CMMapi8 object
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
    oyCheckType__m( oyOBJECT_CMM_API8_S, return 0 )

  s = oyCMMapi8_Copy_( s, object );

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



/* Include "CMMapi8.public_methods_definitions.c" { */
oyCMMui_s *        oyCMMapi8_GetUi   ( oyCMMapi8_s       * api )
{
  return ((oyCMMapi8_s_*)api)->ui;
}

/* } Include "CMMapi8.public_methods_definitions.c" */

