/** @file oyranos_elektra.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    elektra dependent functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#ifndef OYRANOS_ELEKTRA_H
#define OYRANOS_ELEKTRA_H

#include "oyranos.h"
#include "oyOptions_s.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- internal API definition --- */

/* separate from the external functions */
void oyDBOpen  (void);
void oyDBClose (void);
/* better use oyExportReset_(EXPORT_SETTING) */
void oyCloseReal__(void);

/* elektra key wrappers */
char *   oyDBGetString_              ( const char        * key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc );
int      oyDBSetString_              ( const char        * keyName,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment );
char*    oyDBSearchEmptyKeyname_     ( const char        * keyParentName,
                                       oySCOPE_e           scope );
char **  oyDBKeySetGetNames_         ( const char        * key_parent_name,
                                       oySCOPE_e           scope,
                                       int               * n );
int      oyDBGetStrings_             ( oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n,
                                       oySCOPE_e           scope );
int      oyDBEraseKey_               ( const char        * key_name,
                                       oySCOPE_e           scope );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ELEKTRA_H */
