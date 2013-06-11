#include "oyCMMapi_s.h"

OYAPI const char *  OYEXPORT
                   oyCMMinfo_GetCMM  ( oyCMMinfo_s       * info );
OYAPI const char *  OYEXPORT
                   oyCMMinfo_GetVersion
                                     ( oyCMMinfo_s       * info );
OYAPI oyCMMGetText_f  OYEXPORT
                   oyCMMinfo_GetTextF( oyCMMinfo_s       * info );
OYAPI const char **  OYEXPORT
                   oyCMMinfo_GetTexts( oyCMMinfo_s       * info );
OYAPI int  OYEXPORT
                   oyCMMinfo_GetCompatibility
                                     ( oyCMMinfo_s       * info );
OYAPI oyCMMapi_s * OYEXPORT
                   oyCMMinfo_GetApi  ( oyCMMinfo_s       * info );
OYAPI void  OYEXPORT
                   oyCMMinfo_SetApi  ( oyCMMinfo_s       * info,
                                       oyCMMapi_s        * first_api );
#ifdef ICON_IS_OBJECTIFIED
OYAPI oyIcon_s * OYEXPORT
                   oyCMMinfo_GetIcon ( oyCMMinfo_s       * info );
#endif
OYAPI oyCMMinfoInit_f  OYEXPORT
                   oyCMMinfo_GetInitF( oyCMMinfo_s       * info );
OYAPI void  OYEXPORT
                   oyCMMinfo_SetInitF( oyCMMinfo_s       * info,
                                       oyCMMinfoInit_f     init );
