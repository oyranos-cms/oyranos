/** @file oyjl-args.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2019-2022  Kai-Uwe Behrmann
 *
 *  @brief    Oyjl Args Tool
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2019/06/26
 */

#include "oyjl.h"
#include "oyjl_version.h"
extern char **environ;
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"

#include "oyjl_tree_internal.h"

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{

  const char * file = NULL;
  int oyjl_args = 0;
  int completion_bash = 0;
  int test = 0;
  double d = 0;
  const char * s = NULL;
  const char * export = NULL;
  const char * render = NULL;
  const char * help = NULL;
  int verbose = 0;
  int version = 0;
  int state = 0;

  /* handle options */
  oyjlUiHeaderSection_s * sections = oyjlUiInfo_( _("Tool to convert UI JSON description from *-X export* into source code."),
                                                 "2019-6-26T12:00:00", _("June 26, 2019") );

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s A_choices[] = {{_("Convert eXported developer JSON to C source"),_("oyjl-args -X export | oyjl-args -i -"),NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{"oyjl(1) oyjl-translate(1) oyjl-args-qml(1)","https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html",               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "i", "input", NULL, _("input"), _("Set Input"), _("For C code output (default) and --completion-bash output use -X=export JSON. For --render=XXX use -X=json JSON."), _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,    NULL, "c-stand-alone",NULL, _("C Stand Alone"),_("Generate C code for oyjl_args.c inclusion."), _("Omit libOyjlCore reference."), NULL, oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &oyjl_args} },
    {"oiwi", 0,    NULL, "completion-bash",NULL, _("Completion Bash"),_("Generate bash completion code"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &completion_bash} },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,  NULL,"test",    NULL, _("Test"),    _("Generate test Args Export"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &test} },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE|OYJL_OPTION_FLAG_EDITABLE,  "o",NULL,    NULL, "O",    NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0},oyjlSTRING, {0} },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,  NULL, "option",    NULL, "Option",    NULL, NULL, NULL, oyjlOPTIONTYPE_NONE, {0},oyjlNONE, {0} },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,  "t",NULL,    NULL, "T",    NULL, NULL, NULL, oyjlOPTIONTYPE_DOUBLE, {0},oyjlDOUBLE, {.d=&d} },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE|OYJL_OPTION_FLAG_EDITABLE,  NULL, "format",    NULL, "Format",    NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0},oyjlSTRING, {.s=&s} },
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG,"h", "help", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE,{0}, oyjlSTRING, {.s = &help} },
    {"oiwi", 0,    NULL, "synopsis",NULL, NULL,         NULL,                NULL, NULL,          oyjlOPTIONTYPE_NONE, {0},oyjlNONE, {0} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &verbose} },
    {"oiwi", 0,     "V", "version", NULL, _("version"), _("Version"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &version} },
    {"oiwi", 0,     "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export} },
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("EXAMPLES"), NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("SEE ALSO"),NULL,                      NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc )}, oyjlNONE,      {}},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "R", "render",  NULL, NULL,  NULL,         NULL, NULL,          oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &render} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Convert"),_("Generate source code"),NULL, "i", "c-stand-alone,completion-bash,v","i,c-stand-alone,completion-bash" }, /* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h,X,R,V",   "i,v",      "h,X,R,V,v" }, /* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-args", _("Ui to source code"), _("Convert UI JSON to C code using libOyjl"), "oyjl",
                                       sections, oarray, groups, &state );
  /* done with options handling */

  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyjl-args\n\n", _("For more information read the man page:") );
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
    puts( json_commands );
    goto clean_main;
  }

  /* Render boilerplate */
  if(ui && render && !file)
  {
#if !defined(NO_OYJL_ARGS_RENDER)
    int debug = verbose;
    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );
#else
    fprintf( stderr, "No render support compiled in. Use a GUI use -X json and load into oyjl-args-qml viewer." );
#endif
  } else
  if(ui && test)
  {
    puts("{");
    puts("  \"command_set\": \"oyjl-args\",");
    puts("  \"command_get\": \"oyjl-args\",");
    puts("  \"command_get_args\": [\"-X\",\"json+command\"],");
    puts("  \"org\": {");
    puts("    \"freedesktop\": {");
    puts("      \"oyjl\": {");
    puts("        \"modules\": [{");
    puts("            \"oyjl_module_api_version\": \"1\",");
    puts("            \"type\": \"tool\",");
    puts("            \"label\": \"Werkzeug\",");
    puts("            \"nick\": \"oyjl-test\",");
    puts("            \"name\": \"Test OyjlArgsUi\",");
    puts("            \"description\": \"Test file for various Oyjl Args Ui elements.\",");
    puts("            \"logo\": \"oyjl\",");
    puts("            \"information\": [{");
    puts("                \"type\": \"version\",");
    puts("                \"label\": \"Version\",");
    puts("                \"name\": \"1.0.0\"");
    puts("              }],");
    puts("            \"groups\": [{");
    puts("                \"name\": \"Group\",");
    puts("                \"description\": \"Simple Group\",");
    puts("                \"help\": \"Simple Group help text\",");
    puts("                \"mandatory\": \"o\",");
    puts("                \"optional\": \"option,t,format\",");
    puts("                \"changed\": \"-o=ARG2,--option,-t=-3\",");
    puts("                \"options\": [{");
    puts("                    \"key\": \"o\",");
    puts("                    \"name\": \"Option\",");
    puts("                    \"description\": \"Option with single dash and single letter\",");
    puts("                    \"help\": \"Option help text\",");
    puts("                    \"value_name\": \"ARGUMENT\",");
    puts("                    \"choices\": [{");
    puts("                        \"nick\": \"ARG1\",");
    puts("                        \"name\": \"ARG1\"");
    puts("                      },{");
    puts("                        \"nick\": \"ARG2\",");
    puts("                        \"name\": \"ARG2\"");
    puts("                      }],");
    puts("                    \"default\": \"ARG2\",");
    puts("                    \"changed\": \"ARG2\",");
    puts("                    \"type\": \"string\"");
    puts("                  },{");
    puts("                    \"key\": \"option\",");
    puts("                    \"name\": \"Long Option\",");
    puts("                    \"description\": \"Option with double dash and multiple letters [a-z,A-Z,0-9,-,+]\",");
    puts("                    \"default\": \"1\",");
    puts("                    \"changed\": \"1\",");
    puts("                    \"type\": \"bool\",");
    puts("                    \"choices\": [{");
    puts("                        \"nick\": \"0\",");
    puts("                        \"name\": \"No\"");
    puts("                      },{");
    puts("                        \"nick\": \"1\",");
    puts("                        \"name\": \"Yes\"");
    puts("                      }]");
    puts("                  },{");
    puts("                    \"key\": \"t\",");
    puts("                    \"name\": \"Twilight\",");
    puts("                    \"description\": \"Set Twilight angle\",");
    puts("                    \"help\": \"0:sunrise/sunset|-6:civil|-12:nautical|-18:astronomical\",");
    puts("                    \"value_name\": \"ANGLE_IN_DEGREE\",");
    puts("                    \"default\": \"-3\",");
    puts("                    \"changed\": \"-3\",");
    puts("                    \"start\": -18,");
    puts("                    \"end\": 18,");
    puts("                    \"tick\": 1,");
    puts("                    \"type\": \"double\"");
    puts("                  },{");
    puts("                    \"key\": \"format\",");
    puts("                    \"name\": \"Format\",");
    puts("                    \"description\": \"Option with single dash and single letter\",");
    puts("                    \"value_name\": \"ARGUMENT\",");
    puts("                    \"choices\": [{");
    puts("                        \"nick\": \"ARG1\",");
    puts("                        \"name\": \"ARG1\"");
    puts("                      },{");
    puts("                        \"nick\": \"ARG2\",");
    puts("                        \"name\": \"ARG2\"");
    puts("                      }],");
    puts("                    \"type\": \"choice\"");
    puts("                  }]");
    puts("              }]");
    puts("          }]");
    puts("      }");
    puts("    }");
    puts("  }");
    puts("}");
  } else
  if(ui && !file)
  {
    const char * r = oyjlOptions_ResultsToJson(ui->opts, OYJL_JSON);
    fprintf(stdout, "%s", r?r:"----");
  }
  else if(file)
  {
    FILE * fp;
    int size = 0;
    char * text = NULL;
   
    if(strcmp(file,"-") == 0)
      fp = stdin;
    else
      fp = fopen(file,"rb");

    if(fp)
    {
      text = oyjlReadFileStreamToMem( fp, &size ); 
      if(fp != stdin) fclose( fp );
    }

    if(render)
    {    
#if !defined(NO_OYJL_ARGS_RENDER)
      int debug = verbose;
      oyjlArgsRender( argc, argv, text, NULL,NULL, debug, ui, NULL );
#else
      fprintf( stderr, "No render support compiled in. Use a GUI use -X json and load into oyjl-args-qml viewer." );
#endif
    } else
    {
      char error_buffer[128] = {0};
      oyjl_val json = oyjlTreeParse( text, error_buffer, 128 );
      char * sources = oyjlUiJsonToCode( json, completion_bash ? OYJL_COMPLETION_BASH : oyjl_args ? OYJL_SOURCE_CODE_C | OYJL_WITH_OYJL_ARGS_C : OYJL_SOURCE_CODE_C );
      fprintf( stderr, "wrote %d to stdout\n", sources&&strlen(sources)?(int)strlen(sources):0 );
      if(sources)
        puts( sources );
    }
  }

  oyjlUi_Release( &ui);

  fflush(stdout);

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

  return 0;
}

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;
  oyjlTr_s * trc = NULL;
  char * loc = NULL;
  char * lang = getenv("LANG");

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
#endif
#ifdef OYJL_HAVE_LOCALE_H
  loc = setlocale(LC_ALL,"");
#endif
  if(!loc)
  {
    fprintf( stderr, "%s", oyjlTermColor(oyjlRED,"Usage Error:") );
    fprintf( stderr, " Environment variable possibly not correct. Translations might fail - LANG=%s\n", oyjlTermColor(oyjlBOLD,loc) );
  }
  if(lang)
    loc = lang;
  if(loc)
    trc = oyjlTr_New( loc, 0,0,0,0,0,0 );
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, &trc, NULL );
  oyjlTr_Release( &trc );

  myMain(argc, (const char **)argv);

  oyjlLibRelease();

  return 0;
}

