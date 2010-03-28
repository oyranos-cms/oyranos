/** @file oyranos_cmm_oyRE.cc
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Yiannis Belias
 *
 *  @brief    Oyranos "RAW image & EXIF data" device backend for Oyranos
 *  @internal
 *  @author   Yiannis Belias <orion@linux.gr>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/07/05
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
#include "helper.c"
/* --- internal definitions --- */

#define DBG printf("%s: %d\n", __FILE__, __LINE__ ); fflush(NULL);
/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "oyRE"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.device.icc_profile.raw-image." CMM_NICK
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( oyRE , CMMInit )
#define CMMallocateFunc         catCMMfunc( oyRE , CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( oyRE , CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( oyRE , CMMMessageFuncSet )
#define ConfigsFromPatternUsage catCMMfunc( oyRE , ConfigsFromPatternUsage )
#define DeviceFromName_         catCMMfunc( oyRE , DeviceFromName_ )
#define GetDevices              catCMMfunc( oyRE , GetDevices )
#define _api8                   catCMMfunc( oyRE , _api8 )
#define _rank_map               catCMMfunc( oyRE , _rank_map )
#define Configs_FromPattern     catCMMfunc( oyRE , Configs_FromPattern )
#define Config_Rank             catCMMfunc( oyRE , Config_Rank )
#define GetText                 catCMMfunc( oyRE , GetText )
#define _texts                  catCMMfunc( oyRE , _texts )
#define _cmm_module             catCMMfunc( oyRE , _cmm_module )
#define _api8_ui                catCMMfunc( oyRE, _api8_ui )
#define Api8UiGetText           catCMMfunc( oyRE, Api8UiGetText )
#define _api8_ui_texts          catCMMfunc( oyRE, _api8_ui_texts )
#define _api8_icon              catCMMfunc( oyRE, _api8_icon )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x

const char * GetText                 ( const char        * select,
                                       oyNAME_e            type );
const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type );

/** @instance _rank_map
 *  @brief    oyRankPad map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  \todo { Pick better rank fields }
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
      /* Makernote Tags: Serial Number */
   {const_cast < char *>("Exif.Canon.SerialNumber"), 1, 0, 0},        /**< nice to match */
   {const_cast < char *>("Exif.Fujifilm.SerialNumber"), 1, 0, 0},     /**< nice to match */
   {const_cast < char *>("Exif.Nikon3.SerialNumber"), 1, 0, 0},       /**< nice to match */
   {const_cast < char *>("Exif.Nikon3.SerialNO"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>("Exif.Olympus.SerialNumber"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>("Exif.Olympus.SerialNumber2"), 1, 0, 0},     /**< nice to match */
   {const_cast < char *>("Exif.OlympusEq.SerialNumber"), 1, 0, 0},    /**< nice to match */
   {const_cast < char *>("Exif.OlympusEq.InternalSerialNumber"), 1, 0, 0},/**< nice to match */
   {const_cast < char *>("Exif.Sigma.SerialNumber"), 1, 0, 0},        /**< nice to match */

      /* Makernote Tags: Lens */
   {const_cast < char *>("Exif.CanonCs.LensType"), 1, 0, 0},          /**< nice to match */
   {const_cast < char *>("Exif.CanonCs.Lens"), 1, 0, 0},              /**< nice to match */
   {const_cast < char *>("Exif.Minolta.LensID"), 1, 0, 0},            /**< nice to match */
   {const_cast < char *>("Exif.Nikon1.AuxiliaryLens"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>("Exif.Nikon2.AuxiliaryLens"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>("Exif.Nikon3.AuxiliaryLens"), 1, 0, 0},      /**< nice to match */
   {const_cast < char *>("Exif.Nikon3.LensType"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>("Exif.Nikon3.Lens"), 1, 0, 0},               /**< nice to match */
   {const_cast < char *>("Exif.OlympusEq.LensType"), 1, 0, 0},        /**< nice to match */
   {const_cast < char *>("Exif.OlympusEq.LensSerialNumber"), 1, 0, 0},/**< nice to match */
   {const_cast < char *>("Exif.OlympusEq.LensFirmwareVersion"), 1, 0, 0},/**< nice to match */
   {const_cast < char *>("Exif.Pentax.LensType"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>("Exif.Pentax.LensInfo"), 1, 0, 0},           /**< nice to match */
   {const_cast < char *>("Exif.Sigma.LensRange"), 1, 0, 0},           /**< nice to match */

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

int CMMInit( oyStruct_s * filter )
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

   // Standard EXIF Tags
   OPTIONS_ADD_EXIF(Exif.Image.Make)
   OPTIONS_ADD_EXIF(Exif.Image.Model)
   OPTIONS_ADD_EXIF(Exif.Photo.ISOSpeedRatings)
   OPTIONS_ADD_EXIF(Exif.Photo.ExposureProgram)
   OPTIONS_ADD_EXIF(Exif.Photo.Flash)
 
   // Makernote Tags: Serial Number
   OPTIONS_ADD_EXIF(Exif.Canon.SerialNumber)
   OPTIONS_ADD_EXIF(Exif.Fujifilm.SerialNumber)
   //OPTIONS_ADD_EXIF(Minolta) //Non existant?
   OPTIONS_ADD_EXIF(Exif.Nikon3.SerialNumber)
   OPTIONS_ADD_EXIF(Exif.Nikon3.SerialNO)
   OPTIONS_ADD_EXIF(Exif.Olympus.SerialNumber)
   OPTIONS_ADD_EXIF(Exif.Olympus.SerialNumber2)
   OPTIONS_ADD_EXIF(Exif.OlympusEq.SerialNumber)
   OPTIONS_ADD_EXIF(Exif.OlympusEq.InternalSerialNumber)
   //OPTIONS_ADD_EXIF(Exif.Panasonic.InternalSerialNumber) //!in libexiv2?
   //OPTIONS_ADD_EXIF(Pentax) //Non existant?
   OPTIONS_ADD_EXIF(Exif.Sigma.SerialNumber)
   //OPTIONS_ADD_EXIF(Sony) //Non existant?

   // Makernote Tags: Lens
   OPTIONS_ADD_EXIF(Exif.CanonCs.LensType)
   OPTIONS_ADD_EXIF(Exif.CanonCs.Lens)
   //OPTIONS_ADD_EXIF(Fujifilm) //Non existant?
   OPTIONS_ADD_EXIF(Exif.Minolta.LensID)
   OPTIONS_ADD_EXIF(Exif.Nikon1.AuxiliaryLens)
   OPTIONS_ADD_EXIF(Exif.Nikon2.AuxiliaryLens)
   OPTIONS_ADD_EXIF(Exif.Nikon3.AuxiliaryLens)
   OPTIONS_ADD_EXIF(Exif.Nikon3.LensType)
   OPTIONS_ADD_EXIF(Exif.Nikon3.Lens)
   OPTIONS_ADD_EXIF(Exif.OlympusEq.LensType)
   OPTIONS_ADD_EXIF(Exif.OlympusEq.LensSerialNumber)
   OPTIONS_ADD_EXIF(Exif.OlympusEq.LensFirmwareVersion)
   //OPTIONS_ADD_EXIF(Exif.Panasonic.ConversionLens) //!in libexiv2?
   //OPTIONS_ADD_EXIF(Exif.Panasonic.LensType) //!in libexiv2?
   //OPTIONS_ADD_EXIF(Exif.Panasonic.LensSerialNumber) //!in libexiv2?
   OPTIONS_ADD_EXIF(Exif.Pentax.LensType)
   OPTIONS_ADD_EXIF(Exif.Pentax.LensInfo)
   OPTIONS_ADD_EXIF(Exif.Sigma.LensRange)

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
   oyOption_s *version_opt = NULL;

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
            if (is_raw(Exiv2::ImageFactory::getType(filename)))
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

      //The std::auto_ptr::get() method returns the pointer owned by the auto_ptr
      if (device_handle.get() && device_handle->good())
         DeviceFromHandle(&device->backend_core, device_handle);
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
         device_list_opt->value = (oyValue_u*)device_list_opt->oy_->allocateFunc_(sizeof(oyValue_u));
         int i = 0;
         while(device_list[i++]);
         printf("################### Found %d devices #######################\n",i-1);
         //device_list_opt->value->string_list = const_cast<char**>(device_list);
         device_list_opt->value->string_list = (char**) device_list_opt->oy_->allocateFunc_(sizeof(char*) * i);
         memset( device_list_opt->value->string_list, 0, sizeof(char*) * i );

         const char ** cameras = device_list;
         i = 0;
         int mnft_n = -1;
         ptrdiff_t len;
         char manufacturer[128] = {0},
              manufacturer_old[128] = {0};

#define STRING_ADD(t_,add) \
{ int l_[2] = {0,0}; char * tmp = 0; if(t_) l_[0] = strlen(t_); \
  if(add) l_[1] = strlen(add); \
  tmp = (char*)device_list_opt->oy_->allocateFunc_(l_[0]+l_[1]+1); \
  tmp[0] = 0; \
  if(t_) sprintf(tmp, "%s", t_); sprintf( &tmp[strlen(tmp)], "%s", add ); \
  if(t_) device_list_opt->oy_->deallocateFunc_(t_); t_ = tmp; tmp = 0; }

         if(cameras)
         while(cameras[i])
         {
           len = strchr(cameras[i], ' ') - cameras[i];
           memcpy( manufacturer, cameras[i], len );
           manufacturer[len] = '\000';
           if(strcmp(manufacturer,manufacturer_old) != 0)
           {
             mnft_n++;
             if(mnft_n)
               device_list_opt->value->string_list[mnft_n-1][strlen(device_list_opt->value->string_list[mnft_n-1])-1] = '\000';
             STRING_ADD(device_list_opt->value->string_list[mnft_n],manufacturer)
             STRING_ADD(device_list_opt->value->string_list[mnft_n],"\n")
             sprintf( manufacturer_old, "%s", manufacturer );
           }

           STRING_ADD( device_list_opt->value->string_list[mnft_n],
                       &cameras[i][len+1] )
           STRING_ADD(device_list_opt->value->string_list[mnft_n],"\n")
           //cout << "  " << &cameras[i][len] << endl;
           ++i;
         }

#undef STRING_ADD

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
      oyConfig_Release(&device);

      message(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
           "This point should not be reached. Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
       );

      ConfigsFromPatternUsage((oyStruct_s *) options);
   } //End of Configuration

   /*Return the Configuration object*/
   oyConfigs_s *devices = *s;
   if(!devices)
     devices = oyConfigs_New(0);
   oyConfigs_MoveIn(devices, &device, -1);
   *s = devices;
 
   return error;
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
      ConfigsFromPatternUsage((oyStruct_s *) options);
      return 0;
   }

   /*Init for "driver_version" option*/
   int driver_version_number = LibRaw::versionNumber();
   const char *driver_version_string = LibRaw::version();

   const char *command_list = oyOptions_FindString(options, "command", "list");
   const char *command_properties = oyOptions_FindString(options, "command", "properties");

   oyOption_s *context_opt = oyOptions_Find(options, "device_context");
   oyOption_s *handle_opt = oyOptions_Find(options, "device_handle");
   oyOption_s *version_opt = oyOptions_Find(options, "driver_version");


   return 0;
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
  else if(strcmp(select, "device_class") == 0)
    {
        if(type == oyNAME_NICK)
            return _("RawCamera");
        else if(type == oyNAME_NAME)
            return _("RawCamera");
        else
            return _("Raw camera data, which are in file containing raw sensor data from a camera still picture.");
    }
  else if(strcmp(select,"category") == 0)
  {
    if(!category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Colour"),_("Device"),_("CameraRaw"),0};
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
const char * _api8_ui_texts[] = {"name", "help", "device_class", "category", 0};

/** @instance _api8_ui
 *  @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI parts for oyRE devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMui_s _api8_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,1,10},                            /**< int32_t module_api[3] */

  0, /* oyCMMFilter_ValidateOptions_f */
  0, /* oyWidgetEvent_f */

  "Colour/Device/CameraRaw", /* category */
  0,   /* const char * options */

  0,    /* oyCMMuiGet_f oyCMMuiGet */

  Api8UiGetText,  /* oyCMMGetText_f getText */
  _api8_ui_texts  /* (const char**)texts */
};

oyIcon_s _api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, (char*)"oyranos_logo.png"
};

/** @instance _api8
 *  @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMapi8_s oyranos::_api8 = {
   oyOBJECT_CMM_API8_S,
   0, 0, 0,
   (oyCMMapi_s*) 0,                                                   /**< next */
   CMMInit,                                                           /**< oyCMMInit_f      oyCMMInit */
   CMMMessageFuncSet,                                                 /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
   const_cast < char *>(CMM_BASE_REG),                                /**< registration */
   {0, 1, 0},                                                         /**< int32_t version[3] */
   {0, 1, 10},                                                        /**< int32_t module_api[3] */
   0,                                                                 /**< char * id_ */
   0,                                                                 /**< oyCMMapi5_s * api5_ */
   Configs_FromPattern,                                               /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
   Configs_Modify,                                                    /**< oyConfigs_Modify_f oyConfigs_Modify */
   Config_Rank,                                                       /**< oyConfig_Rank_f oyConfig_Rank */

  &_api8_ui,                                                          /**< device class UI name and help */
  &_api8_icon,                                                        /**< device icon */

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
         return _("Oyranos RAW Image");
      else
         return _("The raw image (hopefully)usefull backend of Oyranos.");
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
#define DFC_OPT_ADD_FLOAT_ARR(name, i, n) if(!error) { \
if (i==n-1) { \
   oyOption_s *opt = oyOption_New(CMM_BASE_REG OY_SLASH #name, 0); \
   opt->value_type = oyVAL_DOUBLE_LIST; /*FIXME*/ \
   oyOption_SetFromDouble(opt, params->name[i], i, 0); \
   oyOptions_MoveIn((*config)->backend_core, &opt, -1); \
} else { \
   oyOption_s *opt = oyOptions_Find((*config)->backend_core, #name); \
   oyOption_SetFromDouble(opt, params->name[i], i, 0); \
} }
#define DFC_OPT_ADD_FLOAT(name) if(!error) { \
        oyOption_s *opt = oyOption_New(CMM_BASE_REG OY_SLASH #name, 0); \
        oyOption_SetFromDouble(opt, params->name, 0, 0); \
        oyOptions_MoveIn((*config)->backend_core, &opt, -1); \
}
int DeviceFromContext(oyConfig_s **config, libraw_output_params_t *params)
{
   int error = 0;

   DFC_OPT_ADD_FLOAT_ARR(aber,3,4) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,2,4) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,1,4) //4
   DFC_OPT_ADD_FLOAT_ARR(aber,0,4) //4
   DFC_OPT_ADD_FLOAT_ARR(gamm,4,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,3,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,2,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,1,5) //5
   DFC_OPT_ADD_FLOAT_ARR(gamm,0,5) //5
   DFC_OPT_ADD_FLOAT_ARR(user_mul,3,4) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,2,4) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,1,4) //4
   DFC_OPT_ADD_FLOAT_ARR(user_mul,0,4) //4

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
