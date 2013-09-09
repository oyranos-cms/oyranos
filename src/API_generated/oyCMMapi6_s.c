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
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/12/13
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

/** Function oyCMMapi6_Copy
 *  @memberof oyCMMapi6_s
 *  @brief   copy or reference a CMMapi6 object
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
    oyCheckType__m( oyOBJECT_CMM_API6_S, return 0 )

  s = oyCMMapi6_Copy_( s, object );

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



/* Include "CMMapi6.public_methods_definitions.c" { */
const char *       oyCMMapi6_GetDataType
                                     ( oyCMMapi6_s       * api,
                                       int                 out_direction )
{
  if(out_direction)
    return ((oyCMMapi6_s_*)api)->data_type_out;
  else
    return ((oyCMMapi6_s_*)api)->data_type_in;
}

oyModuleData_Convert_f
                   oyCMMapi6_GetConvertF
                                     ( oyCMMapi6_s       * api )
{
  return ((oyCMMapi6_s_*)api)->oyModuleData_Convert;
}

/* } Include "CMMapi6.public_methods_definitions.c" */

