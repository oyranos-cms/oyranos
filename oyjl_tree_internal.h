/**
 *  @internal
 *  @file     oyjl_tree_internal.h
 *
 *  oyjl - convinient tree JSON APIs
 *
 *  @par Copyright:
 *            2011-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    tree based JSON API
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2016/10/28
 */

#ifndef OYJL_TREE_INTERNAL_H
#define OYJL_TREE_INTERNAL_H 1

#include "oyjl.h"
#include "oyjl_i18n.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) 
#define snprintf sprintf_s
#endif

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__unix__) || (!defined(_WIN32) && !defined(_MSC_VER)) || (defined(_WIN32) && defined(__CYGWIN__)) || defined(__MINGW32__) || defined(__MINGW32)
# include <unistd.h>
# if defined(_POSIX_VERSION)
#  define HAVE_POSIX 1
# endif
#endif

#define OYJL_LOCALE_VAR "OYJL_LOCALEDIR"
#define OYJL_DEBUG "OYJL_DEBUG"
#define OYJL_PRINT_POINTER "0x%tx"
extern int * oyjl_debug;
extern char * oyjl_debug_node_path_;
extern char * oyjl_debug_node_value_;
void       oyjlValueDebug_           ( oyjl_val            v,
                                       const char        * xpath,
                                       const char        * desc,
                                       int                 flags );
#define OYJL_DEBUG_NODE_IS_VALUE(xpath_,value_text_) ( oyjl_debug_node_path_ && oyjl_debug_node_path_[0] && xpath_ && oyjlPathMatch( xpath_, oyjl_debug_node_path_, 0 ) && ( (oyjl_debug_node_value_ && value_text_ && strstr( value_text_, oyjl_debug_node_value_ ) != NULL) || oyjl_debug_node_value_ == NULL))
int oyjlIsDirFull_ (const char* name);
int oyjlMakeDir_ (const char* path);

int        oyjlTreePathsGetIndex_    ( const char        * term,
                                       int               * index );
char *     oyjlTreePrint             ( oyjl_val            v );
#define OYJL_ENUM_CASE_TO_STRING(case_) case case_: return #case_
#define OYJL_ENUM_CASE_TO_VALUE(case_, val) case case_: value = val; break;

#define oyjlMEMORY_ALLOCATION_SECTIONS 0x01
#define oyjlMEMORY_ALLOCATION_ARRAY    0x02
#define oyjlMEMORY_ALLOCATION_GROUPS   0x04
#define oyjlMEMORY_ALLOCATION_OPTIONS  0x08

typedef struct {
  char       ** options; /* detected vanilla args + probably "@" for anonymous args */
  char       ** values; /* the vanilla args from main(argv[]) */
  int           count; /* number of detected options */
  int           group; /* detected group */
  void        * attr; /* oyjl_val attributes */
  int           memory_allocation; /* 0: as usual; 1 - sections, 2 - opts->groups and 4 - opts->array are owned and need to be released */
} oyjlOptsPrivate_s;
char * oyjlOptionsResultValueCopy_   ( const char        * arg,
                                       int                 flags );

oyjlUiHeaderSection_s * oyjlUiInfo_  ( const char        * documentation,
                                       const char        * date_name,
                                       const char        * date_description );
oyjl_val     oyjlUi_ExportToJson_    ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED );
int oyjlOptionMandatoryIndex_        ( oyjlOption_s      * opt,
                                       oyjlOptionGroup_s * g );
char *         oyjlOptionGetKey_     ( const char        * ostring );
int oyjlOptions_GroupHasOptionL_     ( oyjlOptions_s     * opts,
                                       int                 group_pos,
                                       const char        * option );
void oyjlOptions_Print_              ( oyjlOptions_s     * opts,
                                       int                 pos );
enum {
  oyjlOPTIONSTYLE_ONELETTER = 0x01,                     /* the single dash '-' style option is prefered: -o */
  oyjlOPTIONSTYLE_STRING = 0x02,                        /* add double dash '-' style option as well: --long-option */
  oyjlOPTIONSTYLE_OPTIONAL_START = 0x04,                /* print the option as optional: EBNF [--option] */
  oyjlOPTIONSTYLE_OPTIONAL_END = 0x08,                  /* print the option as optional: EBNF [--option] */
  oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP = 0x10,         /* add second level grouping: [[--option1|--option2]] */
  oyjlOPTIONSTYLE_MAN = 0x20,                           /* add Unix MAN page groff syntax decoration */
  oyjlOPTIONSTYLE_MARKDOWN = 0x40,                      /* add markdown syntax decoration */
  oyjlOPTIONSTYLE_GROUP_SUBCOMMAND = 0x080,             /* supresses dash(es): option */
  oyjlOPTIONSTYLE_OPTION_ONLY = 0x100                   /* print the option without any args */
};
char *       oyjlOption_PrintArg_    ( oyjlOption_s      * o,
                                       int                 style );
char *       oyjlOptions_PrintHelpSynopsis_ (
                                       oyjlOptions_s  *    opts,
                                       oyjlOptionGroup_s * g,
                                       int                 style );
void oyjlUiCanonicaliseVariableName_ ( char             ** name );
int  oyjlManArgIsNum_                ( const char        * arg );
int  oyjlManArgIsEditable_           ( const char        * arg );
int  oyjlManAddOptionToGroup_        ( char             ** group,
                                       char                o,
                                       const char        * option,
                                       const char        * delimiter,
                                       int                 flags );

char *       oyjlStringToLower_      ( const char        * t );
char *       oyjlStringToUpper_      ( const char        * t );

#ifdef __cplusplus
}
#endif

#endif /* OYJL_TREE_INTERNAL_H */
