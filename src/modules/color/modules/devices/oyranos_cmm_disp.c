/** @file oyranos_cmm_disp.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2007-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 module for Oyranos
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2007/12/12
 */

/** \addtogroup monitor_device disp Module(s)
 *  @{ */

#include "oyCMM_s.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMui_s_.h"

#include "oyConfig_s.h"
#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#ifdef oyX1
# include "oyranos_monitor_internal_x11.h"
#endif
#ifdef qarz
# include "oyranos_monitor_internal_cs.h"
#endif
#include "oyranos_monitor_internal.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

/* --- internal definitions --- */

/* MONITOR_REGISTRATION */

/* MONITOR_REGISTRATION -------------------------------------------------*/

/** Common monitor registration base string
 *
 *  Dependent on your platform are different CMM_NICKs implemented.
 *  A few are "qarz" and "oyX1". They can be labeld as "native" in
 *  case the module fits to the primary platform displaying system.
 */
#define MONITOR_REGISTRATION_BASE OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "device" OY_SLASH "config.icc_profile.monitor."
#if defined(qarz) || defined(wcsD)
#define MONITOR_REGISTRATION MONITOR_REGISTRATION_BASE "native." CMM_NICK
#else
#define MONITOR_REGISTRATION MONITOR_REGISTRATION_BASE CMM_NICK
#endif

oyMessage_f _msg = 0;

extern oyCMMapi8_s_  _api8;
static char * _category = NULL;
static char * _help_desc = NULL;
static int _initialised = 0;

int                DeviceFromName_   ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device );
int                Configs_Modify    ( oyConfigs_s       * devices,
                                       oyOptions_s       * options );
const char *       GetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

/* --- implementations --- */

int                CMMapiInit        ( oyStruct_s        * filter )
{
  int error = 0;
  const char * rfilter = "config.icc_profile.monitor.oyX1.qarz";
  const char * rank_json = hooks3->rank_map;
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*) filter;

  if(!_initialised)
    error = oyDeviceCMMInit( filter, rfilter, 1 );
  ++_initialised;

  if(s->rank_map == NULL)
  {
    oyRankMap * rank_map = NULL;
    error = oyRankMapFromJSON ( rank_json, NULL, &rank_map, oyAllocateFunc_ );

    if(!rank_map || error || !rank_map[0].key)
      oyMessageFunc_p( oyMSG_WARN, filter, "%s() %s: %s  %d", __func__,
                       _("Creation of rank_map failed from"), rank_json, error );
    else
      s->rank_map = rank_map;
  }

  return error;
}

int CMMapiReset( oyStruct_s * filter )
{
  int error = 0;

  if(_initialised)
  {
    error = oyDeviceCMMReset( filter );
    if(_category) oyFree_m_(_category);
    if(_help_desc) oyFree_m_(_help_desc);
    _initialised = 0;
  }

  return error;
}

int CMMinit( oyStruct_s * filter OY_UNUSED ) { return 0; }
int CMMreset( oyStruct_s * filter OY_UNUSED ) { return 0; }


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

/** @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int                CMMMessageFuncSet ( oyMessage_f         message_func )
{
  _msg = message_func;
  return 0;
}

const char * _help_list = 
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
const char * _help_properties =
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
      " color characteristics as found in EDID as doubles."
;
const char * _help_setup =
      "The presence of option \"command=setup\" will setup the device from a\n"
      " profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"
      " file name.\n"
      " The option \"gamma_only:yes\" can be added to skip the ICC profile\n"
      " assign stage."
;
const char * _help_unset =
      "The presence of call \"command=unset\" will invalidate a profile of\n"
      " a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
;
const char * _help_add_edid_to_icc =
      "The presence of option \"command=add_meta\" will embedd device\n"
      " informations from a provided EDID block to a provided ICC profile.\n"
      " The option \"edid\" must be present and contain an\n"
      " oyBlob_s object with the valid EDID data block.\n"
      " The bidirectional option \"icc_profile\" options must be present,\n"
      " containing a oyProfile_s object.\n"
;
const char * _help =
      "The following help text informs about the communication protocol."
;

void               ConfigsUsage      ( oyStruct_s        * options )
{
    /** oyMSG_WARN shall make shure our message will be visible. */
    _msg( oyMSG_WARN, options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_, _help );
    _msg( oyMSG_WARN, options, "%s()\n %s", __func__, _help_list );
    _msg( oyMSG_WARN, options, "%s()\n %s", __func__, _help_properties );
    _msg( oyMSG_WARN, options, "%s()\n %s", __func__, _help_system_specific );
    _msg( oyMSG_WARN, options, "%s()\n %s", __func__, _help_setup );
    _msg( oyMSG_WARN, options, "%s()\n %s", __func__, _help_unset );
    _msg( oyMSG_WARN, options, "%s()\n %s", __func__, _help_add_edid_to_icc );
#if 0
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"get\" will provide a oyProfile_s of the\n"
      " device in a \"icc_profile\" option.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );
#endif

  return;
}

oyMonitor_s *    dispDeviceGetMonitor( oyConfig_s        * device )
{
  const char * device_name = oyConfig_FindString( device, "device_name", 0 );
  oyPointer_s * oy_struct = (oyPointer_s*)oyOptions_GetType( *oyConfig_GetOptions( device, "data" ), -1, "device_handle",
                                              oyOBJECT_POINTER_S );
  oyMonitor_s * moni = (oyMonitor_s*) oyPointer_GetPointer( oy_struct );
  oyPointer_Release( &oy_struct );

  if(moni)
    return moni;

  moni = GetMonitorFromName( device_name );
  if(moni)
  {
    /*Handle "device_handle" option */
    oyPointer_s * handle_ptr = oyPointer_New(0);
    oyPointer_Set(handle_ptr,
                            CMM_NICK,
                            "handle",
                            (oyPointer)moni,
                            "FreeMonitor",
                            (oyPointer_release_f) FreeMonitor );
    oyOptions_MoveInStruct( oyConfig_GetOptions( device, "data" ),
                                      MONITOR_REGISTRATION_BASE OY_SLASH "device_handle",
                                      (oyStruct_s **) &handle_ptr, OY_CREATE_NEW);
  }
  return moni;
}

int              DeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device )
{
  const char * value3 = 0;
  int error = !device;
  oyOption_s * o = 0;
  oyMonitor_s * moni = NULL;

    value3 = oyOptions_FindString( options, "edid", 0 );

    if(!error)
    {
      char * EDID_manufacturer=0, * EDID_mnft=0, * EDID_model=0,
           * EDID_serial=0, * EDID_vendor = 0,
           * host=0, * display_geometry=0, * system_port=0,
           * debug_info=0;
      double colors[9] = {0,0,0,0,0,0,0,0,0};
      oyBlob_s * edid = 0;
      int week=0, year=0, EDID_mnft_id=0, EDID_model_id=0;

      if(!device_name)
      {
        _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", OY_DBG_ARGS_ );
        error = 1;
      }

      if(error <= 0)
      {
        char * edid_data = NULL;
        size_t edid_size = 0;
        moni = GetMonitorFromName( device_name );
        if(moni)
        {
          /*Handle "device_handle" option */
          oyPointer_s * handle_ptr = oyPointer_New(0);
          oyPointer_Set(handle_ptr,
                            CMM_NICK,
                            "handle",
                            (oyPointer)moni,
                            "FreeMonitor",
                            (oyPointer_release_f) FreeMonitor );
          oyOptions_MoveInStruct( oyConfig_GetOptions( *device, "data" ),
                                      MONITOR_REGISTRATION_BASE OY_SLASH "device_handle",
                                      (oyStruct_s **) &handle_ptr, OY_CREATE_NEW);

          error = GetMonitorInfo_lib( moni,
                      &EDID_manufacturer, &EDID_mnft, &EDID_model, &EDID_serial,
                      &EDID_vendor, &display_geometry, &system_port, &host,
                      &week, &year, &EDID_mnft_id, &EDID_model_id,
                                        colors,
                                        &edid_data, &edid_size,
                                        oyOptions_FindString( options, "edid", "refresh" ) ? 1 : 0,
                      &debug_info );
        } else
          _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "Unable to open monitor device \"%s\"\n", OY_DBG_ARGS_, device_name);

        if(edid_data && edid_size)
        {
          edid = oyBlob_New(0);
          oyBlob_SetFromData( edid, edid_data, edid_size, 0 );
          free( edid_data );
        }
      }

      if(error != 0)
        _msg( oyMSG_WARN, (oyStruct_s*)options, 
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  "CMM_NICK"GetMonitorInfo_lib returned with %s; device_name:"
                 " \"%s\"", OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 oyNoEmptyString_m_( device_name ) );

      if(error <= 0 && edid)
      {
        error = oyDeviceFillEdid(   MONITOR_REGISTRATION,
                                    device, oyBlob_GetPointer(edid),
                                    oyBlob_GetSize(edid),
                                    device_name,
                                    host, display_geometry, system_port,
                                    options );
      } else if(error <= 0)
      {
        if(!error && device_name)
          oyOptions_SetFromString( oyConfig_GetOptions(*device,"backend_core"),
                                       MONITOR_REGISTRATION OY_SLASH "device_name",
                                       device_name, OY_CREATE_NEW );

        error = oyDeviceFillInfos( MONITOR_REGISTRATION, device,
                                   device_name, host, display_geometry,
                                   system_port,
                                   EDID_manufacturer, EDID_mnft, EDID_model,
                                   EDID_serial, EDID_vendor,
                                   week, year,
                                   EDID_mnft_id, EDID_model_id,
                                   colors, options );
      }
      if(debug_info)
        error = oyOptions_SetFromString( oyConfig_GetOptions(*device,"data"),
                                       MONITOR_REGISTRATION OY_SLASH
                                       "debug_info",
                                       debug_info, OY_CREATE_NEW );

      if(error != 0)
        _msg( oyMSG_WARN, (oyStruct_s*)options,
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  "CMM_NICK"GetMonitorInfo_lib returned with %s %d; device_name:"
                 " \"%s\"",OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 error, oyNoEmptyString_m_( device_name ) );

      if(value3)
      {
        if(!error && edid)
        {
          int has = 0;
          o = oyConfig_Find( *device,
                             MONITOR_REGISTRATION OY_SLASH "edid" );
          if(o)
            has = 1;
          else
            o = oyOption_FromRegistration(
                                MONITOR_REGISTRATION OY_SLASH "edid", 0 );
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
      if(debug_info) oyFree_m_(debug_info);
    }

  return error;
}


/** Function Configs_FromPattern
 *  @brief   oyCMMapi8_s monitors
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/28
 */
int                Configs_FromPattern (
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
  int rank = oyFilterRegistrationMatch( _api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );


  /** 1. In case no option is provided or something fails, show a message. */
  if(!options || !oyOptions_Count( options ))
  {
    ConfigsUsage( (oyStruct_s*)options );
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
#ifdef oyX1
      const char * tmp = getenv("DISPLAY");
      if(!tmp)
      {
        _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
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
      texts_n = GetAllScreenNames( device_name, &texts );

      /** 3.1.1 iterate over all requested devices */
      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odevice_name && strcmp(odevice_name, texts[i]) != 0)
          continue;

        device = oyConfig_FromRegistration( MONITOR_REGISTRATION, 0 );
        error = !device;

         /** 3.1.2 tell the "device_name" */
        if(error <= 0)
        error = oyOptions_SetFromString( oyConfig_GetOptions(device,"backend_core"),
                                       MONITOR_REGISTRATION OY_SLASH
                                       "device_name",
                                       texts[i], OY_CREATE_NEW );
        if(error <= 0)
        error = DeviceFromName_( texts[i], options, &device );

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
      {
        if(devices && oyConfigs_Count(devices))
          error = Configs_Modify( devices, options );
        else if(oy_debug)
          _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "No monitor devices found.\n Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      }

      if(error <= 0)
        *s = oyConfigs_Copy( devices, 0 );

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
        _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "The device_name/profile_name option is missed. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
      {
        oyProfile_s * p = oyProfile_FromName( oprofile_name, 0, 0 );
        size_t size = oyProfile_GetSize( p, 0 );
        char * data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
        const char * profile_fullname = oyProfile_GetFileName( p, -1 );
        int gamma_only = oyOptions_FindString( options, "gamma_only", "yes" ) != NULL;
        oyMonitor_s * moni = GetMonitorFromName( odevice_name );
        error = moni?0:-1;

        _msg(error?oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "command: setup on device_name: %s \"%s\" %lu%s",
                  OY_DBG_ARGS_, odevice_name, oprofile_name, size,
                  gamma_only?" only VCGT":"" );

        if(error == 0)
        {
          fprintf(stderr, "%s [" CMM_NICK "] SetupMonitorCalibration:", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
          if(!gamma_only)
            error = SetupMonitorProfile( moni, profile_fullname, data, size );
          error = SetupMonitorCalibration( moni, profile_fullname, data, size );

          FreeMonitor( &moni );
        }
        oyProfile_Release( &p );
      }

      goto cleanup;

    } else

    /** 3.4 "unset" call; clear a profile from a device */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "unset" ))
    {
      error = !odevice_name;
      if(error >= 1)
        _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "The device_name option is missed. Options:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
      {
        oyMonitor_s * moni = GetMonitorFromName( odevice_name );
        error = moni?0:-1;
        _msg(error?oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "command: unset on device_name: \"%s\"",
                  OY_DBG_ARGS_, odevice_name );
        if(error == 0)
        {
          error = UnsetMonitorProfile( moni );
          FreeMonitor( &moni );
        }
      }

      goto cleanup;
    }

    /** 3.5 "help" call; display a help text */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "help" ))
    {
      ConfigsUsage( (oyStruct_s*)options );

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
        _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "\"edid\" or \"icc_profile\" missed:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
        ConfigsUsage( (oyStruct_s*)options );
      }
      else
      {
        oyOptions_s * opts = 0;
        error = oyDeviceFillEdid(   MONITOR_REGISTRATION,
                                    &device, oyBlob_GetPointer(edid),
                                    oyBlob_GetSize( edid ),
                                    NULL,
                                    NULL, NULL, NULL,
                                    options );
        if(error <= 0)
        {
          error = oyOptions_SetFromString( &opts, "///set_device_attributes",
                                                "true", OY_CREATE_NEW );
          error = oyOptions_SetFromString( &opts, "///key_prefix_required",
                                                "EDID_.prefix", OY_CREATE_NEW );
        }
        oyProfile_AddDevice( prof, device, opts );

        error = oyOptions_SetFromString( oyConfig_GetOptions(device,"backend_core"),
                                       MONITOR_REGISTRATION OY_SLASH
                                       "device_name",
                                       device_name, OY_CREATE_NEW );
        if(error <= 0 && !oyConfig_GetRankMap(device))
          oyConfig_SetRankMap(device, _api8.rank_map );
        oyConfigs_MoveIn( devices, &device, -1 );
        if(error <= 0)
          *s = oyConfigs_Copy( devices, 0 );

        oyOptions_Release( &opts );
      }

      goto cleanup;
    }
  }


  _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  ConfigsUsage( (oyStruct_s*)options );


  cleanup:
  if(device_name_temp)
    oyFree_m_( device_name_temp );
  oyConfigs_Release( &devices );


  return error;
}


/** Function Configs_Modify
 *  @brief   oyCMMapi8_s monitor manipulation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
int            Configs_Modify        ( oyConfigs_s       * devices,
                                       oyOptions_s       * options )
{
  oyConfig_s * device = 0;
  oyOption_s * o = 0, * o_tmp = 0;
  oyRectangle_s * rect = 0;
  oyRectangle_s * r = 0;
  oyProfile_s * prof = 0;
  char * text = 0;
  int n = 0, i,
      error = !devices || !oyConfigs_Count( devices ),
      t_err = 0,
      has;
  const char * oprofile_name = 0,
             * device_name = 0;
  int rank = 0;
  const char * tmp = 0;


  /** 1. In case no option is provided or something fails, show a message. */
  if(!options || !oyOptions_Count( options ))
  {
    ConfigsUsage( (oyStruct_s*)options );
    return 0;
  }

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    rank += oyFilterRegistrationMatch( _api8.registration,
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
        oyMonitor_s * moni;
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( _api8.registration,
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

        moni = dispDeviceGetMonitor( device );

        /** 3.1.3 tell the "device_rectangle" in a oyRectangle_s */
        if(oyOptions_FindString( options, "device_rectangle", 0 ) ||
           oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          double x,y,w,h;
          has = 0;
          if(GetRectangleFromMonitor( moni, &x,&y,&w,&h))
          {
#if defined(__APPLE__) && !defined(qarz)
            if(oy_debug)
#endif
            WARNc1_S( "Could not obtain rectangle information for %s",
                      device_name );
          } else
          {
            rect = oyRectangle_NewWith( x,y,w,h, NULL );
            o = oyConfig_Find( device, "device_rectangle" );
            if(o)
              has = 1;
            else
              o = oyOption_FromRegistration( MONITOR_REGISTRATION OY_SLASH
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
              _msg( oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Try %s(_xxx) from %s",
                     OY_DBG_ARGS_,
                     oyOptions_FindString(options, "x_color_region_target", 0) ? 
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
                     XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE :
                     XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE,
#else
# ifdef qarz
                     "ColorSync" : "Base",
# else
                     "no XCM_X11 device":"no XCM_X11",
# endif
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
            o_tmp = oyOptions_Find( *oyConfig_GetOptions(device,"data"), "color_matrix."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                                    oyNAME_PATTERN );
             /* try to obtain already included EDID keys */
            if(!o_tmp)
            {
              oyOption_s * pandg = oyDeviceToChromaticity( device,
                                MONITOR_REGISTRATION OY_SLASH "color_matrix."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma" );
              if(pandg)
              {
                o_tmp = oyOption_Copy( pandg, NULL );
                oyOptions_MoveIn( *oyConfig_GetOptions(device,"data"), &pandg, -1 );
              }
            }

            if(!o_tmp)
            {
              oyOptions_SetFromString( &options,
                                     MONITOR_REGISTRATION OY_SLASH
                                     "edid",
                                     "yes", OY_CREATE_NEW );
              if(oy_debug)
                _msg( oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "CallingDeviceFromName_( %s )", OY_DBG_ARGS_, device_name );
              error = DeviceFromName_( device_name, options, &device );
              if(error)
              {
                _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                "Could not create device from name %s",
                     OY_DBG_ARGS_, device_name );
              }
              o_tmp = oyOptions_Find( *oyConfig_GetOptions(device,"data"), "color_matrix."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                                    oyNAME_PATTERN);
            }

            if(o_tmp)
            {
              oyOptions_s * opts = oyOptions_New(0),
                          * result = 0;
              int32_t icc_profile_flags = 0;
              oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags );
              oyOptions_SetFromInt( &opts, "///icc_profile_flags", icc_profile_flags, 0, OY_CREATE_NEW );
              oyOptions_MoveIn( opts, &o_tmp, -1 );
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
                _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
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
              if(error)
              {
                _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                      "Adding dscp tag gave error %s",
                      OY_DBG_ARGS_, text );
              }
              oyDeAllocateFunc_( text ); text = 0;
              t = oyConfig_FindString( device, "EDID_manufacturer", 0);
              if(!t)
              {
                t = oyConfig_FindString( device, "EDID_mnft", 0);
                if(!t)
                  error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t);
                else
                _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
            "Could not obtain \"EDID_manufacturer\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              } else
                error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t);
              t = oyConfig_FindString( device, "EDID_model", 0 );
              if(!t)
                _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                   "Could not obtain \"EDID_model\" from monitor device for %s",
                     OY_DBG_ARGS_, device_name );
              else
                error = oyProfile_AddTagText( prof, icSigDeviceModelDescTag, t);

              if(error)
              {
                _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                      "Adding info tags gave error",
                      OY_DBG_ARGS_ );
              }
              oyOptions_SetFromString( oyConfig_GetOptions(device,"backend_core"),
                                       MONITOR_REGISTRATION OY_SLASH
                                       "OPENICC_automatic_generated",
                                       "1", OY_CREATE_NEW );

              /* embed meta tag */
              oyOptions_SetFromString( &opts, "///key_prefix_required",
                                                "EDID_.OPENICC_",
                                                OY_CREATE_NEW );
              oyProfile_AddDevice( prof, device, opts );
              oyOptions_Release( &opts);

              data = oyProfile_GetMem( prof, &size, 0, oyAllocateFunc_ );
              header = (icHeader*) data;
              o_tmp = oyConfig_Find( device, "EDID_mnft" );
              t = oyConfig_FindString( device, "EDID_mnft", 0 );
              if(!t)
                _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
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
              o = oyOption_FromRegistration( MONITOR_REGISTRATION OY_SLASH
                                "icc_profile.fallback", 0 );
            error = -1;
          } else if(!has)
          {
            data = GetMonitorProfile( moni, flags, &size );

            if(data && size)
            {
              prof = oyProfile_FromMem( size, data, 0, 0 );
              free( data ); data = NULL; size = 0;
              if(has == 0)
              {
                const char * key = MONITOR_REGISTRATION OY_SLASH "icc_profile";
                if(oyOptions_FindString(options, "x_color_region_target", 0))
                  key = MONITOR_REGISTRATION OY_SLASH "icc_profile.x_color_region_target";
                o = oyOption_FromRegistration( key, 0 );
              }
            }
          }

          if(!o)
              o = oyOption_FromRegistration( MONITOR_REGISTRATION OY_SLASH
                                "icc_profile", 0 );

          if(prof)
          {
            t_err = oyOption_MoveInStruct( o, (oyStruct_s**) &prof );
            if(t_err > 0)
              error = t_err;
          }
          else if(!has)
          /** Warn and return issue on not found profile. */
          {
            _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
                     "Could not obtain %s(_xxx) information for %s",
                     OY_DBG_ARGS_,
                     oyOptions_FindString(options, "x_color_region_target", 0) ? 
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
                     XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE :
                     XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE,
#else
# ifdef qarz
                     "ColorSync" : "Base",
# else
                     "XCM_X11 device" : "XCM_X11 target",
# endif
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
          o = oyOptions_Find( *oyConfig_GetOptions(device,"data"), "device_rectangle",
                              oyNAME_PATTERN );
          r = (oyRectangle_s*) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );

          text = 0; tmp = 0;
      
          tmp = oyRectangle_Show( (oyRectangle_s*)r );
          STRING_ADD( text, tmp );
          oyOption_Release( &o );
          oyRectangle_Release( &r );

          o = oyOptions_Find( *oyConfig_GetOptions(device,"data"), "icc_profile",
                              oyNAME_PATTERN );

          if( o )
          {
            prof = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
            /*  oyProfile_GetFileName is very expensive. The function iterates
             *  over all on disk profiles until it finds the searched one. As 
             *  a alternative the internal description oyNAME_DESCRIPTION 
             *  (as done below, or the more technical profile ID can be printed.
             *  oyProfile_GetFileName( prof, 0 ); */
            tmp = oyProfile_GetText( prof, oyNAME_DESCRIPTION );

            STRING_ADD( text, "  \"" );
            if(tmp)
            {
              STRING_ADD( text, tmp );
            } else
            {
              tmp = oyProfile_GetFileName( prof, 0 );
              if(tmp)
              {
                if(oyStrrchr_( tmp, OY_SLASH_C ))
                  STRING_ADD( text, oyStrrchr_( tmp, OY_SLASH_C ) + 1 );
                else
                  STRING_ADD( text, tmp );
              }
            }
            STRING_ADD( text, "\"" );

            oyProfile_Release( &prof );
            oyOption_Release( &o );
          }

          if(error <= 0)
          {
            t_err = oyOptions_SetFromString( oyConfig_GetOptions(device,"data"),
                                         MONITOR_REGISTRATION OY_SLASH
                                         "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
            if(t_err > 0)
              error = t_err;
          }
          oyFree_m_( text );
        }

        /** 3.1.6 add the rank scheme to combine properties */
        if(error <= 0 && !oyConfig_GetRankMap(device))
          oyConfig_SetRankMap(device, _api8.rank_map );

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
        oyMonitor_s * moni;
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( _api8.registration,
                                          oyConfig_GetRegistration(device),
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        moni = dispDeviceGetMonitor( device );

        /** 3.1.2 get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );
        oprofile_name = oyOptions_FindString( options, "profile_name", 0 );

        error = !device_name || !oprofile_name;
        if(error >= 1)
          _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "The device_name/profile_name option is missed. Options:\n%s",
                  OY_DBG_ARGS_,
                  oyOptions_GetText( options, oyNAME_NICK )
                  );
        else
        {
          oyProfile_s * p = oyProfile_FromName( oprofile_name, 0, 0 );
          size_t size = oyProfile_GetSize( p, 0 );
          char * data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
          int gamma_only = oyOptions_FindString( options, "gamma_only", "yes" ) != NULL;
          const char * profile_fullname = oyProfile_GetFileName( p, -1 );

          fprintf(stderr, "%s [" CMM_NICK "] SetupMonitorCalibration: ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
          _msg(oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "command: setup on device_name: %s \"%s\" %lu %s",
                  OY_DBG_ARGS_, device_name, oprofile_name, size,
                  gamma_only?"only VCGT":"" );

          if(!gamma_only)
            error = SetupMonitorProfile( moni, profile_fullname, data, size );
          error = SetupMonitorCalibration( moni, profile_fullname, data, size );
          oyProfile_Release( &p );
        }
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
        oyMonitor_s * moni;
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( _api8.registration,
                                          oyConfig_GetRegistration(device),
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        moni = dispDeviceGetMonitor( device );

        /** 3.1.2 get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );
        oprofile_name = oyOptions_FindString( options, "profile_name", 0 );

        error = !device_name || !oprofile_name;
        if(error >= 1)
          _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "The device_name option is missed. Options:\n%s",
                  OY_DBG_ARGS_,
                  oyOptions_GetText( options, oyNAME_NICK )
                  );
        else
        {
          _msg(oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                  "command: unset on device_name: \"%s\"",
                  OY_DBG_ARGS_, device_name );
          error = UnsetMonitorProfile( moni );
        }

        oyConfig_Release( &device );
      }

      goto cleanup;
    }

    /** 3.5 "help" call; display a help text */
    if(error <= 0 &&
       oyOptions_FindString( options, "command", "help" ))
    {
      ConfigsUsage( (oyStruct_s*)options );

      goto cleanup;
    }
  }


  _msg(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  ConfigsUsage( (oyStruct_s*)options );


  cleanup:

  return error;
}

/** Function Config_Rank
 *  @brief   oyCMMapi8_s monitor check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/26
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
int                Config_Rank       ( oyConfig_s        * config )
{
  int error = !config,
      rank = 1;

  if(!config)
  {
    _msg(oyMSG_DBG, (oyStruct_s*)config, OY_DBG_FORMAT_ "\n "
                "No config argument provided.\n", OY_DBG_ARGS_ );
    return 0;
  }

  if(error <= 0)
  {
    /* evaluate a driver specific part of the options */
  }

  return rank;
}

const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name") == 0 ||
     strcmp(select,"help") == 0)
  {
    /* The "help" and "name" texts are identical, as the module contains only
     * one filter to provide help for. */
    return GetText(select,type,context);
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
    if(!_category)
    {
      STRING_ADD( _category, _("Color") );
      STRING_ADD( _category, _("/") );
      /* CMM: abbreviation for Color Matching Module */
      STRING_ADD( _category, _("Device") );
      STRING_ADD( _category, _("/") );
      STRING_ADD( _category, _("Monitor") );
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return _category;
    else
      return _category;
  } 
  return 0;
}
const char * _api8_ui_texts[] = {"name", "help", "device_class", "icc_profile_class", "category", "key_prefix", 0};

/** @brief    Monitor Device Node UI
 *
 * oyCMMapi8_s::ui implementation
 *
 *  The UI for Monitor devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/14 (Oyranos: 0.1.10)
 *  @date    2009/12/16
 */
oyCMMui_s_ _api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Color/Device/Monitor", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  Api8UiGetText,  /* oyCMMGetText_f getText */
  _api8_ui_texts, /* (const char**)texts */
  (oyCMMapiFilter_s*)&_api8 /* oyCMMapiFilter_s*parent */
};

oyIcon_s _api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"
};

/** @brief    Monitor Device Node
 *
 *  oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.13
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2010/12/09
 */
oyCMMapi8_s_ _api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  next_api,                  /**< next API */

  CMMapiInit,
  CMMapiReset,
  CMMMessageFuncSet,         /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */

  MONITOR_REGISTRATION,      /**< registration */
  CMM_VERSION,               /**< int32_t version[3] */
  CMM_API_VERSION,           /**< int32_t module_api[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  0,                         /**< oyPointer_s * runtime_context */

  Configs_FromPattern,       /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  Configs_Modify,            /**< oyConfigs_Modify_f oyConfigs_Modify */
  Config_Rank,               /**< oyConfig_Rank_f oyConfig_Rank */

  (oyCMMui_s*)&_api8_ui,     /**< device class UI name and help */
  &_api8_icon,               /**< device icon */

  NULL                       /**< oyRankMap ** rank_map */
};

/* MONITOR_REGISTRATION -------------------------------------------------*/


/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * GetText                 ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Oyranos " CMM_NICK);
    else
      return _("The window support module of Oyranos.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
            /* "The " CMM_NICK " module supports the generic device protocol." */
      return _("The " CMM_NICK " module supports the generic device protocol.");
    else
    {
      if(!_help_desc)
      {
        _help_desc = malloc( strlen(_help) + strlen(_help_list)
                    + strlen(_help_properties)
                    + strlen(_help_system_specific)
                    + strlen(_help_setup)
                    + strlen(_help_unset)
                    + strlen(_help_add_edid_to_icc) + 2);
        sprintf( _help_desc, "%s\n%s%s%s%s%s%s", _help, _help_list,
                 _help_properties, _help_system_specific,  _help_setup,
                 _help_unset, _help_add_edid_to_icc );
      }
      return _help_desc;
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
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  }
  return 0;
}
const char *_texts[5] = {"name","copyright","manufacturer","help",0};

/** @brief    Module Infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMM_s _cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.9",
  GetText, /* oyCMMinfoGetText_f get Text */
  (char**)_texts, /* texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & _api8,

  &_api8_icon,

  CMMinit,
  CMMreset
};

/**  @} *//* monitor_device */
