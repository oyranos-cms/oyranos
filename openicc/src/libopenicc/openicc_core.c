/*  @file openicc_core.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

#include "openicc_config_internal.h"
#include "oyjl_macros.h"

int openicc_debug_local = 0;
int * openicc_debug = &openicc_debug_local;
#ifndef HAVE_OPENICC
#define level_PROG openicc_level_PROG
int level_PROG = 0;
#endif
int openicc_backtrace = 0;

#if HAVE_POSIX
#include <unistd.h>  /* getpid() */
#endif
#include <string.h>  /* strdup() */
#include <stdarg.h>  /* vsnprintf() */
#include <stdio.h>   /* vsnprintf() */


/** \addtogroup misc
 *  @{
 */

void           openiccSetDebugVariable(int               * cmm_debug ) { openicc_debug = cmm_debug; }

openiccOBJECT_e    openiccObjectToType(const void        * contextObject )
{
  struct { openiccOBJECT_e type; } const * x = contextObject;
  if(x)
    return x->type;
  return openiccOBJECT_NONE;
}

const char *       openiccObjectTypeToString (
                                       openiccOBJECT_e     type )
{
  const char * type_name = "unknown";
  switch(type)
  {
    case openiccOBJECT_NONE: type_name = ""; break;
    case openiccOBJECT_CONFIG: type_name = "openiccConfig_s"; break;
    case openiccOBJECT_DB: type_name = "openiccDB_s"; break;
    case openiccOBJECT_OPTION: type_name = "openiccOption_s"; break;
    case openiccOBJECT_OPTION_GROUP: type_name = "openiccOptionGroup_s"; break;
    case openiccOBJECT_OPTIONS: type_name = "openiccOptions_s"; break;
    case openiccOBJECT_UI_HEADER_SECTION: type_name = "openiccUiHeaderSection_s"; break;
    case openiccOBJECT_UI: type_name = "openiccUi_s"; break;
  }
  return type_name;
}

/** @brief   default function to form a message string
 *
 *  This default message function is used as a message formatter.
 *  The resulting string can be placed anywhere, e.g. in a GUI.
 *
 *  @see the openiccMessageFunc() needs just to replace the fprintf with your 
 *  favourite GUI call.
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/01/15
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int                openiccMessageFormat (
                                       char             ** message_text,
                                       int                 code,
                                       const void        * context_object,
                                       const char        * string )
{
  char * text = 0, * t = 0;
  int i;
  openiccOBJECT_e type = openiccObjectToType( context_object );
  const char * type_name = openiccObjectTypeToString( type );
#ifdef HAVE_POSIX
  pid_t pid = 0;
#else
  int pid = 0;
#endif
  FILE * fp = 0;
  const char * id_text = 0;
  char * id_text_tmp = 0;
  openiccConfig_s * c = NULL;

  if(code == openiccMSG_DBG && !*openicc_debug)
    return 0;

  if(type == openiccOBJECT_CONFIG)
    c = (openiccConfig_s*) context_object;

  if(c)
  {
    id_text = c->info;
    if(id_text)
      id_text_tmp = strdup(id_text);
    id_text = id_text_tmp;
  }

  oyjlAllocHelper_m(text, char, 256, malloc, if(id_text_tmp) free(id_text_tmp); return 1);

# define MAX_LEVEL 20
  if(level_PROG < 0)
    level_PROG = 0;
  if(level_PROG > MAX_LEVEL)
    level_PROG = MAX_LEVEL;
  for (i = 0; i < level_PROG; i++)
    sprintf( &text[strlen(text)], " ");

  STRING_ADD( t, text );

  text[0] = 0;

  switch(code)
  {
    case openiccMSG_WARN:
         STRING_ADD( t, _("WARNING") );
         break;
    case openiccMSG_ERROR:
         STRING_ADD( t, _("!!! ERROR"));
         break;
  }

  /* reduce output for non core messages */
  if( (openiccMSG_ERROR <= code && code <= 399) )
  {
    oyjlStringAdd( &t, 0,0,
                        " %03f: ", DBG_UHR_);
    oyjlStringAdd( &t, 0,0,
                        "%s%s%s%s ", type_name,
             id_text ? "=\"" : "", id_text ? id_text : "", id_text ? "\"" : "");
  }

  STRING_ADD( t, string );

  if(openicc_backtrace)
  {
#   define TMP_FILE "/tmp/openicc_gdb_temp." OPENICC_VERSION_NAME "txt"
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
      i = system("gdb -batch -x " TMP_FILE);
    } else
      fprintf(stderr,
      OI_DBG_FORMAT_"Could not open " TMP_FILE "\n",OI_DBG_ARGS_);
  }

  free( text ); text = NULL;
  if(id_text_tmp) {free(id_text_tmp); id_text_tmp = 0;}

  *message_text = t;

  return 0;
}

/** @brief   default message function to console
 *
 *  The default message function is used as a message printer to the console 
 *  from library start.
 *
 *  @param         code                a message code understood be your message
 *                                     handler or openiccMSG_e
 *  @param         context_object      a openicc object is expected
 *  @param         format              the text format string for following args
 *  @param         ...                 the variable args fitting to format
 *  @return                            0 - success; 1 - error
 *
 *  @version OpenICC: 0.1.0
 *  @date    2009/07/20
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int  openiccMessageFunc              ( int/*openiccMSG_e*/ code,
                                       const void        * context_object,
                                       const char        * format,
                                       ... )
{
  char * text = 0, * msg = 0;
  int error = 0;
  va_list list;
  size_t sz = 0;
  int len = 0;
  const openiccConfig_s * c = context_object;


  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  {
    text = calloc( sizeof(char), len+2 );
    if(!text)
    {
      fprintf(stderr,
      OI_DBG_FORMAT_"Could not allocate 256 byte of memory.\n",OI_DBG_ARGS_);
      return 1;
    }
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  error = openiccMessageFormat( &msg, code, c, text );

  if(msg)
    fprintf( stderr, "%s\n", msg );
  else if(error)
    fprintf( stderr, "%s\n", format );

  free( text ); text = 0;
  free( msg ); msg = 0;

  return error;
}

openiccMessage_f     openiccMessage_p = openiccMessageFunc;

/** @brief   set a custom message listener
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int            openiccMessageFuncSet ( openiccMessage_f    message_func )
{
  if(message_func)
    openiccMessage_p = message_func;
  return 0;
}

/** @brief   runtime version
 *
 *  @version OpenICC: 0.1.0
 *  @date    2015/08/27
 *  @since   2015/08/27 (OpenICC: 0.1.0)
 */
int            openiccVersion        ( void )
{
  return OPENICC_VERSION;
}

const char * openicc_domain_path = OPENICC_LOCALEDIR;
int openicc_i18n_init = 0;
/** @brief   init the library; optionally
 *
 *  Additionally use setlocale() somewhere in your application.
 *  The message catalog search path is detected from the OI_LOCALEDIR and
 *  the LOCPATH environment variables. If those are not present;
 *  the inbuild ::OPENICC_LOCALEDIR macro is used.
 *
 *  return -1 for no USE_GETTEXT defined, otherwise 1
 *
 *  @version OpenICC: 0.1.0
 *  @date    2021/11/11
 *  @since   2015/08/27 (OpenICC: 0.1.0)
 */
int            openiccInit           ( const char        * loc )
{
  int use_gettext = 0;
  if(!openicc_i18n_init)
  {
    oyjlTranslation_s * trc = NULL;
    if(loc)
      trc = oyjlTranslation_New( loc, "openicc", 0,0,0,0,0 );
#ifdef USE_GETTEXT
    use_gettext = 1;
#endif
    ++openicc_i18n_init;
    oyjlInitLanguageDebug( "OpenICC", OI_DEBUG, openicc_debug, use_gettext, "OI_LOCALEDIR", OPENICC_LOCALEDIR, &trc, openiccMessage_p );
    oyjlTranslation_Release( &trc );
  }

  if(getenv(OI_DEBUG))
  {
    int v = openiccVersion();
    *openicc_debug = atoi(getenv(OI_DEBUG));
    if(*openicc_debug)
      DBGc_S( "OpenICC v%s config: %d", OPENICC_VERSION_NAME, v );
  }

  if(use_gettext)
    return openicc_i18n_init;
  else
    return -1;
}

/** @brief   return the malloced copy of a pointer
 *
 *  @version OpenICC: 0.1.1
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void *         openiccMemDup         ( const void        * src,
                                       size_t              size )
{
  void * dest = malloc(size);
  if(dest)
    memcpy( dest, src, size );
  return dest;
}

/*  @} *//* misc */

/* private stuff */

oyjlUiHeaderSection_s * oiUiInfo     ( const char          * documentation )
{
  oyjlUiHeaderSection_s s[] = {
    /* type,  nick,      label,name,                 description */
    { "oihs", "version", NULL, OPENICC_VERSION_NAME, NULL },
    { "oihs", "manufacturer", NULL, "Kai-Uwe Behrmann", "http://www.openicc.org" },
    { "oihs", "copyright", NULL, "Copyright 2018 Kai-Uwe Behrmann", NULL },
    { "oihs", "license", NULL, "newBSD", "http://www.openicc.org" },
    { "oihs", "url",     NULL, "http://www.openicc.org", NULL },
    { "oihs", "support", NULL, "https://www.github.com/OpenICC/config/issues", NULL },
    { "oihs", "download",NULL, "https://github.com/OpenICC/config/releases", NULL },
    { "oihs", "sources", NULL, "https://github.com/OpenICC/config", NULL },
    { "oihs", "development", NULL, "https://github.com/OpenICC/config", NULL },
    { "oihs", "openicc_module_author", NULL, "Kai-Uwe Behrmann", "http://www.behrmann.name" },
    { "oihs", "documentation", NULL, "http://www.openicc.info", documentation },
    { "oihs", "date", NULL, "1970-01-01T12:00:00", "" },
    { "", NULL, NULL, NULL, NULL }
  };
  return (oyjlUiHeaderSection_s*) oyjlStringAppendN( NULL, (const char*)s, sizeof(s), malloc );
}


