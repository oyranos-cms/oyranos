/** @file oyjl.c
 *
 *  oyjl - Yajl tree extension
 *
 *  @par Copyright:
 *            2016-2023 (C) Kai-Uwe Behrmann
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
#include "oyjl_i18n_internal.h"
extern char **environ;


#ifdef __ANDROID__
# include "oyjl.i18n.c"
#endif

#include <ctype.h>   /* isspace() */

oyjl_val oyjlTreeParse2_             ( const char        * input,
                                       int                 size,
                                       const char        * xpath,
                                       char             ** first_path, /* provide first path together with xpath */
                                       oyjl_val          * root,
                                       char            *** path_list,
                                       const char        * try_format,
                                       int                 paths,
                                       const char        * error_name,
                                       oyjlOptions_s     * opts,
                                       int                 flags,
                                       int                 verbose )
{
  oyjl_val value = NULL, root_ = NULL;
  const char * text = input;
  char * text_tmp = NULL;
  int delimiter = 0;

  if(text && try_format && strcasecmp(try_format, "JSON") == 0 && text[0] != '{' && strstr(text, "\n{"))
    text = strstr(text, "\n{") + 1;
  else if(text && try_format && strcasecmp(try_format, "XML") == 0 && text[0] != '<' && strstr(text, "\n<"))
    text = strstr(text, "\n<") + 1;

  if(text && try_format && strcasecmp(try_format, "CSV") == 0)
    delimiter = OYJL_DELIMITER_COMMA;
  if(text && try_format && strcasecmp(try_format, "CSV-semicolon") == 0)
    delimiter = OYJL_DELIMITER_SEMICOLON;

  if(try_format && strcasecmp(try_format, "JSON") != 0 && strcasecmp(try_format, "XML") != 0 && strcasecmp(try_format, "CSV") != 0 && strcasecmp(try_format, "CSV-semicolon") != 0)
  {
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, "try-format", 0/* flags */ );
    char * t = oyjlOption_PrintArg_(o, oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_OPTION_ONLY);
    fprintf(stderr, "%s\tparsing \"%s\":\n", oyjlTermColor(oyjlRED,_("Usage Error:")), error_name );
    oyjlOptions_Print_( opts, -1 );
    fprintf(stderr, "%s %s=%s\n", _("wrong argument to option:"), t, oyjlTermColor(oyjlITALIC,_(try_format)));
    fprintf(stderr, "%s\n", _("... try with --help|-h option for usage text. give up") );
    free(t);
  }

  if(text)
  {
    char ** path_list_ = NULL;
    int count = 0, i = 0;
    char * first_path_ = NULL;
    const char * xpath_full = NULL;

    if(verbose)
      fprintf(stderr, "%s file read:\t\"%s\" %d\n", oyjlPrintTime(OYJL_BRACKETS, oyjlNO_MARK), error_name, size);

    /* convert exported C declarated string into plain text using cc compiler */
    if(oyjlStringStartsWith(text, "#define ") && !delimiter)
    {
      int size = 0;
      char * t = oyjlStringCopy( text, 0 ), * name;
      while(t[i] && !isspace(t[i])) ++i;
      while(t[i] && isspace(t[i])) ++i;
      name = oyjlStringCopy( &t[i], 0 );
      i = 0;while(name[i] && !isspace(name[i]) && name[i] != '{') ++i;
      name[i] = '\000';
      if(verbose)
        fprintf( stderr, "Found C object: %s\n", name );

      oyjlStringAdd( &t, 0,0, "\n#include <stdio.h>\nint main(int argc, char**argv)\n{\nputs(%s);\n}\n", name );
      oyjlWriteFile( "oyjl_tmp.c", t, strlen(t) );
      free(t);
      free(name);
      text_tmp = oyjlReadCommandF( &size, "r", malloc, "cc -g oyjl_tmp.c -o oyjl-tmp; ./oyjl-tmp" );
      if(!verbose)
      { remove("oyjl-tmp"); remove("oyjl_tmp.c"); }
      text = text_tmp;
    }

    int status = 0;
    root_ = oyjlTreeParse2( text, delimiter | flags, __func__, &status );
    if(status)
      fprintf(stderr, "%s\t\"%s\"\n", oyjlTermColor(oyjlRED,_("Usage Error:")), oyjlPARSE_STATE_eToString(status));
    else if(!root_)
      fprintf(stderr, "%s\tparsing \"%s\":\n%s", oyjlTermColor(oyjlRED,_("Usage Error:")), error_name, text);
    if(verbose)
      fprintf(stderr, "%s file parsed:\t\"%s\"\n", oyjlPrintTime(OYJL_BRACKETS, oyjlNO_MARK), error_name);

    if(paths || xpath || path_list)
      path_list_ = oyjlTreeToPaths( root_, 1000000, xpath, 0, &count );

    if(paths)
      for(i = 0; i < count; ++i)
        fprintf(stdout,"%s\n", path_list_[i]);
    if(xpath)
    {
      xpath_full = (count && path_list_[0]) ? path_list_[0] : "";
      first_path_ = oyjlStringCopy((count && path_list_[0]) ? strchr(path_list_[0],'/') ? strrchr(path_list_[0],'/') + 1 : path_list_[0] : "", malloc);
      if(first_path)
        *first_path = first_path_;

      if(verbose)
        fprintf(stderr, "processed:\t\"%s\"\n", error_name);

      value = oyjlTreeGetValue( root_, 0, xpath_full );

      if(verbose)
        fprintf(stderr, "%s xpath \"%s\"\n", value?"found":"found not", xpath);
    }
    else
      value = root_;

    if(path_list)
      *path_list = path_list_;
    else
      oyjlStringListRelease( &path_list_, count, free );

    if(!first_path && first_path_)
      free(first_path_);

  } else
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "no data input", OYJL_DBG_ARGS );

  if(root)
    *root = root_;
  else
  {
    value = NULL;
    oyjlTreeFree(root_);
  }
  if(text_tmp)
    free(text_tmp);

  return value;
}

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  char * text = NULL;
  int size = 0;
  oyjl_val root = NULL;
  oyjl_val value = NULL;
  int i_files_n = 0;
  char ** i_files = NULL;

  int error = 0;
  int state = 0;
  int json = 0;
  int yaml = 0;
  int xml = 0;
  int csv = 0;
  int csv_semicolon = 0;
  int paths = 0;
  int key = 0;
  int count = 0;
  int type = 0;
  const char * set = NULL;
  const char * i_filename = NULL;
  const char * xpath = NULL;
  int format = 0;
  const char * plain = 0;
  const char * detect = 0;
  const char * try_format = NULL,
             * wrap = NULL,
             * wrap_name = "wrap";
  int verbose = 0;
  const char * help = NULL;
  int version = 0;
  const char * render = NULL;
  const char * export = NULL;

  /* handle options */
  oyjlUiHeaderSection_s * sections = oyjlUiInfo_( _("The oyjl program can be used to parse, filter sub trees, select values and modify values in JSON texts."),
                                                 "2017-11-12T12:00:00", _("November 12, 2017") );

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s d_choices[] = {{",",           _("Comma"),         _("Decimal Separator"),       NULL},
                                    {".",           _("Dot"),           _("Decimal Separator"),       NULL},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s r_choices[] = {{"JSON",        "JSON",             NULL,                         NULL},
                                    {"XML",         "XML",              NULL,                         NULL},
                                    {"CSV",         "CSV",              NULL,                         NULL},
                                    {"CSV-semicolon","CSV-semicolon",   NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s w_choices[] = {{"C",           _("C static char"), NULL,                         NULL},
/*                                    {"oiJS",        _("Oyjl static JSON"), NULL,                         NULL},*/
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s W_choices[] = {{"wrap",        "wrap",             NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s A_choices[] = {{"",_("Print JSON to stdout"),_("oyjl -i text.json -x ///[0]"),                         NULL},
                                    {"",_("Print count of leafs in node"),_("oyjl -c -i text.json -x my/path/"),                         NULL},
                                    {"",_("Print key name of node"),_("oyjl -k -i text.json -x ///[0]"),                         NULL},
                                    {"",_("Print all matching paths"),_("oyjl -p -i text.json -x //"),                         NULL},
                                    {"",_("Set a key name to a value"),_("oyjl -i text.json -x my/path/to/key -s value"),                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{"oyjl-args(1) oyjl-translate(1) oyjl-args-qml(1)","https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html",               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "j","json",          NULL,     _("JSON"),     _("Print JSON to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&json}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "y","yaml",          NULL,     _("YAML"),     _("Print YAML to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&yaml}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "m","xml",           NULL,     _("XML"),      _("Print XML to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&xml}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   NULL,"csv",          NULL,     _("CSV"),      _("Print CSV to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&csv}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   NULL,"csv-semicolon",NULL,     _("CSV-semicolon"),_("Print CSV with semicolon to stdout"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&csv_semicolon}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "c","count",         NULL,     _("Count"),    _("Print count of leafs in node"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&count}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "k","key",           NULL,     _("Key Name"), _("Print key name of node"), NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&key}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   NULL,"paths",        NULL,     _("Paths"),    _("Print all matching paths"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&paths}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "s","set",           NULL,     _("Set Value"),_("Set a key name to a value"),NULL,_("STRING"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&set}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "t","type",          NULL,     _("Type"),     _("Get node type"),          NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&type}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_REPETITION, "i","input",         NULL,     _("Input"),    _("File or Stream"),_("A JSON file name or a input stream like \"stdin\"."),_("FILENAME"),
        oyjlOPTIONTYPE_FUNCTION, {0},                oyjlSTRING,    {.s=&i_filename}, "file_names=*.[J,j][S,s][O,o][N,n];*.[X,x][M,m][L,l];*.[Y,y][A,a][M,m][L,l]"},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "x","xpath",         NULL,     _("XPath"),    _("Path specifier"),_("The path consists of slash '/' separated terms. Each term can be a key name or a square bracketed index. A empty term is used for a search inside a tree."),_("PATH"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&xpath}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,   "f","format",        NULL,     _("Format"),   _("Print Data Format"),       NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&format}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG,"p","plain",      NULL,     _("Plain"),    _("No Markup"),               NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlSTRING,    {.s=&plain}, NULL},
    {"oiwi", 0,                          "r","try-format",    NULL,     _("Try Format"),_("Try to find data format, even with offset."), NULL, _("FORMAT"),
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)r_choices, sizeof(r_choices), malloc )}, oyjlSTRING,    {.s=&try_format}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG,"d","detect-numbers",NULL,  _("Detect"),    _("Try to detect numbers from non typesafe formats."),  _("Uses by default dot '.' as decimal separator."), _("SEPARATOR"),
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)d_choices, sizeof(d_choices), malloc )}, oyjlSTRING,    {.s=&detect}, NULL},
    {"oiwi", 0,                          "w","wrap",          NULL,     _("Wrap Type"),_("language specific wrap"),  NULL, _("TYPE"),          
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)w_choices, sizeof(w_choices), malloc ), 0}}, oyjlSTRING,    {.s=&wrap}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "W","wrap-name",     NULL,     _("Wrap Name"),_("A name for the symbol to be defined."), _("Use only letters from alphabet [A-Z,a-z] including optional underscore '_'."), _("NAME"),          
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)W_choices, sizeof(W_choices), malloc ), 0}}, oyjlSTRING,    {.s=&wrap_name}, NULL},
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("Examples"),NULL,                      NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc )}, oyjlNONE,      {}, NULL},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("See Also"),NULL,                      NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc )}, oyjlNONE,      {}, NULL},
    /* default options -h and -v */
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help", NULL,NULL, NULL, NULL,NULL, oyjlOPTIONTYPE_FUNCTION, {0}, oyjlSTRING, {.s=&help}, NULL },
    {"oiwi", 0, NULL, "synopsis", NULL, NULL, NULL,      NULL,     NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0}, NULL },
    {"oiwi", 0, "v","verbose",NULL,_("Verbose"),_("increase verbosity"), NULL, NULL, oyjlOPTIONTYPE_NONE,{0},oyjlINT,{.i=&verbose}, NULL},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, "R","render",NULL,NULL,NULL,NULL,NULL,oyjlOPTIONTYPE_CHOICE,{0},oyjlSTRING,{.s=&render}, NULL},
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version}, NULL },
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export}, NULL },
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     _("Input"),         _("Set input file and path"), NULL,               "",            "",            "i,x,s,p,r,d,w,W", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print JSON"), _("Print JSON to stdout"),"JSON - JavaScript Object Notation",  "j",           "i,x,s,r,p,d,w,W", "j", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print YAML"), _("Print YAML to stdout"),"YAML - Yet Another Markup Language",  "y",           "i,x,s,r,p,d,w,W", "y", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print XML"),  _("Print XML to stdout"), "XML - eXtended Markup Language",  "m",           "i,x,s,r,p,d,w,W", "m", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print CSV"),  _("Print CSV to stdout"), "CSV - Comma Separated Values",  "csv",         "i,x,s,r,p,d,w,W", "csv", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Print CSV-semicolon"),  _("Print CSV-semicolon to stdout"), "CSV - Comma Separated Values","csv-semicolon","i,x,s,r,p,d,w,W", "csv-semicolon", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Count"),      _("Print node count"),    NULL,  "c",           "i,x,r",       "c", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Key Name"),   _("Print key name"),      NULL,  "k",           "i,x,r",       "k", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Type"),       _("Print type"),          NULL,  "t",           "i,x,r",       "t", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Paths"),      _("Print all matching paths."),NULL,  "paths",  "i,x,r",       "paths", NULL},
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,_("Format"),     _("Print Data Format."),  NULL,  "f",           "i,x,r",       "f", NULL},
    {"oiwg", 0,     _("Misc"),          _("General options"),         NULL,               "h|X|V",       "v",           "h,X,V,v", NULL },/* just show in documentation */
    {"",0,0,0,0,0,0,0,0}
  };

#ifndef OYJL_USE_GETTEXT
  //fprintf(stderr,"%s myMain() static OyjlArgs structures initialised.\n", oyjlPrintTime(OYJL_TIME|OYJL_BRACKETS, oyjlNO_MARK));
#endif
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl", _("Oyjl Json Manipulation"), _("Light weight JSON parse and manipulation tool"),
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       NULL,
#endif
                                       sections, oarray, groups, &state );
  //fprintf(stderr,"%s myMain() oyjlUi_s created.\n", oyjlPrintTime(OYJL_TIME|OYJL_BRACKETS, oyjlNO_MARK));
  if( state & oyjlUI_STATE_EXPORT && !ui)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stdout, "%s\n\tman oyjl\n\n", _("For more information read the man page:") );
    goto clean_main;
  }

  if(ui && verbose)
  {
    char * json = oyjlOptions_ResultsToJson( ui->opts, OYJL_JSON );
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
    free(json);
    puts( json_commands );
    free(json_commands);
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
    oyjlTermColorInit( OYJL_RESET_COLORTERM | OYJL_FORCE_COLORTERM );
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
    if(!json && !yaml && !xml && !csv && !csv_semicolon && !paths && !key && !count && !type && !format)
      json = 1;

    if(i_filename)
    {
      int i;
      i_files = oyjlOptions_ResultsToList( ui->opts, "i", &i_files_n );
      if(verbose)
      for(i = 0; i < i_files_n; ++i)
        fprintf(stderr, "%s going to union:\t\"%s\"\n", oyjlPrintTime(OYJL_BRACKETS, oyjlNO_MARK), i_files[i] );

      if(i_files_n > 1 && (!(json || xml || yaml || csv || csv_semicolon || paths || help || version) || set))
      {
        char * t = oyjlStringCopy(oyjlTermColor(oyjlBOLD, set?"--set":argv[1]), 0);
        oyjlOption_s * o = oyjlOptions_GetOptionL( ui->opts, "input", 0/* flags */ );
        char * t2 = oyjlOption_PrintArg_(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
        oyjlOptions_Print_( ui->opts, 1 );
        fprintf( stderr, "%s %s %d - %s: %s\n", oyjlTermColor(oyjlRED,_("Usage Error:")), t2, i_files_n, _("Too many input files for option"), t );
        free(t);
        return 1;
      }
      if(set && (!xpath || count || key || type || paths || format))
      {
        if(count || key || type || paths || format)
        {
          char * t = oyjlStringCopy(oyjlTermColor(oyjlBOLD, argv[1]), 0);
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "--set option not allowed with: %s", OYJL_DBG_ARGS, t );
          free(t); t = NULL;
        }
        else
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "--set option needs as well a --xpath option", OYJL_DBG_ARGS );
        return 1;
      }

      if(json || xml || yaml || csv || csv_semicolon || count || key || type || paths)
      {
        const char * filename;
        char * first_path = NULL;
        int flags = detect ? OYJL_NUMBER_DETECTION : 0;
        oyjl_val root_union = i_files_n > 1 && !paths ? oyjlTreeNew("") : NULL;
        if(detect && strcmp(detect, ",") == 0)
          flags |= OYJL_DECIMAL_SEPARATOR_COMMA;
        for(i = 0; i < i_files_n; ++i)
        {
          int path_list_n = 0, j;
          char ** path_list = NULL;
          int type;

          filename = i_files[i];

          type = oyjlDataFormat(filename);
          if(type == 7 /* JSON */ ||
             type == 8 /* XML */  ||
             type == 9 /* YAML */ ||
             type == 10 /* C */)
            text = oyjlStringCopy( filename, 0 );
          else
            text = oyjlReadFile( filename, OYJL_IO_STREAM, &size );
          if(!text)
          {
            error = 1;
            goto clean_main;
          }
          value = oyjlTreeParse2_( text, size, xpath, key || set ? &first_path : NULL, !paths ? &root : NULL, root_union ? &path_list : NULL, try_format, paths, filename, ui->opts, flags, verbose );
          while(path_list && path_list[path_list_n]) ++path_list_n;

          if(root_union)
          for(j = 0; j < path_list_n; ++j)
          {
            const char * p = path_list[j];
            oyjl_val src = oyjlTreeGetValue( root, 0, p ),
                     v = oyjlTreeGetValue( root_union, OYJL_CREATE_NEW, p );
            oyjlValueCopy( v, src );
          }

          oyjlStringListRelease( &path_list, path_list_n, free );
          if(text) { free(text); text = NULL; }
          if(root_union)
          {
            oyjlTreeFree(root);
            root = NULL;
            value = NULL;
          }
        }

        if(root_union)
          value = root = root_union;
        root_union = NULL;

        if(set)
        {
          if(value)
            oyjlValueSetString( value, set );
          else
            oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "obtained no leaf for xpath \"%s\" from JSON:\t\"%s\"",
                           OYJL_DBG_ARGS, xpath, i_filename );
        }

        if(key)
          fprintf(stdout,"%s\n", first_path);

        if(text) { free(text); text = NULL; }
        if(plain)
        {
          if(strcasecmp( plain, "html" ) == 0)
            flags |= OYJL_HTML;
          else
            flags |= OYJL_NO_MARKUP;
        }

        if(json)
          text = oyjlTreeToText( set ? root : value, OYJL_JSON | flags );
        else if(yaml)
        {
          text = oyjlTreeToText( set ? root : value, OYJL_YAML | flags );
          oyjlStringAdd( &text, 0,0, "\n" );
        }
        else if(xml)
        {
          text = oyjlTreeToText( set ? root : value, OYJL_XML | flags );
          if(text && strcmp( text, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" ) == 0 )
          {
            free(text); text = NULL;
          }
          else
            oyjlStringAdd( &text, 0,0, "\n" );
        } else if(csv)
        {
          text = oyjlTreeToText( set ? root : value, OYJL_CSV | flags );
          if(!text || !text[0]) fprintf(stderr, "%s contains no table:\t\"%s\"\n", oyjlTermColor(oyjlRED,"Usage Error:"), i_filename);
        }
        else if(csv_semicolon)
        {
          text = oyjlTreeToText( set ? root : value, OYJL_CSV_SEMICOLON | flags );
          if(!text || !text[0]) fprintf(stderr, "%s contains no table:\t\"%s\"\n", oyjlTermColor(oyjlRED,"Usage Error:"), i_filename);
        }
        if(verbose)
          fprintf(stderr, "%s file to text:\t\"%s\" %d\n", oyjlPrintTime(OYJL_BRACKETS, oyjlNO_MARK), i_filename, text?(int)strlen(text):0);
        if(text)
        {
          if(wrap)
          {
            char * tmp = NULL;
            if(strcmp(wrap,"oiJS") == 0 || strcmp(wrap,"oiJS.bin") == 0)
            {
              int size = 0, j,
                  binary = strcmp(wrap,"oiJS.bin") == 0,
                  flags = verbose?OYJL_OBSERVE:0;
              oyjl_val oiJS = oyjlTreeSerialise( root, flags, &size );
              if(binary)
              {
                fprintf( stderr, "size: %d\n", size );
                fwrite( oiJS, sizeof(char), size, stdout );
                free(text);
                text = NULL;
              }
              else
              {
                unsigned char * s = (unsigned char*) oiJS;
                oyjl_str t = oyjlStr_New( 5*size + 80, malloc,free );
                oyjlStr_Add( t, "unsigned char %s_oiJS[] = {\n", wrap_name );
                for( i = 0; i < size; i+=16 )
                {
                  for( j = 0; i+j < size && j < 16; ++j )
                    oyjlStr_Add( t, "%3d%s%s", s[i+j], i+j < size-1?",":"", j == 7?" ":"" );
                  if(i+j < size+1 )
                    oyjlStr_Add( t, "\n" );
                }
                oyjlStr_Add( t, "};\n" );
                free(text);
                text = oyjlStr_Pull( t );
                oyjlStr_Release( &t );
              }
              free(oiJS);
            } else
            if(strcmp(wrap,"C") != 0)
            {
              fprintf(stderr,"%sERROR: Only -w C is supported.\n", oyjlBT(0));
              error = 1;
              goto clean_main;
            }
            else
            {
              if(!wrap_name || !wrap_name[0] || strchr(wrap_name,'-') != NULL)
              {
                fprintf(stderr, "%s%s\t\"%s\"\n", oyjlBT(0), oyjlTermColor(oyjlRED,_("Usage Error:")), wrap_name);
                error = 1;
                goto clean_main;
              }
              oyjlStringReplace( &text, "\\", "\\\\", NULL,NULL );
              oyjlStringReplace( &text, "\"", "\\\"", NULL,NULL );
              oyjlStringReplace( &text, "\n", "\\\n", NULL,NULL );
              oyjlStringAdd( &tmp, malloc, free, "#define %s_json \"%s\"\n", wrap_name, text );
              free(text); text = tmp; tmp = NULL;
            }
          }

          if(text)
          {
            fwrite( text, sizeof(char), strlen(text), stdout );
            fflush(stdout);
            free(text);
            text = NULL;
          }
        }
        if(verbose)
          fprintf(stderr, "\n%s file to stdout:\t\"%s\"\n", oyjlPrintTime(OYJL_BRACKETS, oyjlNO_MARK), i_filename);
      }
    }

    if(format)
    {
      int type = oyjlDataFormat( i_filename );
      const char * r;
      if(7 <= type && type <= 10)
        text = oyjlStringCopy( i_filename, 0);
      else
        text = oyjlReadFile( i_filename, OYJL_IO_STREAM, &size );
      type = oyjlDataFormat(text);
      r = oyjlDataFormatToString(type);
      fprintf(stdout, "%s\n", r);
      if(text) { free(text); text = NULL; }
    }

    if(value)
    {
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
    }

    if(root) oyjlTreeFree( root );
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
  oyjlStringListRelease( &i_files, i_files_n, free );
  oyjlUi_Release( &ui );

  return error;
}

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;
  int use_gettext = 0;
  const char * locale = NULL, * loc = NULL, * lang = NULL;
  oyjlTranslation_s * trc = NULL;

#ifdef __ANDROID__
  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start QML */
  environment = environ;
#else
  environment = envv;
#endif

  /* language needs to be initialised before setup of data structures */
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
#ifdef OYJL_HAVE_LOCALE_H
  locale = loc = setlocale(LC_ALL,"");
#endif
  if(*oyjl_debug > 1)
    fprintf(stderr,OYJL_DBG_FORMAT "loc: %s locale: %s\n", OYJL_DBG_ARGS, loc, locale );

  lang = getenv("LANG");
  if((!loc && lang) || (loc && lang && strcmp(loc,lang) != 0))
  {
    locale = lang;
#ifdef OYJL_HAVE_LOCALE_H
    locale = setlocale(LC_ALL, locale);
#endif
  }
  if(!loc && lang)
  {
    fprintf( stderr, "%s", oyjlTermColor(oyjlRED,"Usage Error:") );
    fprintf(stderr,OYJL_DBG_FORMAT "", OYJL_DBG_ARGS);
    fprintf( stderr, " Environment variable possibly not correct. Translations might fail - LANG=%s\n", oyjlTermColor(oyjlBOLD,lang) );
  }
  else
    if(*oyjl_debug > 1)
      fprintf(stderr,OYJL_DBG_FORMAT "lang: %s loc: %s locale: %s\n", OYJL_DBG_ARGS, lang, loc, locale );

  if(locale)
    trc = oyjlTranslation_New( locale, 0, 0,0,0,0, *oyjl_debug > 1?OYJL_OBSERVE:0 );
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, &trc, NULL );
  oyjlTranslation_Release( &trc );
  if(*oyjl_debug > 1)
    fprintf(stderr,OYJL_DBG_FORMAT "oyjlLang: %s use_gettext: %d LANG:%s\n", OYJL_DBG_ARGS, oyjlLang(""), use_gettext, lang);

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif

  oyjlLibRelease();

  return 0;
}


