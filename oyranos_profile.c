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

#include "oyjl/oyjl_tree.h"

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
  "          \"%s\": {\n"
#define OPENICC_DEVICE_JSON_FOOTER \
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
  fprintf( stderr, "      -p NUMBER  %s\n",     _("select tag"));
  fprintf( stderr, "      -n NAME  %s\n",       _("select tag"));
  fprintf( stderr, "      -s NAME  %s\n",       _("add prefix"));
  fprintf( stderr, "      -c NAME  %s\n",       _("use device class"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Dump Device Infos to JSON:"));
  fprintf( stderr, "      %s -o FILE_NAME\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Write to ICC profile:"));
  fprintf( stderr, "      %s -w NAME [-j FILE_NAME] FILE_NAME\n",        argv[0]);
  fprintf( stderr, "      -w NAME  %s\n",       _("use new name"));
  fprintf( stderr, "      -j FILE_NAME  %s\n",  _("embed OpenICC device JSON from file"));
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
             * name_space = 0,
             * json_name = 0,
             * profile_name = 0;
  const char * prefixes[24] = {0}; int pn = 0;
  const char * device_class = "unknown";
  oyProfile_s * p;
  oyProfileTag_s * tag;

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
              case 'j': OY_PARSE_STRING_ARG(json_name); break;
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
              case 'w': OY_PARSE_STRING_ARG(profile_name); break;
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

  if(json_name && !profile_name)
  {
    fprintf(stderr, "%s %s\n", _("missed -w option to write a ICC profile"), _("Exit!"));
    printfHelp(argc, argv);
    exit(1);
  }

  if(profile_name && !file_name)
  {
    fprintf(stderr, "%s %s\n", _("Need a ICC profile to modify."), _("Exit!"));
    printfHelp(argc, argv);
    exit(1);
  }

  if(file_name && profile_name)
  {
    oyConfig_s * config;
    oyjl_value_s * json = 0,
                 * json_device,
                 * json_tmp;
    char * prefix, * val, * key, * tmp;

    p = oyProfile_FromFile( profile_name, 0, 0 );
    if(p)
    {
      fprintf(stderr, "%s: %s %s\n", _("Profile exists already"), profile_name, _("Exit!"));
      printfHelp(argc, argv);
      exit(1);
    }

    p = oyProfile_FromFile( file_name, 0, 0 );
    if(!p)
      exit(1);

    error = oyProfile_AddTagText( p, icSigProfileDescriptionTag, profile_name );

    {
      char * json_text;
      size_t json_size = 0;
      yajl_status status;
      json_text = oyReadFileToMem_( json_name, &json_size, oyAllocateFunc_ );
      config = oyConfig_New( "//" OY_TYPE_STD "/config", 0 );
      status = oyjl_tree_from_json( json_text, &json, 0 );
      oyDeAllocateFunc_(json_text);
    }
    json_device = oyjl_tree_get_value( json,
                                      "org/freedesktop/openicc/device/[0]/[0]");
    json_tmp = oyjl_tree_get_value( json_device, "prefixes" );
    
    prefix = oyjl_print_text( &json_tmp->value.text );
  } else
  if(file_name)
  {
    char ** texts = NULL;
    int32_t texts_n = 0, i,j,k, count;

    p = oyProfile_FromFile( file_name, 0, 0 );

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
        int has_prefix = 0;

        texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                      &tag_size, malloc );

        if(texts_n > 2)
          size = atoi(texts[0]);

        /* collect key prefixes and detect device class */
        if(size == 2)
        for(j = 2; j < texts_n; j += 2)
          if(strcmp(texts[j],"prefix") == 0)
            has_prefix = 1;

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
          CHECK_PREFIX( "EXIF_", OPENICC_DEVICE_CAMERA );
          CHECK_PREFIX( "lRAW_", OPENICC_DEVICE_CAMERA );
          CHECK_PREFIX( "PPD_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "CUPS_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "GUTENPRINT_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "SANE_", OPENICC_DEVICE_SCANNER );
        }

        /* add device class */
        fprintf( stdout, OPENICC_DEVICE_JSON_HEADER, device_class );

        /* add prefix key */
        if(pn && !has_prefix)
        {
          for(j = 0; j < pn; ++j)
          {
            if(j == 0)
            {
                fprintf( stdout, "            \"prefix\": \"" );
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
          int vals_n = 0;
          char ** vals = 0, * val = 0;

          if(texts_n > j+1)
          {
            if(texts[j+1][0] == '<')
              fprintf( stdout, "            \"%s\": \"%s\"",
                     texts[j], texts[j+1] );
            else
            {
              /* split into a array with a useful delimiter */
              vals = oyStringSplit_( texts[j+1], ':', &vals_n, malloc );
              if(vals_n > 1)
              {
                STRING_ADD( val, "            \"");
                STRING_ADD( val, texts[j] );
                STRING_ADD( val, ": [" );
                for(k = 0; k < vals_n; ++k)
                {
                  if(k != 0)
                  STRING_ADD( val, "," );
                  STRING_ADD( val, "\"" );
                  STRING_ADD( val, vals[k] );
                  STRING_ADD( val, "\"" );
                }
                STRING_ADD( val, "]");
                fprintf( stdout, "%s", val );
                if(val) free( val );
              } else
                fprintf( stdout, "            \"%s\": \"%s\"",
                     texts[j], texts[j+1] );

              oyStringListRelease_( &vals, vals_n, free );
            }
          }
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
