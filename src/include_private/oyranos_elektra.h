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
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
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

typedef struct oyDB_s oyDB_s;
oyDB_s * oyDB_newFrom                ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc );
void     oyDB_release                ( oyDB_s           ** db );
char *   oyDB_getString              ( oyDB_s            * db,
                                       const char        * key_name );
int      oyDB_getStrings             ( oyDB_s            * db,
                                       oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n );
char **  oyDB_getKeyNames            ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );
char **  oyDB_getKeyNamesOneLevel    ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );

/* elektra key wrappers */
int      oyDBSetString_              ( const char        * keyName,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment );
char*    oyDBSearchEmptyKeyname_     ( const char        * keyParentName,
                                       oySCOPE_e           scope );
int      oyDBEraseKey_               ( const char        * key_name,
                                       oySCOPE_e           scope );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ELEKTRA_H */
