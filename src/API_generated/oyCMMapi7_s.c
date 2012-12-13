/** @file oyCMMapi7_s.c

   [Template file inheritance graph]
   +-> oyCMMapi7_s.template.c
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


  
#include "oyCMMapi7_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi7_s_.h"
  


/** Function oyCMMapi7_New
 *  @memberof oyCMMapi7_s
 *  @brief   allocate a new CMMapi7 object
 */
OYAPI oyCMMapi7_s * OYEXPORT
  oyCMMapi7_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi7_s_ * cmmapi7 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi7 = oyCMMapi7_New_( s );

  return (oyCMMapi7_s*) cmmapi7;
}

/** Function oyCMMapi7_Copy
 *  @memberof oyCMMapi7_s
 *  @brief   copy or reference a CMMapi7 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi7                 CMMapi7 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi7_s* OYEXPORT
  oyCMMapi7_Copy( oyCMMapi7_s *cmmapi7, oyObject_s object )
{
  oyCMMapi7_s_ * s = (oyCMMapi7_s_*) cmmapi7;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_API7_S, return 0 )

  s = oyCMMapi7_Copy_( s, object );

  return (oyCMMapi7_s*) s;
}
 
/** Function oyCMMapi7_Release
 *  @memberof oyCMMapi7_s
 *  @brief   release and possibly deallocate a oyCMMapi7_s object
 *
 *  @param[in,out] cmmapi7                 CMMapi7 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi7_Release( oyCMMapi7_s **cmmapi7 )
{
  oyCMMapi7_s_ * s = 0;

  if(!cmmapi7 || !*cmmapi7)
    return 0;

  s = (oyCMMapi7_s_*) *cmmapi7;

  oyCheckType__m( oyOBJECT_CMM_API7_S, return 1 )

  *cmmapi7 = 0;

  return oyCMMapi7_Release_( &s );
}



/* Include "CMMapi7.public_methods_definitions.c" { */
OYAPI int OYEXPORT
             oyCMMapi7_Run           ( oyCMMapi7_s       * api7,
                                       oyFilterPlug_s    * plug,
                                       oyPixelAccess_s   * ticket )
{
  oyCMMapi7_s_ * s = (oyCMMapi7_s_*)api7;
  int error = !plug;

  if(!api7)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API7_S, return 1 )

  if(!plug)
  {
    WARNcc_S(api7, "no plug argument given" );
    return error;
  }

  error = s->oyCMMFilterPlug_Run( plug, ticket );

  return error;
}

/* } Include "CMMapi7.public_methods_definitions.c" */

