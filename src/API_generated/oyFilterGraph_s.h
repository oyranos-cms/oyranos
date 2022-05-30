/** @file oyFilterGraph_s.h

   [Template file inheritance graph]
   +-> oyFilterGraph_s.template.h
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



#ifndef OY_FILTER_GRAPH_S_H
#define OY_FILTER_GRAPH_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyFilterGraph_s oyFilterGraph_s;



#include "oyStruct_s.h"

#include "oyFilterNode_s.h"


/* Include "FilterGraph.public.h" { */

/* } Include "FilterGraph.public.h" */


/* Include "FilterGraph.dox" { */
/** @struct  oyFilterGraph_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   A FilterGraph object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */

/* } Include "FilterGraph.dox" */

struct oyFilterGraph_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyFilterGraph_s* OYEXPORT
  oyFilterGraph_New( oyObject_s object );
OYAPI oyFilterGraph_s* OYEXPORT
  oyFilterGraph_Copy( oyFilterGraph_s *filtergraph, oyObject_s obj );
OYAPI int OYEXPORT
  oyFilterGraph_Release( oyFilterGraph_s **filtergraph );



/* Include "FilterGraph.public_methods_declarations.h" { */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_FromNode    ( oyFilterNode_s    * node,
                                       int                 flags );
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterGraph_GetNode     ( oyFilterGraph_s   * graph,
                                       int                 pos,
                                       const char        * registration,
                                       const char        * mark );
OYAPI int OYEXPORT
           oyFilterGraph_CountNodes  ( oyFilterGraph_s   * graph,
                                       const char        * registration,
                                       const char        * mark );
OYAPI int OYEXPORT
           oyFilterGraph_CountEdges  ( oyFilterGraph_s   * graph );
OYAPI oyFilterPlug_s * OYEXPORT
           oyFilterGraph_GetEdge     ( oyFilterGraph_s   * graph,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyFilterGraph_PrepareContexts (
                                       oyFilterGraph_s   * graph,
                                       int                 flags );
OYAPI int  OYEXPORT
           oyFilterGraph_SetFromNode ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * node,
                                       const char        * mark,
                                       int                 flags );
OYAPI char * OYEXPORT
           oyFilterGraph_ToText      ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc );
OYAPI oyOptions_s * OYEXPORT
           oyFilterGraph_GetOptions  ( oyFilterGraph_s   * graph );

/* } Include "FilterGraph.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_GRAPH_S_H */
