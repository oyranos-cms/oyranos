/** @file oyjl.h
 *
 *  oyjl - Basic helper C API's
 *
 *  @par Copyright:
 *            2010-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl API provides a platformindependent C interface for JSON I/O, conversion to and from
 *            XML + YAML, string helpers, file reading, testing and argument handling.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_H
#define OYJL_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#include "oyjl_version.h"
#include "oyjl_args_base.h"
#include "oyjl_io.h"
#include "oyjl_debug.h"
#include "oyjl_macros_compile.h"
#include "oyjl_string.h"
#include "oyjl_tree.h"
#include "oyjl_i18n.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --- Core --- */
/** \addtogroup oyjl Oyranos JSON Library
 *  Basic low level API's
 *  @{ *//* oyjl */
/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */
/** @} *//* oyjl_core */
/** \addtogroup oyjl_tree
 *  @{ */
/** @} */

int            oyjlVersion           ( int                 type );
const char *   oyjlVersionName       ( int                 type );


int            oyjlInitLanguageDebug ( const char        * project_name,
                                       const char        * env_var_debug,
                                       int               * debug_variable,
                                       int                 use_gettext,
                                       const char        * env_var_locdir,
                                       const char        * default_locdir,
                                       oyjlTranslation_s** context,
                                       oyjlMessage_f       msg );

void       oyjlLibRelease            ( );

#include "oyjl_args.h"
/** \addtogroup oyjl_args
 *  @{ */

/** @brief Types of known objects */
typedef enum {
  oyjlOBJECT_NONE,
  oyjlOBJECT_OPTION = 1769433455,      /**< @brief oyjlOption_s */
  oyjlOBJECT_OPTION_GROUP = 1735879023,/**< @brief oyjlOptionGroup_s */
  oyjlOBJECT_OPTIONS = 1937205615,     /**< @brief oyjlOptions_s */
  oyjlOBJECT_UI_HEADER_SECTION = 1936222575, /**< @brief oyjlUiHeaderSection_s */
  oyjlOBJECT_UI = 1769302383,          /**< @brief oyjlUi_s */
  oyjlOBJECT_TR = 1920231791,          /**< @brief oyjlTranslation_s */
  oyjlOBJECT_JSON = 1397385583         /**< @brief oyjlNodes_s */
} oyjlOBJECT_e;

/** 
 *  @} *//* oyjl_args
 */

/** @} *//* oyjl */

#ifdef __cplusplus
}
#endif


#endif /* OYJL_H */
