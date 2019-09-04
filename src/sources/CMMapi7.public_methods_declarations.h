OYAPI int OYEXPORT
             oyCMMapi7_Run           ( oyCMMapi7_s       * api7,
                                       oyFilterPlug_s *    plug,
                                       oyPixelAccess_s *   ticket );
#include "oyCMMapi_s.h"
OYAPI oyCMMapi7_s *  OYEXPORT
             oyCMMapi7_Create        ( oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       const char        * context_type,
                                       oyCMMFilterPlug_Run_f run,
                                       oyConnector_s    ** plugs,
                                       uint32_t            plugs_n,
                                       uint32_t            plugs_last_add,
                                       oyConnector_s    ** sockets,
                                       uint32_t            sockets_n,
                                       uint32_t            sockets_last_add,
                                       const char       ** properties,
                                       oyObject_s          object );
