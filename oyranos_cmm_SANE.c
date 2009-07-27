/** @file oyranos_cmm_SANE.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos SANE device backend for Oyranos
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

#include <oyranos/oyranos_cmm.h>
#include <sane/sane.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "SANE_help.c"
/* --- internal definitions --- */

#define DBG printf("%s: %d\n", __FILE__, __LINE__ ); fflush(NULL);
/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "SANE"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.scanner." CMM_NICK

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( SANE , CMMInit )
#define CMMallocateFunc         catCMMfunc( SANE , CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( SANE , CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( SANE , CMMMessageFuncSet )
#define CMMCanHandle            catCMMfunc( SANE , CMMCanHandle )
#define ConfigsFromPatternUsage catCMMfunc( SANE , ConfigsFromPatternUsage )
#define DeviceFromName_     catCMMfunc( SANE , DeviceFromName_ )
#define GetDevices              catCMMfunc( SANE , GetDevices )
#define _api8                   catCMMfunc( SANE , _api8 )
#define _rank_map               catCMMfunc( SANE , _rank_map )
#define Configs_FromPattern     catCMMfunc( SANE , Configs_FromPattern )
#define Config_Check            catCMMfunc( SANE , Config_Check )
#define GetText                 catCMMfunc( SANE , GetText )
#define _texts                  catCMMfunc( SANE , _texts )
#define _cmm_module             catCMMfunc( SANE , _cmm_module )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x
#define STRING_ADD(a,b) sprintf( &a[strlen(a)], b )

oyMessage_f message = 0;

extern oyCMMapi8_s _api8;
oyRankPad _rank_map[];

/* --- implementations --- */

int                CMMInit       ( )
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

/** @func  CMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2009/02/09
 */
int                CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void         ConfigsFromPatternUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN should make shure our message is visible. */
    message( oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
             _DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n%s", __func__, help_message );

  return;
}

/** @internal
 * @brief Put all the scanner hardware information in a oyConfig_s object
 *
 * @param[in]	device_name			SANE_Device::name
 * @param[in]	options				what does this hold??????
 * @param[out]	device				Holds the scanner H/W info
 * @param[in]	all					Holds the list of all scanner H/W info
 * @param[in]	size					Holds the number of scanners
 *
 * \todo { Untested.
 * The problem is that given the device_name string, the only way
 * to find the SANE_Device struct is to call sane_get_devices(), [which is
 * an expensive call] and then compare all SANE_Device::name fields.
 * Better use the DeviceFromName_() call, only when he calling application
 * does not provide us with a pointer to the SANE_Device struct }
 */
int              DeviceFromName_ ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device,
                                       const char ** all,
													int size )
{
  const char * value3 = 0;
  oyOption_s * o = 0;
  int error = !device;
	
    value3 = oyOptions_FindString( options, "data_blob", 0 ); /*TODO What does this do?*/

    if(!error)
    {
      const char * manufacturer=0, *model=0, *serial=0, *host=0, *system_port=0;
      oyBlob_s * data_blob = 0;
		SANE_Bool local = 1;
		int status,i = 0;

      if(!device_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", _DBG_ARGS_ );
        error = 1;
        return error;
      }
		/* if device string starts with net, it is a remote device */
		if (device_name[0] == 'n' && device_name[1] == 'e' && device_name[2] == 't')
			local = 0;

		/* find device string in all array */
		for (i=0; i<size; i++)
			if (strcmp(all[i],device_name) == 0)
				break;
		if (i == size) {
    		message( oyMSG_WARN, 0, "%s()\n The supplied device string did not match\n", __func__ );
			return 1;
		}

		manufacturer = all[i+size];
		model = all[i+2*size];
		serial = "unsupported";
		host = local?"localhost":"remote";
		system_port = "TODO"; /*TODO scsi/usb/parallel port?*/

      if(error != 0) /*TODO What is this? */
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
			/*TODO make sure the strings get copied, not pointed */
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

int GetDevices(const SANE_Device *** device_list, int* size)
{
	int status, s = 0;
	const SANE_Device ** dl = NULL;

	printf("Scanning SANE devices..."); fflush(NULL);
	status = sane_get_devices(&dl,SANE_FALSE);
	if (status!=SANE_STATUS_GOOD) {
		printf("Cannot get sane devices!\n"); fflush(NULL);
		return 1;
	}
	printf("OK\n"); fflush(NULL);
	*device_list = dl;

	while (device_list[s]) s++;
	*size = s;

	return 0;
}


/** Function GetDevices
 *  @brief Request all devices from SANE and return their SANE_Device::name
 *
 * @param[in/out]	list					NULL terminated array of strings holding device names
 * @param[in]		allocateFunc		memmory allocate function
 * @return									The number of devices
 *
 *  \todo {
 *  name,vendor,model,type should probably be cached someplace.
 *  sane_get_devices() is an expensive function [up to a few seconds?]
 *  Only name is used now. }
 */
int     GetDevices_                   ( char            *** list,
                                       oyAlloc_f           allocateFunc )
{
	int status, pnm_status, version, len, i, l = 0;
	char ** names = NULL,
		  ** vendors = NULL,
		  ** models = NULL,
		  ** types = NULL,
		  ** all = NULL;
	const SANE_Device ** device_list = NULL;
	SANE_Handle handle;
#if 0
	/* If sane_init() is already called by the application, then
	 * this is probably a BUG! FIXME*/
	if (!sane) {
		printf("Initialising SANE..."); fflush(NULL);
		status = sane_init(&version,NULL);
		if (status!=SANE_STATUS_GOOD) {
			printf("Cannot initialise sane!\n");
			return -1;
		}
		printf("OK\n"); fflush(NULL);
		sane = SANE_TRUE;
	}

	pnm_status = sane_open( "pnm:0", &handle ); /*Trick to make pnm bakend appear*/
	printf("Scanning SANE devices..."); fflush(NULL);
	status = sane_get_devices(&device_list,SANE_FALSE);
	if (status!=SANE_STATUS_GOOD) {
		printf("Cannot get sane devices!\n");
		return -1;
	}
	printf("OK\n"); fflush(NULL);

	if (pnm_status == SANE_STATUS_GOOD)
		sane_close(handle);

	while (device_list[l]) l++;
	len = l + 1;

  names = allocateFunc( len*sizeof(char*) );
  vendors = allocateFunc( len*sizeof(char*) );
  models = allocateFunc( len*sizeof(char*) );
  types = allocateFunc( len*sizeof(char*) );

  memset( names, 0, len*sizeof(char*) );
  memset( vendors, 0, len*sizeof(char*) );
  memset( models, 0, len*sizeof(char*) );
  memset( types, 0, len*sizeof(char*) );

  for (i=0; i<l; i++) {
	  names[i] = allocateFunc( strlen(device_list[i]->name)+1 ); strcpy( names[i], device_list[i]->name );
	  vendors[i] = allocateFunc( strlen(device_list[i]->vendor)+1 ); strcpy( vendors[i], device_list[i]->vendor );
	  models[i] = allocateFunc( strlen(device_list[i]->model)+1 ); strcpy( models[i], device_list[i]->model );
	  types[i] = allocateFunc( strlen(device_list[i]->type)+1 ); strcpy( types[i], device_list[i]->type );
  }

  all = allocateFunc( 4*l*sizeof(char*)+1 );
  memcpy(all,names,l*sizeof(char*));
  memcpy(all+l,vendors,l*sizeof(char*));
  memcpy(all+2*l,models,l*sizeof(char*));
  memcpy(all+3*l,types,l*sizeof(char*));
  all[4*l] = NULL;

  *list = all;
  return l;
#endif
}

/** Function Configs_FromPattern
 *  @brief   CMM_NICK oyCMMapi8_s scanner devices
 *
 *  @param[in] 	registration	a string to compare ??????
 *  @param[in]		options			read what to do from the options object
 *  @param[out]	s					Return a configuration for each device found
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int              Configs_FromPattern ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
  oyConfig_s * device = NULL;
  oyConfigs_s * devices = NULL;
  oyOption_s * o = 0;
  int i, num_devices, error;
  int driver_version = 0;
  const char * device_name = 0,
             * command_list = 0,
             * command_properties = 0;
  const SANE_Device ** device_list = NULL;

  int rank = oyFilterRegistrationMatch( _api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;

  /* "error handling" section */
  if(rank==0) {
	  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
			  "Registration match Failed. Options:\n%s", _DBG_ARGS_,
			  oyOptions_GetText( options, oyNAME_NICK )
			  );
	  return 1;
  }
  if(s == NULL) {
	  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
			  "oyConfigs_s is NULL! Options:\n%s", _DBG_ARGS_,
			  oyOptions_GetText( options, oyNAME_NICK )
			  );
	  return 1;
  }
  if(*s != NULL) {
	  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
			  "Devices struct already present! Options:\n%s", _DBG_ARGS_,
			  oyOptions_GetText( options, oyNAME_NICK )
			  );
	  return 1;
  }

  /* "help" call section */
  if(oyOptions_FindString( options, "command", "help" ) || !options || !oyOptions_Count( options ))
  {
    /** oyMSG_WARN should make shure our message is visible. */
    ConfigsFromPatternUsage( (oyStruct_s*)options );
    return 0;
  }

	/*Handle "driver_version" option*/
	/*Make sure we get the SANE version*/
  	error = oyOptions_FindInt( options, "driver_version", 0, &driver_version );
	if (error != 0) {
  		message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
		  "No SANE driver_version present. Options:\n%s", _DBG_ARGS_,
		  oyOptions_GetText( options, oyNAME_NICK )
		  );
  		ConfigsFromPatternUsage( (oyStruct_s*)options );
  		return error;
	}

    command_list = oyOptions_FindString( options, "command", "list" );
    command_properties = oyOptions_FindString( options, "command", "properties" );
    device_name = oyOptions_FindString( options, "device_name", 0 );

    if(command_list) {
   /* "list" call section */

    	devices = oyConfigs_New(0);
      error = GetDevices( &device_list, &num_devices );

      for (i = 0; i<num_devices; ++i) {
			const char* sane_name = device_list[i]->name;
			const char* sane_model = device_list[i]->model;
		  /*Handle "device_name" option [IN]*/
        /* if current device does not match the requested, try the next */
        if(device_name && strcmp(device_name, sane_name) != 0)
          continue;

        device = oyConfig_New( CMM_BASE_REG, 0 );

		  /*Handle "device_name" option [OUT]*/
        error = oyOptions_SetFromText( &device->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       sane_name, OY_CREATE_NEW );

		  /*Handle "oyNAME_NAME" option*/
        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
			  error = oyOptions_SetFromText( &device->data,
                                       CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                       sane_model, OY_CREATE_NEW );

		  /*Handle "device_context" option*/ //TODO
        if(oyOptions_FindString( options, "device_context", 0 )) {}
		  /*Handle "device_handle" option*/ //TODO
        if(oyOptions_FindString( options, "device_handle", 0 )) {}

        device->rank_map = oyRankMapCopy( _rank_map,
                                                device->oy_->allocateFunc_);

        oyConfigs_MoveIn( devices, &device, -1 );
      }

		//Handle errors: FIXME
      if(error <= 0)
        *s = devices;

      return error;
    } else if (command_properties) {
		 /* "properties" call section */
      //texts_n = GetDevices( &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(device_name&& strcmp(device_name, texts[i]) != 0)
          continue;

        device = oyConfig_New( CMM_BASE_REG, 0 );
        error = !device;

        if(error <= 0)
        error = oyOptions_SetFromText( &device->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );

        error = DeviceFromName_( device_name, options, &device, texts, texts_n );

        if(error <= 0 && device)
          device->rank_map = oyRankMapCopy( _rank_map,
                                                device->oy_->allocateFunc_);
        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        *s = devices;

      return error;
    } else {
		 /*wrong or no command*/
	 }
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
 *  @brief    oyRankPad map for mapping device to configuration informations
 *
 *  This is the static part for the well known options. The final array will
 *  be created by the oyCreateRankMap_() function.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyRankPad _rank_map[] = {
	/* Scanner H/W information */
  {"device_name", 2, -1, 0},           /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"serial", 10, 0, 0},                /**< currently not avaliable */
  {"host", 1, 0, 0},                   /**< currently only local or remote */
  {"system_port", 2, 0, 0},            /**< good to match */
  {"driver_version", 2, 0, 0},            /**< good to match */

  /* User supplied information */
  {"media", 1, -1, 0},                 /**< type of paper/film/slide/... */
  {0,0,0,0}                            /**< end of list */
};

/** @instance _api8
 *  @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyCMMapi8_s _api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  CMMInit,                   /**< oyCMMInit_f      oyCMMInit */
  CMMMessageFuncSet,         /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  CMMCanHandle,              /**< oyCMMCanHandle_f oyCMMCanHandle */

  CMM_BASE_REG,              /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  Configs_FromPattern,       /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  Config_Check,              /**< oyConfig_Check_f oyConfig_Check */
  _rank_map                  /**< oyRankPad ** rank_map */
};




/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  \todo { Add usage info }
 */
const char * GetText                 ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos Scanner");
    else
      return _("The scanner (hopefully)usefull backend of Oyranos.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("orion");
    else if(type == oyNAME_NAME)
      return _("Yiannis Belias");
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
const char * _texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance _cmm_module
 *  @brief    CMM_NICK module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/06/23
 */
oyCMMInfo_s _cmm_module = {

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
  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

/* Helper functions */

/** @internal
 * @brief Create a rank map from a scanner handle
 *
 * @param[in]	device_handle				SANE_Handle
 * @param[out]	rank_map						All scanner options affecting color as a rank map
 *
 * \todo { Untested }
 */
int oyCreateRankMap_ ( SANE_Handle device_handle, oyRankPad** rank_map )
{
	oyRankPad * rm = NULL;

	const SANE_Option_Descriptor *opt = NULL;
	SANE_Int num_options = 0;
	SANE_Status status;
	
	unsigned int opt_num = 0, i = 0, chars = 0;

	/* Get the nuber of scanner options */
	status = sane_control_option(device_handle, 0, SANE_ACTION_GET_VALUE, &num_options, 0);
	if (status != SANE_STATUS_GOOD) {
    	message( oyMSG_WARN, 0, "%s()\n Unable to determine option count\n", __func__ );
		return -1;
	}

	/* we allocate enough memmory to hold all options */
	rm = calloc(num_options,sizeof(oyRankPad));
	memset(rm,0,sizeof(oyRankPad)*num_options);

	for (opt_num = 1; opt_num < num_options; opt_num++) {
		opt = sane_get_option_descriptor(device_handle, opt_num);
		if (opt->cap & SANE_CAP_COLOUR) {
			rm[i].key = (char*)malloc( strlen(opt->name)+1 );
			strcpy(rm[i].key, opt->name);
			rm[i].match_value = 5;
			rm[i].none_match_value = -5;
			i++;
		}
	}

	num_options = i + sizeof(_rank_map)/sizeof(oyRankPad); /* color options + static options */
	/* resize rm array to hold only needed options */
	*rank_map = realloc( rm, num_options*sizeof(oyRankPad) );
	/* copy static options at end of new rank map */
	memcpy( *rank_map+i, _rank_map, sizeof(_rank_map) );

	return 0;
}
