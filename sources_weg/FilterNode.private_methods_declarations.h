int  oyFilterNode_AddToAdjacencyLst_ ( oyFilterNode_s    * s,
                                       oyFilterNodes_s   * nodes,
                                       oyFilterPlugs_s   * edges,
                                       const char        * mark,
                                       int                 flags );
int          oyFilterNode_ContextSet_( oyFilterNode_s    * node,
                                       oyBlob_s          * blob );
oyStructList_s * oyFilterNode_DataGet_(oyFilterNode_s    * node,
                                       int                 get_plug );
oyFilterNode_s *   oyFilterNode_GetLastFromLinear_ (
                                       oyFilterNode_s    * first );
oyFilterNode_s *   oyFilterNode_GetNextFromLinear_ (
                                       oyFilterNode_s    * first );
oyPointer    oyFilterNode_TextToInfo_( oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
