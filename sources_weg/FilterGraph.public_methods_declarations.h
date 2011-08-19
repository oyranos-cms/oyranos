OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_FromNode    ( oyFilterNode_s    * node,
                                       int                 flags );
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterGraph_GetNode     ( oyFilterGraph_s   * graph,
                                       int                 pos,
                                       const char        * registration,
                                       const char        * mark );
OYAPI int  OYEXPORT
           oyFilterGraph_PrepareContexts (
                                       oyFilterGraph_s   * graph,
                                       int                 flags );
OYAPI int  OYEXPORT
           oyFilterGraph_SetFromNode ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * node,
                                       const char        * mark,
                                       int                 flags );
oyBlob_s * oyFilterGraph_ToBlob      ( oyFilterGraph_s   * graph,
                                       int                 node_pos,
                                       oyObject_s          object );
OYAPI char * OYEXPORT
           oyFilterGraph_ToText      ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc );

