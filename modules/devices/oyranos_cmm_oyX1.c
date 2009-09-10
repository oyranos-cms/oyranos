/** @file oyranos_cmm_oyX1.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/12/12
 */

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

#define CMM_NICK "oyX1"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/* OYX1_MONITOR_REGISTRATION */

int                oyX1CMMInit       ( );
int            oyX1CMMMessageFuncSet ( oyMessage_f         message_func );

/* OYX1_MONITOR_REGISTRATION -------------------------------------------------*/

#define OYX1_MONITOR_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.monitor." CMM_NICK

oyMessage_f message = 0;

extern oyCMMapi8_s oyX1_api8;
oyRankPad oyX1_rank_map[];

int          oyX1DeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device );
int            oyX1Configs_Modify    ( oyConfigs_s       * devices,
                                       oyOptions_s       * options );

/* --- implementations --- */

int                oyX1CMMInit       ( )
{
  int error = 0;
  return error;
}


/*
oyPointer          oyCMMallocateFunc   ( size_t            size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               oyCMMdeallocateFunc ( oyPointer         mem )
{
  if(mem)
    free(mem);
}*/

/** @func  oyX1CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX1CMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                     OYX1_MONITOR_REGISTRATION OY_SLASH #name, \
                                       name, OY_CREATE_NEW ); \
        if(name) oyDeAllocateFunc_( name ); name = 0;

void     oyX1ConfigsUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN shall make shure our message will be visible. */
    message( oyMSG_WARN, options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=list\" will provide a list of \n"
      " available devices. The actual device name can be found in option\n"
      " \"device_name\". The call is as lightwight as possible.\n"
      " The option \"display_name\" is optional to pass the X11 display name\n"
      " and obtain a unfiltered result. It the way to get all monitors\n"
      " connected to a display.\n"
      " The option \"oyNAME_NAME\" returns a string containting geometry and\n"
      " if available, the profile name or size.\n"
      " The bidirectional option \"device_rectangle\" will cause to\n"
      " additionally add display geometry information as a oyRectangle_s\n"
      " object.\n"
      " The bidirectional option \"icc_profile\" will always add a\n"
      " oyProfile_s being it filled or set to NULL to show it was not found.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n"
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n"
      " string.\n"
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The bidirectional optional \"edid\" (specific) key word will\n"
      " additionally add the EDID information inside a oyBlob_s struct.\n"
      " The option \"device_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION\n"
      " above.\n"
      " Informations are stored in the returned oyConfig_s::data member."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=properties\" will provide the devices\n"
      " properties. Requires one device identifier returned with the \n"
      " \"list\" option. The properties may cover following entries:\n"
      " - \"manufacturer\"\n"
      " - \"model\"\n"
      " - \"serial\"\n"
      " - \"host\"\n"
      " - \"system_port\"\n"
      " - \"display_geometry\" (specific) widthxheight+x+y ,e.g."
      " \"1024x786+0+0\"\n"
      " \n"
      " One option \"device_name\" will select the according X display.\n"
      " If not the module will try to get this information from \n"
      " your \"DISPLAY\" environment variable or uses what the system\n"
      " provides. The \"device_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one.\n"
      " Informations are stored in the returned oyConfig_s::backend_core member."
       );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=setup\" will setup the device from a\n"
      " profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"      " file name."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of call \"command=unset\" will invalidate a profile of\n"
      " a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );
#if 0
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"get\" will provide a oyProfile_s of the\n"
      " device in a \"icc_profile\" option.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );
#endif

  return;
}

int          oyX1DeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device )
{
  const char * value3 = 0;
  oyOption_s * o = 0;
  int error = !device;

    value3 = oyOptions_FindString( options, "edid", 0 );

    if(!error)
    {
      char * manufacturer=0, *model=0, *serial=0, *host=0, *display_geometry=0,
           * system_port=0;
      double colours[9];
      oyBlob_s * edid = 0;

      if(!device_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", OY_DBG_ARGS_ );
        error = 1;
      }

      if(error <= 0)
        error = oyGetMonitorInfo_lib( device_name,
                                      &manufacturer, &model, &serial,
                                      &display_geometry, &system_port,
                                      &host, colours,
                                      value3 ? &edid : 0,oyAllocateFunc_,
                                      (oyStruct_s*)options );

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  oyGetMonitorInfo_lib returned with %s; device_name:"
                 " \"%s\"", OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 oyNoEmptyString_m_( device_name ) );

      if(error <= 0)
      {
        if(!*device)
          *device = oyConfig_New( OYX1_MONITOR_REGISTRATION, 0 );
        error = !*device;
        if(!error && device_name)
        error = oyOptions_SetFromText( &(*device)->backend_core,
                                       OYX1_MONITOR_REGISTRATION OY_SLASH "device_name",
                                       device_name, OY_CREATE_NEW );

        OPTIONS_ADD( (*device)->backend_core, manufacturer )
        OPTIONS_ADD( (*device)->backend_core, model )
        OPTIONS_ADD( (*device)->backend_core, serial )
        OPTIONS_ADD( (*device)->backend_core, display_geometry )
        OPTIONS_ADD( (*device)->backend_core, system_port )
        OPTIONS_ADD( (*device)->backend_core, host )
        if(!error)
        {
          int i;
          for(i = 0; i < 9; ++i)
            error = oyOptions_SetFromDouble( &(*device)->data,
                             OYX1_MONITOR_REGISTRATION OY_SLASH "edid1_colours",
                                             colours[i], i, OY_CREATE_NEW );
        }

        if(!error && edid)
        {
          int has = 0;
          o = oyConfig_Find( *device,
                             OYX1_MONITOR_REGISTRATION OY_SLASH "edid" );
          if(o)
            has = 1;
          else
            o = oyOption_New( OYX1_MONITOR_REGISTRATION OY_SLASH "edid", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, edid->ptr, edid->size );
          if(!error)
          {
            if(has)
              oyOption_Release( &o );
            else
              oyOptions_MoveIn( (*device)->data, &o, -1 );
          }
          oyBlob_Release( &edid );
        }
        if(edid) oyDeAllocateFunc_( edid ); edid = 0;
      }
    }

  return error;
}


/** Function oyX1Configs_FromPattern
 *  @brief   oyX1 oyCMMapi8_s Xorg monitors
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/28
 */
int            oyX1Configs_FromPattern (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
  oyConfigs_s * devices = 0;
  oyConfig_s * device = 0;
  char ** texts = 0;
  char * text = 0,
       * device_name_temp = 0;
  int texts_n = 0, i,
      error = !s;
  const char * odevice_name = 0,
             * oprofile_name = 0,
             * odisplay_name = 0,
             * device_name = 0;
  int rank = oyFilterRegistrationMatch( oyX1_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  const char * tmp = 0;


  /** 1. In case no option is provided or something fails, show a message. */
  if(!options || !oyOptions_Count( options ))
  {
    oyX1ConfigsUsage( (oyStruct_s*)options );
    return 0;
  }

  if(rank && error <= 0)
  {
    devices = oyConfigs_New(0);


    /** 2. obtain a proper device_name */
    odisplay_name = oyOptions_FindString( options, "display_name", 0 );
    odevice_name = oyOptions_FindString( options, "device_name", 0 );
    /*message(oyMSG_WARN, (oyStruct_s*)options, "list: %s", value2);*/

    if(odisplay_name && odisplay_name[0])
      device_name = odisplay_name;
    else if(odevice_name && odevice_name[0])
      device_name = odevice_name;
    else
    {
      tmp = getenv("DISPLAY");
#if !defined(__APPLE__)
      if(!tmp)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "DISPLAY variable not set: giving up\n. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
        error = 1;
        return error;
      }

      device_name_temp = oyStringCopy_( tmp, oyAllocateFunc_ );
      if(device_name_temp &&
         (text = strchr(device_name_temp,':')) != 0)
        if( (text = strchr(device_name_temp, '.')) != 0 )
          text[0] = '\000';

      device_name = device_name_temp;
#endif
      text = 0;
    }

    /** 3.  handle the actual call */
    /** 3.1 "list" call */
    if(oyOptions_FindString( options, "command", "list" ) ||
       oyOptions_FindString( options, "command", "properties" ))
    {
      texts_n = oyGetAllScreenNames( device_name, &texts, allocateFunc );

      /** 3.1.1 iterate over all requested devices */
      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odevice_name && strcmp(odevice_name, texts[i]) != 0)
          continue;

        device = oyConfig_New( OYX1_MONITOR_REGISTRATION, 0 );
        error = !device;

         /** 3.1.2 tell the "device_name" */
        if(error <= 0)
        error = oyOptions_SetFromText( &device->backend_core,
                                       OYX1_MONITOR_REGISTRATION OY_SLASH
                                       "device_name",
                                       texts[i], OY_CREATE_NEW );

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        error = oyX1Configs_Modify( devices, options );

      if(error <= 0)
        *s = devices;

      oyStringListRelease_( &texts, texts_n, free );

      goto cleanup;

    } else

    /** 3.3 "setup" call; bring a profile to the device */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "setup" ))
    {
      oprofile_name = oyOptions_FindString( options, "profile_name", 0 );
      error = !odevice_name || !oprofile_name;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "The device_name/profile_name option is missed. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX1MonitorProfileSetup( odevice_name, oprofile_name );

      goto cleanup;

    } else

    /** 3.4 "unset" call; clear a profile from a device */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "unset" ))
    {
      error = !odevice_name;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "The device_name option is missed. Options:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX1MonitorProfileUnset( odevice_name );

      goto cleanup;
    }

    /** 3.5 "help" call; display a help text */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "help" ))
    {
      oyX1ConfigsUsage( (oyStruct_s*)options );

      goto cleanup;
    }
  }


  message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  oyX1ConfigsUsage( (oyStruct_s*)options );


  cleanup:
  if(device_name_temp)
    oyFree_m_( device_name_temp );


  return error;
}

/** Function oyX1Configs_Modify
 *  @brief   oyX1 oyCMMapi8_s Xorg monitor manipulation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
int            oyX1Configs_Modify    ( oyConfigs_s       * devices,
                                       oyOptions_s       * options )
{
  oyConfig_s * device = 0;
  oyOption_s * o = 0;
  oyRectangle_s * rect = 0;
  const oyRectangle_s * r = 0;
  oyProfile_s * p = 0;
  char * text = 0;
  int n = 0, i,
      error = !devices || !oyConfigs_Count( devices ),
      has;
  const char * oprofile_name = 0,
             * device_name = 0;
  int rank = 0;
  oyAlloc_f allocateFunc = malloc;
  const char * tmp = 0;


  /** 1. In case no option is provided or something fails, show a message. */
  if(!options || !oyOptions_Count( options ))
  {
    oyX1ConfigsUsage( (oyStruct_s*)options );
    return 0;
  }

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    rank += oyFilterRegistrationMatch( oyX1_api8.registration,
                                       device->registration,
                                       oyOBJECT_CMM_API8_S );
    oyConfig_Release( &device );
  }

  if(rank && error <= 0)
  {
    /** 3.  handle the actual call */
    /** 3.1 "list" call */
    if(oyOptions_FindString( options, "command", "list" ))
    {
      n = oyConfigs_Count( devices );

      /** 3.1.1 iterate over all provided devices */
      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( oyX1_api8.registration,
                                          device->registration,
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        /** 3.1.2 get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );

        /** 3.1.3 tell the "device_rectangle" in a oyRectangle_s */
        if(oyOptions_FindString( options, "device_rectangle", 0 ) ||
           oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          has = 0;
          rect = oyX1Rectangle_FromDevice( device_name );
          if(!rect)
          {
            WARNc1_S( "Could not obtain rectangle information for %s",
                      device_name );
          } else
          {
            o = oyConfig_Find( device, "device_rectangle" );
            if(o)
              has = 1;
            else
              o = oyOption_New( OYX1_MONITOR_REGISTRATION OY_SLASH
                                "device_rectangle", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &rect );
            if(has)
              oyOption_Release( &o );
            else
              oyOptions_MoveIn( device->data, &o, -1 );
          }
        }

        /** 3.1.4 tell the "icc_profile" in a oyProfile_s */
        if( oyOptions_FindString( options, "icc_profile", 0 ) ||
            oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 0;
          char * data = oyX1GetMonitorProfile( device_name, &size,
                                               allocateFunc );

          
          has = 0;
          o = oyConfig_Find( device, "icc_profile" );
          if(o)
            has = 1;
          else
            o = oyOption_New( OYX1_MONITOR_REGISTRATION OY_SLASH "icc_profile",
                              0 );
          /** Warn and return issue on not found profile. */
          if(!size || !data)
          {
            message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Could not obtain _ICC_PROFILE(_xxx) information for %s",
                     OY_DBG_ARGS_, device_name );
            /* Show the "icc_profile" option is understood. */
            p = 0;
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            error = -1;
          } else
          {
            p = oyProfile_FromMem( size, data, 0, 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            free( data );
          }
          if(has)
            oyOption_Release( &o );
          else
            oyOptions_MoveIn( device->data, &o, -1 );
        }

        /** 3.1.5 construct a oyNAME_NAME string */
        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          o = oyOptions_Find( device->data, "device_rectangle" );
          r = (oyRectangle_s*) o->value->oy_struct;

          text = 0; tmp = 0;
      
          tmp = oyRectangle_Show( (oyRectangle_s*)r );
          STRING_ADD( text, tmp );
          oyOption_Release( &o );

          o = oyOptions_Find( device->data, "icc_profile" );

          if( o && o->value && o->value->oy_struct && 
              o->value->oy_struct->type_ == oyOBJECT_PROFILE_S)
          {
            p = oyProfile_Copy( (oyProfile_s*) o->value->oy_struct, 0 );
            tmp = oyProfile_GetFileName( p, 0 );

            STRING_ADD( text, "  " );
            if(tmp)
            {
              if(oyStrrchr_( tmp, OY_SLASH_C ))
                STRING_ADD( text, oyStrrchr_( tmp, OY_SLASH_C ) + 1 );
              else
                STRING_ADD( text, tmp );
            }

            oyProfile_Release( &p );
          }

          if(error <= 0)
          error = oyOptions_SetFromText( &device->data,
                                         OYX1_MONITOR_REGISTRATION OY_SLASH
                                         "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          oyFree_m_( text );
        }


        /** 3.1.6 add the rank scheme to combine properties */
        if(error <= 0 && !device->rank_map)
          device->rank_map = oyRankMapCopy( oyX1_rank_map,
                                            device->oy_->allocateFunc_ );

        oyConfig_Release( &device );
      }

      goto cleanup;

    } else


    /** 3.2 "properties" call; provide extensive infos for the DB entry */
    if(oyOptions_FindString( options, "command", "properties" ))
    {
      n = oyConfigs_Count( devices );

      /** 3.1.1 iterate over all provided devices */
      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( oyX1_api8.registration,
                                          device->registration,
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        /** 3.1.2 get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );

        /** 3.2.1 add properties */
        error = oyX1DeviceFromName_( device_name, options, &device );


        /** 3.2.2 add the rank map to wight properties for ranking in the DB */
        if(error <= 0 && device && !device->rank_map)
          device->rank_map = oyRankMapCopy( oyX1_rank_map,
                                            device->oy_->allocateFunc_);
        oyConfig_Release( &device );
      }

      goto cleanup;

    } else

    /** 3.3 "setup" call; bring a profile to the device */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "setup" ))
    {
      n = oyConfigs_Count( devices );

      /** 3.1.1 iterate over all provided devices */
      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( oyX1_api8.registration,
                                          device->registration,
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        /** 3.1.2 get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );
        oprofile_name = oyOptions_FindString( options, "profile_name", 0 );

        error = !device_name || !oprofile_name;
        if(error >= 1)
          message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "The device_name/profile_name option is missed. Options:\n%s",
                  OY_DBG_ARGS_,
                  oyOptions_GetText( options, oyNAME_NICK )
                  );
        else
          error = oyX1MonitorProfileSetup( device_name, oprofile_name );

        oyConfig_Release( &device );
      }

      goto cleanup;

    } else

    /** 3.4 "unset" call; clear a profile from a device */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "unset" ))
    {
      n = oyConfigs_Count( devices );

      /** 3.1.1 iterate over all provided devices */
      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( oyX1_api8.registration,
                                          device->registration,
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        /** 3.1.2 get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );
        oprofile_name = oyOptions_FindString( options, "profile_name", 0 );

        error = !device_name || !oprofile_name;
        if(error >= 1)
          message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "The device_name option is missed. Options:\n%s",
                  OY_DBG_ARGS_,
                  oyOptions_GetText( options, oyNAME_NICK )
                  );
        else
          error = oyX1MonitorProfileUnset( device_name );

        oyConfig_Release( &device );
      }

      goto cleanup;
    }

    /** 3.5 "help" call; display a help text */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "help" ))
    {
      oyX1ConfigsUsage( (oyStruct_s*)options );

      goto cleanup;
    }
  }


  message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  oyX1ConfigsUsage( (oyStruct_s*)options );


  cleanup:

  return error;
}

/** Function oyX1Config_Rank
 *  @brief   oyX1 oyCMMapi8_s Xorg monitor check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/26
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
int            oyX1Config_Rank       ( oyConfig_s        * config )
{
  int error = !config,
      rank = 1;

  if(!config)
  {
    message(oyMSG_DBG, (oyStruct_s*)config, OY_DBG_FORMAT_ "\n "
                "No config argument provided.\n", OY_DBG_ARGS_ );
    return 0;
  }

  if(error <= 0)
  {
    /* evaluate a driver specific part of the options */
  }

  return rank;
}

/** @instance oyX1_rank_map
 *  @brief    oyRankPad map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/27
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 */
oyRankPad oyX1_rank_map[] = {
  {"device_name", 2, -1, 0},           /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {"display_geometry", 3, -1, 0},      /**< important to match */
  {0,0,0,0}                            /**< end of list */
};

/** @instance oyX1_api8
 *  @brief    oyX1 oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
oyCMMapi8_s oyX1_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  (oyCMMapi_s*) 0, /**< next */

  oyX1CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  oyX1CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */

  OYX1_MONITOR_REGISTRATION, /**< registration */
  {0,3,0},                   /**< int32_t version[3] */
  {0,1,10},                  /**< int32_t module_api[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  oyX1Configs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  oyX1Configs_Modify,        /**< oyConfigs_Modify_f oyConfigs_Modify */
  oyX1Config_Rank,           /**< oyConfig_Rank_f oyConfig_Rank */
  oyX1_rank_map              /**< oyRankPad ** rank_map */
};

/* OYX1_MONITOR_REGISTRATION -------------------------------------------------*/


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * oyX1GetText             ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos X11");
    else
      return _("The window support module of Oyranos.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Kai-Uwe");
    else if(type == oyNAME_NAME)
      return _("Kai-Uwe Behrmann");
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("newBSD");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2009 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}
const char *oyX1_texts[4] = {"name","copyright","manufacturer",0};

/** @instance oyX1_cmm_module
 *  @brief    oyX1 module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMMInfo_s oyX1_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.3",
  oyX1GetText, /* oyCMMInfoGetText_f get Text */
  (char**)oyX1_texts, /* texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyX1_api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

