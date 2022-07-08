#ifdef USE_OYJL_H
#include "oyjl.h"
#else
#include "oyjl_args_base.h"
#endif
#define _(x) x
/* This function is called
 * * for executing the tool.
 */
int main( int argc, const char ** argv )
{
  int error = 0;
  int state = 0;
  const char * input = 0;
  int c_stand_alone = 0;
  int completion_bash = 0;
  int test = 0;
  const char * o = 0;
  double t = 0;
  const char * format = 0;
  const char * help = 0;
  int verbose = 0;
  int version = 0;
  const char * export_var = 0;

  /* handle options */
  /* Select a nick from *version*, *manufacturer*, *copyright*, *license*,
   * *url*, *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation*. Choose what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                     description */
    {"oihs", "version",       NULL,  _("1.0.0"),               NULL},
    {"oihs", "manufacturer",  NULL,  _("Kai-Uwe Behrmann"),    _("http://www.oyranos.org")},
    {"oihs", "copyright",     NULL,  _("Copyright © 2017-2022 Kai-Uwe Behrmann"),NULL},
    {"oihs", "license",       NULL,  _("newBSD"),              _("http://www.oyranos.org")},
    {"oihs", "url",           NULL,  _("http://www.oyranos.org"),NULL},
    {"oihs", "support",       NULL,  _("https://www.gitlab.com/oyranos/oyranos/issues"),NULL},
    {"oihs", "download",      NULL,  _("https://gitlab.com/oyranos/oyranos/-/releases"),NULL},
    {"oihs", "sources",       NULL,  _("https://gitlab.com/oyranos/oyranos"),NULL},
    {"oihs", "development",   NULL,  _("https://gitlab.com/oyranos/oyranos"),NULL},
    {"oihs", "oyjl_module_author",NULL,_("Kai-Uwe Behrmann"),  _("http://www.behrmann.name")},
    {"oihs", "documentation", NULL,  _("http://www.oyranos.org"),_("Tool to convert UI JSON description from *-X export* into source code.")},
    {"oihs", "date",          NULL,  _("2019-6-26T12:00:00"),  _("June 26, 2019")},
    {"",0,0,0,0}};

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s h_choices[] = {{"-",           _("Full Help"),     _("Print help for all groups"),""},
                                    {"synopsis",    _("Synopsis"),      _("List groups"),             _("Show all groups including syntax")},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s X_choices[] = {{"man",         _("Man"),           _("Unix Man page"),           _("Get a unix man page")},
                                    {"markdown",    _("Markdown"),      _("Formated text"),           _("Get formated text")},
                                    {"json",        _("Json"),          _("GUI"),                     _("Get a Oyjl Json UI declaration")},
                                    {"json+command",_("Json + Command"),_("GUI + Command"),           _("Get Oyjl Json UI declaration incuding command")},
                                    {"export",      _("Export"),        _("All available data"),      _("Get UI data for developers")},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s A_choices[] = {{_("Convert eXported developer JSON to C source"),_("oyjl-args -X export | oyjl-args -i -"),NULL,NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{_("oyjl(1) oyjl-translate(1) oyjl-args-qml(1)"),_("https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html"),NULL,NULL},
                                    {NULL,NULL,NULL,NULL}};


  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name, properties */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "i","input",         NULL,     _("input"),    _("Set Input"),               _("For C code output (default) and --completion-bash output use -X=export JSON. For --render=XXX use -X=json JSON."),_("FILENAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&input},   NULL},
    {"oiwi", 0,                          NULL,"c-stand-alone",NULL,     _("C Stand Alone"),_("Generate C code for oyjl_args.c inclusion."),_("Omit libOyjlCore reference."),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&c_stand_alone},NULL},
    {"oiwi", 0,                          NULL,"completion-bash",NULL,   _("Completion Bash"),_("Generate bash completion code"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&completion_bash},NULL},
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,NULL,"test",        NULL,     _("Test"),     _("Generate test Args Export"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&test},    NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE,"o",NULL,NULL,_("O"),NULL,                       NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&o},       NULL},
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,NULL,"option",      NULL,     _("Option"),   NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {0},           NULL},
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE,"t",NULL,           NULL,     _("T"),        NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_DOUBLE,   {.dbl = {.d = 0, .start = 0, .end = 0, .tick = 0}},oyjlDOUBLE,{.d=&t},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE,NULL,"format",NULL,_("Format"),NULL,             NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&format},  NULL},
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG,"h","help",       NULL,     _("help"),     _("Print help text"),         _("Show usage information and hints for the tool."),_("synopsis|..."),
        oyjlOPTIONTYPE_CHOICE,   {.choices = {h_choices, 0}},oyjlSTRING,{.s=&help},NULL},
    {"oiwi", 0,                          NULL,"synopsis",     NULL,     NULL,          NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {0},           NULL},
    {"oiwi", 0,                          "v","verbose",       NULL,     _("verbose"),  _("verbose"),                 NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&verbose}, NULL},
    {"oiwi", 0,                          "V","version",       NULL,     _("version"),  _("Version"),                 NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&version}, NULL},
    {"oiwi", 0,                          "X","export",        NULL,     _("export"),   _("Export formated text"),    _("Get UI converted into text formats"),_("json|json+command|man|markdown"),
        oyjlOPTIONTYPE_CHOICE,   {.choices = {X_choices, 0}},oyjlSTRING,{.s=&export_var},NULL},
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("EXAMPLES"), NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices = {A_choices, 0}},oyjlNONE,{0},NULL},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("SEE ALSO"), NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices = {S_choices, 0}},oyjlNONE,{0},NULL},
    /* default option template -X|--export */
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},oyjlNONE,{0},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail,        properties */
    {"oiwg", 0,     _("Convert"),       _("Generate source code"),    NULL,               "i",           "c-stand-alone,completion-bash,v","i,c-stand-alone,completion-bash",NULL},
    {"oiwg", 0,     _("Misc"),          _("General options"),         NULL,               "h,X,V",       "i,v",         "h,X,V,v",     NULL},
    {"",0,0,0,0,0,0,0,0}
  };

  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-args", _("Ui to source code"), _("Convert UI JSON to C code using libOyjl"),
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       "oyjl",
#endif
                                       sections, oarray, groups, &state );
  if( state & oyjlUI_STATE_EXPORT && !ui )
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyjl-args\n\n", _("For more information read the man page:") );
    goto clean_main;
  }

  if(ui && verbose)
  {
    int count = 0, i;
    char ** list = oyjlOptions_ResultsToList( ui->opts, NULL, &count );
    for(i = 0; i < count; ++i)
    {
      fputs( list[i], stderr );
      fputs( "\n", stderr );
    }
  }

  if(ui)
  {
    /* ... working code goes here ... */
  }
  else error = 1;

  clean_main:
  //oyjlUi_Release( &ui );

  return error;
}

