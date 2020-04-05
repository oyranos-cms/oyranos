/*  @file oyjl_translate.c
 *
 *  libOyjl - JSON helper tool
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
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

#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef OYJL_USE_GETTEXT
# ifdef OYJL_HAVE_LIBINTL_H
#  include <libintl.h> /* bindtextdomain() */
# endif
# define _(text) dgettext( OYJL_DOMAIN, text )
#else
# define _(text) text
#endif

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  int add = 0;
  int list_empty = 0;
  int extract = 0;
  int size = 0;
  int translations_only = 0;
  const char * output = NULL,
             * input = NULL,
             * format = NULL,
             * wrap = NULL,
             * key_list = "name,description,help",
             * locales = "cs_CZ,de_DE,eo_EO,eu_ES,fr_FR,ru_RU",
             * localedir = OYJL_LOCALEDIR,
             * domain = OYJL_DOMAIN;
  char * json = NULL;
  oyjl_val root = NULL,v;
  char * text = NULL;

  int error = 0;
  int state = 0;
  int verbose = 0;
  int help = 0;
  int version = 0;
  const char * render = NULL;
  const char * export = NULL;

  /* handle options */
  /* Select a nick from *version*, *manufacturer*, *copyright*, *license*,
   * *url*, *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation*. Choose what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s * sections = oyjlUiInfo( _("Convert Oyjl UI JSON to C translatable strings for use with gettext tools and translate a tools UI using the programs own text domain. The resulting Oyjl UI JSON can be used for translated rendering."),
                                                 "2020-01-02T12:00:00", _("January 2, 2020") );

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s w_choices[] = {{"C",           _("C static char"), NULL,                         NULL},
                                    {"","","",""}};
  oyjlOptionChoice_s A_choices[] = {{_("Convert JSON to gettext ready C strings"),_("oyjl-translate -e [-v] -i oyjl-ui.json -o result.json -f '_(\\\"%%s\\\"); ' -k name,description,help"),NULL,                         NULL},
                                    {_("Add gettext translated keys to JSON"),_("oyjl-translate -a -i oyjl-ui.json -o result.json -k name,description,help -d TEXTDOMAIN -p LOCALEDIR -l de_DE,es_ES"),NULL,                         NULL},
                                    {_("View MAN page"),_("oyjl-translate -X man | groff -T utf8 -man -"), NULL,NULL},
                                    {"","","",""}};

  oyjlOptionChoice_s S_choices[] = {{"oyjl(1) oyjl-args(1) oyjl-args-qml(1)","https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html",               NULL,                         NULL},
                                    {"","","",""}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "i","input",         NULL,     _("Input"),    _("File or Stream"),          _("A JSON file name or a input stream like \"stdin\"."),_("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&input}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "o","output",        NULL,     _("Output"),   _("File or Stream"),          _("A JSON file name or a output stream like \"stdout\"."),_("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&output}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "f","format",        NULL,     _("Format"),   _("Format string"),           _("A output format string."),_("'_(\\\"%s\\\"); '"), 
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&format}},
    {"oiwi", 0,                          "a","add",           NULL,     _("Add"),      _("Add Translation"),         _("Add gettext translated keys to JSON"),NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&add}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "d","domain",        NULL,     _("Domain"),   _("Text Domain"),             _("text domain of your project"),_("TEXTDOMAIN"),    
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&domain}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "k","key-list",      NULL,     _("Key Names"),_("Key Name List"),           _("to be used key names in a comma separated list"),_("name,description,help"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&key_list}},
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
    {"oiwi", 0,                          "h","help",          NULL,     _("help"),     _("Help"),                    NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&help}},
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
    {"oiwg", 0,     _("Misc"),          _("General options"),         NULL,               "h,X",         "v",           "h,v,X" },/* just show in documentation */
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
    if( (!input && (add || extract)) ||
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
    {
      char * text = malloc(256);

      text[0] = 0;

      /* parse json ... */
      root = oyjlTreeParse( json, text, 256 );
      if(text[0])
      {
        fprintf( stderr, "ERROR: %s\n%s\n", input, text );
      }
    }

    if(root)
    {
      char ** paths = NULL;
      int count = 0, i;

      oyjlTreeToPaths( root, 1000000, NULL, 0, &paths );
      if(verbose)
        fprintf(stderr, "processed:\t\"%s\"\n", input);
      while(paths && paths[count]) ++count;

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
              if(t)
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
        int ln = 0, n = 0;
        char ** langs = oyjlStringSplit( locales, ',', &ln, malloc );
        char * var = NULL;
        const char * oyjl_domain_path = OYJL_LOCALEDIR;
        char ** list = oyjlStringSplit( key_list, ',', &n, malloc );
        char * dir;
        oyjl_val new_translations = NULL;

#ifdef OYJL_USE_GETTEXT
        if(verbose)
          fprintf(stderr, "use:\t%d langs - %s : %s\n", ln, domain, dgettext( domain, "Rendering Intent" ));
#endif
        if(verbose)
          fprintf(stderr, "use:\t%d keys\n", n);

        if(localedir)
        {
          if(!oyjlIsDirFull_ (localedir))
          {
            fprintf(stderr, "ERROR: Can not find absolute path:\t%s\n", localedir);
            exit(1);
          }
          oyjl_domain_path = localedir;
        }
        if(!oyjl_domain_path && getenv(OYJL_LOCALE_VAR) && strlen(getenv(OYJL_LOCALE_VAR)))
          oyjl_domain_path = strdup(getenv(OYJL_LOCALE_VAR));

        if(translations_only)
          new_translations = oyjlTreeNew(0);

#ifdef OYJL_USE_GETTEXT
        var = textdomain( domain );
        dir = bindtextdomain( domain, oyjl_domain_path );
#endif

        if(*oyjl_debug)
          fprintf(stderr, "%s = bindtextdomain() to \"%s\"\ntextdomain: %s == %s\n", dir, oyjl_domain_path, domain, var );
        var = NULL;

        oyjlStringAdd( &var, 0,0, "NLSPATH=%s", oyjl_domain_path );
        putenv(var); /* Solaris */

        for(i = 0; i < ln; ++i)
        {
          char * lang = langs[i];
          int j;

          const char * checklocale = setlocale( LC_MESSAGES, lang );
          if(*oyjl_debug || checklocale == NULL)
            fprintf(stderr, "setlocale(%s) == %s\n", lang, checklocale );

          if(!checklocale)
            continue;

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
                  oyjlStringReplace( &new_key, "/", "%37", NULL,NULL );
                  tmp = oyjlJsonEscape( new_key );
                  free(new_key); new_key = tmp; tmp = NULL;
                  oyjlStringAdd( &new_path, malloc, free, "org/freedesktop/oyjl/translations/%s/%s", lang, new_key );
                  v = oyjlTreeGetValue( new_translations?new_translations:root, OYJL_CREATE_NEW, new_path );
                  tmp = oyjlJsonEscape( tr );
                  if(tmp)
                  {
                    oyjlValueSetString( v, t != tr ? tmp : "" );
                    free(tmp);
                  }
                  oyjlStringAdd( &text, malloc, free, "%s\n", tr );
                  free(new_path);
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

      } else
        for(i = 0; i < count; ++i)
          fprintf( stdout, "%s\n", paths[i] );

      oyjlStringListRelease( &paths, count, free );

      if(text)
      {
        if(wrap)
        {
          char * tmp = NULL;
          char * sname = strdup(domain);
          if(strcmp(wrap,"C") != 0)
          {
            fprintf(stderr,"ERROR: Only -w C is supported.\n");
            exit(1);
          }

          oyjlStringReplace( &text, "\"", "\\\"", NULL,NULL );
          oyjlStringReplace( &text, "\n", "\\n\\\n", NULL,NULL );
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


