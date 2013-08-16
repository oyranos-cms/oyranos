/** @file oyranos_cmm_qarz.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos Quarz module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/12/12
 */

#include "oyCMMapi8_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyCMMinfo_s_.h"
#include "oyCMMui_s_.h"

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_internal_cs.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>


/* --- internal definitions --- */

#define CMM_NICK "qarz"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/* QARZ_MONITOR_REGISTRATION */

int                qarzCMMInit       ( );
int            qarzCMMMessageFuncSet ( oyMessage_f         message_func );

/* QARZ_MONITOR_REGISTRATION -------------------------------------------------*/

#define QARZ_MONITOR_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.device.icc_profile.monitor." CMM_NICK

oyMessage_f message = 0;

extern oyCMMapi8_s_ qarz_api8;
oyRankMap qarz_rank_map[];

int          qarzDeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device );
int            qarzConfigs_Modify    ( oyConfigs_s       * devices,
                                       oyOptions_s       * options );
const char * qarzGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * qarzApi8UiGetText       ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

/* --- implementations --- */

int                qarzCMMInit       ( oyStruct_s        * filter )
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

/** @func  qarzCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            qarzCMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

#define OPTIONS_ADD(opts, name, clear) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                     QARZ_MONITOR_REGISTRATION OY_SLASH #name, \
                                       name, OY_CREATE_NEW ); \
        if(clear && name) { oyDeAllocateFunc_( (char*)name ); name = 0; }
#define OPTIONS_ADD_INT(opts, name) if(!error && name) { \
        oySprintf_( num, "%d", name ); \
        error = oyOptions_SetFromText( &opts, \
                                     QARZ_MONITOR_REGISTRATION OY_SLASH #name, \
                                       num, OY_CREATE_NEW ); \
        }

const char * qarz_help_list = 
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
      " A option \"edid\" key word with value \"refresh\" will \n"
      " update the EDID atom if possible.\n"
      " The option \"device_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION\n"
      " above.\n"
      " Informations are stored in the returned oyConfig_s::data member.";
const char * qarz_help_properties =
      "The presence of option \"command=properties\" will provide the devices\n"
      " properties. Requires one device identifier returned with the \n"
      " \"list\" option. The properties may cover following entries:\n"
      " - \"EDID_manufacturer\" description\n"
      " - \"manufacturer\" dupicate of the previous key\n"
      " - \"EDID_mnft\" (decoded EDID_mnft_id)\n"
      " - \"EDID_model\" textual name\n"
      " - \"model\" duplicate of the previous key\n"
      " - \"EDID_serial\" not always present\n"
      " - \"serial\" duplicate of the previous key\n"
      " - \"host\" not always present\n"
      " - \"system_port\"\n"
      " - \"EDID_datek\" manufacture date\n"
      " - \"EDID_mnft_id\" manufacturer ID (undecoded mnft)\n"
      " - \"EDID_model_id\" model ID\n"
      " - \"display_geometry\" (specific) widthxheight+x+y ,e.g."
      " \"1024x786+0+0\"\n"
      " - \"EDID_red_x\" \"EDID_red_y\" \"EDID_green_x\" \"EDID_green_y\" "
      "   \"EDID_blue_x\" \"EDID_blue_x\" \"EDID_white_x\" \"EDID_white_x\" "
      "   \"EDID_gamma\","
      " colour characteristics as found in EDID as text\n"
      " - \"colour_matrix.from_edid."
                   "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\","
      " colour characteristics as found in EDID as doubles\n"
      " \n"
      " One option \"device_name\" will select the according X display.\n"
      " If not the module will try to get this information from \n"
      " your \"DISPLAY\" environment variable or uses what the system\n"
      " provides. The \"device_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one.\n"
      " Informations are stored in the returned oyConfig_s::backend_core member."
;
const char * qarz_help_setup =
      "The presence of option \"command=setup\" will setup the device from a\n"
      " profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"      " file name."
;
const char * qarz_help_unset =
      "The presence of call \"command=unset\" will invalidate a profile of\n"
      " a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
;
const char * qarz_help_add_edid_to_icc =
      "The presence of option \"command=add-edid-meta-to-icc\" will embedds device\n"
      " informations from a provided EDID block to a provided ICC profile.\n"
      " The option \"edid\" must be present and contain an\n"
      " oyBlob_s object with the valid EDID data block.\n"
      " The bidirectional option \"icc_profile\" options must be present,\n"
      " containing a oyProfile_s object.\n"
;
const char * qarz_help =
      "The following help text informs about the communication protocol."
;

void     qarzConfigsUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN shall make shure our message will be visible. */
    message( oyMSG_WARN, options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_, qarz_help );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, qarz_help_list );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, qarz_help_properties );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, qarz_help_setup );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, qarz_help_unset );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, qarz_help_add_edid_to_icc );
#if 0
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"get\" will provide a oyProfile_s of the\n"
      " device in a \"icc_profile\" option.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );
#endif

  return;
}


int          qarzDeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device )
{
  const char * value3 = 0;
  int error = !device;
  oyOption_s * o = 0;

    value3 = oyOptions_FindString( options, "edid", 0 );

    if(!error)
    {
      char * manufacturer=0, * mnft=0, * model=0, * serial=0, * vendor = 0,
           * host=0, * display_geometry=0, * system_port=0;
      double colours[9] = {0,0,0,0,0,0,0,0,0};
      oyBlob_s * edid = 0;
      uint32_t week=0, year=0, mnft_id=0, model_id=0;

      if(!device_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", OY_DBG_ARGS_ );
        error = 1;
      }

      if(error <= 0)
        error = qarzGetMonitorInfo_lib( device_name,
                                      &manufacturer, &mnft, &model, &serial,
                                      &vendor, &display_geometry, &system_port,
                                      &host, &week, &year, &mnft_id, &model_id,
                                      colours,
                                      &edid, oyAllocateFunc_,
                                      (oyStruct_s*)options );

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  qarzGetMonitorInfo_lib returned with %s; device_name:"
                 " \"%s\"", OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 oyNoEmptyString_m_( device_name ) );

      if(error <= 0 && edid)
        error = oyDeviceFillEdid(   QARZ_MONITOR_REGISTRATION,
                                    device, oyBlob_GetPointer(edid), oyBlob_GetSize(edid),
                                    device_name,
                                    host, display_geometry, system_port,
                                    options );
      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options,
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  qarzGetMonitorInfo_lib returned with %s %d; device_name:"
                 " \"%s\"",OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 error, oyNoEmptyString_m_( device_name ) );

      if(value3)
      {
        if(!error && edid)
        {
          int has = 0;
          o = oyConfig_Find( *device,
                             QARZ_MONITOR_REGISTRATION OY_SLASH "edid" );
          if(o)
            has = 1;
          else
            o = oyOption_FromRegistration( QARZ_MONITOR_REGISTRATION OY_SLASH "edid", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, oyBlob_GetPointer(edid), oyBlob_GetSize(edid) );
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
    }

  return error;
}


/** Function qarzConfigs_FromPattern
 *  @brief   qarz oyCMMapi8_s Xorg monitors
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/28
 */
int            qarzConfigs_FromPattern (
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
  int rank = oyFilterRegistrationMatch( qarz_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  const char * tmp = 0;


  /** 1. In case no option is provided or something fails, show a message. */
  if(!options || !oyOptions_Count( options ))
  {
    qarzConfigsUsage( (oyStruct_s*)options );
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
      texts_n = qarzGetAllScreenNames( device_name, &texts, allocateFunc );

      /** 3.1.1 iterate over all requested devices */
      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odevice_name && strcmp(odevice_name, texts[i]) != 0)
          continue;

        device = oyConfig_FromRegistration( QARZ_MONITOR_REGISTRATION, 0 );
        error = !device;

         /** 3.1.2 tell the "device_name" */
        if(error <= 0)
        error = oyOptions_SetFromText( oyConfig_GetOptions(device,"backend_core"),
                                       QARZ_MONITOR_REGISTRATION OY_SLASH
                                       "device_name",
                                       texts[i], OY_CREATE_NEW );

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
      {
        if(devices && oyConfigs_Count(devices))
          error = qarzConfigs_Modify( devices, options );
        else if(oy_debug)
        {
          const char * t = oyOptions_GetText( options, oyNAME_NICK );
          message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "No monitor devices found.\n Options:\n%s", OY_DBG_ARGS_,
                oyNoEmptyString_m_( t )
                );
        }
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
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "The device_name/profile_name option is missed. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = qarzMonitorProfileSetup( odevice_name, oprofile_name );

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
        error = qarzMonitorProfileUnset( odevice_name );

      goto cleanup;
    }

    /** 3.5 "help" call; display a help text */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "help" ))
    {
      qarzConfigsUsage( (oyStruct_s*)options );

      goto cleanup;
    }

    /** 3.6 internal "add-edid-meta-to-icc" call; Embedd infos to ICC profile 
     *      as meta tag. Might be moved to a oyCMMapi10_s object. */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "add-edid-meta-to-icc" ))
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
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "\"edid\" or \"icc_profile\" missed:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
        qarzConfigsUsage( (oyStruct_s*)options );
      }
      else
      {
        oyOptions_s * opts = 0;
        error = oyDeviceFillEdid(   QARZ_MONITOR_REGISTRATION,
                                    &device, oyBlob_GetPointer(edid), oyBlob_GetSize(edid),
                                    NULL,
                                    NULL, NULL, NULL,
                                    options );
        if(error <= 0)
          error = oyOptions_SetFromText( &opts, "///key_prefix_required",
                                                "EDID_" , OY_CREATE_NEW );
        oyProfile_AddDevice( prof, device, opts );
        oyOptions_Release( &opts );
      }

      goto cleanup;
    }
  }


  message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  qarzConfigsUsage( (oyStruct_s*)options );


  cleanup:
  if(device_name_temp)
    oyFree_m_( device_name_temp );


  return error;
}


/** Function qarzConfigs_Modify
 *  @brief   qarz oyCMMapi8_s Xorg monitor manipulation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
int            qarzConfigs_Modify    ( oyConfigs_s       * devices,
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
    qarzConfigsUsage( (oyStruct_s*)options );
    return 0;
  }

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    rank += oyFilterRegistrationMatch( qarz_api8.registration,
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
        rank = oyFilterRegistrationMatch( qarz_api8.registration,
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
          rect = qarzRectangle_FromDevice( device_name );
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
              o = oyOption_FromRegistration( QARZ_MONITOR_REGISTRATION OY_SLASH
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
              message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Try %s from %s",
                     OY_DBG_ARGS_,
                     oyOptions_FindString(options, "x_color_region_target", 0) ? 
                     "ColorSync" : "Base",
                     device_name );
            flags |= 0x01;
          }
          data = qarzGetMonitorProfile( device_name, flags, &size,
                                        allocateFunc );


          has = 0;
          o = oyConfig_Find( device, "icc_profile" );
          if(o)
          {
            /* the device might have assigned a dummy icc_profile, to show 
             * it can handle. But thats not relevant here. */
            p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
            if(oyProfile_GetSignature( p, oySIGNATURE_MAGIC ) == icMagicNumber)
              has = 1;
            else
              oyOption_Release( &o );

            oyProfile_Release( &p );
          }

          if(data && size)
          {
            prof = oyProfile_FromMem( size, data, 0, 0 );
            free( data );
            if(has == 0)
            {
              const char * key = QARZ_MONITOR_REGISTRATION OY_SLASH "icc_profile";
              if(oyOptions_FindString(options, "x_color_region_target", 0))
                key = QARZ_MONITOR_REGISTRATION OY_SLASH "icc_profile.x_color_region_target";
              o = oyOption_FromRegistration( key, 0 );
            }
          } else if(oyOptions_FindString( options, "icc_profile.fallback", 0 ))
          {
            icHeader * header = 0;
            /* fallback: try to get EDID to build a profile */
            o_tmp = oyConfig_Find( device, "colour_matrix."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");
            if(!o_tmp)
            {
              oyOptions_SetFromText( &options,
                                     QARZ_MONITOR_REGISTRATION OY_SLASH
                                     "edid",
                                     "yes", OY_CREATE_NEW );
              error = qarzDeviceFromName_( device_name, options, &device );
              o_tmp = oyConfig_Find( device, "colour_matrix."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");
            }

            if(o_tmp)
            {
              oyOptions_s * opts = oyOptions_New(0),
                          * result = 0;
              error = oyOptions_MoveIn( opts, &o_tmp, -1 );
              oyOptions_Handle( "///create_profile.icc",
                                opts,"create_profile.icc_profile.colour_matrix",
                                &result );
              prof = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                        oyOBJECT_PROFILE_S );
              oyOptions_Release( &result );
            }

            if(prof)
            {
              const char * t = 0;
              t = oyConfig_FindString( device, "model", 0 );
              if(!t)
                message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                "Could not obtain \"manufacturer\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              else
                STRING_ADD( text, oyConfig_FindString( device, "model", 0 ) );
              STRING_ADD( text, "_edid" );
              error = oyProfile_AddTagText( prof,
                                            icSigProfileDescriptionTag, text);
              oyDeAllocateFunc_( text ); text = 0;
              t = oyConfig_FindString( device, "manufacturer", 0);
              if(!t)
                message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                "Could not obtain \"manufacturer\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              else
                error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t);
              t = oyConfig_FindString( device, "model", 0 );
              if(!t)
                message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Could not obtain \"model\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              else
                error = oyProfile_AddTagText( prof, icSigDeviceModelDescTag, t);
              data = oyProfile_GetMem( prof, &size, 0, oyAllocateFunc_ );
              header = (icHeader*) data;
              o_tmp = oyConfig_Find( device, "mnft" );
              t = oyConfig_FindString( device, "mnft", 0 );
              if(!t)
                message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Could not obtain \"mnft\" from monitor device for %s",
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
            if(has == 0)
              o = oyOption_FromRegistration( QARZ_MONITOR_REGISTRATION OY_SLASH
                                "icc_profile.fallback", 0 );
            error = -1;
          }

          if(!o)
              o = oyOption_FromRegistration( QARZ_MONITOR_REGISTRATION OY_SLASH
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
            message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Could not obtain %s information for %s",
                     OY_DBG_ARGS_,
                     oyOptions_FindString(options, "x_color_region_target", 0) ? 
                     "ColorSync" : "Base",
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
                                         QARZ_MONITOR_REGISTRATION OY_SLASH
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
          error = qarzDeviceFromName_( device_name, options, &device );

        /** 3.1.6 add the rank scheme to combine properties */
        if(error <= 0 && !oyConfig_GetRankMap(device))
          oyConfig_SetRankMap(device, qarz_rank_map );

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
        rank = oyFilterRegistrationMatch( qarz_api8.registration,
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
          message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "The device_name/profile_name option is missed. Options:\n%s",
                  OY_DBG_ARGS_,
                  oyOptions_GetText( options, oyNAME_NICK )
                  );
        else
          error = qarzMonitorProfileSetup( device_name, oprofile_name );

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
        rank = oyFilterRegistrationMatch( qarz_api8.registration,
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
          message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "The device_name option is missed. Options:\n%s",
                  OY_DBG_ARGS_,
                  oyOptions_GetText( options, oyNAME_NICK )
                  );
        else
          error = qarzMonitorProfileUnset( device_name );

        oyConfig_Release( &device );
      }

      goto cleanup;
    }

    /** 3.5 "help" call; display a help text */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "help" ))
    {
      qarzConfigsUsage( (oyStruct_s*)options );

      goto cleanup;
    }
  }


  message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  qarzConfigsUsage( (oyStruct_s*)options );


  cleanup:

  return error;
}

/** Function qarzConfig_Rank
 *  @brief   qarz oyCMMapi8_s Xorg monitor check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/26
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
int            qarzConfig_Rank       ( oyConfig_s        * config )
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

/** @instance qarz_rank_map
 *  @brief    oyRankMap map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/27
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 */
oyRankMap qarz_rank_map[] = {
  {"device_name", 2, -1, 0},           /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {"display_geometry", 3, -1, 0},      /**< important to match, as fallback */
  {"manufacturer", 0, 0, 0},           /**< is nice, covered by mnft_id */
  {"EDID_mnft", 0, 0, 0},              /**< is nice, covered by mnft_id */
  {"EDID_mnft_id", 1, -1, 0},          /**< is nice */
  {"model", 0, 0, 0},                  /**< important, covered by model_id */
  {"EDID_model_id", 5, -5, 0},         /**< important, should not fail */
  {"EDID_date", 2, 0, 0},              /**< good to match */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"EDID_red_x", 1, -5, 0},    /**< is nice, should not fail */
  {"EDID_red_y", 1, -5, 0},    /**< is nice, should not fail */
  {"EDID_green_x", 1, -5, 0},  /**< is nice, should not fail */
  {"EDID_green_y", 1, -5, 0},  /**< is nice, should not fail */
  {"EDID_blue_x", 1, -5, 0},   /**< is nice, should not fail */
  {"EDID_blue_y", 1, -5, 0},   /**< is nice, should not fail */
  {"EDID_white_x", 1, -5, 0},  /**< is nice, should not fail */
  {"EDID_white_y", 1, -5, 0},  /**< is nice, should not fail */
  {"EDID_gamma", 1, -5, 0},            /**< is nice, should not fail */
  {0,0,0,0}                            /**< end of list */
};

const char * qarzApi8UiGetText       ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0 ||
     strcmp(select,"help") == 0)
  {
    /* The "help" and "name" texts are identical, as the module contains only
     * one filter to provide help for. */
    return qarzGetText(select,type,context);
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
      STRING_ADD( category, _("Colour") );
      STRING_ADD( category, _("/") );
      /* CMM: abbreviation for Colour Matching Module */
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
const char * qarz_api8_ui_texts[] = {"name", "help", "device_class", "icc_profile_class", "category", "key_prefix", 0};

/** @instance qarz_api8_ui
 *  @brief    qarz oyCMMapi8_s::ui implementation
 *
 *  The UI for qarz devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/14 (Oyranos: 0.1.10)
 *  @date    2009/12/16
 */
oyCMMui_s_ qarz_api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,9,0},                            /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Colour/Device/Monitor", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  qarzApi8UiGetText,  /* oyCMMGetText_f getText */
  qarz_api8_ui_texts  /* (const char**)texts */
};

oyIcon_s qarz_api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"
};

/** @instance qarz_api8
 *  @brief    qarz oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
oyCMMapi8_s_ qarz_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  (oyCMMapi_s*) 0, /**< next */

  qarzCMMInit,               /**< oyCMMInit_f      oyCMMInit */
  qarzCMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */

  QARZ_MONITOR_REGISTRATION, /**< registration */
  CMM_VERSION,               /**< int32_t version[3] */
  {0,9,0},                   /**< int32_t module_api[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  qarzConfigs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  qarzConfigs_Modify,        /**< oyConfigs_Modify_f oyConfigs_Modify */
  qarzConfig_Rank,           /**< oyConfig_Rank_f oyConfig_Rank */

  (oyCMMui_s*)&qarz_api8_ui,             /**< device class UI name and help */
  &qarz_api8_icon,           /**< device icon */

  qarz_rank_map              /**< oyRankMap ** rank_map */
};

/* QARZ_MONITOR_REGISTRATION -------------------------------------------------*/


/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * qarzGetText             ( const char        * select,
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
      return _("The qarz module supports the generic device protocol.");
    else
    {
      if(!t)
      {
        t = malloc( strlen(qarz_help) + strlen(qarz_help_list)
                    + strlen(qarz_help_properties) + strlen(qarz_help_setup)
                    + strlen(qarz_help_unset)
                    + strlen(qarz_help_add_edid_to_icc) + 1);
        sprintf( t, "%s\n%s%s%s%s%s", qarz_help, qarz_help_list,
                 qarz_help_properties, qarz_help_setup, qarz_help_unset,
                 qarz_help_add_edid_to_icc );
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
const char *qarz_texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance qarz_cmm_module
 *  @brief    qarz module infos
 *
 *  @version Oyranos: 0.1.13
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2010/11/08
 */
oyCMMinfo_s_ qarz_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.3",
  qarzGetText, /* oyCMMinfoGetText_f get Text */
  (char**)qarz_texts, /* texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & qarz_api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

