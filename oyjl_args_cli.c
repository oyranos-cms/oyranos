/** @file oyjl_args_cli.c
 *
 *  Oyjl JSON Clil is a command line renderer of UI files.
 *
 *  @par Copyright:
 *            2021 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2021/07/04
 *
 *  basic Cli handling for the -R=cli option
 */

#include <stdlib.h> // free()
#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_i18n.h"

void oyjlArgsCliGroupPrintSection_   ( oyjl_val            g,
                                       int                 level )
{
  oyjl_val v;

  const char * gdesc, * gname, * ghelp;
  v = oyjlTreeGetValue(g, 0, "name");
  gname = OYJL_GET_STRING(v);
  v = oyjlTreeGetValue(g, 0, "description");
  gdesc = OYJL_GET_STRING(v);
  v = oyjlTreeGetValue(g, 0, "help");
  ghelp = OYJL_GET_STRING(v);
  if(gname || gdesc || ghelp)
    printf( "  %s", level?"  ":"" );
  if(gname)
    printf( "%s", oyjlTermColor(oyjlUNDERLINE,gname) );
  if(gdesc)
    printf( " %s", oyjlTermColor(oyjlUNDERLINE,gdesc) );
  if(ghelp)
    printf( "\n%s%s\n", level?"      ":"    ", ghelp );
  if(gname || gdesc || ghelp)
    printf( "\n" );
}

void oyjlArgsCliGroupPrint_          ( oyjl_val            g )
{
  oyjl_val v;
  const char * txt;

  int j, count;
  v = oyjlTreeGetValue(g, 0, "options");
  count = oyjlValueCount( v );
  for(j = 0; j < count; ++j)
  {
    int k, kn = 0;
    const char * key, * name, * desc, * help, * value_name/*, * default_var*/, * type, * no_dash;
    char * text;
    oyjl_val opt = oyjlTreeGetValueF(v, 0, "[%d]", j);
    oyjl_val o = oyjlTreeGetValue(opt, 0, "option");
    oyjl_val choices;
    txt = OYJL_GET_STRING(o);
    if(txt && strlen(txt) > 4 && memcmp( txt, "man-", 4 ) == 0)
      continue;
    o = oyjlTreeGetValue(opt, 0, "key");
    key = OYJL_GET_STRING(o);
    o = oyjlTreeGetValue(opt, 0, "name");
    name = OYJL_GET_STRING(o);
    o = oyjlTreeGetValue(opt, 0, "description");
    desc = OYJL_GET_STRING(o);
    o = oyjlTreeGetValue(opt, 0, "help");
    help = OYJL_GET_STRING(o);
    o = oyjlTreeGetValue(opt, 0, "value_name");
    value_name = OYJL_GET_STRING(o);
    if(value_name) text = oyjlStringCopy(oyjlTermColor(oyjlITALIC,value_name),0); else text = NULL;
    /*o = oyjlTreeGetValue(opt, 0, "default");
    default_var = OYJL_GET_STRING(o);*/
    o = oyjlTreeGetValue(opt, 0, "type");
    type = OYJL_GET_STRING(o);
    o = oyjlTreeGetValue(opt, 0, "no_dash");
    no_dash = OYJL_GET_STRING(o);
    if(key[0] == '@')
      no_dash = "1";
    printf( "      %s%s%s%s%s%s%s%s%s%s\n",
        no_dash?"":strlen(key) == 1?"-":"--", key[0] == '@'?"":oyjlTermColor(oyjlBOLD,key),
        value_name && key[0] != '@'?"=":"", value_name?text:"",
        name?"\t":"", name?name:"",
        desc?" : ":"", desc?desc:"",
        help&&help[0]?"\n          ":"", help&&help[0]?help:"" );
    if(text) { free(text); text = NULL; }
    choices = oyjlTreeGetValue(opt, 0, "choices");
    if(type && strcmp(type,"bool") != 0)
      kn = oyjlValueCount( choices );
    for(k = 0; k < kn; ++k)
    {
      oyjl_val c = oyjlTreeGetValueF(choices, 0, "[%d]", k);
      oyjl_val cv;
      const char * nick;
      cv = oyjlTreeGetValue(c, 0, "name");
      name = OYJL_GET_STRING(cv);
      cv = oyjlTreeGetValue(c, 0, "nick");
      nick = OYJL_GET_STRING(cv);
      cv = oyjlTreeGetValue(c, 0, "description");
      desc = OYJL_GET_STRING(cv);
      cv = oyjlTreeGetValue(c, 0, "help");
      help = OYJL_GET_STRING(cv);
      printf( "          %s%s%s%s%s%s%s%s%s%s\n", no_dash?"":strlen(key) == 1?"-":"--", key[0] != '@'?key:"",
        nick && key[0] != '@'?" ":"", nick?nick:"",
        name&&name[0]?"\t\t# ":"", name?name:"",
        desc?" : ":"", desc&&desc[0]?desc:"",
        help&&help[0]?" - ":"", help&&help[0]?help:"" );
    }
  }
}

int oyjlArgsCliStart__               ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands OYJL_UNUSED,
                                       const char        * output OYJL_UNUSED,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv) OYJL_UNUSED)
{
  char * input = NULL;

  if(debug)
  {
    for(int i = 0; i < argc; ++i)
      fprintf( stderr, "%s ", argv[i] );
    fprintf( stderr, "\n" );
  }

  oyjl_val root = NULL;
  char error_buffer[256] = {0};
  int r = 0;
  if( json && strlen( json ) )
  {
    r = oyjlIsFile( json, "r", NULL, 0 );
    if(!r && oyjlDataFormat(json) == 7)
    {
      root = oyjlTreeParse( json, error_buffer, 256 );
      if(error_buffer[0] != '\000')
      {
        fprintf(stderr, "ERROR:\t\"%s\"\n", error_buffer);
        char * error = NULL;
        oyjlStringAdd( &error, 0,0, "{\"error\": \"%s\"}", json );
        json = error;
        r = -1;
      }
    }
  }

  if( (root && oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules")) || // use UI JSON
      (!root && json && strlen(json)) ) // assume JSON filename
  {
    if(!root && json && strlen(json))
    {
      if(r)
      {
        if(debug)
          fprintf( stderr, "Found file name: %s\n", json );
      }
      else
        fprintf( stderr, "Assume file name or stream: %s\n", json );
    } else
      if(debug)
        fprintf( stderr, "Found Json org/freedesktop/oyjl/modules: %lu\n", strlen(json) );
    //mgr.setUri( QString(json) );
    input = oyjlStringCopy( json, 0 );
  }
  else
  {
    json = oyjlUi_ToJson( ui, 0 ); // generate JSON from ui data struct
    char * merged = NULL;
    if(debug)
      fprintf( stderr, "oyjlUi_ToJson(): %lu\n", json?strlen(json):0);
    if(root && json)
    {
      oyjl_val module = oyjlTreeParse( json, error_buffer, 256 );
      oyjl_val rv = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/translations");
      oyjl_val mv = oyjlTreeGetValue(module, OYJL_CREATE_NEW, "org/freedesktop/oyjl/translations");
      if(rv && mv) // merge in translations
      {
        size_t size = sizeof(*rv);
        memcpy( mv, rv, size );
        memset( rv, 0, size );
        int level = 0;
        oyjlTreeToJson( module, &level, &merged );
        if(debug)
          fprintf( stderr, "merge UI JSON with translation\n");
      } else
        fprintf( stderr, "expected translation is missing\n");
      oyjlTreeFree( module );
    }
    if(!merged && json)
    {
      merged = oyjlStringCopy( json, NULL );
      if(debug)
        fprintf( stderr, "use generated UI JSON\n");
    }
    input = oyjlStringCopy( merged, 0 );
    if(merged)
      free(merged);
  }

  if(root) { oyjlTreeFree( root ); root = NULL; }

  if(input)
  {
    if(oyjlDataFormat(input) == 7)
    {
      root = oyjlTreeParse( input, error_buffer, 256 );
      if(error_buffer[0] != '\000')
      {
        fprintf(stderr, "ERROR:\t\"%s\"\n", error_buffer);
        char * error = NULL;
        oyjlStringAdd( &error, 0,0, "{\"error\": \"%s\"}", input );
        input = error;
        r = -1;
      }
    }
    if(debug)
      fprintf( stderr, "input: %s\n", input );
  }

  if(root)
  {
    oyjl_val v = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules/[0]"); // use UI JSON
    oyjl_val val = oyjlTreeGetValue(v, 0, "nick");
    const char * nick = OYJL_GET_STRING(val),
               * desc = NULL, * version = NULL, * docu = NULL, * txt;
    int i = 0, n;
    const char * key_list = "name,description,help,label";
    const char * lang = oyjlLang("");
    if(debug)
      fprintf( stderr, "using lang: %s\n", lang );
    oyjlTranslateJson( root, NULL, key_list );

    val = oyjlTreeGetValue(v, 0, "description");
    desc = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue(v, 0, "information");
    n = oyjlValueCount( val );
    for(i = 0; i < n; ++i)
    {
      oyjl_val info = oyjlTreeGetValueF(val, 0, "[%d]/type", i);
      txt = OYJL_GET_STRING(info);
      if(txt && strcmp(txt,"version") == 0)
      {
        info = oyjlTreeGetValueF(val, 0, "[%d]/name", i);
        version = OYJL_GET_STRING(info);
      }
      if(txt && strcmp(txt,"documentation") == 0)
      {
        info = oyjlTreeGetValueF(val, 0, "[%d]/description", i);
        docu = OYJL_GET_STRING(info);
      }
    }
    printf( "%s%s%s - %s\n", nick?oyjlTermColor(oyjlBOLD,nick):v?val?"found nick node":"found modules node":"----", version?" v":"", version?version:"", desc );
    if(docu)
    {
      printf( "\n%s:\n  %s\n\n", oyjlTermColor(oyjlBOLD,_("Description")), docu );
    }
    val = oyjlTreeGetValue(v, 0, "groups");
    n = oyjlValueCount( val );
    if(n)
      printf( "%s:\n", oyjlTermColor(oyjlBOLD,_("Usage")) );
    for(i = 0; i < n; ++i)
    {
      int j, count, has_options;
      oyjl_val g = oyjlTreeGetValueF(val, 0, "[%d]", i);
      has_options = oyjlTreeGetValue(g, 0, "options") == 0?0:1;
      v = oyjlTreeGetValue(g, 0, "groups");
      count = oyjlValueCount( v );
      if(count || has_options)
        oyjlArgsCliGroupPrintSection_(g, 0);
      if(has_options)
        oyjlArgsCliGroupPrint_(g);
      for(j = 0; j < count; ++j)
      {
        g = oyjlTreeGetValueF(v, 0, "[%d]", j);
        if(oyjlTreeGetValue(g, 0, "options"))
        {
          oyjlArgsCliGroupPrintSection_(g, 1);
          oyjlArgsCliGroupPrint_(g);
        }
      }
      if(count || has_options)
        printf( "\n" );
    }
    if(n)
      printf( "\n" );
  }

  return 0;
}


#ifdef __cplusplus
extern "C" {
#endif
// internal API entry for public API in libOyjlCore
int oyjlArgsCli_                     ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
    int r = oyjlArgsCliStart__(argc, argv, json, commands, output, debug, ui, callback );
    return r;
}

#ifdef COMPILE_STATIC
#include "oyjl_tree_internal.h" /* oyjlStringToLower() */
static int oyjlArgsRendererSelect   (  oyjlUi_s          * ui )
{
  const char * arg = NULL, * name = NULL;
  oyjlOption_s * R;
  int error = -1;

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
      char * low = oyjlStringToLower( arg );
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
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,low );
          free(low);
          return 1;
        }
        if(strcmp(name,"OyjlArgsCli") == 0)
          error = 0;
        else
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,low );
        free(low);
      }
    }
    else /* report all available renderers */
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsCli available - option -R=\"cli\"", OYJL_DBG_ARGS );
    }
  }

  return error;
}
// public API for liboyjl-args-cli-static.a
int oyjlArgsRender                   ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
  int result = 1;
  if(oyjlArgsRendererSelect(ui) == 0)
    result = oyjlArgsCli_(argc, argv, json, commands, output, debug, ui, callback );
  fflush(stdout);
  fflush(stderr);
  return result;
}

#endif /* COMPILE_STATIC */

#ifdef __cplusplus
} /* extern "C" */
#endif
