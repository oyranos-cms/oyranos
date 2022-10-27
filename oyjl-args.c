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
#include "oyjl_i18n_internal.h"

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
  const char * test = 0;
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
  oyjlOptionChoice_s F_choices[] = {{_("Synopsis"),_("Tools follows syntax rules."),_("OyjlArgs checks and enforces rules. These rules are expresses in each synopsis line."), _("A tool can provide different modules with each using different rules, expressed in different synopsis lines.")},
                                    {_("prog -o"),_("Simple tool with only one option."), _("The option consists of one single letter and thus starts with a single dash."),_("The command line tool is in the following examples called \"prog\" for simplicity.")},
                                    {_("prog --option"),_("Simple tool with only one option."), _("The option consists of more than one letter starting with two dashs."),_("This is called a long option name. The same option can be triggered by the single letter name or the long option name. Inside the Synopsis line only one form is noticed.")},
                                    {_("prog -o=ARG --name=one|two|..."),_("Simple tool with two options, which both accept arguments."), _("The argument can be representet by a big letter content hint, like FILE, NUMBER etc. Or it is a collection of pipe separated choices."),_("The later --name option names a few choices and shows with the immediately following three dots, that the choices are not exclusive and might be edited. OyjlArgs checks for args following the option name even without the equal sign '='.")},
                                    {_("prog -o [-v]"),_("Tool with two differently required options."), _("By default all options are required like the -o one and is mandatory. The second option is enclosed in squared brackets is not required but might be used and thus is optional. "),""},
                                    {_("prog -h[=synopsis|...] [--option[=NUMBER]]"),_("Tool options, which might be follwed by an argument."), "",""},
                                    {_("prog -f=FILE ... [-i=FILE ...]"),_("Tool options with three dots after empty space ' ...' can occure multiple times."), _("Command line example: prog -f=file1.ext -f=file2.ext -f file3.ext"),""},
                                    {_("prog | [-v]"),_("Tool without option requirement."), _("The tool can be called without any option. But one optional option might occure."),""},
                                    {_("prog sub -o [-i] [-v]"),_("Tool with sub tool option syntax."), _("The tool has one long mandatory option name without leading dashes."),_("This style is used sometimes for one complex tool for multiple connected tasks. The sub tool sections help in separating the different tool areas.")},
                                    {_("prog [-v] FILE ..."),_("Tool with default option free style arguments."), _("The @ option argument(s) are mentioned as last in order to not confuse with sub tool options or with option arguments."),""},
                                    {_("Option syntax"),_("The options are described each individually in more detail."), _("One letter option name and long name forms are show separated by the pipe symbol '|'."),_("E.g. -o|--option")},
                                    {_("-k|--kelvin=NUMBER        Lambert (NUMBER:0 [≥0 ≤25000 Δ100])"),_("Line for a number argument."), _("The single letter and long option names are noticed and followed by the number symbolic name. After that the short name of the option is printed. After the opening brace is the symbolic name repated, followed by the default value. In square brackets follow boundaries ≥ minimal value, ≤ maximal value and Δ the step or tick."), ""},
                                    {_("Command line parser"),_("The OyjlArgs command line parser follows the above rules."), "",""},
                                    {_("prog -hvi=file.ext"),_("Options can be concatenated on the command line."), _("The OyjlArgs parser takes each letter after a single dash as a separated option."),_("The last option can have a argument.")},
                                    {_("prog -i=file-in.ext -o file-out.ext"),_("Arguments for options can be written with equal sign or with empty space."), "",""},
                                    {_("prog -i=file1.ext -i file2.ext -i file3.ext"),_("Multiple arguments for one option need each one option in front."), "",""},
                                    {NULL,NULL,NULL,NULL}};
  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "i", "input", NULL, _("input"), _("Set Input"), _("For C code output (default) and --completion-bash output use -X=export JSON. For --render=XXX use -X=json JSON."), _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &file}, NULL },
    {"oiwi", 0,    NULL, "c-stand-alone",NULL, _("C Stand Alone"),_("Generate C code for oyjl_args.c inclusion."), _("Omit libOyjlCore reference."), NULL, oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &oyjl_args}, NULL },
    {"oiwi", 0,    NULL, "completion-bash",NULL, _("Completion Bash"),_("Generate bash completion code"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &completion_bash}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE|OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_ACCEPT_NO_ARG,  NULL,"test",    NULL, _("Test"),    _("Generate test Args Export"), NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0},oyjlSTRING, {.s = &test}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE|OYJL_OPTION_FLAG_EDITABLE,  "o",NULL,    NULL, "O",    NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0},oyjlSTRING, {0}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,  NULL, "option",    NULL, "Option",    NULL, NULL, NULL, oyjlOPTIONTYPE_NONE, {0},oyjlNONE, {0}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,  "t",NULL,    NULL, "T",    NULL, NULL, NULL, oyjlOPTIONTYPE_DOUBLE, {0},oyjlDOUBLE, {.d=&d}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE|OYJL_OPTION_FLAG_EDITABLE,  NULL, "format",    NULL, "Format",    NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0},oyjlSTRING, {.s=&s}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG,"h", "help", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE,{0}, oyjlSTRING, {.s = &help}, NULL },
    {"oiwi", 0,    NULL, "synopsis",NULL, NULL,         NULL,                NULL, NULL,          oyjlOPTIONTYPE_NONE, {0},oyjlNONE, {0}, NULL },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &verbose}, NULL },
    {"oiwi", 0,     "V", "version", NULL, _("version"), _("Version"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {0},oyjlINT, {.i = &version}, NULL },
    {"oiwi", 0,     "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export}, NULL },
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("EXAMPLES"), NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,{}, NULL},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("SEE ALSO"),NULL,                      NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc )}, oyjlNONE,{}, NULL},
    {"oiwi", 0,                          "F","man-format",  NULL,       _("Format"),NULL,                      NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)F_choices, sizeof(F_choices), malloc )}, oyjlNONE,{}, NULL},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "R", "render",  NULL, NULL,  NULL,         NULL, NULL,          oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &render}, NULL},
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{},NULL}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Convert"),_("Generate source code"),NULL, "i", "c-stand-alone,completion-bash,v","i,c-stand-alone,completion-bash", NULL }, /* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h,X,R,V",   "i,v",      "h,X,R,V,v", NULL }, /* just show in documentation */
    {"",0,0,0,0,0,0,0,0}
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
    if(strcmp(test,"1") == 0 || strcmp(test,"json") == 0 || strcmp(test, "json+command") == 0)
    {
      puts("{");
      if(strcmp(test, "json+command") == 0)
      {
        puts("  \"comment\": \"Use *command_set* to name a command to be combined with the supplied arguments. With the -R|--render=gui|cli option a renderer can detect the calling command.\",");
        puts("  \"command_set\": \"oyjl-args\",");
        puts("  \"comment\": \"Use *command_get* to name a command to obtain the UI JSON for a UI update. Without no UI update will be performed.\",");
        puts("  \"command_get\": \"oyjl-args\",");
        puts("  \"comment\": \"Use *command_get_args* to name custom args together with *command_get*. Without *command_get_args* \\\"-X=json+command\\\" will be used.\",");
        puts("  \"command_get_args\": [\"-X\",\"json+command\"],");
      }
      puts("  \"comment\": \"Following is the path to the Oyjl UI render array org/freedesktop/oyjl/modules\",");
      puts("  \"comment\": \"1. path part\",");
      puts("  \"org\": {");
      puts("    \"comment\": \"2. path part\",");
      puts("    \"freedesktop\": {");
      puts("      \"comment\": \"3. path part\",");
      puts("      \"oyjl\": {");
      puts("        \"comment\": \"4. path part\",");
      puts("        \"modules\": [{");
      puts("            \"comment\": \"*oyjl_module_api_version*: name the version\",");
      puts("            \"oyjl_module_api_version\": \"1\",");
      puts("            \"comment\": \"*type* of the source can be *tool* for command line or *module* for other UIs\",");
      puts("            \"type\": \"tool\",");
      puts("            \"comment\": \"*label* of the modules *type*\",");
      puts("            \"label\": \"Werkzeug\",");
      puts("            \"comment\": \"*nick* is a short four byte ID for *type*:*module* or the command line name for *type*:*tool*\",");
      puts("            \"nick\": \"oyjl-test\",");
      puts("            \"comment\": \"*name* translated display name for tool bars, app lists ...\",");
      puts("            \"name\": \"Test OyjlArgsUi\",");
      puts("            \"comment\": \"*description* translated display short explanation in one sentence.\",");
      puts("            \"description\": \"Test file for various Oyjl Args Ui elements.\",");
      puts("            \"comment\": \"*logo* icon file name to search on disk or path. Typical without ending of PNG or SVG.\",");
      puts("            \"logo\": \"oyjl\",");
      puts("            \"comment\": \"*information* object is a array of sections for dynamic header information. We describe here a particular tool/module. Each property object contains at least one 'type' and one 'name' key. All values shall be strings. *description* keys are optional. If they are not contained, fall back to *name*. Well known *type* values are *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *development*, *oyjl_modules_author*, *documentation*, *date* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier.\",");
      puts("            \"information\": [{");
      puts("                \"comment\": \"*type* marker. Well known types are mentioned in the information comment above.\",");
      puts("                \"type\": \"version\",");
      puts("                \"comment\": \"*label* translated *type* for display, in case this section *type* is not well known. It shall explain to users what kind of information is shown.\",");
      puts("                \"label\": \"Version\",");
      puts("                \"comment\": \"*name* translated display item. The actual pice of information for the *type*.\",");
      puts("                \"name\": \"1.0.0\"");
      puts("              }],");
      puts("            \"comment\": \"*groups* is a array of single groups containing the UI.\",");
      puts("            \"groups\": [{");
      puts("                \"comment\": \"*name* translated title.\",");
      puts("                \"name\": \"Group\",");
      puts("                \"comment\": \"*description* translated longer text.\",");
      puts("                \"description\": \"Simple Group\",");
      puts("                \"comment\": \"*help* translated usage explanations.\",");
      puts("                \"help\": \"Simple Group help text\",");
      puts("                \"synopsis\": \"<strong>oyjl-args</strong> <strong>-o=ARGUMENT</strong> ... [<strong>--option</strong>] [<strong>-t</strong>=<em>ANGLE_IN_DEGREE</em>] [<strong>--format</strong>=<em>ARGUMENT</em>]\",");
      puts("                \"comment\": \"*mandatory* list of mandatory options from a one letter option or multi letter option for this group of associated options; one single option here makes a subcommand and is usualy easier to understand.\",");
      puts("                \"mandatory\": \"o\",");
      puts("                \"comment\": \"*optional* list of non mandatory options from a one letter option or multi letter option for this group of associated options.\",");
      puts("                \"optional\": \"option,t,format\",");
      puts("                \"comment\": \"*changed* names altered option with the actual value. It is useful for preserving options during a UI update.\",");
      puts("                \"changed\": \"-o=ARG2,--option,-t=-3\",");
      puts("                \"comment\": \"*options* the group interactive options.\",");
      puts("                \"options\": [{");
      puts("                    \"comment\": \"*key* one letter UTF-8 e.g. 'm' on command line shown as '-m' or string without white space, e.g. 'my-option' option name on command line typical shown as '--my-option'.\",");
      puts("                    \"key\": \"o\",");
      puts("                    \"comment\": \"*name* translated display string for key.\",");
      puts("                    \"name\": \"Option Name\",");
      puts("                    \"comment\": \"*description* translated elaborating display string for *key*.\",");
      puts("                    \"description\": \"Description of option with single dash and single letter\",");
      puts("                    \"comment\": \"*help* translated display string for *key* containing addional usage information.\",");
      puts("                    \"help\": \"Option help text\",");
      puts("                    \"comment\": \"*value_name* is a optional string hinting the options arguments usage in preferedly upper letters. Or it contains a list of valid arguments in usual lower letter or numbers divided by '|'.\",");
      puts("                    \"value_name\": \"ARGUMENT\",");
      puts("                    \"comment\": \"*repetition* marks the *key* option as accepting multiple times occuring.\",");
      puts("                    \"repetition\": \"1\",");
      puts("                    \"comment\": \"*choices* array\",");
      puts("                    \"choices\": [{");
      puts("                        \"comment\": \"*nick* value\",");
      puts("                        \"nick\": \"ARG1\",");
      puts("                        \"comment\": \"*name* short display string\",");
      puts("                        \"name\": \"ARG1\"");
      puts("                      },{");
      puts("                        \"nick\": \"ARG2\",");
      puts("                        \"name\": \"ARG2\"");
      puts("                      }],");
      puts("                    \"comment\": \"*default* recommended value\",");
      puts("                    \"default\": \"ARG2\",");
      puts("                    \"comment\": \"*changed* names preselected actual value\",");
      puts("                    \"changed\": \"ARG2\",");
      puts("                    \"comment\": \"*type* accepted is here *string* for editable choices, *choice* for fixed choices, *double* for numbers, *bool* for yes:'1' no:'0' options. \",");
      puts("                    \"type\": \"string\"");
      puts("                  },{");
      puts("                    \"key\": \"option\",");
      puts("                    \"name\": \"Long Option\",");
      puts("                    \"description\": \"Option with double dash and multiple letters [a-z,A-Z,0-9,-,+]\",");
      puts("                    \"default\": \"1\",");
      puts("                    \"changed\": \"1\",");
      puts("                    \"comment\": \"*type* accepted is *bool* for yes:'1' no:'0'. For simplicity values are made of strings \\\"0\\\" and \\\"1\\\".\",");
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
      puts("                    \"comment\": \"*start* for *type*:*double* is the minimum value.\",");
      puts("                    \"start\": -18,");
      puts("                    \"comment\": \"*end* for *type*:*double* is the maximum value.\",");
      puts("                    \"end\": 18,");
      puts("                    \"comment\": \"*tick* for *type*:*double* is a UI hint on how to in-or decrease the value.\",");
      puts("                    \"tick\": 1,");
      puts("                    \"comment\": \"*type* accepted is *double*. Values are made of strings and need to be string converted to numbers for applications.\",");
      puts("                    \"type\": \"double\"");
      puts("                  },{");
      puts("                    \"key\": \"format\",");
      puts("                    \"name\": \"Format\",");
      puts("                    \"description\": \"Option with double dash and multiple letters [a-z,A-Z,0-9,-,+]\",");
      puts("                    \"value_name\": \"ARGUMENT\",");
      puts("                    \"choices\": [{");
      puts("                        \"nick\": \"ARG1\",");
      puts("                        \"name\": \"ARG1\"");
      puts("                      },{");
      puts("                        \"nick\": \"ARG2\",");
      puts("                        \"name\": \"ARG2\"");
      puts("                      }],");
      puts("                    \"comment\": \"*type* accepted is *choice*. Values are non editable.\",");
      puts("                    \"type\": \"choice\"");
      puts("                  }]");
      puts("              }]");
      puts("          }]");
      puts("      }");
      puts("    }");
      puts("  }");
      puts("}");
    } else
    if(strcmp(test,"export") == 9)
    {
    }
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
      oyjlTermColorInit( OYJL_RESET_COLORTERM | OYJL_FORCE_COLORTERM );
      oyjlArgsRender( argc, argv, text, NULL,NULL, debug, ui, NULL );
#else
      fprintf( stderr, "No render support compiled in. Use a GUI use -X json and load into oyjl-args-qml viewer." );
#endif
    } else
    {
      oyjl_val json = oyjlTreeParse2( text, 0, __func__, NULL );
      char * sources = oyjlUiJsonToCode( json, completion_bash ? OYJL_COMPLETION_BASH : oyjl_args ? OYJL_SOURCE_CODE_C | OYJL_WITH_OYJL_ARGS_C : OYJL_SOURCE_CODE_C );
      fprintf( stderr, "wrote %d to stdout\n", sources&&strlen(sources)?(int)strlen(sources):0 );
      if(sources)
      {
        puts( sources );
        free(sources);
        sources = NULL;
      }
    }
  }

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
  oyjlUi_Release( &ui);

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

