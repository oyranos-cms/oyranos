/** @file oyranos_db.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016-2018 (C) Kai-Uwe Behrmann
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

#define oyjlStringAdd                oyStringAddPrintf
#define oyjlStringCopy               oyStringCopy
#define oyjlStringSplit              oyStringSplit
#define oyjlStringAppendn            oyStringAppendN
#define oyjlStringAddn               oyStringAddN
#define oyjlStringReplace            oyStringReplace
#define oyjlStringListAddList      oyStringListAdd
#define oyjlStringListCatList      oyStringListCat
#define oyjlStringListRelease       oyStringListRelease
#define oyjlStringListFreeDoubles  oyStringListFreeDoubles
#define oyjlStringListAddStaticString oyStringListAddStaticString
#define oyjlStringToLong            oyStringToLong
#define oyjlStringToDouble          oyStringToDouble
#include "oyjl/oyjl.h"

int          oyJson                  ( const char        * json );
oyjl_val     oyJsonParse             ( const char        * json );
char *       oyJsonFromModelAndUi    ( const char        * data,
                                       const char        * ui_text,
                                       oyAlloc_f           allocate_func );
char *       oyJsonPrint             ( oyjl_val            root );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_JSON_H */
