/** @file oyranos_db.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    JSON functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/11/17
 */

#ifndef OYRANOS_JSON_H
#define OYRANOS_JSON_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "oyranos_string.h"
#include "oyjl.h"

oyjl_val     oyJsonParse             ( const char        * json,
                                       int               * status );
char *       oyJsonFromModelAndUi    ( const char        * data,
                                       const char        * ui_text,
                                       oyAlloc_f           allocate_func );
char *       oyJsonPrint             ( oyjl_val            root );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_JSON_H */
