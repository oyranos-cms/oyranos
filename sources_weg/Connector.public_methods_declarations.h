int              oyConnector_SetName ( oyConnector_s     * obj,
                                       const char        * string,
                                       oyNAME_e            type );
const char *     oyConnector_GetName ( oyConnector_s     * obj,
                                       oyNAME_e            type );
int              oyConnector_IsPlug  ( oyConnector_s     * obj );
int              oyConnector_SetIsPlug(oyConnector_s     * obj,
                                       int                 is_plug );
const char *     oyConnector_GetReg  ( oyConnector_s     * obj );
int              oyConnector_SetReg  ( oyConnector_s     * obj,
                                       const char        * type_registration );
int              oyConnector_SetMatch( oyConnector_s     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func );
oyCMMFilterSocket_MatchPlug_f oyConnector_GetMatch (
                                       oyConnector_s     * obj );
