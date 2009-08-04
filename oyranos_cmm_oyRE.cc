/** @file oyranos_cmm_oyRE.cc
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos dummy device backend for Oyranos
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

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>

#include <libraw/libraw.h>

#include "oyRE_help.c"
/* --- internal definitions --- */

#define DBG printf("%s: %d\n", __FILE__, __LINE__ ); fflush(NULL);
/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "oyRE"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.raw-image." CMM_NICK

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( oyRE , CMMInit )
#define CMMallocateFunc         catCMMfunc( oyRE , CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( oyRE , CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( oyRE , CMMMessageFuncSet )
#define CMMCanHandle            catCMMfunc( oyRE , CMMCanHandle )
#define ConfigsFromPatternUsage catCMMfunc( oyRE , ConfigsFromPatternUsage )
#define DeviceFromName_     catCMMfunc( oyRE , DeviceFromName_ )
#define GetDevices              catCMMfunc( oyRE , GetDevices )
#define _api8                   catCMMfunc( oyRE , _api8 )
#define _rank_map               catCMMfunc( oyRE , _rank_map )
#define Configs_FromPattern     catCMMfunc( oyRE , Configs_FromPattern )
#define Config_Check            catCMMfunc( oyRE , Config_Check )
#define GetText                 catCMMfunc( oyRE , GetText )
#define _texts                  catCMMfunc( oyRE , _texts )
#define _cmm_module             catCMMfunc( oyRE , _cmm_module )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x
#define STRING_ADD(a,b) sprintf( &a[strlen(a)], b )

/** @instance _rank_map
 *  @brief    oyRankPad map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  \todo { In progress }
 */
oyranos::oyRankPad _rank_map[] = {
   {const_cast < char *>("device_name"), 0, 0, 0},                   /**< Unused?*/
   {const_cast < char *>("driver_version"), 2, -1, 0},               /**< is good */
   {const_cast < char *>("profile_name"), 0, 0, 0},                  /**< non relevant for device properties*/
       /* EXIF Fields */                                              
   {const_cast < char *>("Exif.Image.Make"), 1, -1, 0},              /**< is nice */
   {const_cast < char *>("Exif.Image.Model"), 5, -5, 0},             /**< important, should not fail */
   {const_cast < char *>("Exif.Photo.ISOSpeedRatings"), 1, 0, 0},    /**< is nice */
   {const_cast < char *>("Exif.Photo.ExposureProgram"), 1, 0, 0},    /**< nice to match */
   {const_cast < char *>("Exif.Photo.Flash"), 1, 0, 0},              /**< nice to match */
       /*Makernote Fields - no 1-1 mapping with exif tags */          
   {const_cast < char *>("Exif.SerialNumber"), 10, -2, 0},           /**< important, could slightly fail *//*E.g. Exif.Canon.SerialNumber */
   {const_cast < char *>("Exif.Lens"), 2, -1, 0},                    /**< is good *//*E.g. Exif.CanonCs.Lens */
       /* Possibly not relevant options are marked with: O->Output R->Repair */
       /* LibRaw Options affecting open_file() */                     
       /* LibRaw Options affecting unpack() */                        
   {const_cast < char *>("use_camera_wb"), 1, -1, 0},                /**< is nice */
   {const_cast < char *>("use_camera_matrix"), 1, -1, 0},            /**< is nice */
   {const_cast < char *>("half_size"), 1, -1, 0},                    /**< is nice */
//{const_cast<char*>("filtering_mode"), 1, -1, 0},                   /**< is nice */ /*not in libraw-lite*/
//This is a bit-field. Out of all the possible flags, only LIBRAW_FILTERING_NORAWCURVE
//seems to be relevant to color [From LibRaw API docs]:               
//This bit turns off tone curve processing (for tone curves read from file metadata or
//calculated from constants). This setting is supported only for bayer-pattern cameras
//with tone curve;                                                    
   {const_cast < char *>("threshold"), 1, -1, 0},                    /**< is nice */ /*R*/
   {const_cast < char *>("aber"), 1, -1, 0},                         /**< is nice */ /*R*/
       /* LibRaw Options affecting dcraw_process() */                 
   {const_cast < char *>("greybox"), 1, -1, 0},                      /**< is nice */
   {const_cast < char *>("gamm"), 1, -1, 0},                         /**< is nice */
   {const_cast < char *>("user_mul"), 1, -1, 0},                     /**< is nice */
   {const_cast < char *>("bright"), 1, -1, 0},                       /**< is nice */
   {const_cast < char *>("four_color_rgb"), 1, -1, 0},               /**< is nice */
   {const_cast < char *>("highlight"), 1, -1, 0},                    /**< is nice */
   {const_cast < char *>("use_auto_wb"), 1, -1, 0},                  /**< is nice */
   {const_cast < char *>("output_color"), 1, -1, 0},                 /**< is nice */
   {const_cast < char *>("camera_profile"), 1, -1, 0},               /**< is nice */
   {const_cast < char *>("output_bps"), 1, -1, 0},                   /**< is nice */
   {const_cast < char *>("user_qual"), 1, -1, 0},                    /**< is nice */
   {const_cast < char *>("user_black"), 1, -1, 0},                   /**< is nice */
   {const_cast < char *>("user_sat"), 1, -1, 0},                     /**< is nice */
   {const_cast < char *>("med_passes"), 1, -1, 0},                   /**< is nice */
   {const_cast < char *>("auto_bright_thr"), 1, -1, 0},              /**< is nice */
   {const_cast < char *>("no_auto_bright"), 1, -1, 0},               /**< is nice */
       /* Extra options (user supplied) */                            
   {const_cast < char *>("illumination_source"), 1, -1, 0},          /**< is nice */
   {0, 0, 0, 0}                                                      /**< end of list */
};

#ifdef __cplusplus
extern "C" {
   namespace oyranos {
#endif                          /* __cplusplus */

      oyMessage_f message = 0;

      extern oyranos::oyCMMapi8_s _api8;

#ifdef __cplusplus
} /* extern "C" */ }            /* namespace oyranos */
#endif                          /* __cplusplus */
using namespace oyranos;

bool is_raw( int id );
int DeviceFromContext(oyConfig_s **config, libraw_output_params_t *params);

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

void ConfigsFromPatternUsage(oyStruct_s * options)
{
    /** oyMSG_WARN should make shure our message is visible. */
   message(oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
           _DBG_ARGS_, "The following help text informs about the communication protocol.");
   message(oyMSG_WARN, options, "%s()\n%s", __func__, help_message);

   return;
}

/** @internal
 *  @brief Get the options from EXIF Tags, and put them in a configuration object
 *
 *  @param[in]image              The Exiv2 raw image
 *  @param[out]options           The configuration object to hold the option-value pairs
 *
 * \todo { Untested }
 */
#define OPTIONS_ADD_EXIF(name) if (!error) { \
   Exiv2::ExifKey key( #name ); \
   Exiv2::ExifData::iterator pos = exif_data.findKey(key); \
   if (pos != exif_data.end()) { \
      error = oyOptions_SetFromText( options, \
                                     CMM_BASE_REG OY_SLASH #name, \
                                     pos->toString().c_str(), OY_CREATE_NEW ); \
   } \
}
int DeviceFromHandle(oyOptions_s **options, Exiv2::Image::AutoPtr image)
{
   int error = 0;

   image->readMetadata();
   Exiv2::ExifData &exif_data = image->exifData();
   if (exif_data.empty())
      return 1;

   OPTIONS_ADD_EXIF(Exif.Image.Make)
   OPTIONS_ADD_EXIF(Exif.Image.Model)
   OPTIONS_ADD_EXIF(Exif.Photo.ISOSpeedRatings)
   OPTIONS_ADD_EXIF(Exif.Photo.ExposureProgram)
   OPTIONS_ADD_EXIF(Exif.Photo.Flash)
 
   //"Exif.SerialNumber" //TODO
   //"Exif.Lens" //TODO

   return error;
}

/** Function Configs_FromPattern
 *  @brief   CMM_NICK oyCMMapi8_s raw images
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  todo { Error Handling }
 */
int Configs_FromPattern(const char *registration, oyOptions_s * options, oyConfigs_s ** s)
{
   oyOption_s *context_opt = NULL, *handle_opt = NULL;

   int num_devices, error = 0;
   const char *device_name = 0, *command_list = 0, *command_properties = 0;

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
      ConfigsFromPatternUsage((oyStruct_s *) options);
      return 0;
   }

   /*Prepare for "driver_version" option*/
   int driver_version_number = LibRaw::versionNumber();
   const char *driver_version_string = LibRaw::version();
   oyOption_s *version_opt_int= NULL, *version_opt_str = NULL, *version_opt = NULL;
   error = oyOption_SetFromText(version_opt_str, driver_version_string, 0);
   error = oyOption_SetFromInt(version_opt_int, driver_version_number, 0, 0);

   command_list = oyOptions_FindString(options, "command", "list");
   command_properties = oyOptions_FindString(options, "command", "properties");

   context_opt = oyOptions_Find(options, "device_context");
   handle_opt = oyOptions_Find(options, "device_handle");
   version_opt = oyOptions_Find(options, "driver_version");

   oyConfig_s *device = NULL;
   device = oyConfig_New(CMM_BASE_REG, 0);

   /*Handle "device_handle" option [IN]*/
   Exiv2::Image::AutoPtr device_handle;
   if (handle_opt) {
      const char *filename = NULL;
      oyBlob_s *raw_blob = NULL;
      const Exiv2::byte *raw_data = NULL;
      long size;
      switch (handle_opt->value_type) {
         case oyVAL_STRING:
            filename = handle_opt->value->string;
            if (is_raw(Exiv2::ImageFactory::getType(filename))) //TODO
               device_handle = Exiv2::ImageFactory::open(filename);
            break;
         case oyVAL_STRUCT:
            raw_blob = (oyBlob_s*)handle_opt->value->oy_struct;
            raw_data = (Exiv2::byte*)raw_blob->ptr;
            size = raw_blob->size;
            if (is_raw(Exiv2::ImageFactory::getType(raw_data, size)))
               device_handle = Exiv2::ImageFactory::open(raw_data, size);
            break;
         default:
            printf("Option \"device_handle\" is of a wrong type\n");
            break;
      }
       
      if (device_handle->good())
         DeviceFromHandle(&device->backend_core, device_handle); //TODO
      else {
         printf("Unable to open raw image.\n");
         return 1;
      }
   }

   if (command_list) {
      /* "list" call section */

      const char **device_list = LibRaw::cameraList();
      int num_devices = LibRaw::cameraCount();

      /*Handle "driver_version" option [IN] */
      if (version_opt) {
         error = oyOptions_SetFromText(&device->data,
                                       CMM_BASE_REG OY_SLASH "driver_version_string",
                                       driver_version_string,
                                       OY_CREATE_NEW);
         error = oyOptions_SetFromInt(&device->data,
                                      CMM_BASE_REG OY_SLASH "driver_version_number",
                                      driver_version_number,
                                      0,
                                      OY_CREATE_NEW);
      }
      

      /*Handle "device_handle" option [OUT:informative]*/
      if (!handle_opt)
         error = oyOptions_SetFromText(&device->data,
                                       CMM_BASE_REG OY_SLASH "device_handle",
                                       "filename\nblob",
                                       OY_CREATE_NEW);

      /*Handle "supported_devices_info" option [OUT:informative]*/
      if (!handle_opt) {
         oyOption_s *device_list_opt = oyOption_New(CMM_BASE_REG OY_SLASH "supported_devices_info", 0);
         device_list_opt->value->string_list = const_cast<char**>(device_list);
         device_list_opt->value_type = oyVAL_STRING_LIST;
         oyOptions_MoveIn(device->data, &device_list_opt, -1);
      }

      /*Copy the rank map*/
      device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);

   } else if (command_properties) {
      /* "properties" call section */

      /*Bail out if no "device_handle" given*/
      if (!handle_opt) {
         printf("Missing \"device_handle\" option\n");
         return 1;
      }

      /*Handle "device_context" option [IN]*/
      if (context_opt) {
         libraw_output_params_t *device_context =
            (libraw_output_params_t*)oyOption_GetData(context_opt, NULL, allocateFunc);
         DeviceFromContext(&device, device_context);
      }

      /*Copy the rank map*/
      device->rank_map = oyRankMapCopy(_rank_map, device->oy_->allocateFunc_);
   } else {
      /* not to be reached section, e.g. warning */
      oyOption_Release(&version_opt_int);
      oyOption_Release(&version_opt_str);
      oyConfig_Release(&device);

      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
           "This point should not be reached. Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
       );

      ConfigsFromPatternUsage((oyStruct_s *) options);
   } //End of Configuration

   /*Return the Configuration object*/
   oyConfigs_s *devices = NULL;
   oyConfigs_MoveIn(devices, &device, -1);
   *s = devices;
 
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

/** @instance _api8
 *  @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyCMMapi8_s oyranos::_api8 = {
   oyOBJECT_CMM_API8_S,
   0, 0, 0,
   0,                        /**< next */

   CMMInit,                  /**< oyCMMInit_f      oyCMMInit */
   CMMMessageFuncSet,        /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
   CMMCanHandle,             /**< oyCMMCanHandle_f oyCMMCanHandle */

   const_cast < char *>(CMM_BASE_REG),          /**< registration */
   {0, 1, 0},                /**< int32_t version[3] */
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
         return _("Oyranos RAW Image");
      else
         return _("The raw image (hopefully)usefull backend of Oyranos.");
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
   {oyOBJECT_ICON_S, 0, 0, 0, 0, 0, 0, const_cast < char *>("oyranos_logo.png")},
};

/* Helper functions */

/** @internal
 * @brief Get the options from LibRaw, and put them in a configuration object
 *
 * @param[in]	params				The LibRaw parameter structure
 * @param[out]	config				The configuration object to hold the option-value pairs
 *
 * \todo { Untested }
 */
#define DFC_OPT_ADD_INT_ARR(name, i) if(!error) \
        error = oyOptions_SetFromInt( &((*config)->backend_core), \
                                      CMM_BASE_REG OY_SLASH #name, \
                                      params->name[i], i, OY_CREATE_NEW );
#define DFC_OPT_ADD_INT(name) if(!error) \
        error = oyOptions_SetFromInt( &((*config)->backend_core), \
                                      CMM_BASE_REG OY_SLASH #name, \
                                      params->name, 0, OY_CREATE_NEW );
#define DFC_OPT_ADD_FLOAT_ARR(name, i) if(!error) { \
        oyOption_s *opt = oyOption_New(CMM_BASE_REG OY_SLASH #name, 0); \
        oyOption_SetFromDouble(opt, params->name[i], i, 0); \
        oyOptions_MoveIn((*config)->backend_core, &opt, -1); \
}
#define DFC_OPT_ADD_FLOAT(name) if(!error) { \
        oyOption_s *opt = oyOption_New(CMM_BASE_REG OY_SLASH #name, 0); \
        oyOption_SetFromDouble(opt, params->name, 0, 0); \
        oyOptions_MoveIn((*config)->backend_core, &opt, -1); \
}
int DeviceFromContext(oyConfig_s **config, libraw_output_params_t *params)
{
   int error;

   DFC_OPT_ADD_FLOAT_ARR(aber,0) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,1) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,2) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,3) //4
   DFC_OPT_ADD_FLOAT_ARR(gamm,0) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,1) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,2) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,3) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,4) //5
   DFC_OPT_ADD_FLOAT_ARR(user_mul,0) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,1) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,2) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,3) //4
   DFC_OPT_ADD_FLOAT(auto_bright_thr)
   DFC_OPT_ADD_FLOAT(bright)
   DFC_OPT_ADD_FLOAT(threshold)

   DFC_OPT_ADD_INT(four_color_rgb)
   DFC_OPT_ADD_INT(gamma_16bit) //TODO is it needed?
   DFC_OPT_ADD_INT(half_size)
   DFC_OPT_ADD_INT(highlight)
   DFC_OPT_ADD_INT(med_passes)
   DFC_OPT_ADD_INT(no_auto_bright)
   DFC_OPT_ADD_INT(output_bps)
   DFC_OPT_ADD_INT(output_color)
   DFC_OPT_ADD_INT(use_auto_wb)
   DFC_OPT_ADD_INT(use_camera_matrix)
   DFC_OPT_ADD_INT(use_camera_wb)
   DFC_OPT_ADD_INT(user_black)
   DFC_OPT_ADD_INT(user_qual)
   DFC_OPT_ADD_INT(user_sat)
   DFC_OPT_ADD_INT_ARR(greybox,0) //4
   DFC_OPT_ADD_INT_ARR(greybox,1) //4
   DFC_OPT_ADD_INT_ARR(greybox,2) //4
   DFC_OPT_ADD_INT_ARR(greybox,3) //4
   DFC_OPT_ADD_INT(shot_select)

   return error;
}

bool is_raw( int id )
{
   //using namespace Exiv2::ImageType;
   switch (id) {
      case 7: //cr2:
      case 3: //crw:
      case 5: //mrw:
      case 9: //orf:
      case 8: //raf:
      case 16: //rw2:
         return true;
         break;
      default:
         return false;
   }
}
