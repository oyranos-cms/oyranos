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
#define DeviceInfoFromContext_  catCMMfunc( SANE , DeviceInfoFromContext_ )
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

int CMMInit()
{
   int error = 0;
   return error;
}

oyPointer CMMallocateFunc(size_t size)
{
   oyPointer p = 0;
   if (size)
      p = malloc(size);
   return p;
}

void CMMdeallocateFunc(oyPointer mem)
{
   if (mem)
      free(mem);
}

/** @func  CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int CMMMessageFuncSet(oyMessage_f message_func)
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
int CMMCanHandle(oyCMMQUERY_e type, uint32_t value)
{
   return 0;
}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void ConfigsFromPatternUsage(oyStruct_s * options)
{
    /** oyMSG_WARN should make shure our message is visible. */
   message(oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
           _DBG_ARGS_, "The following help text informs about the communication protocol.");
   message(oyMSG_WARN, options, "%s()\n%s", __func__, help_message);

   return;
}

/** @internal
 * @brief Put all the scanner hardware information in a oyOptions_s object
 *
 * @param[in]  device_context    The SANE_Device to get the options from
 * @param[out] options           The options object to store the H/W info
 *
 * \todo {
 *         * Untested
 *         * Error handling
 *       }
 */
int DeviceInfoFromContext_(const SANE_Device * device_context, oyOptions_s **options)
{
   const char *device_name = device_context->name,
              *manufacturer = device_context->vendor,
              *model = device_context->model,
              *serial = NULL,
              *host = NULL,
              *system_port = NULL;
   int error = 0;

   serial = "unsupported";
   /* if device string starts with net, it is a remote device */
   if (strncmp(device_name,"net:",4) == 0)
      host = "remote";
   else
      host = "localhost";
   /*TODO scsi/usb/parallel port? */
   system_port = "TODO";

   /*TODO make sure the strings get copied, not pointed */
   OPTIONS_ADD(options, device_name)
   OPTIONS_ADD(options, manufacturer)
   OPTIONS_ADD(options, model)
   OPTIONS_ADD(options, serial)
   OPTIONS_ADD(options, system_port)
   OPTIONS_ADD(options, host)

   return error;
}
/** Function GetDevices
 *  @brief Request all devices from SANE
 *
 * @param[out]    device_list       pointer to -> NULL terminated array of SANE_Device's
 * @param[out]    size              the number of devices
 * @return                          0 OK - else error
 *
 */
int GetDevices(const SANE_Device *** device_list, int *size)
{
   int status, s = 0;
   const SANE_Device **dl = NULL;

   printf("Scanning SANE devices...");
   fflush(NULL);
   status = sane_get_devices(&dl, SANE_FALSE);
   if (status != SANE_STATUS_GOOD) {
      printf("Cannot get sane devices!\n");
      fflush(NULL);
      return 1;
   }
   *device_list = dl;

   while (dl[s]) s++;
   *size = s;

   printf("OK [%d]\n", s);
   fflush(NULL);

   return 0;
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
int Configs_FromPattern(const char *registration, oyOptions_s * options, oyConfigs_s ** s)
{
   oyConfig_s *device = NULL;
   oyConfigs_s *devices = NULL;
   oyOption_s *context_opt = NULL, *handle_opt = NULL, *version_opt = NULL;
   oyRankPad *dynamic_rank_map = NULL;
   int i, num_devices, error = 0;
   int driver_version = 0;
   const char *device_name = 0, *command_list = 0, *command_properties = 0;
   const SANE_Device **device_list = NULL;

   int rank = oyFilterRegistrationMatch(_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S);
   oyAlloc_f allocateFunc = malloc;

   /* "error handling" section */
   if (rank == 0) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Registration match Failed. Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }
   if (s == NULL) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "oyConfigs_s is NULL! Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }
   if (*s != NULL) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Devices struct already present! Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }

   /* "help" call section */
   if (oyOptions_FindString(options, "command", "help") || !options || !oyOptions_Count(options)) {
    /** oyMSG_WARN should make shure our message is visible. */
      ConfigsFromPatternUsage((oyStruct_s *) options);
      return 0;
   }

   /*Handle "driver_version" option [IN] */
   if (oyOptions_FindInt(options, "driver_version", 0, &driver_version) == 0) {
      version_opt = oyOption_New(CMM_BASE_REG OY_SLASH "driver_version", 0);
      if (driver_version == 0)
         if (sane_init(&driver_version, NULL) == SANE_STATUS_GOOD) {
            /*Handle "driver_version" option [OUT] */
            printf("SANE v.(%d) init...OK\n", driver_version);
            error = oyOption_SetFromInt(version_opt, driver_version, -1, 0);
         } else {
            message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
                    "Unable to init SANE. Giving up. Options:\n%s", _DBG_ARGS_,
                    oyOptions_GetText(options, oyNAME_NICK));
            return 1;
         }
   }
   /*if version==0 => not supplied, else if version>0 => use version_opt*/

   command_list = oyOptions_FindString(options, "command", "list");
   command_properties = oyOptions_FindString(options, "command", "properties");
   device_name = oyOptions_FindString(options, "device_name", 0);
   context_opt = oyOptions_Find(options, "device_context");
   handle_opt = oyOptions_Find(options, "device_handle");

   devices = oyConfigs_New(0);
   if (command_list) {
      /* "list" call section */

      error = GetDevices(&device_list, &num_devices);
      //FIXME #1 If a user provides *only* a device_name option (or && device_handle),
      //then there is no need to call GetDevices()
      //FIXME #2 If a user provides a device_name that is not found
      //by sane_get_devices(), an empty devices object will be returned...

      for (i = 0; i < num_devices; ++i) {
         const char *sane_name = device_list[i]->name;
         const char *sane_model = device_list[i]->model;
         /*Handle "device_name" option [IN] */
         /* if current device does not match the requested, try the next */
         if (device_name && strcmp(device_name, sane_name) != 0)
            continue;

         device = oyConfig_New(CMM_BASE_REG, 0);

         /*Handle "driver_version" option [OUT] */
         if (version_opt)
            oyOptions_MoveIn(device->data, &version_opt, -1);

         /*Handle "device_name" option [OUT] */
         error = oyOptions_SetFromText(&device->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name", sane_name, OY_CREATE_NEW);

         /*Handle "oyNAME_NAME" option */
         if (oyOptions_Find(options, "oyNAME_NAME"))
            error = oyOptions_SetFromText(&device->data,
                                          CMM_BASE_REG OY_SLASH "oyNAME_NAME", sane_model, OY_CREATE_NEW);

         /*Handle "device_context" option */
         if (context_opt) {
            oyBlob_s *context_blob = oyBlob_New(NULL);
            oyOption_s *context_opt = oyOption_New(CMM_BASE_REG OY_SLASH "device_context", 0);

            oyBlob_SetFromData(context_blob, (oyPointer) device_list[i], sizeof(SANE_Device), "sane");
            oyOption_StructMoveIn(context_opt, (oyStruct_s **) & context_blob);
            oyOptions_MoveIn(device->backend_core, &context_opt, -1);
         }
         /*Handle "device_handle" option */
         if (handle_opt) {
            oyBlob_s *handle_blob = NULL; //FIXME Replace with oyCMMptr_s
            SANE_Handle h;

            if (sane_open(sane_name, &h) == SANE_STATUS_GOOD) {
               handle_blob = oyBlob_New(NULL);
               oyBlob_SetFromData(handle_blob, (oyPointer) & h, sizeof(SANE_Handle), "sane");
               oyOptions_MoveInStruct(&(device->backend_core),
                                      CMM_BASE_REG OY_SLASH "device_handle",
                                      (oyStruct_s **) & handle_blob, OY_CREATE_NEW);
            } else
               printf("Unable to open sane device \"%s\"\n", sane_name);
         }

         device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);

         oyConfigs_MoveIn(devices, &device, -1);
      }

      //Handle errors: FIXME
      //if(error <= 0)
      *s = devices;

      return error;
   } else if (command_properties) {
      /* "properties" call section */
      const SANE_Device *device_context = NULL;
      SANE_Handle device_handle;

      /*Return a full list of scanner H/W &
       * SANE driver S/W color options
       * with the according rank map */

      if (!device_name) {
         printf("device_name is mandatory for properties command.\n");
         return 1;
      }
      device = oyConfig_New(CMM_BASE_REG, 0);

      /*Handle "driver_version" option [OUT] */
      if (version_opt)
         oyOptions_MoveIn(device->backend_core, &version_opt, -1);

      /*1a. Get the "device_context"*/
      if (!context_opt) {
         error = GetDevices(&device_list, &num_devices);
         device_context = *device_list;
         while (device_context)
            if (strcmp(device_name,device_context->name) == 0)
               break;
         if (!device_context) {
            printf("device_name does not match any installed device.\n");
            return 1;
         }
      } else {
         device_context = (SANE_Device*)oyOption_GetData(context_opt, NULL, allocateFunc);
      }

      /*1b. Use the "device_context"*/
      error = DeviceInfoFromContext_(device_context, &(device->backend_core));

      /*2a. Get the "device_handle"*/
      if (!handle_opt) {
         error = sane_open( device_name, &device_handle );
         if (error != SANE_STATUS_GOOD) {
            printf("Unable to open sane device \"%s\"\n", device_name);
            return 1;
         }
      } else {
         device_handle = *(SANE_Handle*)oyOption_GetData(handle_opt, NULL, allocateFunc);
      }

      /*2b. Use the "device_handle"*/
      error = ColorInfoFromHandle(device_handle, &(device->backend_core));

      /*3. Create the rank map*/
      error = CreateRankMap_(device_handle, &dynamic_rank_map);
      device->rank_map = oyRankMapCopy(dynamic_rank_map, device->oy_->allocateFunc_);
      oyConfigs_MoveIn(devices, &device, -1);

      free(dynamic_rank_map);

      //Handle errors: FIXME
      //if(error <= 0)
      *s = devices;
 
      return error;
   } else {
      /*wrong or no command */
      /*TODO Message+deallocation*/
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
int Config_Check(oyConfig_s * config)
{
   int error = !config, rank = 1;

   if (!config) {
      message(oyMSG_DBG, (oyStruct_s *) config, _DBG_FORMAT_ "\n " "No config argument provided.\n", _DBG_ARGS_);
      return 0;
   }

   if (error <= 0) {
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
   {"device_name", 2, -1, 0},                     /**< is good */
   {"profile_name", 0, 0, 0},                     /**< non relevant for device properties*/
   {"manufacturer", 1, -1, 0},                    /**< is nice */
   {"model", 5, -5, 0},                           /**< important, should not fail */
   {"serial", 10, 0, 0},                          /**< currently not avaliable */
   {"host", 1, 0, 0},                             /**< currently only local or remote */
   {"system_port", 2, 0, 0},                      /**< good to match */
   {"driver_version", 2, 0, 0},                   /**< good to match */
                                                                      
                                                  /* User supplied information */
   {"media", 1, -1, 0},                           /**< type of paper/film/slide/... */
   {0, 0, 0, 0}                                   /**< end of list */
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
   0, 0, 0,
   0,                        /**< next */

   CMMInit,                  /**< oyCMMInit_f      oyCMMInit */
   CMMMessageFuncSet,        /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
   CMMCanHandle,             /**< oyCMMCanHandle_f oyCMMCanHandle */

   CMM_BASE_REG,             /**< registration */
   {0, 1, 0}
   ,                         /**< int32_t version[3] */
   0,                        /**< char * id_ */

   0,                        /**< oyCMMapi5_s * api5_ */
   Configs_FromPattern,      /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
   Config_Check,             /**< oyConfig_Check_f oyConfig_Check */
   _rank_map                 /**< oyRankPad ** rank_map */
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
const char *GetText(const char *select, oyNAME_e type)
{
   if (strcmp(select, "name") == 0) {
      if (type == oyNAME_NICK)
         return _(CMM_NICK);
      else if (type == oyNAME_NAME)
         return _("Oyranos Scanner");
      else
         return _("The scanner (hopefully)usefull backend of Oyranos.");
   } else if (strcmp(select, "manufacturer") == 0) {
      if (type == oyNAME_NICK)
         return _("orion");
      else if (type == oyNAME_NAME)
         return _("Yiannis Belias");
      else
         return
             _
             ("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
   } else if (strcmp(select, "copyright") == 0) {
      if (type == oyNAME_NICK)
         return _("MIT");
      else if (type == oyNAME_NAME)
         return _("Copyright (c) 2009 Kai-Uwe Behrmann; MIT");
      else
         return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
   } else if (strcmp(select, "help") == 0) {
      if (type == oyNAME_NICK)
         return _("help");
      else if (type == oyNAME_NAME)
         return _("My filter introduction.");
      else
         return _("All the small details for using this module.");
   }
   return 0;
}
const char *_texts[5] = { "name", "copyright", "manufacturer", "help", 0 };

/** @instance _cmm_module
 *  @brief    CMM_NICK module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/06/23
 */
oyCMMInfo_s _cmm_module = {

   oyOBJECT_CMM_INFO_S,/**< ::type; the object type */
   0, 0, 0,            /**< static objects omit these fields */
   CMM_NICK,           /**< ::cmm; the four char filter id */
   (char *)"0.2",      /**< ::backend_version */
   GetText,            /**< ::getText; UI texts */
   (char **)_texts,    /**< ::texts; list of arguments to getText */
   OYRANOS_VERSION,    /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
   (oyCMMapi_s *) & _api8,

  /** ::icon; zero terminated list of a icon pyramid */
   {oyOBJECT_ICON_S, 0, 0, 0, 0, 0, 0, "oyranos_logo.png"},
};

/* Helper functions */

/** @internal
 * @brief Put all the SANE backend color information in a oyOptions_s object
 *
 * @param[in]   device_handle          The SANE_Handle to talk to SANE backend
 * @param[out]  options                The options object to store the Color info to
 *
 * \todo { Untested
 *         error handling }
 */
int ColorInfoFromHandle(const SANE_Handle device_handle, oyOptions_s **options)
{
   const SANE_Option_Descriptor *opt = NULL;
   SANE_Int num_options = 0;
   SANE_Status status;
   int error = 0;
   unsigned int opt_num = 0, i, count;
   char cmm_base_reg[] = CMM_BASE_REG OY_SLASH;

   /* We got a device, find out how many options it has */
   status = sane_control_option(device_handle, 0, SANE_ACTION_GET_VALUE, &num_options, 0);
   if (status != SANE_STATUS_GOOD) {
      fprintf(stderr, "unable to determine option count\n");
      return 1;
   }

   for (opt_num = 1; opt_num < num_options; opt_num++) {
      opt = sane_get_option_descriptor(device_handle, opt_num);
      if (opt->cap & SANE_CAP_COLOUR) {
         void *value = malloc(opt->size);
         char *registration = malloc(sizeof(cmm_base_reg)+strlen(opt->name));

         sprintf(registration, "%s%s", cmm_base_reg, opt->name); //FIXME not optimal

         sane_control_option(device_handle, opt_num, SANE_ACTION_GET_VALUE, value, 0);
         switch (opt->type) {
            case SANE_TYPE_BOOL:
               oyOptions_SetFromInt(options, registration, *(SANE_Bool *) value, 0, OY_CREATE_NEW);
               break;
            case SANE_TYPE_INT:
               if (opt->size == (SANE_Int)sizeof(SANE_Word))
                  oyOptions_SetFromInt(options, registration, *(SANE_Int *) value, 0, OY_CREATE_NEW);
               else {
                  int count = opt->size/sizeof(SANE_Word);
                  oyOption_s *option = oyOption_New(registration, 0);
                  for (i=count-1; i>=0; --i)
                     oyOption_SetFromInt(option, *(SANE_Int *) value+i, i, 0);
                  oyOptions_MoveIn(*options, &option, -1);
               }
               break;
            case SANE_TYPE_FIXED:
               if (opt->size == (SANE_Int)sizeof(SANE_Word)) {
                  oyOption_s *option = oyOption_New(registration, 0);
                  oyOption_SetFromDouble(option, SANE_UNFIX(*(SANE_Fixed *) value), 0, 0);
                  oyOptions_MoveIn(*options, &option, -1);
               } else {
                  int count = opt->size/sizeof(SANE_Word);
                  oyOption_s *option = oyOption_New(registration, 0);
                  for (i=count-1; i>=0; --i)
                     oyOption_SetFromDouble(option, SANE_UNFIX(*(SANE_Fixed *) value+i), i, 0);
                  oyOptions_MoveIn(*options, &option, -1);
               }
               break;
            case SANE_TYPE_STRING:
               oyOptions_SetFromText(options, registration, (const char *)value, OY_CREATE_NEW);
               break;
            default:
               fprintf(stderr, "Do not know what to do with option %d\n", opt->type);
               return 1;
               break;
         }
      }
   }

   return error;
}

/** @internal
 * @brief Create a rank map from a scanner handle
 *
 * @param[in]	device_handle				SANE_Handle
 * @param[out]	rank_map						All scanner options affecting color as a rank map
 *
 * \todo { Untested }
 */
int CreateRankMap_(SANE_Handle device_handle, oyRankPad ** rank_map)
{
   oyRankPad *rm = NULL;

   const SANE_Option_Descriptor *opt = NULL;
   SANE_Int num_options = 0;
   SANE_Status status;

   unsigned int opt_num = 0, i = 0, chars = 0;

   /* Get the nuber of scanner options */
   status = sane_control_option(device_handle, 0, SANE_ACTION_GET_VALUE, &num_options, 0);
   if (status != SANE_STATUS_GOOD) {
      message(oyMSG_WARN, 0, "%s()\n Unable to determine option count\n", __func__);
      return -1;
   }

   /* we allocate enough memmory to hold all options */
   rm = calloc(num_options, sizeof(oyRankPad));
   memset(rm, 0, sizeof(oyRankPad) * num_options);

   for (opt_num = 1; opt_num < num_options; opt_num++) {
      opt = sane_get_option_descriptor(device_handle, opt_num);
      if (opt->cap & SANE_CAP_COLOUR) {
         rm[i].key = (char *)malloc(strlen(opt->name) + 1);
         strcpy(rm[i].key, opt->name);
         rm[i].match_value = 5;
         rm[i].none_match_value = -5;
         i++;
      }
   }

   num_options = i + sizeof(_rank_map) / sizeof(oyRankPad); /* color options + static options */
   /* resize rm array to hold only needed options */
   *rank_map = realloc(rm, num_options * sizeof(oyRankPad));
   /* copy static options at end of new rank map */
   memcpy(*rank_map + i, _rank_map, sizeof(_rank_map));

   return 0;
}
