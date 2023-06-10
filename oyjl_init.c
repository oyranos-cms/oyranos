/** @file oyjl_init.c
 *
 *  oyjl - basic helpers
 *
 *  @par Copyright:
 *            2016-2023 (C) Kai-Uwe Behrmann
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

/* --- Init_Section --- */

/** \addtogroup oyjl
 *  @{ *//* oyjl */

static char * oyjl_nls_path_ = NULL;
char * oyjl_debug_node_path_ = NULL;
char * oyjl_debug_node_value_ = NULL;
extern char * oyjl_term_color_;
extern char * oyjl_term_color_html_;
extern char * oyjl_term_color_plain_;
extern int oyjl_args_render_init_;
extern void *  oyjl_args_render_lib_;
#ifdef HAVE_DL
# include <dlfcn.h>   /* dlopen() dlclose() */
#endif
/** @brief Reset and free all resources */
void oyjlLibRelease() {
  int i;
  if(oyjl_nls_path_)
  {
    putenv("NLSPATH=C"); free(oyjl_nls_path_); oyjl_nls_path_ = NULL;
  }
#if defined(HAVE_DL) && (!defined(COMPILE_STATIC) || !defined(HAVE_QT))
  if(oyjl_args_render_lib_)
  {
    dlclose(oyjl_args_render_lib_); oyjl_args_render_lib_ = NULL; oyjl_args_render_init_ = 0;
  }
#endif
  if(oyjl_translation_context_)
  {
    i = 0;
    while(oyjl_translation_context_[i])
    {
      oyjlTranslation_Release( &oyjl_translation_context_[i] );
      ++i;
    }
    free(oyjl_translation_context_);
    oyjl_translation_context_ = NULL;
  }
  if(oyjl_debug_node_path_)
  {
    free(oyjl_debug_node_path_);
    oyjl_debug_node_path_ = NULL;
  }
  if(oyjl_debug_node_value_)
  {
    free(oyjl_debug_node_value_);
    oyjl_debug_node_value_ = NULL;
  }
  if(oyjl_term_color_)
  {
    free(oyjl_term_color_);
    oyjl_term_color_ = NULL;
  }
  if(oyjl_term_color_html_)
  {
    free(oyjl_term_color_html_);
    oyjl_term_color_html_ = NULL;
  }
  if(oyjl_term_color_plain_)
  {
    free(oyjl_term_color_plain_);
    oyjl_term_color_plain_ = NULL;
  }
}
int oyjlIsFileFull_ (const char* fullFileName, const char * read_mode);
#define OyjlToString2_M(t) OyjlToString_M(t)
#define OyjlToString_M(t) #t
void   oyjlGettextSetup_             ( int                 use_gettext OYJL_UNUSED,
                                       const char        * loc_domain OYJL_UNUSED,
                                       const char        * env_var_locdir OYJL_UNUSED,
                                       const char        * default_locdir OYJL_UNUSED )
{
#ifdef OYJL_HAVE_LIBINTL_H
  {
    if( use_gettext )
    {
      char * var = NULL, * tmp = NULL;
      const char * environment_locale_dir = NULL,
                 * locpath = NULL,
                 * path = NULL,
                 * domain_path = default_locdir;

      if(getenv(env_var_locdir) && strlen(getenv(env_var_locdir)))
      {
        tmp = strdup(getenv(env_var_locdir));
        environment_locale_dir = domain_path = tmp;
        if(*oyjl_debug)
          oyjlMessage_p( oyjlMSG_INFO, 0,"found environment variable: %s=%s", env_var_locdir, domain_path );
      } else
        if(environment_locale_dir == NULL && getenv("LOCPATH") && strlen(getenv("LOCPATH")))
      {
        domain_path = NULL;
        locpath = getenv("LOCPATH");
        if(*oyjl_debug)
          oyjlMessage_p( oyjlMSG_INFO, 0,"found environment variable: LOCPATH=%s", locpath );
      } else
        if(*oyjl_debug)
        oyjlMessage_p( oyjlMSG_INFO, 0,"no %s or LOCPATH environment variable found; using default path: %s", env_var_locdir, domain_path );

      if(domain_path || locpath)
      {
        oyjlStringAdd( &var, 0,0, "NLSPATH=");
        oyjlStringAdd( &var, 0,0, domain_path ? domain_path : locpath);
      }
      if(var) /* do not release */
      {
        putenv(var); /* Solaris */
        if(oyjl_nls_path_)
          free(oyjl_nls_path_);
        oyjl_nls_path_ = var;
      }

      /* LOCPATH appears to be ignored by bindtextdomain("domain", NULL),
       * so it is set here to bindtextdomain(). */
      path = domain_path ? domain_path : locpath;
      const char * d = textdomain( NULL );
      const char * dpath = bindtextdomain( loc_domain, path );
      if(*oyjl_debug)
      {
        char * fn = NULL;
        int stat = -1;
        const char * gettext_call = OyjlToString2_M(_());
        const char * domain = textdomain(NULL);

        oyjlMessage_p( oyjlMSG_INFO, 0,"bindtextdomain( \"%s\", \"%s\" ) = %s textdomain( NULL ) = %s", loc_domain, path, dpath, d );
        if(path)
          oyjlStringAdd( &fn, 0,0, "%s/de/LC_MESSAGES/%s.mo", path ? path : "", loc_domain);
        if(fn)
          stat = oyjlIsFileFull_( fn, "r" );
        oyjlMessage_p( oyjlMSG_INFO, 0,"bindtextdomain(\"%s\"/%s) to %s\"%s\" %s for %s  test:%s", loc_domain, domain, locpath?"effectively ":"", path ? path : "", (stat > 0)?"Looks good":"Might fail", gettext_call, _("Example") );
        if(fn) free(fn);
      }
      if(tmp)
        free(tmp);
    }
  }
#endif /* OYJL_HAVE_LIBINTL_H */
}
void   oyjlInitI18n_                 ( const char        * loc )
{
#ifndef OYJL_SKIP_TRANSLATE
  oyjl_val oyjl_catalog = NULL;
  oyjlTranslation_s * trc = NULL;
  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#else
# include "liboyjl.i18n.h"
  int size = sizeof(liboyjl_i18n_oiJS);
  oyjl_catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  if(*oyjl_debug)
    oyjlMessage_p( oyjlMSG_INFO, 0,OYJL_DBG_FORMAT "loc: \"%s\" domain: \"%s\" catalog-size: %d", OYJL_DBG_ARGS, loc, OYJL_DOMAIN, size );
#endif
  oyjlGettextSetup_( use_gettext, OYJL_DOMAIN, "OYJL_LOCALEDIR", OYJL_LOCALEDIR );
  trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &oyjl_catalog, 0,0,0, *oyjl_debug > 1?OYJL_OBSERVE:0 );
  oyjlTranslation_SetFlags( trc, 0 );
  oyjlTranslation_Set( &trc );
#endif
}

/** @brief   init the libraries language; optionaly
 *
 *  Additionally use setlocale() to obtain locale in your application.
 *  The message catalog search path is detected from the project specific
 *  environment variable specified in \em env_var_locdir and
 *  the \em LOCPATH environment variables. If those are not present
 *  a expected fall back directory from \em default_locdir is used.
 *
 *  @param         project_name        project name display string; e.g. "MyProject"
 *  @param         env_var_debug       environment debug variable string;
 *                                     e.g. "MP_DEBUG"
 *  @param         debug_variable      int C variable; e.g. my_project_debug
 *  @param         use_gettext         switch gettext support on or off
 *  @param         env_var_locdir      environment variable string for locale path;
 *                                     e.g. "MP_LOCALEDIR"
 *  @param         default_locdir      default locale path C string;
 *                                     e.g. "/usr/local/share/locale"
 *  @param         context             locale, domain and possibly more information; use oyjlTranslation_New()
 *                                     - domain: po and mo files; e.g. "myproject"
 *  @param         msg                 your message function of type oyjlMessage_f; optional - default is Oyjl message function
 *  @return                            error
 *                                     - -1 : issue
 *                                     - 0 : success
 *                                     - >= 1 : found error
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/01/13
 *  @since   2019/01/13 (Oyjl: 1.0.0)
 */
int oyjlInitLanguageDebug            ( const char        * project_name,
                                       const char        * env_var_debug,
                                       int               * debug_variable,
                                       int                 use_gettext OYJL_UNUSED,
                                       const char        * env_var_locdir OYJL_UNUSED,
                                       const char        * default_locdir OYJL_UNUSED,
                                       oyjlTranslation_s** context,
                                       oyjlMessage_f       msg )
{
  int error = -1;
  oyjlTranslation_s * trc = context?*context:NULL;
  const char * loc = oyjlTranslation_GetLang( trc );
  const char * loc_domain = oyjlTranslation_GetDomain( trc );

  if(!msg) msg = oyjlMessage_p;

  if(debug_variable)
    oyjlDebugVariableSet( debug_variable );
  oyjlMessageFuncSet(msg);

  if(debug_variable && *debug_variable)
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "loc: %s loc_domain: %s", OYJL_DBG_ARGS, loc, loc_domain );

  if(debug_variable && getenv(env_var_debug))
  {
    *debug_variable = atoi(getenv(env_var_debug));
    if(*debug_variable)
    {
      int v = oyjlVersion(0);
      if(*debug_variable)
        msg( oyjlMSG_INFO, 0, "%s (Oyjl compile v: %s runtime v: %d)", project_name, OYJL_VERSION_NAME, v );
    }
  }

  oyjlInitI18n_( loc );

  if(loc_domain)
  {
    oyjlGettextSetup_( use_gettext, loc_domain, env_var_locdir, default_locdir );
    int state = oyjlTranslation_Set( context ); /* just pass domain in */
    if(*oyjl_debug)
      msg( oyjlMSG_INFO, 0, "use_gettext: %d loc_domain: %s env_var_locdir: %s default_locdir: %s oyjlTranslation_Set: %d", use_gettext, loc_domain, env_var_locdir, default_locdir, state );
  }

  return error;
}
/* --- Init_Section --- */
