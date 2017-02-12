/** @file oyranos_cmm_SANE.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009-2010 (C) Yiannis Belias
 *
 *  @brief    Oyranos SANE device backend for Oyranos
 *  @internal
 *  @author   Yiannis Belias <orion@linux.gr>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/07/05
 */

#include "oyranos_cmm.h"
#include "oyranos_i18n.h"
#include "oyObject_s_.h"
#include "oyCMM_s.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMui_s_.h"

#include <sane/sane.h>
#ifdef HAVE_LCMS_never
#include <lcms.h>
#endif

#include <locale.h>   /* LC_NUMERIC */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#include "SANE_help.c"
/* --- internal definitions --- */

#define PRFX "scanner.SANE: "
/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "SANE"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "device" OY_SLASH "config.icc_profile.scanner." CMM_NICK
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( SANE , CMMInit )
#define CMMallocateFunc         catCMMfunc( SANE , CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( SANE , CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( SANE , CMMMessageFuncSet )
#define ConfigsFromPatternUsage catCMMfunc( SANE , ConfigsFromPatternUsage )
#define DeviceInfoFromContext_  catCMMfunc( SANE , DeviceInfoFromContext_ )
#define GetDevices              catCMMfunc( SANE , GetDevices )
#define _api8                   catCMMfunc( SANE , _api8 )
#define Configs_FromPattern     catCMMfunc( SANE , Configs_FromPattern )
#define Configs_Modify          catCMMfunc( SANE , Configs_Modify )
#define Config_Rank             catCMMfunc( SANE , Config_Rank )
#define GetText                 catCMMfunc( SANE , GetText )
#define _texts                  catCMMfunc( SANE , _texts )
#define _cmm_module             catCMMfunc( SANE , _cmm_module )
#define _api8_ui                catCMMfunc( oyRE, _api8_ui )
#define Api8UiGetText           catCMMfunc( oyRE, Api8UiGetText )
#define _api8_ui_texts          catCMMfunc( oyRE, _api8_ui_texts )
#define _api8_icon              catCMMfunc( oyRE, _api8_icon )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__

static int _initialised = 0;
const char * GetText                 ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

oyMessage_f message = 0;

extern oyCMMapi8_s_ _api8;

int ColorInfoFromHandle(const SANE_Handle device_handle, oyOptions_s **options);
int sane_release_handle(oyPointer * handle_ptr);
int check_driver_version(oyOptions_s *options, oyOption_s **version_opt_p, int *call_sane_exit);

/* --- implementations --- */

int CMMInit( oyStruct_s * filter )
{
  int error = 0;
  const char * rfilter = "config.icc_profile.scanner.SANE";

  if(!_initialised)
    error = oyDeviceCMMInit( filter, rfilter );

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
   oyOption_s *context_opt = NULL,
              *handle_opt = NULL,
              *version_opt = NULL,
              *name_opt = NULL;
   int i, num_devices, g_error = 0, status, call_sane_exit = 0;
   const char *device_name = 0, *command_list = 0, *command_properties = 0;
   const SANE_Device **device_list = NULL;
   int rank;
   oyAlloc_f allocateFunc = malloc;

   printf(PRFX "Entering %s(). Options:\n%s", __func__, oyOptions_GetText(options, oyNAME_NICK));

   rank = oyFilterRegistrationMatch(_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S);
   command_list = oyOptions_FindString(options, "command", "list");
   command_properties = oyOptions_FindString(options, "command", "properties");
   device_name = oyOptions_FindString(options, "device_name", 0);

   /* "error handling" section */
   if (rank == 0) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Registration match Failed. Options:\n%s", _DBG_ARGS_,
              oyOptions_GetText(options, oyNAME_NICK));
      return 1;
   }
   if (s == NULL) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "oyConfigs_s is NULL! Options:\n%s", _DBG_ARGS_,
              oyOptions_GetText(options, oyNAME_NICK));
      return 1;
   }
   if (*s != NULL) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Devices struct already present! Options:\n%s", _DBG_ARGS_,
              oyOptions_GetText(options, oyNAME_NICK));
      return 1;
   }

   if (!device_name && command_properties) {
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Device_name is mandatory for properties command:\n%s",
              _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK));
      return 1;
   }

   /* "help" call section */
   if (oyOptions_FindString(options, "command", "help") || !options || !oyOptions_Count(options)) {
    /** oyMSG_WARN should make shure our message is visible. */
      ConfigsFromPatternUsage((oyStruct_s *) options);
      return 0;
   }

   context_opt = oyOptions_Find(options, "device_context", oyNAME_PATTERN);
   handle_opt = oyOptions_Find(options, "device_handle", oyNAME_PATTERN);
   name_opt = oyOptions_Find(options, "oyNAME_NAME", oyNAME_PATTERN);

   /*Handle "driver_version" option [IN] */
   check_driver_version(options, &version_opt, &call_sane_exit);

   devices = oyConfigs_New(0);
   if (command_list) {
      /* "list" call section */

      if (device_name &&   /*If a user provides a device_name option,*/
          !context_opt &&  /*and does not need the device_context data,*/
          !name_opt        /*or the oyNAME_NAME description*/
         )
         num_devices = 1;  /*then we can get away without calling GetDevices()*/
      else if (GetDevices(&device_list, &num_devices) != 0) {
         num_devices = 0; /*So that for loop will not run*/
         ++g_error;
      }

      for (i = 0; i < num_devices; ++i) {
         int error = 0;
         const char *sane_name = NULL,
                    *sane_model = NULL;

         if (device_list) {
            sane_name = device_list[i]->name;
            sane_model = device_list[i]->model;
         } else {
            sane_name = device_name;
         }

         /*Handle "device_name" option [IN] */
         if (device_name &&                        /*device_name is provided*/
             sane_name &&                          /*and sane_name has been retrieved*/
             strcmp(device_name, sane_name) != 0)  /*and they don't match,*/
            continue;                              /*then try the next*/

         device = oyConfig_FromRegistration(CMM_BASE_REG, 0);

         /*Handle "driver_version" option [OUT] */
         if (version_opt) {
            oyOption_s * tmp = oyOption_Copy(version_opt, 0);
            oyOptions_MoveIn(*oyConfig_GetOptions(device,"backend_core"), &tmp, -1);
         }

         /*Handle "device_name" option [OUT] */
         oyOptions_SetFromText(oyConfig_GetOptions(device,"backend_core"),
                               CMM_BASE_REG OY_SLASH "device_name",
                               sane_name,
                               OY_CREATE_NEW);

         /*Handle "oyNAME_NAME" option */
         if (name_opt)
            oyOptions_SetFromText(oyConfig_GetOptions(device,"backend_core"),
                                  CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                  sane_model,
                                  OY_CREATE_NEW);

         /*Handle "device_context" option */
         /* SANE Backend protocol states that device_context is *always* returned
          * This is a slight variation: Only when GetDevices() is called will it be returned,
          * unless we call sane_exit*/
         if (device_list && !call_sane_exit) {
            oyBlob_s *context_blob = oyBlob_New(NULL);
            oyOption_s *context_opt = oyOption_FromRegistration(
                                     CMM_BASE_REG OY_SLASH "device_context", 0);

            oyBlob_SetFromData(context_blob, (oyPointer) device_list[i], sizeof(SANE_Device), "sane");
            oyOption_MoveInStruct(context_opt, (oyStruct_s **) & context_blob);
            oyOptions_MoveIn(*oyConfig_GetOptions(device,"data"), &context_opt, -1);
         }

         /*Handle "device_handle" option */
         if (handle_opt && !call_sane_exit) {
            oyPointer_s *handle_ptr = NULL;
            SANE_Handle h;
            status = sane_open(sane_name, &h);
            if (status == SANE_STATUS_GOOD) {
               handle_ptr = oyPointer_New(0);
               oyPointer_Set(handle_ptr,
                            "SANE",
                            "handle",
                            (oyPointer)h,
                            "sane_release_handle",
                            sane_release_handle);
               oyOptions_MoveInStruct(oyConfig_GetOptions(device,"data"),
                                      CMM_BASE_REG OY_SLASH "device_handle",
                                      (oyStruct_s **) &handle_ptr, OY_CREATE_NEW);
            } else
               printf(PRFX "Unable to open sane device \"%s\": %s\n", sane_name, sane_strstatus(status));
         }

         oyConfig_SetRankMap( device, _api8.rank_map );

         error = oyConfigs_MoveIn(devices, &device, -1);

         /*Cleanup*/
         if (error) {
            oyConfig_Release(&device);
            ++g_error;
         }
      }

      *s = devices;
   } else if (command_properties) {
      /* "properties" call section */
      const SANE_Device *device_context = NULL;
      SANE_Device *aux_context = NULL;
      SANE_Handle device_handle = NULL;

      /*Return a full list of scanner H/W &
       * SANE driver S/W color options
       * with the according rank map */

      device = oyConfig_FromRegistration(CMM_BASE_REG, 0);

      /*Handle "driver_version" option [OUT] */
      if (version_opt) {
         oyOption_s *tmp = oyOption_Copy(version_opt, 0);
         oyOptions_MoveIn(*oyConfig_GetOptions(device,"backend_core"), &tmp, -1);
      }

      /*1a. Get the "device_context"*/
      if (!context_opt) { /*we'll have to get it ourselves*/
         if (GetDevices(&device_list, &num_devices) == 0) {
            device_context = *device_list;
            while (device_context) {
               if(device_name && device_context->name &&
                  strcmp(device_name,device_context->name) == 0)
                  break;
               device_context++;
            }
            if (!device_context) {
               printf(PRFX "device_name does not match any installed device.\n");
               g_error++;
            }
         } else {
            g_error++;
         }
      } else {
         aux_context = (SANE_Device*)oyOption_GetData(context_opt, NULL, allocateFunc);
         device_context = aux_context;
      }

      /*1b. Use the "device_context"*/
      if (device_context)
         DeviceInfoFromContext_(device_context, oyConfig_GetOptions(device,"backend_core"));

      /*2a. Get the "device_handle"*/
      if (!handle_opt) {
         status = sane_open( device_name, &device_handle );
         if (status != SANE_STATUS_GOOD) {
            printf(PRFX "Unable to open sane device \"%s\": %s\n", device_name, sane_strstatus(status));
            g_error++;
         }
      } else {
        oyPointer_s * oy_struct = (oyPointer_s*) oyOption_GetStruct( handle_opt,
                                                           oyOBJECT_POINTER_S );
        device_handle = (SANE_Handle)oyPointer_GetPointer(oy_struct);
        oyPointer_Release( &oy_struct );
      }

      if (device_handle) {
         /*2b. Use the "device_handle"*/
         ColorInfoFromHandle(device_handle, oyConfig_GetOptions(device,"backend_core"));

         /*3. Set the rank map*/
         oyConfig_SetRankMap( device, _api8.rank_map );
      }
      oyConfigs_MoveIn(devices, &device, -1);

      /*Cleanup*/
      free(aux_context);

      *s = devices;
   } else {
      /*unsupported, wrong or no command */
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "No supported commands in options:\n%s", _DBG_ARGS_,
              oyOptions_GetText(options, oyNAME_NICK) );
      ConfigsFromPatternUsage((oyStruct_s *) options);
      g_error = 1;
   }

   /*Global Cleanup*/
   if (call_sane_exit) {
      printf(PRFX "sane_exit()\n");
      sane_exit();
   }

   oyOption_Release(&context_opt);
   oyOption_Release(&handle_opt);
   oyOption_Release(&version_opt);
   oyOption_Release(&name_opt );

   printf(PRFX "Leaving %s\n", __func__);
   return g_error;
}

/** Function Configs_Modify
 *  @brief   oyCMMapi8_s SANE scanner manipulation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 *
 *  \todo { Test }
 */
int Configs_Modify(oyConfigs_s * devices, oyOptions_s * options)
{
   oyOption_s *version_opt = NULL;
   oyOption_s *version_opt_dev = NULL;
   oyConfig_s *device = NULL;
   int num_devices, g_error = 0;
   int call_sane_exit = 0;
   const char *command_list = NULL,
              *command_properties = NULL;

   oyAlloc_f allocateFunc = malloc;

   printf(PRFX "Entering %s(). Options:\n%s", __func__, oyOptions_GetText(options, oyNAME_NICK));

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
      int i;

      for (i = 0; i < num_devices; ++i) {
         const SANE_Device *device_context = NULL;
         SANE_Status status = SANE_STATUS_INVAL;
         oyOption_s *name_opt_dev = NULL,
                    *handle_opt_dev = NULL,
                    *context_opt_dev = NULL;
         const char *sane_name = NULL,
                    *sane_model = NULL;
         int error = 0;

         device = oyConfigs_Get(devices, i);

         if(oyOptions_Count(*oyConfig_GetOptions(device,"backend_core")))
           printf(PRFX "Backend core:\n%s", oyOptions_GetText(*oyConfig_GetOptions(device,"backend_core"), oyNAME_NICK));
         if(oyOptions_Count(*oyConfig_GetOptions(device,"data")))
           printf(PRFX "Data:\n%s", oyOptions_GetText(*oyConfig_GetOptions(device,"data"), oyNAME_NICK));

         /*Ignore device without a device_name*/
         if (!oyOptions_FindString(*oyConfig_GetOptions(device,"backend_core"), "device_name", NULL)) {
            message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ ": %s\n",
                    _DBG_ARGS_, "The \"device_name\" is missing from config object!");
            oyConfig_Release(&device);
            g_error++;
            continue;
         }

         /*Handle "driver_version" option [OUT] */
         version_opt_dev = oyConfig_Find(device, "driver_version");
         if (!version_opt_dev && version_opt)
            oyOptions_MoveIn(*oyConfig_GetOptions(device,"backend_core"), &version_opt, -1);
         oyOption_Release(&version_opt_dev);

         /*Handle "device_context" option */
         /*This is always provided by Configs_FromPattern()
          * [or should be alternatively by the user].
          * Configs_Modify() will not scan for SANE devices
          * because it takes too long*/
         context_opt_dev = oyConfig_Find(device, "device_context");
         if (!context_opt_dev) {
            message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ ": %s\n",
                    _DBG_ARGS_, "The \"device_context\" option is missing!");
            error = g_error = 1;
         }
         if (!error) {
            device_context = (SANE_Device*)oyOption_GetData(context_opt_dev, NULL, allocateFunc);
            sane_name  = device_context->name;
            sane_model = device_context->model;
         }

         /*Handle "oyNAME_NAME" option */
         name_opt_dev = oyConfig_Find(device, "oyNAME_NAME");
         if (!error && !name_opt_dev && oyOptions_Find(options, "oyNAME_NAME", oyNAME_PATTERN))
            oyOptions_SetFromText(oyConfig_GetOptions(device,"backend_core"),
                                  CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                  sane_model,
                                  OY_CREATE_NEW);

         /*Handle "device_handle" option */
         handle_opt_dev = oyConfig_Find(device, "device_handle");
         if (!error && !handle_opt_dev) {
            oyPointer_s *handle_ptr = NULL;
            SANE_Handle h;
            status = sane_open(sane_name, &h);
            if (status == SANE_STATUS_GOOD) {
               handle_ptr = oyPointer_New(0);
               oyPointer_Set(handle_ptr,
                            "SANE",
                            "handle",
                            (oyPointer)h,
                            "sane_release_handle",
                            sane_release_handle);
               oyOptions_MoveInStruct(oyConfig_GetOptions(device,"data"),
                                      CMM_BASE_REG OY_SLASH "device_handle",
                                      (oyStruct_s **) &handle_ptr, OY_CREATE_NEW);
            } else
               printf(PRFX "Unable to open sane device \"%s\": %s\n", sane_name, sane_strstatus(status));
         }

         /*Create static rank_map, if not already there*/
         if (!oyConfig_GetRankMap( device))
           oyConfig_SetRankMap( device, _api8.rank_map );

         /*Cleanup*/
         oyConfig_Release(&device);
         oyOption_Release(&context_opt_dev);
         oyOption_Release(&name_opt_dev);
         oyOption_Release(&handle_opt_dev);
      }
   } else if (command_properties) {
      /* "properties" call section */
      int i;

      /*Return a full list of scanner H/W &
       * SANE driver S/W color options
       * with the according rank map */

      for (i = 0; i < num_devices; ++i) {
         SANE_Device *device_context = NULL;
         SANE_Status status = SANE_STATUS_INVAL;
         SANE_Handle device_handle;
         oyOption_s *name_opt_dev = NULL,
                    *handle_opt_dev = NULL,
                    *context_opt_dev = NULL;
         oyConfig_s *device_new = NULL;
         char *device_name = NULL;

         /* All previous device properties are considered obsolete
          * and a new device is created. Basic options are moved from
          * the old to new device */
         device = oyConfigs_Get(devices, i);
         device_new = oyConfig_FromRegistration(CMM_BASE_REG, 0);

         printf(PRFX "Backend core:\n%s", oyOptions_GetText(*oyConfig_GetOptions(device,"backend_core"), oyNAME_NICK));
         printf(PRFX "Data:\n%s", oyOptions_GetText(*oyConfig_GetOptions(device,"data"), oyNAME_NICK));

         /*Ignore device without a device_name*/
         if (!oyOptions_FindString(*oyConfig_GetOptions(device,"backend_core"), "device_name", NULL)) {
            message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ ": %s\n",
                    _DBG_ARGS_, "The \"device_name\" is NULL, or missing from config object!");
            oyConfig_Release(&device);
            oyConfig_Release(&device_new);
            g_error++;
            continue;
         }

         /*Handle "driver_version" option [OUT] */
         if (version_opt) {
            oyOption_s *tmp = oyOption_Copy(version_opt, 0);
            oyOptions_MoveIn(*oyConfig_GetOptions(device_new,"backend_core"), &tmp, -1);
         }

         /* 1. Get the "device_name" from old device */
         name_opt_dev = oyConfig_Find(device, "device_name");
         device_name = oyOption_GetValueText(name_opt_dev, allocateFunc);
         oyOptions_MoveIn(*oyConfig_GetOptions(device_new,"backend_core"), &name_opt_dev, -1);

         /* 2. Get the "device_context" from old device */
         /* It should be there, see "list" call above */
         context_opt_dev = oyConfig_Find(device, "device_context");
         if (context_opt_dev) {
            device_context = (SANE_Device*)oyOption_GetData(context_opt_dev, NULL, allocateFunc);
            if (device_context) {
               oyOptions_MoveIn(*oyConfig_GetOptions(device_new,"data"), &context_opt_dev, -1);
            } else {
               message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ ": %s\n",
                       _DBG_ARGS_, "The \"device_context\" is NULL!");
               oyOption_Release(&context_opt_dev);
               g_error++;
            }
         } else {
            message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ ": %s\n",
                    _DBG_ARGS_, "The \"device_context\" option is missing!");
            g_error++;
         }

         /* 3. Get the scanner H/W properties from old device */
         /* FIXME: we only recompute them, just in case they are not in old device */
         if (device_context) {
            DeviceInfoFromContext_(device_context, oyConfig_GetOptions(device_new,"backend_core"));
         }

         /* 4. Get the "device_handle" from old device */
         /* If not there, get one from SANE */
         handle_opt_dev = oyConfig_Find(device, "device_handle");
         if (handle_opt_dev) {
           oyPointer_s * oy_struct = (oyPointer_s*)oyOption_GetStruct(
                                           handle_opt_dev, oyOBJECT_POINTER_S );
           device_handle = (SANE_Handle)oyPointer_GetPointer(oy_struct);
           oyPointer_Release( &oy_struct );
            oyOptions_MoveIn(*oyConfig_GetOptions(device_new,"data"), &handle_opt_dev, -1);
         } else {
            printf(PRFX "Opening sane device \"%s\"..", device_name); fflush(NULL);
            status = sane_open( device_name, &device_handle );
            if (status != SANE_STATUS_GOOD)
               printf("[FAIL: %s]\n", sane_strstatus(status));
            else
               printf("[OK]\n");
         }

         if (handle_opt_dev || status == SANE_STATUS_GOOD) {
            /* Use the device_handle to get the device color options */
            ColorInfoFromHandle(device_handle, oyConfig_GetOptions(device_new,"backend_core"));

            /*5. Set the rank map*/
            oyConfig_SetRankMap( device_new, _api8.rank_map );
         }

         /*Cleanup*/
         /* Remove old, add new device */
         oyConfig_Release(&device);
         oyConfigs_ReleaseAt(devices, i);
         oyConfigs_MoveIn(devices, &device_new, -1);

         /*If we had to open a SANE device, we'll have to close it*/
         if (status == SANE_STATUS_GOOD) {
            printf(PRFX "sane_close(%s)\n", device_name);
            sane_close(device_handle);
         }

         free(device_context);
         free(device_name);
      }
   } else {
      /*unsupported, wrong or no command */
      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "No supported commands in options:\n%s", _DBG_ARGS_,
              oyOptions_GetText(options, oyNAME_NICK) );
      ConfigsFromPatternUsage((oyStruct_s *) options);
      g_error = 1;
   }

   /*Cleanup*/
   if (call_sane_exit) {
      printf(PRFX "sane_exit()\n");
      sane_exit();
   }

   oyOption_Release(&version_opt);

   printf(PRFX "Leaving %s\n", __func__);
   return g_error;
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
    return GetText(select,type,context);
  }
  else if(strcmp(select, "device_class") == 0)
    {
        if(type == oyNAME_NICK)
            return "scanner";
        else if(type == oyNAME_NAME)
            return _("Scanner");
        else
            return _("Scanner data, which come from SANE library.");
    }
  else if(strcmp(select, "icc_profile_class")==0)
    {
      return "input";
    } 
  else if(strcmp(select,"category") == 0)
  {
    if(!category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[4] = {0,0,0,0};
      int len;
      i18n[0] = _("Color");
      i18n[1] = _("Device");
      i18n[2] = _("Scanner");
      len =  strlen(i18n[0]) + strlen(i18n[1]) + strlen(i18n[2]);
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
const char * _api8_ui_texts[] = {"name", "help", "device_class", "icc_profile_class", "category", 0};

/** @instance _api8_ui
 *  @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI parts for oyRE devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMui_s_   _api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Color/Device/CameraRaw", /* category */
  0,   /* const char * options */

  0,    /* oyCMMuiGet_f oyCMMuiGet */

  Api8UiGetText,  /* oyCMMGetText_f getText */
  _api8_ui_texts  /* (const char**)texts */
};

oyIcon_s _api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"
};

/** @instance _api8
 *  @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMapi8_s_ _api8 = {
   oyOBJECT_CMM_API8_S,
   0, 0, 0,
   0,                                                                 /**< next */
   CMMInit,                                                           /**< oyCMMInit_f      oyCMMInit */
   CMMMessageFuncSet,                                                 /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
   CMM_BASE_REG,                                                      /**< registration */
   CMM_VERSION,                                                         /**< int32_t version[3] */
   CMM_API_VERSION,                                                   /**< int32_t module_api[3] */
   0,                                                                 /**< char * id_ */
   0,                                                                 /**< oyCMMapi5_s * api5_ */
   0,                                                                 /**< runtime_context */
   Configs_FromPattern,                                               /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
   Configs_Modify,                                                    /**< oyConfigs_Modify_f oyConfigs_Modify */
   Config_Rank,                                                       /**< oyConfig_Rank_f oyConfig_Rank */
   (oyCMMui_s*)&_api8_ui,                                             /**< device class UI name and help */
   &_api8_icon,                                                       /**< device icon */
   NULL                                                               /**< oyRankMap ** rank_map */
};

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  \todo { Add usage info }
 */
const char *GetText(const char *select, oyNAME_e type,
                                       oyStruct_s        * context)
{
   if (strcmp(select, "name") == 0) {
      if (type == oyNAME_NICK)
         return CMM_NICK;
      else if (type == oyNAME_NAME)
         return _("Oyranos Scanner");
      else
         return _("The scanner (hopefully)usefull backend of Oyranos.");
   } else if (strcmp(select, "manufacturer") == 0) {
      if (type == oyNAME_NICK)
         return "orionas";
      else if (type == oyNAME_NAME)
         return "Yiannis Belias";
      else
         return
             _
             ("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
   } else if (strcmp(select, "copyright") == 0) {
      if (type == oyNAME_NICK)
         return "MIT";
      else if (type == oyNAME_NAME)
         return _("Copyright (c) 2009 Kai-Uwe Behrmann; MIT");
      else
         return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
   } else if (strcmp(select, "help") == 0) {
      if (type == oyNAME_NICK)
         return "help";
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
oyCMM_s _cmm_module = {

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
   &_api8_icon
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
   unsigned int opt_num = 0;
   char cmm_base_reg[] = CMM_BASE_REG OY_SLASH;
   char *value_str = NULL;
   const size_t value_size = 100; /*Better not allow more than 100 characters in the option value string*/

   /* We got a device, find out how many options it has */
   status = sane_control_option(device_handle, 0, SANE_ACTION_GET_VALUE, &num_options, 0);
   if (status != SANE_STATUS_GOOD) {
      message(oyMSG_WARN, 0,
              "%s()\n Unable to determine option count: %s\n",
              __func__, sane_strstatus(status));
      return -1;
   }

   oyOptions_SetFromText(options, CMM_BASE_REG OY_SLASH "prefix", "SANE_", OY_CREATE_NEW);

   value_str = malloc(sizeof(char)*value_size);

   for (opt_num = 1; opt_num < num_options; opt_num++) {
      opt = sane_get_option_descriptor(device_handle, opt_num);
      /*if ((opt->cap & SANE_CAP_COLOUR))*/ /*&& !(opt->cap & SANE_CAP_INACTIVE)*/
      if(opt->name)
      {
         void *value = malloc(opt->size);
         char *registration = malloc(sizeof(cmm_base_reg) + 6 + strlen(opt->name)+1);

         sprintf(registration, "%sSANE_%s", cmm_base_reg, opt->name);

         sane_control_option(device_handle, opt_num, SANE_ACTION_GET_VALUE, value, 0);
         switch (opt->type) {
            case SANE_TYPE_BOOL:
               value_str[0] = *(SANE_Bool *) value ? '1' : '0';
               value_str[1] = '\0';
               oyOptions_SetFromText(options, registration, value_str, OY_CREATE_NEW);
               break;
            case SANE_TYPE_INT:
               if (opt->size == (SANE_Int)sizeof(SANE_Word)) {
                  snprintf(value_str, value_size, "%d", *(SANE_Int *) value);
                  oyOptions_SetFromText(options, registration, value_str, OY_CREATE_NEW);
               } else {
                  int count = opt->size/sizeof(SANE_Word);
                if (strstr(opt->name, "gamma-table")) {
                  /* If the option contains a gamma table, calculate the gamma value
                   * as a float and save that instead */
#ifdef HAVE_LCMS_never
                   LPGAMMATABLE lt = cmsAllocGamma(count);
                   float norm = 65535.0/(count-1);

                   /*Normalise table to 65535. lcms expects that*/
                   for (i=0; i<count; ++i)
                      lt->GammaTable[i] = (WORD)((float)(*(SANE_Int *) value+i)*norm);

                   snprintf(value_str, value_size, "%f", cmsEstimateGamma(lt));
                   oyOptions_SetFromText(options, registration, value_str, OY_CREATE_NEW);
                   cmsFreeGamma(lt);
#endif
                } else {
                   int chars = 0;
                   for (i=0; i<count; ++i) {
                     int printed = snprintf(value_str+chars, value_size-chars, "%d, ", *(SANE_Int *) value+i);
                     if (printed >= value_size-chars)
                        break;
                     else
                        chars += printed;
                   }
                   oyOptions_SetFromText(options, registration, value_str, OY_CREATE_NEW);
                }
               }
               break;
            case SANE_TYPE_FIXED:
              {
               char * save_locale = oyStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
               setlocale(LC_NUMERIC, "C");
               if (opt->size == (SANE_Int)sizeof(SANE_Word)) {
                  snprintf(value_str, value_size, "%f", SANE_UNFIX(*(SANE_Fixed *) value));
                  oyOptions_SetFromText(options, registration, value_str, OY_CREATE_NEW);
               } else {
                  int count = opt->size/sizeof(SANE_Word);

                  int chars = 0;
                  for (i=0; i<count; ++i) {
                    int printed = snprintf(value_str+chars,
                                           value_size-chars,
                                           "%f, ",
                                           SANE_UNFIX(*(SANE_Fixed *) value+i));
                    if (printed >= value_size-chars)
                       break;
                    else
                       chars += printed;
                  }
                  oyOptions_SetFromText(options, registration, value_str, OY_CREATE_NEW);
               }
               setlocale(LC_NUMERIC, save_locale);
               free( save_locale );
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
      free(registration );
      }
   }
   free(value_str);

   return error;
}

/** @internal
 * @brief Release the SANE_Handle.
 *
 * This function is a oyPointer_release_f and is used in the
 * oyPointer_s device handle.
 *
 * @param[in]	handle_ptr				SANE_Handle
 * @return 0 for success
 *
 */
int sane_release_handle(oyPointer *handle_ptr)
{
   SANE_Handle h = (SANE_Handle)*handle_ptr;
   sane_close(h);

   printf("SANE handle deleted.\n");
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
   int driver_version = 0, status;
   oyOption_s *context_opt = oyOptions_Find(options, "device_context", oyNAME_PATTERN);
   oyOption_s *handle_opt = oyOptions_Find(options, "device_handle", oyNAME_PATTERN);
   int error = oyOptions_FindInt(options, "driver_version", 0, &driver_version);

   if (!error && driver_version > 0) /*driver_version is provided*/
      *version_opt_p = oyOptions_Find(options, "driver_version", oyNAME_PATTERN);
   else { /*we have to call sane_init()*/
      status = sane_init(&driver_version, NULL);
      if (status == SANE_STATUS_GOOD) {
         printf(PRFX "SANE v.(%d.%d.%d) init...OK\n",
                SANE_VERSION_MAJOR(driver_version),
                SANE_VERSION_MINOR(driver_version),
                SANE_VERSION_BUILD(driver_version));

         if (error &&                     /*we've not been given a driver_version*/
             !context_opt && !handle_opt  /*we've not been given other options*/
            ) {                           /*when we are over*/
            *call_sane_exit = 1;
         } else {
            *version_opt_p = oyOption_FromRegistration(
                                     CMM_BASE_REG OY_SLASH "driver_version", 0);
            oyOption_SetFromInt(*version_opt_p, driver_version, 0, 0);
         }
      } else {
        message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
                "Unable to init SANE. Giving up.[%s] Options:\n%s", _DBG_ARGS_,
                sane_strstatus(status), oyOptions_GetText(options, oyNAME_NICK));
        return 1;
      }
   }
   return 0;
}
