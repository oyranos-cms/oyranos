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

#include "oyFilterPlug_s.h"
#include "oyFilterSocket_s.h"
/** typedef  oyCMMFilterSocket_MatchPlug_f
 *  @brief   verify connectors matching each other
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  A implementation for images is included in the core function
 *  oyFilterSocket_MatchImagePlug().
 *
 *  @param         socket              a filter socket
 *  @param         plug                a filter plug
 *  @return                            1 on success, otherwise 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/20 (Oyranos: 0.1.10)
 *  @date    2009/04/20
 */
typedef int          (*oyCMMFilterSocket_MatchPlug_f) (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug );

OYAPI int  OYEXPORT
                 oyConnector_SetMatch( oyConnector_s     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func );
OYAPI oyCMMFilterSocket_MatchPlug_f  OYEXPORT
                 oyConnector_GetMatch (
                                       oyConnector_s     * obj );
