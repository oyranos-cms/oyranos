/** @file oyranos_gamma.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    gamma loader
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/03
 *
 * It reads the default profile(s) from the Oyranos CMS and recalls this
 * profile(s)
 * as new default profile for a screen, including a possible curves upload to
 * the video card.
 * Currently You need xcalib installed to do the curves upload.
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_config_internal.h"
#ifdef HAVE_X11
#include <X11/Xcm/Xcm.h>
#include <X11/Xcm/XcmEvents.h>
#ifdef HAVE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif
#endif

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_icc.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include "oyProfile_s_.h"
#include "oyRectangle_s.h"


#ifdef XCM_HAVE_X11
int   updateOutputConfiguration      ( Display           * display );
int            getDeviceProfile      ( Display           * display,
                                       oyConfig_s        * device,
                                       int                 screen );
void  cleanDisplay                   ( Display           * display );
int  runDaemon                       ( const char        * display_name );
#endif

void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }
int   compareRanks                   ( const void       * rank1,
                                       const void       * rank2 )
{const int32_t *r1=(int32_t*)rank1, *r2=(int32_t*)rank2; if(r1[1] < r2[1]) return 1; else return 0;}

int main( int argc , char** argv )
{
  char *display_name = 0;
  char *monitor_profile = 0;
  int error = 0;

  /* the functional switches */
  int erase = 0;
  int unset = 0;
  int list = 0;
  int setup = 0;
  int daemon = 0;
  char * format = 0;
  char * output = 0;
  int server = 0;
  int x_color_region_target = 0;
  int device_meta_tag = 0;
  char * add_meta = 0,
       * prof_name = 0,
       * module_name = 0,
       * new_profile_name = 0;
  char * device_class = 0;
  int list_modules = 0;
  int list_taxi = 0;
  int verbose = 0;
  int simple = 0;

  char *ptr = NULL;
  int x = 0, y = 0;
  int device_pos = -1;
  char *oy_display_name = NULL;
  oyProfile_s * prof = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  oyConfig_s * c = 0;
  oyOption_s * o = 0;
  size_t size = 0;
  const char * filename = 0;
  char * data = 0;
  uint32_t n = 0;
  int i;

  if(getenv(OY_DEBUG))
  {
    int value = atoi(getenv(OY_DEBUG));
    if(value > 0)
      oy_debug += value;
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  STRING_ADD( device_class, "monitor" );

  if(getenv("DISPLAY"))
    display_name = strdup(getenv("DISPLAY"));

  if(argc != 1)
  {
    int pos = 1;
    const char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'e': erase = 1; monitor_profile = 0; break;
              case 'c': x_color_region_target = 1; monitor_profile = 0; break;
              case 'd': server = 1; OY_PARSE_INT_ARG( device_pos ); break;
              case 'f': OY_PARSE_STRING_ARG(format); monitor_profile = 0; break;
              case 'l': list = 1; monitor_profile = 0; break;
              case 'm': device_meta_tag = 1; break;
              case 'o': OY_PARSE_STRING_ARG(output); monitor_profile = 0; break;
              case 'u': unset = 1; monitor_profile = 0; break;
              case 'x': server = 1; OY_PARSE_INT_ARG( x ); break;
              case 'y': server = 1; OY_PARSE_INT_ARG( y ); break;
              case 'v': if(verbose) oy_debug += 1; verbose = 1; break;
              case 's': setup = 1; break; /* implicite -> setup */
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("erase"))
                        { erase = 1; monitor_profile = 0; i=100; break; }
                        else if(OY_IS_ARG("unset"))
                        { unset = 1; monitor_profile = 0; i=100; break; }
                        else if(strcmp(&argv[pos][2],"x_color_region_target") == 0)
                        { x_color_region_target = 1; i=100; break; }
                        else if(OY_IS_ARG("setup"))
                        { setup = 1; i=100; break; }
                        else if(OY_IS_ARG("daemon"))
                        { daemon = 1; i=100; break; }
                        else if(OY_IS_ARG("format"))
                        { OY_PARSE_STRING_ARG2(format, "format"); break; }
                        else if(OY_IS_ARG("output"))
                        { OY_PARSE_STRING_ARG2(output, "output"); break; }
                        else if(OY_IS_ARG("add-edid"))
                        { OY_PARSE_STRING_ARG2(add_meta,"add-edid"); break; }
                        else if(OY_IS_ARG("name"))
                        { OY_PARSE_STRING_ARG2(new_profile_name, "name"); break; }
                        else if(OY_IS_ARG("profile"))
                        { OY_PARSE_STRING_ARG2(prof_name, "profile"); break; }
                        else if(OY_IS_ARG("display"))
                        { const char * t=0; OY_PARSE_STRING_ARG2(t, "display");
                          if(t) display_name = strdup(t); break; }
                        else if(OY_IS_ARG("modules"))
                        { list_modules = 1; i=100; break; }
                        else if(OY_IS_ARG("module"))
                        { OY_PARSE_STRING_ARG2(module_name, "module"); break; }
                        else if(OY_IS_ARG("list"))
                        { list = 1; monitor_profile = 0; i=100; break; }
                        else if(OY_IS_ARG("list-taxi"))
                        { list_taxi = 1; i=100; break; }
                        else if(OY_IS_ARG("short"))
                        { simple = 1; i=100; break;}
                        else if(OY_IS_ARG("verbose"))
                        { if(verbose) oy_debug += 1; verbose = 1; i=100; break;}
                        }
              default:
                        printf("\n");
                        printf("oyranos-monitor v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a color profile administration tool for monitors"));
                        printf("%s:\n",                 _("Usage"));
                        printf("  %s\n",               _("Set new profile:"));
                        printf("      %s [-x pos -y pos | -d number] %s\n", argv[0],
                                                       _("profile name"));
                        printf("\n");
                        printf("  %s\n",               _("Erase profile:"));
                        printf("      %s -e [-x pos -y pos | -d number]\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Activate profiles:"));
                        printf("      %s [-x pos -y pos | -d number]\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("List devices:"));
                        printf("      %s -l [-x pos -y pos | -d number] --short\n", argv[0]);
                        printf("      %s\n",           _("--short print only the file name"));
                        printf("\n");
                        printf("  %s\n",               _("List Taxi DB profiles for selected device:"));
                        printf("      %s --list-taxi [-x pos -y pos | -d number]\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("List modules:"));
                        printf("      %s --modules\n",        argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Dump data:"));
                        printf("      %s -f=[edid|icc|edid_icc] [-o=edid.bin] [-x pos -y pos | -d number] [-m]\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("General options:"));
                        printf("      %s\n",           _("-v verbose"));
                        printf("      %s\n",           _("--module name"));
                        printf("      %s\n",           _("-d device_position_start_from_zero"));
                        printf("\n");
                        printf(_("For more informations read the man page:"));
                        printf("\n");
                        printf("      man oyranos-monitor\n");
                        exit (0);
                        break;
            }
            break;
        default:
            monitor_profile = argv[pos];
            erase = 0;
            unset = 0;
      }
      if( wrong_arg )
      {
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) fprintf( stderr, "%s\n", argv[1] );
  }

  if(verbose)
    fprintf(stderr, "oyranos-monitor v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a color profile administration tool for monitors"));

#ifndef __APPLE__
  if(!display_name)
  {
    WARNc_S( _("DISPLAY variable not set: giving up.") );
    error = 1;
    return error;
  }
#endif

  /* cut off the screen information */
  if(display_name &&
     (ptr = strchr(display_name,':')) != 0)
    if( (ptr = strchr(ptr, '.')) != 0 )
      ptr[0] = '\000';


  {
    if(!erase && !unset && !list && !setup && !format &&
       !add_meta && !list_modules && !list_taxi)
      setup = 1;

    if(module_name)
    {
      STRING_ADD( device_class, ".");
      STRING_ADD( device_class, module_name);
    }

    /* by default a given monitor profile is used to setup the major monitor */
    if(monitor_profile && !server && device_pos == -1)
      device_pos = 0;

    if(device_pos != -1)
    {

      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
      error = oyOptions_SetFromText( &options, "//"OY_TYPE_STD"/config/edid",
                                       "1", OY_CREATE_NEW );
      if(server)
        error = oyOptions_SetFromText( &options,
                                       "//"OY_TYPE_STD"/config/device_name",
                                       oy_display_name, OY_CREATE_NEW );
      else
        error = oyOptions_SetFromText( &options,
                                       "//"OY_TYPE_STD"/config/display_name",
                                       display_name, OY_CREATE_NEW );

      error = oyDevicesGet( 0, device_class, options, &devices );

      n = oyConfigs_Count( devices );
      if(error <= 0 && 0 <= device_pos && device_pos < n )
      {
        c = oyConfigs_Get( devices, device_pos );
        oy_display_name = strdup( oyConfig_FindString( c, "device_name", 0 ));
        oyConfig_Release( &c );
      } else
        fprintf( stderr, "%s %d. %s: %d\n", _("Could not resolve device"),
                 device_pos, _("Available devices"), n);
      oyConfigs_Release( &devices );
      oyOptions_Release( &options );
    } else if(server)
      oy_display_name = oyGetDisplayNameFromPosition2 ( OY_TYPE_STD,
                                                    device_class,
                                                    display_name, x,y,
                                                    oyAllocFunc);

    if(list_modules)
    {
      uint32_t count = 0,
             * rank_list = 0;
      char ** texts = 0;

      error = oyConfigDomainList( device_class, &texts, &count,
                                  &rank_list, 0 );

      for(i = 0; i < count; ++i)
        printf("%s\n", strstr(texts[i],"monitor.") + 8 );
      return error;
    }

    if(list_taxi)
    {
      oyConfig_s * taxi_dev;
      int32_t * ranks;
      int head = 0;
      devices = 0;

      if(!oy_display_name)
      {
        fprintf(stderr,
                "%s\n", _("Please specify a monitor with the -d option.") );
        return error;
      }

      error = oyOptions_SetFromText( &options,
                                 "//" OY_TYPE_STD "/config/command",
                                 "properties", OY_CREATE_NEW );
      error = oyDeviceGet( OY_TYPE_STD, device_class, oy_display_name, 0,
                           &device );
      oyDevicesFromTaxiDB( device, options, &devices, NULL );
      n = oyConfigs_Count( devices );
      if(n == 0)
      fprintf(stderr,
              "%s\n", _("Zero profiles found in Taxi ICC DB") );
      ranks = calloc(sizeof(int32_t), n*2+1);
      for(i = 0; i < n; ++i)
      {
        taxi_dev = oyConfigs_Get( devices, i );
        ranks[2*i+0] = i;
        error = oyConfig_Compare( device, taxi_dev, &ranks[2*i+1] );

        oyConfig_Release( &taxi_dev );
      }
      qsort( ranks, n, sizeof(int32_t)*2, compareRanks );
      for(i = 0; i < n; ++i)
      {
        taxi_dev = oyConfigs_Get( devices, ranks[2*i+0] );
        if(ranks[2*i+1] <= 0 && !verbose)
        {
          oyConfig_Release( &taxi_dev );
          continue;
        }

        if(!head)
        {
          oyOptions_s * cs_options = 0;
          char * text = NULL,
               * report = NULL;

          head = 1;

          if(verbose)
          {
            if(x_color_region_target)
            {
              /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
              error = oyOptions_SetFromText( &cs_options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
            }
            error = oyDeviceGetInfo( device, oyNAME_NICK, cs_options, &text,
                                     oyAllocFunc );
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                "\"%s\" ", text ? text : "???" );
            error = oyDeviceGetInfo( device, oyNAME_NAME, cs_options, &text,
                                     oyAllocFunc );
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                "%s", text ? text : "???" );
            fprintf( stderr, "%s: %s\n", _("Taxi DB entries for device"),
                     report );
          }
          fprintf( stderr, "%s [%s] \"%s\"\n", _("Taxi ID"),
                   _("match value"), _("description") );
          oyOptions_Release( &cs_options );
        }

        printf("%s/0 [%d] ", oyNoEmptyString_m_(
                 oyConfig_FindString(taxi_dev, "TAXI_id", 0)), ranks[2*i+1]);
        printf("\"%s\"\n", oyNoEmptyString_m_(
                 oyConfig_FindString(taxi_dev, "TAXI_profile_description", 0)));

        if(oy_debug)
        {
          char * json_text = 0;
          oyDeviceToJSON( taxi_dev, 0, &json_text, oyAllocateFunc_ );
          fprintf(stderr, "%s\n", json_text );
          oyFree_m_(json_text);
        }

        oyConfig_Release( &taxi_dev );
      }
      oyConfig_Release( &device );
      oyConfigs_Release( &devices );
      oyOptions_Release( &options );

      return error;
    }

    if(format &&
       (strcmp(format,"edid") == 0 ||
        strcmp(format,"icc") == 0 ||
        strcmp(format,"edid_icc") == 0))
    {
      icHeader * header = 0;
      char * out_name = 0;

      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
      error = oyOptions_SetFromText( &options, "//"OY_TYPE_STD"/config/edid",
                                       "1", OY_CREATE_NEW );
      if(oy_display_name)
        error = oyOptions_SetFromText( &options,
                                       "//"OY_TYPE_STD"/config/device_name",
                                       oy_display_name, OY_CREATE_NEW );
      else
        error = oyOptions_SetFromText( &options,
                                       "//"OY_TYPE_STD"/config/display_name",
                                       display_name, OY_CREATE_NEW );

      error = oyDevicesGet( 0, device_class, options, &devices );

      n = oyConfigs_Count( devices );
      if(!error)
      {
        for(i = 0; i < n; ++i)
        {
          c = oyConfigs_Get( devices, i );

          if(strcmp(format,"edid_icc") == 0)
          {
            o = oyConfig_Find( c, "color_matrix.from_edid."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");

            if(o)
            {
              oyOptions_s * opts = oyOptions_New(0),
                          * result = 0;

              error = oyOptions_MoveIn( opts, &o, -1 );
              oyOptions_Handle( "///create_profile.icc",
                                opts,"create_profile.icc_profile.color_matrix",
                                &result );
              prof = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                        oyOBJECT_PROFILE_S );
              oyOptions_Release( &result );
            }

            if(prof)
            {
              uint32_t model_id = 0;
              const char * t = 0;
              error = oyProfile_AddTagText( prof, icSigProfileDescriptionTag,
                                            (char*) output ? output : format );
              t = oyConfig_FindString( c, "EDID_manufacturer", 0 );
              if(t)
                error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t );
              t =  oyConfig_FindString( c, "EDID_model", 0 );
              if(t)
                error = oyProfile_AddTagText( prof, icSigDeviceModelDescTag, t);
              if(device_meta_tag)
              {
                oyOptions_s * opts = 0;
                error = oyOptions_SetFromText( &opts, "///key_prefix_required",
                                                      "EDID_" , OY_CREATE_NEW );
                oyProfile_AddDevice( prof, c, opts );
                oyOptions_Release( &opts );
              }
              data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
              header = (icHeader*) data;
              t = oyConfig_FindString( c, "EDID_mnft", 0 );
              if(t)
                sprintf( (char*)&header->manufacturer, "%s", t );
              t = oyConfig_FindString( c, "EDID_model_id", 0 );
              if(t)
                model_id = atoi( t );
              model_id = oyValueUInt32( model_id );
              memcpy( &header->model, &model_id, 4 );
              oyOption_Release( &o );
            }
          } else
          if(strcmp(format,"edid") == 0)
          {
            o = oyConfig_Find( c, "edid" );
            data = oyOption_GetData( o, &size, oyAllocFunc );
          } else
          if(strcmp(format,"icc") == 0)
          {
            oyOptions_s * cs_options = 0;
            if(x_color_region_target)
            {
              /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
              error = oyOptions_SetFromText( &cs_options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
            }
            oyDeviceAskProfile2( c, cs_options, &prof );
            oyOptions_Release( &cs_options );
            if(device_meta_tag)
            {
              oyOptions_s * opts = 0;
              error = oyOptions_SetFromText( &opts, "///set_device_attributes",
                                                    "true", OY_CREATE_NEW );
              error = oyOptions_SetFromText( &opts, "///key_prefix_required",
                                                    "EDID_" , OY_CREATE_NEW );
              oyProfile_AddDevice( prof, c, opts );
              oyOptions_Release( &opts );
            }
            data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
          }

          if(data && size)
          {
            if(output)
              error = oyWriteMemToFile2_( output,
                                          data, size, 0x01,
                                          &out_name, oyAllocFunc );
            else
              fwrite( data, sizeof(char), size, stdout );
            oyDeAllocFunc( data ); size = 0;
          } else
            error = 1;

          if(!error)
          { if(verbose) fprintf( stderr, "  written to %s\n", out_name ); }
          else
            printf( "Could not write %d bytes to %s\n",
                    (int)size, out_name?out_name:format);
          if(out_name) oyDeAllocFunc(out_name); out_name = 0;

          oyProfile_Release( &prof );
          oyOption_Release( &o );
          oyConfig_Release( &c );
        }
      } else
        WARNc2_S("oyDevicesGet %s %d", _("found issues"),error);

      oyConfigs_Release( &devices );
      oyOptions_Release( &options );

    } else
    if(prof_name && add_meta)
    {
      uint32_t id[4];
      oyBlob_s * edid = oyBlob_New(0);
      char * edid_fn = oyResolveDirFileName_(add_meta);

      data = oyReadFileToMem_( edid_fn, &size, oyAllocateFunc_ );
      oyBlob_SetFromData( edid, data, size, "edid" );
      oyFree_m_(data);
      prof = oyProfile_FromFile( prof_name, OY_NO_CACHE_READ, 0 );
      device = 0;
      oyOptions_Release( &options );
      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "add_meta", OY_CREATE_NEW );
      error = oyOptions_MoveInStruct( &options,
                                     "//" OY_TYPE_STD "/config/icc_profile",
                                      (oyStruct_s**)&prof, OY_CREATE_NEW );
      error = oyOptions_MoveInStruct( &options,
                                     "//" OY_TYPE_STD "/config/edid",
                                      (oyStruct_s**)&edid, OY_CREATE_NEW );
      error = oyDeviceGet( OY_TYPE_STD, device_class, ":0.0", options, &device);
      if(verbose && device)
      {
        /* We need a newly opened profile, otherwise we obtaine cached
           modifications. */
        oyProfile_s * p = oyProfile_FromFile( prof_name, OY_NO_CACHE_READ, 0 );
        oyConfig_s * p_device = oyConfig_FromRegistration( 
                                       oyConfig_GetRegistration( device ), 0 );
        int32_t rank = 0;
        int old_oy_debug = oy_debug;
        char * json = 0;
        oyProfile_GetDevice( p, p_device );

        if(oy_debug > 1)
        {
          error = oyDeviceToJSON( p_device, 0, &json, oyAllocateFunc_ );
          fprintf(stderr, "device from profile %s:\n%s\n", prof_name, json );
          oyFree_m_( json );
        }
        if(oy_debug > 1)
        {
          error = oyDeviceToJSON( device, 0, &json, oyAllocateFunc_ );
          fprintf(stderr, "device from edid %s:\n%s\n", edid_fn, json );
          oyFree_m_( json );
        }

        /*p_device->db = oyOptions_Copy( p_device->backend_core, 0 );
        device->db = oyOptions_Copy( device->backend_core, 0 );*/
        if(oy_debug < 2) oy_debug = 2;
        error = oyConfig_Compare( p_device, device, &rank );
        oy_debug = old_oy_debug;
        fprintf(stderr, "rank of edid to previous profile %d\n", rank);
      }
      oyConfig_Release( &device );
      oyFree_m_(edid_fn);
      prof = (oyProfile_s*)oyOptions_GetType( options, -1, "icc_profile",
                                              oyOBJECT_PROFILE_S );
      oyOptions_Release( &options );
      if(new_profile_name)
        error = oyProfile_AddTagText( prof, icSigProfileDescriptionTag, new_profile_name );
      /* serialise before requesting a ICC md5 */
      data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
      oyFree_m_(data);
      oyProfile_GetMD5( prof, OY_COMPUTE, id );
      oyProfile_ToFile_( (oyProfile_s_*)prof, prof_name );
      oyProfile_Release( &prof );
    }

    if(list)
    {
      char * text = 0,
           * report = 0;
      uint32_t n = 0, i;
      oyOptions_s * cs_options = 0;

      if(x_color_region_target)
      {
        /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
        error = oyOptions_SetFromText( &cs_options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
      }
      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
      error = oyDevicesGet( 0, device_class, options, &devices );


      n = oyConfigs_Count( devices );
      if(error <= 0)
      {
        const char * device_name = 0;
        for(i = 0; i < n; ++i)
        {
          c = oyConfigs_Get( devices, i );
          device_name = oyConfig_FindString( c, "device_name", 0 );

          if( oy_display_name && device_name &&
              strcmp( oy_display_name, device_name ) != 0 )
          {
            oyConfig_Release( &c );
            device_name = 0;
            continue;
          }

          if(verbose)
          fprintf(stderr,"------------------------ %d ---------------------------\n",i);

          error = oyDeviceGetInfo( c, oyNAME_NICK, cs_options, &text,
                                   oyAllocFunc );
          if(!simple)
          {
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                "%d: ", i );
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                "\"%s\" ", text ? text : "???" );
            error = oyDeviceGetInfo( c, oyNAME_NAME, cs_options, &text,
                                     oyAllocFunc );
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                "%s%s", text ? text : "???",
                                (i+1 == n) || device_pos != -1 ? "" : "\n" );
          } else
          {
            oyDeviceAskProfile2( c, cs_options, &prof );
            data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
            if(size && data)
              oyDeAllocFunc( data );
            filename = oyProfile_GetFileName( prof, -1 );
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                "%s%s", filename ? (strrchr(filename,OY_SLASH_C) ? strrchr(filename,OY_SLASH_C)+1:filename) : OY_PROFILE_NONE,
                                (i+1 == n) || device_pos != -1 ? "" : "\n" );
          }
          if(verbose)
          {
            error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, cs_options, &text,
                                     oyAllocFunc );
            fprintf( stderr,"%s\n", text ? text : "???" );
          }

          if(text)
            free( text );

          /* verbose adds */
          if(verbose)
          {
            oyDeviceAskProfile2( c, cs_options, &prof );
            data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
            if(size && data)
              oyDeAllocFunc( data );
            filename = oyProfile_GetFileName( prof, -1 );
            fprintf( stderr, " server profile \"%s\" size: %d\n",
                    filename?filename:OY_PROFILE_NONE, (int)size );

            text = 0;
            oyDeviceProfileFromDB( c, &text, oyAllocFunc );
            fprintf( stderr, " DB profile \"%s\"\n  keys: %s\n",
                    text?text:OY_PROFILE_NONE,
                    oyConfig_FindString( c, "key_set_name", 0 ) ?
                      oyConfig_FindString( c, "key_set_name", 0 ) :
                      OY_PROFILE_NONE );

            oyProfile_Release( &prof );
            oyDeAllocFunc( text );
          }

          oyConfig_Release( &c );
        }

        if(report)
          fprintf( stdout, "%s\n", report );
        oyDeAllocFunc( report ); report = 0;
      } else
        WARNc2_S("oyDevicesGet %s %d", _("found issues"),error);
        
      oyConfigs_Release( &devices );
      oyOptions_Release( &cs_options );
    }

    if(oy_display_name)
    /* make shure the display name is correctly including the screen number */
    {
      error = oyDeviceGet( OY_TYPE_STD, device_class, oy_display_name, 0,
                           &device );

      if(monitor_profile)
      {
        if(verbose)
          fprintf( stdout, "oyDeviceSetProfile()\n" );
        oyDeviceSetProfile( device, monitor_profile );
        if(verbose)
          fprintf( stdout, "oyDeviceUnset()\n" );
        oyDeviceUnset( device );
      } else
      if(erase || unset)
      {
        if(verbose)
          fprintf( stdout, "oyDeviceUnset()\n" );
        oyDeviceUnset( device );
      }
      if(erase)
      {
        if(verbose)
          fprintf( stdout, "oyConfig_EraseFromDB()\n" );
        oyConfig_EraseFromDB( device );
      }

      if(setup)
      {
        if(verbose)
          fprintf( stdout, "oyDeviceSetup()\n" );
        oyDeviceSetup( device );
      }

      oyConfig_Release( &device );
    }
    else if(erase || unset || setup)
    {
      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "list", OY_CREATE_NEW );
      error = oyOptions_SetFromText( &options,
                                     "//"OY_TYPE_STD"/config/display_name",
                                     display_name, OY_CREATE_NEW );

      error = oyDevicesGet( 0, device_class, options, &devices );

      n = oyConfigs_Count( devices );
      if(!error)
      {
        for(i = 0; i < n; ++i)
        {
          device = oyConfigs_Get( devices, i );

          if(erase || unset)
            oyDeviceUnset( device );
          if(erase)
            oyConfig_EraseFromDB( device );
          if(setup)
            oyDeviceSetup( device );

          oyConfig_Release( &device );
        }
      }
      oyConfigs_Release( &devices );
      oyOptions_Release( &options );
    }
  }

  if(oy_display_name)
    oyDeAllocFunc(oy_display_name);

#if defined(XCM_HAVE_X11)
  if(daemon)
    error = runDaemon( display_name );
#else
  if(daemon)
    fprintf( stderr, "daemon mode not supported on your OS\n" );
#endif

  return error;
}

#ifdef XCM_HAVE_X11
void cleanDisplay( Display * display )
{
  int error = 0;
  oyOptions_s * options = 0;
  oyConfigs_s * devices = 0;
  char * display_name = 0, * t;
  int old_oy_debug, i;

    display_name = strdup(XDisplayString(display));
    if(display_name && strchr(display_name,'.'))
    {
      t = strrchr(display_name,'.');
      t[0] = 0;
    }

    /* clean up to 20 displays */
    error = oyOptions_SetFromText( &options,
                                   "//"OY_TYPE_STD"/config/command",
                                   "unset", OY_CREATE_NEW );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    if(display_name)
    {
      t = calloc(sizeof(char), strlen(display_name));
    } else
    {
      display_name = strdup(":0");
      t = calloc(sizeof(char), 8);
    }

    if(t && display_name)
    {
      for(i = 0; i < 20; ++i)
      {
        sprintf( t, "%s.%d", display_name, i );
        error = oyOptions_SetFromText( &options,
                                       "//" OY_TYPE_STD "/config/device_name",
                                       t, OY_CREATE_NEW );
        error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
        if(error) WARNc2_S("%s %d", _("found issues"),error);
        oyConfigs_Release( &devices );
      }
    }
    oyOptions_Release( &options );


    /* get number of connected devices */
    error = oyOptions_SetFromText( &options,
                                   "//"OY_TYPE_STD"/config/command",
                                   "list", OY_CREATE_NEW );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    error = oyOptions_SetFromText( &options,
                                   "//" OY_TYPE_STD "/config/display_name",
                                   display_name, OY_CREATE_NEW );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    oyConfigs_Release( &devices );
    oyOptions_Release( &options );

    /** Monitor hotplugs can easily mess up the ICC profile to device assigment.
     *  So first we erase the _ICC_PROFILE(_xxx) to get a clean state.
     *  We setup the EDID atoms and ICC profiles new.
     *  The ICC profiles are moved to the right places through the 
     *  PropertyChange events recieved by the color server.
     */

    /* refresh EDID */
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "list", OY_CREATE_NEW );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    sprintf( t, "%s.%d", display_name, 0 );
    error = oyOptions_SetFromText( &options,
                                   "//" OY_TYPE_STD "/config/device_name",
                                   t, OY_CREATE_NEW );
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/edid",
                                   "refresh", OY_CREATE_NEW );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    old_oy_debug = oy_debug;
    /*oy_debug = 1;*/
    error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    oy_debug = old_oy_debug;
    oyConfigs_Release( &devices );
    oyOptions_Release( &options );

    free(display_name); display_name = 0;
    free(t); t = 0;
}

int            getDeviceProfile      ( Display           * display,
                                       oyConfig_s        * device,
                                       int                 screen )
{
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;
  oyProfile_s * dst_profile = 0;
  const char * device_name = 0;
  char num[12];
  int error = 0, t_err = 0;

  snprintf( num, 12, "%d", (int)screen );

    o = oyConfig_Find( device, "device_rectangle" );
    if( !o )
    {
      oyMessageFunc_p( oyMSG_WARN, 0, "monitor rectangle request failed" );
      return 1;
    }
    r = (oyRectangle_s*) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );
    if( !r )
    {
      oyMessageFunc_p( oyMSG_WARN, 0,
                   "monitor rectangle request failed" );
      return 1;
    }
    oyOption_Release( &o );

    device_name = oyConfig_FindString( device, "device_name", 0 );
    if(device_name && device_name[0])
    {
      fprintf( stderr,"%d %s %gx%g+%g+%g\n",
             __LINE__, device_name,
             oyRectangle_GetGeo1(r,2), oyRectangle_GetGeo1(r,3),
             oyRectangle_GetGeo1(r,0), oyRectangle_GetGeo1(r,1) );

    } else
    {
       oyMessageFunc_p( oyMSG_WARN, 0,
       "oyDevicesGet list answere included no device_name");
    }

    {
      oyOptions_s * options = 0;
      /*oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/command",
                                       "list", OY_CREATE_NEW );
      oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                                       "yes", OY_CREATE_NEW );*/
      t_err = oyDeviceGetProfile( device, options, &dst_profile );
      oyOptions_Release( &options );
    }

    if(dst_profile)
    {
      /* check that no sRGB is delivered */
      if(t_err)
      {
        oyProfile_s * web = oyProfile_FromStd( oyASSUMED_WEB, 0 );
        if(oyProfile_Equal( web, dst_profile ))
        {
          oyMessageFunc_p( oyMSG_WARN, 0,
                           "Output %s ignoring fallback %d",
                           device_name, error);
          oyProfile_Release( &dst_profile );
          error = 1;
        }
        oyProfile_Release( &web );
      }
    } else
    {
      oyMessageFunc_p( oyMSG_WARN, 0,
                       "Output %s: no ICC profile found %d",
                       device_name, error);
      error = 1;
    }

  return error;
}

int updateOutputConfiguration( Display * display )
{
  int error = 0,
      i, n;
  oyOptions_s * options = 0;
  oyConfigs_s * devices = 0;
  oyConfig_s * device = 0;

  fprintf( stderr,"%s:%d %s()\n", __FILE__, __LINE__, __func__);

  /* allow Oyranos to see modifications made to the compiz Xlib context */
  XFlush( display );

  /* obtain device informations, including geometry and ICC profiles
     from the according Oyranos module */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  if(error) WARNc2_S("%s %d", _("found issues"),error);
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  if(error) WARNc2_S("%s %d", _("found issues"),error);
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  if(error) WARNc2_S("%s %d", _("found issues"),error);
  n = oyOptions_Count( options );
  oyOptions_Release( &options );

  n = oyConfigs_Count( devices );

  cleanDisplay( display );

  for(i = 0; i < n; ++i)
  {
    device = oyConfigs_Get( devices, i );

    error = getDeviceProfile( display, device, i );
    if(error) WARNc2_S("%s %d", _("found issues"),error);

    oyConfig_Release( &device );
  }
  oyConfigs_Release( &devices );

  return 0;
}

int  runDaemon                       ( const char        * display_name )
{
  Display * display;
  Window root;
  Atom net_desktop_geometry, icc_color_desktop;
  int rr_event_base = 0, rr_error_base = 0;
  Atom actual;
  int format;
  unsigned long left, n;
  unsigned char * data = 0;
  XcmeContext_s * c = XcmeContext_New( );

  XcmeContext_Setup2( c, display_name, 0 );
  display = XcmeContext_DisplayGet( c );

  if(!display)
    return 1;

  root = RootWindow( display, DefaultScreen( display ) );

#ifdef HAVE_XRANDR
  XRRQueryExtension( display, &rr_event_base, &rr_error_base );
  XRRSelectInput( display, root, RRScreenChangeNotifyMask | RRCrtcChangeNotifyMask | RROutputChangeNotifyMask | RROutputPropertyNotifyMask);
#endif

  net_desktop_geometry = XInternAtom( display,
                                      "_NET_DESKTOP_GEOMETRY", False );
  icc_color_desktop = XInternAtom( display,
                                      XCM_COLOR_DESKTOP, False );


  for(;;)
  {
    XEvent event;
    XNextEvent( display, &event);
    XGetWindowProperty( display, RootWindow(display,0),
                        icc_color_desktop, 0, ~0, False, XA_STRING,
                        &actual,&format, &n, &left, &data );
    n += left;
    /* we rely on any color server doing X11 setup by its own */
    if(n && data)
      continue;

#ifdef HAVE_XRANDR
    if(event.type == rr_event_base + RRNotify)
    {
      XRRNotifyEvent *rrn = (XRRNotifyEvent *) &event;
      if(rrn->subtype == RRNotify_OutputChange ||
         rrn->subtype == RR_Rotate_0)
      {
        fprintf( stderr,"detected RRNotify_OutputChange event -> update\n");
        updateOutputConfiguration( display );
      }
    } else
#endif
    if( event.type == PropertyNotify &&
        event.xproperty.atom == net_desktop_geometry)
    {
        fprintf( stderr,"detected _NET_DESKTOP_GEOMETRY event -> update\n");
        updateOutputConfiguration( display );
    }
  }

  XcmeContext_Release( &c );

  return 0;
}
#endif

