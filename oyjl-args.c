/** @file oyjl-args.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2019  Kai-Uwe Behrmann
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
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"


int main( int argc, const char ** argv )
{

  const char * file = NULL;
  const char * export = NULL;
  int help = 0;
  int verbose = 0;
  int state = 0;

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_modules_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("Tool to convert UI JSON description from *-X export* into source code.")},
    {"oihs", "date",          NULL,  "2019-6-26T12:00:00", _("June 26, 2019")},
    {"",0,0,0,0}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose} },
    {"oiwi", 0,     "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Convert"),_("Generate source code"),NULL, "i", "v",      "i" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h",       "v",      "h,v" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-args", _("Ui to source code"), _("Convert UI JSON to C code using libOyjl"), "logo",
                                       sections, oarray, groups, &state );
  /* done with options handling */

  if(ui && verbose)
    oyjlOptions_PrintHelp( ui->opts, ui, 4, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );

  if(ui && !file)
  {
    const char * r = oyjlOptions_ResultsToJson(ui->opts);
    fprintf(stdout, "%s", r?r:"----");
  }

  if(file)
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
    
    char error_buffer[128] = {0};
    oyjl_val json = oyjlTreeParse( text, error_buffer, 128 );
    char * sources = oyjlUiJsonToCode( json, OYJL_SOURCE_CODE_C );
    fprintf( stderr, "wrote %d to stdout\n", sources&&strlen(sources)?(int)strlen(sources):0 );
    puts( sources );
  }

  oyjlUi_Release( &ui);

  fflush(stdout);

  return 0;
}

