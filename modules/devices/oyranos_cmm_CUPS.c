/** @file oyranos_printer.c
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

#define CUPS_DATADIR "/usr/local/share/color/icc/"

//********Backend-specific functions*********

int PPDGetProfile ( const char ** profileName, oyOptions_s * options );
const char * GetProfileSelector (oyOptions_s * options);
ppd_file_t * ppdFromDeviceName( const char * device_name );


// *************************************************

char          pathprog[1024];         /* Complete path to program/filter */
const char    *ptr = 0;               /* Pointer into string */
char          *root = "";             /* Root directory */

oyMessage_f message = 0;

extern oyCMMapi8_s _api8;
oyRankPad _rank_map[];

char *    getProfileSelector(const char * device, char ** colorspace, char ** paper, char ** resolution);
int *    setProfileSelector(const char * device, const char * profile_selector);

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

int GetDevices                   (          char            *** list,
                                       oyAlloc_f           allocateFunc )
{  
    httpInitialize();
   
    // Open access to printer(s) installed on system.
    cups_dest_t *dests, *dest;     
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
        texts[p] = allocateFunc(24); 
        sprintf( texts[p], dest->name );
    }

    // Free memory that is used by CUPS.
    cupsFreeDests (num_dests, dests);

    // Return printer name(s) and the number of printers installed on system.
    *list = texts; 
    return num_dests;
}

int          DeviceFromName_ (         const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device,
                                       oyAlloc_f           allocateFunc )
{    
    oyOption_s * o = 0;
    int error = !device;

    if(!error)
    {
      char * manufacturer= 0, *model=0, *serial=0, *host=0, *system_port=0, *profile_name = 0;
      oyBlob_s * device_context = 0, *device_settings = 0;

      if(!device_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", _DBG_ARGS_ );
        error = 1;
        return error;
      }

      int i, p;
     
      httpInitialize();
      http_t * http = httpConnectEncrypt ( cupsServer(),
                                         ippPort(),
                                         cupsEncryption());

      cups_dest_t *dests, *dest;
      int num_dests = cupsGetDests2(http, &dests); 

      const char * ppd_file_location = cupsGetPPD2(http, device_name);
      ppd_file_t * ppd = ppdOpenFile(ppd_file_location);  

      manufacturer = ppd->manufacturer;
      model = ppd->modelname;   
      serial = 0;                       // Not known at this time.
      system_port = device_name; 
  
      cupsFreeDests (num_dests, dests);        

      host = cupsServer(); 

      device_context = ppd; 
      
      const char * pname = 0, *spec = 0;
      ppd_attr_t * attrs = ppdFindAttr(ppd,"cupsICCProfile", 0);
 
      const char * printer_name = attrs->value;
      
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
        OPTIONS_ADD( (*device)->backend_core, printer_name )
                               
        device_context = ppd;
        
        if(!error && device_context)
        {           
          o = oyOption_New( CMM_BASE_REG OY_SLASH "device_context", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, device_context->ptr, device_context->size );
          
          if(!error)
            oyOptions_MoveIn( (*device)->data, &o, -1 );
          oyBlob_Release( &device_context );  
        }
      }
    ppdClose(ppd);
    httpClose(http);
    }
 
  return error;
}




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
               * value4 = 0,
               * profile_name = 0;

    int rank = oyFilterRegistrationMatch( _api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );

    // Initialize the CUPS server.
    httpInitialize();
    http_t * http = httpConnectEncrypt ( cupsServer(),
                                          ippPort(),
                                          cupsEncryption());

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

        if(error <= 0){ 
        error = oyOptions_SetFromText( &device->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );
        }

        value4 = oyOptions_FindString( options, "icc_profile", 0 );   
        if(value4 || oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {              
          // Open PPD file for profile extraction.
          const char * ppd_file_location = cupsGetPPD2(http, texts[i]);
          ppd_file_t * ppd = ppdOpenFile(ppd_file_location);  
          
          // Grab an available ICC Profile using an attribute.
          ppd_attr_t * attrs = ppdFindAttr(ppd,"cupsICCProfile", 0); 
          
          /* Does a profile/attribute exist?
             NOTE For right now, PPD files WITHOUT the cupsICCProfile
                  identifer is assumed to be a non-color printer. 
          */
          if(!attrs)
              profile_name = "Gray.icc";
          else if (attrs)
              profile_name = attrs->value;

          // Save 'profile_name' option to Oyranos.
          error = oyOptions_SetFromText( &device->backend_core,
                                       CMM_BASE_REG OY_SLASH "profile_name",
                                       profile_name, OY_CREATE_NEW );
             
          // Generate cups HTTP-specific strings.
          char uri[1024];         
          char temp_profile_location[1024];
                
          // Create the complete path to the profile on the server (ie. http://host:port/profiles/****.icc
          httpAssembleURIf(HTTP_URI_CODING_ALL, uri, sizeof(uri), "http", NULL, cupsServer(), ippPort(), "/profiles/%s", profile_name);         
          
          // Get a file descriptor to download the file from the server to the /tmp directory.
          int tempfd = cupsTempFd(temp_profile_location, sizeof(temp_profile_location));
          
          // Download file using the descriptor and uri path.
          cupsGetFd(http, uri, tempfd);             

          // Open the file.
          FILE * old_file = fopen(temp_profile_location, "rb");   
                    
          // Find the total size.
          fseek(old_file , 0, SEEK_END);
          long lsize = ftell(old_file);
          rewind (old_file);

          // Create buffer to read contents into a profile.
          char * data = (char*) malloc (sizeof(char)*lsize);
          if (data == NULL) {fputs ("Unable to find profile size.",stderr); exit (2);}

          size_t size = fread( data, 1, lsize, old_file);                     
                    
          // Check to see if profile is a custom one.
          if( oyProfile_FromFile(profile_name, 0, 0) == NULL )
          {
            message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "Could not obtain profile information for %s -- building profile %s.",
                _DBG_ARGS_, texts[i], profile_name);
           
            // Create a new file to copy the profile.
            FILE * new_file = fopen(profile_name, "wb");         
            fwrite(data, 1, size, new_file);

            // Use Oyranos to save the file.
            p = oyProfile_FromFile(profile_name, 0, 0);
            o = oyOption_New( CMM_BASE_REG OY_SLASH "icc_profile", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( device->data, &o, -1 );

            free( data );
            fclose( new_file );
           
          } 

          // If Oyranos knows the profile, simply use the buffer.
          else
          {
            p = oyProfile_FromMem( size, (const oyPointer)data, 0, 0 );
            o = oyOption_New( CMM_BASE_REG OY_SLASH "icc_profile", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( device->data, &o, -1 );
            free( data );  
            
          }

           fclose( old_file );
           
           ppdClose(ppd);
           httpClose(http);
        } 

        /** Build oyNAME_NAME */
        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          text = calloc( 4096, sizeof(char) );

          o = oyOptions_Find( device->data, "icc_Profile" );

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


    if(error <= 0 && 
       oyOptions_FindString( options, "command", "setup"))
    { 
         value4 = oyOptions_FindString( options, "profile_name", 0 );
         error = !value3 || !value4;
         if (error >= 1)
           message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
              "The device/profile name option is missing. OPTIONS:\n%s",
                _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK)
                 );
          else
          {
              //error = CUPSProfileSetup( value1, "sRGB.icc");
          }

    }

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
            return _("Oyranos CUPS");
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
            return _("Copyright (c) 2009 Joseph Simon; MIT");
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





/*                       NOTE
   The functions below are reserved for a possible profile policy.
*/


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

char *    getProfileSelector(const char * device, char ** colorspace, char ** paper, char ** resolution)
{
    const char * ppd_file_location = cupsGetPPD(device);
    ppd_file_t * ppd_file = ppdOpenFile(ppd_file_location);  

    ppd_option_t * options = ppd_file->groups->options;

    int i;
    int option_num = options->num_choices;

    const char * keyword = 0;
    
    char * selectorA = 0;
    char * selectorB = 0;
    char * selectorC = 0;
    char * custom_qualifer_B = 0;
    char * custom_qualifer_C = 0;

    for (i = 0; i < option_num; i++)
    {  
        if (options[i].conflicted)
            break;
 
        keyword = (options[i].keyword);
        
        if (strcmp(keyword, "ColorModel") == 0)
            selectorA = (options[i].defchoice);
        else if (strcmp(keyword, "MediaType") == 0)
            selectorB = (options[i].defchoice);
        else if (strcmp(keyword, "Resolution") == 0)
            selectorC = (options[i].defchoice);
        else if (strcmp(keyword, "cupsICCQualifer2") == 0)
            custom_qualifer_B = (options[i].defchoice);
        else if (strcmp(keyword, "cupsICCQualifer3") == 0)
            custom_qualifer_C = (options[i].defchoice);      
    } 

    ppd_attr_t * attr = 0;
    int attr_amt = ppd_file->num_attrs;

    for (i = 0; i < attr_amt; i++)
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

    const char * profile_selector = selectorA;
    profile_selector = strcat(profile_selector, ".");
    profile_selector = strcat(profile_selector, selectorB);
    profile_selector = strcat(profile_selector, ".");
    profile_selector = strcat(profile_selector, selectorC);   
    profile_selector = strcat(profile_selector, ": \"");   

    colorspace = selectorA;
    paper = selectorB;
    resolution = selectorC;
    //ppdClose(ppd_file);
    return profile_selector;             
}


int CUPSProfileSetup( const char * device_name, const char * profile_name)
{

    oyProfile_s * p = 0;


    ppd_file_t * ppd_file = ppdFromDeviceName(device_name);
    ppd_attr_t * profile_attr = ppdFindAttr ( ppd_file, "cupsICCProfile", 0);

    p = oyProfile_FromFile(profile_name, 0, 0);
    
    icSignature cups_profile_sig = oyProfile_GetSignature(p, oySIGNATURE_COLOUR_SPACE);
    const char * color_space = 0;

    if (cups_profile_sig == icSigRgbData)
        color_space = "RGB";
    else if (cups_profile_sig == icSigGrayData)
        color_space = "Gray";
    else if (cups_profile_sig == icSigCmykData)
        color_space = "Cmyk";  

    

    //char ** cs = 0, ** paper = 0, ** res = 0;
    //getProfileSelector(device_name, &cs, &paper, &res);

    //printf("paper = %s", *paper);

    // Save new profile name
    //profile_attr->value = profile_name;
    ppdClose(ppd_file);
}

ppd_file_t * ppdFromDeviceName( const char * device_name )
{
    const char * ppd_file_location = cupsGetPPD(device_name);
    ppd_file_t * ppd_file = ppdOpenFile(ppd_file_location);  
}


int *    setProfileSelector(const char * device, const char * profile_selector)
{    
    ppd_file_t * ppd_file = ppdFromDeviceName(device);

    int attr_amt = ppd_file->num_attrs;
    char * keyword = 0;
    
    int i = 0;
    for (i = 0; i < attr_amt; i++)
    {
        keyword = ppd_file->attrs[i]->name;
        
        if (strcmp(keyword, "cupsICCProfile") == 0)
        {
            ppd_file->attrs[i]->value = profile_selector;
        }

    }

    ppdClose(ppd_file);
}
