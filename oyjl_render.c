/** @file oyjl_render.c
 *
 *  oyjl - string, file i/o and other basic helpers
 *
 *  @par Copyright:
 *            2016-2022 (C) Kai-Uwe Behrmann
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

/* --- Render_Section --- */
#warning "compile dynamic section"
#ifdef COMPILE_STATIC
#warning "COMPILE_STATIC defined and compile oyjlArgsRender()"
#endif
#ifdef HAVE_QT
#warning "HAVE_QT defined"
#endif

#ifdef HAVE_DL
#include <dlfcn.h>
void *  oyjl_args_render_lib_ = NULL;
static char *  oyjlLibNameCreate_    ( const char        * lib_base_name,
                                       int                 version )
{
  char * fn = NULL;

#ifdef __APPLE__
    oyjlStringAdd( &fn, 0,0, "%s%s.%d%s", OYJL_LIB_PREFIX, lib_base_name, version, OYJL_LIB_SUFFIX );
#elif defined(_WIN32)
    oyjlStringAdd( &fn, 0,0, "%s%s-%d%s", OYJL_LIB_PREFIX, lib_base_name, version, OYJL_LIB_SUFFIX );
#else
    oyjlStringAdd( &fn, 0,0, "%s%s%s.%d", OYJL_LIB_PREFIX, lib_base_name, OYJL_LIB_SUFFIX, version );
#endif
  return fn;
}
static char *  oyjlFuncNameCreate_   ( const char        * base_name )
{
  char * func = NULL;

  func = oyjlStringCopy( base_name, NULL );
  if(func)
  {
    func[0] = tolower(func[0]);
    oyjlStringAdd( &func, 0,0, "_" );
  }
  return func;
}
#else
#warning "HAVE_DL not defined (possibly dlfcn.h not found?): dynamic loading of libOyjlArgsQml will not be possible"
#endif /* HAVE_DL */

#ifdef __cplusplus
extern "C" { // "C" API wrapper 
#endif
typedef int (*oyjlArgsRender_f)     ( int                 argc,
                                      const char       ** argv,
                                      const char        * json,
                                      const char        * commands,
                                      const char        * output,
                                      int                 debug,
                                      oyjlUi_s          * ui,
                                      int               (*callback)(int argc, const char ** argv));
static int (*oyjlArgsRender_p)       ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv)) = NULL;
int oyjl_args_render_init_ = 0;
static int oyjlArgsRendererLoad_( const char * render_lib )
{
  const char * name = render_lib;
  char * fn = oyjlLibNameCreate_(name, 1);
  int error = -1;

#ifdef HAVE_DL
#if 0 // keep nested calls alive
  if(oyjl_args_render_lib_)
    dlclose( oyjl_args_render_lib_ );
#endif
  oyjl_args_render_lib_ = NULL;
  oyjlArgsRender_p = NULL;
  oyjl_args_render_lib_ = dlopen(fn, RTLD_LAZY);
  if(!oyjl_args_render_lib_)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "not existent: %s", OYJL_DBG_ARGS, fn );
    error = 1;
  }
  else
  {
    char * func = oyjlFuncNameCreate_(name);
    oyjlArgsRender_p = (oyjlArgsRender_f)dlsym( oyjl_args_render_lib_, func );
    if(oyjlArgsRender_p)
      error = 0; /* found */
    else
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s: %s", OYJL_DBG_ARGS, func, dlerror() );
    if(func) free(func);
  }
#else
  oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no dlopen() API available for %s", OYJL_DBG_ARGS, fn );
  error = 1;
#endif
  free(fn);

  return error;
}

static int oyjlArgsRendererSelect_  (  oyjlUi_s          * ui )
{
  const char * name = NULL;
  char * arg;
  oyjlOption_s * R;
  int error = 0;

  if( !ui )
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"ui\" argument passed in", OYJL_DBG_ARGS );
    return 1;
  }

  R = oyjlOptions_GetOptionL( ui->opts, "R", 0 );
  if(!R)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" argument found: Can not select", OYJL_DBG_ARGS );
    return 1;
  }

  if(R->variable_type != oyjlSTRING)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable declared", OYJL_DBG_ARGS );
    return 1;
  }

  arg = oyjlStringCopy( *R->variable.s, NULL );
  if(!arg)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable found", OYJL_DBG_ARGS );
    return 1;
  }
  else
  {
    if(arg[0])
    {
      char * low = oyjlStringToLower_( arg );
      if(low)
      {
        if(strlen(low) >= strlen("gui") && memcmp("gui",low,strlen("gui")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("qml") && memcmp("qml",low,strlen("qml")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("cli") && memcmp("cli",low,strlen("cli")) == 0)
          name = "OyjlArgsCli";
        else
        if(strlen(low) >= strlen("web") && memcmp("web",low,strlen("web")) == 0)
          name = "OyjlArgsWeb";
        if(!name)
        {
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,oyjlTermColor(oyjlBOLD,low) );
          free(low);
          return 1;
        }
        free(low);
        error = oyjlArgsRendererLoad_( name );
      }
    }
    else /* report all available renderers */
    {
      if(oyjlArgsRendererLoad_( "OyjlArgsQml" ) == 0)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsQml found - option -R=\"gui\"", OYJL_DBG_ARGS );
      if(oyjlArgsRendererLoad_( "OyjlArgsCli" ) == 0)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsCli found - option -R=\"cli\"", OYJL_DBG_ARGS );
      if(oyjlArgsRendererLoad_( "OyjlArgsWeb" ) == 0)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsWeb found - option -R=\"web\"", OYJL_DBG_ARGS );
    }
  }

  if(!oyjl_args_render_init_)
  {
    char * fn = oyjlLibNameCreate_(name, 1);
    ++oyjl_args_render_init_;

#ifdef HAVE_DL
#else
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no dlopen() API available for %s", OYJL_DBG_ARGS, fn );
#endif

    free(fn);
  }

  if(arg) free(arg);

  return error;
}


/** \addtogroup oyjl_args
 *  @{ *//* oyjl_args */
/** @brief Load renderer for graphical rendering options
 *
 *  The function version in libOyjlCore dynamicaly dlopen() s libOyjlArgsQml.
 *  The function version in liboyjl-core-static needs as well
 *  liboyjl-args-qml-static to work. You should check wich library is linked
 *  and possibly guard the according code in case no QML library is available:
 *  @code
    #if !defined(NO_OYJL_ARGS_RENDER)
    if(render && ui)
      // code with oyjlArgsRender() goes here
    #endif
    @endcode
 *
 *  @param[in]     argc                number of arguments from main()
 *  @param[in]     argv                arguments from main()
 *  @param[in]     json                JSON UI text; optional, can be generated from ui
 *  @param[in]     commands            JSON commands/config text; optional, can be generated from ui or passed in as --render="XXX" option
 *  @param[in]     output              write ui interaction results; optional
 *  @param[in]     debug               set debug level
 *  @param[in]     ui                  user interface structure
 *  @param[in,out] callback            the function resembling main() to call into;
 *                                     It will be used to parse args, show help texts,
 *                                     all options handling and data processing
 *  @return                            return value for main()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/03/05
 *  @since   2019/12/14 (Oyjl: 1.0.0)
 */
int oyjlArgsRender                   ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
  int result = -1;
  oyjlArgsRendererSelect_(ui);
  if(oyjlArgsRender_p)
    result = oyjlArgsRender_p(argc, argv, json, commands, output, debug, ui, callback );
  fflush(stdout);
  fflush(stderr);
  return result;
}
/** @} *//* oyjl_args */
#ifdef __cplusplus
} /* extern "C" */
#endif


/* --- Render_Section --- */
