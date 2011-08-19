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
  oyFilterNode_s * node = 0;
  oyFilterNode_s_ ** node_ = (oyFilterNode_s_**)&node;
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;
  int i, n, m = -1, found;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < n; ++i)
  {
    node = oyFilterNodes_Get( s->nodes, i );

    found = 1;

    if(found && registration &&
       !oyFilterRegistrationMatch( (*node_)->core->api4_->registration,
                                   registration, 0 ))
      found = 0;

    if(found && mark &&
       oyOptions_FindString( (*node_)->tags, mark, 0 ) == 0 )
      found = 0;

    if(found)
    if(pos == -1 || ++m == pos)
      break;

    oyFilterNode_Release( &node );
  }

  return node;
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
  oyFilterNode_s * node = 0;
  oyFilterNode_s_ ** node_ = (oyFilterNode_s_**)&node;
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;
  int i, n, do_it;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < n; ++i)
  {
    node = oyFilterNodes_Get( s->nodes, i );

    if(flags || !(*node_)->backend_data)
      do_it = 1;
    else
      do_it = 0;

    if(do_it &&
       (*node_)->core->api4_->oyCMMFilterNode_ContextToMem &&
       strlen((*node_)->api7_->context_type))
      oyFilterNode_ContextSet_( node, 0 );

    oyFilterNode_Release( &node );
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

    oyFilterNode_AddToAdjacencyLst_( (oyFilterNode_s_*)node, s->nodes, s->edges, mark, flags );
  }

  return 0;
}

/** Function oyFilterGraph_ToBlob
 *  @memberof oyBlob_s
 *  @brief   node context to binary blob
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
  oyFilterNode_s * node = 0;
  int do_it;
  oyFilterGraph_s * s = graph;
  oyBlob_s * blob = 0;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  node = oyFilterNodes_Get( s->nodes, node_pos );

  if(node)
  {
    if(flags || !node->backend_data)
      do_it = 1;
    else
      do_it = 0;

    if(do_it &&
       node->core->api4_->oyCMMFilterNode_ContextToMem &&
       strlen(node->core->api4_->context_type))
    {
      blob = oyBlob_New( object );
      oyFilterNode_ContextSet_( node, blob );
    }

    oyFilterNode_Release( &node );
  }

  return blob;
}

/** Function oyFilterGraph_ToText
 *  @memberof oyFilterGraph_s
 *  @brief   text description of a graph
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
       * tmp = 0, * txt = 0, * t = 0;
  oyFilterNode_s * node = 0;
  char * save_locale = 0;
  oyFilterGraph_s * s = graph;

  oyFilterPlug_s * p = 0;
  int i, n,
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
    node = oyFilterNodes_Get( s->nodes, i );
    n = oyFilterNode_EdgeCount( node, 1, 0 );

    /** The function is more verbose with the oy_debug variable set. */
    if(!oy_debug &&
       oyStrchr_( node->core->api4_->id_, OY_SLASH_C ))
    {
      STRING_ADD( tmp, node->core->api4_->id_ );
      t = oyStrrchr_( tmp, OY_SLASH_C );
      *t = 0;
      STRING_ADD( txt, t+1 );
      oyFree_m_(tmp);
    } else
      STRING_ADD( txt, node->core->api4_->id_ );

    oySprintf_(temp, "  %d [ label=\"{<plug> %d| Filter Node %d\\n"
                     " Category: \\\"%s\\\"\\n CMM: \\\"%s\\\"\\n"
                     " Type: \\\"%s\\\"|<socket>}\"];\n",
                     oyFilterNode_GetId( node ), n,
                     node->oy_->id_,
                     node->core->category_,
                     txt,
                     node->core->registration_ );
    STRING_ADD( text, temp );
    oyFree_m_(txt);

    oyFilterNode_Release( &node );
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
    p = oyFilterPlugs_Get( s->edges, i );

    oySprintf_( temp,
                "    %d:socket -> %d:plug [arrowhead=crow, arrowtail=box];\n",
                oyFilterNode_GetId( p->remote_socket_->node ),
                oyFilterNode_GetId( p->node ) );
    STRING_ADD( text, temp );

    oyFilterPlug_Release( &p );
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
