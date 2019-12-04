/** @file mini-app.c
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  Copyright 2018-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl Args + Qml example
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2019/11/28
 */

#include "oyjl.h"
#include "oyjl_version.h"

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
  int output = 0;
  const char * file = NULL;
  int file_count = 0;
  int show_status = 0;
  int gui = 0;
  int help = 0;
  int verbose = 0;
  int error = 0;
  int state = 0;

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_modules_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  NULL,                  _("The example tool demontrates the usage of the libOyjl API's and links to libOyjlArgsQml.")},
    {"oihs", "date",          NULL,  "2019-11-28T12:00:00", _("November 28, 2019")},
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
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &show_status} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "@", "",        NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlINT, {.i = &file_count} },
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output} },
    /* The --gui option can be hidden and used only internally. */
    {"oiwi", 0,     "G", "gui",     NULL, _("gui"),     _("GUI"),            NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &gui} },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           _("Print help for command line style interface"), NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &help} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), NULL,                NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &verbose} },
    {"oiwi", 0,     "e", "error",   NULL, _("error"),   NULL,                NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &error} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {0},0,{0}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode2"),_("Any arg mode"),    NULL, "@",       "o,v",    "@,o" },/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "G,o,v",  "i,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h,e",       "v",    "h,v,e" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "mApp", "mini App", _("mini OyjlArgsQml example"), ":/images/logo",
                                       sections, oarray, groups_no_args, &state );
  /* done with options handling */

  static int main_count = 0;
  if(verbose)
  {
    ++main_count;
    fprintf(stderr, "started %s %d\n", __func__, main_count );
  }

  /* GUI boilerplate */
  if(ui && gui)
  {
    int debug = verbose;
    oyjlArgsQmlStart( argc, argv, NULL, debug, ui, myMain );
  }
  else if(ui)
  {
    /* get the results and do something with it */
    const char * result = oyjlOptions_ResultsToJson(ui->opts);
    if(verbose)
        fprintf(stdout, "%s\n", result);
    if(output)
        fprintf( stdout, "output:\t%d\n", output );
    if(file)
        fprintf( stdout, "file:\t%s\n", file );
    if(file_count)
    {
        fprintf( stdout, "file_count:\t%d\n", file_count );
        int n = 0;
        char ** input = oyjlOptions_ResultsToList( ui->opts, "@", &n );
        for(int i = 0; i < n; ++i)
            fprintf( stdout, "[%d]:\t%s\n", i, input[i] );
    }
    if(error)
        fprintf( stderr, "send to stderr\n" );
  }

  oyjlUi_Release( &ui);

  if(verbose)
  {
    fprintf(stderr, "finished %s %d\n", __func__, main_count );
    --main_count;
  }

  return 0;
}

int main( int argc_ OYJL_UNUSED, char**argv_ OYJL_UNUSED)
{
  int argc = argc_;
  const char * argv[] = {"test",argc_>=2?argv_[1]:NULL,argc_>=3?argv_[2]:NULL,argc_>=4?argv_[3]:NULL,argc_>=5?argv_[4]:NULL, NULL};
  if(argc > 4) argc = 4;

#ifdef __ANDROID__
  setenv("COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */
  argv[argc++] = "--gui"; /* start QML */
#endif

  fprintf(stderr, "started %s\n", __func__ );

  myMain(argc, argv);

  fprintf(stderr, "finished %s\n", __func__ );

  return 0;
}


