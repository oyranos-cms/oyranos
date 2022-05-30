/** @file oyFilterSocket_s_.h

   [Template file inheritance graph]
   +-> oyFilterSocket_s_.template.h
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


#ifndef OY_FILTER_SOCKET_S__H
#define OY_FILTER_SOCKET_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyFilterSocketPriv_m( var ) ((oyFilterSocket_s_*) (var))

typedef struct oyFilterSocket_s_ oyFilterSocket_s_;

  
#include <oyranos_object.h>


#include "oyFilterGraph_s_.h"
#include "oyFilterPlugs_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterNode_s_.h"
  

#include "oyFilterSocket_s.h"

/* Include "FilterSocket.private.h" { */

/* } Include "FilterSocket.private.h" */

/** @internal
 *  @struct   oyFilterSocket_s_
 *  @brief      A filter connection structure
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyFilterSocket_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "FilterSocket.members.h" { */
  oyFilterNode_s_    * node;           /**< filter node for this connector */
  oyFilterPlugs_s    * requesting_plugs_;/**< @private all remote inputs */
  oyStruct_s         * data;           /**< unprocessed data model */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  char               * relatives_;     /**< @private hint about belonging to a filter */

/* } Include "FilterSocket.members.h" */

};


oyFilterSocket_s_*
  oyFilterSocket_New_( oyObject_s object );
oyFilterSocket_s_*
  oyFilterSocket_Copy_( oyFilterSocket_s_ *filtersocket, oyObject_s object);
oyFilterSocket_s_*
  oyFilterSocket_Copy__( oyFilterSocket_s_ *filtersocket, oyObject_s object);
int
  oyFilterSocket_Release_( oyFilterSocket_s_ **filtersocket );



/* Include "FilterSocket.private_methods_declarations.h" { */

/* } Include "FilterSocket.private_methods_declarations.h" */



void oyFilterSocket_Release__Members( oyFilterSocket_s_ * filtersocket );
int oyFilterSocket_Init__Members( oyFilterSocket_s_ * filtersocket );
int oyFilterSocket_Copy__Members( oyFilterSocket_s_ * dst, oyFilterSocket_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_SOCKET_S__H */
