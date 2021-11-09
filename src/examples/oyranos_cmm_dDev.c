/** @file oyranos_cmm_dDev.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos dummy device module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/02/09
 *
 *
 *
 *  The library name must cover the four char nick + "_cmm_module", which is
 *  "dDev_cmm_module" for this example module. E.g.:
 *  $(CC) $(CFLAGS) -shared oyranos_cmm_dDev.c -o liboyranos_dDev_cmm_module.so
 */

#include "oyCMM_s.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMui_s_.h"
#include "oyProfile_s.h"

#include "oyranos_cmm.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "dDev"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "device" OY_SLASH "config.icc_profile.dummy." CMM_NICK
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( dDev, CMMInit )
#define CMMReset                catCMMfunc( dDev, CMMReset )
#define CMMallocateFunc         catCMMfunc( dDev, CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( dDev, CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( dDev, CMMMessageFuncSet )
#define ConfigsUsage            catCMMfunc( dDev, ConfigsUsage )
#define DeviceFromName_         catCMMfunc( dDev, DeviceFromName_ )
#define GetDevices              catCMMfunc( dDev, GetDevices )
#define _api8                   catCMMfunc( dDev, _api8 )
#define _rank_map               catCMMfunc( dDev, _rank_map )
#define Configs_FromPattern     catCMMfunc( dDev, Configs_FromPattern )
#define Configs_Modify          catCMMfunc( dDev, Configs_Modify )
#define Config_Check            catCMMfunc( dDev, Config_Check )
#define GetText                 catCMMfunc( dDev, GetText )
#define _texts                  catCMMfunc( dDev, _texts )
#define _cmm_module             catCMMfunc( dDev, _cmm_module )
#define _api8_ui                catCMMfunc( dDev, _api8_ui )
#define Api8UiGetText           catCMMfunc( dDev, Api8UiGetText )
#define _api8_ui_texts          catCMMfunc( dDev, _api8_ui_texts )
#define _api8_icon              catCMMfunc( dDev, _api8_icon )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x
#define STRING_ADD(a,b) strcpy( &a[strlen(a)], b )

const char * GetText                 ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

oyMessage_f message = 0;

extern oyCMMapi8_s_ _api8;
oyRankMap _rank_map[];

/* --- implementations --- */

int                CMMInit       ( )
{
  int error = 0;
  return error;
}

int                CMMReset      ( )
{
  int error = 0;
  return error;
}



oyPointer          CMMallocateFunc   ( size_t              size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               CMMdeallocateFunc ( oyPointer           mem )
{
  if(mem)
    free(mem);
}

/** @func  CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int            CMMMessageFuncSet     ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromString( opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void         ConfigsUsage            ( oyStruct_s        * options )
{
    /** oyMSG_WARN should make shure our message is visible. */
    message( oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
             _DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=list\" will provide a list of\n"
      " available devices. The actual device name can be found in\n"
      " option \"device_name\". The call is as lightwight as possible.\n"
      " The option \"oyNAME_NAME\" returns a short string containting\n"
      " informations and if available, the profile name or size.\n"
      " The bidirectional option \"icc_profile\" will add a oyProfile_s.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n" 
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n" 
      " string.\n" 
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The option \"device_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION above."
      " Informations are stored in the returned oyConfig_s::data member."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=properties\" will provide the\n"
      " devices properties. Requires a device_name identifier\n"
      " returned with the \"list\" option.\n"
      " The properties may cover following entries:\n"
      " - \"manufacturer\"\n"
      " - \"model\"\n"
      " - \"serial\"\n"
      " - \"host\"\n"
      " - \"system_port\"\n"
      " - and more as appropriate for the device ...\n"
      " \n"
      " One option \"device_name\" will select the according device.\n"
      " If not the module might be able to get this information from \n"
      " elsewhere, but this is optional.\n"
      " The \"device_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one. Informations are\n" 
      " stored in the returned oyConfig_s::backend_core member."
       );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=setup\" will setup the device\n"
      " from a profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"
      " file name."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=unset\" will invalidate a profile of\n"
      " a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );

  return;
}

int              DeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device,
                                       oyAlloc_f           allocateFunc OY_UNUSED )
{
  oyOption_s * o = 0;
  int error = !device;

    if(!error)
    {
      char * manufacturer=0, *model=0, *serial=0, *host=0, *system_port=0;
      oyBlob_s * data_blob = 0;

      if(!device_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", _DBG_ARGS_ );
        error = 1;
        return error;
      }

      /* now get the data from somewhere*/
      if(strcmp(device_name, "dDev_1") == 0)
      {
        manufacturer = "People_1";
        model = "people-one";
        serial = "11";
        system_port = "usb-01";
      } else if(strcmp(device_name, "dDev_2") == 0)
      {
        manufacturer = "Village_2";
        model = "yard-two";
        serial = "22";
        system_port = "usb-02";
      }

      host = "localhost";

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 _DBG_FORMAT_ "Could not complete \"properties\" call.\n"
                 " oyGetMonitorInfo_lib returned with %s; device_name:"
                 " \"%s\"", _DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 device_name ? device_name : "" );

      if(error <= 0)
      {
        if(!*device)
          *device = oyConfig_FromRegistration( CMM_BASE_REG, 0 );
        error = !*device;
        if(!error && device_name)
        error = oyOptions_SetFromString( oyConfig_GetOptions(*device,"backend_core"),
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       device_name, OY_CREATE_NEW );

        OPTIONS_ADD( oyConfig_GetOptions(*device,"backend_core"), manufacturer )
        OPTIONS_ADD( oyConfig_GetOptions(*device,"backend_core"), model )
        OPTIONS_ADD( oyConfig_GetOptions(*device,"backend_core"), serial )
        OPTIONS_ADD( oyConfig_GetOptions(*device,"backend_core"), system_port )
        OPTIONS_ADD( oyConfig_GetOptions(*device,"backend_core"), host )
        if(!error && data_blob)
        {
          o = oyOption_FromRegistration( CMM_BASE_REG OY_SLASH "data_blob", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, oyBlob_GetPointer(data_blob),
                                        oyBlob_GetSize( data_blob) );
          if(!error)
            oyOptions_MoveIn( *oyConfig_GetOptions(*device,"data"), &o, -1 );
          oyBlob_Release( &data_blob );
        }
      }
    }

  return error;
}

int     GetDevices                   ( char            *** list,
                                       oyAlloc_f           allocateFunc )
{
  int len = sizeof(char*) * 3;
  char ** texts = allocateFunc( len );

  memset( texts, 0, len );
  texts[0] = allocateFunc(24); sprintf( texts[0], "dDev_1" );
  texts[1] = allocateFunc(24); sprintf( texts[1], "dDev_2" );

  *list = texts;
  return 2;
}

/** Function Configs_Modify
 *  @brief   CMM_NICK oyCMMapi8_s dummy devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/26
 */
int              Configs_Modify      ( oyConfigs_s       * devices,
                                       oyOptions_s       * options )
{
  oyConfig_s * device = 0;
  oyOption_s * o = 0;
  oyProfile_s * p = 0;
  char ** texts = 0;
  char * text = 0;
  int i,n,
      error = !oyConfigs_Count( devices );
  const char * device_name = 0;
  int rank = 0;
  static char * num = 0;
  const char * tmp = 0;

  if(!num)
    num = malloc( 80 );

  if(!options || !oyOptions_Count( options ))
  {
    /** oyMSG_WARN should make shure our message is visible. */
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
    /** 3.1 "list" call section */
    if(oyOptions_FindString( options, "command", "list" ))
    {
      n = oyConfigs_Count( devices );

      /** 3.1.1 iterate over all provided devices */
      for( i = 0; i < n; ++i )
      {
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

        /** 3.1.3 tell the "icc_profile" in a oyProfile_s */
        if(oyOptions_FindString( options, "icc_profile", 0 ) ||
           oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 6;
          const char * data = "dummy";

          /* In case the devices do not support network transparent ICC profile
           * setup, then Oyranos uses the DB stored profile, e.g.
           * @see oyDeviceProfileFromDB() + oyProfile_FromName()
           * Then the module will only report about color related options to
           * guide Oyranos core in selecting the correct profiles, matching
           * to the device and its driver settings.
           *
           * One the opposite the Xorg-"oyX1" module puts the profile in 
           * X server. This shall be included in the Oyranos device options 
           * on request.
           * If it is not found, then a empty option of type oyVAL_STRUCT is to
           * be included to tell Oyranos that option is understoof. 
           * The module needs as well to report a issue to inform Oyranos, 
           * as seen below.
           */

          /** Warn and return issue on not found profile. */
          if(!size || !data)
          {
            message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "Could not obtain icc_profile information for %s",
                _DBG_ARGS_, texts[i]);
            /* Show the "icc_profile" option is understood. */
            p = 0;
            error = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
            error = -1;
          } else
          {
            p = oyProfile_FromMem( size, (const oyPointer)data, 0, 0 );
            o = oyOption_FromRegistration( CMM_BASE_REG OY_SLASH "icc_profile",
                                           0 );
            error = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( *oyConfig_GetOptions(device,"data"), &o, -1 );
          }
        }

        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          text = calloc( 4096, sizeof(char) );

          o = oyOptions_Find( *oyConfig_GetOptions(device,"data"),
                              "icc_profile", oyNAME_PATTERN );

          p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
          if( p )
          {
            /* our dummy profile will certainly fail */
            tmp = oyProfile_GetFileName( p, 0 );

            STRING_ADD( text, "  " );
            if(strrchr( tmp, OY_SLASH_C ))
              STRING_ADD( text, strrchr( tmp, OY_SLASH_C ) + 1 );
            else
              STRING_ADD( text, tmp );

            oyProfile_Release( &p );
          }

          if(error <= 0)
          error = oyOptions_SetFromString( oyConfig_GetOptions(device,"data"),
                                         CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          free( text );
        }

        if(error <= 0 && !oyConfig_GetRankMap(device))
          oyConfig_SetRankMap(device, _rank_map );

        oyConfig_Release( &device );
      }

      return error;
    }

    /* "properties" call section; provide extensive infos for the DB entry */
    if(oyOptions_FindString( options, "command", "properties" ))
    {
      n = oyConfigs_Count( devices );

      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( _api8.registration,
                                          oyConfig_GetRegistration(device),
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        /** get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );

        error = oyOptions_SetFromString( oyConfig_GetOptions(device,"data"),
                                         CMM_BASE_REG OY_SLASH "port",
                                         "8", OY_CREATE_NEW );
        oyConfig_Release( &device );
      }

      return error;
    }

    /* "setup" call section */
    if(oyOptions_FindString( options, "command", "setup" ))
    {
      n = oyConfigs_Count( devices );

      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( _api8.registration,
                                          oyConfig_GetRegistration(device),
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        /** get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );

        if(!device_name || !oyOptions_FindString( options, "profile_name", 0 ))
        {
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "The device_name/profile_name option is missed. Options:\n%s",
                _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
          error = 1;
        } else
          error = 0; /* doSetup */

        oyConfig_Release( &device );
      }
      return error;
    }

    /* "unset" call section */
    if(oyOptions_FindString( options, "command", "unset" ))
    {
      n = oyConfigs_Count( devices );

      for( i = 0; i < n; ++i )
      {
        device = oyConfigs_Get( devices, i );
        rank = oyFilterRegistrationMatch( _api8.registration,
                                          oyConfig_GetRegistration(device),
                                          oyOBJECT_CMM_API8_S );
        if(!rank)
        {
          oyConfig_Release( &device );
          continue;
        }

        /** get the "device_name" */
        if(error <= 0)
        device_name = oyConfig_FindString( device, "device_name", 0 );

        if(!device_name)
        {
          message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "The device_name property is missed. Options:\n%s",
                _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
          error = 1;
        } else
          error = 0; /* doUnset */

        oyConfig_Release( &device );
      }
      return error;
    }
  }


  if(oyOptions_FindString( options, "command", "help" ))
  {
    ConfigsUsage( (oyStruct_s*)options );

    return error;
  }


  /* not to be reached section, e.g. warning */
  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  ConfigsUsage( (oyStruct_s*)options );

  return error;
}

/** Function Configs_FromPattern
 *  @brief   CMM_NICK oyCMMapi8_s dummy devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int              Configs_FromPattern ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
  oyConfigs_s * devices = 0;
  oyConfig_s * device = 0;
  char ** texts = 0;
  int texts_n = 0, i,
      error = !s;
  const char * device_name = 0;
  int rank = oyFilterRegistrationMatch( _api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  static char * num = 0;

  if(!num)
    num = malloc( 80 );

  if(!options || !oyOptions_Count( options ))
  {
    /** oyMSG_WARN should make shure our message is visible. */
    ConfigsUsage( (oyStruct_s*)options );
    return 0;
  }

  if(rank && error <= 0)
  {
    devices = oyConfigs_New(0);

    /* "list" call section */
    device_name = oyOptions_FindString( options, "device_name", 0 );
    {
      texts_n = GetDevices( &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(device_name && strcmp(device_name, texts[i]) != 0)
          continue;

        device = oyConfig_FromRegistration( CMM_BASE_REG, 0 );
        error = !device;

        if(error <= 0)
        error=oyOptions_SetFromString( oyConfig_GetOptions(device,"backend_core"),
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );



        oyConfigs_MoveIn( devices, &device, -1 );
      }

      Configs_Modify( devices, options );

      if(error <= 0)
        *s = devices;

      return error;
    }
  }


  if(oyOptions_FindString( options, "command", "help" ))
  {
    ConfigsUsage( (oyStruct_s*)options );

    return error;
  }


  /* not to be reached section, e.g. warning */
  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  ConfigsUsage( (oyStruct_s*)options );

  return error;
}

/** Function Config_Check
 *  @brief   CMM_NICK oyCMMapi8_s device check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int                Config_Check      ( oyConfig_s        * config )
{
  int error = !config,
      rank = 1;

  if(!config)
  {
    message(oyMSG_DBG, (oyStruct_s*)config, _DBG_FORMAT_ "\n "
                "No config argument provided.\n", _DBG_ARGS_ );
    return 0;
  }

  if(error <= 0)
  {
    /* evaluate a driver specific part of the options */
  }

  return rank;
}

/** @instance _rank_map
 *  @brief    oyRankMap map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyRankMap _rank_map[] = {
  {"device_name", 2, -1, 0},           /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {0,0,0,0}                            /**< end of list */
};

const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0 ||
     strcmp(select,"help") == 0)
  {
    /* The "help" and "name" texts are identical, as the module contains only
     * one filter to provide help for. */
    return GetText(select, type, context);
  }
  /* provide a useful device name */
  else if(strcmp(select, "device_class")==0)
  {
        if(type == oyNAME_NICK)
            return _("Example");
        else if(type == oyNAME_NAME)
            return _("Example Device");
        else
            return _("Example Devices, for testing and learning purposes only.");
  } 
  else if(strcmp(select,"category") == 0)
  {
    if(!category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Color"),_("Device"),_("Example"),0};
      int len =  strlen(i18n[0]) + strlen(i18n[1]) + strlen(i18n[2]);
      category = (char*)malloc( len + 64 );
      if(category)
        sprintf( category,"%s/%s/%s", i18n[0], i18n[1], i18n[2] );
      else
        message(oyMSG_WARN, (oyStruct_s *) 0, _DBG_FORMAT_ "\n " "Could not allocate enough memory.", _DBG_ARGS_);
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
/* All possible "select" arguments for Api8UiGetText(). */
const char * _api8_ui_texts[] = {"name", "help", "device_class", "category", 0};

/** @instance _api8_ui
 *  @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI parts for example devices.
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/11
 *  @since   2009/12/22 (Oyranos: 0.1.10)
 */
oyCMMui_s_   _api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_VERSION,                         /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Color/Device/Example", /* category */
  0,   /* const char * options */

  0,    /* oyCMMuiGet_f oyCMMuiGet */

  Api8UiGetText,  /* oyCMMGetText_f getText */
  _api8_ui_texts, /* (const char**)texts */
  (oyCMMapiFilter_s*)&_api8 /* oyCMMapiFilter_s*parent */
};

oyIcon_s _api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"
};

/** @instance _api8
 *  @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/11
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 */
oyCMMapi8_s_ _api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  CMMInit,                   /**< oyCMMInit_f      oyCMMInit */
  CMMReset,                  /* oyCMMReset_f       oyCMMReset */
  CMMMessageFuncSet,         /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */

  CMM_BASE_REG,              /**< registration */
  CMM_VERSION,               /**< int32_t version[3] */
  CMM_VERSION,               /**< int32_t module_api[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  0,                         /**< oyPointer_s * runtime_context */

  Configs_FromPattern,       /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  Configs_Modify,            /**< oyConfigs_Modify_f oyConfigs_Modify */
  Config_Check,              /**< oyConfig_Check_f oyConfig_Check */

  (oyCMMui_s*)&_api8_ui,     /**< device class UI name and help */
  &_api8_icon,               /**< device icon */

  _rank_map                  /**< oyRankMap ** rank_map */
};




/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
const char * GetText                 ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos Dummy");
    else
      return _("The dummy useless module of Oyranos.");
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
      return _("MIT");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2009 Kai-Uwe Behrmann; MIT");
    else
      return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("My filter introduction.");
    else
      return _("All the small details for using this module.");
  }
  return 0;
}
/** All possible select arguments for GetText(). */
const char * _texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance _cmm_module
 *  @brief    CMM_NICK module infos
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/11
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 */
oyCMM_s _cmm_module = {

  oyOBJECT_CMM_INFO_S, /**< ::type; the object type */
  0,0,0,               /**< static objects omit these fields */
  CMM_NICK,            /**< ::cmm; the four char filter id */
  (char*)"0.2",        /**< ::backend_version */
  GetText,             /**< ::getText; UI texts */
  (char**)_texts,      /**< ::texts; list of arguments to getText */
  OYRANOS_VERSION,     /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
  (oyCMMapi_s*) & _api8,

  /** ::icon; zero terminated list of a icon pyramid */
  &_api8_icon,

  NULL,                                /**< init() */
  NULL                                 /**< reset() */
};

