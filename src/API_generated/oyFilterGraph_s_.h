/** @file oyFilterGraph_s_.h

   [Template file inheritance graph]
   +-> oyFilterGraph_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/10/08
 */


#ifndef OY_FILTER_GRAPH_S__H
#define OY_FILTER_GRAPH_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyFilterGraphPriv_m( var ) ((oyFilterGraph_s_*) (var))

typedef struct oyFilterGraph_s_ oyFilterGraph_s_;

  
#include <oyranos_object.h>


#include "oyFilterNodes_s.h"
#include "oyFilterPlugs_s.h"
  

#include "oyFilterGraph_s.h"

/* Include "FilterGraph.private.h" { */

/* } Include "FilterGraph.private.h" */

/** @internal
 *  @struct   oyFilterGraph_s_
 *  @brief      A FilterGraph object
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyFilterGraph_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */



/* Include "FilterGraph.members.h" { */
  oyFilterNodes_s    * nodes;          /**< the nodes in the graph */
  oyFilterPlugs_s    * edges;          /**< the edges in the graph */
  oyOptions_s        * options;        /**< options, "dirty" ... */

/* } Include "FilterGraph.members.h" */

};


oyFilterGraph_s_*
  oyFilterGraph_New_( oyObject_s object );
oyFilterGraph_s_*
  oyFilterGraph_Copy_( oyFilterGraph_s_ *filtergraph, oyObject_s object);
oyFilterGraph_s_*
  oyFilterGraph_Copy__( oyFilterGraph_s_ *filtergraph, oyObject_s object);
int
  oyFilterGraph_Release_( oyFilterGraph_s_ **filtergraph );



/* Include "FilterGraph.private_methods_declarations.h" { */

/* } Include "FilterGraph.private_methods_declarations.h" */



void oyFilterGraph_Release__Members( oyFilterGraph_s_ * filtergraph );
int oyFilterGraph_Init__Members( oyFilterGraph_s_ * filtergraph );
int oyFilterGraph_Copy__Members( oyFilterGraph_s_ * dst, oyFilterGraph_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_GRAPH_S__H */
