/** @file oyranos_db.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    Configuration DB functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/25
 */

#ifndef OYRANOS_ELEKTRA_H
#define OYRANOS_ELEKTRA_H

#include "oyranos_config.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- internal API definition --- */
extern oyDB_newFrom_f             oyDB_newFrom;
extern oyDB_release_f             oyDB_release;
extern oyDB_getString_f           oyDB_getString;
extern oyDB_getStrings_f          oyDB_getStrings;
extern oyDB_getKeyNames_f         oyDB_getKeyNames;
extern oyDB_getKeyNamesOneLevel_f oyDB_getKeyNamesOneLevel;
extern oyDBSetString_f            oyDBSetString;
extern oyDBSearchEmptyKeyname_f   oyDBSearchEmptyKeyname;
extern oyDBEraseKey_f             oyDBEraseKey;



#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ELEKTRA_H */
