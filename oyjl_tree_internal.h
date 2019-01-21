/**
 *  @internal
 *  @file     oyjl_tree_internal.h
 *
 *  oyjl - convinient tree JSON APIs
 *
 *  @par Copyright:
 *            2011-2016 (C) Kai-Uwe Behrmann
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

#if defined(__GNUC__)
# define  OYJL_DBG_FORMAT_ "%s:%d %s() "
# define  OYJL_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__,__func__
#else
# define  OYJL_DBG_FORMAT_ "%s:%d "
# define  OYJL_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
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

int        oyjlTreePathsGetIndex     ( const char        * term,
                                       int               * index );

#ifdef __cplusplus
}
#endif

#endif /* OYJL_TREE_INTERNAL_H */
