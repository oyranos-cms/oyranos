/** @file oyranos_gamma.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2010 (C) Kai-Uwe Behrmann
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


#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include "oyranos_texts.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }


int main( int argc , char** argv )
{
  char *display_name = getenv("DISPLAY");
  char *monitor_profile = 0;
  int error = 0;

  /* the functional switches */
  int erase = 0;
  int list = 0;
  int setup = 0;
  int database = 0;
  char * format = 0;
  char * output = 0;
  int server = 0;
  int net_color_region_target = 0;
  int device_meta_tag = 0;
  char * add_edid = 0,
       * prof_name = 0,
       * module_name = 0;
  char * device_class = 0;
  int list_modules = 0;

  char *ptr = NULL;
  int x = 0, y = 0;
  char *oy_display_name = NULL;
  oyProfile_s * prof = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  size_t size = 0;
  const char * filename = 0;
  char * data = 0;
  uint32_t n = 0;

  if(getenv("OYRANOS_DEBUG"))
  {
    int value = atoi(getenv("OYRANOS_DEBUG"));
    if(value > 0)
      oy_debug += value;
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

#ifndef __APPLE__
  if(!display_name)
  {
    WARNc_S( _("DISPLAY variable not set: giving up.") );
    error = 1;
    return error;
  }
#endif

  STRING_ADD( device_class, "monitor" );

  /* cut off the screen information */
  if(display_name &&
     (ptr = strchr(display_name,':')) != 0)
    if( (ptr = strchr(ptr, '.')) != 0 )
      ptr[0] = '\000';

/* allow "-opt val" and "-opt=val" syntax */
#define OY_PARSE_INT_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = atoi( argv[pos+1] ); \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = atoi( &argv[pos][i+2] ); \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) printf(#opt "=%d\n",opt)
#define OY_PARSE_STRING_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = &argv[pos][i+2]; \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) printf(#opt "=%s\n",opt)
#define OY_PARSE_STRING_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = &argv[pos][i+strlen(arg)+2]; \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) printf(arg "=%s\n",opt)
#define OY_IS_ARG( arg ) \
                        (strlen(argv[pos])-2 >= strlen(arg) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)

  if(argc != 1)
  {
    int pos = 1, i;
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
              case 'b': database = 1; monitor_profile = 0; break;
              case 'c': net_color_region_target = 1; monitor_profile = 0; break;
              case 'f': OY_PARSE_STRING_ARG(format); monitor_profile = 0; break;
              case 'l': list = 1; monitor_profile = 0; break;
              case 'm': device_meta_tag = 1; break;
              case 'o': OY_PARSE_STRING_ARG(output); monitor_profile = 0; break;
              case 'x': server = 1; OY_PARSE_INT_ARG( x ); break;
              case 'y': server = 1; OY_PARSE_INT_ARG( y ); break;
              case 'v': oy_debug += 1; break;
              case 's': setup = 1; break; /* implicite -> setup */
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("unset"))
                        { erase = 1; monitor_profile = 0; i=100; break; }
                        else if(strcmp(&argv[pos][2],"net_color_region_target") == 0)
                        { net_color_region_target = 1; i=100; break; }
                        else if(OY_IS_ARG("setup"))
                        { setup = 1; i=100; break; }
                        else if(OY_IS_ARG("format"))
                        { OY_PARSE_STRING_ARG2(format, "format"); break; }
                        else if(OY_IS_ARG("output"))
                        { OY_PARSE_STRING_ARG2(output, "output"); break; }
                        else if(OY_IS_ARG("database"))
                        { database = 1; monitor_profile = 0; i=100; break; }
                        else if(OY_IS_ARG("add-edid"))
                        { OY_PARSE_STRING_ARG2(add_edid,"add-edid"); break; }
                        else if(OY_IS_ARG("profile"))
                        { OY_PARSE_STRING_ARG2(prof_name, "profile"); break; }
                        else if(OY_IS_ARG("modules"))
                        { list_modules = 1; i=100; break; }
                        else if(OY_IS_ARG("module"))
                        { OY_PARSE_STRING_ARG2(module_name, "module"); break; }
                        else if(OY_IS_ARG("list"))
                        { list = 1; monitor_profile = 0; i=100; break; }
                        else if(OY_IS_ARG("verbose"))
                        { oy_debug += 1; i=100; break; }
                        }
              default:
                        printf("\n");
                        printf("oyranos-monitor v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a colour profile administration tool for monitors"));
                        printf("%s:\n",                 _("Usage"));
                        printf("  %s\n",               _("Set new profile:"));
                        printf("      %s -x pos -y pos %s\n", argv[0],
                                                       _("profile name"));
                        printf("\n");
                        printf("  %s\n",               _("Unset profile:"));
                        printf("      %s -e -x pos -y pos\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Activate profiles:"));
                        printf("      %s\n",           argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Query server profile:"));
                        printf("      %s -x pos -y pos\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Query device data base profile:"));
                        printf("      %s -b -x pos -y pos\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("List devices:"));
                        printf("      %s -l\n",        argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("List modules:"));
                        printf("      %s --modules\n",        argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("Dump data:"));
                        printf("      %s -f=[edid|icc|edid_icc] -o=edid.bin -x=pos -y=pos [-m]\n", argv[0]);
                        printf("\n");
                        printf("  %s\n",               _("General options:"));
                        printf("      %s\n",           _("-v verbose"));
                        printf("      %s\n",           _("--module name"));
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
            /* activate all profiles at once */
            /*error = oyActivateMonitorProfiles (display_name); */

            erase = 0;
      }
      if( wrong_arg )
      {
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) printf( "%s\n", argv[1] );

    if(!erase && !list && !database && !setup && !server && !format &&
       !add_edid && !list_modules)
      setup = 1;

    if(module_name)
    {
      STRING_ADD( device_class, ".");
      STRING_ADD( device_class, module_name);
    }

    oy_display_name = oyGetDisplayNameFromPosition2(OY_TYPE_STD, device_class,
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

    if(!monitor_profile && !erase && !list && !setup && !format && !add_edid)
    {
      char * fn = 0;
      error = oyDeviceGet( OY_TYPE_STD, device_class, oy_display_name, 0,
                           &device );
      if(net_color_region_target)
        error = oyOptions_SetFromText( &options,
                                       "//"OY_TYPE_STD"/config/icc_profile.net_color_region_target",
                                       "yes", OY_CREATE_NEW );

      if(database)
      {
        error = oyDeviceProfileFromDB( device, &fn, oyAllocFunc ); 
        prof = oyProfile_FromFile( fn, 0, 0 );
        filename = fn;
      } else {
        error = oyDeviceGetProfile( device, options, &prof );
        filename = oyProfile_GetFileName( prof, -1 );
      }
      data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
      if(size && data) oyDeAllocFunc( data ); data = 0;

      if(filename && strrchr(filename, '/') != 0)
        filename = strrchr(filename, '/') + 1;

      printf("%s:%d profile \"%s\" size: %d\n",
             strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,__LINE__,
             filename?filename:OY_PROFILE_NONE, (int)size);

      if(fn) oyDeAllocFunc( fn ); fn = 0;
      oyProfile_Release( &prof );
      oyConfig_Release( &device );
      oyOptions_Release( &options );
    }

    if(format &&
       (strcmp(format,"edid") == 0 ||
        strcmp(format,"icc") == 0 ||
        strcmp(format,"edid_icc") == 0))
    {
      oyConfigs_s * devices = 0;
      oyConfig_s * c = 0;
      icHeader * header = 0;
      oyOption_s * o = 0;
      char * out_name = 0;

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
      if(!error)
      {
        for(i = 0; i < n; ++i)
        {
          c = oyConfigs_Get( devices, i );

          if(strcmp(format,"edid_icc") == 0)
          {
            o = oyConfig_Find( c, "colour_matrix.from_edid."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");

            if(o)
            {
              oyOptions_s * opts = oyOptions_New(0),
                          * result = 0;

              error = oyOptions_MoveIn( opts, &o, -1 );
              oyOptions_Handle( "///create_profile.icc",
                                opts,"create_profile.icc_profile.colour_matrix",
                                &result );
              prof = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                        oyOBJECT_PROFILE_S );
              oyOptions_Release( &result );
            }

            if(prof)
            {
              uint32_t model_id = 0;
              error = oyProfile_AddTagText( prof, icSigProfileDescriptionTag,
                                            (char*) output ? output : format );
              error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag,
                                  oyConfig_FindString( c, "manufacturer", 0 ) );
              error = oyProfile_AddTagText( prof, icSigDeviceModelDescTag,
                                  oyConfig_FindString( c, "model", 0 ) );
              if(device_meta_tag)
                error = oyProfile_DeviceAdd( prof, c, 0 );
              data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
              header = (icHeader*) data;
              o = oyConfig_Find( c, "mnft" );
              sprintf( (char*)&header->manufacturer, "%s",
                       oyConfig_FindString( c, "mnft", 0 ) );
              oyOption_Release( &o );
              o = oyConfig_Find( c, "model_id" );
              if(o)
                model_id = atoi( oyConfig_FindString( c, "model_id", 0 ) );
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
            if(net_color_region_target)
            {
              /* get OY_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE */
              error = oyOptions_SetFromText( &cs_options,
              "//"OY_TYPE_STD"/config/icc_profile.net_color_region_target", "yes", OY_CREATE_NEW );
            }
            oyDeviceAskProfile2( c, cs_options, &prof );
            oyOptions_Release( &cs_options );
            if(device_meta_tag)
              oyProfile_DeviceAdd( prof, c, 0 );
            data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
          }

          if(data && size)
          {
            error = oyWriteMemToFile2_( output ? output : format,
                                        data, size, 0x01,
                                        &out_name, oyAllocFunc );
            oyDeAllocFunc( data ); size = 0;
          } else
            error = 1;

          if(!error)
          { if(oy_debug) printf( "  written to %s\n", out_name ); }
          else
            printf( "Could not write %d bytes to %s\n",
                    (int)size, out_name?out_name:format);
          if(out_name) oyDeAllocFunc(out_name); out_name = 0;

          oyProfile_Release( &prof );
          oyOption_Release( &o );
          oyConfig_Release( &c );
        }
      }
      oyConfigs_Release( &devices );
      oyOptions_Release( &options );

    } else
    if(prof_name && add_edid)
    {
      oyBlob_s * edid = oyBlob_New(0);
      data = oyReadFileToMem_( add_edid, &size, oyAllocateFunc_ );
      oyBlob_SetFromData( edid, data, size, "edid" );
      prof = oyProfile_FromFile( prof_name, 0, 0 );
      device = 0;
      oyOptions_Release( &options );
      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "add-edid-meta-to-icc", OY_CREATE_NEW );
      error = oyOptions_MoveInStruct( &options,
                                     "//" OY_TYPE_STD "/config/icc_profile",
                                      (oyStruct_s**)&prof, OY_CREATE_NEW );
      error = oyOptions_MoveInStruct( &options,
                                     "//" OY_TYPE_STD "/config/edid",
                                      (oyStruct_s**)&edid, OY_CREATE_NEW );
      error = oyDeviceGet( OY_TYPE_STD, device_class, ":0.0", options, &device);
      oyConfig_Release( &device );
      prof = (oyProfile_s*)oyOptions_GetType( options, -1, "icc_profile",
                                              oyOBJECT_PROFILE_S );
      oyOptions_Release( &options );
      oyProfile_ToFile_( prof, prof_name );
      oyProfile_Release( &prof );
    }

    if(list)
    {
      char * text = 0,
           * report = 0;
      uint32_t n = 0, i;
      oyConfig_s * c = 0;
      oyOption_s * o = 0;
      oyOptions_s * cs_options = 0;

      if(net_color_region_target)
      {
        /* get OY_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE */
        error = oyOptions_SetFromText( &cs_options,
              "//"OY_TYPE_STD"/config/icc_profile.net_color_region_target", "yes", OY_CREATE_NEW );
      }
      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
      error = oyDevicesGet( 0, device_class, options, &devices );


      n = oyConfigs_Count( devices );
      if(error <= 0)
      {
        for(i = 0; i < n; ++i)
        {
          c = oyConfigs_Get( devices, i );

          if(oy_debug)
          printf("------------------------ %d ---------------------------\n",i);

          error = oyDeviceGetInfo( c, oyNAME_NICK, cs_options, &text,
                                   oyAllocFunc );
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "\"%s\" ", text ? text : "???" );
          error = oyDeviceGetInfo( c, oyNAME_NAME, cs_options, &text,
                                   oyAllocFunc );
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "%s%s", text ? text : "???",
                                      i+1 == n ? "" : "\n" );
          if(oy_debug)
          {
            error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, cs_options, &text,
                                     oyAllocFunc );
            printf( "%s\n", text ? text : "???" );
          }

          if(text)
            free( text );

          /* verbose adds */
          if(oy_debug)
          {
            o = oyConfig_Find( c, "colour_matrix.from_edid" );
            text = oyOption_GetValueText( o, oyAllocFunc );
            printf( OY_DBG_FORMAT_" \"%s\":\n%s\n", OY_DBG_ARGS_,
                    o->registration, text?text:"----" );
            if(text) oyDeAllocFunc( text ); text = 0;
            oyOption_Release( &o );

            oyDeviceAskProfile2( c, cs_options, &prof );
            data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
            if(size && data)
              oyDeAllocFunc( data );
            filename = oyProfile_GetFileName( prof, -1 );
            printf( OY_DBG_FORMAT_" server profile \"%s\" size: %d\n",
                    OY_DBG_ARGS_,
                    filename?filename:OY_PROFILE_NONE, (int)size );

            oyDeviceProfileFromDB( c, &text, oyAllocFunc );
            printf( OY_DBG_FORMAT_
                    " DB profile \"%s\"\n  DB registration key set: %s\n",
                    OY_DBG_ARGS_,
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
      }
      oyConfigs_Release( &devices );
      oyOptions_Release( &cs_options );
    }

    /* make shure the display name is correct including the screen */
    {
      error = oyDeviceGet( OY_TYPE_STD, device_class, oy_display_name, 0,
                           &device );
      if(monitor_profile)
        oyDeviceSetProfile( device, monitor_profile );
      if(monitor_profile || erase)
        oyDeviceUnset( device );
      if(erase)
        oyConfig_EraseFromDB( device );

      oyConfig_Release( &device );
    }
  } else
    setup = 1;

  if(setup || monitor_profile)
  {
    int i = 0;

    /* 1. set a general request */
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "list", OY_CREATE_NEW );
    /* we want a fuzzy look at our display, not as narrow as "device_name"*/
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/display_name",
                                   display_name, OY_CREATE_NEW );
    error = oyConfigs_FromDeviceClass ( OY_TYPE_STD, device_class, options,
                                            &devices, 0 );

    n = oyConfigs_Count( devices );
    for(i = 0; i < n; ++i)
    {
      device = oyConfigs_Get( devices, i );

      oyDeviceSetup( device );

      oyConfig_Release( &device );
    }

    oyConfigs_Release( &devices );
    oyOptions_Release( &options );
  }

  if(oy_display_name)
    oyDeAllocFunc(oy_display_name);

  return error;
}
