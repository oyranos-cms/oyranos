/** @file oyranos_cmm_CUPS.c
 *
 * @brief Printer Device Detection (CUPS backend)
 *
 * @author Joseph Simon <j.simon.iii@astound.net>
*/

#include "oyranos_cmm.h"

#include <cups/cups.h>
#include <cups/ppd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/* --- internal definitions --- */

#define CMM_NICK "CUPS"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.printer." CMM_NICK

#define catCMMfunc(nick,func) CUPS ## func

#define CMMInit                 catCMMfunc( CUPS, CMMInit )
#define CMMallocateFunc         catCMMfunc( CUPS, CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( CUPS, CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( CUPS, CMMMessageFuncSet )
#define CMMCanHandle            catCMMfunc( CUPS, CMMCanHandle )
#define ConfigsFromPatternUsage catCMMfunc( CUPS, ConfigsFromPatternUsage )
#define DeviceFromName_         catCMMfunc( CUPS, DeviceFromName_ )
#define GetDevices              catCMMfunc( CUPS, GetDevices )
#define _api8                   catCMMfunc( CUPS, _api8 )
#define _rank_map               catCMMfunc( CUPS, _rank_map )
#define Configs_FromPattern     catCMMfunc( CUPS, Configs_FromPattern )
#define Config_Check            catCMMfunc( CUPS, Config_Check )
#define GetText                 catCMMfunc( CUPS, GetText )
#define _texts                  catCMMfunc( CUPS, _texts )
#define _cmm_module             catCMMfunc( CUPS, _cmm_module )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x
#define STRING_ADD(a,b) sprintf( &a[strlen(a)], b )

#define CUPS_DATADIR "/usr/local/share/cups"

/*
             *********Backend-specific functions*********
                                                                */
int               PPDGetProfile       ( const char ** profileName,
                                        oyOptions_s * options );
const char *      GetProfileSelector  (oyOptions_s * options);
oyOptions_s *     SaveCUPSOptions     (const char * device_name, oyConfig_s ** device);

//         *************************************************

char          pathprog[1024];         /* Complete path to program/filter */
const char    *ptr = 0;               /* Pointer into string */
char          *root = "";             /* Root directory */

oyMessage_f message = 0;

extern oyCMMapi8_s _api8;
oyRankPad _rank_map[];

int CMMInit ()
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

/** @func  CMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2009/02/09
 */
int            CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void     ConfigsFromPatternUsage( oyStruct_s        * options )
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
      " The bidirectional option \"icc_profile\" will add a Profile_s.\n"
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
      " If not the backend might be able to get this information from \n"
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

// Function to save all CUPS-specific options (using a specified PPD file) and save them as Oyranos options.
//                *** IN PROGRESS ***
oyOptions_s *       SaveCUPSOptions ( const char * device_name, oyConfig_s ** device)
{
    http_t * http = httpConnectEncrypt ( cupsServer(),
                                         ippPort(),
                                         cupsEncryption());

    const char * ppd_file_location = cupsGetPPD2(http, device_name);
    ppd_file_t * ppd = ppdOpenFile(ppd_file_location);  

    ppd_option_t * options = ppd->groups->options;

    int i;
    int option_num = options->num_choices;

    char * keyword = 0;
    char * value = 0;

    oyConfig_AddDBData(*device, "device_name", device_name, OY_CREATE_NEW);

    for (i = 0; i < option_num; i++)
    {  
        if (options[i].conflicted)
            break;
 
        keyword = (options[i].keyword);
        value = (options[i].defchoice);

        

        oyConfig_AddDBData (*device, keyword, value, OY_CREATE_NEW);
    } 

    ppd_attr_t * attr = 0;
    int attr_amt = ppd->num_attrs;

    for (i = 0; i < attr_amt; i++)
    {

        keyword = ppd->attrs[i]->name;
        value = ppd->attrs[i]->value;

        oyConfig_AddDBData (*device, keyword, value, OY_CREATE_NEW);
    }

    oyConfig_SaveToDB(*device);
    oyOptions_s * o = (*device)->db;
    
    ppdClose(ppd);
    
    return o;
}

//                     ***IN PROGRESS***
int          DeviceFromName_ ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device,
                                       oyAlloc_f           allocateFunc )
{
    //const char * value3 = 0;
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
      int i, p;

      cups_dest_t *dests, *dest;
      int num_dests = cupsGetDests(&dests); 

      for (p = 0, i = num_dests, dest = dests; i > 0; i--, dest++, p++)
      {
          if(strcmp(device_name, dest->name) == 0)
          {
              const char * ppd_file_location = cupsGetPPD(device_name);
              ppd_file_t * ppd = ppdOpenFile(ppd_file_location);  

              manufacturer = ppd->manufacturer;
              model = ppd->modelname;   
              serial = "not known at this time"; 
              system_port = "not known at this time"; 
              break;
          }  
      }

      cupsFreeDests (num_dests, dests);  

      host = "not known at this time"; 

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 _DBG_FORMAT_ "Could not complete \"properties\" call.\n"
                 " oyGetMonitorInfo_lib returned with %s; device_name:"
                 " \"%s\"", _DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 device_name ? device_name : "" );

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
        OPTIONS_ADD( (*device)->backend_core, system_port )
        OPTIONS_ADD( (*device)->backend_core, host ) 

        if(!error && data_blob)
        {  
          o = oyOption_New( CMM_BASE_REG OY_SLASH "data_blob", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, data_blob->ptr, data_blob->size );
          
          if(!error)
            oyOptions_MoveIn( (*device)->data, &o, -1 );
          oyBlob_Release( &data_blob );  
        }
      }
    }

  return error;
}

//                     ***COMPLETED***
int GetDevices                   ( char            *** list,
                                       oyAlloc_f           allocateFunc )
{  
    httpInitialize();
    // Open access to printer(s) installed on system.
    cups_dest_t *dests, *dest;
    //printf("server = %s, port = %i\n", cupsServer(), ippPort()); 
 
    http_t * http = httpConnectEncrypt ( cupsServer(),
                                         ippPort(),
                                         cupsEncryption());

    int num_dests = cupsGetDests2(http, &dests); 
    int i, p;

    int len = sizeof(char*) * num_dests;
    char ** texts = allocateFunc( len );

    memset( texts, 0, len );

    // Use CUPS to obtain printer name(s) on the default server.
    for (p = 0, i = num_dests, dest = dests; i > 0; i--, dest++, p++)
    {
        texts[p] = allocateFunc(strlen(dest->name)+1); 
        sprintf( texts[p], dest->name );
    }

    // Free memory that is used by CUPS.
    cupsFreeDests (num_dests, dests);

    // Return printer name(s) and the number of printers installed on system.
    *list = texts; 
    return num_dests;
}

//                  *** IN PROGRESS ***
int            Configs_FromPattern (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
    oyConfigs_s * devices = 0;
    oyConfig_s * device = 0;
    oyOption_s * o = 0;
    oyProfile_s * p = 0;
    char ** texts = 0;
    char * text = 0;
    int texts_n = 0, i,
    error = !s;
    const char * value1 = 0,
               * value2 = 0,
               * value3 = 0,
               * value4 = 0;

    int rank = oyFilterRegistrationMatch( _api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );

    oyAlloc_f allocateFunc = malloc;
    static char * num = 0;
    const char * tmp = 0;

    if(!num)
        num = malloc( 80 );

    if(!options || !oyOptions_Count( options ))
    {
        /** oyMSG_WARN should make shure our message is visible. */
        ConfigsFromPatternUsage( (oyStruct_s*)options );
        return 0;
    }
    
    if(rank && error <= 0)
    {
        devices = oyConfigs_New(0);

        /* "list" call section */
    value1 = oyOptions_FindString( options, "device_name", 0 );
    value2 = oyOptions_FindString( options, "command", "list" );

    if(oyOptions_FindString( options, "command", "list" ) ||
       (!oyOptions_FindString( options, "command", "properties" ) &&
        !oyOptions_FindString( options, "command", "setup" ) &&
        !oyOptions_FindString( options, "command", "unset" ))
      )
    {
      texts_n = GetDevices( &texts, allocateFunc );

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


        value4 = oyOptions_FindString( options, "icc_profile", 0 );
        if(value4 || oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 6;
          const char * data = "dummy";

          /* In case the devices do not support network transparent ICC profile
           * setup, then use the DB stored profile, e.g.
           * @see oyDeviceProfileFromDB() + Profile_FromFile()
           * This will then turn the backend in a pure local one.
           *
           * One the opposite the Xorg-"oyX1" backend puts the profile in 
           * X server.
           * Then it is up to Oyranos to take action. The backend needs to
           * report a issue to inform Oyranos, as seen below.
           */

          /** Warn and return issue on not found profile. */
          if(!size || !data)
          {
            message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "Could not obtain icc_profile information for %s",
                _DBG_ARGS_, texts[i]);
            error = -1;
          } else
          {
            p = oyProfile_FromMem( size, (const oyPointer)data, 0, 0 );
            o = oyOption_New( CMM_BASE_REG OY_SLASH "icc_profile", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( device->data, &o, -1 );
          }
        }

        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          text = calloc( 4096, sizeof(char) );

          o = oyOptions_Find( device->data, "icc_Profile" );

          if( o && o->value && o->value->oy_struct && 
              o->value->oy_struct->type_ == oyOBJECT_PROFILE_S)
          {
            /* our dummy profile will certainly fail */
            p = oyProfile_Copy( (oyProfile_s*) o->value->oy_struct, 0 );
            tmp = oyProfile_GetFileName( p, 0 );

            STRING_ADD( text, "  " );
            if(strrchr( tmp, OY_SLASH_C ))
              STRING_ADD( text, strrchr( tmp, OY_SLASH_C ) + 1 );
            else
              STRING_ADD( text, tmp );

            oyProfile_Release( &p );
          }

          if(error <= 0)
          error = oyOptions_SetFromText( &device->data,
                                         CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          free( text );
        }

        if(error <= 0)
          device->rank_map = oyRankMapCopy( _rank_map,
                                                device->oy_->allocateFunc_);

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        *s = devices;

      return error;
    }

    /* "properties" call section */
    value2 = oyOptions_FindString( options, "command", "properties" );
    if(value2)
    {
      texts_n = GetDevices( &texts, allocateFunc );

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

        error = DeviceFromName_( value1, options, &device,
                                         allocateFunc );

        if(error <= 0 && device)
          device->rank_map = oyRankMapCopy( _rank_map,
                                                device->oy_->allocateFunc_);
        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        *s = devices;

      return error;
    }

    /* "setup" call section */
    value2 = oyOptions_FindString( options, "command", "setup" );
    value3 = oyOptions_FindString( options, "profile_name", 0 );
    if(error <= 0 && value2)
    {
      error = !value1 || !value3;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
              "The device_name/profile_name option is missed. Options:\n%s",
                _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = 0; /* doSetup( value1, value3 ); */
      return error;
    }

    /* "unset" call section */
    value2 = oyOptions_FindString( options, "command", "unset" );
    if(error <= 0 && value2)
    {
      error = !value1;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "The device_name option is missed. Options:\n%s",
                _DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = 0; /* doUnset( value1 ); */
      return error;
    }
  }

  /* not to be reached section, e.g. warning */
  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  ConfigsFromPatternUsage( (oyStruct_s*)options );

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
//                  ***IN PROGRESS***
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


/** @instance _api8
 *  @brief     oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyCMMapi8_s _api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  CMMCanHandle,          /**< oyCMMCanHandle_f oyCMMCanHandle */

  CMM_BASE_REG,              /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  Configs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  Config_Check,          /**< oyConfig_Check_f oyConfig_Check */
  _rank_map              /**< oyRankPad ** rank_map */
};

/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
//                  TODO
const char * GetText             ( const char        * select,
                                       oyNAME_e            type )
{
    if(strcmp(select, "name")==0)
    {
        if(type == oyNAME_NICK)
            return _(CMM_NICK);
        else if(type == oyNAME_NAME)
            return _("Oyranos Printer");
        else
            return _("The CUPS/printer backend for Oyranos.");
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
            return _("MIT");
        else if(type == oyNAME_NAME)
            return _("Copyright (c) 2009 Kai-Uwe Behrmann; MIT");
        else
            return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
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
//                  TODO
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


//                  *** IN PROGRESS ***
int   PPDGetProfile     ( const char ** profileName,
                                  oyOptions_s * options )
{
    if(!options)
         return 0;
   
    *profileName = oyOptions_FindString(options, "cupsICCProfile", 0);

    oyOption_s * o = 0;
    int option_n = options->list->n_;

    int count = 0;
    
    if(profileName)
    {
        if(profileName[0] == '/')
            snprintf(pathprog, sizeof(pathprog), "%s%s", root, profileName);
        else
        {
            if (( ptr = getenv("CUPS_DATADIR")) == NULL)
                  ptr = CUPS_DATADIR;
            if(*ptr == '/' || !*root)
                  snprintf(pathprog, sizeof(pathprog), "%s%s/profiles/%s", root,
                       ptr, profileName);
            else
                snprintf(pathprog, sizeof(pathprog), "%s/%s/profiles/%s", root,
                       ptr, profileName);
        }

    }
    
    if(!profileName || !profileName[0])
    {
        profileName = "null";
    }

    return 0;
}

const char *    PPDGetProfileSelector(oyOptions_s * options)
{
    const char * selectorA = oyOptions_FindString(options, "DefaultColorSpace", 0);
    const char * selectorB = oyOptions_FindString(options, "MediaType", 0);
    const char * selectorC = oyOptions_FindString(options, "Resolution", 0);

    const char * custom_qualifer_B = oyOptions_FindString(options, "cupsICCQualifer2", 0);
    const char * custom_qualifer_C = oyOptions_FindString(options, "cupsICCQualifer3", 0);

    if(custom_qualifer_B != NULL)
        selectorB = custom_qualifer_B;
    if(custom_qualifer_C != NULL)
        selectorC = custom_qualifer_C;

    const char * profile_selector = selectorA;
    profile_selector = strcat(profile_selector, ".");
    profile_selector = strcat(profile_selector, selectorB);
    profile_selector = strcat(profile_selector, ".");
    profile_selector = strcat(profile_selector, selectorC);
    
    return profile_selector;             
}

