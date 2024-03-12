/** @file oyranos_file.cpp
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2011-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    image file handler
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2011/04/06
 *
 */
/* c++ -Wall -g oyranos_file.cpp `pkg-config --cflags --libs oyranos` -o oyranos-file */

#include <oyConversion_s.h>
#include <oyranos_devices.h>

#include "oyranos_helper.h"
#include "oyranos_helper_macros_cli.h"
#include "oyranos_i18n.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
typedef struct oyProfile_s_ oyProfile_s_;
int          oyProfile_ToFile_       ( oyProfile_s_      * profile,
                                       const char        * file_name );
void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }
}

void displayHelp(char ** argv)
{
  printf("\n");
  printf("oyranos-camera-raw v%d.%d.%d %s\n",
         OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
         _("is a CameraRaw conversion tool"));
  printf("%s:\n",                 _("Usage"));
  printf("  %s\n",               _("Convert CameraRaw image to Rgb image:"));
  printf("      %s -i file.dng -o file.ppm\n", argv[0]);
  printf("\n");
  printf("  %s\n",               _("Dump data:"));
  printf("      %s -f=[icc|fallback-icc|openicc|openicc-rank-map] -i file.dng [-o=device.json] [--only-db]\n", argv[0]);
  printf("         -f icc   \t%s\n",_("extract ICC color profile"));
  printf("         -f fallback-icc   \t%s\n",_("create fallback ICC profile"));
  printf("         -f openicc\t%s\n",_("generate OpenICC device color reproduction JSON"));
  /* JSON is a text based file format for exchange of data. The rank map is used to sort color profiles according to their fit to a specific device. */
  printf("         -f openicc-rank-map\t%s\n",_("dump OpenICC device color state rank map JSON"));
  printf("         --only-db\t%s\n",_("use only DB keys for -f=openicc"));
  printf("\n");
  printf("  %s\n",               _("Modify Profile:"));
  printf("      %s -i file.dng -p my_profile.icc -o my_modified_profile.icc\n", argv[0]);
  printf("         -p %s\t%s\n",    _("ICC_FILE_NAME"), _("embedd device and driver information into ICC meta tag"));
  printf("         -o %s\t%s\n",    _("ICC_FILE_NAME"), _("write to specified file"));
  printf("\n");
  printf("  %s\n",               _("Show Help:"));
  printf("      %s [-h]\n", argv[0]);
  printf("\n");
  printf("  %s\n",               _("General options:"));
  printf("         -i %s\t%s\n",    _("FILE"),   _("CameraRaw image"));
  printf("         -o %s\t%s\n",    _("FILE"),   _("write to specified file"));
  printf("         -v      \t%s\n", _("verbose"));
  printf("\n");
  printf("  %s:\n",               _("Example"));
  printf("    %s:\n",             _("Embedd device meta tag from image device into given device profile"));
  printf("      %s --image image.dng --profile camera.icc --output camera_new.icc\n", argv[0]);
  printf("\n");
  printf("    %s\n",             _("Convert CameraRaw image to Rgb image:"));
  printf("      %s -i image.dng -o image.png\n", argv[0]);
  printf("\n");
  printf("%s", _("For more information read the man page:"));
  printf("\n");
  printf("      man oyranos-camera-raw\n");
}

/** Function oyConversion_FromImageFileName
 *  @brief   generate a Oyranos graph from a image file name
 *
 *  @param[in]     file_name           name of image file
 *  @param[in]     profile_name        name of ICC profile file
 *  @param[in]     flags               set options
 *                                     - 0x01 - request device
 *                                     - 0x02 - no data processing
 *  @param[in]     icc_profile_flags   ICC profile selection flags, see ::oyProfile_FromFile()
 *  @param[in]     obj                 Oyranos object (optional)
 *  @return                            generated new graph, owned by caller
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/04/06 (Oyranos: 0.3.0)
 *  @date    2011/04/06
 */
oyConversion_s * oyConversion_FromImageFileName  (
                                       const char        * file_name,
                                       const char        * profile_name,
                                       uint32_t            flags,
                                       uint32_t            icc_profile_flags,
                                       oyObject_s          obj )
{
  oyFilterNode_s * in, * out;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;

  if(!file_name)
    return NULL;

  /* start with an empty conversion object */
  conversion = oyConversion_New( obj );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/file_read.meta", 0, obj );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );

  /* add a file name argument */
  /* get the options of the input node */
  if(in)
  options = oyFilterNode_GetOptions( in, OY_SELECT_FILTER );
  /* add a new option with the appropriate value */
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/file_read/filename",
                                 file_name, OY_CREATE_NEW );
  /* set image process options */
  if(flags & 0x01)
    error = oyOptions_SetFromString( &options,
                                   "//" OY_TYPE_STD "/file_read/device",
                                   "1", OY_CREATE_NEW );

  if(profile_name)
  {
    oyProfile_s * prof = oyProfile_FromName( profile_name, icc_profile_flags, 0 );
    error = oyOptions_MoveInStruct( &options,
                                "//" OY_TYPE_STD "/config/icc_profile.add_meta",
                                    (oyStruct_s**)&prof, OY_CREATE_NEW );
  }
  if(flags & 0x02)
    error = oyOptions_SetFromString( &options,
                                   "//" OY_TYPE_STD "/file_read/render",
                                   "0", OY_CREATE_NEW );

  if(icc_profile_flags)
    error = oyOptions_SetFromInt( &options,
                                  "//" OY_TYPE_STD "/icc_profile_flags",
                                  icc_profile_flags, 0, OY_CREATE_NEW );
  /* release the options object, this means its not any more refered from here*/
  oyOptions_Release( &options );

  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, obj );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  /* set the output node of the conversion */
  if(!error)
    oyConversion_Set( conversion, 0, out );

  return conversion;
}


int main(int argc, char ** argv)
{
  int error = 0;

  /* the functional switches */
  char * format = 0;
  char * output = 0;
  int only_db = 0;
  char * prof_name = 0,
       * new_profile_name = 0,
       * image_name = 0;
  int verbose = 0;
  uint32_t icc_profile_flags = 0;

  int i;
  oyOptions_s * options = 0;
  size_t size = 0;
  char * data = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  {
    int pos = 1;
    const char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; (unsigned int)i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'f': OY_PARSE_STRING_ARG(format); break;
              case 'i': OY_PARSE_STRING_ARG(image_name); break;
              case 'o': OY_PARSE_STRING_ARG(output); break;
              case 'p': OY_PARSE_STRING_ARG(prof_name); break;
              case 'v': if(verbose) oy_debug += 1; verbose = 1; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("image"))
                        { OY_PARSE_STRING_ARG2(image_name, "image"); break; }
                        else if(OY_IS_ARG("format"))
                        { OY_PARSE_STRING_ARG2(format, "format"); break; }
                        else if(OY_IS_ARG("output"))
                        { OY_PARSE_STRING_ARG2(output, "output"); break; }
                        else if(OY_IS_ARG("only-db"))
                        { only_db = 1; i=100; break; }
                        else if(OY_IS_ARG("name"))
                        { OY_PARSE_STRING_ARG2(new_profile_name, "name"); break; }
                        else if(OY_IS_ARG("profile"))
                        { OY_PARSE_STRING_ARG2(prof_name, "profile"); break; }
                        else if(OY_IS_ARG("verbose"))
                        { if(verbose) oy_debug += 1; verbose = 1; i=100; break;}
                        } OY_FALLTHROUGH
              default:
                        displayHelp(argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        displayHelp(argv);
                        exit (0);
      }
      if( wrong_arg )
      {
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) printf( "%s\n", argv[1] );
  }
  if(argc == 1)
  {
                        displayHelp(argv);
                        exit (0);
  }

  oyConversion_s * c;
  oyImage_s * image = NULL;
  oyOptions_s * image_tags = NULL;
  oyConfig_s * device = NULL;
  char * out_name = NULL;
  oyOption_s * o = NULL;
  oyProfile_s * profile = NULL;
  oyOption_s * opt = NULL;
  icHeader * header;

  icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                            "//" OY_TYPE_STD "/icc_color", NULL, 0 );

  if(format &&
     (strcmp(format,"icc") == 0 ||
      strcmp(format,"fallback-icc") == 0))
  {
    oyOptions_s * options = NULL;
    if(strcmp(format,"fallback-icc") == 0)
      oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile.fallback",
                             "yes", OY_CREATE_NEW );
    else
      oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile",
                             "yes", OY_CREATE_NEW );
    error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );  
    if(icc_profile_flags)
      error = oyOptions_SetFromInt( &options,
                                  "//" OY_TYPE_STD "/icc_profile_flags",
                                  icc_profile_flags, 0, OY_CREATE_NEW );
    error = oyDeviceGet( 0, "camera", image_name, options, &device );

    oyOption_s * o = oyOptions_Find( *oyConfig_GetOptions(device, "data"),
                                     "icc_profile", oyNAME_PATTERN );
    if( o )
    {
      profile = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
      oyOption_Release( &o );
    }

    if(strcmp(format,"icc") == 0 && !profile)
    {
      c = oyConversion_FromImageFileName( image_name, prof_name,
                                          0x01 | 0x04, icc_profile_flags, 0 );
      image = oyConversion_GetImage( c, OY_OUTPUT );
      oyImage_Release( &image );
      image = oyConversion_GetImage( c, OY_INPUT );
      profile = oyImage_GetProfile( image );
    }

    data = (char*) oyProfile_GetMem( profile, &size, 0, oyAllocFunc);
    if(size && data)
    {
      if(output)
        error = oyWriteMemToFile2_( output,
                                    data, size, 0x01,
                                    &out_name, oyAllocFunc );
      else
        fwrite( data, sizeof(char), size, stdout );

      oyDeAllocFunc( data ); data = 0;
    }
    exit(0);

  } else
  {
    c = oyConversion_FromImageFileName( image_name, prof_name,
                                        0x01 | 0x04, icc_profile_flags, 0 );
    image = oyConversion_GetImage( c, OY_OUTPUT );

    oyOptions_s * image_tags = oyImage_GetTags( image );
    if(image)
      device = (oyConfig_s*)oyOptions_GetType( image_tags, 0,
                                           "device",
                                           oyOBJECT_CONFIG_S );
    opt = oyConfig_Find( device, "icc_profile.add_meta" );
    profile = (oyProfile_s*) oyOption_GetStruct( opt, oyOBJECT_PROFILE_S );
    if(prof_name && profile)
    {
      char * new_name = NULL, * tmp;

      if(output)
      {
        char * t = strrchr(output, '.'),
             * ext = NULL;

        tmp = oyStringCopy( output, oyAllocFunc );

        if(t)
        {
          ++t;
          STRING_ADD( ext, t );
          if(oyStringCaseCmp_(ext,"icc") != 0 &&
             oyStringCaseCmp_(ext,"icm") != 0)
          {
            oyFree_m_(ext);
            ext = NULL;
          }
        }
        if(!ext)
          STRING_ADD( tmp, ".icc" );
        else
          oyFree_m_(ext);

        output = tmp;
        tmp = NULL;
      }

      oyStringAddPrintf( &new_name, oyAllocFunc, oyDeAllocFunc,
                         "%s", output );
      if((tmp = strstr(new_name, ".icc")) != NULL)
        tmp[0] = '\000';

          uint32_t model_id = 0;
          const char * t = 0;
          error = oyProfile_AddTagText( profile, icSigProfileDescriptionTag,
                                        (char*) new_name );
          t = oyConfig_FindString( device, "manufacturer", 0 );
          if(t)
            error = oyProfile_AddTagText( profile, icSigDeviceMfgDescTag, t );
          t =  oyConfig_FindString( device, "model", 0 );
          if(t)
            error = oyProfile_AddTagText( profile, icSigDeviceModelDescTag, t);

          {
            oyOptions_s * opts = 0;
            t = oyConfig_FindString( device, "prefix", 0 );
            error = oyOptions_SetFromString( &opts, "///key_prefix_required",
                                                  t, OY_CREATE_NEW );
            oyProfile_AddDevice( profile, device, opts );
            oyOptions_Release( &opts );
          }

          data = (char*) oyProfile_GetMem( profile, &size, 0, oyAllocFunc );
          header = (icHeader*) data;
          t = oyConfig_FindString( device, "mnft", 0 );
          if(t)
            sprintf( (char*)&header->manufacturer, "%s", t );
          t = oyConfig_FindString( device, "model_id", 0 );
          if(t)
            model_id = atoi( t );
          model_id = oyValueUInt32( model_id );
          memcpy( &header->model, &model_id, 4 );
          oyOption_Release( &o );

      oyFree_m_( new_name );
    } else if(format && device)
    {
    if(strcmp(format,"openicc") == 0 ||
       strcmp(format,"openicc-rank-map") == 0)
    {
      char * json = 0;

      if(strcmp(format,"openicc") == 0)
      {
        error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/options/source",
                                       "db", OY_CREATE_NEW );  
        error = oyDeviceToJSON( device, options, &json, oyAllocFunc );
        oyOptions_Release( &options );

        /* it is possible that no DB keys are available; use all others */
        if(!json && !only_db)
          error = oyDeviceToJSON( device, NULL, &json, oyAllocFunc );

        if(!json)
        {
          fprintf( stderr, "no DB data available\n" );
          exit(0);
        }
      } else
      {
        const oyRankMap * map = oyConfig_GetRankMap( device );
        if(!map)
        { fprintf( stderr, "no RankMap found\n" ); exit(0);
        }

        error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/options/device_class",
                                       "camera", OY_CREATE_NEW );  
        oyRankMapToJSON( map, options, &json, oyAllocFunc );
        oyOptions_Release( &options );
        if(!json)
        { fprintf( stderr, "no JSON from RankMap available\n" ); exit(0);
        }
      }

      if(output)
        error = oyWriteMemToFile2_( output,
                                    json, strlen(json), 0x01,
                                    &out_name, oyAllocFunc );
      else
        fprintf( stdout, "%s", json );

      if(json)
        size = strlen(json);
      oyDeAllocFunc( json ); json = 0;
    } else {
      displayHelp(argv);
      exit (1);
    }
    } else
    {
      oyImage_Release( &image );
      image = oyConversion_GetImage( c, OY_INPUT );
      char * comment = 0;
      STRING_ADD( comment, "source image was " );
      STRING_ADD( comment, image_name );
      oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/file_write/comment",
                             comment, OY_CREATE_NEW );
      if((error = oyImage_ToFile( image, output, options )) != 0)
        fprintf( stderr, "error in oyImage_ToFile( %s )\n", output );
      else if(verbose)
        fprintf( stderr, "wrote to %s\n", output );
      exit(0);
    }

    if(output)
    {
      error = oyWriteMemToFile2_( output,
                                  data, size, 0x01,
                                  &out_name, oyAllocFunc );
      fprintf( stderr, "wrote to %s\n", out_name );

    } else
      fwrite( data, sizeof(char), size, stdout );

    exit(0);
  }


  oyProfile_Release( &profile );
  oyOption_Release( &opt );
  oyConfig_Release( &device );
  oyConversion_Release( &c );
  oyOptions_Release( &image_tags );

  oyImage_Release( &image );

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return 0;
}
