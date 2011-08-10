OYAPI int  OYEXPORT
                 oyConnector_SetName ( oyConnector_s     * obj,
                                       const char        * string,
                                       oyNAME_e            type );
OYAPI const char *  OYEXPORT
                oyConnector_GetName ( oyConnector_s     * obj,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                 oyConnector_IsPlug  ( oyConnector_s     * obj );
OYAPI int  OYEXPORT
                 oyConnector_SetIsPlug(oyConnector_s     * obj,
                                       int                 is_plug );
OYAPI const char *  OYEXPORT
                oyConnector_GetReg  ( oyConnector_s     * obj );
OYAPI int  OYEXPORT
                 oyConnector_SetReg  ( oyConnector_s     * obj,
                                       const char        * type_registration );
OYAPI int  OYEXPORT
                 oyConnector_SetMatch( oyConnector_s     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func );
OYAPI oyCMMFilterSocket_MatchPlug_f  OYEXPORT
                 oyConnector_GetMatch (
                                       oyConnector_s     * obj );
