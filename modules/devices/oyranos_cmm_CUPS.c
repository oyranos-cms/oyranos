/** @file oyranos_cmm_CUPS.c
 *
 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2009 (C) Joseph Simon III
 *
 *  @brief    Printer Device Detection (CUPS module)
 *  @internal
 *  @author   Joseph Simon III <j.simon.iii@astound.net>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/08/17
 *
 *
 * @author Joseph Simon <j.simon.iii@astound.net>
*/
#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_i18n.h"

#include <cups/cups.h>
#include <cups/ppd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <locale.h>

/* internal Oyranos APIs */
#include "oyranos_texts.h"
#include "oyranos_helper.h"

/* --- internal definitions --- */

#define CMM_NICK "CUPS"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.device.icc_profile.printer." CMM_NICK
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( CUPS, CMMInit )
#define CMMallocateFunc         catCMMfunc( CUPS, CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( CUPS, CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( CUPS, CMMMessageFuncSet )
#define ConfigsFromPatternUsage catCMMfunc( CUPS, ConfigsFromPatternUsage )
#define DeviceAttributes_       catCMMfunc( CUPS, DeviceAttributes_ )
#define GetDevices              catCMMfunc( CUPS, GetDevices )
#define _api8                   catCMMfunc( CUPS, _api8 )
#define _rank_map               catCMMfunc( CUPS, _rank_map )
#define Configs_Modify          catCMMfunc( CUPS, Configs_Modify )
#define Configs_FromPattern     catCMMfunc( CUPS, Configs_FromPattern )
#define Config_Check            catCMMfunc( CUPS, Config_Check )
#define GetText                 catCMMfunc( CUPS, GetText )
#define _texts                  catCMMfunc( CUPS, _texts )
#define _cmm_module             catCMMfunc( CUPS, _cmm_module )
#define _api8_ui                catCMMfunc( CUPS, _api8_ui )
#define Api8UiGetText           catCMMfunc( CUPS, Api8UiGetText )
#define _api8_ui_texts          catCMMfunc( CUPS, _api8_ui_texts )
#define _api8_icon              catCMMfunc( CUPS, _api8_icon )
#define _help                   catCMMfunc( CUPS, _help )
#define _help_list              catCMMfunc( CUPS, _help_list )
#define _help_properties        catCMMfunc( CUPS, _help_properties )
#define _help_setup             catCMMfunc( CUPS, _help_setup )
#define _help_unset             catCMMfunc( CUPS, _help_unset )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__

int CUPSgetProfiles                  ( const char        * device_name,
                                       ppd_file_t        * ppd_file,
                                       oyConfigs_s       * devices,
                                       oyOptions_s       * user_options );
const char * GetText                 ( const char        * select,
                                       oyNAME_e            type );
const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type );

oyMessage_f message = 0;

extern oyCMMapi8_s _api8;
extern oyRankPad _rank_map[];

int CMMInit                          ( oyStruct_s        * filter )
{
    int error = 0;
    return error;
}

oyPointer CMMallocateFunc ( size_t size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void CMMdeallocateFunc ( oyPointer mem )
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
int            CMMMessageFuncSet ( oyMessage_f message_func )
{
  message = message_func;
  return 0;
}


#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );
const char * _help =
      "The following help text informs about the communication protocol.";
const char * _help_list = 
      "The presence of option \"command=list\" will provide a list of\n"
      " available devices. The actual device name can be found in\n"
      " option \"device_name\". The call is as lightwight as possible.\n"
      " The option \"oyNAME_NAME\" returns a short string containting\n"
      " informations and if available, the profile name or size.\n"
      " The bidirectional option \"icc_profile\" will add a Profile_s.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n" 
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n" 
      " string.\n" 
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The option \"device_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION above."
      " Informations are stored in the returned oyConfig_s::data member.";
const char * _help_properties =
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
      " stored in the returned oyConfig_s::backend_core member.";
const char * _help_setup =
      "The presence of option \"command=setup\" will setup the device\n"
      " from a profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"
      " file name.";
const char * _help_unset =
      "The presence of option \"command=unset\" will invalidate a profile of\n"
      " a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n";

void     ConfigsFromPatternUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN should make shure our message is visible. */
    message( oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
             _DBG_ARGS_, _help );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, _help_list );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, _help_properties );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, _help_setup );
    message( oyMSG_WARN, options, "%s()\n %s", __func__, _help_unset );

  return;
}

static http_t * cups_http_ = 0;

int  oyCloseCUPSConnection()
{
  httpClose(cups_http_);
  cups_http_ = 0;
  return 0;
}

http_t * oyGetCUPSConnection()
{
  if(!cups_http_)
  {
    httpInitialize();
    /* Open access to printer(s) installed on system. */
    cups_http_ = httpConnectEncrypt ( cupsServer(),
                                      ippPort(),
                                      cupsEncryption());
  }
  return cups_http_;
}

int GetDevices                       ( http_t            * http,
                                       char            *** list,
                                       oyAlloc_f           allocateFunc )
{
    cups_dest_t *dests, *dest;

    int num_dests;
    int i, p;

    int len;
    char ** texts;

    /* Open access to printer(s) installed on system. */
    num_dests = cupsGetDests2(http, &dests);
    len = sizeof(char*) * num_dests;
    texts = allocateFunc( len );

    memset( texts, 0, len );

    /* Use CUPS to obtain printer name(s) on the default server. */
    for (p = 0, i = num_dests, dest = dests; i > 0; i--, dest++, p++)
    {
        texts[p] = allocateFunc(24); 
        sprintf( texts[p], "%s", dest->name );
    }

    /* Free memory that is used by CUPS. */
    cupsFreeDests (num_dests, dests);

    /* Return printer name(s) and the number of printers installed on system. */
    *list = texts; 
    return num_dests;
}

int          DeviceAttributes_       ( ppd_file_t        * ppd,
                                       oyOptions_s       * options,
                                       oyConfig_s        * device,
                                       const char        * ppd_file_location )
{
    oyOption_s * o = 0;
    int error = !device;
 
    const char * value3 = oyOptions_FindString( options, "device_context", 0 );
    const char * device_name = oyConfig_FindString( device, "device_name", 0 );

    if(!error)
    {
      char * manufacturer= 0,
           * model=0,
           * serial=0,
           * device_settings = 0;
      const char * system_port = 0,
                 * host = 0;
      ppd_attr_t * attrs = 0;

      if(!device_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_
                "The \"device_name\" is\n"
                " missed to select a appropriate device.", _DBG_ARGS_ );
        error = 1;
        return error;
      }
 
      manufacturer = ppd->manufacturer;
      model = ppd->modelname;
      serial = 0;                       /* Not known at this time. */
      system_port = device_name;

      host = cupsServer();
      attrs = ppdFindAttr(ppd, "cupsICCProfile", 0);

      if(attrs && attrs->text)
        device_settings = attrs->text;
 
      if(error <= 0)
      {
        size_t size = 0;
        char * data = 0;

        OPTIONS_ADD( device->backend_core, manufacturer )
        OPTIONS_ADD( device->backend_core, model )
        OPTIONS_ADD( device->backend_core, serial )
        OPTIONS_ADD( device->backend_core, system_port )
        OPTIONS_ADD( device->backend_core, host )
        OPTIONS_ADD( device->backend_core, device_settings )

        if (value3)
        {
          /* open the PPD data */
          {
            FILE * fp = fopen( ppd_file_location, "r" );
            size_t lsize = 0;

            /* Find the total size. */
            fseek(fp , 0, SEEK_END);
            lsize = ftell(fp);
            rewind (fp);

            /* Create buffer to read contents into a profile. */
            data = (char*) malloc (sizeof(char)*lsize + 1);
            if (data == NULL) fputs ("Unable to open PPD size.",stderr);

            size = fread( data, 1, lsize, fp);
            data[size] = 0;
          }

          if(!error && data && size)
          {           
            o = oyOption_New( CMM_BASE_REG OY_SLASH "device_context", 0 );
            error = !o;
            if(!error)
              error = oyOption_SetFromData( o, data, size );
          
            if(!error)
              oyOptions_MoveIn( device->data, &o, -1 );
          }
        }
      }
    }
 
  return error;
}


int            Configs_Modify    ( oyConfigs_s       * devices,
                                   oyOptions_s       * options )
{
  oyConfig_s * device = 0;
  oyOption_s * o = 0;
  oyProfile_s * p = 0;
  char * text = 0;
  int n = 0, i,
  error = !devices;
  const char   * unset_request = 0,
               * profile_in = 0,
               * profile_request = 0,
               * properties_request = 0,
               * setup_request = 0;
  http_t * http = 0;
  static char * num = 0;
  const char * tmp = 0, * printer_name = 0;

  /* Initialize the CUPS server. */
  http = oyGetCUPSConnection();

  if(!num)
    num = malloc( 80 );

  if(!options || !oyOptions_Count( options ))
  {        
        /** oyMSG_WARN should make shure our message is visible. */
        ConfigsFromPatternUsage( (oyStruct_s*)options );
        return 0;
  }

  if(error <= 0)
  {

    /** "list" call section */
    properties_request = oyOptions_FindString( options, "command", "properties" );
    if(oyOptions_FindString( options, "command", "list" ) ||
       properties_request)
    {
      n = oyConfigs_Count( devices );
      for( i = 0; i < n; i++ )
      {
        device = oyConfigs_Get( devices, i );

        printer_name = oyConfig_FindString( device, "device_name", 0 );


        /* Building of a icc_profile option should be already done with the 
           first call to "list" */
        profile_request = oyOptions_FindString( options, "icc_profile", 0 );   

        /* Build oyNAME_NAME */
        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        { 
          text = 0;
          o = oyOptions_Find( device->data, "icc_profile" );

          if( o && o->value && o->value->oy_struct && 
              o->value->oy_struct->type_ == oyOBJECT_PROFILE_S)
          {            
            p = oyProfile_Copy( (oyProfile_s*) o->value->oy_struct, 0 );
            tmp = oyProfile_GetFileName( p, 0 );
            STRING_ADD( text, "  " );
            if(strrchr( tmp, OY_SLASH_C ))
              STRING_ADD( text, strrchr( tmp, OY_SLASH_C ) + 1 );
            else
              STRING_ADD( text, tmp );

              oyProfile_Release( &p );

            error = oyOptions_SetFromText( &device->data,
                                         CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
            oyDeAllocateFunc_( text );
          }
        }

        if(error <= 0 && !device->rank_map)
          device->rank_map = oyRankMapCopy( _rank_map,
                                            device->oy_->allocateFunc_);

        oyConfig_Release( &device );
      }

      goto clean;
    }

    /** "setup" call section */
    setup_request = oyOptions_FindString( options, "command", "setup" );
    if(error <= 0 && setup_request)
    {
      profile_in = oyOptions_FindString( options, "profile_name", 0 );
      printer_name = oyOptions_FindString( options, "device_name", 0 );
      error = !printer_name || !profile_in;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
              "The device_name/profile_name option is missed. Options:\n%s",
                _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

      else
      {
          /* NOTE  New profile delivery to CUPS server is not ready at the moment. */
      }

      goto clean;
    }

    /** "unset" call section */
    unset_request = oyOptions_FindString( options, "command", "unset" );
    if(error <= 0 && unset_request)
    {
      profile_in = oyOptions_FindString( options, "profile_name", 0 );
      printer_name = oyOptions_FindString( options, "device_name", 0 );

      error = !printer_name;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "The device_name option is missed. Options:\n%s",
                _DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
      {

          /* TODO Build "config_file" */

      }

      goto clean;
    }
  }

  /** "help" call section */
  if(error <= 0 &&
     oyOptions_FindString( options, "command", "help" ))
  {
    ConfigsFromPatternUsage( (oyStruct_s*)options );
    goto clean;
  }


  /* not to be reached section, e.g. warning */
  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );


  clean:
  oyCloseCUPSConnection(); http=0;
  return error;
}

int            Configs_FromPattern (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{

  oyConfigs_s * devices = 0;
  oyConfig_s * device = 0;
  char ** texts = 0;
  int texts_n = 0, i,
  error = !s;
  const char   * profile_request = 0;
  http_t * http = 0;
  oyAlloc_f allocateFunc = malloc;
  static char * num = 0;
  const char * printer_name = 0;

  int rank = oyFilterRegistrationMatch( _api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );

  /* Initialize the CUPS server. */
  http = oyGetCUPSConnection();

  if(!num)
    num = malloc( 80 );

  if(!options || !oyOptions_Count( options ))
  {        
        /** oyMSG_WARN should make shure our message is visible. */
        ConfigsFromPatternUsage( (oyStruct_s*)options );
        return 0;
  }

  printer_name = oyOptions_FindString( options, "device_name", 0 );

  if(rank && error <= 0)
  {
    devices = oyConfigs_New(0);

    /** common resolving */
    {
      texts_n = GetDevices( http, &texts, allocateFunc );

      for( i = 0; i < texts_n; i++ )
      {
        /* filter */
        if(printer_name && strcmp(printer_name, texts[i]) != 0)
          continue;

        device = oyConfig_New( CMM_BASE_REG, 0 );
        error = !device;

        /* Build icc_profile */
        profile_request = oyOptions_FindString( options, "icc_profile", 0 );   
        if(1 || profile_request ||
           oyOptions_FindString( options, "oyNAME_NAME", 0 ) )
        {
          int n, j;
          const char * device_name = texts[i];
          const char * ppd_file_location = cupsGetPPD2( oyGetCUPSConnection(),
                                                        device_name );
          ppd_file_t * ppd_file = ppdOpenFile( ppd_file_location );
          oyConfigs_s * devices_ = oyConfigs_New(0);
          oyConfig_s * tmp = oyConfig_Copy( device, 0 );
          oyConfigs_MoveIn( devices_, &tmp, -1 );

          /* Search for CUPS ICC profiles */
          CUPSgetProfiles( device_name, ppd_file, devices_, options );

          /* add additional devices */
          n = oyConfigs_Count( devices_ );
          for(j = 0; j < n; ++j)
          {
            oyConfig_s * d = oyConfigs_Get( devices_, j );
 
            /* set the device name */
            error = oyOptions_SetFromText( &d->backend_core,
                                     CMM_BASE_REG OY_SLASH "device_name",
                                     device_name, OY_CREATE_NEW );

            /* additional properties */
            error = DeviceAttributes_( ppd_file, options, d, ppd_file_location);

            if(j)
              oyConfigs_MoveIn( devices, &d, -1 );
            else
              oyConfig_Release( &d );
          }
          ppdClose( ppd_file ); ppd_file = 0;
          
          oyConfigs_Release( &devices_ );
        }

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      /* advanced stuff */
      if(oyConfigs_Count( devices ))
        Configs_Modify( devices, options );

      if(error <= 0)
        *s = devices;

      goto clean;
    }
  }

  clean:
  oyCloseCUPSConnection(); http=0;
  return error;
}

/** @instance dDev_rank_map
 *  @brief    oyRankPad map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyRankPad _rank_map[] = { 
  {"device_name", 2, -1, 0},       /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {0,0,0,0}                            /**< end of list */
};

/** Function Config_Check
 *  @brief    oyCMMapi8_s device check
 *
 *  @param[in]     config              the printer device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int   Config_Check ( oyConfig_s        * config )
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

const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0 ||
     strcmp(select,"help") == 0)
  {
    /* The "help" and "name" texts are identical, as the module contains only
     * one filter to provide help for. */
    return GetText(select,type);
  }
  else if(strcmp(select, "device_class")==0)
    {
        if(type == oyNAME_NICK)
            return _("Printer");
        else if(type == oyNAME_NAME)
            return _("Printer");
        else
            return _("Printers, which are accessible through the CUPS spooling system.");
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
      STRING_ADD( category, _("Printer CUPS") );
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
const char * _api8_ui_texts[] = {"name", "help", "device_class", "category", 0};

/** @instance _api8_ui
 *  @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI parts for CUPS devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/09/06
 */
oyCMMui_s _api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,1,10},                            /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Colour/Device/Printer", /* category */
  0,   /* const char * options */

  0,    /* oyCMMuiGet_f oyCMMuiGet */

  Api8UiGetText,  /* oyCMMGetText_f getText */
  _api8_ui_texts  /* (const char**)texts */
};

oyIcon_s _api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"
};

/** @instance _api8
 *  @brief     oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
oyCMMapi8_s _api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */

  CMM_BASE_REG,          /**< registration */
  CMM_VERSION,                         /**< int32_t version[3] */
  {0,1,10},                  /**< int32_t module_api[3] */
  0,                     /**< char * id_ */
  0,                     /**< oyCMMapi5_s * api5_ */
  Configs_FromPattern,   /**< oyConfigs_FromPattern_f oyConfigs_FromPattern */
  Configs_Modify,        /**< oyConfigs_Modify_f oyConfigs_Modify */
  Config_Check,          /**< oyConfig_Check_f oyConfig_Check */

  &_api8_ui,             /**< device class UI name and help */
  &_api8_icon,           /**< device icon */

  _rank_map              /**< oyRankPad ** rank_map */
};

/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */

const char * GetText                 ( const char        * select,
                                       oyNAME_e            type )
{
    if(strcmp(select, "name")==0)
    {
        if(type == oyNAME_NICK)
            return _(CMM_NICK);
        else if(type == oyNAME_NAME)
            return _("Oyranos CUPS");
        else
            return _("The CUPS/printer module for Oyranos.");
    } 
    else if(strcmp(select, "manufacturer")==0)
    {
        if(type == oyNAME_NICK)
            return _("Joe");
        else if(type == oyNAME_NAME)
            return _("Joseph Simon III");
        else
            return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
    } 
    else if(strcmp(select, "copyright")==0)
    {
        if(type == oyNAME_NICK)
            return "MIT";
        else if(type == oyNAME_NAME)
            return _("Copyright (c) 2009 Joseph Simon; MIT");
        else
            return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
    }
    else if(strcmp(select, "help")==0)
    {
      static char * t = 0;
      if(type == oyNAME_NICK)
        return "help";
      else if(type == oyNAME_NAME)
        return _("The CUPS module supports the generic device protocol.");
      else
      {
        if(!t)
        {
          t = malloc( strlen(_help) + strlen(_help_list)
                    + strlen(_help_properties) + strlen(_help_setup) +
                    + strlen(_help_unset) + 1);
          sprintf( t, "%s\n%s%s%s%s", _help, _help_list,
                 _help_properties, _help_setup, _help_unset );
        }
        return t;
      }
    }
    return 0;
}
const char * _texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance _cmm_module
 *  @brief     module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyCMMInfo_s _cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  (char*)"0.2",
  GetText,
  (char**)_texts, /* oyCMMInfoGetText_f */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & _api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

/** @brief get for possible profiles eachs qualifier
 *
 *  @param         device_name         the device to request the ppd for
 *  @param         device              add the 3 qualifiers and the profile
 */
int CUPSgetProfiles                  ( const char        * device_name,
                                       ppd_file_t        * ppd_file,
                                       oyConfigs_s       * devices,
                                       oyOptions_s       * user_options )
{
    int error = 0;
    ppd_option_t * options = ppd_file->groups->options;
    int i, pos = 0;
    const char * keyword = 0;
    const char * selectorA = "ColorModel",
               * selectorB = "MediaType",
               * selectorC = "Resolution",
               * custom_qualifer_B = 0, * custom_qualifer_C = 0;
    int attr_n;
    oyProfile_s * p = 0;
    oyConfig_s * device = 0;


#if 0
    int option_num = options->num_choices;
    for (i = 0; i < option_num; i++)
    {  
        if(options[i].conflicted)
            break;
 
        keyword = (options[i].keyword);
        
        printf( "option_%d: \"%s\" \"%s\" \"%s\" %d\n", i,
                options[i].keyword, options[i].defchoice, options[i].text,
                options[i].ui );
        for(j = 0; j < options[i].num_choices; ++j)
          printf( "  choice_%d: \"%s\" \"%s\"\n", j,
                  options[i].choices[j].choice, options[i].choices[j].text );
    } 
#endif

    attr_n = ppd_file->num_attrs;

    for (i = 0; i < attr_n; i++)
    {
        keyword = ppd_file->attrs[i]->name;
        
        if (strcmp(keyword, "cupsICCQualifer2") == 0)
            custom_qualifer_B = (options[i].defchoice);
        else if (strcmp(keyword, "cupsICCQualifer3") == 0)
            custom_qualifer_C = (options[i].defchoice);   
    }
 
    if(custom_qualifer_B != NULL)
        selectorB = custom_qualifer_B;
    if(custom_qualifer_C != NULL)
        selectorC = custom_qualifer_C;

    for (i = 0; i < attr_n; i++)
    {
      int count = 0;
      int must_move = 0;
      int is_custom_profile = 0;
      char ** texts = 0;
      char * profile_name = 0;

      keyword = ppd_file->attrs[i]->name;

      if(strcmp(keyword,"cupsICCProfile") != 0)
        continue;

      profile_name = ppd_file->attrs[i]->value;
      if(!profile_name || !profile_name[0])
        continue;

      texts = oyStringSplit_( ppd_file->attrs[i]->spec, '.', &count,
                              oyAllocateFunc_);
      if(count != 3)
      {
        message(oyMSG_WARN, 0, _DBG_FORMAT_ "\n "
                "cupsICCProfile specifiers are non conforming: %d %s",
                _DBG_ARGS_, count, oyNoEmptyString_m_(profile_name) );
        break;
      }

      device = oyConfigs_Get( devices, pos );
      if(!device)
      {
        device = oyConfig_New( CMM_BASE_REG, 0 );
        must_move = 1;
      }

      oyOptions_SetFromText( &device->data,
                             CMM_BASE_REG OY_SLASH "profile_name",
                             profile_name, OY_CREATE_NEW );
 
      if(selectorA && texts[0] && texts[0][0])
      {
        char * reg_name = 0;
        STRING_ADD( reg_name, CMM_BASE_REG OY_SLASH );
        STRING_ADD( reg_name, selectorA );
        oyOptions_SetFromText( &device->backend_core,
                               reg_name,
                               texts[0], OY_CREATE_NEW );
        oyDeAllocateFunc_( reg_name );
      }
      if(selectorB && texts[1] && texts[1][0])
      {
        char * reg_name = 0;
        STRING_ADD( reg_name, CMM_BASE_REG OY_SLASH );
        STRING_ADD( reg_name, selectorB );
        oyOptions_SetFromText( &device->backend_core,
                               reg_name,
                               texts[1], OY_CREATE_NEW );
        oyDeAllocateFunc_( reg_name );
      }
      if(selectorC && texts[2] && texts[2][0])
      {
        char * reg_name = 0;
        STRING_ADD( reg_name, CMM_BASE_REG OY_SLASH );
        STRING_ADD( reg_name, selectorC );
        oyOptions_SetFromText( &device->backend_core,
                               reg_name,
                               texts[2], OY_CREATE_NEW );
        oyDeAllocateFunc_( reg_name );
      }

          /* Check to see if profile is a custom one.
             If Oyranos knows the profile, simply use the buffer. */
          if(profile_name)
              p = oyProfile_FromFile(profile_name, 0, 0);

          if( p == NULL && profile_name )
          {
            /* Generate cups HTTP-specific strings. */
            char uri[1024];         
            char temp_profile_location[1024];
            FILE * old_file = 0;
            void * data = 0;
            size_t size = 0;
            int tempfd = 0;

            message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "Could not obtain profile information for %s. Downloading new profile: '%s'.",
                _DBG_ARGS_, device_name, profile_name);
           
            /* Create the complete path to the profile on the server 
             * (ie. http://host:port/profiles/xxx.icc
             */
#if CUPS_VERSION_MAJOR*100+CUPS_VERSION_MINOR < 102
            printf( uri, "http://%s:%s/profiles/%s",
                    cupsServer(), ippPort(), profile_name );
#else
            httpAssembleURIf( HTTP_URI_CODING_ALL, uri,
                              sizeof(uri), "http", NULL, cupsServer(),
                              ippPort(), "/profiles/%s", profile_name);         
#endif
            /* Get a file descriptor to download the file from the server to
             * the /tmp directory.
             */
            tempfd = cupsTempFd( temp_profile_location,
                                 sizeof(temp_profile_location));
          
            /* Download file using the descriptor and uri path. */
            cupsGetFd( oyGetCUPSConnection(), uri, tempfd);             

            /* Open the file. */
            old_file = fopen(temp_profile_location, "rb");
                    
            /* Find the total size. */
            if(old_file)
            {
              size_t lsize = 0;
              fseek(old_file , 0, SEEK_END);
              lsize = ftell(old_file);
              rewind (old_file);

              /* Create buffer to read contents into a profile. */
              data = (char*) malloc (sizeof(char)*lsize);
              if (data == NULL)
              { fputs ("Unable to find profile size.\n",stderr); }

              if(lsize)
                size = fread( data, 1, lsize, old_file);

              fclose( old_file );
            }

            if(data && size)
            {      
              is_custom_profile = 1;       /* Mark as a custom profile. */

              /* Use Oyranos to save the file in memory. */
              p = oyProfile_FromMem( size, (const oyPointer)data, 0, 0 );
              free( data ); data = 0;
            }
          }
   
          if(is_custom_profile == 1)
          {
              int success = 0;
              /* Create a file for the custom profile in a local directory. */
              /* ex. '/home/bob/.config/color/icc/custom.icc' */
              char * profile_path = 0;
              STRING_ADD( profile_path, getenv("HOME") );
              STRING_ADD( profile_path, "/.config/color/icc/" );
              STRING_ADD( profile_path, profile_name );

              /* Output to file. */
              success = oyProfile_ToFile_( p, profile_path);

              is_custom_profile = 0;      /* Done. Unmark as a custom profile. */
          }

          if(p)
          {
            oyOption_s * o = oyOption_New( CMM_BASE_REG OY_SLASH "icc_profile",
                                           0 );
            int l_error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( device->data, &o, -1 );
            if(l_error)
              error = l_error;
          }
 
      if(must_move)
        oyConfigs_MoveIn( devices, &device, pos );
      else
        oyConfig_Release( &device );

      ++pos;

    }

    return error;
}
