OYAPI int  OYEXPORT
                 oyFilterNode_Connect (oyFilterNode_s    * input,
                                       const char        * socket_nick,
                                       oyFilterNode_s    * output,
                                       const char        * plug_nick,
                                       int                 flags );
OYAPI int  OYEXPORT
               oyFilterNode_ConnectorMatch (
                                       oyFilterNode_s    * node_first,
                                       int                 pos_first,
                                       oyFilterPlug_s    * plug );
OYAPI oyFilterNode_s *  OYEXPORT
                oyFilterNode_Create  ( const char        * registration,
                                       oyFilterCore_s    * filter,
                                       oyObject_s          object );
OYAPI oyStruct_s *  OYEXPORT
                oyFilterNode_GetData ( oyFilterNode_s    * node,
                                       int                 socket_pos );
OYAPI int  OYEXPORT
                 oyFilterNode_SetData( oyFilterNode_s    * node,
                                       oyStruct_s        * data,
                                       int                 socket_pos,
                                       oyObject_s        * object );
OYAPI int  OYEXPORT
                 oyFilterNode_Disconnect (
                                       oyFilterNode_s    * node,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilterNode_EdgeCount (
                                       oyFilterNode_s    * node,
                                       int                 input,
                                       int                 flags );
OYAPI int  OYEXPORT
               oyFilterNode_GetConnectorPos (
                                       oyFilterNode_s    * node,
                                       int                 is_input,
                                       const char        * pattern,
                                       int                 nth_of_type,
                                       int                 flags );
OYAPI int  OYEXPORT
               oyFilterNode_GetId    ( oyFilterNode_s    * node );
OYAPI oyFilterPlug_s * OYEXPORT
               oyFilterNode_GetPlug  ( oyFilterNode_s    * node,
                                       int                 pos );
OYAPI int  OYEXPORT
                oyFilterNode_Run     ( oyFilterNode_s    * node,
                                       oyFilterPlug_s *    plug,
                                       oyPixelAccess_s *   ticket );
OYAPI oyFilterSocket_s * OYEXPORT
               oyFilterNode_GetSocket( oyFilterNode_s    * node,
                                       int                 pos );
OYAPI oyFilterNode_s * OYEXPORT
               oyFilterNode_GetSocketNode (
                                       oyFilterNode_s    * node,
                                       int                 pos,
                                       int                 plugs_pos );
OYAPI int OYEXPORT
               oyFilterNode_CountSocketNodes (
                                       oyFilterNode_s    * node,
                                       int                 pos,
                                       int                 flags );
OYAPI oyFilterNode_s * OYEXPORT
               oyFilterNode_GetPlugNode ( 
                                       oyFilterNode_s    * node,
                                       int                 pos );
OYAPI const char *  OYEXPORT
                oyFilterNode_GetText  ( oyFilterNode_s    * node,
                                       oyNAME_e            name_type );
OYAPI oyFilterNode_s *  OYEXPORT
                oyFilterNode_NewWith (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
OYAPI oyOptions_s *  OYEXPORT
                oyFilterNode_GetOptions (
                                       oyFilterNode_s    * node,
                                       int                 flags );
OYAPI oyConnector_s * OYEXPORT
               oyFilterNode_ShowConnector (
                                       oyFilterNode_s    * node,
                                       int                 as_pos,
                                       int                 plug );
OYAPI int  OYEXPORT
                 oyFilterNode_GetUi  ( oyFilterNode_s     * node,
                                       char              ** ui_text,
                                       char             *** namespaces,
                                       oyAlloc_f            allocateFunc );
OYAPI oyOptions_s *  OYEXPORT
                 oyFilterNode_GetTags( oyFilterNode_s     * node );
OYAPI oyFilterCore_s *  OYEXPORT
                 oyFilterNode_GetCore( oyFilterNode_s     * node );
OYAPI const char *  OYEXPORT
                 oyFilterNode_GetRegistration
                                     ( oyFilterNode_s     * node );
OYAPI const char *  OYEXPORT
                 oyFilterNode_GetRelatives
                                     ( oyFilterNode_s     * node );
OYAPI const char *  OYEXPORT
                 oyFilterNode_GetModuleName
                                     ( oyFilterNode_s     * node );
OYAPI oyPointer_s *  OYEXPORT
                 oyFilterNode_GetContext(
                                       oyFilterNode_s     * node );
OYAPI int  OYEXPORT
                 oyFilterNode_SetContext(
                                       oyFilterNode_s     * node,
                                       oyPointer_s        * data );
OYAPI oyBlob_s * OYEXPORT
                 oyFilterNode_ToBlob ( oyFilterNode_s    * node,
                                       oyObject_s          object );
OYAPI oyPointer_s *  OYEXPORT
                 oyFilterNode_GetBackendContext(
                                       oyFilterNode_s     * node );
OYAPI int  OYEXPORT
                 oyFilterNode_SetBackendContext(
                                       oyFilterNode_s     * node,
                                       oyPointer_s        * data );
