/** @file oyCMMui_s_.h

   [Template file inheritance graph]
   +-> oyCMMui_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_CMM_UI_S__H
#define OY_CMM_UI_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMuiPriv_m( var ) ((oyCMMui_s_*) (var))

typedef struct oyCMMui_s_ oyCMMui_s_;

  
#include <oyranos_object.h>


#include "oyranos_module_internal.h"
  

#include "oyCMMui_s.h"

/* Include "CMMui.private.h" { */

/* } Include "CMMui.private.h" */

/** @internal
 *  @struct   oyCMMui_s_
 *  @brief      The CMM API UI part
 *  @ingroup  module_api
 *  @extends  oyStruct_s
 */
struct oyCMMui_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "CMMui.members.h" { */
  /** 0: major - should be stable for the live time of a filters UI, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  /** check options for validy and correct */
  oyCMMFilter_ValidateOptions_f    oyCMMFilter_ValidateOptions;
  oyWidgetEvent_f                  oyWidget_Event; /**< handle widget events */

  const char     * category;           /**< menu structure */
  const char     * options;            /**< default options as Elektra/XFORMS 
                                        *   compatible XML */
  oyCMMuiGet_f     oyCMMuiGet;         /**< xml ui elements for filter options*/

  oyCMMGetText_f   getText;            /**< translated UI texts */
  /** zero terminated categories for getText,
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
   */
  const char    ** texts;
  oyCMMapiFilter_s*parent;             /**< link to the owner of this structure */

/* } Include "CMMui.members.h" */

};


oyCMMui_s_*
  oyCMMui_New_( oyObject_s object );
oyCMMui_s_*
  oyCMMui_Copy_( oyCMMui_s_ *cmmui, oyObject_s object);
oyCMMui_s_*
  oyCMMui_Copy__( oyCMMui_s_ *cmmui, oyObject_s object);
int
  oyCMMui_Release_( oyCMMui_s_ **cmmui );



/* Include "CMMui.private_methods_declarations.h" { */

/* } Include "CMMui.private_methods_declarations.h" */



void oyCMMui_Release__Members( oyCMMui_s_ * cmmui );
int oyCMMui_Init__Members( oyCMMui_s_ * cmmui );
int oyCMMui_Copy__Members( oyCMMui_s_ * dst, oyCMMui_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_UI_S__H */
