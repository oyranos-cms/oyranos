/** @file oyCMMapi5_s.c

   [Template file inheritance graph]
   +-> oyCMMapi5_s.template.c
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


  
#include "oyCMMapi5_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyCMMapi5_s_.h"

#include "oyranos_image_internal.h"
#include "oyConnectorImaging_s_.h"
#include "oyFilterPlug_s_.h"
#include "oyImage_s_.h"
  


/** Function oyCMMapi5_New
 *  @memberof oyCMMapi5_s
 *  @brief   allocate a new CMMapi5 object
 */
OYAPI oyCMMapi5_s * OYEXPORT
  oyCMMapi5_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi5_s_ * cmmapi5 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi5 = oyCMMapi5_New_( s );

  return (oyCMMapi5_s*) cmmapi5;
}

/** Function  oyCMMapi5_Copy
 *  @memberof oyCMMapi5_s
 *  @brief    Copy or Reference a CMMapi5 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi5                 CMMapi5 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi5_s* OYEXPORT
  oyCMMapi5_Copy( oyCMMapi5_s *cmmapi5, oyObject_s object )
{
  oyCMMapi5_s_ * s = (oyCMMapi5_s_*) cmmapi5;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_API5_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapi5_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapi5_s" );

  return (oyCMMapi5_s*) s;
}
 
/** Function oyCMMapi5_Release
 *  @memberof oyCMMapi5_s
 *  @brief   release and possibly deallocate a oyCMMapi5_s object
 *
 *  @param[in,out] cmmapi5                 CMMapi5 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi5_Release( oyCMMapi5_s **cmmapi5 )
{
  oyCMMapi5_s_ * s = 0;

  if(!cmmapi5 || !*cmmapi5)
    return 0;

  s = (oyCMMapi5_s_*) *cmmapi5;

  oyCheckType__m( oyOBJECT_CMM_API5_S, return 1 )

  *cmmapi5 = 0;

  return oyCMMapi5_Release_( &s );
}



/* Include "CMMapi5.public_methods_definitions.c" { */

/* } Include "CMMapi5.public_methods_definitions.c" */

