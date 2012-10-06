OYAPI int  OYEXPORT
                 oyFilterSocket_Callback (
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI int  OYEXPORT
                 oyFilterSocket_SignalToGraph (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterSocket_GetNode  ( oyFilterSocket_s  * c );
OYAPI oyStruct_s * OYEXPORT
             oyFilterSocket_GetData  ( oyFilterSocket_s  * c );
OYAPI int OYEXPORT
             oyFilterSocket_SetData  ( oyFilterSocket_s  * socket,
                                       oyStruct_s        * data );
OYAPI oyFilterPlugs_s * OYEXPORT
             oyFilterSocket_GetPlugs ( oyFilterSocket_s  * c );
OYAPI oyConnector_s * OYEXPORT
             oyFilterSocket_GetPattern(oyFilterSocket_s  * c );
OYAPI const char * OYEXPORT
             oyFilterSocket_GetRelatives (
                                       oyFilterSocket_s  * c );
OYAPI int  OYEXPORT
             oyFilterSocket_MatchImagingPlug (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug );
