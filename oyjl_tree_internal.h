/**
 *  @internal
 *  @file     oyjl_tree_internal.h
 *
 *  oyjl - convinient tree JSON APIs
 *
 *  @par Copyright:
 *            2011-2020 (C) Kai-Uwe Behrmann
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
extern int * oyjl_debug;
int oyjlIsDirFull_ (const char* name);

int        oyjlTreePathsGetIndex_    ( const char        * term,
                                       int               * index );

typedef struct {
  char       ** options; /* detected vanilla args + probably "@" for anonymous args */
  const char ** values; /* the vanilla args from main(argv[]) */
  int           count; /* number of detected options */
  int           group; /* detected group */
  void        * attr; /* oyjl_val attributes */
  int           memory_allocation; /* 0: as usual; 1: sections, opts->groups and opts->array are owned and need to be released */
} oyjlOptsPrivate_s;

oyjlUiHeaderSection_s * oyjlUiInfo_  ( const char          * documentation,
                                       const char          * date_name,
                                       const char          * date_description );
int oyjlOptionMandatoryIndex_        ( oyjlOption_s      * opt,
                                       oyjlOptionGroup_s * g );

char *       oyjlStringToLower       ( const char        * t );
char *       oyjlStringToUpper       ( const char        * t );

#ifdef __cplusplus
}
#endif

#endif /* OYJL_TREE_INTERNAL_H */
