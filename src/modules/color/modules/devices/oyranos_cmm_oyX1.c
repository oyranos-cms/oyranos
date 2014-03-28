/** @file oyranos_cmm_oyX1.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2007-2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/12/12
 */
#include "oyCMM_s.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyCMMui_s_.h"

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_internal_x11.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <X11/Xcm/Xcm.h>
#include <X11/Xcm/XcmEvents.h>

/* --- internal definitions --- */

#define CMM_NICK "oyX1"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/* OYX1_MONITOR_REGISTRATION */

int                oyX1CMMInit       ( );
int            oyX1CMMMessageFuncSet ( oyMessage_f         message_func );

/* OYX1_MONITOR_REGISTRATION -------------------------------------------------*/

#define OYX1_MONITOR_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.device.icc_profile.monitor." CMM_NICK

oyMessage_f oyX1_msg = 0;

extern oyCMMapi8_s_ oyX1_api8;
extern oyCMMapi10_s_    oyX1_api10_set_xcm_region_handler;
oyRankMap oyX1_rank_map[];

int          oyX1DeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device );
int            oyX1Configs_Modify    ( oyConfigs_s       * devices,
                                       oyOptions_s       * options );
const char * oyX1GetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * oyX1Api8UiGetText       ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

/* --- implementations --- */

int                oyX1CMMInit       ( oyStruct_s        * filter )
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
  oyX1_msg = message_func;
  return 0;
}

const char * oyX1_help_list = 
      "The presence of option \"command=list\" will provide a list of \n"
      " available devices. The actual device name can be found in option\n"
      " \"device_name\". The call is as lightwight as possible.\n"
      " The option \"display_name\" is optional to pass the X11 display name\n"
      " and obtain a unfiltered result. Its the way to get all monitors\n"
      " connected to a display.\n"
      " The option \"oyNAME_NAME\" returns a string containting geometry and\n"
      " if available, the profile name or size.\n"
      " The bidirectional option \"device_rectangle\" will cause to\n"
      " additionally add display geometry information as a oyRectangle_s\n"
      " object.\n"
      " The bidirectional option \"icc_profile\" will always add in the\n"
      " devices \"data\" section a\n"
      " oyProfile_s being filled or set to NULL to show it was not found.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n"
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n"
      " string.\n"
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The bidirectional optional \"edid\" (specific) key word will\n"
      " additionally add the EDID information inside a oyBlob_s struct.\n"
      " A option \"edid\" key word with value \"refresh\" will \n"
      " update the EDID atom if possible.\n"
      " The option \"device_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION\n"
      " above.\n"
      " Informations are stored in the returned oyConfig_s::data member.";
const char * oyX1_help_properties =
      "The presence of option \"command=properties\" will provide the devices\n"
      " properties. Requires one device identifier returned with the \n"
      " \"list\" option. The properties may cover following entries:\n"
      " - \"EDID_manufacturer\" description\n"
      " - \"manufacturer\" duplicate of the previous key\n"
      " - \"EDID_mnft\" (decoded EDID_mnft_id)\n"
      " - \"EDID_model\" textual name\n"
      " - \"model\" duplicate of the previous key\n"
      " - \"EDID_serial\" not always present\n"
      " - \"serial\" duplicate of the previous key\n"
      " - \"host\" not always present\n"
      " - \"system_port\"\n"
      " - \"EDID_date\" manufacture date\n"
      " - \"EDID_mnft_id\" manufacturer ID (undecoded EDID_mnft)\n"
      " - \"EDID_model_id\" model ID\n"
      " - \"display_geometry\" (specific) widthxheight+x+y ,e.g."
      " \"1024x786+0+0\"\n"
      " - \"EDID_red_x\" \"EDID_red_y\" \"EDID_green_x\" \"EDID_green_y\" "
      "   \"EDID_blue_x\" \"EDID_blue_x\" \"EDID_white_x\" \"EDID_white_x\" "
      "   \"EDID_gamma\","
      " color characteristics as found in EDID as text\n"
      " - \"color_matrix.from_edid."
                   "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\","
      " color characteristics as found in EDID as doubles\n"
      " \n"
      " One option \"device_name\" will select the according X display.\n"
      " If not the module will try to get this information from \n"
      " your \"DISPLAY\" environment variable or uses what the system\n"
      " provides. The \"device_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one.\n"
      " Informations are stored in the returned oyConfig_s::backend_core member."
;
const char * oyX1_help_setup =
      "The presence of option \"command=setup\" will setup the device from a\n"
      " profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"      " file name."
;
const char * oyX1_help_unset =
      "The presence of call \"command=unset\" will invalidate a profile of\n"
      " a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
;
const char * oyX1_help_add_edid_to_icc =
      "The presence of option \"command=add_meta\" will embedd device\n"
      " informations from a provided EDID block to a provided ICC profile.\n"
      " The option \"edid\" must be present and contain an\n"
      " oyBlob_s object with the valid EDID data block.\n"
      " The bidirectional option \"icc_profile\" options must be present,\n"
      " containing a oyProfile_s object.\n"
;
const char * oyX1_help =
      "The following help text informs about the communication protocol."
;

void     oyX1ConfigsUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN shall make shure our message will be visible. */
    oyX1_msg( oyMSG_WARN, options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_, oyX1_help );
    oyX1_msg( oyMSG_WARN, options, "%s()\n %s", __func__, oyX1_help_list );
    oyX1_msg( oyMSG_WARN, options, "%s()\n %s", __func__, oyX1_help_properties );
    oyX1_msg( oyMSG_WARN, options, "%s()\n %s", __func__, oyX1_help_setup );
    oyX1_msg( oyMSG_WARN, options, "%s()\n %s", __func__, oyX1_help_unset );
    oyX1_msg( oyMSG_WARN, options, "%s()\n %s", __func__, oyX1_help_add_edid_to_icc );
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
  int error = !device;
  oyOption_s * o = 0;

    value3 = oyOptions_FindString( options, "edid", 0 );

    if(!error)
    {
      char * EDID_manufacturer=0, * EDID_mnft=0, * EDID_model=0,
           * EDID_serial=0, * EDID_vendor = 0,
           * host=0, * display_geometry=0, * system_port=0;
      double colors[9] = {0,0,0,0,0,0,0,0,0};
      oyBlob_s * edid = 0;
      uint32_t week=0, year=0, EDID_mnft_id=0, EDID_model_id=0;

      if(!device_name)
      {
        oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", OY_DBG_ARGS_ );
        error = 1;
      }

      if(error <= 0)
        error = oyX1GetMonitorInfo_lib( device_name,
                      &EDID_manufacturer, &EDID_mnft, &EDID_model, &EDID_serial,
                      &EDID_vendor, &display_geometry, &system_port, &host,
                      &week, &year, &EDID_mnft_id, &EDID_model_id,
                                        colors,
                                        &edid, oyAllocateFunc_,
                                        (oyStruct_s*)options );

      if(error != 0)
        oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, 
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  oyX1GetMonitorInfo_lib returned with %s; device_name:"
                 " \"%s\"", OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 oyNoEmptyString_m_( device_name ) );

      if(error <= 0 && edid)
      {
        error = oyDeviceFillEdid(   OYX1_MONITOR_REGISTRATION,
                                    device, oyBlob_GetPointer(edid),
                                    oyBlob_GetSize(edid),
                                    device_name,
                                    host, display_geometry, system_port,
                                    options );
      } else if(error <= 0)
      {
        if(!error && device_name)
        error = oyOptions_SetFromText( oyConfig_GetOptions(*device,"backend_core"),
                                       OYX1_MONITOR_REGISTRATION OY_SLASH "device_name",
                                       device_name, OY_CREATE_NEW );

        error = oyDeviceFillInfos( OYX1_MONITOR_REGISTRATION, device,
                                   device_name, host, display_geometry,
                                   system_port,
                                   EDID_manufacturer, EDID_mnft, EDID_model,
                                   EDID_serial, EDID_vendor,
                                   week, year,
                                   EDID_mnft_id, EDID_model_id,
                                   colors, options );
      }

      if(error != 0)
        oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  oyX1GetMonitorInfo_lib returned with %s %d; device_name:"
                 " \"%s\"",OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 error, oyNoEmptyString_m_( device_name ) );

      if(value3)
      {
        if(!error && edid)
        {
          int has = 0;
          o = oyConfig_Find( *device,
                             OYX1_MONITOR_REGISTRATION OY_SLASH "edid" );
          if(o)
            has = 1;
          else
            o = oyOption_FromRegistration(
                                OYX1_MONITOR_REGISTRATION OY_SLASH "edid", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, oyBlob_GetPointer(edid),
                                        oyBlob_GetSize( edid ) );
          if(!error)
          {
            if(has)
              oyOption_Release( &o );
            else
              oyOptions_MoveIn( *oyConfig_GetOptions(*device,"data"), &o, -1 );
          }
        }
      }

      oyBlob_Release( &edid );
      if(EDID_manufacturer) oyFree_m_(EDID_manufacturer);
      if(EDID_mnft) oyFree_m_(EDID_mnft);
      if(EDID_model) oyFree_m_(EDID_model);
      if(EDID_serial) oyFree_m_(EDID_serial);
      if(EDID_vendor) oyFree_m_(EDID_vendor);
      if(host) oyFree_m_(host);
      if(display_geometry) oyFree_m_(display_geometry);
      if(system_port) oyFree_m_(system_port);
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
        oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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
      texts_n = oyX1GetAllScreenNames( device_name, &texts, allocateFunc );

      /** 3.1.1 iterate over all requested devices */
      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odevice_name && strcmp(odevice_name, texts[i]) != 0)
          continue;

        device = oyConfig_FromRegistration( OYX1_MONITOR_REGISTRATION, 0 );
        error = !device;

         /** 3.1.2 tell the "device_name" */
        if(error <= 0)
        error = oyOptions_SetFromText( oyConfig_GetOptions(device,"backend_core"),
                                       OYX1_MONITOR_REGISTRATION OY_SLASH
                                       "device_name",
                                       texts[i], OY_CREATE_NEW );

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
      {
        if(devices && oyConfigs_Count(devices))
          error = oyX1Configs_Modify( devices, options );
        else if(oy_debug)
          oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "No monitor devices found.\n Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      }

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
        oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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
        oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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

    /** 3.6 internal "add_meta" call; Embedd infos to ICC profile 
     *      as meta tag. Might be moved to a oyCMMapi10_s object. */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "add_meta" ))
    {
      oyProfile_s * prof;
      oyBlob_s * edid;
      oyConfig_s * device = 0;

      prof = (oyProfile_s*)oyOptions_GetType( options, -1, "icc_profile",
                                        oyOBJECT_PROFILE_S );
      
      edid = (oyBlob_s*)oyOptions_GetType( options, -1, "edid",
                                        oyOBJECT_BLOB_S );

      if(!prof || !edid)
        error = 1;
      if(error >= 1)
      {
        oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "\"edid\" or \"icc_profile\" missed:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
        oyX1ConfigsUsage( (oyStruct_s*)options );
      }
      else
      {
        oyOptions_s * opts = 0;
        error = oyDeviceFillEdid(   OYX1_MONITOR_REGISTRATION,
                                    &device, oyBlob_GetPointer(edid),
                                    oyBlob_GetSize( edid ),
                                    NULL,
                                    NULL, NULL, NULL,
                                    options );
        if(error <= 0)
        {
          error = oyOptions_SetFromText( &opts, "///set_device_attributes",
                                                "true", OY_CREATE_NEW );
          error = oyOptions_SetFromText( &opts, "///key_prefix_required",
                                                "EDID_.prefix", OY_CREATE_NEW );
        }
        oyProfile_AddDevice( prof, device, opts );

        error = oyOptions_SetFromText( oyConfig_GetOptions(device,"backend_core"),
                                       OYX1_MONITOR_REGISTRATION OY_SLASH
                                       "device_name",
                                       device_name, OY_CREATE_NEW );
        if(error <= 0 && !oyConfig_GetRankMap(device))
          oyConfig_SetRankMap(device, oyX1_rank_map );
        oyConfigs_MoveIn( devices, &device, -1 );
        if(error <= 0)
          *s = devices;

        oyOptions_Release( &opts );
      }

      goto cleanup;
    }
  }


  oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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
  oyOption_s * o = 0, * o_tmp = 0;
  oyRectangle_s * rect = 0;
  const oyRectangle_s * r = 0;
  oyProfile_s * prof = 0;
  char * text = 0;
  int n = 0, i,
      error = !devices || !oyConfigs_Count( devices ),
      t_err = 0,
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
                                       oyConfig_GetRegistration(device),
                                       oyOBJECT_CMM_API8_S );
    oyConfig_Release( &device );
  }

  if(rank && error <= 0)
  {
    /** 3.  handle the actual call */
    /** 3.1 "list" call */
    if(oyOptions_FindString( options, "command", "list" ) ||
       oyOptions_FindString( options, "command", "properties" ))
    {
      n = oyConfigs_Count( devices );

      /** 3.1.1 iterate over all provided devices */
      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( oyX1_api8.registration,
                                          oyConfig_GetRegistration(device),
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
              o = oyOption_FromRegistration( OYX1_MONITOR_REGISTRATION OY_SLASH
                                "device_rectangle", 0 );
            error = oyOption_MoveInStruct( o, (oyStruct_s**) &rect );
            if(has)
              oyOption_Release( &o );
            else
              oyOptions_MoveIn( *oyConfig_GetOptions(device,"data"), &o, -1 );
          }
        }

        /** 3.1.4 tell the "icc_profile" in a oyProfile_s */
        if( oyOptions_FindString( options, "icc_profile", 0 ) ||
            oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 0;
          uint32_t flags = 0;
          char * data = 0;
          oyProfile_s * p = 0;

          if(oyOptions_FindString( options, "x_color_region_target", 0 ))
          {
            if(oy_debug)
              oyX1_msg( oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Try %s(_xxx) from %s",
                     OY_DBG_ARGS_,
                     oyOptions_FindString(options, "x_color_region_target", 0) ? 
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
                     XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE :
                     XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE,
#else
                     "no XCM_X11 device":"no XCM_X11",
#endif
                     device_name );
            flags |= 0x01;
          }

          has = 0;
          o = oyConfig_Find( device, "icc_profile" );
          if(o)
          {
            /* the device might have assigned a dummy icc_profile, to show 
             * it can handle the format. But thats not relevant here. */
            p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
            if(oyProfile_GetSignature( p, oySIGNATURE_MAGIC ) == icMagicNumber)
              has = 1;
            else
              oyOption_Release( &o );

            oyProfile_Release( &p );
          }

          if(oyOptions_FindString( options, "icc_profile.fallback", 0 ))
          {
            icHeader * header = 0;
            /* fallback: try to get EDID to build a profile */
            o_tmp = oyConfig_Find( device, "color_matrix."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");
            {
              oyOptions_SetFromText( &options,
                                     OYX1_MONITOR_REGISTRATION OY_SLASH
                                     "edid",
                                     "yes", OY_CREATE_NEW );
              error = oyX1DeviceFromName_( device_name, options, &device );
            }
            if(!o_tmp)
              o_tmp = oyConfig_Find( device, "color_matrix."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");

            if(o_tmp)
            {
              oyOptions_s * opts = oyOptions_New(0),
                          * result = 0;
              error = oyOptions_MoveIn( opts, &o_tmp, -1 );
              oyOptions_Handle( "//"OY_TYPE_STD"/create_profile.icc",
                                opts,"create_profile.icc_profile.color_matrix",
                                &result );
              prof = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                        oyOBJECT_PROFILE_S );
              oyOptions_Release( &result );
              oyOptions_Release( &opts );
            }

            if(prof)
            {
              const char * t = 0;
              oyOption_s * edid = 0;
              oyOptions_s * opts = NULL;

              if((t = oyConfig_FindString( device, "manufacturer", 0 )) != 0)
              {
                STRING_ADD( text, t );
                STRING_ADD( text, " " );
              } else
              {
                t = oyConfig_FindString( device, "EDID_model_id", 0 );
                if(t)
                  STRING_ADD( text, t );
                else
                oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                "Could not obtain \"EDID_model\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              }
              if((t = oyConfig_FindString( device, "model", 0 )) != 0)
              {
                STRING_ADD( text, t );
                STRING_ADD( text, " " );
              }
              if((t = oyConfig_FindString( device, "serial", 0 )) != 0)
                STRING_ADD( text, t );
              else
              if((t = oyConfig_FindString( device, "model", 0 )) != 0)
              {
                if((t = oyConfig_FindString( device, "EDID_year", 0 )) != 0)
                  STRING_ADD( text, t );
                if((t = oyConfig_FindString( device, "EDID_week", 0 )) != 0)
                  STRING_ADD( text, t );
              }

              edid = oyConfig_Find( device, "edid" );
              if(edid)
                STRING_ADD( text, "_edid" );
              else
                STRING_ADD( text, "_xorg" );
              oyOption_Release( &edid );

              error = oyProfile_AddTagText( prof,
                                            icSigProfileDescriptionTag, text);
              oyDeAllocateFunc_( text ); text = 0;
              t = oyConfig_FindString( device, "EDID_manufacturer", 0);
              if(!t)
              {
                t = oyConfig_FindString( device, "EDID_mnft", 0);
                if(!t)
                  error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t);
                else
                oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
            "Could not obtain \"EDID_manufacturer\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              } else
                error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t);
              t = oyConfig_FindString( device, "EDID_model", 0 );
              if(!t)
                oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                   "Could not obtain \"EDID_model\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              else
                error = oyProfile_AddTagText( prof, icSigDeviceModelDescTag, t);

              error = oyOptions_SetFromText( oyConfig_GetOptions(device,"backend_core"),
                                       OYX1_MONITOR_REGISTRATION OY_SLASH
                                       "OYRANOS_automatic_generated",
                                       "1", OY_CREATE_NEW );

              /* embed meta tag */
              error = oyOptions_SetFromText( &opts, "///key_prefix_required",
                                                "EDID_.OYRANOS_",
                                                OY_CREATE_NEW );
              oyProfile_AddDevice( prof, device, opts );
              oyOptions_Release( &opts);

              data = oyProfile_GetMem( prof, &size, 0, oyAllocateFunc_ );
              header = (icHeader*) data;
              o_tmp = oyConfig_Find( device, "EDID_mnft" );
              t = oyConfig_FindString( device, "EDID_mnft", 0 );
              if(!t)
                oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                    "Could not obtain \"EDID_mnft\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              else
                sprintf( (char*)&header->manufacturer, "%s", t );
            }
            oyOption_Release( &o_tmp );
            oyProfile_Release( &prof );
            if(data && size)
            {
              prof = oyProfile_FromMem( size, data, 0, 0 );
              oyDeAllocateFunc_( data ); data = NULL; size = 0;
            }

            if(!has)
              o = oyOption_FromRegistration( OYX1_MONITOR_REGISTRATION OY_SLASH
                                "icc_profile.fallback", 0 );
            error = -1;
          } else
          {
            data = oyX1GetMonitorProfile( device_name, flags, &size,
                                          allocateFunc );

            if(data && size)
            {
              prof = oyProfile_FromMem( size, data, 0, 0 );
              free( data );
              if(has == 0)
              {
                const char * key = OYX1_MONITOR_REGISTRATION OY_SLASH "icc_profile";
                if(oyOptions_FindString(options, "x_color_region_target", 0))
                  key = OYX1_MONITOR_REGISTRATION OY_SLASH "icc_profile.x_color_region_target";
                o = oyOption_FromRegistration( key, 0 );
              }
            }
          }

          if(!o)
              o = oyOption_FromRegistration( OYX1_MONITOR_REGISTRATION OY_SLASH
                                "icc_profile", 0 );

          if(prof)
          {
            t_err = oyOption_MoveInStruct( o, (oyStruct_s**) &prof );
            if(t_err > 0)
              error = t_err;
          }
          else
          /** Warn and return issue on not found profile. */
          {
            oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Could not obtain %s(_xxx) information for %s",
                     OY_DBG_ARGS_,
                     oyOptions_FindString(options, "x_color_region_target", 0) ? 
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
                     XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE :
                     XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE,
#else
                     "no XCM_X11 device":"no XCM_X11",
#endif
                     device_name );

            /* Show the "icc_profile" option is understood. */
            prof = 0;
            error = oyOption_MoveInStruct( o, (oyStruct_s**) &prof );
            error = -1;
          }

          if(!has)
            oyOptions_Set( *oyConfig_GetOptions(device,"data"), o, -1, 0 );

          oyOption_Release( &o );
        }

        /** 3.1.5 construct a oyNAME_NAME string */
        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          o = oyOptions_Find( *oyConfig_GetOptions(device,"data"), "device_rectangle" );
          r = (oyRectangle_s*) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );

          text = 0; tmp = 0;
      
          tmp = oyRectangle_Show( (oyRectangle_s*)r );
          STRING_ADD( text, tmp );
          oyOption_Release( &o );

          o = oyOptions_Find( *oyConfig_GetOptions(device,"data"), "icc_profile" );

          if( o )
          {
            prof = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
            tmp = oyProfile_GetFileName( prof, 0 );

            STRING_ADD( text, "  " );
            if(tmp)
            {
              if(oyStrrchr_( tmp, OY_SLASH_C ))
                STRING_ADD( text, oyStrrchr_( tmp, OY_SLASH_C ) + 1 );
              else
                STRING_ADD( text, tmp );
            } else
            {
              tmp = oyProfile_GetText( prof, oyNAME_DESCRIPTION );
              if(tmp)
                STRING_ADD( text, tmp );
            }

            oyProfile_Release( &prof );
          }

          if(error <= 0)
          {
            t_err = oyOptions_SetFromText( oyConfig_GetOptions(device,"data"),
                                         OYX1_MONITOR_REGISTRATION OY_SLASH
                                         "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
            if(t_err > 0)
              error = t_err;
          }
          oyFree_m_( text );
        }

        if(!oyOptions_FindString( options, "icc_profile.fallback", 0 ) &&
           (oyOptions_FindString( options, "command", "properties" ) ||
            oyOptions_FindString( options, "edid", "refresh" )))
          error = oyX1DeviceFromName_( device_name, options, &device );

        /** 3.1.6 add the rank scheme to combine properties */
        if(error <= 0 && !oyConfig_GetRankMap(device))
          oyConfig_SetRankMap(device, oyX1_rank_map );

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
                                          oyConfig_GetRegistration(device),
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
          oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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
                                          oyConfig_GetRegistration(device),
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
          oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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


  oyX1_msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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
    oyX1_msg(oyMSG_DBG, (oyStruct_s*)config, OY_DBG_FORMAT_ "\n "
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
 *  @brief    oyRankMap map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/27
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 */
oyRankMap oyX1_rank_map[] = {
  {"device_name", 2, -1, 0},           /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {"display_geometry", 3, -1, 0},      /**< important to match, as fallback */
  {"manufacturer", 0, 0, 0},           /**< is nice, covered by mnft_id */
  {"EDID_mnft", 0, 0, 0},              /**< is nice, covered by mnft_id */
  {"EDID_mnft_id", 1, -1, 0},          /**< is nice */
  {"model", 0, 0, 0},                  /**< important, covered by model_id */
  {"EDID_model_id", 1, 0, 0},          /**< important, but fails sometimes */
  {"EDID_date", 2, 0, 0},              /**< good to match */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"EDID_red_x", 2, -5, 0},    /**< is nice, should not fail */
  {"EDID_red_y", 2, -5, 0},    /**< is nice, should not fail */
  {"EDID_green_x", 2, -5, 0},  /**< is nice, should not fail */
  {"EDID_green_y", 2, -5, 0},  /**< is nice, should not fail */
  {"EDID_blue_x", 2, -5, 0},   /**< is nice, should not fail */
  {"EDID_blue_y", 2, -5, 0},   /**< is nice, should not fail */
  {"EDID_white_x", 2, -5, 0},  /**< is nice, should not fail */
  {"EDID_white_y", 2, -5, 0},  /**< is nice, should not fail */
  {"EDID_gamma", 2, -5, 0},            /**< is nice, should not fail */
  {0,0,0,0}                            /**< end of list */
};

const char * oyX1Api8UiGetText       ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0 ||
     strcmp(select,"help") == 0)
  {
    /* The "help" and "name" texts are identical, as the module contains only
     * one filter to provide help for. */
    return oyX1GetText(select,type,context);
  }
  else if(strcmp(select, "device_class")==0)
  {
        if(type == oyNAME_NICK)
            return "monitor";
        else if(type == oyNAME_NAME)
            return _("Monitor");
        else
            return _("Monitors, which can be detected through the video card driver and windowing system.");
  }
  else if(strcmp(select, "icc_profile_class")==0)
    {
      return "display";
    } 
  else if(strcmp(select, "key_prefix")==0)
    {
      return "EDID_";
    } 
  else if(strcmp(select,"category") == 0)
  {
    if(!category)
    {
      STRING_ADD( category, _("Color") );
      STRING_ADD( category, _("/") );
      /* CMM: abbreviation for Color Matching Module */
      STRING_ADD( category, _("Device") );
      STRING_ADD( category, _("/") );
      STRING_ADD( category, _("Monitor") );
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return category;
    else
      return category;
  } 
  return 0;
}
const char * oyX1_api8_ui_texts[] = {"name", "help", "device_class", "icc_profile_class", "category", "key_prefix", 0};

/** @instance oyX1_api8_ui
 *  @brief    oyX1 oyCMMapi8_s::ui implementation
 *
 *  The UI for oyX1 devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/14 (Oyranos: 0.1.10)
 *  @date    2009/12/16
 */
oyCMMui_s_ oyX1_api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,9,5},                            /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Color/Device/Monitor", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyX1Api8UiGetText,  /* oyCMMGetText_f getText */
  oyX1_api8_ui_texts  /* (const char**)texts */
};

oyIcon_s oyX1_api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"
};

/** @instance oyX1_api8
 *  @brief    oyX1 oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.13
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2010/12/09
 */
oyCMMapi8_s_ oyX1_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  (oyCMMapi_s*) & oyX1_api10_set_xcm_region_handler, /**< next */

  oyX1CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  oyX1CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */

  OYX1_MONITOR_REGISTRATION, /**< registration */
  CMM_VERSION,                   /**< int32_t version[3] */
  {0,9,5},                  /**< int32_t module_api[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  0,                         /**< oyPointer_s * runtime_context */

  oyX1Configs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  oyX1Configs_Modify,        /**< oyConfigs_Modify_f oyConfigs_Modify */
  oyX1Config_Rank,           /**< oyConfig_Rank_f oyConfig_Rank */

  (oyCMMui_s*)&oyX1_api8_ui,             /**< device class UI name and help */
  &oyX1_api8_icon,           /**< device icon */

  oyX1_rank_map              /**< oyRankMap ** rank_map */
};

/* OYX1_MONITOR_REGISTRATION -------------------------------------------------*/


/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * oyX1GetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Oyranos X11");
    else
      return _("The window support module of Oyranos.");
  } else if(strcmp(select, "help")==0)
  {
    static char * t = 0;
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The oyX1 module supports the generic device protocol.");
    else
    {
      if(!t)
      {
        t = malloc( strlen(oyX1_help) + strlen(oyX1_help_list)
                    + strlen(oyX1_help_properties) + strlen(oyX1_help_setup)
                    + strlen(oyX1_help_unset)
                    + strlen(oyX1_help_add_edid_to_icc) + 2);
        sprintf( t, "%s\n%s%s%s%s%s", oyX1_help, oyX1_help_list,
                 oyX1_help_properties, oyX1_help_setup, oyX1_help_unset,
                 oyX1_help_add_edid_to_icc );
      }
      return t;
    }
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "newBSD";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2010 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}
const char *oyX1_texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance oyX1_cmm_module
 *  @brief    oyX1 module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMM_s oyX1_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.3",
  oyX1GetText, /* oyCMMinfoGetText_f get Text */
  (char**)oyX1_texts, /* texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyX1_api8,

  &oyX1_api8_icon
};

#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
int XcolorRegionFind(XcolorRegion * old_regions, unsigned long old_regions_n, Display * dpy, Window win, XRectangle * rectangle)
{   
  XRectangle * rect = 0;
  int nRect = 0;
  int pos = -1;
  unsigned long i;
  int j;
    
  /* get old regions */ 
  old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
  /* search region */
  for(i = 0; i < old_regions_n; ++i) 
  {     
                 
    if(!old_regions[i].region || pos >= 0)
      break;                    

    rect = XFixesFetchRegion( dpy, ntohl(old_regions[i].region),
                              &nRect );

    for(j = 0; j < nRect; ++j)
    {
      if(oy_debug) 
        printf( "reg[%lu]: %dx%d+%d+%d %dx%d+%d+%d\n",
                   i,
                   rectangle->width, rectangle->height,
                   rectangle->x, rectangle->y,
                   rect[j].width, rect[j].height, rect[j].x, rect[j].y
                  );
      if(rectangle->x == rect[j].x &&
         rectangle->y == rect[j].y &&
         rectangle->width == rect[j].width &&
         rectangle->height == rect[j].height )
      {
        pos = i;
        break;
      }
    }
  }

  return pos;
}
#endif

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
int          oyX1MOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  oyOption_s * o = 0;
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"set_xcm_region", 0))
    {
      o = oyOptions_Find( options, "window_rectangle" );
      if(!o)
      {
        oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option window_rectangle found");
        error = 1;
      }
      oyOption_Release( &o );
      o = oyOptions_Find( options, "window_id" );
      if(!o)
      {
        oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option window_id found");
        error = 1;
      }
      oyOption_Release( &o );
      o = oyOptions_Find( options, "display_id" );
      if(!o)
      {
        oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option display_id found");
        error = 1;
      }
      oyOption_Release( &o );

      return error;
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"set_xcm_region", 0))
  {
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
    oyProfile_s * p = NULL;
    oyRectangle_s * win_rect = NULL;
    oyRectangle_s * old_rect = NULL;

    Display * dpy = NULL;
    Window win = 0;
    char * blob = 0;
    size_t size = 0;
    XcolorProfile * profile = 0;
    XserverRegion reg = 0;
    XcolorRegion region;
    int error;
    XRectangle rec[2] = { { 0,0,0,0 }, { 0,0,0,0 } };
    double rect[4];

    oyBlob_s * win_id, * display_id;

    win_id = (oyBlob_s*) oyOptions_GetType( options, -1, "window_id",
                                          oyOBJECT_BLOB_S );
    display_id = (oyBlob_s*) oyOptions_GetType( options, -1, "display_id",
                                          oyOBJECT_BLOB_S );
    win = (Window) oyBlob_GetPointer(win_id);
    dpy = (Display *) oyBlob_GetPointer(display_id);

    oyBlob_Release( &win_id );
    oyBlob_Release( &display_id );

    /* now handle the options */
    win_rect = (oyRectangle_s*) oyOptions_GetType( options, -1, "window_rectangle",
                                          oyOBJECT_RECTANGLE_S );
    old_rect = (oyRectangle_s*) oyOptions_GetType( options, -1,
                                 "old_window_rectangle", oyOBJECT_RECTANGLE_S );
    o = oyOptions_Find( options, "icc_profile" );
    p = (oyProfile_s*) oyOptions_GetType( options, -1, "icc_profile",
                                          oyOBJECT_PROFILE_S );
    if(!win || !dpy)
      oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                "options display_id or window_id not found");
    if(!win_rect)
      oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                "option window_rectangle not found");


    if(old_rect)
    {
      XcolorRegion *old_regions = 0;
      unsigned long old_regions_n = 0;
      int pos = -1;

      oyRectangle_GetGeo( old_rect, &rect[0], &rect[1], &rect[2], &rect[3] );

      rec[0].x = rect[0];
      rec[0].y = rect[1];
      rec[0].width = rect[2];
      rec[0].height = rect[3];

      /* get old regions */
      old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
      /* remove specified region */
      pos = XcolorRegionFind( old_regions, old_regions_n, dpy, win, rec );
      XFree( old_regions );
      if(pos >= 0)
      {
        int undeleted_n = old_regions_n;
        XcolorRegionDelete( dpy, win, pos, 1 );
        old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
        if(undeleted_n - old_regions_n != 1)
          oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "removed %d; have still %d",
             OY_DBG_ARGS_, pos, (int)old_regions_n );
      } else
        oyX1_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                  "region not found in %lu\n",OY_DBG_ARGS_, old_regions_n );

      XFlush( dpy );

    }

    oyRectangle_GetGeo( win_rect, &rect[0], &rect[1], &rect[2], &rect[3] );

    rec[0].x = rect[0];
    rec[0].y = rect[1];
    rec[0].width = rect[2];
    rec[0].height = rect[3];

    if(p)
    {
        blob = (char*)oyProfile_GetMem( p, &size, 0,0 );

        if(blob && size)
        {
          int result;
        /* Create a XcolorProfile object that will be uploaded to the display.*/
          profile = (XcolorProfile*)malloc(sizeof(XcolorProfile) + size);

          oyProfile_GetMD5(p, 0, (uint32_t*)profile->md5);

          profile->length = htonl(size);
          memcpy(profile + 1, blob, size);

          result = XcolorProfileUpload( dpy, profile );
          if(result)
            oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                "XcolorProfileUpload: %d\n", result);

          XFlush( dpy );
        }
    }

    if( rect[0] || rect[1] || rect[2] || rect[3] )
    {
      reg = XFixesCreateRegion( dpy, rec, 1);

      region.region = htonl(reg);
      if(blob && size)
        memcpy(region.md5, profile->md5, 16);
      else
        memset( region.md5, 0, 16 );

      /* upload the new or changed region to the X server */
      error = XcolorRegionInsert( dpy, win, 0, &region, 1 );
      if(error)
          oyX1_msg( oyMSG_WARN, (oyStruct_s*)options,
                    "XcolorRegionInsert failed %d\n", error );
      XFlush( dpy );
    }
#endif
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
const char * oyX1InfoGetTextMyHandler( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "set_xcm_region")==0)
  {
         if(type == oyNAME_NICK)
      return "set_xcm_region";
    else if(type == oyNAME_NAME)
      return _("Set a X Color Management region.");
    else
      return _("The set_xcm_region takes minimal three options. The key name "
               "\"window_rectangle\" specifies in a oyRectangle_s object the "
               "requested window region in coordinates relative to the window. "
               "If its parameters are all set to zero, then the rectangle is "
               "ignored. The \"old_window_rectangle\" is similiar to the "
               "\"window_rectangle\" "
               "option but optionally specifies to remove a old rectangle. "
               "The \"window_id\" specifies a X11 window id as oyBlob_s. "
               "The \"display_id\" specifies a X11 Display struct as oyBlob_s. "
               "The " "\"icc_profile\" option of type oyProfile_s optionally "
               "provides a ICC profile to upload to the server.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"set_xcm_region\" handler lets you set "
               "X Color Management compatible client side color regions. "
               "The implementation uses libXcm and Oyranos.");
  }
  return 0;
}
const char *oyX1_texts_set_xcm_region[4] = {"can_handle","set_xcm_region","help",0};

/** @instance oyX1_api10_set_xcm_region_handler
 *  @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Manageent server side regions setup
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
oyCMMapi10_s_    oyX1_api10_set_xcm_region_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) NULL,

  oyX1CMMInit,
  oyX1CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "set_xcm_region._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  {0,9,5},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandler,             /**< getText */
  (char**)oyX1_texts_set_xcm_region,       /**<texts; list of arguments to getText*/
 
  oyX1MOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};


