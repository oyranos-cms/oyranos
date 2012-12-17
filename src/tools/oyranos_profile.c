/** @file oyranos_profile.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2011-2012 (C) Kai-Uwe Behrmann
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


#include "oyProfile_s.h"
#include "oyProfile_s_.h"           /* oyProfile_ToFile_() */
#include "oyProfileTag_s.h"

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
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
  "          \"%s\": [{\n"
#define OPENICC_DEVICE_JSON_FOOTER \
  "            }\n" \
  "          ]\n" \
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
  fprintf( stderr, "      %s -l FILE_NAME\n",        argv[0]);
  fprintf( stderr, "      -p NUMBER     %s\n",  _("select tag"));
  fprintf( stderr, "      -n NAME       %s\n",  _("select tag"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Dump Device Infos to OpenICC device JSON:"));
  fprintf( stderr, "      %s -o FILE_NAME\n",        argv[0]);
  fprintf( stderr, "      -c NAME       %s scanner, monitor, printer, camera ...\n",  _("use device class") );
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Show Profile ID:"));
  fprintf( stderr, "      %s -m FILE_NAME\n",        argv[0]);
  fprintf( stderr, "      -w FILE_NAME  %s\n",  _("write profile with correct ID"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Write to ICC profile:"));
  fprintf( stderr, "      %s -w NAME [-j FILE_NAME] FILE_NAME\n",        argv[0]);
  fprintf( stderr, "      -w NAME       %s\n",  _("use new name"));
  fprintf( stderr, "      -j FILE_NAME  %s\n",  _("embed OpenICC device JSON from file"));
  fprintf( stderr, "      -s NAME       %s\n",  _("add prefix"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      %s\n",           _("-v verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s:\n",               _("Example"));
  fprintf( stderr, "      oyranos-profile -lv -p=1 sRGB.icc\n");
  fprintf( stderr, "      oyranos-profile -w test -j test.json sRGB.icc\n");
  fprintf( stderr, "      oyranos-profile -mv sRGB.icc");
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
      list_hash = 0,
      tag_pos = -1,
      dump_openicc_json = 0,
      verbose = 0;
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
              case 'm': list_hash = 1; break;
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
              case 'v': if(!verbose) verbose = 1; else oy_debug += 1; break;
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
    oyConfig_s * device;
    oyOptions_s * opts = NULL;
    char * json_text;
    char * data = 0;
    size_t size = 0;
    char * pn = 0;
    char * ext = 0;
    const char * t = strrchr(profile_name, '.');
    int i;

    if(error <= 0 && list_hash)
    {
      uint32_t id[4];
      p = oyProfile_FromFile( file_name, 0, 0 );
      oyProfile_GetMD5( p, OY_COMPUTE, id );

      fprintf( stderr, "%s %s\n", _("Write to ICC profile:"), profile_name);
      error = oyProfile_ToFile_( (oyProfile_s_*)p, profile_name);
      return error;
    }

    STRING_ADD( pn, profile_name );
    if(t)
    {
      ++t;
      STRING_ADD( ext, t );
      i = 0;
      while(ext[i])
      {
        ext[i] = tolower(ext[i]);
        ++i;
      }
      if(strcmp(ext,"icc") != 0 &&
         strcmp(ext,"icm") != 0)
      {
        oyFree_m_(ext);
        ext = 0;
      }
    }
    if(!ext)
      STRING_ADD( pn, ".icc" );
    else
      oyFree_m_(ext);

    p = oyProfile_FromFile( pn, 0, 0 );
    if(p)
    {
      fprintf(stderr, "%s: \"%s\" - %s\n", _("Profile exists already"), pn, _("Exit!"));
      printfHelp(argc, argv);
      exit(1);
    }

    p = oyProfile_FromFile( file_name, 0, 0 );
    if(!p)
      exit(1);

    error = oyProfile_AddTagText( p, icSigProfileDescriptionTag, profile_name );

    {
      size_t json_size = 0;
      json_text = oyReadFileToMem_( json_name, &json_size, oyAllocateFunc_ );
      oyDeviceFromJSON( json_text, NULL, &device );
    }
      
    error = oyOptions_SetFromText( &opts, "///set_device_attributes",
                                   "true", OY_CREATE_NEW );
    oyProfile_AddDevice( p, device, opts );
    oyOptions_Release( &opts );
    data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );

    if(data && size)
    {
      uint32_t id[4];

      oyFree_m_(data);

      oyProfile_GetMD5( p, OY_COMPUTE, id );
      oyProfile_ToFile_( (oyProfile_s_*)p, pn );
      oyFree_m_( pn );
    }

    oyDeAllocateFunc_(json_text);

    oyProfile_Release( &p );
    oyConfig_Release( &device );

  } else
  if(file_name)
  {
    char ** texts = NULL;
    int32_t texts_n = 0, i,j,k, count;

    p = oyProfile_FromFile( file_name, 0, 0 );

    if(!p)
    {
      error = 1;
    }

    if(error <= 0 && list_tags)
    {
      fprintf(stderr, "%s \"%s\" %d:\n", _("ICC profile"), file_name,
              (int)oyProfile_GetSize(p,0));
      count = oyProfile_GetTagCount( p );
      for(i = 0; i < count; ++i)
      {
        tag = oyProfile_GetTagByPos( p, i );

        if(tag &&
           ((tag_name == NULL && (tag_pos == -1 || tag_pos == i)) ||
            (tag_name != NULL && (strcmp(oyICCTagName(oyProfileTag_GetUse(tag)),
                                         tag_name) == 0)))
          )
        {
          int32_t tag_size = 0;
          texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                        &tag_size, malloc );

          fprintf( stdout, "%s/%s[%d] %d @ %d %s",
                   oyICCTagName(oyProfileTag_GetUse(tag)),
                   oyICCTagTypeName(oyProfileTag_GetType(tag)), i,
                   (int)tag_size, (int)oyProfileTag_GetOffset( tag ),
                   oyICCTagDescription(oyProfileTag_GetUse(tag)));
          if((verbose || oy_debug) && texts)
          {
            fprintf( stdout, ":\n" );
              for(j = 0; j < texts_n; ++j)
                if(texts[j])
                  fprintf( stdout, "%s\n", texts[j] );
          } else
          {
            fprintf( stdout, "\n" );
          }
        }

        oyProfileTag_Release( &tag );
      }
    } else
    if( error <= 0 && dump_openicc_json )
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
          if(texts[j] && strcmp(texts[j],"prefix") == 0)
            has_prefix = 1;

        if(size == 2)
        for(j = 2; j < texts_n; j += 2)
        {
          int len = texts[j] ? strlen( texts[j] ) : 0;

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
          int vals_n = 0;
          char ** vals = 0, * val = 0;

          if(texts[j] && texts[j+1] && texts_n > j+1)
          {
            if(texts[j+1][0] == '<')
              fprintf( stdout, "              \"%s\": \"%s\"",
                     texts[j], texts[j+1] );
            else
            {
              /* split into a array with a useful delimiter */
              vals = oyStringSplit_( texts[j+1], ':', &vals_n, malloc );
              if(vals_n > 1)
              {
                STRING_ADD( val, "              \"");
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
                fprintf( stdout, "              \"%s\": \"%s\"",
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

    if(error <= 0 && list_hash)
    {
      uint32_t * i;
      uint32_t md5[4],
               id[4];
      oyProfile_GetMD5(p, OY_FROM_PROFILE, id);
      error = oyProfile_GetMD5(p, OY_COMPUTE, md5);

      i = (uint32_t*)md5;
      if(!verbose)
        fprintf( stdout, "%x%x%x%x\n",
            i[0],i[1],i[2],i[3] );
      else
        fprintf( stdout, "%x%x%x%x[%x%x%x%x] %s\n",
            i[0],i[1],i[2],i[3], id[0],id[1],id[2],id[3], file_name );
    }

    oyProfile_Release( &p );
  }


  return error;
}
