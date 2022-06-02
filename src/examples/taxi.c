/* gcc -Wall -g taxi.c -o taxi `pkg-config oyranos --libs --cflags` */

#include <stdio.h>

#include <sys/stat.h> /* mkdir() */

#include <oyConfigs_s.h>
#include <oyProfile_s.h>

#include "oyranos_config_internal.h"
#include <oyranos_db.h>
#include <oyranos_devices.h>
#include "oyranos_helper_macros_cli.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"

#define TAXI_URL "https://icc.opensuse.org"

#ifndef oyNoEmptyString_m_
#define oyNoEmptyString_m_(t) (t?t:"----")
#endif
#ifndef _
#define _(t) (t)
#endif
#define oyAllocateFunc_ malloc
#define oyDeAllocateFunc_ free

void  printfHelp (int argc OY_UNUSED, char** argv)
{
  const char * version = oyVersionString(1),
             * id = oyVersionString(2),
             * cfg_date =  oyVersionString(3),
             * devel_time = oyVersionString(4);

  fprintf( stderr, "\n");
  fprintf( stderr, "oyranos-taxi %s\n",
                                _("is a Taxi DB tool"));
  fprintf( stderr, "  Oyranos v%s config: %s devel period: %s\n",
                  oyNoEmptyString_m_(version),
                  oyNoEmptyString_m_(cfg_date), oyNoEmptyString_m_(devel_time) );
  if(id)
  fprintf( stderr, "  Oyranos git id %s\n", oyNoEmptyString_m_(id) );
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",
                                           _("Hint: search paths are influenced by the XDG_CONFIG_HOME shell variable."));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("List available device manufacturers:"));
  fprintf( stderr, "      %s -l\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("List search URL:"));
  fprintf( stderr, "      %s -p\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("List ICC Profiles:"));
  fprintf( stderr, "      %s -m MANUFACTURER_NAME\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Get ICC Profile:"));
  fprintf( stderr, "      %s --taxi TAXI_ID [--profile SAVE_FILENAME] -m MANUFACTURER_NAME\n", argv[0]);
  fprintf( stderr, "      %s --db-download -m MANUFACTURER_NAME\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      %s\n",           _("-v verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s:\n",               _("Example"));
  fprintf( stderr, "      SAVEIFS=$IFS ; IFS=$'\\n\\b'; manufacturers=(`oyranos-taxi -l`); IFS=$SAVEIFS; for mnft in \"${manufacturers[@]}\"; do ls \"$mnft\"; done");
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}


int oyLowerStrcmpWrap (const void * a_, const void * b_)
{
  const char * a = *(const char **)a_,
             * b = *(const char **)b_;
#ifdef HAVE_POSIX
  return strcasecmp(a,b);
#else
  return strcmp(a,b);
#endif
}

int oyStrCmp(const char * a, const char * b) { return (a && b) ? strcmp(a,b) : 1; }

#define OPENICC_DEVICE_JSON_HEADER_BASE \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"device\": {\n" \
  "          \"%s\":\n"
#define OPENICC_DEVICE_JSON_FOOTER_BASE \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"


void storeTaxiProfile( const char * taxi_id, const char * taxi_full_id, const char * profile_name )
{
  oyOptions_s * options = 0;
  oyProfile_s * profile;

  oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/argv/TAXI_id",
                                   taxi_full_id,
                                   OY_CREATE_NEW );

  profile = oyProfile_FromTaxiDB( options, NULL );
  oyOptions_Release( &options );
  if(profile)
    {
      {
        size_t size = 0;
        char * mem = oyProfile_GetMem( profile, &size, 9, oyAllocateFunc_ );
        char * fn = 0;

        oyStringAddPrintf_( &fn, oyAllocateFunc_, oyDeAllocateFunc_,
                            "%s.icc", profile_name ? profile_name : taxi_id );

        oyWriteMemToFile_(fn, mem, size);
        printf( "wrote  \"%s\" %u to \"%s\"",
                oyProfile_GetText(profile, oyNAME_DESCRIPTION), (unsigned int)size,
                fn );
      }
      oyProfile_Release( &profile );
    } else
      printf("obtained no profile\n");

  oyOptions_Release( &options );
}

int main( int argc, char ** argv )
{
  int error = 0;
  char * profile_name = 0;          /* the file to write to */
  int i,
      verbose = 0,
      list_manufacturers = 0,
      list_urls = 0;
  char * taxi_id = 0;
  char * mnft = 0;
  int db_download = 0;

  size_t size = 0;
  char * short_name = NULL,
       * long_name = NULL,
       * count_name = NULL;
  oyjl_val root = 0;
  char * val = NULL;
  oyjl_val v = 0, tv = 0;
  int count;


#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

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
              case 'l': list_manufacturers = 1; break;
              case 'm': OY_PARSE_STRING_ARG(mnft); break;
              case 'p': list_urls = 1; break;
              case 'v': if(verbose == 0) oy_debug += 1; ++verbose; break;
              case 'h':
              case '-':
                        if(OY_IS_ARG("taxi"))
                        { OY_PARSE_STRING_ARG2(taxi_id, "taxi"); break; }
                        else if(OY_IS_ARG("profile"))
                        { OY_PARSE_STRING_ARG2(profile_name, "profile"); break; }
                        else if(OY_IS_ARG("db-download")) /* needs mnft */
                        { db_download = 1; i = 100; break; }
                        OY_FALLTHROUGH
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
       fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
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

  

  if(list_manufacturers)
  {
    char * manufacturers = oyReadUrlToMem_( TAXI_URL "/manufacturers",
                                            &size, "r", oyAllocateFunc_ );
    if(manufacturers && manufacturers[0])
    {
      root = oyJsonParse( manufacturers, NULL );
      if(!root)
      {
        WARNc2_S( "%s:\n%s", _("found issues parsing JSON"), manufacturers );
      }
    } else
    {
      fprintf(stderr, "%s\n", _("Could not download from WWW."));
      return 1;
    }
    if(root)
    {
      int count = oyjlValueCount(root);
      const char ** sort = calloc( sizeof(char**), 3*count + 3 );

      for(i = 0; i < count; ++i)
      {
        v = oyjlValuePosGet    ( root, i );
        v = oyjlTreeGetValueF( root, 0, 
                              "[%d]/short_name", i );
        short_name = oyjlValueText( v, oyAllocateFunc_ );
        v = oyjlTreeGetValueF( root, 0, 
                              "[%d]/long_name", i );
        long_name = oyjlValueText( v, oyAllocateFunc_ );
        v = oyjlTreeGetValueF( root, 0, 
                              "[%d]/count", i );
        count_name = oyjlValueText( v, oyAllocateFunc_ );
        sort[i*3+0] = short_name;
        sort[i*3+1] = long_name;
        sort[i*3+2] = count_name;
      }
      qsort( sort, count, sizeof(char**)*3, oyLowerStrcmpWrap );

      for(i = 0; i < count; ++i)
      {
        if(!i || (i && oyStrCmp( sort[3*i+0], sort[3*(i-1)+0] ) != 0) ||
           verbose)
        {
          if(verbose)
            printf("[%d] ",i);
          printf("%s", oyNoEmptyString_m_(sort[3*i+0]));
          if(verbose)
            printf(" %s", oyNoEmptyString_m_(sort[3*i+1]));
          if(verbose >= 2)
            printf(" (%s)", oyNoEmptyString_m_(sort[3*i+2]));
          printf("\n");
        }
      }
      oyjlTreeFree( root ); root = 0;
    }
  }

  if(list_urls)
    printf("%s\n",TAXI_URL);

  if(mnft)
  {
        /* the device DB JSON contains all device meta data for one
         * mnft / manufacturer */
    char * device_db = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                            TAXI_URL "/devices/%s", mnft ),
         * t = 0;
    char * fn = 0;
    const char * device_class = "dummy";

    if(!device_db)
    {
      fprintf(stderr, "%s\n", _("Could not download from WWW."));
      return 1;
    }

    if(strstr(device_db, "EDID_") != NULL)
      device_class = "monitor";

    if(oy_debug)
      fprintf( stderr, "%s\n", device_db );

        /* put a cloak around the bare meta data, so it behaves like OpenICC
         * JSON */
        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            OPENICC_DEVICE_JSON_HEADER_BASE, device_class );

        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            device_db );

        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            "\n"OPENICC_DEVICE_JSON_FOOTER_BASE );
        oyDeAllocateFunc_(device_db);


        if(db_download)
        {
                oyStringAddPrintf_( &fn, oyAllocateFunc_, oyDeAllocateFunc_,
                            "%s/%s.json", mnft, mnft );

                oyWriteMemToFile_(fn, t, strlen(t+1) );
                if(fn) { oyDeAllocateFunc_(fn); fn = 0; }
        }
        device_db = t; t = NULL;

        root = oyJsonParse( device_db, NULL );


        tv = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]" );
        count = oyjlValueCount(tv);
        for(i = 0; i < count; ++i)
        {
          char * id = 0,
               * id_full = 0;

          v = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]/[%d]/_id/$oid", i );
          val = oyjlValueText( v, oyAllocateFunc_ );
          oyStringAddPrintf_( &id_full, oyAllocateFunc_, oyDeAllocateFunc_,
                              "%s/0", val );
          oyStringAddPrintf_( &id, oyAllocateFunc_, oyDeAllocateFunc_,
                              "%s", val );

          if(val)
          {
            int j,n;

            if(!taxi_id)
              printf("%s/0 ",val);

            if(val) { oyDeAllocateFunc_(val); val = 0; }

            v = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]/[%d]/profile_description", i );
            n = oyjlValueCount(v);
            for(j = 0; j < n; ++j)
            {
              v = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]/[%d]/profile_description/[%d]", i, j );
              val = oyjlValueText( v, oyAllocateFunc_ );
              if(verbose && !taxi_id)
                printf("%s",val);
              if(val) { oyDeAllocateFunc_(val); val = 0; }
              /* store all profile descriptions */
              if((taxi_id && strcmp(taxi_id, id_full) == 0) ||
                 db_download)
              {
                oyStringAddPrintf_( &fn, oyAllocateFunc_, oyDeAllocateFunc_,
                            "%s/%s.json", mnft, profile_name ? profile_name : id );

                v = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]/[%d]", i );
                val = oyjlValueText( v, oyAllocateFunc_ );

                oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            OPENICC_DEVICE_JSON_HEADER_BASE, "dummy" );

                oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            "%s", val );

                oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            "\n"OPENICC_DEVICE_JSON_FOOTER_BASE );

                mkdir(mnft,
#if !defined(_WIN32)
                      S_IRWXU | S_IRWXG | S_IRWXO
#else
                      0
#endif
                     );
                if(val)
                {
                  oyWriteMemToFile_(fn, t, strlen(t+1) );
                  printf( "wrote id %s to \"%s\"\n", id, fn );
                }
                if(fn) { oyDeAllocateFunc_(fn); fn = 0; }

                oyStringAddPrintf_( &fn, oyAllocateFunc_, oyDeAllocateFunc_,
                            "%s/%s", mnft, profile_name ? profile_name : id );
                storeTaxiProfile( fn, id_full, NULL );

                if(val) { oyDeAllocateFunc_(val); val = 0; }
                if(t) { oyDeAllocateFunc_(t); t = 0; }
                if(fn) { oyDeAllocateFunc_(fn); fn = 0; }
              }
              //break;
            }
            if(!taxi_id)
              printf("\n");
          }

          if(val) { oyDeAllocateFunc_(val); val = 0; }
          if(id) { oyDeAllocateFunc_(id); id = 0; }
          if(id_full) { oyDeAllocateFunc_(id_full); id_full = 0; }
        }

        if(!count)
          fprintf(stderr, "%s\n", _("Did you specify a 3 letter manufacturer code?"));

        if(device_db) { oyDeAllocateFunc_(device_db); device_db = NULL; }
  }

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return error;
}
