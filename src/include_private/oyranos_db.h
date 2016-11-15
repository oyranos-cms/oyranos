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

#define oyjl_string_add                oyStringAddPrintf
#define oyjl_string_copy               oyStringCopy
#define oyjl_string_split              oyStringSplit
#define oyjl_string_list_add_list      oyStringListAdd
#define oyjl_string_list_cat_list      oyStringListCat
#define oyjl_string_list_release       oyStringListRelease_
#define oyjl_string_list_free_doubles  oyStringListFreeDoubles
#define oyjl_string_list_add_static_string oyStringListAddStaticString
#define oyjl_string_to_long            oyStringToLong
#define oyjl_array_free                oyJArrayFree
#define oyjl_message_p                 oy_oyjl_message_p
#define oyjl_object_free               oyJObjectFree
#define oyjl_tree_callbacks            oy_jtree_callbacks
#define oyjl_tree_free                 oyJTreeFree
#define oyjl_tree_free_content         oyJTreeFreeContent
#define oyjl_tree_free_node            oyJTreeFreeNode
#define oyjl_tree_get                  oyJTreeGet
#define oyjl_tree_get_value            oyJTreeGetValue
#define oyjl_tree_get_valuef           oyJTreeGetValuef
#define oyjl_tree_parse                oyJTreeParse
#define oyjl_tree_paths_get_index      oyJTreePathsGetIndex
#define oyjl_tree_to_json              oyJTreeToJson
#define oyjl_tree_to_paths             oyJTreeToPaths
#define oyjl_value_count               oyJValueCount
#define oyjl_value_pos_get             oyJValuePosGet
#define oyjl_value_set_string          oyJValueSetString
#define oyjl_value_text                oyJValueText
#include "oyjl/oyjl_tree.h"


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ELEKTRA_H */
