/** @file oyCMMui_s.c

   [Template file inheritance graph]
   +-> oyCMMui_s.template.c
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


  
#include "oyCMMui_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMui_s_.h"
  


/** Function oyCMMui_New
 *  @memberof oyCMMui_s
 *  @brief   allocate a new CMMui object
 */
OYAPI oyCMMui_s * OYEXPORT
  oyCMMui_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMui_s_ * cmmui = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmui = oyCMMui_New_( s );

  return (oyCMMui_s*) cmmui;
}

/** Function oyCMMui_Copy
 *  @memberof oyCMMui_s
 *  @brief   copy or reference a CMMui object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmui                 CMMui struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMui_s* OYEXPORT
  oyCMMui_Copy( oyCMMui_s *cmmui, oyObject_s object )
{
  oyCMMui_s_ * s = (oyCMMui_s_*) cmmui;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_UI_S, return 0 )

  s = oyCMMui_Copy_( s, object );

  return (oyCMMui_s*) s;
}
 
/** Function oyCMMui_Release
 *  @memberof oyCMMui_s
 *  @brief   release and possibly deallocate a oyCMMui_s object
 *
 *  @param[in,out] cmmui                 CMMui struct object
 */
OYAPI int OYEXPORT
  oyCMMui_Release( oyCMMui_s **cmmui )
{
  oyCMMui_s_ * s = 0;

  if(!cmmui || !*cmmui)
    return 0;

  s = (oyCMMui_s_*) *cmmui;

  oyCheckType__m( oyOBJECT_CMM_UI_S, return 1 )

  *cmmui = 0;

  return oyCMMui_Release_( &s );
}



/* Include "CMMui.public_methods_definitions.c" { */

oyCMMGetText_f     oyCMMui_GetTextF  ( oyCMMui_s         * ui )
{
  return ((oyCMMui_s_*)ui)->getText;
}
/** Function  oyCMMui_GetTexts
 *  @memberof oyCMMui_s
 *  @brief    Get texts for GetTextF
 *
 *  @param[in]     node                ui object
 *  @return                            the zero terminated string list
 *
 *  zero terminated categories for getText,
 *  e.g. {"name","category","help",0}
 *
 *  The "name" texts shall include information about the module.
 *  The "category" gives in oyNAME_NAME a translated version of
 *  oyCMMui_s::category.
 *  The "help" texts should provide general infromations about the module.
 *
 *  The oyNAME_NICK for the several oyCMMui_s::getText() texts is typical not
 *  translated. For "name" the oyNAME_NICK should be the module name. For 
 *  other texts like "help" and "category" oyNAME_NICK makes no sense.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/15
 *  @since    2012/09/15 (Oyranos: 0.5.0)
 */
const char **      oyCMMui_GetTexts  ( oyCMMui_s         * ui )
{
  return ((oyCMMui_s_*)ui)->texts;
}

/* } Include "CMMui.public_methods_definitions.c" */

