/** @file oyranos_profile.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC profile informations - on the command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2011/06/24
 *
 *  The program informs about a ICC profile.
 */


#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_config.h"
#include "oyranos_string.h"
#include "oyranos_version.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void* oyAllocFunc(size_t size) {return malloc (size);}

#define OPENICC_DEVICE_JSON_HEADER \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"device\": {\n" \
  "          \"%s\": {\n" \
  "            \"%d\": {\n"
#define OPENICC_DEVICE_JSON_FOOTER \
  "            }\n" \
  "          }\n" \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"
#define OPENICC_DEVICE_MONITOR "monitor"
#define OPENICC_DEVICE_SCANNER "scanner"
#define OPENICC_DEVICE_PRINTER "printer"
#define OPENICC_DEVICE_CAMERA  "camera"


void  printfHelp (int argc, char** argv)
{
  char * version = oyVersionString(1,0);
  char * id = oyVersionString(2,0);
  char * cfg_date =  oyVersionString(3,0);
  char * devel_time = oyVersionString(4,0);

  fprintf( stderr, "\n");
  fprintf( stderr, "oyranos-profile %s\n",
                                _("is a ICC profile information tool"));
  fprintf( stderr, "  Oyranos v%s config: %s devel period: %s\n",
                  oyNoEmptyName_m_(version),
                  oyNoEmptyName_m_(cfg_date), oyNoEmptyName_m_(devel_time) );
  if(id)
  fprintf( stderr, "  Oyranos git id %s\n", oyNoEmptyName_m_(id) );
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",
                                           _("Hint: search paths are influenced by the XDG_CONFIG_HOME shell variable."));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("List included ICC tags:"));
  fprintf( stderr, "      %s -l [-d] FILE_NAME\n",        argv[0]);
  fprintf( stderr, "      -p NUMBER  %s\n",       _("select tag"));
  fprintf( stderr, "      -n NAME  %s\n",       _("select tag"));
  fprintf( stderr, "      -s NAME  %s\n",       _("add prefix"));
  fprintf( stderr, "      -c NAME  %s\n",       _("use device class"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Dump Device Infos to JSON:"));
  fprintf( stderr, "      %s -o FILE_NAME\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      %s\n",           _("-v verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s:\n",               _("Example"));
  fprintf( stderr, "      oyranos-profile -lv -p=1 sRGB.icc");
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");

  if(version) oyDeAllocateFunc_(version);
  if(id) oyDeAllocateFunc_(id);
  if(cfg_date) oyDeAllocateFunc_(cfg_date);
  if(devel_time) oyDeAllocateFunc_(devel_time);
}


int main( int argc , char** argv )
{
  int error = 0;
  int list_tags = 0,
      tag_pos = -1,
      dump_openicc_json = 0;
  const char * file_name = 0,
             * tag_name = 0,
             * name_space = 0;
  const char * prefixes[24] = {0}; int pn = 0;
  const char * device_class = "unknown";

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

  if(argc >= 2)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    DBG_PROG1_S("argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'c': OY_PARSE_STRING_ARG(device_class); break;
              case 'l': list_tags = 1; break;
              case 'n': OY_PARSE_STRING_ARG(tag_name); break;
              case 'o': dump_openicc_json = 1; break;
              case 'p': OY_PARSE_INT_ARG( tag_pos ); break;
              case 's': OY_PARSE_STRING_ARG(name_space);
                        if(name_space)
                        {
                          int n = pn - 1, found = 0;
                          while(n >= 0)
                            if(strcmp( prefixes[n--], name_space ) == 0)
                              found = 1;
                          if( !found )
                            prefixes[pn++] = name_space;
                        }
                        break;
              case 'v': oy_debug += 1; break;
              case 'h':
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        file_name = argv[pos];
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


  if(file_name)
  {
    oyProfile_s * p = oyProfile_FromFile( file_name, 0, 0 );
    oyProfileTag_s * tag;
    char ** texts = NULL;
    int32_t texts_n = 0, i,j, count;

    if(list_tags)
    {
      fprintf(stderr, "%s \"%s\" %d:\n", _("ICC profile"), file_name,
              (int)p->size_);
      count = oyProfile_GetTagCount( p );
      for(i = 0; i < count; ++i)
      {
        tag = oyProfile_GetTagByPos( p, i );

        if(tag &&
           ((tag_name == NULL && (tag_pos == -1 || tag_pos == i)) ||
            (tag_name != NULL && (strcmp(oyICCTagName(tag->use),tag_name)==0)))
          )
        {
          int32_t tag_size = 0;
          texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                        &tag_size, malloc );

          fprintf( stdout, "%s/%s[%d] %d @ %d %s",
                   oyICCTagName(tag->use), oyICCTagTypeName(tag->tag_type_), i,
                   (int)tag->size_, (int)tag->offset_orig,
                   oyICCTagDescription(tag->use));
          if(oy_debug && texts)
          {
            fprintf( stdout, ":\n" );
              for(j = 0; j < texts_n; ++j)
                fprintf( stdout, "%s\n", texts[j] );
          } else
          {
            fprintf( stdout, "\n" );
          }
        }

        oyProfileTag_Release( &tag );
      }
    } else
    if( dump_openicc_json )
    {
      tag = oyProfile_GetTagById( p, icSigMetaDataTag );
      if(tag)
      {
        int32_t tag_size = 0;
        int size = 0;

        texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                      &tag_size, malloc );

        if(texts_n > 2)
          size = atoi(texts[0]);

        /* collect key prefixes and detect device class */
        if(size == 2)
        for(j = 2; j < texts_n; j += 2)
        {
          int len = strlen( texts[j] );

          #define CHECK_PREFIX( name_, device_class_ ) { \
            int plen = strlen(name_), n=pn-1, found = 0; \
            while(n >= 0) if(strcmp( prefixes[n--], name_ ) == 0) found = 1; \
          if( !found && len >= plen && memcmp( texts[j], name_, plen) == 0 ) \
          { \
            device_class = device_class_; \
            prefixes[pn++] = name_; \
          }}
          CHECK_PREFIX( "EDID_", OPENICC_DEVICE_MONITOR );
          CHECK_PREFIX( "Exif_", OPENICC_DEVICE_CAMERA );
          CHECK_PREFIX( "lraw_", OPENICC_DEVICE_CAMERA );
          CHECK_PREFIX( "PPD_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "CUPS_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "GUTENPRINT_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "SANE_", OPENICC_DEVICE_SCANNER );
        }

        /* add device class */
        fprintf( stdout, OPENICC_DEVICE_JSON_HEADER, device_class, 1 );

        /* add prefix key */
        if(pn)
        {
          for(j = 0; j < pn; ++j)
          {
            if(j == 0)
            {
                fprintf( stdout, "              \"prefix\": \"" );
            }
                fprintf( stdout, "%s",
                         prefixes[j] );
            if(pn > 1 && j < pn-1)
                fprintf( stdout, "," );
            if(j == pn-1)
              fprintf( stdout, "\",\n" );
          }
        }

        /* add device and driver calibration properties */
        for(j = 2; j < texts_n; j += 2)
        {
          if(texts_n > j+1)
            fprintf( stdout, "              \"%s\": \"%s\"",
                     texts[j], texts[j+1] );
          if(j < texts_n - 2)
            fprintf( stdout, ",\n" );
        }

        fprintf( stdout, "\n"OPENICC_DEVICE_JSON_FOOTER );
      }
    }

    oyProfile_Release( &p );
  }


  return error;
}
