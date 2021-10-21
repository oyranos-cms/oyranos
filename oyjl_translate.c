/*  @file oyjl_translate.c
 *
 *  libOyjl - JSON helper tool
 *
 *  @par Copyright:
 *            2018-2021 (C) Kai-Uwe Behrmann
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
#include "oyjl_i18n.h"
#include "oyjl_tree_internal.h"

oyjl_val oyjlTreeParseXXX            ( const char        * text,
                                       const char        * input,
                                       int                 verbose )
{
  int error_buffer_size = 256;
  char * error_buffer = malloc(error_buffer_size);
  char * json = oyjlStringCopy( text, 0 );
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
    free(json);
    if(strcmp(input,"-") == 0)
      fprintf( stderr, "%s accept gettext input only as file name not as data stream\n", oyjlTermColor(oyjlRED,_("Usage Error:")) );
    else
    {
      json = oyjlReadCommandF( &size, "r", malloc, "lconvert-qt5 -locations relative %s", input );
      if(verbose)
      {
        const char * fn = "oyjl-translate-temp.ts";
        oyjlWriteFile( fn, json, strlen(json) );
        fprintf( stderr, "wrote: %s %lu\n", fn, strlen(json) );
      }
      root = oyjlTreeParseXXX( json, input, verbose );
    }
  }

  if(error_buffer[0])
  {
    fprintf( stderr, "%s %s\n%s\n", oyjlTermColor(oyjlRED,_("Usage Error:")), input, error_buffer );
  }

  free( json );

  return root;
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
             * wrap = NULL,
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
  oyjlOptionChoice_s w_choices[] = {{"C",           _("C static char"), NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s A_choices[] = {{_("Convert JSON to gettext ready C strings"),_("oyjl-translate -e [-v] -i oyjl-ui.json -o result.json -f '_(\"%s\"); ' -k name,description,help"),NULL,                         NULL},
                                    {_("Add gettext translated keys to JSON"),_("oyjl-translate -a -i oyjl-ui.json -o result.json -k name,description,help -d TEXTDOMAIN -p LOCALEDIR -l de_DE,es_ES"),NULL,                         NULL},
                                    {_("Copy translated keys to JSON. Skip gettext."),_("oyjl-translate -c -i lang.tr -o result.json --locale de_DE"),NULL,                         NULL},
                                    {_("View MAN page"),_("oyjl-translate -X man | groff -T utf8 -man -"), NULL,NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{"oyjl(1) oyjl-args(1) oyjl-args-qml(1)","https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html",               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "i","input",         NULL,     _("Input"),    _("File or Stream"),          _("A JSON file name or a input stream like \"stdin\"."),_("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&input}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "o","output",        NULL,     _("Output"),   _("File or Stream"),          _("A JSON file name or a output stream like \"stdout\"."),_("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&output}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "f","format",        NULL,     _("Format"),   _("Format string"),           _("A output format string."),_("FORMAT"), 
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&format}},
    {"oiwi", 0,                          "a","add",           NULL,     _("Add"),      _("Add Translation"),         _("Add gettext translated keys to JSON"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&add}},
    {"oiwi", 0,                          "c","copy",          NULL,     _("Copy"),     _("Copy Translations"),       _("Copy translated keys to JSON. Skip gettext."),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&copy}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "d","domain",        NULL,     _("Domains"),   _("Text Domain List"),       _("text domain list of your project"),_("TEXTDOMAIN1,TEXTDOMAIN2"),    
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&domain}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "k","key-list",      NULL,     _("Key Names"),_("Key Name List"),           _("to be used key names in a comma separated list"),_("name,description,help"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&key_list}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"locale",       NULL,     _("Locale"),   _("Single Locale"),           NULL, _("de_DE"),   
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&locale}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "l","locales",       NULL,     _("Locales"),  _("Locales List"),            _("locales in a comma separated list"),_("de_DE,es_ES"),   
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&locales}},
    {"oiwi", 0,                          "n","list-empty",    NULL,     _("List empty"),_("List not translated"),     _("list empty translations too"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_empty}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "p","localedir",     NULL,     _("Directory"),_("Locale Directory"),        _("locale directory containing the your-locale/LC_MESSAGES/your-textdomain.mo gettext translations"),_("LOCALEDIR"),     
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&localedir}},
    {"oiwi", 0,                          "t","translations-only",NULL,     _("Translations only"),_("Only Translations"),       _("output only translations"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&translations_only}},
    {"oiwi", 0,                          "e","extract",       NULL,     _("Extract"),  _("Extract translatable Messages"),_("Convert JSON to gettext ready C strings"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&extract}},
    {"oiwi", 0,                          "w","wrap",          NULL,     _("Wrap Type"),_("language specific wrap"),  NULL, _("TYPE"),          
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)w_choices, sizeof(w_choices), malloc ), 0}}, oyjlSTRING,    {.s=&wrap}},
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("EXAMPLES"), NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("SEE ALSO"), NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h","help",      NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&help}},
    {"oiwi", 0,                        NULL, "synopsis",      NULL,     NULL,          NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {0} },
    {"oiwi", 0,                          "v","verbose",       NULL,     _("Verbose"),  _("increase verbosity"),      NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&verbose}},
    {"oiwi", 0,                          "V","version",       NULL,     _("version"),  _("Version"),                 NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&version}},
    /* default option template -X|--export */
    {"oiwi", 0,                          "X","export",        NULL,     NULL,          NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&export}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "R","render",        NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&render}},
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     _("Common Options"),_("Set basic parameters"),    NULL,               "",            "",            "i,o"},
    {"oiwg", 0,     _("Extract"),       _("Convert JSON to gettext ready C strings"),NULL,"e,k",         "i,o,f,v",     "e,f,k"},
    {"oiwg", 0,     _("Add"),           _("Add gettext translated keys to JSON"), NULL,   "a,d,k",       "i,o,l,p,w,t,n,v",            "a,d,l,p,k,w,t,n"},
    {"oiwg", 0,     _("Copy"),          _("Copy keys to JSON"),       _("Import translations from other formats without gettext. Supported --input=Qt-xml-format.tr"),"c,locale",           "i,o,n,v",     "c,locale,n"},
    {"oiwg", 0,     _("Misc"),          _("General options"),         NULL,               "h,X,V",       "v",           "h,X,V,v" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
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
    char * json = ui ? oyjlOptions_ResultsToJson( ui->opts ) : NULL;
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
    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );
#else
    fprintf( stderr, "No render support compiled in. For a GUI use -X json and load into oyjl-args-qml viewer." );
#endif
  }
  else if(ui)
  {
    /* ... working code goes here ... */
    if( (!input && (add || extract || copy)) ||
        (strcmp(input,"-") == 0 || strcmp(input,"stdin") == 0) )
    {
      json = oyjlReadFileStreamToMem( stdin, &size );
    }
    else
    if(input)
    {
      json = oyjlReadFile( input, &size );
      if(!json && verbose)
      {
        if(oyjlIsFile(input, "r", NULL, 0))
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
        fprintf(stderr, "processed:\t\"%s\"\n", input);

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
                if(format)
                  oyjlStringAdd( &text, malloc, free, format, t );
                else
                  oyjlStringAdd( &text, malloc, free, "// %s:%s\n{ const char * t = _(\"%s\"); }\n\n",
                                 input, path, t );
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
            fprintf(stderr, "%sERROR: Can not find absolute path:\t%s\n", oyjlBT(0), localedir);
            exit(1);
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
          char * lang = langs[i];
          int j, k;

          const char * checklocale = setlocale( LC_MESSAGES, lang );
          if(*oyjl_debug || checklocale == NULL || verbose)
            fprintf(stderr, "setlocale(%s) == %s\n", lang, checklocale );

          if(!checklocale)
            continue;

          for(k = 0; k < domains_n; ++k)
          {
            domain = domains[k];

#ifdef OYJL_USE_GETTEXT
            var = textdomain( domain );
            dir = bindtextdomain( domain, oyjl_domain_path );
            if(verbose)
            {
              char * t = NULL;
              char * language = oyjlLanguage( lang );
              oyjlStringAdd( &t, 0,0, "%s/%s/LC_MESSAGES/%s.mo", oyjl_domain_path, language, domain );
              if(oyjlIsFile(t, "r", NULL, 0))
                fprintf(stderr, "Found translation file: %s\n", t);
              free(t);
              free(language);
            }
#endif

            if(*oyjl_debug || verbose)
              fprintf( stderr, "%s = bindtextdomain() to \"%s\"\ntextdomain: %s == %s\n", dir, oyjl_domain_path, domain, var );

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
                  if(verbose)
                    fprintf(stderr, "found:\t key: %s value[%s]: \"%s\"\n", path, domain, tr?tr:"----" );
                  if(t != tr || list_empty)
                  {
                    char * new_path = NULL, * new_key = oyjlStringCopy( t, NULL ), * tmp;
                    tmp = oyjlJsonEscape( new_key, OYJL_KEY | OYJL_NO_INDEX );
                    free(new_key); new_key = tmp; tmp = NULL;
                    oyjlStringAdd( &new_path, malloc, free, "org/freedesktop/oyjl/translations/%s/%s", lang, new_key );
                    v = oyjlTreeGetValue( new_translations?new_translations:root, OYJL_CREATE_NEW, new_path );
                    tmp = oyjlJsonEscape( tr, 0 );
                    if(tmp)
                    {
                      oyjlValueSetString( v, t != tr ? tr : "" );
                      free(tmp);
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
        oyjlTreeToJson( new_translations?new_translations:root, &i, &text );

      } else if(copy)
      {
        int count;
        const char * lang = locale;
        oyjl_val trans;
        if(!lang || !lang[0] || (lang && strchr(lang,',')))
        {
          fprintf( stderr, "%s need locale option with exactly one locale: %s\n", oyjlTermColor(oyjlRED,_("Usage Error:")), lang?lang:"---" );
          error = 1;
        }
        v = oyjlTreeGetValue( root, 0, "TS/context/message" );
        count = oyjlValueCount( v );
        if(!error)
        {
          trans = oyjlTreeNew("org/freedesktop/oyjl/translations");
          for(i = 0; i < count; ++i)
          {
            oyjl_val val = oyjlTreeGetValueF( v, 0, "[%d]/source", i );
            const char * t = OYJL_GET_STRING(val), *tr, *type;
            val = oyjlTreeGetValueF( v, 0, "[%d]/translation", i );
            tr = OYJL_GET_STRING(val);
            val = oyjlTreeGetValueF( v, 0, "[%d]/translation/@type", i );
            type = OYJL_GET_STRING(val);
            if(verbose)
              fprintf( stderr, "t:\"%s\" tr:\"%s\" type:%s\n", t, tr?tr:"---", type?type:"---" );
            if(tr || (list_empty && type && strcmp(type,"unfinished") == 0))
            {
              char * new_path = NULL, * new_key = oyjlStringCopy( t, NULL ), * tmp;
              tmp = oyjlJsonEscape( new_key, OYJL_KEY | OYJL_NO_INDEX );
              free(new_key); new_key = tmp; tmp = NULL;
              oyjlStringAdd( &new_path, malloc, free, "org/freedesktop/oyjl/translations/%s/%s", lang, new_key );
              val = oyjlTreeGetValue( trans, OYJL_CREATE_NEW | OYJL_NO_INDEX, new_path );
              tmp = oyjlJsonEscape( tr, 0 );
              if(tmp)
              {
                oyjlValueSetString( val, t != tr ? tmp : "" );
                free(tmp);
              }
              free(new_path);
            }
          }
        }
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
          if(strcmp(wrap,"C") != 0)
          {
            fprintf(stderr,"%sERROR: Only -w C is supported.\n", oyjlBT(0));
            error = 1;
            goto clean_main;
          }

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
    }
  }
  else error = 1;

  clean_main:
  {
    int i = 0;
    while(oarray[i].type[0])
    {
      if(oarray[i].value_type == oyjlOPTIONTYPE_CHOICE && oarray[i].values.choices.list)
        free(oarray[i].values.choices.list);
      ++i;
    }
  }
  oyjlLibRelease();

  return error;
}

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  setenv("COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */

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
#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_ALL,"");
#endif
#endif
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, OYJL_DOMAIN, NULL );

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif

  return 0;
}


