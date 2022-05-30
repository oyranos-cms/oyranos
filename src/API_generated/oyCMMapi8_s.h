/** @file oyCMMapi8_s.h

   [Template file inheritance graph]
   +-> oyCMMapi8_s.template.h
   |
   +-> oyCMMapiFilter_s.template.h
   |
   +-> oyCMMapi_s.template.h
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



#ifndef OY_CMM_API8_S_H
#define OY_CMM_API8_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi8_s oyCMMapi8_s;



#include "oyStruct_s.h"

#include "oyPointer_s.h"


/* Include "CMMapi8.public.h" { */

/* } Include "CMMapi8.public.h" */


/* Include "CMMapi8.dox" { */
/** @struct  oyCMMapi8_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Configuration handler
 *  @internal
 *  
 *  Oyranos knows about configurations sets as oyConfig_s structures.
 *  These configurations can be created, modified and compared by this module 
 *  type and stored by Oyranos' core.
 *  They are stored under the base key path decided by each configuration 
 *  module individualy in its oyCMMapi8_s::registration string.
 *
 *  This API provides weak interface compile time checking.
 *
 *  The module knows what to configure. This can be devices, filters or 
 *  something else. A oyCMMapi8_s module counts the understood endities
 *  and veryfies a provided oyConfig_s for correctness. Oyranos requires just
 *  the fourth type string in the registration path starting with "config".
 *  This requirement is useful to group DB keys alphabetically.
 *
 *  In the case of a device, a application can ask Oyranos for all or a subset
 *  of available devices with oyConfigs_FromPattern.
 *
 *  A application can modify a selection of oyConfig_s objects and
 *  modify them.
 *  It is adviced to let Oyranos ask the module in advance through 
 *  oyConfig_Rank_f, if the modified oyConfig_s object is still valid.
 *
 *  @Future directions:
 *  For automatic UI's this module API should provide a XFORMS UI about the
 *  result and return a oyConfig_s for a filled form.
 *
 *  A empty device module is provided with oyranos_cmm_dDev.c in Oyranos'
 *  examples/ directory.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/12/15
  */

/* } Include "CMMapi8.dox" */

struct oyCMMapi8_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyCMMapi8_s* OYEXPORT
  oyCMMapi8_New( oyObject_s object );
OYAPI oyCMMapi8_s* OYEXPORT
  oyCMMapi8_Copy( oyCMMapi8_s *cmmapi8, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi8_Release( oyCMMapi8_s **cmmapi8 );

OYAPI int  OYEXPORT
           oyCMMapi8_SetBackendContext          ( oyCMMapi8_s       * api,
                                       oyPointer_s       * ptr );
OYAPI oyPointer_s * OYEXPORT
           oyCMMapi8_GetBackendContext          ( oyCMMapi8_s       * api );



/* Include "CMMapi8.public_methods_declarations.h" { */
#include "oyCMMui_s.h"

oyCMMui_s *        oyCMMapi8_GetUi   ( oyCMMapi8_s       * api );

/* } Include "CMMapi8.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API8_S_H */
