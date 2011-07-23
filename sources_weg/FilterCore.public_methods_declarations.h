const char * oyFilterCore_CategoryGet( oyFilterCore_s    * filter,
                                       int                 nontranslated );
const char * oyFilterCore_GetName    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type );
const char * oyFilterCore_GetText    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type );
oyFilterCore_s * oyFilterCore_NewWith( const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
