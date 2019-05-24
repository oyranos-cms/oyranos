/** @file test.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2018-2019  Kai-Uwe Behrmann
 *
 *  @brief    Oyjl Args Qml test
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2019/04/18
 */

#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testArgs, "Options Qml handling", 1 );

#define OYJL_TEST_MAIN_SETUP  printf("\n    OyjlArgsQml Test Program\n");
#define OYJL_TEST_MAIN_FINISH printf("\n    OyjlArgsQml Test Program finished\n\n");
#include "oyjl_test_main.h"
#include "oyjl.h"
#include "oyjl_version.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"


/* --- actual tests --- */

oyjlTESTRESULT_e testVersion()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OYJL_VERSION );
  fprintf(zout, " runtime version:     %d\n", oyjlVersion(0) );

  if(OYJL_VERSION == oyjlVersion(0))
    result = oyjlTESTRESULT_SUCCESS;
  else
    result = oyjlTESTRESULT_FAIL;

  return result;
}

int testMain( int argc, const char ** argv )
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int output = 0;
  const char * file = NULL;
  int file_count = 0;
  int show_status = 0;
  int gui = 0;
  int help = 0;
  int verbose_ = 0;
  int state = 0;

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_modules_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("The example tool demontrates the usage of the libOyjl API's.")},
    {"oihs", "date",          NULL,  "2018-10-10T12:00:00", _("October 10, 2018")},
    {"",0,0,0,0}};

  /* declare some option choices */
  oyjlOptionChoice_s i_choices[] = {{"oyjl.json", _("oyjl.json"), _("oyjl.json"), ""},
                                    {"oyjl2.json", _("oyjl2.json"), _("oyjl2.json"), ""},
                                    {"","","",""}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("Print All"), ""},
                                    {"1", _("Print Camera"), _("Print Camera JSON"), ""},
                                    {"2", _("Print None"), _("Print None"), ""},
                                    {"","","",""}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     '#', "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &show_status} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     '@', "",        NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlINT, {.i = &file_count} },
    {"oiwi", 0,     'i', "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     'o', "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output} },
    {"oiwi", 0,     'g', "gui",     NULL, _("gui"),     _("GUI"),            NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &gui} },
    {"oiwi", 0,     'h', "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help} },
    {"oiwi", 0,     'v', "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "ov",     "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode2"),_("Any arg mode"),    NULL, "@",       "ov",     "@o" },/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "gov",    "io" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h",       "v",      "hv" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, &state );
  /* done with options handling */

  fprintf(stderr, "started %s\n", __func__ );

  if(ui && verbose)
    oyjlOptions_PrintHelp( ui->opts, ui, 4, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );

  if(gui)
  {
    int debug = 0;
    char * json = oyjlUi_ToJson( ui, 0 );
    const char * jcommands = "{\n\
  \"command_set\": \"dummy-callback-placeholder_needed-to-trigger-calling-Process-in-main.qml\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\"\n\
}";
    char * json_commands = strdup(jcommands);
    json_commands[strlen(json_commands)-2] = ',';
    json_commands[strlen(json_commands)-1] = '\000';
    if(!json) return result;
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] );
    free(json);
    json = json_commands;
    size_t size = oyjlWriteFile( "test.json",
                           json,
                           strlen(json) );
    oyjlArgsQmlStart( 0, NULL, json, debug, ui, testMain );
  }

  if(ui)
  {
    fprintf(stdout, "%s", oyjlOptions_ResultsToJson(ui->opts));
    result = oyjlTESTRESULT_SUCCESS;
  }

  oyjlUi_Release( &ui);

  fflush(stdout);
  fprintf(stderr, "finished %s\n", __func__ );

  return result;
}

oyjlTESTRESULT_e testArgs()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  int argc = 5;
  const char * argv[] = {"test","-v","--input","file-name.json","--gui", "-z"};

  fprintf(stdout, "\n" );

  result = testMain(argc, argv); 

  return result;
}

/* --- end actual tests --- */

