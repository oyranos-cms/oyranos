/*  @file openicc_device.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2017 (C) Kai-Uwe Behrmann
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


void printfHelp(int argc OI_UNUSED, char ** argv)
{
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a color management data base tool"));
  fprintf( stderr, "  v%s\n",
                  OPENICC_VERSION_NAME );
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("List devices:"));
  fprintf( stderr, "      %s -l [-v] [-db-file FILE_NAME] [--long]\n",        argv[0]);
  fprintf( stderr, "                        [-d NUMBER|--device NUMBER] [-j]\n");
  fprintf( stderr, "        --db-file FILE_NAME  %s\n", _("specify DB file"));
  fprintf( stderr, "        --long          %s\n", _("listing all key/values pairs"));
  fprintf( stderr, "        -d NUMBER | --device NUMBER\n"
                   "                        %s\n", _("select device by position"));
  fprintf( stderr, "        -j | --json     %s\n", _("dump raw JSON"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Add device:"));
  fprintf( stderr, "      %s -a [-f FILE_NAME] [-v] [-db-file FILE_NAME] \n",argv[0]);
  fprintf( stderr, "        -w              %s\n", _("write to selected DB file"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Erase device:"));
  fprintf( stderr, "      %s -e [-d NUMBER|--device NUMBER] [-v] [-db-file FILE_NAME] \n",argv[0]);
  fprintf( stderr, "                        %s\n", _("implies --json option"));
  fprintf( stderr, "        -w              %s\n", _("write to selected DB file"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Show DB path:"));
  fprintf( stderr, "        --show-path [-s]   %s\n", _("locate DB"));
  fprintf( stderr, "        -s              %s\n", _("list system DB (default is user DB)"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "        -v              %s\n", _("verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}


int main(int argc, char ** argv)
{
  int count = 0;

  int verbose = 0;
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

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  openiccInit();

  if(argc >= 2)
  {
    int pos = 1;
    unsigned i;
    char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'a': add_device = 1; dump_json = 1; break;
              case 'c': OY_PARSE_STRING_ARG(device_class); break;
              case 'd': OY_PARSE_INT_ARG(list_pos); break;
              case 'e': erase_device = 1; dump_json = 1; break;
              case 'f': OY_PARSE_STRING_ARG(file_name); break;
              case 'j': dump_json = 1; break;
              case 'l': list_devices = 1; break;
              case 's': scope = ucmm_local_system; break;
              case 'v': ++verbose; ++*openicc_debug; break;
              case 'w': write_db_file = 1; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("verbose"))
                        { ++*openicc_debug; ++verbose; i=100; break; }
                        else if(OY_IS_ARG("json"))
                        { dump_json = 1; i=100; break; }
                        else if(OY_IS_ARG("long"))
                        { list_long = 1; i=100; break; }
                        else if(OY_IS_ARG("device"))
                        { OY_PARSE_INT_ARG2( list_pos, "device" ); break; }
                        else if(OY_IS_ARG("show-path"))
                        { show_path = 1; i=100; break; }
                        else if(OY_IS_ARG("db-file"))
                        { OY_PARSE_STRING_ARG2( db_file, "db-file"); break; }
                        } OI_FALLTHROUGH
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
      }
      if( wrong_arg )
      {
       WARN( 0, "%s %s", "wrong argument to option:", wrong_arg);
       printfHelp(argc, argv);
       exit(1);
      }
      ++pos;
    }
  } else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  if(erase_device && list_pos == -1)
  {
                        printfHelp(argc, argv);
                        exit (0);
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
                            (scope == ucmm_local_system) ? xdg_local : xdg_user,
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
  } else
    fprintf( stderr, "using DB: %s\n", db_file);
 
  {
    /* read JSON input file */
    text = openiccOpenFile( db_file, &size );

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
      text = openiccOpenFile( file_name, &size );
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

