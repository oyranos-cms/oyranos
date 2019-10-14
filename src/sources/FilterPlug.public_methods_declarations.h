#include "oyConnector_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterSocket_s.h"
#include "oyPixelAccess_s.h"
#include "oyImage_s.h"
OYAPI int  OYEXPORT
                 oyFilterPlug_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI int  OYEXPORT
                 oyFilterPlug_ConnectIntoSocket (
                                       oyFilterPlug_s    * p,
                                       oyFilterSocket_s  * s );
OYAPI oyImage_s * OYEXPORT
             oyFilterPlug_ResolveImage(oyFilterPlug_s    * plug,
                                       oyFilterSocket_s  * socket,
                                       oyPixelAccess_s   * ticket );
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterPlug_GetNode    ( oyFilterPlug_s    * p );
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterPlug_GetRemoteNode
                                     ( oyFilterPlug_s    * p );
OYAPI oyFilterSocket_s * OYEXPORT
             oyFilterPlug_GetSocket  ( oyFilterPlug_s    * p );
OYAPI oyConnector_s * OYEXPORT
             oyFilterPlug_GetPattern ( oyFilterPlug_s    * p );
OYAPI const char * OYEXPORT
             oyFilterPlug_GetRelatives(oyFilterPlug_s    * p );
