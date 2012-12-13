/** @file oyCMMapi_s.c

   [Template file inheritance graph]
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


  
#include "oyCMMapi_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi_s_.h"
  


/** Function oyCMMapi_New
 *  @memberof oyCMMapi_s
 *  @brief   allocate a new CMMapi object
 */
OYAPI oyCMMapi_s * OYEXPORT
  oyCMMapi_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi_s_ * cmmapi = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi = oyCMMapi_New_( s );

  return (oyCMMapi_s*) cmmapi;
}

/** Function oyCMMapi_Copy
 *  @memberof oyCMMapi_s
 *  @brief   copy or reference a CMMapi object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi                 CMMapi struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi_s* OYEXPORT
  oyCMMapi_Copy( oyCMMapi_s *cmmapi, oyObject_s object )
{
  oyCMMapi_s_ * s = (oyCMMapi_s_*) cmmapi;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_API_S, return 0 )

  s = oyCMMapi_Copy_( s, object );

  return (oyCMMapi_s*) s;
}
 
/** Function oyCMMapi_Release
 *  @memberof oyCMMapi_s
 *  @brief   release and possibly deallocate a oyCMMapi_s object
 *
 *  @param[in,out] cmmapi                 CMMapi struct object
 */
OYAPI int OYEXPORT
  oyCMMapi_Release( oyCMMapi_s **cmmapi )
{
  oyCMMapi_s_ * s = 0;

  if(!cmmapi || !*cmmapi)
    return 0;

  s = (oyCMMapi_s_*) *cmmapi;

  oyCheckType__m( oyOBJECT_CMM_API_S, return 1 )

  *cmmapi = 0;

  return oyCMMapi_Release_( &s );
}



/* Include "CMMapi.public_methods_definitions.c" { */
oyCMMapi_s *       oyCMMapi_GetNext  ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->next;
}
oyCMMInit_f        oyCMMapi_GetInitF ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->oyCMMInit;
}
oyCMMMessageFuncSet_f
                   oyCMMapi_GetMessageFuncSetF
                                     ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->oyCMMMessageFuncSet;
}
const char *       oyCMMapi_GetRegistration
                                     ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->registration;
}

/* } Include "CMMapi.public_methods_definitions.c" */

