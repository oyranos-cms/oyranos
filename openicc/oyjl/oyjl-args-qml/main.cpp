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
  const char * render = "gui";

#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_ALL,"");
#endif

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

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s A_choices[] = {{_("Load a UI JSON declaration from file"),_("oyjl-args-qml -i oyjl-ui-text.json -c oyjl-command.json"),NULL, NULL},
                                    {_("Load a UI JSON declaration from tool"),_("oyjl -X json+command | oyjl-args-qml -i - -c +"),NULL, NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{oyjlStringCopy("oyjl(1) oyjl-args(1) oyjl-translate(1)",NULL),NULL,               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};
  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "i", "input", _("Input"), NULL,_("JSON UI Description"), NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &json} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "c", "command", _("Command"), NULL, _("JSON Command"), NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &command} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "o", "output", _("Output"), NULL,_("Results JSON"), NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &output} },
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG,"h", "help", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE,{0}, oyjlSTRING, {.i = &help} },
    {"oiwi", 0,     "v", "verbose", NULL, _("Verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &verbose} },
    {"oiwi", 0,     "X", "export",  NULL, NULL,         NULL,                NULL, NULL,          oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &exportX} },
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "R", "render", NULL, _("Render"), _("Render"), NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &render} },
    {"oiwi", 0,     "A", "man-examples",NULL,_("EXAMPLES"),NULL,             NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices={(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {0}},
    {"oiwi", 0,     "S", "man-see_also",NULL,_("SEE ALSO"),NULL,             NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices={(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {0}},
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {NULL},oyjlNONE,{NULL}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("GUI"),  _("QML UI"),          NULL, "i",       "c,o,v",  "i,c,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h,X",     "v",      "h,v,X" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-args-qml", _("Oyjl Args QML Viewer"), _("Small JSON UI tool using libOyjl and QML"), "logo",
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
  oyjlArgsRender( argc, argv, json, command, output, debug, ui, NULL );

  oyjlUi_Release( &ui);
  free(S_choices[0].nick);

  return 0;
}
