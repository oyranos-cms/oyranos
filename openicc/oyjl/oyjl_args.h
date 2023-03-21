/** @file oyjl_args.h
 *
 *  oyjl - Args helper C API's
 *
 *  @par Copyright:
 *            2010-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl API provides a platformindependent C interface for argument handling.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_ARGS_H
#define OYJL_ARGS_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#include "oyjl_args_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup oyjl_args
 *  @{ *//* oyjl_args */

void oyjlOptionChoice_Release        ( oyjlOptionChoice_s**choices );
int  oyjlOptionChoice_Count          ( oyjlOptionChoice_s* list );
char * oyjlOption_PropertiesGetValue ( oyjlOption_s      * o,
                                       const char        * key );
int    oyjlOptions_Count             ( oyjlOptions_s     * opts );
int    oyjlOptions_CountGroups       ( oyjlOptions_s     * opts );
oyjlOption_s * oyjlOptions_GetOption ( oyjlOptions_s     * opts,
                                       const char        * ol );
#define OYJL_QUIET                     0x100000 /**< @brief be silent on error */
oyjlOption_s * oyjlOptions_GetOptionL( oyjlOptions_s     * opts,
                                       const char        * ostring,
                                       int                 flags );
oyjlOptions_s *    oyjlOptions_New   ( int                 argc,
                                       const char       ** argv );
oyjlOPTIONSTATE_e  oyjlOptions_Parse ( oyjlOptions_s     * opts );
oyjlOPTIONSTATE_e  oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       const char        * oc,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int );
char *   oyjlOptions_ResultsToJson   ( oyjlOptions_s     * opts,
                                       int                 flags );
char *   oyjlOptions_ResultsToText   ( oyjlOptions_s     * opts );
void     oyjlOptions_SetAttributes   ( oyjlOptions_s     * opts,
                                       oyjl_val          * root );
char * oyjlOptions_PrintHelp         ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       FILE             ** out_file,
                                       const char        * motto_format,
                                                           ... );
oyjlUi_s *         oyjlUi_New        ( int                 argc,
                                       const char       ** argv );
oyjlUi_s *         oyjlUi_Copy       ( oyjlUi_s          * src );
oyjlUi_s *         oyjlUi_FromOptions( const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOptions_s     * opts,
                                       int               * status );
void               oyjlUi_ReleaseArgs( oyjlUi_s         ** ui );
void               oyjlUi_Release    ( oyjlUi_s         ** ui );
int      oyjlUiHeaderSection_Count   ( oyjlUiHeaderSection_s * sections );
oyjlUiHeaderSection_s * oyjlUiHeaderSection_Append (
                                       oyjlUiHeaderSection_s * sections,
                                       const char        * nick,
                                       const char        * label,
                                       const char        * name,
                                       const char        * description );
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick );
char *             oyjlUi_ToJson     ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ToMan      ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ToMarkdown ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ExportToJson(oyjlUi_s          * ui,
                                       int                 flags );
/** @brief export type */
typedef enum {
  oyjlARGS_EXPORT_HELP,                /**< @brief help text for command line */
  oyjlARGS_EXPORT_JSON,                /**< @brief rendering data in JSON format */
  oyjlARGS_EXPORT_MAN,                 /**< @brief Unix Man page */
  oyjlARGS_EXPORT_MARKDOWN,            /**< @brief Markdown for conversion into HTML */
  oyjlARGS_EXPORT_EXPORT               /**< @brief object data in JSON format */
} oyjlARGS_EXPORT_e;
char *             oyjlUi_ToText     ( oyjlUi_s          * ui,
                                       oyjlARGS_EXPORT_e   type,
                                       int                 flags );
oyjlUi_s *         oyjlUi_ImportFromJson(
                                       oyjl_val            root,
                                       int                 flags );
#define OYJL_SOURCE_CODE_C             0x01 /**< @brief C programming language source code */
#define OYJL_WITH_OYJL_ARGS_C          0x02 /**< @brief Include oyjl_args.c . Skip libOyjlCore code. */
#define OYJL_NO_DEFAULT_OPTIONS        0x04 /**< @brief omit automatic options generation for --help, --X export or --verbose */
#define OYJL_SUGGEST_VARIABLE_NAMES    0x08 /**< @brief automatic suggestion of variable names for missing oyjlOption_s::o and oyjlOption_s::option members */
#define OYJL_WITH_OYJL_ARGS_BASE_API   0x10 /**< @brief Restrict to oyjl_args_base.h API's. */
#define OYJL_COMPLETION_BASH           0x100 /**< @brief bash completion source code */
char *             oyjlUiJsonToCode  ( oyjl_val            root,
                                       int                 flags );
void               oyjlUi_Translate  ( oyjlUi_s          * ui,
                                       oyjlTr_s          * context );

/** link with libOyjlArgsWeb and use microhttps WWW renderer as library \n
 *  link with libOyjlArgsQml and use Qt's QML to render in a GUI
 *  @see oyjlUi_ToJson()
 */
int                oyjlArgsRender    ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv) );

/** 
 *  @} *//* oyjl_args
 */


#ifdef __cplusplus
}
#endif


#endif /* OYJL_ARGS_H */
