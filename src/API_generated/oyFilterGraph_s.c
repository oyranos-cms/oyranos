/** @file oyFilterGraph_s.c

   [Template file inheritance graph]
   +-> oyFilterGraph_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/12/13
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

/** Function oyFilterGraph_Copy
 *  @memberof oyFilterGraph_s
 *  @brief   copy or reference a FilterGraph object
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
    oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  s = oyFilterGraph_Copy_( s, object );

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
 *  @memberof oyFilterNode_s
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
  o = oyOptions_Find( s->options, "dirty" );
  oyOption_SetFromText( o, "false", 0 );

  return 0;
}

/** Function  oyFilterGraph_SetFromNode
 *  @memberof oyFilterNode_s
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

/** Function  oyFilterGraph_ToBlob
 *  @memberof oyBlob_s
 *  @brief    Node context to binary blob
 *
 *  Typical a context from a CMM will be returned.
 *
 *  @param         graph               graph object
 *  @param         node_pos            node position in the graph
 *  @param         object              the optional object
 *  @return                            the data blob
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/12 (Oyranos: 0.1.10)
 *  @date    2009/06/12
 */
oyBlob_s * oyFilterGraph_ToBlob      ( oyFilterGraph_s   * graph,
                                       int                 node_pos,
                                       oyObject_s          object )
{
  int flags = 1;
  oyFilterNode_s_ * node = 0;
  int do_it;
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;
  oyBlob_s * blob = 0;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  node = (oyFilterNode_s_*) oyFilterNodes_Get( s->nodes, node_pos );

  if(node)
  {
    if(flags || !node->backend_data)
      do_it = 1;
    else
      do_it = 0;

    if(do_it &&
       ((oyCMMapi4_s_*)node->core->api4_)->oyCMMFilterNode_ContextToMem &&
       ((oyCMMapi4_s_*)node->core->api4_)->oyCMMFilterNode_ContextToMem &&
       strlen(((oyCMMapi4_s_*)node->core->api4_)->context_type))
    {
      blob = oyBlob_New( object );
      oyFilterNode_SetContext_( node, (oyBlob_s_*)blob );
    }

    oyFilterNode_Release( (oyFilterNode_s**)&node );
  }

  return blob;
}

/** Function  oyFilterGraph_ToText
 *  @memberof oyFilterGraph_s
 *  @brief    Text description of a graph
 *
 *  @todo Should this function generate XFORMS compatible output? How?
 *
 *  @param[in]     graph               graphy object
 *  @param[in]     input               start node of a oyConversion_s
 *  @param[in]     output              end node and if present a switch
 *                                     to interprete input and output as start
 *                                     and end node of a oyConversion_s
 *  @param[in]     head_line           text for inclusion
 *  @param[in]     reserved            future format selector (dot, xml ...)
 *  @param[in]     allocateFunc        allocation function
 *  @return                            the graph description
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/10/04 (Oyranos: 0.1.8)
 *  @date    2009/03/05
 */
OYAPI char * OYEXPORT
           oyFilterGraph_ToText      ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc )
{
  char * text = 0,
       * temp = oyAllocateFunc_(1024),
       * temp2 = oyAllocateFunc_(1024),
       * tmp = 0, * txt = 0, * t = 0;
  oyFilterNode_s * node = 0;
  char * save_locale = 0;
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;

  oyFilterPlug_s_ * p = 0;
  int i, j, n, len,
      nodes_n = 0;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  save_locale = oyStringCopy_( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_);


  STRING_ADD( text, "digraph G {\n" );
  STRING_ADD( text, "bgcolor=\"transparent\"\n" );
  STRING_ADD( text, "  rankdir=LR\n" );
  STRING_ADD( text, "  graph [fontname=Helvetica, fontsize=12];\n" );
  STRING_ADD( text, "  node [shape=record, fontname=Helvetica, fontsize=10, style=\"filled,rounded\"];\n" );
  STRING_ADD( text, "  edge [fontname=Helvetica, fontsize=10];\n" );
  STRING_ADD( text, "\n" );
  if(input && output)
  {
  STRING_ADD( text, "  conversion [shape=plaintext, label=<\n" );
  STRING_ADD( text, "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n" );
  STRING_ADD( text, "  <tr><td>oyConversion_s</td></tr>\n" );
  STRING_ADD( text, "  <tr><td>\n" );
  STRING_ADD( text, "     <table border=\"0\" cellborder=\"0\" align=\"left\">\n" );
  STRING_ADD( text, "       <tr><td align=\"left\">...</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\" port=\"in\">+input</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\" port=\"out\">+out_</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\">...</td></tr>\n" );
  STRING_ADD( text, "     </table>\n" );
  STRING_ADD( text, "     </td></tr>\n" );
  STRING_ADD( text, "  <tr><td> </td></tr>\n" );
  STRING_ADD( text, "</table>>,\n" );
  STRING_ADD( text, "                    style=\"\", color=black];\n" );
  }
  STRING_ADD( text, "\n" );

  /* add more node descriptions */
  nodes_n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < nodes_n; ++i)
  {
    oyFilterCore_s_ * node_core;
    node = oyFilterNodes_Get( s->nodes, i );
    n = oyFilterNode_EdgeCount( node, 1, 0 );
    node_core = (oyFilterCore_s_*)oyFilterNode_GetCore( node );

    /** The function is more verbose with the oy_debug variable set. */
    if(!oy_debug &&
       oyStrchr_( ((oyCMMapi4_s_*)node_core->api4_)->id_, OY_SLASH_C ))
    {
      STRING_ADD( tmp, ((oyCMMapi4_s_*)node_core->api4_)->id_ );
      t = oyStrrchr_( tmp, OY_SLASH_C );
      *t = 0;
      STRING_ADD( txt, t+1 );
      oyFree_m_(tmp);
    } else
    {
      STRING_ADD( txt, ((oyCMMapi4_s_*)node_core->api4_)->id_ );
      if(oy_debug)
      {
        int node_sockets_n = oyFilterNode_EdgeCount( node, 0,
                                                     OY_FILTEREDGE_CONNECTED );
      for(j = 0; j < node_sockets_n; ++j)
      {
        oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, j );
        oyStruct_s * socket_data = oyFilterSocket_GetData( socket );
        if(socket && socket_data)
        {
          const char * name = oyObject_GetName( socket_data->oy_, 1 );
          int k, pos = 0;
          len = strlen(name);
          for(k = 0; k < len; ++k)
            if(k && name[k] == '"' && name[k-1] != '\\')
            {
              sprintf( &temp2[pos], "\\\"" );
              pos += 2;
            } else if(name[k] == '<')
            {
              sprintf( &temp2[pos], "\\<" );
              pos += 2;
            } else if(name[k] == '>')
            {
              sprintf( &temp2[pos], "\\>" );
              pos += 2;
            } else if(name[k] == '[')
            {
              sprintf( &temp2[pos], "\\[" );
              pos += 2;
            } else if(name[k] == ']')
            {
              sprintf( &temp2[pos], "\\]" );
              pos += 2;
            } else if(name[k] == '\n')
            {
              sprintf( &temp2[pos], "\\n" );
              pos += 2;
            } else
              temp2[pos++] = name[k];
          temp2[pos] = 0;
          printf("%s\n", name);
          printf("%s\n", temp2);
          oySprintf_(temp, "  %d [ label=\"{<data> | Data %d\\n"
                     " Type: \\\"%s\\\"\\n"
                     " XML: \\\"%s\\\"|<socket>}\"];\n",
                     oyObject_GetId( socket->oy_ ),
                     j,
                     oyStructTypeToText( socket_data->type_ ),
                     temp2);
          printf("%s\n", temp);
          STRING_ADD( text, temp );
        }
        oyFilterSocket_Release( &socket );
      }
      }
    }

    oySprintf_(temp, "  %d [ label=\"{<plug> %d| Filter Node %d\\n"
                     " Category: \\\"%s\\\"\\n CMM: \\\"%s\\\"\\n"
                     " Type: \\\"%s\\\"|<socket>}\"];\n",
                     oyFilterNode_GetId( node ), n,
                     node->oy_->id_,
                     oyFilterCore_GetCategory( (oyFilterCore_s*)node_core, 1 ),
                     txt,
                     oyFilterNode_GetRegistration( node ));
    STRING_ADD( text, temp );
    oyFree_m_(txt);

    oyFilterNode_Release( &node );
    oyFilterCore_Release( (oyFilterCore_s**)&node_core );
  }


  STRING_ADD( text, "\n" );
  STRING_ADD( text, "  subgraph cluster_0 {\n" );
  STRING_ADD( text, "    label=\"" );
  STRING_ADD( text, head_line );
  STRING_ADD( text, "\"\n" );
  STRING_ADD( text, "    color=gray;\n" );
  STRING_ADD( text, "\n" );

  /* add more node placements */
  n = oyFilterPlugs_Count( s->edges );
  for(i = 0; i < n; ++i)
  {
    p = (oyFilterPlug_s_*)oyFilterPlugs_Get( s->edges, i );

    oySprintf_( temp,
                "    %d:socket -> %d:plug [arrowhead=crow, arrowtail=box];\n",
                oyFilterNode_GetId( (oyFilterNode_s*)p->remote_socket_->node ),
                oyFilterNode_GetId( (oyFilterNode_s*)p->node ) );
    STRING_ADD( text, temp );

    oyFilterPlug_Release( (oyFilterPlug_s**)&p );
  }
  
  for(i = 0; i < nodes_n; ++i)
  {
    node = oyFilterNodes_Get( s->nodes, i );
    if(oy_debug)
    {
      int node_sockets_n = oyFilterNode_EdgeCount( node, 0,
                                                   OY_FILTEREDGE_CONNECTED );
      for(j = 0; j < node_sockets_n; ++j)
      {
        oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, j );
        oyStruct_s * socket_data = oyFilterSocket_GetData( socket );
        if(socket && socket_data)
        {
          oySprintf_( temp,
               "    %d:socket -> %d:data [arrowhead=crow, arrowtail=box];\n",
                oyFilterNode_GetId( node ),
                oyObject_GetId( socket->oy_ ));
          STRING_ADD( text, temp );
          if(socket_data->release)
            socket_data->release( &socket_data );
        }
        oyFilterSocket_Release( &socket );
          
      }
    }
    oyFilterNode_Release( &node );
  }

  STRING_ADD( text, "\n" );
  if(input && output)
  {
  oySprintf_( temp, "    conversion:in -> %d [arrowhead=none, arrowtail=normal];\n", oyFilterNode_GetId( input ) );
  STRING_ADD( text, temp );
  oySprintf_( temp, "    conversion:out -> %d;\n",
                    oyFilterNode_GetId( output ) );
  STRING_ADD( text, temp );
  }
  STRING_ADD( text, "  }\n" );
  STRING_ADD( text, "\n" );
  if(input && output)
  STRING_ADD( text, "  conversion\n" );
  STRING_ADD( text, "}\n" );
  STRING_ADD( text, "\n" );

  setlocale(LC_NUMERIC, "C");
  /* sensible printing */
  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );

  STRING_ADD( text, "" );

  oyFree_m_( temp );

  return text;
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

  return oyOptions_Copy( s->options, 0 );
}

/* } Include "FilterGraph.public_methods_definitions.c" */

