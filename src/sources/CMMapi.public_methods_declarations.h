void               oyCMMapi_Set      ( oyCMMapi_s        * api,
                                       oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3] );
void               oyCMMapi_SetNext  ( oyCMMapi_s        * api,
                                       oyCMMapi_s        * next );
oyCMMapi_s *       oyCMMapi_GetNext  ( oyCMMapi_s        * api );
oyCMMInit_f        oyCMMapi_GetInitF ( oyCMMapi_s        * api );
oyCMMReset_f       oyCMMapi_GetResetF( oyCMMapi_s        * api );
oyCMMMessageFuncSet_f
                   oyCMMapi_GetMessageFuncSetF
                                     ( oyCMMapi_s        * api );
const char *       oyCMMapi_GetRegistration
                                     ( oyCMMapi_s        * api );
