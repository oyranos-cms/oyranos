/** @file oyFilterPlug_s_.h

   [Template file inheritance graph]
   +-> oyFilterPlug_s_.template.h
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


#ifndef OY_FILTER_PLUG_S__H
#define OY_FILTER_PLUG_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyFilterPlugPriv_m( var ) ((oyFilterPlug_s_*) (var))

typedef struct oyFilterPlug_s_ oyFilterPlug_s_;

  
#include <oyranos_object.h>


#include "oyFilterNode_s.h"
#include "oyFilterSocket_s_.h"
  

#include "oyFilterPlug_s.h"

/* Include "FilterPlug.private.h" { */

/* } Include "FilterPlug.private.h" */

/** @internal
 *  @struct   oyFilterPlug_s_
 *  @brief      A filter connection structure
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyFilterPlug_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "FilterPlug.members.h" { */
  oyFilterNode_s_    * node;           /**< filter node for this connector */
  oyFilterSocket_s_  * remote_socket_; /**< @private the remote output */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  char               * relatives_;     /**< @private hint about belonging to a filter */

/* } Include "FilterPlug.members.h" */

};


oyFilterPlug_s_*
  oyFilterPlug_New_( oyObject_s object );
oyFilterPlug_s_*
  oyFilterPlug_Copy_( oyFilterPlug_s_ *filterplug, oyObject_s object);
oyFilterPlug_s_*
  oyFilterPlug_Copy__( oyFilterPlug_s_ *filterplug, oyObject_s object);
int
  oyFilterPlug_Release_( oyFilterPlug_s_ **filterplug );



/* Include "FilterPlug.private_methods_declarations.h" { */

/* } Include "FilterPlug.private_methods_declarations.h" */



void oyFilterPlug_Release__Members( oyFilterPlug_s_ * filterplug );
int oyFilterPlug_Init__Members( oyFilterPlug_s_ * filterplug );
int oyFilterPlug_Copy__Members( oyFilterPlug_s_ * dst, oyFilterPlug_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_PLUG_S__H */
