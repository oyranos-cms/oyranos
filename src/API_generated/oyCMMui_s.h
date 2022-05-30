/** @file oyCMMui_s.h

   [Template file inheritance graph]
   +-> oyCMMui_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
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
#include "oyOptions_s.h"
#include "oyCMMapiFilter_s.h"
/** typedef  oyCMMuiGet_f
 *  @brief   obtain a XFORMS ui description
 *  @ingroup module_api
 *  @memberof oyCMMui_s
 *
 *  The structures can provide a XFORMS ui based on the modules own
 *  set of options. The options are in the property of the caller.
 *
 *  @param[in]     module              the owner
 *  @param[in]     options             the options to display
 *  @param[in]     flags               modificators
 *                                     - oyNAME_JSON : order JSON flavour
 *  @param[out]    ui_text             the XFORMS string
 *  @param[in]     allocateFunc        user allocator
 *  @return                            0 on success; error >= 1; -1 not understood; unknown < -1
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/01/17
 *  @since   2009/01/18 (Oyranos: 0.1.10)
 */
typedef int  (*oyCMMuiGet_f)         ( oyCMMapiFilter_s   * module,
                                       oyOptions_s        * options,
                                       int                  flags,
                                       char              ** ui_text,
                                       oyAlloc_f            allocateFunc );


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
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

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
#include "oyCMMapiFilter_s.h"
OYAPI oyCMMui_s *  OYEXPORT
                   oyCMMui_Create    ( const char        * category,
                                       oyCMMGetText_f      getText,
                                       const char       ** texts,
                                       oyObject_s          object );
OYAPI void  OYEXPORT
                   oyCMMui_SetUiOptions(
                                       oyCMMui_s         * ui,
                                       const char        * options,
                                       oyCMMuiGet_f        oyCMMuiGet );
oyCMMGetText_f     oyCMMui_GetTextF  ( oyCMMui_s         * ui );
const char **      oyCMMui_GetTexts  ( oyCMMui_s         * ui );
oyCMMapiFilter_s * oyCMMui_GetParent ( oyCMMui_s         * ui );

/* } Include "CMMui.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_UI_S_H */
