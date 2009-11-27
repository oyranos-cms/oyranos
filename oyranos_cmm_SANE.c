/** @file oyranos_cmm_SANE.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Yiannis Belias
 *
 *  @brief    Oyranos SANE device backend for Oyranos
 *  @internal
 *  @author   Yiannis Belias <orion@linux.gr>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/07/05
 */

#include <oyranos/oyranos_cmm.h>
#include <sane/sane.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SANE_help.c"
/* --- internal definitions --- */

#define DBG printf("%s: %d\n", __FILE__, __LINE__ ); fflush(NULL);
#define PRFX "config.scanner.SANE: "
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
#define ConfigsFromPatternUsage catCMMfunc( SANE , ConfigsFromPatternUsage )
#define DeviceInfoFromContext_  catCMMfunc( SANE , DeviceInfoFromContext_ )
#define GetDevices              catCMMfunc( SANE , GetDevices )
#define _api8                   catCMMfunc( SANE , _api8 )
#define _rank_map               catCMMfunc( SANE , _rank_map )
#define Configs_FromPattern     catCMMfunc( SANE , Configs_FromPattern )
#define Config_Rank             catCMMfunc( SANE , Config_Rank )
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

int ColorInfoFromHandle(const SANE_Handle device_handle, oyOptions_s **options);
int CreateRankMap_(SANE_Handle device_handle, oyRankPad ** rank_map);
int sane_release_handle(oyPointer * handle_ptr);

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

   printf(PRFX "Scanning SANE devices...");
   fflush(NULL);
   status = sane_get_devices(&dl, SANE_FALSE);
   if (status != SANE_STATUS_GOOD) {
      message(oyMSG_WARN, 0,
              "%s()\n Cannot get sane devices: %s\n",
              __func__, sane_strstatus(status));
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
   int i, num_devices, error = 0, status;
   int driver_version = 0;
   bool call_sane_exit = false;
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

   command_list = oyOptions_FindString(options, "command", "list");
   command_properties = oyOptions_FindString(options, "command", "properties");
   device_name = oyOptions_FindString(options, "device_name", 0);
   context_opt = oyOptions_Find(options, "device_context");
   handle_opt = oyOptions_Find(options, "device_handle");

   /*Handle "driver_version" option [IN] */
   error = oyOptions_FindInt(options, "driver_version", 0, &driver_version);
   if (driver_version > 0) /*driver_version is provided*/
      version_opt = oyOptions_Find(options, "driver_version");
   else { /*we have to call sane_init()*/
      status = sane_init(&driver_version, NULL);
      if (status == SANE_STATUS_GOOD) {
         printf(PRFX "SANE v.(%d.%d.%d) init...OK\n",
                SANE_VERSION_MAJOR(driver_version),
                SANE_VERSION_MINOR(driver_version),
                SANE_VERSION_BUILD(driver_version));
         if (error == 0) { /*we've been given a driver_version==0*/
            version_opt = oyOption_New(CMM_BASE_REG OY_SLASH "driver_version", 0); //TODO deallocate
            oyOption_SetFromInt(version_opt, driver_version, 0, 0);
         } else if (!context_opt && !handle_opt) /*no driver_version, nor other options*/
            call_sane_exit = true; /*when list call is over*/
            //TODO: Since driver_version is calculated anyway, why not add it to options?
      } else {
        message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
                "Unable to init SANE. Giving up.[%s] Options:\n%s", _DBG_ARGS_,
                sane_strstatus(status), oyOptions_GetText(options, oyNAME_NICK));
        return 1;
      }
   }

   devices = oyConfigs_New(0);
   if (command_list) {
      /* "list" call section */

      error = GetDevices(&device_list, &num_devices);
      //FIXME #1: If a user provides *only* a device_name option (or && device_handle),
      //then there is no need to call GetDevices() above ---^
      //FIXME #2: If a user provides a device_name that is not found
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
         if (version_opt) {
            oyOption_s *tmp = oyOption_Copy(version_opt, 0);
         //FIXME #4: Backend protocol states that driver_version goes to ::backend_core
            oyOptions_MoveIn(device->data, &tmp, -1);
         }

         /*Handle "device_name" option [OUT] */
         error = oyOptions_SetFromText(&device->backend_core,
                                       CMM_BASE_REG OY_SLASH "device_name", sane_name, OY_CREATE_NEW);

         /*Handle "oyNAME_NAME" option */
         if (oyOptions_Find(options, "oyNAME_NAME"))
            error = oyOptions_SetFromText(&device->data,
                                          CMM_BASE_REG OY_SLASH "oyNAME_NAME", sane_model, OY_CREATE_NEW);

         /*Handle "device_context" option */
         //FIXME #3a: Backend protocol states that device_context is *always* returned
         if (context_opt) {
            oyBlob_s *context_blob = oyBlob_New(NULL);
            oyOption_s *context_opt = oyOption_New(CMM_BASE_REG OY_SLASH "device_context", 0);

            oyBlob_SetFromData(context_blob, (oyPointer) device_list[i], sizeof(SANE_Device), "sane");
            oyOption_StructMoveIn(context_opt, (oyStruct_s **) & context_blob);
            oyOptions_MoveIn(device->backend_core, &context_opt, -1);
         }
         /*Handle "device_handle" option */
         if (handle_opt) {
            oyCMMptr_s *handle_ptr = NULL;
            SANE_Handle h;
            status = sane_open(sane_name, &h);
            if (status == SANE_STATUS_GOOD) {
               handle_ptr = oyCMMptr_New(allocateFunc);
               oyCMMptr_Set(handle_ptr,
                            "SANE",
                            "handle",
                            (oyPointer)h,
                            "sane_release_handle",
                            sane_release_handle);
               oyOptions_MoveInStruct(&(device->backend_core),
                                      CMM_BASE_REG OY_SLASH "device_handle",
                                      (oyStruct_s **) &handle_ptr, OY_CREATE_NEW);
            } else
               printf(PRFX "Unable to open sane device \"%s\": %s\n", sane_name, sane_strstatus(status));
         }

         device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);

         oyConfigs_MoveIn(devices, &device, -1);
      }

      //Handle errors: FIXME
      //if(error <= 0)
      *s = devices;

      if (call_sane_exit) {
         printf(PRFX "sane_exit()\n");
         sane_exit();
      }

      return 0;
   } else if (command_properties) {
      /* "properties" call section */
      const SANE_Device *device_context = NULL;
      SANE_Handle device_handle;

      /*Return a full list of scanner H/W &
       * SANE driver S/W color options
       * with the according rank map */

      if (!device_name) {
         printf(PRFX "device_name is mandatory for properties command.\n");
         return 1;
      }
      device = oyConfig_New(CMM_BASE_REG, 0);

      /*Handle "driver_version" option [OUT] */
      if (version_opt)
         //FIXME #5: "list" call uses a temporary option
         oyOptions_MoveIn(device->backend_core, &version_opt, -1);

      /*1a. Get the "device_context"*/
      //FIXME #3b: Backend protocol states that device_context is *always* returned
      //by list call, so it should be avaliable to properties call.
      //[Unless properties is called but not list]
      if (!context_opt) {
         error = GetDevices(&device_list, &num_devices);
         device_context = *device_list;
         while (device_context) {
            if (strcmp(device_name,device_context->name) == 0)
               break;
            device_context++;
         }
         if (!device_context) {
            printf(PRFX "device_name does not match any installed device.\n");
            return 1;
         }
      } else {
         device_context = (SANE_Device*)oyOption_GetData(context_opt, NULL, allocateFunc);
      }

      /*1b. Use the "device_context"*/
      error = DeviceInfoFromContext_(device_context, &(device->backend_core));

      /*2a. Get the "device_handle"*/
      if (!handle_opt) {
         status = sane_open( device_name, &device_handle );
         if (status != SANE_STATUS_GOOD) {
            printf(PRFX "Unable to open sane device \"%s\": %s\n", device_name, sane_strstatus(status));
            return 1;
         }
      } else {
         device_handle = (SANE_Handle)((oyCMMptr_s*)handle_opt->value->oy_struct)->ptr;
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
 
      //FIXME #6: Call sane exit
      return 0;
   } else {
      /*wrong or no command */
      /*TODO Message+deallocation*/
   }
}

/** Function Configs_Modify
 *  @brief   oyCMMapi8_s SANE scanner manipulation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 *
 *  \todo { Unimplemented }
 */
int Configs_Modify(oyConfigs_s * devices, oyOptions_s * options)
{
   oyOption_s *context_opt = NULL,
              *handle_opt = NULL,
              *version_opt = NULL;
   oyOption_s *version_opt_dev = NULL;
   oyConfig_s *device = NULL;
   int i, num_devices, error = 0, status;
   int call_sane_exit = 0;
   const char *device_name = NULL,
              *command_list = NULL,
              *command_properties = NULL;
   const SANE_Device **device_list = NULL;

   oyAlloc_f allocateFunc = malloc;

   /* "error handling" section */
   if (!devices || !oyConfigs_Count(devices)) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "No devices given! Options:\n%s", _DBG_ARGS_,
              oyOptions_GetText(options, oyNAME_NICK) );
      return 1;
   }

   /* "help" call section */
   if (oyOptions_FindString(options, "command", "help") || !options || !oyOptions_Count(options)) {
    /** oyMSG_WARN should make shure our message is visible. */
      ConfigsFromPatternUsage((oyStruct_s *) options);
      return 0;
   }

   num_devices = oyConfigs_Count(devices);
   command_list = oyOptions_FindString(options, "command", "list");
   command_properties = oyOptions_FindString(options, "command", "properties");
   context_opt = oyOptions_Find(options, "device_context");
   handle_opt = oyOptions_Find(options, "device_handle");

   /* Now we get some options [IN], and we already have some devices with
    * possibly already assigned options. Those provided through the input
    * oyOptions_s should take presedence over ::data & ::backend_core ones.
    * OTOH, all device_* options have a 1-1 relationship meaning if
    * one changes, probably all other should. So the simplest [naive] approach
    * would be to ignore all device_* options [IN] that are already in device.
    * Except from driver_version which has a special meaning.
    */

   /* Handle "driver_version" option [IN] */
   /* Check the first device to see if a positive driver_version is provided. */
   /* If not, consult the input options */
   device = oyConfigs_Get(devices, 0);
   version_opt_dev = oyConfig_Find(device, "driver_version");
   if (version_opt_dev && oyOption_GetValueInt(version_opt_dev, 0) > 0)
      call_sane_exit = 0;
   else
      check_driver_version(options, &version_opt, &call_sane_exit);
   oyConfig_Release(&device);
   oyOption_Release(&version_opt_dev);

   if (command_list) {
      /* "list" call section */

      for (i = 0; i < num_devices; ++i) {
         oyOption_s *name_opt_dev = NULL,
                    *handle_opt_dev = NULL,
                    *context_opt_dev = NULL;
         const char *sane_name = NULL,
                    *sane_model = NULL;
         device = oyConfigs_Get(devices, i); //TODO deallocate

         /*Handle "driver_version" option [OUT] */
         version_opt_dev = oyConfig_Find(device, "driver_version");
         if (!version_opt_dev && version_opt) {
            oyOption_s *tmp = oyOption_Copy(version_opt, 0); //TODO does it need deallocation?
            oyOptions_MoveIn(device->backend_core, &tmp, -1);
         }
         oyOption_Release(&version_opt_dev);

         /*Handle "device_context" option */
         /*This is always provided by Configs_FromPattern()
          * [or should be alternatively by the user].
          * Configs_Modify() will not scan for SANE devices
          * because it takes too long*/
         context_opt_dev = oyConfig_Find(device, "device_context");
         if (!context_opt_dev) {
            message(oyMSG_WARN, options, _DBG_FORMAT_ ": %s\n",
                    DBG_ARGS_, "The \"device_context\" option is missing!");
            return 1;
         }
         device_context = (SANE_Device*)oyOption_GetData(context_opt_dev, NULL, allocateFunc);
         sane_name  = device_context->name;
         sane_model = device_context->model;

         /*Handle "oyNAME_NAME" option */
         name_opt_dev = oyConfig_Find(device, "oyNAME_NAME");
         if (!name_opt_dev && oyOptions_Find(options, "oyNAME_NAME"))
            error = oyOptions_SetFromText(&device->data,
                                          CMM_BASE_REG OY_SLASH "oyNAME_NAME", sane_model, OY_CREATE_NEW);

         /*Handle "device_handle" option */
         handle_opt_dev = oyConfig_Find(device, "device_handle");
         if (!handle_opt_dev && handle_opt) {
            oyCMMptr_s *handle_ptr = NULL;
            SANE_Handle h;
            status = sane_open(sane_name, &h);
            if (status == SANE_STATUS_GOOD) {
               handle_ptr = oyCMMptr_New(allocateFunc);
               oyCMMptr_Set(handle_ptr,
                            "SANE",
                           "handle",
                            (oyPointer)h,
                            "sane_release_handle",
                            sane_release_handle);
               oyOptions_MoveInStruct(&(device->backend_core),
                                      CMM_BASE_REG OY_SLASH "device_handle",
                                      (oyStruct_s **) &handle_ptr, OY_CREATE_NEW);
            } else
               printf(PRFX "Unable to open sane device \"%s\": %s\n", sane_name, sane_strstatus(status));
         }

         /*Create static rank_map, if not already there*/
         if (!device->rank_map)
            device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);
      }

      //Handle errors: FIXME

      if (call_sane_exit) {
         printf(PRFX "sane_exit()\n");
         sane_exit();
      }

      return 0;
   } else if (command_properties) {
      /* "properties" call section */

      /*Return a full list of scanner H/W &
       * SANE driver S/W color options
       * with the according rank map */

      for (i = 0; i < num_devices; ++i) {
         const SANE_Device *device_context = NULL;
         SANE_Handle device_handle;
         oyOption_s *name_opt_dev = NULL,
                    *handle_opt_dev = NULL,
                    *context_opt_dev = NULL;
         oyConfig_s *device_new = NULL;
         oyRankPad *dynamic_rank_map = NULL;
         //const char *sane_name = NULL,
         //           *sane_model = NULL;

         /* All previous device properties are considered obsolete
          * and a new device is created. Basic options are moved from
          * the old to new device */
         device = oyConfigs_Get(devices, i); //TODO deallocate
         device_new = oyConfig_New(CMM_BASE_REG, 0);

         /*Handle "driver_version" option [OUT] */
         if (version_opt) {
            oyOption_s *tmp = oyOption_Copy(version_opt, 0); //TODO does it need deallocation?
            oyOptions_MoveIn(device_new->backend_core, &tmp, -1);
         }

         /* 1. Get the "device_name" from old device */
         name_opt_dev = oyConfig_Find(device, "device_name");
         oyOptions_MoveIn(device_new->backend_core, &name_opt_dev, -1);

         /* 2. Get the "device_context" from old device */
         /* It should be there, see "list" call above */
         context_opt_dev = oyConfig_Find(device, "device_context");
         if (!context_opt_dev) {
            message(oyMSG_WARN, options, _DBG_FORMAT_ ": %s\n",
                    DBG_ARGS_, "The \"device_context\" option is missing!");
            return 1;
         }
         oyOptions_MoveIn(device_new->backend_core, &context_opt_dev, -1);
         device_context = (SANE_Device*)oyOption_GetData(context_opt_dev, NULL, allocateFunc);
         device_name = device_context->name;
         //sane_model = device_context->model;

         /* 3. Get the scanner H/W properties from old device */
         /* FIXME: we only recompute them, just in case they are not in old device */
         error = DeviceInfoFromContext_(device_context, &(device_new->backend_core));

         /* 4. Get the "device_handle" from old device */
         /* If not there, get one from SANE */
         handle_opt_dev = oyConfig_Find(device, "device_handle");
         if (!handle_opt_dev) {
            status = sane_open( device_name, &device_handle );
            if (status != SANE_STATUS_GOOD) {
               printf(PRFX "Unable to open sane device \"%s\": %s\n", device_name, sane_strstatus(status));
               return 1;
            }
         } else {
            device_handle = (SANE_Handle)((oyCMMptr_s*)handle_opt_dev->value->oy_struct)->ptr;
            oyOptions_MoveIn(device_new->backend_core, &handle_opt_dev, -1);
         }

         /* Use the device_handle to get the device color options */
         error = ColorInfoFromHandle(device_handle, &(device_new->backend_core));

         /*5. Create the rank map*/
         error = CreateRankMap_(device_handle, &dynamic_rank_map);
         device_new->rank_map = oyRankMapCopy(dynamic_rank_map, device_new->oy_->allocateFunc_);

         /* Remove old, add new device */
         oyConfigs_ReleaseAt(devices, i);
         oyConfigs_MoveIn(devices, &device_new, -1);

         free(dynamic_rank_map);
         free(device_context);
      }
 
      return 0;
   } else {
      /*wrong or no command */
      /*TODO Message+deallocation*/
   }
}

/** Function Config_Rank
 *  @brief   CMM_NICK oyCMMapi8_s device check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int Config_Rank(oyConfig_s * config)
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
   0,                                                                 /**< next */
   CMMInit,                                                           /**< oyCMMInit_f      oyCMMInit */
   CMMMessageFuncSet,                                                 /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
   CMM_BASE_REG,                                                      /**< registration */
   {0, 1, 0},                                                         /**< int32_t version[3] */
   {0, 1, 10},                                                        /**< int32_t module_api[3] */
   0,                                                                 /**< char * id_ */
   0,                                                                 /**< oyCMMapi5_s * api5_ */
   Configs_FromPattern,                                               /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
   Configs_Modify,                                                    /**< oyConfigs_Modify_f oyConfigs_Modify */
   Config_Rank,                                                       /**< oyConfig_Rank_f oyConfig_Rank */
   _rank_map                                                          /**< oyRankPad ** rank_map */
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
         return _("orionas");
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
   int error = 0, i;
   unsigned int opt_num = 0, count;
   char cmm_base_reg[] = CMM_BASE_REG OY_SLASH;

   /* We got a device, find out how many options it has */
   status = sane_control_option(device_handle, 0, SANE_ACTION_GET_VALUE, &num_options, 0);
   if (status != SANE_STATUS_GOOD) {
      message(oyMSG_WARN, 0,
              "%s()\n Unable to determine option count: %s\n",
              __func__, sane_strstatus(status));
      return -1;
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
               printf(PRFX "Do not know what to do with option %d\n", opt->type);
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
      message(oyMSG_WARN, 0,
              "%s()\n Unable to determine option count: %s\n",
              __func__, sane_strstatus(status));
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

/** @internal
 * @brief Release the SANE_Handle.
 *
 * This function is a oyPointer_release_f and is used in the
 * oyCMMptr_s device handle.
 *
 * @param[in]	handle_ptr				SANE_Handle
 * @return 0 for success
 *
 */
int sane_release_handle(oyPointer *handle_ptr)
{
   SANE_Handle h = (SANE_Handle)*handle_ptr;
   sane_close(h);

   message(oyMSG_DBG, 0,
           "%s() deleting sane handle: %p\n",
           __func__, h);

   return 0;
}

/** @internal
 * @brief Decide if sane_init/exit will be called
 *
 * 1. Checks for driver_version in options and
 * 2. Calls sane_init if needed
 *
 * @param[in]  options     The input options
 * @param[out] version_opt_p
 * @param[out] call_sane_exit
 * @return 0 for success
 *
 */
int check_driver_version(oyOptions_s *options, oyOption_s **version_opt_p, int *call_sane_exit)
{
   int driver_version = 0;
   oyOption_s *context_opt = oyOptions_Find(options, "device_context");
   oyOption_s *handle_opt = oyOptions_Find(options, "device_handle");
   int error = oyOptions_FindInt(options, "driver_version", 0, &driver_version);

   if (!error && driver_version > 0) /*driver_version is provided*/
      *version_opt_p = oyOptions_Find(options, "driver_version");
   else { /*we have to call sane_init()*/
      status = sane_init(&driver_version, NULL);
      if (status == SANE_STATUS_GOOD) {
         printf(PRFX "SANE v.(%d.%d.%d) init...OK\n",
                SANE_VERSION_MAJOR(driver_version),
                SANE_VERSION_MINOR(driver_version),
                SANE_VERSION_BUILD(driver_version));

         *version_opt_p = oyOption_New(CMM_BASE_REG OY_SLASH "driver_version", 0); //TODO deallocate
         oyOption_SetFromInt(*version_opt_p, driver_version, 0, 0);
         if (error && /*we've not been given a driver_version*/
             !context_opt && !handle_opt) /*we've not been given other options*/
            *call_sane_exit = 1; /*when we are over*/
      } else {
        message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
                "Unable to init SANE. Giving up.[%s] Options:\n%s", _DBG_ARGS_,
                sane_strstatus(status), oyOptions_GetText(options, oyNAME_NICK));
        return 1;
      }
   }
   return 0;
}
