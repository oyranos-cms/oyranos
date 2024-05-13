/*  @file oyjl_translate.c
 *
 *  libOyjl - JSON helper tool
 *
 *  @par Copyright:
 *            2018-2024 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl JSON translation helper
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/01/06
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "oyjl_version.h"
#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_i18n_internal.h"
#include "oyjl_tree_internal.h"

oyjl_val oyjlTreeParsePo             ( const char        * text,
                                       const char        * input OYJL_UNUSED,
                                       int                 verbose )
{
  oyjl_val root = NULL;
  int count = 0, i, pos = 0;
  char ** blocks = oyjlStringSplit2( text, "\n\n", oyjlRegExpDelimiter, &count, NULL, NULL );
  if(count)
    root = oyjlTreeNew("!DOCTYPE TS");
  fprintf( stderr, OYJL_DBG_FORMAT "count: %d\n", OYJL_DBG_ARGS, count );
  for(i = 0; i < count; ++i)
  {
    const char * block = blocks[i];
    int fuzzy = strstr(block, "#, fuzzy") != 0 ? 1 : 0;
    if(strstr(block, "msgid") && !strstr(block, "~ msgid"))
    {
      const char * msgid = strstr( block, "msgid" );
      const char * msgstr = strstr( block, "\nmsgstr");
      const char * txt;
      char * t = msgid ? oyjlStringCopy(msgid + 5+2, 0) : NULL;
      char * tr = msgstr ? oyjlStringCopy(msgstr + 7+2, 0) : NULL;
      char * tmp, * new_key;
      if(!(t && t[0] && tr && tr[0]))
      {
        if(t) free(t);
        if(tr) free(tr);
        continue;
      }
      oyjlStringReplace( &t, "\"\n\"", "", NULL,NULL );
      oyjlStringReplace( &tr, "\"\n\"", "", NULL,NULL );
      tmp = (char*)oyjlRegExpDelimiter( t, "\"\n", 0 );
      tmp[0] = '\000';
      tmp = (char*)oyjlRegExpDelimiter( tr, "\"$", 0 );
      if(tmp)
        tmp[0] = '\000';
      if(fuzzy && verbose)
        fprintf( stderr, "%s %s: %s - %s\n", oyjlTermColorF( oyjlBLUE, "[%d]:", i ), t, tr, oyjlTermColor(oyjlITALIC, "fuzzy") );
      if(!(t[0] && tr[0]) || strcmp(t,tr) == 0 || fuzzy)
      {
        free(t); free(tr);
        continue;
      }
      oyjlStringReplace( &t, "\\", "", NULL,NULL );
      oyjlStringReplace( &tr, "\\", "", NULL,NULL );
      new_key = oyjlStringCopy( t, NULL );
      tmp = oyjlStringEscape( new_key,  0, 0 );
      free(new_key); new_key = tmp; tmp = NULL;
      oyjlTreeSetStringF( root, OYJL_CREATE_NEW, new_key,  "TS/context/message/[%d]/source", pos );
      if(verbose)
        fprintf(stderr, "[%d]: %s", i, new_key );
      txt = oyjlJsonEscape( tr, 0 );
      if(txt)
      {
        oyjlTreeSetStringF( root, OYJL_CREATE_NEW, t != tr ? txt : "", "TS/context/message/[%d]/translation", pos );
        if(verbose)
          fprintf(stderr, " %s\n", oyjlTermColor(oyjlBOLD,txt) );
      }
      ++pos;
    }
  }
  oyjlStringListRelease( &blocks, count, free );
  return root;
}

oyjl_val oyjlTreeParseXXX            ( const char        * text,
                                       const char        * input,
                                       int                 verbose )
{
  int error_buffer_size = 256;
  char * error_buffer = malloc(error_buffer_size);
  const char * ptext = oyjlTermColorToPlain( text, OYJL_REGEXP );
  char * json = oyjlStringCopy( ptext, 0 );
  int dformat = oyjlDataFormat( json );
  int size = 0;
  oyjl_val root = NULL;

  error_buffer[0] = 0;

  if(dformat == 7)
  {
    /* parse json ... */
    root = oyjlTreeParse( json, error_buffer, error_buffer_size );
  } else if (dformat == 8)
  {
#if defined(OYJL_HAVE_LIBXML2)
    root = oyjlTreeParseXml( json, 0, error_buffer, error_buffer_size );
#else
    fprintf( stderr, "ERROR: %s\n%s\n", input, "OYJL_HAVE_LIBXML2 is not supported" );
#endif
  } else if(strstr(json, "msgid") != NULL && strstr(json, "msgstr") != NULL)
  {
    free(json); json = NULL;
    if(strcmp(input,"-") == 0)
      root = oyjlTreeParsePo( text, input, verbose );
    else
    {
      root = oyjlTreeParsePo( text, input, verbose );
      if(!root)
      {
        json = oyjlReadCommandF( &size, "r", malloc, "lconvert-qt5 -locations relative %s", input );
        if(verbose)
        {
          const char * fn = "oyjl-translate-temp.ts";
          oyjlWriteFile( fn, json, strlen(json) );
          fprintf( stderr, "wrote: %s %lu\n", fn, (long)strlen(json) );
        }
        root = oyjlTreeParseXXX( json, input, verbose );
      }
    }
  }

  if(error_buffer[0])
  {
    fprintf( stderr, "%s %s\n%s\n", oyjlTermColor(oyjlRED,_("Usage Error:")), input, error_buffer );
  }

  free( error_buffer );
  free( json );

  return root;
}

int oyjlStrcmpWrap_ (const void * a_, const void * b_)
{
  const char * a = *(const char **)a_,
             * b = *(const char **)b_;
  return strcmp(a,b);
}
void oyjlTreeSortStrings             ( oyjl_val          * root,
                                       int                 verbose )
{
  int count = 0, i, r;
  oyjl_val sorted, root_ = *root;
  char ** paths = oyjlTreeToPaths( root_, 1000000, NULL, OYJL_KEY, &count );

  qsort( paths, count, sizeof(char*), oyjlStrcmpWrap_ );

  sorted = oyjlTreeNew("org/freedesktop/oyjl/translations");
  for(i = 0; i < count; ++i)
  {
    const char * path = paths[i], * t;
    t = oyjlTreeGetString_( root_, strstr(path,"cs_CZ") != NULL?OYJL_OBSERVE:0, path );
    r = oyjlTreeSetStringF( sorted, OYJL_CREATE_NEW, t, "%s", path );
    if(verbose || r)
      fprintf( stderr, OYJL_DBG_FORMAT "path[%d]: %s:%s%s\n", OYJL_DBG_ARGS, i, path, t, r?oyjlTermColorF(oyjlNO_MARK," r=%d",r):"" );
  }
  oyjlTreeFree( root_ );
  *root = sorted; sorted = NULL;
}

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  int add = 0;
  int copy = 0;
  int list_empty = 0;
  int extract = 0;
  int size = 0;
  int translations_only = 0;
  const char * output = NULL,
             * input = NULL,
             * format = NULL,
             * plain = NULL,
             * wrap = NULL,
             * function_name = NULL,
             * function_name_out = NULL,
             * key_list = "name,description,help",
             * locale = NULL,
             * locales = "cs_CZ,de_DE,eo_EO,eu_ES,fr_FR,ru_RU",
             * localedir = OYJL_LOCALEDIR,
             * domain = OYJL_DOMAIN;
  char * json = NULL;
  oyjl_val root = NULL,v;
  char * text = NULL;

  int error = 0;
  int state = 0;
  int verbose = 0;
  const char * help = NULL;
  int version = 0;
  const char * render = NULL;
  const char * export = NULL;

  /* handle options */
  /* Select a nick from *version*, *manufacturer*, *copyright*, *license*,
   * *url*, *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation*. Choose what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s * sections = oyjlUiInfo_( _("Convert Oyjl UI JSON to C translatable strings for use with gettext tools and translate a tools UI using the programs own text domain. The resulting Oyjl UI JSON can be used for translated rendering."),
                                                 "2020-01-02T12:00:00", _("January 2, 2020") );

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s f_choices[] = {{"json",        _("I18N JSON"),     _("Create translation Json with -af=json."), _("This option is useful only for smaller projects as a initial start.")},
                                    {"i18n(\\\"%s\\\");",NULL,          NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s F_choices[] = {{"_(\\\"",      NULL,               NULL,                         NULL},
                                    {"i18n(\\\"",   "",                 NULL,                         NULL},
                                    {"QObject::tr(\\\"", NULL,          NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s w_choices[] = {{"C",           _("C static char"), NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s A_choices[] = {{_("Convert JSON to gettext ready C strings"),_("oyjl-translate -e [-v] -i oyjl-ui.json -o result.json -f '_(\"%s\"); ' -k name,description,help"),NULL,                         NULL},
                                    {_("Convert C source to I18N JSON"),_("oyjl-translate -e -f=json -i project.c -o result.json"),NULL,                         NULL},
                                    {_("Add gettext translated keys to JSON"),_("oyjl-translate -a -i oyjl-ui.json -o result.json -k name,description,help -d TEXTDOMAIN -p LOCALEDIR -l de_DE,es_ES"),NULL,                         NULL},
                                    {_("Copy translated keys to JSON. Skip gettext."),_("oyjl-translate -c -i lang.tr -o result.json --locale de_DE"),NULL,                         NULL},
                                    {_("View MAN page"),_("oyjl-translate -X man | groff -T utf8 -man -"), NULL,NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s E_choices[] = {{"OUTPUT_CHARSET", _("Set the GNU gettext output encoding."),_("Alternatively use the -l=de_DE.UTF-8 option."),_("Typical value is UTF-8.")},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{"oyjl(1) oyjl-args(1) oyjl-args-qml(1)","https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html",               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "i","input",         NULL,     _("Input"),    _("File or Stream"),          _("A file name or a input stream like \"stdin\"."),_("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&input}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "o","output",        NULL,     _("Output"),   _("File or Stream"),          _("A file name or a output stream like \"stdout\"."),_("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&output}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "f","format",        NULL,     _("Format"),   _("Format string"),           _("A output format string containing a %s for replacement."),_("FORMAT"), 
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)f_choices, sizeof(f_choices), malloc ), 0}}, oyjlSTRING,    {.s=&format}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG|OYJL_OPTION_FLAG_IMMEDIATE,"P","plain",      NULL,     _("Plain"),    _("No Markup"),               NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlSTRING,    {.s=&plain}, NULL},
    {"oiwi", 0,                          "a","add",           NULL,     _("Add"),      _("Add Translation"),         _("Add gettext translated keys to JSON"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&add}, NULL},
    {"oiwi", 0,                          "c","copy",          NULL,     _("Copy"),     _("Copy Translations"),       _("Copy translated keys to JSON. Skip gettext."),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&copy}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "d","domain",        NULL,     _("Domains"),   _("Text Domain List"),       _("text domain list of your project"),_("TEXTDOMAIN1,TEXTDOMAIN2"),    
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&domain}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "k","key-list",      NULL,     _("Key Names"),_("Key Name List"),           _("to be used key names in a comma separated list"),_("name,description,help"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&key_list}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"locale",       NULL,     _("Locale"),   _("Single Locale"),           NULL, _("de_DE"),   
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&locale}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "l","locales",       NULL,     _("Locales"),  _("Locales List"),            _("locales in a comma separated list"),_("de_DE,es_ES"),   
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&locales}, NULL},
    {"oiwi", 0,                          "n","list-empty",    NULL,     _("List empty"),_("List not translated"),     _("list empty translations too"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_empty}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "p","localedir",     NULL,     _("Directory"),_("Locale Directory"),        _("locale directory containing the your-locale/LC_MESSAGES/your-textdomain.mo gettext translations"),_("LOCALEDIR"),     
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&localedir}, NULL},
    {"oiwi", 0,                          "t","translations-only",NULL,     _("Translations only"),_("Only Translations"),       _("output only translations"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&translations_only}, NULL},
    {"oiwi", 0,                          "e","extract",       NULL,     _("Extract"),  _("Extract translatable Messages"),_("Convert JSON to gettext ready C strings"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&extract}, NULL},
    {"oiwi", 0,                          "w","wrap",          NULL,     _("Wrap Type"),_("language specific wrap"),  NULL, _("TYPE"),          
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)w_choices, sizeof(w_choices), malloc ), 0}}, oyjlSTRING,    {.s=&wrap}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_NO_OPTIMISE,  NULL,"function-name",NULL,     _("Function"), _("Function Name"),           _("A input function name string. e.g.: \"i18n(\\\"\""),_("NAME"), 
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)F_choices, sizeof(F_choices), malloc ), 0}}, oyjlSTRING,    {.s=&function_name}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_NO_OPTIMISE,  NULL,"function-name-out",NULL, _("Function Out"), _("Function Name"),           _("A output funtion name string. e.g.: \"_\""),_("NAME"), 
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)F_choices, sizeof(F_choices), malloc ), 0}}, oyjlSTRING,    {.s=&function_name_out}, NULL},
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("Examples"), NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}, NULL},
    {"oiwi", 0,                          "E","man-environment_variables",NULL,_("Environment Variables"),NULL,       NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)E_choices, sizeof(E_choices), malloc ), 0}}, oyjlNONE,      {}, NULL},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("See Also"), NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h","help",      NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&help}, NULL},
    {"oiwi", 0,                        NULL, "synopsis",      NULL,     NULL,          NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {0}, NULL },
    {"oiwi", 0,                          "v","verbose",       NULL,     _("Verbose"),  _("increase verbosity"),      NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&verbose}, NULL},
    {"oiwi", 0,                          "V","version",       NULL,     _("version"),  _("Version"),                 NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&version}, NULL},
    /* default option template -X|--export */
    {"oiwi", 0,                          "X","export",        NULL,     NULL,          NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&export}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE,  "R","render",        NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&render}, NULL},
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     _("Common Options"),_("Set basic parameters"),    NULL,               "",            "",            "i,o", NULL},
    {"oiwg", 0,     _("Extract"),       _("Convert JSON/C to gettext ready C strings"), _("Two input modes are supported. Read Oyjl UI -X=export JSON. Or parse C sources to --function-name defined strings and replace them in --output by --function-name-out. The later is useful for oyjlTranslation_s, Qt style or other translations."),
                                                                                          "e,k",         "i,o,f,v,function-name,function-name-out",     "e,f,k,function-name,function-name-out", NULL},
    {"oiwg", 0,     _("Add"),           _("Add gettext translated keys to JSON"), NULL,   "a,d,k",       "i,o,l,p,w,t,n,v",            "a,d,l,p,k,w,t,n", NULL},
    {"oiwg", 0,     _("Copy"),          _("Copy keys to JSON"),       _("Import translations from other formats without gettext. Supported --input=Qt-xml-format.tr"),"c,locale",           "i,o,n,v",     "c,locale,n", NULL},
    {"oiwg", 0,     _("Misc"),          _("General options"),         NULL,               "h,X,V",       "v",           "h,X,V,v", NULL },/* just show in documentation */
    {"",0,0,0,0,0,0,0,0}
  };

  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-translate", _("Oyjl Translation"), _("Oyjl UI JSON Translation tool"),
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       NULL,
#endif
                                       sections, oarray, groups, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyjl-translate\n\n", _("For more information read the man page:") );
    goto clean_main;
  }

  if(verbose)
  {
    char * json = ui ? oyjlOptions_ResultsToJson( ui->opts, OYJL_JSON ) : NULL;
    if(json)
      fputs( json, stderr );
    fputs( "\n", stderr );

    char * text = ui ? oyjlOptions_ResultsToText( ui->opts ) : NULL;
    if(text)
      fputs( text, stderr );
    fputs( "\n", stderr );

    fprintf(stderr, "i18n test:\t\"%s\" %s\n", _("Usage"),
#ifdef OYJL_USE_GETTEXT
        textdomain(NULL)
#else
        "----"
#endif
        );
  }

  if((export && strcmp(export,"json+command") == 0))
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = NULL;
    oyjlStringAdd( &json_commands, malloc, free, "{\n  \"command_set\": \"%s\",", argv[0] );
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] ); /* skip opening '{' */
    puts( json_commands );
    goto clean_main;
  }

  /* Render boilerplate */
  if(ui && render)
  {
#if !defined(NO_OYJL_ARGS_RENDER)
    int debug = verbose;
    oyjlTermColorInit( OYJL_RESET_COLORTERM | OYJL_FORCE_COLORTERM );
    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );
#else
    fprintf( stderr, "No render support compiled in. For a GUI use -X json and load into oyjl-args-qml viewer." );
#endif
  }
  else if(ui)
  {
    /* ... working code goes here ... */
    if( (!input && (add || extract || copy)) )
    {
      json = oyjlReadFileStreamToMem( stdin, &size );
    }
    else
    if(input)
    {
      json = oyjlReadFile( input, OYJL_IO_STREAM, &size );
      if(!json && verbose)
      {
        if(oyjlIsFile(input, "r", 0, NULL, 0))
          fprintf(stderr, "File does exist: %s\n", input);
        else
        {
          fprintf(stderr, "File does not exist: %s\n", input);
          system("pwd; ls");
        }
      }
    }

    if(json)
      root = oyjlTreeParseXXX( json, input, verbose );

    if(root)
    {
      int count = 0, i;
      char ** paths = oyjlTreeToPaths( root, 1000000, NULL, 0, &count );
      if(verbose)
        fprintf(stderr, "processed:\t\"%s\" %d\n", input, count);

      if(extract)
      {
        int n = 0;
        char ** list = oyjlStringSplit( key_list, ',', &n, malloc );
        if(verbose)
          fprintf(stderr, "use:\t%d keys\n", n);

        for(i = 0; i < count; ++i)
        {
          char * path = paths[i];
          int j;
          for(j = 0; j < n; ++j)
          {
            char * key = list[j];
            if(oyjlPathMatch(path, key, OYJL_PATH_MATCH_LAST_ITEMS ))
            {
              const char * t = NULL;
              v = oyjlTreeGetValue( root, 0, path );
              if(v)
                t = OYJL_GET_STRING(v);
              if(verbose)
                fprintf(stderr, "found:\t%d key: %s value: \"%s\"\n", i, path, t?t:"----" );
              if(t && t[0])
              {
                char * txt = oyjlStringCopy( t, 0 );
                oyjlStringReplace( &txt, "\\", "\\\\", NULL,NULL );
                oyjlStringReplace( &txt, "\"", "\\\"", NULL,NULL );
                oyjlStringReplace( &txt, "\n", "\\\n", NULL,NULL );
                t = txt;
                if(format)
                  oyjlStringAdd( &text, malloc, free, format, t );
                else if(function_name_out)
                  oyjlStringAdd( &text, malloc, free, "// %s:%s\n{ const char * t = %s%s%s\"); }\n\n",
                                 input, path, function_name_out, strchr(function_name_out,'(')?"":"(\"", t );
                else
                  oyjlStringAdd( &text, malloc, free, "// %s:%s\n{ const char * t = _(\"%s\"); }\n\n",
                                 input, path, t );
                free(txt);
              }
            }
          }
        }

      } else if(add)
      {
        int ln = 0, n = 0, domains_n = 0;
        char ** langs = oyjlStringSplit( locales, ',', &ln, malloc );
        char * var = NULL;
        char * oyjl_domain_path = oyjlStringCopy(OYJL_LOCALEDIR, 0);
        char ** list = oyjlStringSplit( key_list, ',', &n, malloc ),
             ** domains = oyjlStringSplit( domain, ',', &domains_n, malloc );
        char * dir;
        oyjl_val new_translations = NULL;

#ifdef OYJL_USE_GETTEXT
        if(verbose)
          fprintf(stderr, "use:\t%d langs - %s : %s\n", ln, domain, dgettext( domain, "Rendering Intent" ));
#endif
        if(verbose)
          fprintf(stderr, "use:\t%d keys\nlocaledir:\t%s (static: %s)\n", n, localedir, OYJL_LOCALEDIR);

        if(localedir)
        {
          if(!oyjlIsDirFull_ (localedir))
          {
            fprintf(stderr, "Create path:\t%s\n", localedir);
            oyjlMakeDir_(localedir);
            if(!oyjlIsDirFull_ (localedir))
            {
              fprintf(stderr, "%sERROR: Can not find absolute path:\t%s\n", oyjlBT(0), localedir);
              error = 1;
              goto clean_main;
            }
          }
          oyjl_domain_path = oyjlStringCopy(localedir, 0);
        }
        if(!oyjl_domain_path && getenv(OYJL_LOCALE_VAR) && strlen(getenv(OYJL_LOCALE_VAR)))
          oyjl_domain_path = strdup(getenv(OYJL_LOCALE_VAR));
        else if(!oyjl_domain_path || strcmp(localedir,".") == 0)
          oyjl_domain_path = oyjlStringCopy(getenv("PWD"),0);
        else if(!oyjl_domain_path || strcmp(localedir,"locale") == 0)
        {
          oyjl_domain_path = oyjlStringCopy(getenv("PWD"),0);
          oyjlStringAdd( &oyjl_domain_path, 0,0, "/locale" );
        }

        if(verbose)
          fprintf(stderr, "oyjl_domain_path:\t%s\n", oyjl_domain_path);

        if(translations_only)
          new_translations = oyjlTreeNew(0);

        var = NULL;

        oyjlStringAdd( &var, 0,0, "NLSPATH=%s", oyjl_domain_path );
        putenv(var); /* Solaris */

        for(i = 0; i < ln; ++i)
        {
          char * lang = langs[i], * language = NULL;
          int j, k;

          const char * checklocale;
          if(strchr(lang,'.'))
            oyjlStringAdd( &language, 0,0, "%s", lang );
          else
            oyjlStringAdd( &language, 0,0, "%s.UTF-8", lang );
          if(*oyjl_debug || verbose)
            fprintf(stderr, "setenv(%s, 1)\n", oyjlTermColorF(oyjlGREEN,"LANG, %s", language) );
          setenv("LANG", language, 1);
          checklocale = setlocale( LC_MESSAGES, language );
          if(*oyjl_debug || checklocale == NULL || verbose)
            fprintf(stderr, OYJL_DBG_FORMAT "setlocale(%s) == %s\n", OYJL_DBG_ARGS, oyjlTermColorF(oyjlGREEN,language), checklocale );

          free(language);

          if(!checklocale)
            continue;

          language = oyjlLanguage( lang );

          oyjlStringPrepend( &language, "LANGUAGE=", 0,0 );
          if(*oyjl_debug || verbose)
            fprintf(stderr, "putenv(%s)\n", oyjlTermColorF(oyjlGREEN,"%s", language) );
          putenv(language); /* GNU */
          free(language);

          for(k = 0; k < domains_n; ++k)
          {
            domain = domains[k];

#ifdef OYJL_USE_GETTEXT
            var = textdomain( domain );
            dir = bindtextdomain( domain, oyjl_domain_path );
            if(verbose)
            {
              char * t = NULL;
              language = oyjlLanguage( lang );
              oyjlStringAdd( &t, 0,0, "%s/%s/LC_MESSAGES/%s.mo", oyjl_domain_path, language, domain );
              if(oyjlIsFile(t, "r", OYJL_NO_CHECK, NULL, 0))
                fprintf(stderr, "Found translation file: %s\n", oyjlTermColor(oyjlGREEN,t));
              free(t);
              free(language);
            }
#endif

            if(*oyjl_debug || verbose)
              fprintf( stderr, OYJL_DBG_FORMAT "%s = bindtextdomain() to \"%s\"\ntextdomain: %s == %s\n", OYJL_DBG_ARGS, dir, oyjlTermColor(oyjlGREEN,oyjl_domain_path), domain, var );

            for(j = 0; j < count; ++j)
            {
              char * path = paths[j];

              int k;
              for(k = 0; k < n; ++k)
              {
                char * key = list[k];

                if(oyjlPathMatch(path, key, OYJL_PATH_MATCH_LAST_ITEMS ))
                {
                  const char * t = NULL,
                             * tr = NULL;
                  v = oyjlTreeGetValue( root, 0, path );
                  if(v)
                    t = OYJL_GET_STRING(v);
                  if(t && t[0])
#ifdef OYJL_USE_GETTEXT
                    tr = dgettext( domain, t );
#else
                    tr = t;
#endif
                  if(verbose && t && t[0])
                    fprintf(stderr, OYJL_DBG_FORMAT "found:\t key: %s value[%s](%s): \"%s\"\n", OYJL_DBG_ARGS, path, domain, t, tr?(t == tr?oyjlTermColorF(oyjlITALIC,"%s",tr):oyjlTermColorF(oyjlBOLD,"%s",tr)):oyjlTermColorF(oyjlBLUE,"%s","----") );
                  if(t != tr || list_empty)
                  {
                    char * new_path = NULL, * new_key = oyjlStringCopy( t, NULL ), * tmp;
                    tmp = oyjlStringEscape( new_key, OYJL_KEY | OYJL_NO_INDEX, 0 );
                    free(new_key); new_key = tmp; tmp = NULL;
                    oyjlStringAdd( &new_path, malloc, free, "org/freedesktop/oyjl/translations/%s/%s", lang, new_key );
                    v = oyjlTreeGetValue( new_translations?new_translations:root, OYJL_CREATE_NEW, new_path );
                    if(tr)
                    {
                      const char * txt = t != tr ? tr : "";
                      char * t2 = NULL;
                      if(verbose && t && t[0])
                        fprintf(stderr, OYJL_DBG_FORMAT "\toyjlValueSetString( %s ) new_path: %s %s\n", OYJL_DBG_ARGS, oyjlTermColorPtr(oyjlBOLD,&t2,txt), oyjlTermColorF(oyjlITALIC,"%s",new_path), v?"":"failed" );
                      oyjlValueSetString( v, txt );
                      if(t2) free(t2);
                    }
                    oyjlStringAdd( &text, malloc, free, "%s\n", tr );
                    free(new_path);
                  }
                }
              }
            }
          }
        }

        if(verbose)
          fprintf(stderr, "found i18n:\n%s", text );
        if(text) free(text);
        text = NULL;
        i = 0;
        oyjlTreeSortStrings( new_translations ? &new_translations : &root,
                             verbose );
        oyjlTreeToJson( new_translations?new_translations:root, &i, &text );

      } else if(copy)
      {
        int n;
        const char * lang = locale;
        const char * t;
        oyjl_val trans = NULL;
        if(!lang || !lang[0] || (lang && strchr(lang,',')))
        {
          fprintf( stderr, "%s need locale option with exactly one locale: %s\n", oyjlTermColor(oyjlRED,_("Usage Error:")), lang?lang:"---" );
          error = 1;
        }
        /* convert to array */
        v = oyjlTreeGetNewValueFromArray( root, "TS/context", NULL, &n );
        v = oyjlTreeGetValue( root, 0, "TS/context" );
        n = oyjlValueCount( v );
        if(verbose)
        {
          fprintf( stderr, "count in TS/context: %d\n", n );
          if(verbose > 1)
          {
            i = 0;
            oyjlTreeToJson( root, &i, &text );
            if(text)
            {
              fprintf( stderr, "%s\n", text );
              free(text); text = NULL;
            }
          }
        }
        if(!error)
        {
          trans = oyjlTreeNew("org/freedesktop/oyjl/translations");
          for(i = 0; i < n; ++i)
          {
            int j,
                message_n;
            if(verbose)
            {
              v = oyjlTreeGetValueF( root, 0, "TS/context/[%d]/message", i );
              message_n = oyjlValueCount( v );
              v = oyjlTreeGetValueF( root, 0, "TS/context/[%d]/name", i );
              t = OYJL_GET_STRING(v);
              fprintf( stderr, "[%d]/name:\"%s\" message(s):%d\n", i, t?t:"---", message_n );
            }
            v = oyjlTreeGetValueF( root, 0, "TS/context/[%d]/message", i );
            message_n = oyjlValueCount( v );
            for(j = 0; j < message_n; ++j)
            {
              oyjl_val val = oyjlTreeGetValueF( v, 0, "[%d]/source", j );
              const char *tr, *type;
              t = OYJL_GET_STRING(val);
              val = oyjlTreeGetValueF( v, 0, "[%d]/translation", j );
              tr = OYJL_GET_STRING(val);
              val = oyjlTreeGetValueF( v, 0, "[%d]/translation/@type", j );
              type = OYJL_GET_STRING(val);
              if(verbose > 2)
                fprintf( stderr, "t:\"%s\" tr:\"%s\" type:%s\n", t, tr?tr:"---", type?type:"---" );
              if(tr || (list_empty && type && strcmp(type,"unfinished") == 0))
              {
                char * new_path = NULL, * new_key = oyjlStringCopy( t, NULL ), * tmp;
                const char * txt;
                tmp = oyjlStringEscape( new_key, OYJL_KEY | OYJL_NO_INDEX, 0 );
                free(new_key); new_key = tmp; tmp = NULL;
                oyjlStringAdd( &new_path, malloc, free, "org/freedesktop/oyjl/translations/%s/%s", lang, new_key );
                val = oyjlTreeGetValue( trans, OYJL_CREATE_NEW | OYJL_NO_INDEX, new_path );
                txt = oyjlJsonEscape( tr, 0 );
                if(txt)
                  oyjlValueSetString( val, t != tr ? txt : "" );
                free(new_path);
              }
            }
          }
        }
        /* Qt TS/context tree might be chaotic - sort for optimised lookup */
        v = oyjlTreeGetValueF( trans, OYJL_CREATE_NEW | OYJL_NO_INDEX, "org/freedesktop/oyjl/translations/%s", lang );
        n = oyjlValueCount( v );
        if(verbose)
          fprintf( stderr, "count: %d n: %d\n", count, n );
        oyjlStringListRelease( &paths, count, free );
        paths = oyjlTreeToPaths( trans, 1000000, NULL, OYJL_KEY, &count );

        qsort( paths, count, sizeof(char*), oyjlStrcmpWrap_ );

        oyjlTreeFree( root );
        root = oyjlTreeNew("org/freedesktop/oyjl/translations");
        for(i = 0; i < count; ++i)
        {
          const char * path = paths[i];
          t = oyjlTreeGetString_( trans, 0, path );
          oyjlTreeSetStringF( root, OYJL_CREATE_NEW, t, "%s", path );
          if(verbose)
            fprintf( stderr, "path[%d]: %s:%s\n", i, path, t );
        }
        oyjlTreeFree( trans );
        trans = root; root = NULL;
        if(trans)
        {
          i = 0;
          oyjlTreeToJson( trans, &i, &text );
        }

      } else
        for(i = 0; i < count; ++i)
          fprintf( stdout, "%s\n", paths[i] );

      oyjlStringListRelease( &paths, count, free );

      if(text)
      {
        if(wrap)
        {
          char * tmp = NULL;
          char * sname;
          const char * ptext;
          if(strcmp(wrap,"C") != 0)
          {
            fprintf(stderr,"%sERROR: Only -w C is supported.\n", oyjlBT(0));
            error = 1;
            goto clean_main;
          }

          ptext = oyjlTermColorToPlain(text, OYJL_REGEXP);
          free(text);
          text = oyjlStringCopy(ptext,0);
          sname = strdup(domain);
          oyjlStringReplace( &text, "\\", "\\\\", NULL,NULL );
          oyjlStringReplace( &text, "\"", "\\\"", NULL,NULL );
          oyjlStringReplace( &text, "\n", "\\\n", NULL,NULL );
          oyjlStringReplace( &sname, "-", "_", NULL,NULL );
          oyjlStringAdd( &tmp, malloc, free, "#define %s_json \"%s\"\n", sname, text );
          free(text); text = tmp; tmp = NULL;
        }

        if(!output || (strcmp(output,"-") == 0 || strcmp(output,"stdout") == 0))
          fputs( text, stdout );
        else
          oyjlWriteFile( output, text, strlen(text) );
      }
    } else
    if(json && extract)
    {
      int f = oyjlDataFormat(json);
      char * txt = json;
      int n = 0, to_i18n_json = 0;
      if(format && strcasecmp(format, "json") == 0)
        to_i18n_json = 1;
      if(!function_name)
        function_name = "_(\"";
      if(!function_name_out)
        function_name_out = "i18n";
      if(verbose)
        fprintf(stderr,"found input: %d format: %s\n", f, oyjlDataFormatToString(f) );
      if(verbose)
        fprintf(stderr, "search function_name: %s for replacement: %s\n", function_name, to_i18n_json?"i18n_json":function_name_out);

      if(to_i18n_json)
      {
        format = "            \"%s\":\"\",\n";
        oyjlStringAdd( &text, 0,0, "{\n  \"org\": {\n    \"freedesktop\": {\n      \"oyjl\": {\n        \"translations\": {\n          \"cs\": {\n" );
      }

      while((txt = strstr(txt, function_name)) != NULL)
      {
        int qc = 1; /* quotes count */
        int pos = 1;
        int line = 1, i = 0;
        char * t = NULL;

        while(&json[i] < txt)
        {
          if(json[i] == '\n')
            ++line;
          ++i;
        }
        if(verbose >= 2)
          fprintf(stderr, "[%d]:%d %c%c%c%c%c%c%c%c%c...", n, line, txt[0], txt[1], txt[2], txt[3], txt[4], txt[5], txt[6], txt[7], txt[8]);

        txt += strlen(function_name);
        while(txt[pos] && qc)
        {
          if(txt[pos] == '\\' && txt[pos+1] == '\"')
            pos += 2;
          else if(txt[pos] == '\"')
            --qc;
          else
            ++pos;
        }
        if(pos > 1)
        {
          t = oyjlStringAppendN( NULL, (const char*)txt, pos, malloc );
          oyjlStringReplace( &t, "\\", "\\\\", NULL,NULL );
          oyjlStringReplace( &t, "\"", "\\\"", NULL,NULL );
          oyjlStringReplace( &t, "\n", "\\\n", NULL,NULL );
          if(format)
            oyjlStringAdd( &text, malloc, free, format, t );
          else
            oyjlStringAdd( &text, malloc, free, "// %s:%d\n{ const char * t = %s%s%s\"); }\n\n",
                           input, line, function_name_out, strchr(function_name_out,'(')?"":"(\"", t );
        }
        if(verbose >= 2)
          fprintf(stderr, " \"%s\"\n", t?t:"---");
        txt += pos;
        ++n;
      }
      if(to_i18n_json)
      {
        char ** paths;
        int count = 0, i, flags = OYJL_JSON;

        text[strlen(text)-2] = '\000';
        oyjlStringAdd( &text, 0,0, "\n          }\n        }\n      }\n    }\n  }\n}\n" );
        root = oyjlTreeParse2( text, 0, "Internal ERROR", 0 );
        free(text);

        paths = oyjlTreeToPaths( root, 0, NULL, 0, &count );
        qsort( paths, count, sizeof(char*), oyjlStrcmpWrap_ );
        oyjlStringListFreeDoubles( paths, &count, free );
        oyjlTreeFree( root );

        root = oyjlTreeNew("org/freedesktop/oyjl/translations");
        for(i = 0; i < count; ++i)
        {
          const char * path = paths[i];
          oyjlTreeSetStringF( root, OYJL_CREATE_NEW | OYJL_NO_INDEX, "", "%s", path );
        }
        if(plain) flags |= OYJL_NO_MARKUP;
        text = oyjlTreeToText( root, flags );

        n = count;
        oyjlStringListRelease( &paths, count, free );
        oyjlTreeFree( root ); root = NULL;
      }
      if(verbose)
        fprintf(stderr, "found: %d\n", n);
      if(!output || (strcmp(output,"-") == 0 || strcmp(output,"stdout") == 0))
        fputs( text, stdout );
      else
        oyjlWriteFile( output, text, strlen(text) );
    }
  }
  else error = 1;

  clean_main:
  free(sections);
  {
    int i = 0;
    while(oarray[i].type[0])
    {
      if(oarray[i].value_type == oyjlOPTIONTYPE_CHOICE && oarray[i].values.choices.list)
        free(oarray[i].values.choices.list);
      ++i;
    }
  }
  oyjlUi_Release( &ui);

  return error;
}

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;
  oyjlTranslation_s * trc = NULL;
  const char * loc = NULL;

#ifdef __ANDROID__
  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=\"gui\""; /* start QML */
  environment = environ;
#else
  environment = envv;
#endif

  /* language needs to be initialised before setup of data structures */
  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
#ifdef OYJL_HAVE_LOCALE_H
  loc = oyjlSetLocale(LC_ALL,"");
#endif
  if(!loc)
  {
    loc = getenv("LANG");
    fprintf( stderr, "%s", oyjlTermColor(oyjlRED,"Usage Error:") );
    fprintf(stderr,OYJL_DBG_FORMAT "", OYJL_DBG_ARGS);
    fprintf( stderr, " Environment variable possibly not correct. Translations might fail - LANG=%s\n", oyjlTermColor(oyjlBOLD,loc) );
  }
  if(loc)
    trc = oyjlTranslation_New( loc, 0,0,0,0,0,0 );
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, &trc, NULL );
  oyjlTranslation_Release( &trc );

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif
  oyjlLibRelease();

  return 0;
}


