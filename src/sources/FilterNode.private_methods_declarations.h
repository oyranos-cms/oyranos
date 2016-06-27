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
