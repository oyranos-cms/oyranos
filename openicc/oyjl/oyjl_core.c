/** @file oyjl_core.c
 *
 *  oyjl - string, file i/o and other basic helpers
 *
 *  @par Copyright:
 *            2016-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2022  Kai-Uwe Behrmann  <ku.b@gmx.de>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <ctype.h>   /* isspace() tolower() isdigit() */
#include <math.h>    /* NAN */
#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   /* time_t localtime() */
#include <unistd.h>
#include <errno.h>
#include <wchar.h>  /* wcslen() */

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_version.h"
#include "oyjl_tree_internal.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>           /* setlocale LC_NUMERIC */
#endif
#ifdef OYJL_HAVE_REGEX_H
# include <regex.h>
#endif

/*     Sections     */
/* --- Debug_Section --- */
/* --- Message_Section --- */
/* --- String_Section --- */
/* --- IO_Section --- */
/* --- Render_Section --- */
/* --- Init_Section --- */
/* --- I18n_Section --- */
/* --- Misc_Section --- */


/** \addtogroup oyjl_core OyjlCore
 *  @brief Internationalisation, Terminal colors, I/O and String Handling
 *
 *  The oyjlTermColor() function family provides text marking on terminal
 *  for command line interfaces.
 *
 *  A custom message function can be set by oyjlMessageFuncSet(). The default
 *  error function will print to stderr.
 *
 *  @{ *//* oyjl_core */
/** @} *//* oyjl_core */

/** \addtogroup oyjl_io OyjlCore I/O
 *  @brief File I/O
 *
 *  Basic C FILE input and output is provided by oyjlWriteFile(), oyjlReadFile()
 *  and oyjlReadFileStreamToMem().
 *
 *  @{ *//* oyjl_io */
/** @} *//* oyjl_io */

/** \addtogroup oyjl_string OyjlCore String
 *  @brief String Handling
 *
 *  A convinient set of string API's is available in the oyjlStringXXX family.
 *  Those API's handle plain string arrays. oyjlStringAdd() uses variable args
 *  to format and append to a existing string. oyjlStringListXXX
 *  API's handle plain arrays of strings.
 *
 *  The oyjl_str based oyStrXXX API's use a more careful memory
 *  management and thus perform way faster on larger memory arrays as they
 *  need fewer allocations and copies. oyjlStr_New() allocates a new object,
 *  or oyjlStr_NewFrom() wrappes a existing string array into a new object.
 *  oyjlStr() lets you see the contained char array. oyjlStr_Push()
 *  performs fast concatenation. oyjlStr_Replace() uses the object advantages.
 *  oyjlStr_Pull() directly takes the char array out of control of the oyjl_str
 *  object and oyjlStr_Release() frees the object and all memory.
 *
 *  @{ *//* oyjl_string */
/** @} *//* oyjl_string */

/** \addtogroup oyjl
    @section oyjl_intro Introduction
  
    Oyjl API provides a platformindependent C interface for JSON I/O, conversion to and from
    XML + YAML, string helpers, file reading, testing and argument handling.

    The API's are quite independent. 

    @section oyjl_api API Documentation
    The API of the @ref oyjl is declared in the oyjl.h header file.
    - @ref oyjl_tree - OyjlTree JSON modeled C data structure with writers: *libOyjlCore*, all parsers (JSON,YAML,XML) reside in *libOyjl*
    - @ref oyjl_core - OyjlCore API: string, internationalisation, terminal colors and file i/o in *libOyjlCore*
    - @ref oyjl_test - OyjlTest API: header only implementation in *oyjl_test.h* and *oyjl_test_main.h*
    - @ref oyjl_args - OyjlArgs Argument Handling API: link to *libOyjlCore* or with slightly reduced functionality in the stand alone *oyjl_args.c* version

    @section oyjl_tools Tools Documentation
    Oyjl comes with a few tools, which use the Oyjl API's.
    - @ref oyjl - JSON manipulation
    - @ref oyjltranslate - localisation helper tool
    - @ref oyjlargs - code generation tool
    - @ref oyjlargsqml - interactive option renderer written in Qt's QML
 *  @{ *//* oyjl */

/* --- Misc_Section --- */
#include "oyjl_version.h"
#include "oyjl_git_version.h"
/** @brief  give the compiled in library version
 *
 *  @param[in]  vtype          request API type
 *                             - 0 - Oyjl version string
 *                             - 1 - GIT version string
 *  @return                    OYJL_VERSION_NAME at library compile time
 */
const char *   oyjlVersionName       ( int                 vtype )
{
  if(vtype == 1)
    return OYJL_GIT_VERSION;

  return OYJL_VERSION_NAME;
}
#include <yajl/yajl_parse.h>
#include <yajl/yajl_version.h>
/** @brief  give the compiled in library version
 *
 *  @param[in]  vtype          request API type
 *                             - 0 - Oyjl API
 *                             - 1 - Yajl API
 *  @return                    OYJL_VERSION at library compile time
 */
int            oyjlVersion           ( int                 vtype )
{
  if(vtype == 1)
    return YAJL_VERSION;

  return OYJL_VERSION;
}
/* --- Misc_Section --- */

/** @} *//* oyjl */

/* additional i18n strings */
void oyjlDummy_(void)
{
  char * t = oyjlStringCopy( _("Information"), 0 );
  free(t);
}


