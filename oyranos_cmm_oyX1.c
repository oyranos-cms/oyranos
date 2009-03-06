/** @file oyranos_cmm_oyX1.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 backend for Oyranos
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
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.monitor." CMM_NICK

oyMessage_f message = 0;

extern oyCMMapi8_s oyX1_api8;
oyRankPad oyX1_rank_map[];

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

/** @func  oyX1CMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX1CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void     oyX1ConfigsFromPatternUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN should make shure our message is visible. */
    message( oyMSG_WARN, options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"list\" will provide a list of available\n"
      " devices. The actual device name can be found in option\n"
      " \"device_name\". The call is as lightwight as possible.\n"
      " The option \"display_name\" is optional to pass the X11 display name\n"
      " and obtain a unfiltered result.\n"
      " The option \"oyNAME_NAME\" returns a string containting geometry and\n"
      " if available, the profile name or size.\n"
      " The bidirectional option \"display_geometry\" will cause to\n"
      " additionally add display geometry information as a oyRegion_s\n"
      " object.\n"
      " The bidirectional option \"icc_profile\" will add a oyProfile_s.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n"
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n"
      " string.\n"
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The bidirectional optional \"edid\" (specific) key word will\n"
      " additionally add the EDID information inside a oyBlob_s struct.\n"
      " The option \"device_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION above."
      " Informations are stored in the returned oyConfig_s::data member."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"properties\" will provide the devices \n"
      " properties. Requires one device identifier returned with the \n"
      " \"list\" option. The properties may cover following entries:\n"
      " - \"manufacturer\"\n"
      " - \"model\"\n"
      " - \"serial\"\n"
      " - \"host\"\n"
      " - \"system_port\"\n"
      " - \"display_geometry\" (specific) x,y,widthxheight ,e.g."
      " \"0,0,1024x786\"\n"
      " \n"
      " One option \"device_name\" will select the according X display.\n"
      " If not the backend will try to get this information from \n"
      " your \"DISPLAY\" environment variable or uses what the system\n"
      " provides. The \"device_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one. Informations are\n"
      " stored in the returned oyConfig_s::backend_core member."
       );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"setup\" will setup the device from a profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"      " file name."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"unset\" will invalidate a profile of a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"get\" will provide a oyProfile_s of the\n"
      " device in a \"icc_profile\" option.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );

  return;
}

int          oyX1DeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device,
                                       oyAlloc_f           allocateFunc )
{
  const char * value3 = 0;
  oyOption_s * o = 0;
  int error = !device;

    value3 = oyOptions_FindString( options, "edid", 0 );

    if(!error)
    {
      char * manufacturer=0, *model=0, *serial=0, *host=0, *display_geometry=0,
           * system_port=0;
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
                                      &host, value3 ? &edid : 0, allocateFunc,
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
          *device = oyConfig_New( CMM_BASE_REG, 0 );
        error = !*device;
        if(!error && device_name)
        error = oyOptions_SetFromText( &(*device)->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       device_name, OY_CREATE_NEW );

        OPTIONS_ADD( (*device)->backend_core, manufacturer )
        OPTIONS_ADD( (*device)->backend_core, model )
        OPTIONS_ADD( (*device)->backend_core, serial )
        OPTIONS_ADD( (*device)->backend_core, display_geometry )
        OPTIONS_ADD( (*device)->backend_core, system_port )
        OPTIONS_ADD( (*device)->backend_core, host )
        if(!error && edid)
        {
          o = oyOption_New( CMM_BASE_REG OY_SLASH "edid", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, edid->ptr, edid->size );
          if(!error)
            oyOptions_MoveIn( (*device)->data, &o, -1 );
          oyBlob_Release( &edid );
        }
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
  oyOption_s * o = 0;
  oyRegion_s * rect = 0;
  const oyRegion_s * r = 0;
  oyProfile_s * p = 0;
  char ** texts = 0;
  char * text = 0,
       * device_name_temp = 0;
  int texts_n = 0, i,
      error = !s;
  const char * value1 = 0,
             * value2 = 0,
             * value3 = 0,
             * value4 = 0,
             * display_name = 0,
             * device_name = 0;
  int rank = oyFilterRegistrationMatch( oyX1_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  static char * num = 0;
  const char * tmp = 0;

  if(!num)
    oyAllocHelper_m_( num, char, 80, 0, error = 1; return error );

  if(!options || !oyOptions_Count( options ))
  {
    /** oyMSG_WARN should make shure our message is visible. */
    oyX1ConfigsFromPatternUsage( (oyStruct_s*)options );
    return 0;
  }

  if(rank && error <= 0)
  {
    devices = oyConfigs_New(0);

    display_name = oyOptions_FindString( options, "display_name", 0 );
    value1 = oyOptions_FindString( options, "device_name", 0 );
    /*message(oyMSG_WARN, (oyStruct_s*)options, "list: %s", value2);*/

    if(display_name && display_name[0])
      device_name = display_name;
    else if(value1 && value1[0])
      device_name = value1;
    else
    {
      tmp = getenv("DISPLAY");
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
      text = 0;
    }

    value2 = oyOptions_FindString( options, "list", 0 );
    value3 = oyOptions_FindString( options, "display_geometry", 0 );
    if(value2)
    {
      texts_n = oyGetAllScreenNames( device_name, &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(value1 && strcmp(value1, texts[i]) != 0)
          continue;

        device = oyConfig_New( CMM_BASE_REG, 0 );
        error = !device;

        if(error <= 0)
        error = oyOptions_SetFromText( &device->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );

        if(value3 || oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          rect = oyX1Region_FromDevice( texts[i] );
          if(!rect)
          {
            WARNc1_S("Could not obtain region information for %s", texts[i]);
          } else
          {
            o = oyOption_New( CMM_BASE_REG OY_SLASH "display_geometry", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &rect );
            oyOptions_MoveIn( device->data, &o, -1 );
          }
        }

        value4 = oyOptions_FindString( options, "icc_profile", 0 );
        if(value4 || oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 0;
          char * data = oyX1GetMonitorProfile( texts[i], &size, allocateFunc );

          
          /** Warn and return issue on not found profile. */
          if(!size || !data)
          {
            WARNc1_S("\n  Could not obtain _ICC_PROFILE(_xxx) information for "
                     "%s", texts[i]);
            error = -1;
          } else
          {
            p = oyProfile_FromMem( size, data, 0, 0 );
            o = oyOption_New( CMM_BASE_REG OY_SLASH "icc_profile", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( device->data, &o, -1 );
            free( data );
          }
        }

        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          o = oyOptions_Find( device->data, "display_geometry" );
          r = (oyRegion_s*) o->value->oy_struct;

          num[0] = 0; text = 0; tmp = 0;
          sprintf( num, "%d+%d:%dx%d", (int)r->x, (int)r->y,
                                       (int)r->width, (int)r->height );
      
          tmp = oyRegion_Show( (oyRegion_s*)r );
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
                                         CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          oyFree_m_( text );
        }

        if(error <= 0)
          device->rank_map = oyRankMapCopy( oyX1_rank_map,
                                               device->oy_->allocateFunc_ );

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        *s = devices;

      oyStringListRelease_( &texts, texts_n, free );

      goto cleanup;
    }

    value2 = oyOptions_FindString( options, "properties", 0 );
    if(value2)
    {
      texts_n = oyGetAllScreenNames( device_name, &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(value1 && strcmp(value1, texts[i]) != 0)
          continue;

        device = oyConfig_New( CMM_BASE_REG, 0 );
        error = !device;

        if(error <= 0)
        error = oyOptions_SetFromText( &device->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );

        error = oyX1DeviceFromName_( texts[i], options, &device,
                                         allocateFunc );

        if(error <= 0 && device)
          device->rank_map = oyRankMapCopy( oyX1_rank_map,
                                                device->oy_->allocateFunc_);
        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        *s = devices;

      oyStringListRelease_( &texts, texts_n, free );

      goto cleanup;
    }

    value2 = oyOptions_FindString( options, "setup", 0 );
    value3 = oyOptions_FindString( options, "profile_name", 0 );
    if(error <= 0 && value2)
    {
      error = !value1 || !value3;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "The device_name/profile_name option is missed. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX1MonitorProfileSetup( value1, value3 );

      goto cleanup;
    }

    value2 = oyOptions_FindString( options, "unset", 0 );
    if(error <= 0 && value2)
    {
      error = !value1;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "The device_name option is missed. Options:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX1MonitorProfileUnset( value1 );

      goto cleanup;
    }
  }

  message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  oyX1ConfigsFromPatternUsage( (oyStruct_s*)options );

  cleanup:
  if(device_name_temp)
    oyFree_m_( device_name_temp );


  return error;
}

/** Function oyX1Config_Check
 *  @brief   oyX1 oyCMMapi8_s Xorg monitor check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/26
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
int            oyX1Config_Check      ( oyConfig_s        * config )
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
  {"device_name", 2, -1, 0},       /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {"display_geometry", 3, -1, 0},       /**< important to match */
  {0,0,0,0}                            /**< end of list */
};

/** @instance oyX1_api8
 *  @brief    oyX1 oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/19
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 */
oyCMMapi8_s oyX1_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  oyX1CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  oyX1CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  oyX1CMMCanHandle,          /**< oyCMMCanHandle_f oyCMMCanHandle */

  CMM_BASE_REG,              /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  oyX1Configs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  oyX1Config_Check,          /**< oyConfig_Check_f oyConfig_Check */
  oyX1_rank_map              /**< oyRankPad ** rank_map */
};



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
      return _("The window support backend of Oyranos.");
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
      return _("Copyright (c) 2005-2008 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}

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
  "0.2",
  oyX1GetText, /* oyCMMInfoGetText_f */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyX1_api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

