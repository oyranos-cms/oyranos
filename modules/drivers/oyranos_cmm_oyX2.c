/** @file oyranos_cmm_oyX2.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 driver backend for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/05/10
 */

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

#define CMM_NICK "oyX2"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/* OY_IMAGE_DISPLAY_REGISTRATION */
/* OYX22_MONITOR_REGISTRATION */

int                oyX2CMMInit       ( );
int            oyX2CMMMessageFuncSet ( oyMessage_f         message_func );
int            oyX2CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value );

/* OYX2_MONITOR_REGISTRATION -------------------------------------------------*/

#define OYX2_MONITOR_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.monitor.driver." CMM_NICK

oyMessage_f message = 0;

extern oyCMMapi8_s oyX2_api8;
oyRankPad oyX2_rank_map[];

/* --- implementations --- */

int                oyX2CMMInit       ( )
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

/** @func  oyX2CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX2CMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

/** @func  oyX2CMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX2CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                     OYX2_MONITOR_REGISTRATION OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void     oyX2ConfigsFromPatternUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN shall make shure our message will be visible. */
    message( oyMSG_WARN, options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"list\" will provide a list of available\n"
      " driver settings. The actual driver name can be found in option\n"
      " \"driver_name\". The call is as lightwight as possible.\n"
      " The option \"display_name\" is optional to pass the X11 display name\n"
      " and obtain a unfiltered result. It the way to get all monitors\n"
      " connected to a display.\n"
      " The option \"oyNAME_NAME\" returns a string containting geometry and\n"
      " if available, the profile name or size.\n"
      " The bidirectional option \"driver_rectangle\" will cause to\n"
      " additionally add display geometry information as a oyRectangle_s\n"
      " object.\n"
      " The bidirectional option \"icc_profile\" will add a oyProfile_s.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n"
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n"
      " string.\n"
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The option \"driver_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION above."
      " Informations are stored in the returned oyConfig_s::data member."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"properties\" will provide the drivers \n"
      " properties. Requires one driver identifier returned with the \n"
      " \"list\" option. The properties may cover following entries:\n"
      " - \"manufacturer\"\n"
      " - \"name\"\n"
      " - \"version\"\n"
      " - \"system_port\"\n"
      " - \"display_geometry\" (specific) x,y,widthxheight ,e.g."
      " \"0,0,1024x786\"\n"
      " \n"
      " One option \"driver_name\" will select the according X display.\n"
      " If not the backend will try to get this information from \n"
      " your \"DISPLAY\" environment variable or uses what the system\n"
      " provides. The \"driver_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one. Informations are\n"
      " stored in the returned oyConfig_s::backend_core member."
       );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"setup\" will setup the driver from a profile.\n"
      " The option \"driver_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"      " file name."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"unset\" will invalidate a profile of a driver.\n"
      " The option \"driver_name\" must be present, see \"list\" above.\n"
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"get\" will provide a oyProfile_s of the\n"
      " driver in a \"icc_profile\" option.\n"
      " The option \"driver_name\" must be present, see \"list\" above.\n"
      );

  return;
}

int          oyX2DriverFromName_     ( const char        * driver_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** driver,
                                       oyAlloc_f           allocateFunc )
{
  const char * value3 = 0;
  oyOption_s * o = 0;
  int error = !driver;

    value3 = oyOptions_FindString( options, "edid", 0 );

    if(!error)
    {
      char * manufacturer=0, *model=0, *serial=0, *host=0, *display_geometry=0,
           * system_port=0;
      oyBlob_s * edid = 0;

      if(!driver_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                "The \"driver_name\" argument is\n"
                " missed to select a appropriate driver for the"
                " \"properties\" call.", OY_DBG_ARGS_ );
        error = 1;
      }

      if(error <= 0)
        error = oyGetMonitorInfo_lib( driver_name,
                                      0, 0, 0,
                                      &display_geometry, &system_port,
                                      &host, 0, allocateFunc,
                                      (oyStruct_s*)options );

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  oyGetMonitorInfo_lib returned with %s; driver_name:"
                 " \"%s\"", OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 oyNoEmptyString_m_( driver_name ) );

      if(error <= 0)
      {
        if(!*driver)
          *driver = oyConfig_New( OYX2_MONITOR_REGISTRATION, 0 );
        error = !*driver;
        if(!error && driver_name)
        error = oyOptions_SetFromText( &(*driver)->backend_core,
                                       OYX2_MONITOR_REGISTRATION OY_SLASH "driver_name",
                                       driver_name, OY_CREATE_NEW );

        OPTIONS_ADD( (*driver)->backend_core, manufacturer )
        OPTIONS_ADD( (*driver)->backend_core, model )
        OPTIONS_ADD( (*driver)->backend_core, serial )
        OPTIONS_ADD( (*driver)->backend_core, display_geometry )
        OPTIONS_ADD( (*driver)->backend_core, system_port )
        OPTIONS_ADD( (*driver)->backend_core, host )
      }
    }

  return error;
}


/** Function oyX2Configs_FromPattern
 *  @brief   oyX2 oyCMMapi8_s Xorg monitors
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/28
 */
int            oyX2Configs_FromPattern (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
  oyConfigs_s * drivers = 0;
  oyConfig_s * driver = 0;
  oyOption_s * o = 0;
  oyRectangle_s * rect = 0;
  const oyRectangle_s * r = 0;
  oyProfile_s * p = 0;
  char ** texts = 0;
  char * text = 0,
       * driver_name_temp = 0;
  int texts_n = 0, i,
      error = !s;
  const char * odriver_name = 0,
             * oprofile_name = 0,
             * odisplay_name = 0,
             * driver_name = 0;
  int rank = oyFilterRegistrationMatch( oyX2_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  const char * tmp = 0;


  /** 1. In case no option is provided or something fails, show a message. */
  if(!options || !oyOptions_Count( options ))
  {
    oyX2ConfigsFromPatternUsage( (oyStruct_s*)options );
    return 0;
  }

  if(rank && error <= 0)
  {
    drivers = oyConfigs_New(0);


    /** 2. obtain a proper driver_name */
    odisplay_name = oyOptions_FindString( options, "display_name", 0 );
    odriver_name = oyOptions_FindString( options, "driver_name", 0 );
    /*message(oyMSG_WARN, (oyStruct_s*)options, "list: %s", value2);*/

    if(odisplay_name && odisplay_name[0])
      driver_name = odisplay_name;
    else if(odriver_name && odriver_name[0])
      driver_name = odriver_name;
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

      driver_name_temp = oyStringCopy_( tmp, oyAllocateFunc_ );
      if(driver_name_temp &&
         (text = strchr(driver_name_temp,':')) != 0)
        if( (text = strchr(driver_name_temp, '.')) != 0 )
          text[0] = '\000';

      driver_name = driver_name_temp;
#endif
      text = 0;
    }

    /** 3.  handle the actual call */
    /** 3.1 "list" call */
    if(oyOptions_FindString( options, "list", 0 ))
    {
      texts_n = oyGetAllScreenNames( driver_name, &texts, allocateFunc );

      /** 3.1.1 iterate over all requested drivers */
      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odriver_name && strcmp(odriver_name, texts[i]) != 0)
          continue;

        driver = oyConfig_New( OYX2_MONITOR_REGISTRATION, 0 );
        error = !driver;

        /** 3.1.2 tell the "driver_name" */
        if(error <= 0)
        error = oyOptions_SetFromText( &driver->backend_core,
                                       OYX2_MONITOR_REGISTRATION OY_SLASH "driver_name",
                                       texts[i], OY_CREATE_NEW );

        /** 3.1.3 tell the "driver_rectangle" in a oyRectangle_s */
        if(oyOptions_FindString( options, "driver_rectangle", 0 ) ||
           oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          rect = oyX1Rectangle_FromDevice( texts[i] );
          if(!rect)
          {
            WARNc1_S("Could not obtain rectangle information for %s", texts[i]);
          } else
          {
            o = oyOption_New( OYX2_MONITOR_REGISTRATION OY_SLASH "driver_rectangle", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &rect );
            oyOptions_MoveIn( driver->data, &o, -1 );
          }
        }

        /** 3.1.4 tell the "icc_profile" in a oyProfile_s */
        if( oyOptions_FindString( options, "icc_profile", 0 ) ||
            oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 0;
          char * data = oyX2GetMonitorProfile( texts[i], &size, allocateFunc );

          
          /** Warn and return issue on not found profile. */
          if(!size || !data)
          {
            message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
            "Could not obtain _ICC_PROFILE(_xxx) information for ",OY_DBG_ARGS_,
                     "%s", texts[i]);
            error = -1;
          } else
          {
            p = oyProfile_FromMem( size, data, 0, 0 );
            o = oyOption_New( OYX2_MONITOR_REGISTRATION OY_SLASH "icc_profile",
                              0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( driver->data, &o, -1 );
            free( data );
          }
        }

        /** 3.1.5 contruct a oyNAME_NAME string */
        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          o = oyOptions_Find( driver->data, "driver_rectangle" );
          r = (oyRectangle_s*) o->value->oy_struct;

          text = 0; tmp = 0;
      
          tmp = oyRectangle_Show( (oyRectangle_s*)r );
          STRING_ADD( text, tmp );
          oyOption_Release( &o );

          o = oyOptions_Find( driver->data, "icc_profile" );

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
          error = oyOptions_SetFromText( &driver->data,
                                         OYX2_MONITOR_REGISTRATION OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          oyFree_m_( text );
        }


        /** 3.1.6 add the rank scheme to combine properties */
        if(error <= 0)
          driver->rank_map = oyRankMapCopy( oyX2_rank_map,
                                            driver->oy_->allocateFunc_ );

        oyConfigs_MoveIn( drivers, &driver, -1 );
      }

      if(error <= 0)
        *s = drivers;

      oyStringListRelease_( &texts, texts_n, free );

      goto cleanup;

    } else


    /** 3.2 "properties" call; provide extensive infos for the DB entry */
    if(oyOptions_FindString( options, "properties", 0 ))
    {
      texts_n = oyGetAllScreenNames( driver_name, &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odriver_name && strcmp(odriver_name, texts[i]) != 0)
          continue;

        driver = oyConfig_New( OYX2_MONITOR_REGISTRATION, 0 );
        error = !driver;

        if(error <= 0)
        error = oyOptions_SetFromText( &driver->backend_core,
                                       OYX2_MONITOR_REGISTRATION OY_SLASH "driver_name",
                                       texts[i], OY_CREATE_NEW );

        /** 3.2.1 add properties */
        error = oyX2DriverFromName_( texts[i], options, &driver,
                                     allocateFunc );


        /** 3.2.2 add the rank map to wight properties for ranking in the DB */
        if(error <= 0 && driver)
          driver->rank_map = oyRankMapCopy( oyX2_rank_map,
                                            driver->oy_->allocateFunc_);
        oyConfigs_MoveIn( drivers, &driver, -1 );
      }

      if(error <= 0)
        *s = drivers;

      oyStringListRelease_( &texts, texts_n, free );

      goto cleanup;

    } else

    /** 3.3 "setup" call; bring a profile to the driver */
    if(error <= 0 &&
       oyOptions_FindString( options, "setup", 0 ))
    {
      oprofile_name = oyOptions_FindString( options, "profile_name", 0 );
      error = !odriver_name || !oprofile_name;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "The driver_name/profile_name option is missed. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX2MonitorProfileSetup( odriver_name, oprofile_name );

      goto cleanup;

    } else

    /** 3.4 "unset" call; clear a profile from a driver */
    if(error <= 0 &&
       oyOptions_FindString( options, "unset", 0 ))
    {
      error = !odriver_name;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "The driver_name option is missed. Options:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX2MonitorProfileUnset( odriver_name );

      goto cleanup;
    }
  }


  message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  oyX2ConfigsFromPatternUsage( (oyStruct_s*)options );


  cleanup:
  if(driver_name_temp)
    oyFree_m_( driver_name_temp );


  return error;
}

/** Function oyX2Config_Check
 *  @brief   oyX2 oyCMMapi8_s Xorg monitor check
 *
 *  @param[in]     config              the monitor driver configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/26
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
int            oyX2Config_Check      ( oyConfig_s        * config )
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

/** @instance oyX2_rank_map
 *  @brief    oyRankPad map for mapping driver to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/05/10
 */
oyRankPad oyX2_rank_map[] = {
  {"driver_name", 2, -1, 0},           /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for driver properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"system_port", 2, 0, 0},            /**< good to match */
  {"display_geometry", 3, -1, 0},      /**< important to match */
  {0,0,0,0}                            /**< end of list */
};

/** @instance oyX2_api8
 *  @brief    oyX2 oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/05/10
 */
oyCMMapi8_s oyX2_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  (oyCMMapi_s*) 0, /**< next */

  oyX2CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  oyX2CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  oyX2CMMCanHandle,          /**< oyCMMCanHandle_f oyCMMCanHandle */

  OYX2_MONITOR_REGISTRATION, /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  oyX2Configs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  oyX2Config_Check,          /**< oyConfig_Check_f oyConfig_Check */
  oyX2_rank_map              /**< oyRankPad ** rank_map */
};

/* OYX2_MONITOR_REGISTRATION -------------------------------------------------*/


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/05/10
 */
const char * oyX2GetText             ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos X11");
    else
      return _("The window settings support backend of Oyranos.");
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

/** @instance oyX2_cmm_module
 *  @brief    oyX2 module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMMInfo_s oyX2_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.2",
  oyX2GetText, /* oyCMMInfoGetText_f */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyX2_api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

