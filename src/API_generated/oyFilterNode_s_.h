/** @file oyFilterNode_s_.h

   [Template file inheritance graph]
   +-> oyFilterNode_s_.template.h
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


#ifndef OY_FILTER_NODE_S__H
#define OY_FILTER_NODE_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyFilterNodePriv_m( var ) ((oyFilterNode_s_*) (var))

typedef struct oyFilterNode_s_ oyFilterNode_s_;

  
#include <oyranos_object.h>


#include "oyCMMapi7_s_.h"
#include "oyBlob_s_.h"
#include "oyFilterNodes_s_.h"
#include "oyFilterPlugs_s_.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterCore_s_.h"
#include "oyFilterSocket_s.h"
#include "oyObserver_s_.h"
  

#include "oyFilterNode_s.h"

/* Include "FilterNode.private.h" { */

/* } Include "FilterNode.private.h" */

/** @internal
 *  @struct   oyFilterNode_s_
 *  @brief      A FilterNode object
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyFilterNode_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "FilterNode.members.h" { */
  oyFilterPlug_s_   ** plugs;          /**< possible input connectors */
  int                  plugs_n_;       /**< readonly number of inputs */
  oyFilterSocket_s_ ** sockets;        /**< possible output connectors */
  int                  sockets_n_;     /**< readonly number of outputs */

  oyFilterCore_s_    * core;           /**< the filter core */
  char               * relatives_;     /**< @private hint about belonging to a filter */
  oyOptions_s        * tags;           /**< infos, e.g. group markers */

  /** the filters private data, requested over 
   *  oyCMMapi4_s::oyCMMFilterNode_ContextToMem() and converted to
   *  oyCMMapi4_s::context_type */
  oyPointer_s        * backend_data;
  /** the processing function and node connector descriptions */
  oyCMMapi7_s_       * api7_;

/* } Include "FilterNode.members.h" */

};


oyFilterNode_s_*
  oyFilterNode_New_( oyObject_s object );
oyFilterNode_s_*
  oyFilterNode_Copy_( oyFilterNode_s_ *filternode, oyObject_s object);
oyFilterNode_s_*
  oyFilterNode_Copy__( oyFilterNode_s_ *filternode, oyObject_s object);
int
  oyFilterNode_Release_( oyFilterNode_s_ **filternode );



/* Include "FilterNode.private_methods_declarations.h" { */
int  oyFilterNode_AddToAdjacencyLst_ ( oyFilterNode_s_    * s,
                                       oyFilterNodes_s_   * nodes,
                                       oyFilterPlugs_s_   * edges,
                                       const char         * mark,
                                       int                  flags );
int          oyFilterNode_SetContext_( oyFilterNode_s_    * node,
                                       oyBlob_s_          * blob );
oyStructList_s * oyFilterNode_GetData_(oyFilterNode_s_    * node,
                                       int                  get_plug );
oyFilterNode_s *   oyFilterNode_GetLastFromLinear_ (
                                       oyFilterNode_s_    * first );
oyFilterNode_s *   oyFilterNode_GetNextFromLinear_ (
                                       oyFilterNode_s_    * first );
oyPointer    oyFilterNode_TextToInfo_( oyFilterNode_s_    * node,
                                       size_t             * size,
                                       oyAlloc_f            allocateFunc );
int      oyFilterNode_Observe_       ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );
oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s_   * filter,
                                       oyFilterNode_s_   * node,
                                       uint32_t            flags,
                                       oyObject_s          object );

/* } Include "FilterNode.private_methods_declarations.h" */



void oyFilterNode_Release__Members( oyFilterNode_s_ * filternode );
int oyFilterNode_Init__Members( oyFilterNode_s_ * filternode );
int oyFilterNode_Copy__Members( oyFilterNode_s_ * dst, oyFilterNode_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_NODE_S__H */
