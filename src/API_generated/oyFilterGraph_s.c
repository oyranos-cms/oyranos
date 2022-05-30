/** @file oyFilterGraph_s.c

   [Template file inheritance graph]
   +-> oyFilterGraph_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyFilterGraph_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyFilterGraph_s_.h"

#include "oyBlob_s_.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyFilterNode_s_.h"
#include "oyFilterPlug_s_.h"
  


/** Function oyFilterGraph_New
 *  @memberof oyFilterGraph_s
 *  @brief   allocate a new FilterGraph object
 */
OYAPI oyFilterGraph_s * OYEXPORT
  oyFilterGraph_New( oyObject_s object )
{
  oyObject_s s = object;
  oyFilterGraph_s_ * filtergraph = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  filtergraph = oyFilterGraph_New_( s );

  return (oyFilterGraph_s*) filtergraph;
}

/** Function  oyFilterGraph_Copy
 *  @memberof oyFilterGraph_s
 *  @brief    Copy or Reference a FilterGraph object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     filtergraph                 FilterGraph struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyFilterGraph_s* OYEXPORT
  oyFilterGraph_Copy( oyFilterGraph_s *filtergraph, oyObject_s object )
{
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*) filtergraph;

  if(s)
  {
    oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return NULL )
  }
  else
    return NULL;

  s = oyFilterGraph_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyFilterGraph_s" );

  return (oyFilterGraph_s*) s;
}
 
/** Function oyFilterGraph_Release
 *  @memberof oyFilterGraph_s
 *  @brief   release and possibly deallocate a oyFilterGraph_s object
 *
 *  @param[in,out] filtergraph                 FilterGraph struct object
 */
OYAPI int OYEXPORT
  oyFilterGraph_Release( oyFilterGraph_s **filtergraph )
{
  oyFilterGraph_s_ * s = 0;

  if(!filtergraph || !*filtergraph)
    return 0;

  s = (oyFilterGraph_s_*) *filtergraph;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  *filtergraph = 0;

  return oyFilterGraph_Release_( &s );
}



/* Include "FilterGraph.public_methods_definitions.c" { */
/** Function  oyFilterGraph_FromNode
 *  @memberof oyFilterGraph_s
 *  @brief    Get a graphs adjazency list
 *
 *  @param[in]     node                filter node
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *  @return                            the graph
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_FromNode    ( oyFilterNode_s    * node,
                                       int                 flags )
{
  oyFilterNode_s * s = node;
  oyFilterGraph_s * graph = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  {
    graph = oyFilterGraph_New( 0 );
    oyFilterGraph_SetFromNode( graph, s, 0, flags );
  }

  return graph;
}

/** Function  oyFilterGraph_GetNode
 *  @memberof oyFilterGraph_s
 *  @brief    Select a node
 *
 *  @param[in]     graph               a filter graph
 *  @param[in]     pos                 the position in a matching list,
                                       or -1 to select the first match
 *  @param[in]     registration        criterium to generate the matching list,
                                       or zero for no criterium
 *  @param[in]     mark                marking string to use as a selector
 *  @return                            the filter node
 *
 *  @see @ref registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/01
 */
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterGraph_GetNode     ( oyFilterGraph_s   * graph,
                                       int                 pos,
                                       const char        * registration,
                                       const char        * mark )
{
  oyFilterNode_s_ * node = 0;
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;
  int i, n, m = -1, found;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < n; ++i)
  {
    node = (oyFilterNode_s_*) oyFilterNodes_Get( s->nodes, i );

    found = 1;

    if(found && registration &&
       !oyFilterRegistrationMatch( ((oyCMMapi4_s_*)((oyFilterCore_s_*)node->core)->api4_)->registration,
                                   registration, 0 ))
      found = 0;

    if(found && mark &&
       oyOptions_FindString( node->tags, mark, 0 ) == 0 )
      found = 0;

    if(found)
    if(pos == -1 || ++m == pos)
      break;

    oyFilterNode_Release( (oyFilterNode_s**)&node );
  }

  return (oyFilterNode_s*) node;
}

/** Function  oyFilterGraph_CountNodes
 *  @memberof oyFilterGraph_s
 *  @brief    Count nodes
 *
 *  @param[in]     graph               a filter graph
 *  @param[in]     registration        criterium to generate the matching list,
                                       or zero for no criterium
 *  @param[in]     mark                marking string to use as a selector
 *  @return                            the nodes count
 *
 *  @see @ref registration
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/24
 *  @since    2012/09/24 (Oyranos: 0.5.0)
 */
OYAPI int OYEXPORT
           oyFilterGraph_CountNodes  ( oyFilterGraph_s   * graph,
                                       const char        * registration,
                                       const char        * mark )
{
  oyFilterNode_s_ * node = 0;
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;
  int i, n, m = 0, found;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < n; ++i)
  {
    node = (oyFilterNode_s_*) oyFilterNodes_Get( s->nodes, i );

    found = 1;

    if(found && registration &&
       !oyFilterRegistrationMatch( ((oyCMMapi4_s_*)((oyFilterCore_s_*)node->core)->api4_)->registration,
                                   registration, 0 ))
      found = 0;

    if(found && mark &&
       oyOptions_FindString( node->tags, mark, 0 ) == 0 )
      found = 0;

    if(found)
      ++m;

    oyFilterNode_Release( (oyFilterNode_s**)&node );
  }

  return m;
}

/** Function  oyFilterGraph_GetEdge
 *  @memberof oyFilterGraph_s
 *  @brief    Get edges
 *
 *  @param[in]     graph               a filter graph
 *  @param[in]     pos                 the desired index
 *  @return                            the edges count
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/24
 *  @since    2012/09/24 (Oyranos: 0.5.0)
 */
OYAPI oyFilterPlug_s * OYEXPORT
           oyFilterGraph_GetEdge     ( oyFilterGraph_s   * graph,
                                       int                 pos )
{
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  return oyFilterPlugs_Get( s->edges, pos );
}

/** Function  oyFilterGraph_CountEdges
 *  @memberof oyFilterGraph_s
 *  @brief    Count edges
 *
 *  @param[in]     graph               a filter graph
 *  @return                            the edges count
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/24
 *  @since    2012/09/24 (Oyranos: 0.5.0)
 */
OYAPI int OYEXPORT
           oyFilterGraph_CountEdges  ( oyFilterGraph_s   * graph )
{
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  return oyFilterPlugs_Count( s->edges );
}

/** Function  oyFilterGraph_PrepareContexts
 *  @memberof oyFilterGraph_s
 *  @brief    Iterate over a filter graph and possibly prepare contexts
 *
 *  @param[in,out] graph               a filter graph
 *  @param[in]     flags               1 - enforce a context preparation
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/03/01
 */
OYAPI int  OYEXPORT
           oyFilterGraph_PrepareContexts (
                                       oyFilterGraph_s   * graph,
                                       int                 flags )
{
  oyOption_s * o = 0;
  oyFilterNode_s_ * node = 0;
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;
  int i, n, do_it;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < n; ++i)
  {
    node = (oyFilterNode_s_*) oyFilterNodes_Get( s->nodes, i );

    if(flags || !node->backend_data)
      do_it = 1;
    else
      do_it = 0;

    if(do_it &&
       ((oyCMMapi4_s_*)node->core->api4_)->oyCMMFilterNode_ContextToMem &&
       strlen(((oyCMMapi7_s_*)node->api7_)->context_type))
      oyFilterNode_SetContext_( node, 0 );

    oyFilterNode_Release( (oyFilterNode_s**)&node );
  }

  /* clean the graph */
  o = oyOptions_Find( s->options, "dirty", oyNAME_PATTERN );
  oyOption_SetFromString( o, "false", 0 );
  oyOption_Release( &o );

  return 0;
}

/** Function  oyFilterGraph_SetFromNode
 *  @memberof oyFilterGraph_s
 *  @brief    Get a graphs adjazency list
 *
 *  @param[in]     graph               a graph object
 *  @param[in]     node                filter node
 *  @param[in]     mark                a selection
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *  @return                            the graph
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
OYAPI int  OYEXPORT
           oyFilterGraph_SetFromNode ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * node,
                                       const char        * mark,
                                       int                 flags )
{
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  oyFilterNodes_Release( &s->nodes );
  oyFilterPlugs_Release( &s->edges );

  {
    s->nodes = oyFilterNodes_New( 0 );
    s->edges = oyFilterPlugs_New( 0 );

    oyFilterNode_AddToAdjacencyLst_( (oyFilterNode_s_*)node,
                                     (oyFilterNodes_s_*)s->nodes,
                                     (oyFilterPlugs_s_ *)s->edges,
                                     mark, flags );
  }

  return 0;
}

/** Function  oyFilterGraph_GetOptions
 *  @memberof oyFilterGraph_s
 *  @brief    Get options
 *
 *  @param[in]     graph               a filter graph
 *  @return                            the options
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/25
 *  @since    2012/09/25 (Oyranos: 0.5.0)
 */
OYAPI oyOptions_s * OYEXPORT
           oyFilterGraph_GetOptions  ( oyFilterGraph_s   * graph )
{
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  oyOptions_Copy( s->options, 0 );
  return s->options;
}

/* } Include "FilterGraph.public_methods_definitions.c" */

