/** @file oyranos.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#include "oyranos_core.h" /* define HAVE_POSIX */

#include <sys/stat.h>
#ifdef HAVE_POSIX
#include <unistd.h>
#include <langinfo.h>
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"


static oyStruct_RegisterStaticMessageFunc_f * oy_static_msg_funcs_ = 0;
static int oy_msg_func_n_ = 0;

/** @func    oyStruct_RegisterStaticMessageFunc
 *  @brief   register a function for verbosity
 *
 *  @param[in]     type                the object oyOBJECT_e type 
 *  @param[in]     f                   the object string function
 *  @return                            0 - success; >= 1 - error
 *
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/14
 *  @date    2011/01/14
 */
int oyStruct_RegisterStaticMessageFunc (
                                       int                 type,
                                       oyStruct_RegisterStaticMessageFunc_f f )
{
  int error = 0;
  if((int)type >= oy_msg_func_n_)
  {
    int n = oy_msg_func_n_;
    oyStruct_RegisterStaticMessageFunc_f * tmp = 0;

    if(oy_msg_func_n_)
      n *= 2;
    else
      n = (int) oyOBJECT_MAX;


    tmp = oyAllocateFunc_(sizeof(oyStruct_RegisterStaticMessageFunc_f) * n);
    if(tmp && oy_msg_func_n_)
      memcpy( tmp, oy_static_msg_funcs_, sizeof(oyStruct_RegisterStaticMessageFunc_f) * oy_msg_func_n_ );
    else if(!tmp)
    {
      error = 1;
      return error;
    }

    oyDeAllocateFunc_(oy_static_msg_funcs_);
    oy_static_msg_funcs_ = tmp;
    tmp = 0;
    oy_static_msg_funcs_[type] = f;
    oy_msg_func_n_ = n;
  }
  return error;
}
                                       
/** @func    oyStruct_GetInfo
 *  @brief   get a additional string from a object
 *
 *  The content can be provided by object authors by using
 *  oyStruct_RegisterStaticMessageFunc() typical at the first time of object
 *  creation.
 *
 *  @param[in]     context             the object to get informations about
 *  @param[in]     flags               currently not used
 *  @return                            a string or NULL; The pointer might
 *                                     become invalid after further using the
 *                                     object pointed to by context.
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/15
 *  @date    2011/01/15
 */
const char *   oyStruct_GetInfo      ( oyStruct_s        * context,
                                       int                 flags )
{
  const char * text = 0;

  if(oy_static_msg_funcs_ && oy_static_msg_funcs_[context->type_])
    text = oy_static_msg_funcs_[context->type_]( (oyStruct_s*)context, 0 );
  if(!text)
    text = oyStructTypeToText( context->type_ );

  return text;
}

/** @func    oyMessageFormat
 *  @brief   default function to form a message string
 *
 *  This default message function is used as a message formatter.
 *  The resulting string can be placed anywhere, e.g. in a GUI.
 *
 *  @see the oyMessageFunc_() needs just to replaxe the fprintf with your 
 *  favourite GUI call.
 *
 *  @version Oyranos: 0.2.1
 *  @since   2008/04/03 (Oyranos: 0.2.1)
 *  @date    2011/01/15
 */
int                oyMessageFormat   ( char             ** message_text,
                                       int                 code,
                                       const oyStruct_s  * context,
                                       const char        * string )
{
  char * text = 0, * t = 0;
  int i;
  const char * type_name = "";
  int id = -1;
#ifdef HAVE_POSIX
  pid_t pid = 0;
#else
  int pid = 0;
#endif
  FILE * fp = 0;
  const char * id_text = 0;
  char * id_text_tmp = 0;

  if(code == oyMSG_DBG && !oy_debug)
    return 0;

  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
    id_text = oyStruct_GetInfo( (oyStruct_s*)context, 0 );
    if(id_text)
      id_text_tmp = strdup(id_text);
    id_text = id_text_tmp;
  }

  text = calloc( sizeof(char), 256 );

# define MAX_LEVEL 20
  if(level_PROG < 0)
    level_PROG = 0;
  if(level_PROG > MAX_LEVEL)
    level_PROG = MAX_LEVEL;
  for (i = 0; i < level_PROG; i++)
    oySprintf_( &text[oyStrlen_(text)], " ");

  STRING_ADD( t, text );

  text[0] = 0;

  switch(code)
  {
    case oyMSG_WARN:
         STRING_ADD( t, _("WARNING") );
         break;
    case oyMSG_ERROR:
         STRING_ADD( t, _("!!! ERROR"));
         break;
  }

  /* reduce output for non core messages */
  if( id > 0 || (oyMSG_ERROR <= code && code <= 399) )
  {
    oyStringAddPrintf_( &t, oyAllocateFunc_,oyDeAllocateFunc_,
                        " %03f: ", DBG_UHR_);
    oyStringAddPrintf_( &t, oyAllocateFunc_,oyDeAllocateFunc_,
                        "%s[%d]%s%s%s ", type_name, id,
             id_text ? "=\"" : "", id_text ? id_text : "", id_text ? "\"" : "");
  }

  STRING_ADD( t, string );

  if(oy_backtrace)
  {
#   define TMP_FILE "/tmp/oyranos_gdb_temp." OYRANOS_VERSION_NAME "txt"
#ifdef HAVE_POSIX
    pid = (int)getpid();
#endif
    fp = fopen( TMP_FILE, "w" );

    if(fp)
    {
      fprintf(fp, "attach %d\n", pid);
      fprintf(fp, "thread 1\nbacktrace\n"/*thread 2\nbacktrace\nthread 3\nbacktrace\n*/"detach" );
      fclose(fp);
      fprintf( stderr, "GDB output:\n" );
      system("gdb -batch -x " TMP_FILE);
    } else
      fprintf( stderr, "could not open " TMP_FILE "\n" );
  }

  free( text ); text = 0;
  if(id_text_tmp) free(id_text_tmp); id_text_tmp = 0;

  *message_text = t;

  return 0;
}

/** @func    oyMessageFunc_
 *  @brief   default message function to console
 *
 *  The default message function is used as a message printer to the console 
 *  from library start.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/04/03 (Oyranos: 0.1.8)
 *  @date    2009/07/20
 */
int oyMessageFunc_( int code, const oyStruct_s * context, const char * format, ... )
{
  char * text = 0, * msg = 0;
  int error = 0;
  va_list list;
  size_t sz = 256;
  int len;

  text = calloc( sizeof(char), sz );
  if(!text)
  {
    fprintf(stderr,
    "oyranos_core.c:257 oyMessageFunc_() Could not allocate 256 byte of memory.\n");
    return 1;
  }

  text[0] = 0;

  va_start( list, format);
  len = vsnprintf( text, sz-1, format, list);
  va_end  ( list );

  if (len >= ((int)sz - 1))
  {
    text = realloc( text, (len+2)*sizeof(char) );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  error = oyMessageFormat( &msg, code, context, text );

  fprintf( stderr, "%s\n", msg );
  free( text ); text = 0;
  free( msg ); msg = 0;

  return error;
}


oyMessage_f     oyMessageFunc_p = oyMessageFunc_;

/** @func    oyMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/04/03
 *  @since   2008/04/03 (Oyranos: 0.1.8)
 */
int            oyMessageFuncSet      ( oyMessage_f         message_func )
{
  if(message_func)
    oyMessageFunc_p = message_func;
  return 0;
}


/* --- internal API decoupling --- */



/** \addtogroup misc Miscellaneous
 *  Miscellaneous stuff.

 *  @{
 */

/** @brief  switch internationalisation of strings on or off
 *
 *  @param  active         bool
 *  @param  reserved       for future use
 */
void 
oyI18NSet              ( int active,
                         int reserved )
{
  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  oyI18NSet_(active, reserved);

  oyExportEnd_();
  DBG_PROG_ENDE
}

/** @brief  get language code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLanguage            ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  text = oyLanguage_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief  get country code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyCountry             ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  text = oyCountry_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief  get LANG code/variable
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLang                ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  text = oyLang_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief   reset i18n language and  country variables
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/05 (Oyranos: 0.1.10)
 *  @date    2009/01/05
 */
void           oyI18Nreset           ( void )
{
  DBG_PROG_START
  oyExportReset_(EXPORT_I18N);
  oyExportStart_(EXPORT_CHECK_NO);
  oyExportEnd_();
  DBG_PROG_ENDE
}


/** @brief  give the compiled in library version
 *
 *  @param[in]  type           0 - Oyranos API
 *                             1 - start month
 *                             2 - start year
 *                             3 - development last month
 *                             4 - development last year
 *
 *  @return                    OYRANOS_VERSION at library compile time
 */
int            oyVersion             ( int                 type )
{
  if(type == 1)
    return OYRANOS_START_MONTH;
  if(type == 2)
    return OYRANOS_START_YEAR;
  if(type == 3)
    return OYRANOS_DEVEL_MONTH;
  if(type == 4)
    return OYRANOS_DEVEL_YEAR;

  return OYRANOS_VERSION;
}

#include "config.log.h"
/** @brief  give the configure options for Oyranos
 *
 *  @param[in] type
                               - 1  OYRANOS_VERSION_NAME;
                               - 2  git master hash;
                               - 3  OYRANOS_CONFIG_DATE,
                               - 4  development period
 *  @param     allocateFunc    user allocator, e.g. malloc
 *
 *  @return                    Oyranos configure output
 *
 *  @since     Oyranos: version 0.1.8
 *  @date      18 december 2007 (API 0.1.8)
 */
char *       oyVersionString         ( int                 type,
                                       oyAlloc_f           allocateFunc )
{
  char * text = 0, * tmp = 0;
  char temp[24];
  char * git = OYRANOS_GIT_MASTER;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(type == 1)
    return oyStringCopy_(OYRANOS_VERSION_NAME, allocateFunc);
  if(type == 2)
  {
    if(git[0])
      return oyStringCopy_(git, allocateFunc);
    else
      return 0;
  }
  if(type == 3)
    return oyStringCopy_(OYRANOS_CONFIG_DATE, allocateFunc);

  if(type == 4)
  {
#ifdef HAVE_POSIX
    oyStringAdd_( &text, nl_langinfo(MON_1-1+oyVersion(1)),
                                            oyAllocateFunc_, oyDeAllocateFunc_);
#endif
    oySprintf_( temp, " %d - ", oyVersion(2) );
    oyStringAdd_( &text, temp, oyAllocateFunc_, oyDeAllocateFunc_);
#ifdef HAVE_POSIX
    oyStringAdd_( &text, nl_langinfo(MON_1-1+oyVersion(3)),
                                            oyAllocateFunc_, oyDeAllocateFunc_);
#endif
    oySprintf_( temp, " %d", oyVersion(4) );
    oyStringAdd_( &text, temp, oyAllocateFunc_, oyDeAllocateFunc_);

    tmp = oyStringCopy_( text , allocateFunc);
    oyDeAllocateFunc_(text);
    return tmp;
  }

#ifdef HAVE_POSIX
  return oyStringCopy_(oy_config_log_, allocateFunc);
#else
  return oyStringCopy_("----", allocateFunc);
#endif
}


/** @} */



