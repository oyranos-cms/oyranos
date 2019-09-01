/*  @file openicc_db.h
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2015-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2015/08/26
 */

#ifndef __OPENICC_DB_H__
#define __OPENICC_DB_H__

#include "openicc_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** \addtogroup OpenICC_config

 *  @{
 */

typedef struct openiccDB_s openiccDB_s;
openiccDB_s * openiccDB_NewFrom      ( const char        * top_key_name,
                                       openiccSCOPE_e      scope );
void     openiccDB_Release           ( openiccDB_s      ** db );
int      openiccDB_GetString         ( openiccDB_s       * db,
                                       const char        * xpath,
                                       const char       ** value );
int      openiccDB_GetKeyNames       ( openiccDB_s       * db,
                                       const char        * xpath,
                                       int                 child_levels,
                                       openiccAlloc_f      alloc,
                                       openiccDeAlloc_f    dealloc,
                                       char            *** key_names,
                                       int               * n );
  
/* key manipulation */
const char * openiccGetShortKeyFromFullKeyPath( 
                                       const char        * key,
                                       char             ** temp );

/* DB key wrappers */
int      openiccDBSetString          ( const char        * keyName,
                                       openiccSCOPE_e      scope,
                                       const char        * value,
                                       const char        * comment );
char*    openiccDBSearchEmptyKeyname ( const char        * keyParentName,
                                       openiccSCOPE_e      scope );
int      openiccDBEraseKey_          ( const char        * key_name,
                                       openiccSCOPE_e      scope );

/* DB raw access */
char *        openiccDBGetJSONFile   ( openiccSCOPE_e      scope );

/** 
 *  @} *//*OpenICC_config
 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif /* __OPENICC_DB_H__ */
