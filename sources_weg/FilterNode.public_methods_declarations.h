int          oyCMMptr_ConvertData    ( oyCMMptr_s        * cmm_ptr,
                                       oyCMMptr_s        * cmm_ptr_out,
                                       oyFilterNode_s    * node );
int            oyFilterNode_Connect  ( oyFilterNode_s    * input,
                                       const char        * socket_nick,
                                       oyFilterNode_s    * output,
                                       const char        * plug_nick,
                                       int                 flags );
OYAPI int  OYEXPORT
               oyFilterNode_ConnectorMatch (
                                       oyFilterNode_s    * node_first,
                                       int                 pos_first,
                                       oyFilterPlug_s    * plug );
oyFilterNode_s *   oyFilterNode_Create(oyFilterCore_s    * filter,
                                       oyObject_s          object );
oyStruct_s *   oyFilterNode_DataGet  ( oyFilterNode_s    * node,
                                       int                 socket_pos );
int            oyFilterNode_DataSet  ( oyFilterNode_s    * node,
                                       oyStruct_s        * data,
                                       int                 socket_pos,
                                       oyObject_s        * object );
int            oyFilterNode_Disconnect(oyFilterPlug_s    * edge );
int            oyFilterNode_EdgeCount( oyFilterNode_s    * node,
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
OYAPI oyFilterSocket_s * OYEXPORT
               oyFilterNode_GetSocket( oyFilterNode_s    * node,
                                       int                 pos );
const char *   oyFilterNode_GetText  ( oyFilterNode_s    * node,
                                       oyNAME_e            name_type );
oyFilterNode_s *   oyFilterNode_NewWith (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
oyOptions_s*   oyFilterNode_OptionsGet(oyFilterNode_s    * node,
                                       int                 flags );
OYAPI oyConnector_s * OYEXPORT
               oyFilterNode_ShowConnector (
                                       oyFilterNode_s    * node,
                                       int                 as_pos,
                                       int                 plug );
int            oyFilterNode_UiGet    ( oyFilterNode_s     * node,
                                       char              ** ui_text,
                                       char             *** namespaces,
                                       oyAlloc_f            allocateFunc );
