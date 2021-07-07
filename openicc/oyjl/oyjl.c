/** @file oyjl.c
 *
 *  oyjl - Yajl tree extension
 *
 *  @par Copyright:
 *            2016-2020 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2016/12/17
 */

#include "oyjl.h"
#include "oyjl_tree_internal.h"
#include "oyjl_macros.h"
#include "oyjl_version.h"
#include "oyjl_i18n.h"
extern char **environ;


#ifdef __ANDROID__
# include "oyjl.i18n.c"
#endif

static oyjlOptionChoice_s * listInput ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  oyjlOptionChoice_s * c = NULL;

  int size = 0, i,n = 0;
  char * result = oyjlReadCommandF( &size, "r", malloc, "ls -1 *.[J,j][S,s][O,o][N,n]" );
  char ** list = oyjlStringSplit( result, '\n', &n, 0 );

  if(list)
  {
    c = calloc(n+1, sizeof(oyjlOptionChoice_s));
    if(c)
    {
      for(i = 0; i < n; ++i)
      {
        c[i].nick = strdup( list[i] );
        c[i].name = strdup("");
        c[i].description = strdup("");
        c[i].help = strdup("");
      }
    }
    free(list);
  }

  return c;
}

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  char * text = NULL,
       * text_tmp = NULL;
  int size = 0;
  oyjl_val root = NULL;
  oyjl_val value = NULL;
  int index = 0;

  int error = 0;
  int state = 0;
  int json = 0;
  int yaml = 0;
  int xml = 0;
  int paths = 0;
  int key = 0;
  int count = 0;
  int type = 0;
  const char * set = NULL;
  const char * i_filename = NULL;
  const char * xpath = NULL;
  int format = 0;
  const char * try_format = NULL;
  int verbose = 0;
  const char * help = NULL;
  int version = 0;
  const char * render = NULL;
  const char * export = NULL;

  /* handle options */
  oyjlUiHeaderSection_s * sections = oyjlUiInfo_( _("The oyjl program can be used to parse, filter sub trees, select values and modify values in JSON texts."),
                                                 "2017-11-12T12:00:00", _("November 12, 2017") );

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s r_choices[] = {{"JSON",        "JSON",             NULL,                         NULL},
                                    /*{"XML",         "XML",              NULL,                         NULL},
                                    {"YAML",        "YAML",             NULL,                         NULL},*/
                                    {"","","",""}};
  oyjlOptionChoice_s A_choices[] = {{_("Print JSON to stdout"),_("oyjl -i text.json -x ///[0]"),NULL,                         NULL},
                                    {_("Print count of leafs in node"),_("oyjl -c -i text.json -x my/path/"),NULL,                         NULL},
                                    {_("Print key name of node"),_("oyjl -k -i text.json -x ///[0]"),NULL,                         NULL},
                                    {_("Print all matching paths"),_("oyjl -p -i text.json -x //"),NULL,                         NULL},
                                    {_("Set a key name to a value"),_("oyjl -i text.json -x my/path/to/key -s value"),NULL,                         NULL},
                                    {"","","",""}};

  oyjlOptionChoice_s S_choices[] = {{"oyjl-args(1) oyjl-translate(1) oyjl-args-qml(1)","https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html",               NULL,                         NULL},
                                    {"","","",""}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "j","json",          NULL,     _("JSON"),     _("Print JSON to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&json}},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "y","yaml",          NULL,     _("YAML"),     _("Print YAML to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&yaml}},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "m","xml",           NULL,     _("XML"),      _("Print XML to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&xml}},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "c","count",         NULL,     _("Count"),    _("Print count of leafs in node"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&count}},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "k","key",           NULL,     _("Key Name"), _("Print key name of node"), NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&key}},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   NULL,"paths",        NULL,     _("Paths"),    _("Print all matching paths"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&paths}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "s","set",           NULL,     _("Set Value"),_("Set a key name to a value"),NULL,_("STRING"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&set}},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "t","type",          NULL,     _("Type"),     _("Get node type"),          NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&type}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_REPETITION, "i","input",         NULL,     _("Input"),    _("File or Stream"),_("A JSON file name or a input stream like \"stdin\"."),_("FILENAME"),
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listInput}, oyjlSTRING, {.s=&i_filename}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "x","xpath",         NULL,     _("XPath"),    _("Path specifier"),_("The path consists of slash '/' separated terms. Each term can be a key name or a square bracketed index. A empty term is used for a search inside a tree."),_("PATH"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&xpath}},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "f","format",        NULL,     _("Format"),   _("Print Data Format"),       NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&format}},
    {"oiwi", 0,                          "r","try-format",    NULL,     _("Try Format"),_("Try to find data format, even with offset."), NULL, _("FORMAT"),
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)r_choices, sizeof(r_choices), malloc )}, oyjlSTRING,    {.s=&try_format}},
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("EXAMPLES"),NULL,                      NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc )}, oyjlNONE,      {}},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("SEE ALSO"),NULL,                      NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc )}, oyjlNONE,      {}},
    /* default options -h and -v */
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help", NULL,NULL, NULL, NULL,NULL, oyjlOPTIONTYPE_FUNCTION, {0}, oyjlSTRING, {.s=&help} },
    {"oiwi", 0, NULL, "synopsis", NULL, NULL, NULL,      NULL,     NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0} },
    {"oiwi", 0, "v","verbose",NULL,_("Verbose"),_("increase verbosity"), NULL, NULL, oyjlOPTIONTYPE_NONE,{0},oyjlINT,{.i=&verbose}},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, "R","render",NULL,NULL,NULL,NULL,NULL,oyjlOPTIONTYPE_CHOICE,{0},oyjlSTRING,{.s=&render}},
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export} },
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     _("Input"),         _("Set input file and path"), NULL,               "",            "",            "i,x,s,r"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print JSON"), _("Print JSON to stdout"),NULL,  "j",           "i,x,s,r",     "j"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print YAML"), _("Print YAML to stdout"),NULL,  "y",           "i,x,s,r",     "y"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print XML"),  _("Print XML to stdout"), NULL,  "m",           "i,x,s,r",     "m"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Count"),      _("Print node count"),    NULL,  "c",           "i,x,r",       "c"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Key Name"),   _("Print key name"),      NULL,  "k",           "i,x,r",       "k"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Type"),       _("Print type"),          NULL,  "t",           "i,x,r",       "t"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Paths"),      _("Print all matching paths."),NULL,  "paths",  "i,x,r",       "paths"},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Format"),     _("Print Data Format."),  NULL,  "f",           "i,x,r",       "f"},
    {"oiwg", 0,     _("Misc"),          _("General options"),         NULL,               "h|X|V",       "v",           "h,X,V,v" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl", _("Oyjl Json Manipulation"), _("Light weight JSON parse and manipulation tool"),
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
    fprintf( stdout, "%s\n\tman oyjl\n\n", _("For more information read the man page:") );
    goto clean_main;
  }

  if(ui && verbose)
  {
    char * json = oyjlOptions_ResultsToJson( ui->opts );
    if(json)
      fputs( json, stderr );
    fputs( "\n", stderr );

    char * text = oyjlOptions_ResultsToText( ui->opts );
    if(text)
      fputs( text, stderr );
    fputs( "\n", stderr );
  }

  if(ui && (export && strcmp(export,"json+command") == 0))
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = NULL;
    oyjlStringAdd( &json_commands, malloc, free, "{\n  \"command_set\": \"%s\",", argv[0] );
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] );
    puts( json_commands );
    goto clean_main;
  }

  /* Render boilerplate */
  if(ui && render)
  {
#if !defined(NO_OYJL_ARGS_RENDER)
# ifdef __ANDROID__
#   define RENDER_I18N oyjl_json
# else
#   define RENDER_I18N NULL
# endif
    int debug = verbose;
    oyjlArgsRender( argc, argv, RENDER_I18N, NULL,NULL, debug, ui, myMain );
#else
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "No render support compiled in. For a GUI use -X json and load into oyjl-args-qml viewer.", OYJL_DBG_ARGS );
#endif
  }
  else if(ui)
  {
    /* ... working code goes here ... */
    if(strstr(argv[0],"jsontoyaml") && yaml == 0)
      yaml = 1;
    if(strstr(argv[0],"jsontoxml") && xml == 0)
      xml = 1;
    if(!json && !yaml && !xml && !paths && !key && !count && !type )
      json = 1;

    if(i_filename)
    {
      FILE * fp;

      if(strcmp(i_filename,"-") == 0)
        fp = stdin;
      else
        fp = fopen(i_filename,"rb");

      if(fp)
      {
        text = oyjlReadFileStreamToMem( fp, &size ); 
        if(fp != stdin) fclose( fp );
      }
    }

    if(text && try_format && strcasecmp(try_format, "JSON") == 0 && text[0] != '{' && strstr(text, "\n{"))
    {
      text_tmp = text;
      text = strstr(text, "\n{") + 1;
    }

    if(format)
    {
      int type = oyjlDataFormat(text);
      const char * r = oyjlDataFormatToString(type);
      fprintf(stdout, "%s\n", r);

    } else
    if(text)
    {
      char error_buffer[256] = {0};
      if(verbose)
        fprintf(stderr, "file read:\t\"%s\" %d\n", i_filename, size);

      root = oyjlTreeParse( text, error_buffer, 256 );
      if(error_buffer[0] != '\000')
        fprintf(stderr, "ERROR:\t\"%s\"\n", error_buffer);
      else if(!root)
        fprintf(stderr, "ERROR:\tparsing \"%s\":\n%s", i_filename, text);
      if(verbose)
        fprintf(stderr, "file parsed:\t\"%s\"\n", i_filename);

      if(xpath)
      {
        char ** path_list = NULL;
        int count = 0, i;

        oyjlTreeToPaths( root, 1000000, xpath, 0, &path_list );
        while(path_list && path_list[count]) ++count;

        if(paths)
          for(i = 0; i < count; ++i)
            fprintf(stdout,"%s\n", path_list[i]);
        else if(key)
          fprintf(stdout,"%s\n", (count && path_list[0]) ? strchr(path_list[0],'/') ? strrchr(path_list[0],'/') + 1 : path_list[0] : "");

        if(path_list || set)
          value = oyjlTreeGetValue( root,
                                     set ? OYJL_CREATE_NEW : 0,
                                     path_list?path_list[0]:xpath );
        if(verbose)
          fprintf(stderr, "%s xpath \"%s\"\n", value?"found":"found not", xpath);

        oyjlStringListRelease( &path_list, count, free );

        if(set)
        {
          if(value)
            oyjlValueSetString( value, set );
          else
            oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "obtained no leaf for xpath \"%s\" from JSON:\t\"%s\"",
                           OYJL_DBG_ARGS, xpath, i_filename );
        }

        if(verbose)
          fprintf(stderr, "processed:\t\"%s\"\n", i_filename);
      }
      else if(set)
        oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "set argument needs as well a xpath argument", OYJL_DBG_ARGS );
      else
        value = root;
    } else
      oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "no data input", OYJL_DBG_ARGS );

    if(value)
    {
      if(json || yaml || xml)
      {
        char * text = NULL;
        int level = 0;
        if(json)
          oyjlTreeToJson( set ? root : value, &level, &text );
        else if(yaml)
        {
          oyjlTreeToYaml( set ? root : value, &level, &text );
          oyjlStringAdd( &text, 0,0, "\n" );
        }
        else if(xml)
        {
          oyjlTreeToXml( set ? root : value, &level, &text );
          if(text && strcmp( text, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" ) == 0 )
          {
            free(text); text = NULL;
          }
          else
            oyjlStringAdd( &text, 0,0, "\n" );
        }
        if(text)
        {
          fwrite( text, sizeof(char), strlen(text), stdout );
          free(text);
        }
      }

      if(type)
      switch(value->type)
      {
        case oyjl_t_string: puts( "string" ); break;
        case oyjl_t_number: puts( "number" ); break;
        case oyjl_t_object: puts( "object" ); break;
        case oyjl_t_array: puts( "array" ); break;
        case oyjl_t_true: puts( "true" ); break;
        case oyjl_t_false: puts( "false" ); break;
        case oyjl_t_null: puts( "null" ); break;
        case oyjl_t_any: puts( "any" ); break;
      }

      if(count)
      {
        char n[128] = {0};
        sprintf(n, "%d", oyjlValueCount(value));
        puts( n );
      }

      if(key)
      {
      if(!xpath && value->type == oyjl_t_object && oyjlValueCount(value) > index)
        puts( value->u.object.keys[index] );
      }

      if(paths)
      {
        if(!xpath)
        {
          char ** paths = NULL;
          int count = 0, i;

          oyjlTreeToPaths( root, 1000000, NULL, 0, &paths );
          if(verbose)
            fprintf(stderr, "processed:\t\"%s\"\n", i_filename);
          while(paths && paths[count]) ++count;

          for(i = 0; i < count; ++i)
            fprintf(stdout,"%s\n", paths[i]);

          oyjlStringListRelease( &paths, count, free );
        }
      }
    }

    if(root) oyjlTreeFree( root );
    if(text_tmp) free(text_tmp);
    else if(text) free(text);
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
  argv[argc++] = "--render=gui"; /* start QML */
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


