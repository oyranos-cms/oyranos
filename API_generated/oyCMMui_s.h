/** @file oyCMMui_s.h

   [Template file inheritance graph]
   +-> oyCMMui_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/10/08
 */



#ifndef OY_CMM_UI_S_H
#define OY_CMM_UI_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMui_s oyCMMui_s;


#include "oyStruct_s.h"


/* Include "CMMui.public.h" { */

/* } Include "CMMui.public.h" */


/* Include "CMMui.dox" { */
/** @struct  oyCMMui_s
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @brief   The CMM API UI part
 *  @internal
 *  
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/12/17
 */

/* } Include "CMMui.dox" */

struct oyCMMui_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMui_s* OYEXPORT
  oyCMMui_New( oyObject_s object );
OYAPI oyCMMui_s* OYEXPORT
  oyCMMui_Copy( oyCMMui_s *cmmui, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMui_Release( oyCMMui_s **cmmui );



/* Include "CMMui.public_methods_declarations.h" { */
#include "oyranos_module.h"
oyCMMGetText_f     oyCMMui_GetTextF  ( oyCMMui_s         * ui );
const char **      oyCMMui_GetTexts  ( oyCMMui_s         * ui );


/* } Include "CMMui.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_UI_S_H */
