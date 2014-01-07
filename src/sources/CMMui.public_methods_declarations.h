#include "oyranos_module.h"
#include "oyCMMapiFilter_s.h"
OYAPI oyCMMui_s *  OYEXPORT
                   oyCMMui_Create    ( const char        * category,
                                       oyCMMGetText_f      getText,
                                       const char       ** texts,
                                       oyObject_s          object );
OYAPI void  OYEXPORT
                   oyCMMui_SetUiOptions(
                                       oyCMMui_s         * ui,
                                       const char        * options,
                                       oyCMMuiGet_f        oyCMMuiGet );
oyCMMGetText_f     oyCMMui_GetTextF  ( oyCMMui_s         * ui );
const char **      oyCMMui_GetTexts  ( oyCMMui_s         * ui );
oyCMMapiFilter_s * oyCMMui_GetParent ( oyCMMui_s         * ui );
