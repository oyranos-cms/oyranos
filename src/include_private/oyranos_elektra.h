/** @file oyranos_elektra.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
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


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- internal API definition --- */

/* separate from the external functions */
void oyOpen  (void);
void oyClose (void);
/* better use oyExportReset_(EXPORT_SETTING) */
void oyCloseReal__(void);

int     oyGetBehaviour_        (oyBEHAVIOUR_e type);
int     oySetBehaviour_        (oyBEHAVIOUR_e type,
                                int         behaviour);

/* elektra key wrappers */
char*   oyGetKeyString_        (const char       *key_name,
                                oyAlloc_f         allocFunc );
int     oyAddKey_valueComment_ (const char* keyName,
                                const char* value, const char* comment);
char*   oySearchEmptyKeyname_        ( const char        * keyParentName );
char **            oyKeySetGetNames_ ( const char        * key_parent_name,
                                       int               * n );
int                oyEraseKey_       ( const char        * key_name );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ELEKTRA_H */
