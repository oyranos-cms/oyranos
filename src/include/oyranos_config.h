/** @file oyranos_config.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    configuration access
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/25
 */


#ifndef OYRANOS_CONFIG_H
#define OYRANOS_CONFIG_H

#include <stdlib.h> /* for linux size_t */
#include "oyranos_definitions.h"
#include "oyOptions_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- behaviour / policies --- */


/* --- default profiles --- */


/* --- profile lists --- */


/* --- profile checking --- */


/* --- profile access through oyranos --- */


/* --- DB hooks --- */
/* simple object type */
typedef struct oyDB_s oyDB_s;
/* some common struct members for oyDB_s */
struct oyDB_s_common {
  char       type[8];                  /* 4 chars long ID string + zero terminated */
  oySCOPE_e  scope;
  char     * top_key_name;
  oyAlloc_f  alloc;
  oyDeAlloc_f deAlloc;

  /* private members follow here
   *
   * ...
   */
};

typedef  oyDB_s*(* oyDB_newFrom_f)   ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc );
typedef  void   (* oyDB_release_f)   ( oyDB_s           ** db );
typedef  char * (* oyDB_getString_f) ( oyDB_s            * db,
                                       const char        * key_name );
typedef  int    (* oyDB_getStrings_f)( oyDB_s            * db,
                                       oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n );
typedef  char **(* oyDB_getKeyNames_f)(oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );
typedef  char **(* oyDB_getKeyNamesOneLevel_f)
                                     ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );

/* DB write wrappers */
typedef  int    (* oyDBSetString_f)  ( const char        * keyName,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment );
typedef  char*  (* oyDBSearchEmptyKeyname_f) (
                                       const char        * keyParentName,
                                       oySCOPE_e           scope );
typedef  int    (* oyDBEraseKey_f)   ( const char        * key_name,
                                       oySCOPE_e           scope );
typedef struct {
  oyDB_newFrom_f newFrom;
  oyDB_release_f release;
  oyDB_getString_f getString;
  oyDB_getStrings_f getStrings;
  oyDB_getKeyNames_f getKeyNames;
  oyDB_getKeyNamesOneLevel_f getKeyNamesOneLevel;

  oyDBSetString_f setString;
  oyDBSearchEmptyKeyname_f searchEmptyKeyname;
  oyDBEraseKey_f eraseKey;
} oyDbAPI_s;

int                oyDbHandlingReady ( void );
int                oyDbHandlingSet   ( const oyDbAPI_s   * db_api );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CONFIG_H */
