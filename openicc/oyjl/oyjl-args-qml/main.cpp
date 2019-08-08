/** @file main.cpp
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  basic QML handling and environment setup
 */

#include "oyjl.h"
#include "oyjl_i18n.h"


int main(int argc, const char *argv[])
{
  const char * json = NULL,
             * command = NULL,
             * output = NULL;
  int help = 0;
  int verbose = 0;
  int state = 0;
  const char * exportX = NULL;

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_modules_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("The tool graphicaly renders Oyjl JSON UI files using QML.")},
    {"oihs", "date",          NULL,  "2019-05-23T12:00:00", _("May 23, 2019")},
    {"",0,0,0,0}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "i", "input", NULL, NULL,_("JSON UI Description"), NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s = &json} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "c", "command", NULL, NULL, _("JSON Command"), NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s = &command} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "o", "output", NULL, NULL,_("Results JSON"), NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s = &output} },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose} },
    {"oiwi", 0,     "X", "export",  NULL, NULL,         NULL,                NULL, NULL,          oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s = &exportX} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},oyjlNONE,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("GUI"),  _("QML UI"),          NULL, "i",       "c,o,v",  "i,c,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h",       "v",      "h,v,X" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiAQ", "oyjl-args-qml", _("Small JSON UI tool using libOyjl and QML"), "logo",
                                       sections, oarray, groups_no_args, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      exportX)
    return 0;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyjl-args-qml\n\n", _("For more information read the man page:"));
    return 0;
  }
  if(!ui) return 1;
  /* done with options handling */

  if(ui && verbose)
    oyjlOptions_PrintHelp( ui->opts, ui, 4, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );

  int debug = 0;
  oyjlArgsQmlStart2( argc, argv, json, command, output, debug, ui, NULL );

  oyjlUi_Release( &ui);

  return 0;
}
