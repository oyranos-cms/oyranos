/*  @file openicc_config_read.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

/**
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>           /* setlocale LC_NUMERIC */

#include "openicc_config.h"
#include "openicc_config_internal.h"

int main(int argc, char ** argv)
{
  openiccConfig_s * config, * db;
  const char * file_name = argc > 1 ? argv[1] : "../test.json";
  char * text = 0;
  int size = 0;
  char            ** keys = 0;
  char            ** values = 0;
  int i,j, n = 0, devices_n, flags;
  char * json, * device_class;
  const char * devices_filter[] = {OPENICC_DEVICE_CAMERA,NULL},
             * old_device_class = NULL,
             * d = NULL;
  int output = 0;
  const char * file = NULL;
  int help = 0;
  int verbose = 0;
  const char * export = NULL;

  const char * loc = setlocale(LC_ALL,"");
  openiccInit( loc );

  /* handle options */
  /* declare some option choices */
  oyjlOptionChoice_s i_choices[] = {{"openicc.json", _("openicc.json"), _("openicc.json"), ""},
                                    {"","","",""}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("Print All"), ""},
                                    {"1", _("Print Camera"), _("Print Camera JSON"), ""},
                                    {"2", _("Print None"), _("Print None"), ""},
                                    {"","","",""}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = openiccMemDup( i_choices, sizeof(i_choices) )}, oyjlSTRING, {.s = &file}, NULL },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = openiccMemDup( o_choices, sizeof(o_choices) )}, oyjlINT, {.i = &output}, NULL },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE,   {}, oyjlINT, {.i = &help}, NULL },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE,   {}, oyjlINT, {.i = &verbose}, NULL },
    /* default option template -X|--export */
    {"oiwi", 0,     "X", "export",  NULL, NULL,         NULL,                NULL, NULL,          oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING,{.s=&export}, NULL },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{},NULL}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode"), _("Actual mode"),     NULL, "i",       "o,v",    "i,o", NULL },
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "X,v,h", NULL },
    {"",0,0,0,0,0,0,0,NULL}
  };

  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *openicc_modules_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,      , description  */
    {"oihs", "version",       NULL,  "1.0",       NULL},
    {"oihs", "documentation", NULL,  "",          _("The example tool demonstrates the usage of the libOpenIcc config and options API's.")},
    {"",0,0,0,0}};

  oyjlUi_s * ui = oyjlUi_Create( argc, (const char**) argv,
      "openicc-config-read", "OpenICC Config Read", _("Short example tool using libOpenIcc"), "openicc-logo",
      sections, oarray, groups, NULL );
  if(!ui) return 0;
  /* done with options handling */

  /* read JSON input file */
  if(file) file_name = file;
  text = openiccReadFile( file_name, &size );
  if(!text)
  {
    FILE * f = NULL;
    char * t = oyjlOptions_PrintHelp( ui->opts, ui, verbose, &f, "%s example tool", argv[0] );
    fprintf( f, "Obtained no text from file_name: %s\n", file_name );
    fputs( t, f );
    free(t);
    return 0;
  }
 

  /* parse JSON */
  db = openiccConfig_FromMem( text );
  openiccConfig_SetInfo ( db, file_name );
  devices_n = openiccConfig_DevicesCount(db, NULL);
  fprintf(stderr, "Found %d devices.\n", devices_n );

  
  /* print all found key/value pairs */
  if(output == 0)
  for(i = 0; i < devices_n; ++i)
  {
    const char * d = openiccConfig_DeviceGet( db, NULL, i,
                                              &keys, &values, malloc,free );

    if(i)
      fprintf( stderr,"\n");

    n = 0; if(keys) while(keys[n]) ++n;
    fprintf( stderr, "[%d] device class:\"%s\" with %d keys/values pairs\n", i, d, n);
    for( j = 0; j < n; ++j )
    {
      fprintf(stderr, "%s:\"%s\"\n", keys[j], values[j]);
      free(keys[j]);
      free(values[j]);
    }
    free(keys); free(values); keys = NULL; values = NULL;
  }

  /* get a single JSON device */
  i = 1; /* select the second one, we start counting from zero */
  d = openiccConfig_DeviceGetJSON ( db, NULL, i, 0,
                                    old_device_class, &json, malloc,free );
  config = openiccConfig_FromMem( json );
  device_class = openiccConfig_DeviceClassGet( config, malloc );
  openiccConfig_Release( &config );
  fprintf( stderr, "\ndevice class[%d]: \"%s\"\n", i, device_class);
  if(output == 0)
    printf( "%s\n", json );
  free(json);


  /* we want a single device class DB for lets say cameras */
  devices_n = openiccConfig_DevicesCount( db, devices_filter );
  fprintf(stderr, "Found %d %s devices.\n", devices_n, devices_filter[0] );
  old_device_class = NULL;
  for(i = 0; i < devices_n; ++i)
  {
    flags = 0;
    if(i != 0) /* not the first */
      flags |= OPENICC_CONFIGS_SKIP_HEADER;
    if(i != devices_n - 1) /* not the last */
      flags |= OPENICC_CONFIGS_SKIP_FOOTER;

    d = openiccConfig_DeviceGetJSON( db, devices_filter, i, flags,
                                     old_device_class, &json, malloc,free );
    old_device_class = d;
    if(output <= 1)
      printf( "%s\n", json );
    free(json);
  }

  openiccConfig_Release( &db );

  return 0;
}

