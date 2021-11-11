/*  @file openicc_device.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/10/20
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "openicc_conf.h"
#include "openicc_version.h"
#include "openicc_config.h"
#include "openicc_core.h"
#include "openicc_macros.h"
#include "openicc_config_internal.h"
#include "xdg_bds.h"

typedef enum {
	ucmm_ok = 0,
	ucmm_resource,				/* Resource failure (e.g. out of memory) */
	ucmm_invalid_profile,		/* Profile is not a valid display ICC profile */
	ucmm_no_profile,			/* There is no associated profile */
	ucmm_no_home,				/* There is no HOME environment variable defined */
	ucmm_no_edid_or_display,	/* There is no edid or display name */
	ucmm_profile_copy,			/* There was an error copying the profile */
	ucmm_open_config,			/* There was an error opening the config file */
	ucmm_access_config,			/* There was an error accessing the config information */
	ucmm_set_config,			/* There was an error setting the config file */
	ucmm_save_config,			/* There was an error saving the config file */
	ucmm_monitor_not_found,		/* The EDID or display wasn't matched */
	ucmm_delete_key,			/* Delete_key failed */
	ucmm_delete_profile,		/* Delete_key failed */
} ucmm_error;

/* Install scope */
typedef enum {
	ucmm_user,
	ucmm_local_system
} ucmm_scope;


int main(int argc, char ** argv)
{
  int count = 0;

  int help = 0;
  int verbose = 0;
  int version = 0;
  const char * export = NULL;
  int list_devices = 0,
      list_long = 0;
  int error = 0;
  int size = 0;
  char * text = NULL;
  const char * db_file = NULL,
             * file_name = NULL,
             * devices_filter[] = {NULL,NULL};
  int write_db_file = 0;
  int add_device = 0;
  int erase_device = 0;
  int list_pos = -1;
  int dump_json = 0;
  int show_path = 0;
  char *conf_name = NULL;		/* Configuration path to use */
  ucmm_scope scope = ucmm_user;		/* Scope of instalation */
  openiccConfig_s * config = NULL;
  int devices_n = 0, devices_new_n = 0;
  const char * device_classes_[] = {NULL, NULL};
  const char ** device_classes = 0;
  int device_classes_n = 0;

  openiccConfig_s * config_new = NULL;
  char * json_new = NULL;
  const char * d = NULL,
             * old_d = NULL,
             * device_class = NULL;
  int pos = -1;
  char * json;
  int i,j, n = 0,
      flags = OPENICC_CONFIGS_SKIP_HEADER | OPENICC_CONFIGS_SKIP_FOOTER;
  char * loc = NULL;

#ifdef OYJL_HAVE_LOCALE_H
  loc = setlocale(LC_ALL,"");
#endif
  openiccInit(loc);

  /* declare some option choices */
  oyjlOptionChoice_s b_choices[] = {{"DB-file-name.json", _("DB File"), _("File Name of device JSON Data Base"), ""},
                                       {"","","",""}};
  oyjlOptionChoice_s c_choices[] = {{"monitor", _("Monitor"), _("Monitor"), ""},
                                       {"printer", _("Printer"), _("Printer"), ""},
                                       {"camera", _("Camera"), _("Camera"), ""},
                                       {"scanner", _("Scanner"), _("Scanner"), ""},
                                       {"","","",""}};


  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,  option,          key,  name,             description,         help, value_name,    value_type,        values, variable_type, result */
    {"oiwi", 0,    "a", "add",           NULL, _("Add"),         _("Add Device to DB"),NULL,NULL,    oyjlOPTIONTYPE_NONE,     {}, oyjlINT,{.i=&add_device} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"b","db-file",NULL,_("DB File"),_("DB File Name"),_("File Name of OpenICC Device Data Base JSON"), _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING,{.s=&db_file} },
    {"oiwi", 0,    "c", "device-class",  NULL, _("Device Class"),_("Device Class"),   NULL, "monitor|printer|camera|scanner", oyjlOPTIONTYPE_CHOICE, {.choices.list = openiccMemDup( c_choices, sizeof(c_choices) )}, oyjlSTRING, {.s = &device_class} },
    {"oiwi", 0,    "d", "device",        NULL, _("Device"),      _("Device position"),NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE, {.dbl.tick=1,.dbl.start=0,.dbl.end=100}, oyjlINT,{.i=&list_pos} },
    {"oiwi", 0,    "e", "erase-device",  NULL, _("Erase Device"),_("Erase Devices"),  NULL, NULL,    oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=&erase_device} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"f","file-name",NULL,_("File Name"),   _("File Name"),      _("The File Name of the OpenICC Device in Json format."), _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING,{.s=&file_name} },
    {"oiwi", 0,    "j", "dump-json",     NULL, _("OpenICC Json"),_("Dump OpenICC JSON"),NULL,NULL,   oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=&dump_json} },
    {"oiwi", 0,    "h", "help",          NULL, _("Help"),        _("Help"),           NULL, NULL,    oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=&help} },
    {"oiwi", 0,   NULL, "synopsis",      NULL, NULL,             NULL,                NULL, NULL,    oyjlOPTIONTYPE_NONE,     {0},     oyjlNONE,  {0} },
    {"oiwi", 0,    "l", "list-devices",  NULL, _("List Devices"),_("List Devices"),   NULL, NULL,    oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=&list_devices} },
    {"oiwi", 0,    "n", "long",          NULL, _("Long Format"), _("List all key/values pairs"),  NULL, NULL, oyjlOPTIONTYPE_NONE, {},oyjlINT, {.i=&list_long} },
    {"oiwi", 0,    "p", "show-path",     NULL, _("Show Path"),   _("Show Path"),      NULL, NULL,    oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=&show_path} },
    {"oiwi", 0,    "s", "system",        NULL, _("System"),      _("Local System"),   NULL, NULL,    oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=(int*)&scope} },
    {"oiwi", 0,    "v", "verbose",       NULL, _("Verbose"),     _("verbose"),        NULL, NULL,    oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=&verbose} },
    {"oiwi", 0,    "V", "version",       NULL, _("version"),     _("Version"),        NULL, NULL,    oyjlOPTIONTYPE_NONE,     {0},     oyjlINT,   {.i=&version} },
    /* default option template -X|--export */
    {"oiwi", 0,    "X", "export",        NULL, NULL,             NULL,                NULL, NULL,    oyjlOPTIONTYPE_CHOICE,   {},      oyjlSTRING,{.s=&export} },
    {"oiwi", 0,    "w", "write",         NULL, _("Write"),       _("Write DB File"),  NULL, NULL,    oyjlOPTIONTYPE_NONE,     {},      oyjlINT,   {.i=&write_db_file} },
    {"",0,0,0,0,0,0,0, NULL, 0,{},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,              description,                          help, mandatory, optional, detail */
    {"oiwg", 0,     _("List Devices"), _("Print the Devices in the DB"),     NULL, "l",       "d,j,n,b,v","l,d,j,n,b" },
    {"oiwg", 0,     _("Add Device"),   _("Add Device to DB"),                NULL, "a,f",     "b,v",    "a,f,b" },
    {"oiwg", 0,     _("Erase Device"), _("Erase a Devices from the DB"),     NULL, "e,d",     "b,v",    "e,d,b" },
    {"oiwg", 0,     _("Show DB Path"), _("Show Filepath to the DB"),         NULL, "p",       "s,v",    "p,s" },
    {"oiwg", 0,     _("Misc"),         _("General options"),                 NULL, "h,X",     "v",       "b,X,v,h" },
    {"",0,0,0,0,0,0,0}
  };


  oyjlUiHeaderSection_s * info = oiUiInfo(_("Manipulation of OpenICC color management data base device entries."));
  oyjlUi_s * ui = oyjlUi_Create( argc, (const char**) argv,
      "openicc-device", "OpenICC Device", _("OpenICC devices handling tool"), "openicc-logo",
      info, oarray, groups, NULL );
  if(!ui) return 0;

  if((export && strcmp(export,"json+command") == 0))
  {
    const char * jcommands = "{\n\
  \"command_set\": \"openicc-device\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\",\n\
  \"command_get\": \"openicc-device\",\n\
  \"command_get_args\": [\"-X\", \"json\"]\n\
}";
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = strdup(jcommands);
    json_commands[strlen(json_commands)-2] = ',';
    json_commands[strlen(json_commands)-1] = '\000';
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] );
    puts( json_commands );
    exit(0);
  }

  if(!db_file)
  {
    char *config_file = OPENICC_DB_PREFIX "/" OPENICC_DB;
    /* Locate the directories where the config file is, */
    /* and where we should copy the profile to. */
    {
      int npaths;
      xdg_error er;
      char **paths;

      if ((npaths = xdg_bds(&er, &paths, xdg_conf, xdg_write, 
                            (scope == ucmm_user) ? xdg_user : xdg_local,
                            config_file)) == 0)
      {
        return ucmm_open_config;
      }
      if ((conf_name = strdup(paths[0])) == NULL)
      {
        xdg_free(paths, npaths);
        return ucmm_resource;
      }
      if(show_path)
      {
        if(verbose)
          fprintf(stderr, "%s\n", _("Paths:"));
        for(i=0; i < npaths; ++i)
          printf("%s\n", paths[i]);
      }

      xdg_free(paths, npaths);
    }

    db_file = conf_name;
  }

  if(!db_file)
  {
    DBG( 0, "%s at \"%s\"", _("unable to open data base"), db_file);
    exit(0);
  }
 
  {
    /* read JSON input file */
    text = openiccReadFile( db_file, &size );

    /* parse JSON */
    config = openiccConfig_FromMem( text );
    if(text) { free(text); text = NULL; }
    openiccConfig_SetInfo ( config, db_file );

    if(device_class)
    {
      device_classes_[0] = device_class;
      device_classes = device_classes_;
      device_classes_n = 1;
    } else
      device_classes = openiccConfigGetDeviceClasses( device_classes,
                                                  &device_classes_n );

    devices_n = openiccConfig_DevicesCount(config, device_classes);
    DBG( config, "Found %d devices.", devices_n );
  }

  if(add_device)
  {
    if(file_name)
      text = openiccReadFile( file_name, &size );
    else
      error = openiccReadFileSToMem( stdin, &text, &size );

    /* parse JSON */
    config_new = openiccConfig_FromMem( text );
    if(text) { free(text); text = NULL; }
    openiccConfig_SetInfo ( config_new, file_name );
    devices_new_n = openiccConfig_DevicesCount(config_new, NULL);
    DBG( config_new, "Found %d devices.", devices_new_n );
    if(devices_new_n)
      list_devices = 1;
  }

  if(erase_device && devices_n)
  {
    list_devices = 1;
  }

  if(dump_json && devices_n)
  {
    list_devices = 1;
  }


  if(list_devices)
  {
    char            ** keys = 0;
    char            ** values = 0;

    n = 0;
    d = 0;
    old_d = 0;

    if(dump_json)
    {
      pos = -1;

      n = openiccConfig_DevicesCount(config, NULL);

      for(j = 0; j < device_classes_n; ++j)
      {
        devices_filter[0] = device_classes[j];
        devices_n = openiccConfig_DevicesCount(config, devices_filter);

        for(i = 0; i < devices_n; ++i)
        {
          ++pos;

          if(list_pos != -1 && ((!erase_device && pos != list_pos) ||
                                (erase_device && pos == list_pos)))
            continue;

          d = openiccConfig_DeviceGetJSON( config, devices_filter, i,
                                           flags, old_d, &json, malloc,free );

          if(d)
          {
            if(!old_d)
              oyjlStringAdd( &json_new, 0,0, OPENICC_DEVICE_JSON_HEADER, d );
            STRING_ADD( json_new, json );
            old_d = d;
          }
          if(json) { free(json); json = NULL; }
        }

        count = openiccConfig_DevicesCount(config_new, devices_filter);
        for(i = 0; i < count; ++i)
        {
          ++pos;

          d = openiccConfig_DeviceGetJSON( config_new, devices_filter, i,
                                           flags, old_d, &json, malloc,free );

          if(d)
          {
            if(!old_d)
              oyjlStringAdd( &json_new, 0,0, OPENICC_DEVICE_JSON_HEADER, d );
            STRING_ADD( json_new, json );
            old_d = d;
          }

          if(json) { free(json); json = NULL; }
          if(json_new)
            list_devices = 1;
        }
      }
 
      if(json_new)
      {
        STRING_ADD( json_new, "\n" OPENICC_DEVICE_JSON_FOOTER );
        printf( "%s", json_new );
      }

      if(write_db_file)
      {
        if(device_class)
        {
          WARNc_S("Can not write file %s  with single device class \"%s\"",
                    db_file, device_class );
          exit(1);
        }

        if(json_new)
        {
          int len = strlen(json_new) + 1;
          int s = openiccWriteFile( db_file, json_new, len );
          if(s != len)
            WARNc_S("Could not write file %s  %u!=%u", db_file, s, len );
        } else
          remove(db_file);
      }

    } else
    {
      /* print all found key/value pairs */
      for(i = 0; i < devices_n; ++i)
      {
        char * manufacturer = 0,
             * model = 0,
             * prefix = 0;
        const char * check_key;
        if(list_pos != -1 && ((!erase_device && i != list_pos) ||
                              (erase_device && pos == list_pos)))
          continue;

        d = openiccConfig_DeviceGet( config, device_classes, i,
                                     &keys, &values, malloc,free );

        if(i && list_long)
          fprintf( stderr,"\n");

        n = 0; if(keys) while(keys[n]) ++n;
        if(verbose)
        fprintf( stderr, "[%d] device class:\"%s\" with %d keys/values pairs\n",
                 i, d, n );
        for( j = 0; j < n; ++j )
        {
          check_key = keys[j];
          if(strcmp(check_key, "prefix") == 0)
            prefix = values[j];
        }

        for( j = 0; j < n; ++j )
        {
          if(!list_long)
          {
            check_key = keys[j];
            if(prefix && strlen(prefix) < strlen(check_key) &&
               memcmp(prefix, check_key, strlen(prefix)) == 0)
              check_key += strlen(prefix);
            if(strcmp(check_key, "manufacturer") == 0)
              manufacturer = values[j];
            else if(strcmp(check_key, "model") == 0)
              model = values[j];
            else if(strcmp(check_key, "prefix") == 0)
              prefix = values[j];
            else
              free(values[j]);
          }
          else
          {
            fprintf(stdout, "%s:\"%s\"\n", keys[j], values[j]);
            free(values[j]);
          }
          free(keys[j]);
        }
        free(keys); free(values);
        if(!list_long)
          fprintf(stdout, "%d : \"%s\" - \"%s\"\n", i, manufacturer, model);
      }
    }
  }

  openiccConfig_Release( &config );

  if(conf_name)
    free(conf_name);

  return error;
}

