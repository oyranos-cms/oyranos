/** @file oyjl_i18n.c
 *
 *  oyjl - i18n helpers
 *
 *  @par Copyright:
 *            2016-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2023  Kai-Uwe Behrmann  <ku.b@gmx.de>
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


/* --- I18n_Section --- */

/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */
/** \addtogroup oyjl_i18n OyjlCore Internationalisation
 *  @brief Language support and Translations for UI's
 *
 *  The oyjlTranslation_s object is created by oyjlTranslation_New() and holds the context for
 *  translations including a possibly own translation function and user
 *  provided data. It is needed for any translation call, like oyjlTranslate()
 *  or oyjlTranslateJson(). A CLI exist in the @ref oyjltranslate tool.
 *  A typical single language catalog is usually set by the
 *  oyjlInitLanguageDebug() call during application setup.
 *  Multiple catalogs can be internally stored by oyjlTranslation_Set() and obtained by
 *  oyjlTranslation_Get() for advanced use.
 *  oyjlTreeSerialise() turnes a oyjl_val tree into
 *  a memory block, which could be written to file and later used as
 *  translation catalog. This memory block needs no JSON parser. The
 *  @ref oyjl tool provides a command line interface for it by the following
 *  command:
 *  @code
 *  oyjl json ${COLLECT_I18N_INPUT} -w oiJS -W lib${MY_PROJECT_DOWN_NAME}_i18n
 *  @endcode
 *
 *  @{ *//* oyjl_i18n */

/** @brief   obtain language part of i18n locale code
 *
 *  @param         loc                 locale name as from setlocale("")
 *  @return                            language part
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/27
 *  @since   2020/07/27 (Oyjl: 1.0.0)
 */
char *         oyjlLanguage          ( const char        * loc )
{
  char * t = NULL;

  if(loc[0] == 'C')
    t = strdup("");
  else
    if(strchr(loc,'_') != NULL)
  {
    t = strdup(loc);
    char * tmp = strchr(t,'_');
    tmp[0] = '\000';
  } else
    t = strdup(loc);

  if(*oyjl_debug) fprintf(stderr, OYJL_DBG_FORMAT "loc=\"%s\" -> \"%s\"\n", OYJL_DBG_ARGS, loc, t );
  return t;
}

/** @brief   obtain country part of i18n locale code
 *
 *  @param         loc                 locale name as from setlocale("")
 *  @return                            country part
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/27
 *  @since   2020/07/27 (Oyjl: 1.0.0)
 */
char *         oyjlCountry           ( const char        * loc )
{
  if(strchr(loc,'_') != NULL)
  {
    char * t = strdup( strchr(loc,'_') + 1 );
    if(strchr(t,'.') != NULL)
    {
      char * tmp = strchr(t,'.');
      tmp[0] = '\000';
    }
    return t;
  }
  else
    return NULL;
}

const char *   oyjlSetLocale         ( int                 category OYJL_UNUSED,
                                       const char        * loc )
{
  const char * lang = getenv("LANG"),
             * language = getenv("LANGUAGE"),
             * dbg = getenv("OYJL_DEBUG"),
             * setloc = NULL;
  int debug = dbg?atoi(dbg):0;
  if((lang && lang[0] && language && language[0] && strcmp(lang,language) != 0 && !oyjlStringStartsWith(lang,language) && !oyjlStringStartsWith(lang,"C")) ||
     (!(lang && lang[0]) && language && language[0]))
  {
    if(debug && lang) fprintf(stderr, OYJL_DBG_FORMAT "old LANG=%s ", OYJL_DBG_ARGS, getenv("LANG") );
    setenv("LANG", language, 1);
    if(debug) fprintf(stderr, OYJL_DBG_FORMAT "LANG=%s (LANGUAGE=%s) ", OYJL_DBG_ARGS, getenv("LANG"), getenv("LANGUAGE") );
  } else {
    if(!(language && language[0]) && lang && lang[0])
    {
      setenv("LANGUAGE", lang, 1);
      if(debug) fprintf(stderr, OYJL_DBG_FORMAT "LANGUAGE=%s (LANG=%s) ", OYJL_DBG_ARGS, getenv("LANGUAGE"), getenv("LANG") );
    }
  }
#ifdef OYJL_HAVE_LOCALE_H
  setloc = setlocale( category, loc );
#else
  setloc = loc;
#endif
  if(debug) fprintf(stderr, OYJL_DBG_FORMAT "setlocale(loc: %s) = %s\n", OYJL_DBG_ARGS, loc, setloc );
  return setloc;
}

/** @} *//* oyjl_i18n */
/** @} *//* oyjl_core */
/* --- I18n_Section --- */
