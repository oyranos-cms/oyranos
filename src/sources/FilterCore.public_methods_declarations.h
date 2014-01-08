OYAPI const char *  OYEXPORT
                oyFilterCore_GetCategory (
                                       oyFilterCore_s    * filter,
                                       int                 nontranslated );
OYAPI const char *  OYEXPORT
                oyFilterCore_GetRegistration (
                                       oyFilterCore_s    * filter );
OYAPI const char *  OYEXPORT
                oyFilterCore_GetName ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type );
OYAPI const char *  OYEXPORT
                oyFilterCore_GetText ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type );
OYAPI oyFilterCore_s *  OYEXPORT
                oyFilterCore_NewWith ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
#include "oyPointer_s.h"
OYAPI oyPointer_s *  OYEXPORT
                 oyFilterCore_GetBackendContext(
                                       oyFilterCore_s     * filter );
OYAPI int  OYEXPORT
                 oyFilterCore_SetBackendContext(
                                       oyFilterCore_s     * filter,
                                       oyPointer_s        * data );
