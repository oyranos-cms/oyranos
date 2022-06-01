/** @file oyranos_cmm_oyRE.cpp
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009 (C) Yiannis Belias
 *
 *  @brief    Oyranos "RAW image & EXIF data" device backend for Oyranos
 *  @author   Yiannis Belias <orion@linux.gr>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/07/05
 */
/** \addtogroup oyre_device oyRE Module
 *
 *  libraw is a Camera RAW image processing library. See as well <a href="https://www.libraw.org">www.libraw.org</a> .
 *  @{ */
#include "oyCMM_s.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMui_s_.h"
#include "oyProfile_s_.h"

#include <oyranos_color.h>
#include <oyranos_cmm.h>
#include "oyranos_devices.h"
#include "oyranos_string.h"

#include <string.h>
#include <stdarg.h>
#include <stddef.h>           /* size_t ptrdiff_t */
#include <stdio.h>
#include <stdlib.h>

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>

#include <libraw/libraw.h>

#include <string>
#include <sstream>

#include "oyRE_help.c"
#include "helper.c"

using namespace std;
/* --- internal definitions --- */

#define PRFX "raw-image.oyRE: "
#define PRFX_EXIF "EXIF_"
#define PRFX_LRAW "LRAW_"
/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "oyRE"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "device" OY_SLASH "config.icc_profile.raw-image." CMM_NICK
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

#define catCMMfunc(nick,func) nick ## func

#define CMMinit                 catCMMfunc( oyRE , CMMinit )
#define CMMreset                catCMMfunc( oyRE , CMMreset )
#define CMMapiInit              catCMMfunc( oyRE , CMMapiInit )
#define CMMapiReset             catCMMfunc( oyRE , CMMapiReset )
#define CMMallocateFunc         catCMMfunc( oyRE , CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( oyRE , CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( oyRE , CMMMessageFuncSet )
#define ConfigsFromPatternUsage catCMMfunc( oyRE , ConfigsFromPatternUsage )
#define DeviceFromName_         catCMMfunc( oyRE , DeviceFromName_ )
#define GetDevices              catCMMfunc( oyRE , GetDevices )
#define _api8                   catCMMfunc( oyRE , _api8 )
#define Configs_FromPattern     catCMMfunc( oyRE , Configs_FromPattern )
#define Config_Rank             catCMMfunc( oyRE , Config_Rank )
#define GetText                 catCMMfunc( oyRE , GetText )
#define _texts                  catCMMfunc( oyRE , _texts )
#define _cmm_module             catCMMfunc( oyRE , _cmm_module )
#define _api8_ui                catCMMfunc( oyRE, _api8_ui )
#define Api8UiGetText           catCMMfunc( oyRE, Api8UiGetText )
#define _api8_ui_texts          catCMMfunc( oyRE, _api8_ui_texts )
#define _api8_icon              catCMMfunc( oyRE, _api8_icon )
#define _category               catCMMfunc( oyRE, _category )

#define _DBG_FORMAT_ OY_DBG_FORMAT_
#define _DBG_ARGS_ OY_DBG_ARGS_
/* i18n */
#include "oyranos_i18n.h"
#define DUMMY "filename\nblob"

const char * GetText                 ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * Api8UiGetText           ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
int Configs_Modify(oyConfigs_s * devices, oyOptions_s * options);


oyMessage_f oyRE_msg = 0;

static int _initialised = 0;
extern oyCMMapi8_s_ _api8;
static char * _category = NULL;


bool is_raw( int id );
int DeviceFromContext(oyConfig_s **config, libraw_output_params_t *params);
int DeviceFromHandle_opt(oyConfig_s *device, oyOption_s *option);

/* --- implementations --- */

int CMMapiInit( oyStruct_s * filter )
{
  int error = 0;
  const char * rfilter = "config.icc_profile.raw-image.oyRE";

  if(!_initialised)
    error = oyDeviceCMMInit( filter, rfilter, 0 );
  ++_initialised;

  return error;
}
int CMMapiReset( oyStruct_s * filter )
{
  int error = 0;

  if(_initialised)
  {
    error = oyDeviceCMMReset( filter );
    if(_category) oyFree_m_(_category);
    _initialised = 0;
  }

  return error;
}

int CMMinit( oyStruct_s * filter OY_UNUSED ) { return 0; }
int CMMreset( oyStruct_s * filter OY_UNUSED ) { return 0; }

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

/** @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int CMMMessageFuncSet(oyMessage_f message_func)
{
   oyRE_msg = message_func;
   return 0;
}

void ConfigsFromPatternUsage(oyStruct_s * options)
{
    /** oyMSG_WARN should make shure our message is visible. */
   oyRE_msg(oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
           _DBG_ARGS_, "The following help text informs about the communication protocol.");
   oyRE_msg(oyMSG_WARN, options, "%s", help_message);

   return;
}


oyProfile_s * createMatrixProfile      ( libraw_colordata_t & color,
                                         int32_t      icc_profile_flags,
                                         const char * manufacturer,
                                         const char * model,
                                         int        * is_existing OY_UNUSED )
{
  static oyProfile_s * p = NULL;

  if(color.profile_length)
    p = oyProfile_FromMem( color.profile_length, color.profile, 0,0);

  if(!p)
  {
    oyOption_s *matrix = oyOption_FromRegistration("///color_matrix."
              "from_primaries."
              "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma", NULL );

    int fail = 0;
    for(int i = 0; i < 3; ++i)
    {
      for(int j = 0; j < 3; ++j)
      {
        if(i < 3 && color.cam_xyz[i][j] == 0)
          fail = 1;
      }
    }
    oyMAT3 cam_zyx, pre_mul, ab_cm, ab_cm_inverse;
    oyCIExyYTriple ab_cm_inverse_xyY = {{{0,0},{0,0},{0,0}}};
    
    // Convert camera matrix to ICC profile
    // In theory that should perform the same conversion like dcraw/libraw do.

    memset(&pre_mul,0,sizeof(oyMAT3));
    for(int i = 0; i < 3; ++i)
      pre_mul.v[i].n[i] = color.pre_mul[i];
    for(int i = 0; i < 3; ++i)
      for(int j = 0; j < 3; ++j)
        // mirror diagonal
        cam_zyx.v[j].n[i] = color.cam_xyz[i][j];

    // DNG-1.3 says in Mapping Camera Color Space to CIE XYZ Space
    // XYZtoCamera = AB (AnalogBalance:pre_mul?) * CC (CameraCalibration2:?)
    //               * CM (ColorMatrix2:cam_xyz)

    // multiply AB * CM
    oyMAT3per( &ab_cm, &cam_zyx, &pre_mul );
    if(oyMAT3inverse( &ab_cm, &ab_cm_inverse ))
      // convert to CIE*xyY
      fail = oyMAT3toCIExyYTriple( &ab_cm_inverse, &ab_cm_inverse_xyY );
    else
    {
      fail = 1;
      oyRE_msg( oyMSG_WARN, (oyStruct_s*)0,
             OY_DBG_FORMAT_ "ab_cm is singular",
             OY_DBG_ARGS_ );
    }

    if(oy_debug)
    {
      printf("color.cam_xyz:\n%s",oyMat43show( color.cam_xyz ));
      printf("color.cam_mul:\n%s",oyMat4show( color.cam_mul ));
      printf("color.pre_mul:\n%s",oyMat4show( color.pre_mul ));
      printf("pre_mul:\n%s",oyMAT3show( const_cast<oyMAT3*>(&pre_mul) ));
      printf("color.rgb_cam:\n%s",oyMat34show( color.rgb_cam ));
      printf("color.cmatrix:\n%s",oyMat34show( color.cmatrix ));
      printf("ab*cm|pre_mul*cam_xyz:\n%s",oyMAT3show( const_cast<oyMAT3*>(&ab_cm) ));
      printf("ab_cm_inverse:\n%s",oyMAT3show( const_cast<oyMAT3*>(&ab_cm_inverse) ));
      if(!fail)
      printf("=> ");
      printf("ab_cm_inverse_xyY:\n%s", oyCIExyYTriple_Show(const_cast<oyCIExyYTriple*>(&ab_cm_inverse_xyY)));
    }
    if(!fail)
    {
      oyCIExyYTriple * use = &ab_cm_inverse_xyY;
      oyOption_SetFromDouble( matrix, use->v[0].xy[0], 0, 0);
      oyOption_SetFromDouble( matrix, use->v[0].xy[1], 1, 0);
      oyOption_SetFromDouble( matrix, use->v[1].xy[0], 2, 0);
      oyOption_SetFromDouble( matrix, use->v[1].xy[1], 3, 0);
      oyOption_SetFromDouble( matrix, use->v[2].xy[0], 4, 0);
      oyOption_SetFromDouble( matrix, use->v[2].xy[1], 5, 0);

      /* D65 */
      oyOption_SetFromDouble( matrix, 0.31271, 6, 0);
      oyOption_SetFromDouble( matrix, 0.32902, 7, 0);
    } else
    // fall back
    {
    /* http://www.color.org/chardata/rgb/rommrgb.xalter
     * original gamma is 1.8, we adapt to typical cameraRAW gamma of 1.0 */
      oyOption_SetFromDouble( matrix, 0.7347, 0, 0);
      oyOption_SetFromDouble( matrix, 0.2653, 1, 0);
      oyOption_SetFromDouble( matrix, 0.1596, 2, 0);
      oyOption_SetFromDouble( matrix, 0.8404, 3, 0);
      oyOption_SetFromDouble( matrix, 0.0366, 4, 0);
      oyOption_SetFromDouble( matrix, 0.0001, 5, 0);
      oyOption_SetFromDouble( matrix, 0.3457, 6, 0);
      oyOption_SetFromDouble( matrix, 0.3585, 7, 0);
      fail = 1;
    }
    oyOption_SetFromDouble( matrix, 1.0, 8, 0);

    char * name = NULL;
    oyOptions_s * opts = oyOptions_New(0);
    oyOptions_MoveIn( opts, &matrix, -1 );
    if(!fail)
    {
      matrix = oyOptions_Find( opts, "color_matrix", oyNAME_PATTERN );
      const char * ts = oyStringCopy_(oyOption_GetText( matrix, oyNAME_NICK ), oyAllocateFunc_ );
      oyOption_Release( &matrix );
      ts = strstr( ts, "color_matrix:" ) + strlen("color_matrix:");
      char * t = oyjlStringCopy(ts,0);
      oyjlStringReplace( &t, ",", " ", NULL,NULL );
      const char * mnf = NULL;
      if(manufacturer && model)
        mnf = strstr(model, manufacturer);
      oyStringAddPrintf_( &name, oyAllocateFunc_, oyDeAllocateFunc_,
                          "%s%s%s cam_xyz linear %s%s", mnf?"":manufacturer,
                          mnf?"":" ", model, t,
                          (icc_profile_flags & OY_ICC_VERSION_2) ? " v2" : " v4" );
      oyFree_m_( t );
      oyRE_msg(oyMSG_WARN, (oyStruct_s*)0,
          OY_DBG_FORMAT_ " name: \"%s\"",
          OY_DBG_ARGS_, name);


      oyProfile_SetSignature( p, icSigInputClass, oySIGNATURE_CLASS);

    } else
      name = oyStringCopy("ICC Examin ROMM gamma 1.0", oyAllocateFunc_ );

    p = oyProfile_FromName( name, icc_profile_flags, 0 );
    if(!p)
    {
      oyOptions_s * result = 0;

      oyOptions_SetFromInt( &opts, "///icc_profile_flags", icc_profile_flags, 0, OY_CREATE_NEW );
      const char * reg = "//" OY_TYPE_STD "/create_profile.color_matrix.icc";
      oyOptions_Handle( reg, opts, "create_profile.icc_profile.color_matrix",
                        &result );

      p = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                               oyOBJECT_PROFILE_S );
      oyOptions_Release( &result );
      if(!p)
        oyRE_msg(oyMSG_DBG, (oyStruct_s*)0,
          OY_DBG_FORMAT_ " profile creation failed by \"%s\"",
          OY_DBG_ARGS_, reg);

      oyProfile_AddTagText( p, icSigProfileDescriptionTag, name);
    }
    oyFree_m_( name );

    oyOptions_Release( &opts );

    if(oy_debug)
    {
      size_t size = 0;
      char * data = (char*) oyProfile_GetMem( p, &size, 0, malloc );
      if(!fail)
        oyWriteMemToFile_( "cam_xyz gamma 1.0.icc", data, size );
      else
        oyWriteMemToFile_( "ICC Examin ROMM gamma 1.0.icc", data, size );
    }
  }

  return p;
}

class exif2options {
   public:
      exif2options(Exiv2::ExifData *exif_data, oyOptions_s **options)
         :_exif_data_(exif_data), _options_(options) {}

      //FIXME We assume that any exif tag has exactly 2 '.' (dots)
      int add(const char *name)
      {
         std::string n(name), exif(name);
         n.replace(n.find("."),1,"_");
         if(n == PRFX_EXIF "Image_Make")
           n = PRFX_EXIF "manufacturer";
         else if(n == PRFX_EXIF "Image_Model")
           n = PRFX_EXIF "model";
         else if(n.find("SerialNumber") != std::string::npos)
           n = PRFX_EXIF "serial";

         exif.replace(0,5,"Exif.");

         std::ostringstream registration;
         registration << CMM_BASE_REG OY_SLASH << n.c_str();
         Exiv2::ExifKey key( exif );
         Exiv2::ExifData::iterator pos = _exif_data_->findKey(key);
         if (pos != _exif_data_->end()) {
            return oyOptions_SetFromString( _options_,
                                          registration.str().c_str(),
                                          pos->toString().c_str(), OY_CREATE_NEW );
         } else {
            return 0;
         }
      }
   private:
      Exiv2::ExifData *_exif_data_;
      oyOptions_s **_options_;
};

/** @internal
 *  @brief Get the options from EXIF Tags, and put them in a configuration object
 *
 *  @param[in]image              The Exiv2 raw image
 *  @param[out]options           The configuration object to hold the option-value pairs
 *
 * \todo { Untested }
 */
int DeviceFromHandle(oyOptions_s **options, Exiv2::Image::AutoPtr image)
{
   int error = 0;

   image->readMetadata();
   Exiv2::ExifData &exif_data = image->exifData();
   if (exif_data.empty()) //FIXME Add error message
      return 1;

   exif2options e2o(&image->exifData(),options);

   // Standard EXIF Tags
   error += e2o.add(PRFX_EXIF "Image.Model");
   error += e2o.add(PRFX_EXIF "Image.Make");
   error += e2o.add(PRFX_EXIF "Photo.ISOSpeedRatings");
   error += e2o.add(PRFX_EXIF "Photo.ExposureProgram");
   error += e2o.add(PRFX_EXIF "Photo.Flash");
 
   // Makernote Tags: Serial Number
   error += e2o.add(PRFX_EXIF "Canon.SerialNumber");
   error += e2o.add(PRFX_EXIF "Fujifilm.SerialNumber");
   //e2o.add("Minolta"); //Non existant?
   error += e2o.add(PRFX_EXIF "Nikon3.SerialNumber");
   error += e2o.add(PRFX_EXIF "Nikon3.SerialNO");
   //error += e2o.add(PRFX_EXIF "Olympus.SerialNumber"); // !in exiv2-0.23
   error += e2o.add(PRFX_EXIF "Olympus.SerialNumber2");
   error += e2o.add(PRFX_EXIF "OlympusEq.SerialNumber");
   error += e2o.add(PRFX_EXIF "OlympusEq.InternalSerialNumber");
   //e2o.add(PRFX_EXIF "Panasonic.InternalSerialNumber"); //!in libexiv2?
   //e2o.add("Pentax"); //Non existant?
   error += e2o.add(PRFX_EXIF "Sigma.SerialNumber");
   //e2o.add("Sony"); //Non existant?

   // Makernote Tags: Lens
   error += e2o.add(PRFX_EXIF "CanonCs.LensType");
   error += e2o.add(PRFX_EXIF "CanonCs.Lens");
   //e2o.add("Fujifilm"); //Non existant?
   error += e2o.add(PRFX_EXIF "Minolta.LensID");
   error += e2o.add(PRFX_EXIF "Nikon1.AuxiliaryLens");
   error += e2o.add(PRFX_EXIF "Nikon2.AuxiliaryLens");
   error += e2o.add(PRFX_EXIF "Nikon3.AuxiliaryLens");
   error += e2o.add(PRFX_EXIF "Nikon3.LensType");
   error += e2o.add(PRFX_EXIF "Nikon3.Lens");
   error += e2o.add(PRFX_EXIF "OlympusEq.LensType");
   error += e2o.add(PRFX_EXIF "OlympusEq.LensSerialNumber");
   error += e2o.add(PRFX_EXIF "OlympusEq.LensFirmwareVersion");
   //e2o.add(PRFX_EXIF "Panasonic.ConversionLens"); //!in libexiv2?
   //e2o.add(PRFX_EXIF "Panasonic.LensType"); //!in libexiv2?
   //e2o.add(PRFX_EXIF "Panasonic.LensSerialNumber"); //!in libexiv2?
   error += e2o.add(PRFX_EXIF "Pentax.LensType");
   error += e2o.add(PRFX_EXIF "Pentax.LensInfo");
   error += e2o.add(PRFX_EXIF "Sigma.LensRange");

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
   oyOption_s *handle_opt = NULL;

   int error = 0;
   const char *command_list = 0, *command_properties = 0;

   int rank = oyFilterRegistrationMatch(_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S);

   if(oy_debug > 2)
   oyRE_msg( oyMSG_DBG, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
            "entered Options:\n%s", _DBG_ARGS_,
            oyOptions_GetText(options, oyNAME_NICK) );

   /* "error handling" section */
   if (rank == 0) {
      oyRE_msg(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "Registration match Failed. Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }
   if (s == NULL) {
      oyRE_msg(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
              "oyConfigs_s is NULL! Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
          );
      return 1;
   }
   if (*s != NULL) {
      oyRE_msg(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
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

   handle_opt = oyOptions_Find(options, "device_handle", oyNAME_PATTERN);
   version_opt = oyOptions_Find(options, "driver_version", oyNAME_PATTERN);

   oyConfig_s *device = NULL;
   device = oyConfig_FromRegistration(CMM_BASE_REG, 0);
   /*A device *must* have a device_name!*/
   if(oyOptions_FindString( options, "device_name", NULL ))
     oyOptions_SetFromString(oyConfig_GetOptions(device,"backend_core"), CMM_BASE_REG OY_SLASH "device_name", oyOptions_FindString( options, "device_name", NULL ), OY_CREATE_NEW);
   else
     oyOptions_SetFromString(oyConfig_GetOptions(device,"backend_core"), CMM_BASE_REG OY_SLASH "device_name", "dummy", OY_CREATE_NEW);
   oyOptions_SetFromString(oyConfig_GetOptions(device,"backend_core"), CMM_BASE_REG OY_SLASH "prefix",
                          PRFX_EXIF "," PRFX_LRAW, OY_CREATE_NEW);

   oyConfigs_s *devices = *s;

   if (command_list) {
      /* "list" call section */

      if(oy_debug > 2)
      {
      oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX 
               "Backend core:\n%s", _DBG_ARGS_,
               oyOptions_GetText(*oyConfig_GetOptions(device,"backend_core"), oyNAME_NICK));
      oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX
               "Data:\n%s", _DBG_ARGS_,
               oyOptions_GetText(*oyConfig_GetOptions(device,"data"), oyNAME_NICK));
      }

      /*Handle "driver_version" option [IN] */
      if (version_opt) {
         error = oyOptions_SetFromString(oyConfig_GetOptions(device,"data"),
                                       CMM_BASE_REG OY_SLASH "driver_version_string",
                                       driver_version_string,
                                       OY_CREATE_NEW);
         error = oyOptions_SetFromInt(oyConfig_GetOptions(device,"data"),
                                      CMM_BASE_REG OY_SLASH "driver_version_number",
                                      driver_version_number,
                                      0,
                                      OY_CREATE_NEW);
      }

      const char * device_name = oyOptions_FindString( options, "device_name", NULL );
      if(device_name)
      {
        if(!devices)
          devices = oyConfigs_New(0);

        /*Copy the rank map*/
        oyConfig_SetRankMap( device, _api8.rank_map );
        oyConfigs_MoveIn( devices, &device, -1 );
        Configs_Modify( devices, options );

      } else
      /*Handle "supported_devices_info" option [OUT:informative]*/
      if (!handle_opt) {
         int i = 0;
         const char **device_list = LibRaw::cameraList();
         /*int num_devices = LibRaw::cameraCount();*/

         while(device_list[i++]);

         /*Handle "device_handle" option [OUT:informative]*/
         error = oyOptions_SetFromString(oyConfig_GetOptions(device,"data"),
                                       CMM_BASE_REG OY_SLASH "device_handle",
                                       DUMMY,
                                       OY_CREATE_NEW);

         if(oy_debug > 2)
         oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX 
                 "################### Found %d devices #######################",
                  _DBG_ARGS_, i-1);
         char *string_list = 0;
         const char ** cameras = device_list;
         i = 0;
         int mnft_n = -1;
         ptrdiff_t len;
         char manufacturer[128] = {0},
              manufacturer_old[128] = {0};

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
               STRING_ADD(string_list,"\n");
             STRING_ADD(string_list,manufacturer);
             STRING_ADD(string_list,":");
             sprintf( manufacturer_old, "%s", manufacturer );
           } else
           if(cameras[i+1])
             STRING_ADD( string_list,";");

           STRING_ADD( string_list,
                       &cameras[i][len+1] );
           //cout << "  " << &cameras[i][len] << endl;
           ++i;
         }

         oyOptions_SetFromString ( oyConfig_GetOptions(device,"data"),
                                 CMM_BASE_REG OY_SLASH "supported_devices_info",
                                 string_list,
                                 OY_CREATE_NEW | OY_STRING_LIST );
      }

      /*Copy the rank map*/
      oyConfig_SetRankMap( device, _api8.rank_map );

   } else if (command_properties) {
      /* "properties" call section */

      /*Handle "driver_version" option [IN] */
      if (version_opt) {
         error = oyOptions_SetFromString(oyConfig_GetOptions(device,"data"),
                                       CMM_BASE_REG OY_SLASH "driver_version_string",
                                       driver_version_string,
                                       OY_CREATE_NEW);
         error = oyOptions_SetFromInt(oyConfig_GetOptions(device,"data"),
                                      CMM_BASE_REG OY_SLASH "driver_version_number",
                                      driver_version_number,
                                      0,
                                      OY_CREATE_NEW);
      }

      if(!devices)
        devices = oyConfigs_New(0);

      /*Copy the rank map*/
      oyConfig_SetRankMap( device, _api8.rank_map );
      oyConfigs_MoveIn( devices, &device, -1 );
      Configs_Modify( devices, options );

   } else {
      /* not to be reached section, e.g. warning */
      oyConfig_Release(&device);

      oyRE_msg(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
           "This point should not be reached. Options:\n%s", _DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK)
       );

      ConfigsFromPatternUsage((oyStruct_s *) options);
   } //End of Configuration

   /*Return the Configuration object*/
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
   int error = 0;

   if(oy_debug > 2)
   oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX 
            "Options:\n%s",_DBG_ARGS_, oyOptions_GetText(options, oyNAME_NICK));

   /* "error handling" section */
   if (!devices || !oyConfigs_Count(devices)) {
      oyRE_msg(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ "\n "
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

   oyOption_s *context_opt = oyOptions_Find(options, "device_context", oyNAME_PATTERN);
   oyOption_s *handle_opt = oyOptions_Find(options, "device_handle", oyNAME_PATTERN);
   oyOption_s *version_opt = oyOptions_Find(options, "driver_version", oyNAME_PATTERN);

   int num_devices = oyConfigs_Count(devices);
   if (command_list) {
      /* "list" call section */

      for (int i = 0; i < num_devices; ++i) {
         int error = 0;
         oyConfig_s *device = oyConfigs_Get(devices, i);

         const char * t = oyOptions_GetText(*oyConfig_GetOptions(device,"backend_core"), oyNAME_NICK);
         if(oy_debug > 2)
         oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX 
                  "Backend core:\n%s", _DBG_ARGS_, t?t:"");
         t = oyOptions_GetText(*oyConfig_GetOptions(device,"data"), oyNAME_NICK);
         if(oy_debug > 2)
         oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX 
                  "Data:\n%s", _DBG_ARGS_, t?t:"");

         /*Handle "driver_version" option [IN/OUT] */
         oyOption_s *version_opt_dev = oyConfig_Find(device, "driver_version");
         if (!version_opt_dev && version_opt) {
            error = oyOptions_SetFromString(oyConfig_GetOptions(device,"backend_core"),
                                          CMM_BASE_REG OY_SLASH "driver_version_string",
                                          driver_version_string,
                                          OY_CREATE_NEW);
            if(error) WARNc2_S("%s %d", _("found issues"),error);
            error = oyOptions_SetFromInt(oyConfig_GetOptions(device,"backend_core"),
                                         CMM_BASE_REG OY_SLASH "driver_version_number",
                                         driver_version_number,
                                         0,
                                         OY_CREATE_NEW);
            if(error) WARNc2_S("%s %d", _("found issues"),error);
         }

         //FIXME: Should probably be removed, because command_list creates it anyway
         /*Handle "device_handle" option [OUT:informative]*/
         oyOption_s *handle_opt_dev = oyConfig_Find(device, "device_handle");
         if (!handle_opt_dev && handle_opt)
	 {
            error = oyOptions_SetFromString(oyConfig_GetOptions(device,"data"),
                                          CMM_BASE_REG OY_SLASH "device_handle",
                                          DUMMY,
                                          OY_CREATE_NEW);
            if(error) WARNc2_S("%s %d", _("found issues"),error);
         }

         /*Handle "supported_devices_info" option [OUT:informative]*/
         //FIXME: It is not here, because command_list creates it anyway
         //If used, it should become a function.

         /*Create static rank_map, if not already there*/
         if (!oyConfig_GetRankMap(device))
            oyConfig_SetRankMap( device, _api8.rank_map );

         /*Cleanup*/
         oyConfig_Release(&device);
         oyOption_Release(&version_opt_dev);
         oyOption_Release(&handle_opt_dev);
      }
   } else if (command_properties) {
      /* "properties" call section */

      for (int i = 0; i < num_devices; ++i) {
         oyConfig_s *device = oyConfigs_Get(devices, i);
         oyProfile_s * p = 0;
         int32_t icc_profile_flags = 0;
         int is_existing = 0;

         const char * t = NULL,
                    * device_name = oyConfig_FindString(device, "device_name", 0);
         if(oy_debug > 2)
         {
           t = oyOptions_GetText(*oyConfig_GetOptions(device,"backend_core"), oyNAME_NICK);
           oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX 
                     "Backend core:\n%s", _DBG_ARGS_, t?t:"");

           t = oyOptions_GetText(*oyConfig_GetOptions(device,"data"), oyNAME_NICK);
           oyRE_msg( oyMSG_DBG,  (oyStruct_s *) options, _DBG_FORMAT_ PRFX 
                     "Data:\n%s", _DBG_ARGS_, t?t:"");
         }

         /* All previous device properties are considered obsolete
          * and a new device is created. Basic options are moved from
          * the old to new device */

         /*Get the "device_handle" from old device
          * and populate device with H/W options [OUT]*/
         oyOption_s *handle_opt_dev = oyConfig_Find(device, "device_handle");
         if(!handle_opt_dev)
         {
           handle_opt_dev = oyOption_FromRegistration( CMM_BASE_REG OY_SLASH "device_handle", NULL );
           oyOption_SetFromString( handle_opt_dev, device_name, 0 );
         }

         if (handle_opt_dev) {
            DeviceFromHandle_opt(device, handle_opt_dev);
            oyOption_s *tmp = oyOption_Copy(handle_opt_dev, 0);
            oyOptions_MoveIn(*oyConfig_GetOptions(device,"data"), &tmp, -1);
            oyOption_Release(&handle_opt_dev);
         } else { /*Ignore device without a "device_handle"*/
           if(oyOptions_Count( *oyConfig_GetOptions(device,"backend_core") ) < 2)
             oyRE_msg(oyMSG_WARN, (oyStruct_s *) options, _DBG_FORMAT_ ": %s\n",
                     _DBG_ARGS_, "The \"device_handle\" is missing from config object!");
            oyConfig_Release(&device);
            continue;
         }

         /*Handle "driver_version" option [OUT] */
         oyOption_s *version_opt_dev = oyConfig_Find(device, "driver_version");
         if (version_opt_dev) {
            oyOption_s *tmp = oyOption_Copy(version_opt_dev, 0);
            oyOptions_MoveIn(*oyConfig_GetOptions(device,"backend_core"), &tmp, -1);
            oyOption_Release(&version_opt_dev);
         }

         /*Handle "device_name" option [OUT] */
         if (device_name) {
            oyOptions_SetFromString(oyConfig_GetOptions(device,"backend_core"), CMM_BASE_REG OY_SLASH "device_name", device_name, OY_CREATE_NEW);
         }


         /*Handle "device_context" option [OUT]*/
         oyOption_s *context_opt_dev = oyConfig_Find(device, "device_context");
         if (context_opt_dev || device_name) {
            LibRaw rip;
            libraw_output_params_t * device_context = NULL;

            if(!context_opt_dev && device_name)
            {
              error = rip.open_file( device_name );
              device_context = rip.output_params_ptr();
            } else
              device_context = *(libraw_output_params_t**)oyOption_GetData(context_opt, NULL, allocateFunc);

            oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags );

            DeviceFromContext(&device, device_context);
            if(oyOptions_FindString( options, "icc_profile.fallback", 0 ))
              /* fallback: try to get color matrix to build a profile */
              p = createMatrixProfile( rip.imgdata.color, icc_profile_flags,
                                       oyConfig_FindString( device, PRFX_EXIF "manufacturer", 0 ),
                                       oyConfig_FindString( device, PRFX_EXIF "model", 0 ),
                                       &is_existing );

            if(context_opt_dev)
              free(device_context);

            oyOption_s *tmp = oyOption_Copy(context_opt_dev, 0);
            oyOptions_MoveIn(*oyConfig_GetOptions(device,"data"), &tmp, -1);
            oyOption_Release(&context_opt_dev);
         }

         /* add device calibration to ICC device profiles meta tag */
         oyProfile_s * profile = (oyProfile_s*)oyOptions_GetType( options, -1,
                                        "icc_profile.add_meta",
                                        oyOBJECT_PROFILE_S );
         if(profile)
         {
           oyOptions_s * options = 0;
           size_t size = 0;
           oyPointer data = oyProfile_GetMem( profile, &size, 0, malloc );
           oyProfile_s * p = oyProfile_FromMem( size, data, 0, 0 );
           /* Filter the typical name spaces for embedding into the ICC profile.  */
           error = oyOptions_SetFromString( &options, "///set_device_attributes",
                                          "true", OY_CREATE_NEW );
           error = oyOptions_SetFromString( &options, "///key_prefix_required",
                                       PRFX_EXIF "." PRFX_LRAW ".prefix",
                                       OY_CREATE_NEW );
           oyProfile_AddDevice( p, device, options );

           oyProfile_Release( &profile );
           oyOptions_Release( &options );
           oyOptions_MoveInStruct( oyConfig_GetOptions(device,"data"),
                                CMM_BASE_REG OY_SLASH "icc_profile.add_meta",
                                (oyStruct_s**)&p, OY_CREATE_NEW );
         }

         /** tell the "icc_profile" in a oyProfile_s */
         if( oyOptions_FindString( options, "icc_profile", 0 ) ||
             oyOptions_FindString( options, "oyNAME_NAME", 0 ))
         {
            size_t size = 0;
            char * data = 0;
            int has;
            oyOption_s * o,
                       * o_tmp = NULL;

            has = 0;
            o = oyConfig_Find( device, "icc_profile" );
            if(o)
            {
              /* the device might have assigned a dummy icc_profile, to show 
               * it can handle the format. But thats not relevant here. */
              p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
              if(oyProfile_GetSignature( p, oySIGNATURE_MAGIC ) == icMagicNumber)
                has = 1;
              else
                oyOption_Release( &o );

              oyProfile_Release( &p );
            }

            if(oyOptions_FindString( options, "icc_profile.fallback", 0 ))
            {
              /* tried to build profile p from rip.imgdata.color above */
              if(p && !is_existing)
              {
                const char * t = 0;
                oyOptions_s * opts = NULL;

                if((t = oyConfig_FindString( device, PRFX_EXIF "manufacturer", 0 )) != 0)
                  error = oyProfile_AddTagText( p, icSigDeviceMfgDescTag, t);
                if((t = oyConfig_FindString( device, PRFX_EXIF "model", 0 )) != 0)
                  error = oyProfile_AddTagText( p, icSigDeviceModelDescTag, t);

                error = oyOptions_SetFromString( oyConfig_GetOptions(device,"backend_core"),
                                       CMM_BASE_REG OY_SLASH
                                       "OPENICC_automatic_generated",
                                       "1", OY_CREATE_NEW );

                /* embed meta tag */
                oyOptions_SetFromString( &opts, "///set_device_attributes",
                                               "true", OY_CREATE_NEW );
                oyOptions_SetFromString( &opts, "///key_prefix_required",
                                               PRFX_EXIF "." PRFX_LRAW ".OPENICC_",
                                               OY_CREATE_NEW );
                oyProfile_AddDevice( p, device, opts );
                oyOptions_Release( &opts );

                /* install here as images are not permanent devices */
                {
                  const char * desc = oyProfile_GetText( p, oyNAME_DESCRIPTION );
                  oyProfile_s * tmpp = oyProfile_FromName( desc, icc_profile_flags, 0 );
                  if(!tmpp)
                  {
                    oyOptions_SetFromString( &opts, "////device", "1", OY_CREATE_NEW );
                    error = oyProfile_Install( p, oySCOPE_USER, opts );
                    if(error)
                    {
                      oyRE_msg( oyMSG_WARN, (oyStruct_s *) p, _DBG_FORMAT_
                                "Install as user failed.", _DBG_ARGS_);
                    }
                    oyOptions_Release( &opts);
                  } else
                    oyProfile_Release( &tmpp );
                }

                data = (char*) oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
                if(data && size)
                { oyDeAllocateFunc_( data ); data = NULL; size = 0; }
              }
              oyOption_Release( &o_tmp );
              if(has == 0)
                o = oyOption_FromRegistration( CMM_BASE_REG OY_SLASH
                                "icc_profile.fallback", 0 );
              error = -1;
            }

            if(!o)
              o = oyOption_FromRegistration( CMM_BASE_REG OY_SLASH
                                "icc_profile", 0 );

            if(p)
            {
              int t_err = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
              if(t_err > 0)
                error = t_err;
            }
            else
            /** Warn and return issue on not found profile. */
            {
              /* Show the "icc_profile" option is understood. */
              p = 0;
              error = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
              error = -1;
            }

            if(!has)
              oyOptions_Set( *oyConfig_GetOptions(device,"data"), o, -1, 0 );
         }

         /*Copy the rank map*/
         oyConfig_SetRankMap( device, _api8.rank_map );

         /*Cleanup*/
         oyConfig_Release(&device);
      }
   }

   return error;
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
      if(oy_debug > 2)
      oyRE_msg( oyMSG_DBG, (oyStruct_s *) config, _DBG_FORMAT_
               "\n No config argument provided.", _DBG_ARGS_);
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
            return "camera";
        else if(type == oyNAME_NAME)
            return _("RawCamera");
        else
            return _("Raw camera data, which are in file containing raw sensor data from a camera still picture.");
    }
  else if(strcmp(select, "icc_profile_class")==0)
    {
      return "input";
    } 
  else if(strcmp(select,"category") == 0)
  {
    if(!_category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Color"),_("Device"),_("CameraRaw"),0};
      int len =  strlen(i18n[0]) + strlen(i18n[1]) + strlen(i18n[2]);
      _category = (char*)malloc( len + 64 );
      if(_category)
        sprintf( _category,"%s/%s/%s", i18n[0], i18n[1], i18n[2] );
      else
        oyRE_msg(oyMSG_WARN, (oyStruct_s *) 0, _DBG_FORMAT_ "\n " "Could not allocate enough memory.", _DBG_ARGS_);
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return _category;
    else
      return _category;
  }
  return 0;
}
const char * _api8_ui_texts[] = {"name", "help", "device_class", "icc_profile_class", "category", 0};

/** @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI parts for oyRE devices.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMui_s_ _api8_ui = {
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
  _api8_ui_texts,  /* (const char**)texts */
  (oyCMMapiFilter_s*)&_api8 /* oyCMMapiFilter_s*parent */
};

oyIcon_s _api8_icon = {
  oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"
};

/** @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/12/28
 */
oyCMMapi8_s_ oyRE_api8 = {
   oyOBJECT_CMM_API8_S,
   0, 0, 0,
   (oyCMMapi_s*) 0,                                                   /**< next */
   CMMapiInit,                                                        /**< oyCMMInit_f      oyCMMInit */
   CMMapiReset,                                                       /**< oyCMMreset_f     oyCMMReset */
   CMMMessageFuncSet,                                                 /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
   const_cast < char *>(CMM_BASE_REG),                                /**< registration */
   CMM_VERSION,                                                         /**< int32_t version[3] */
   CMM_API_VERSION,                                                   /**< int32_t module_api[3] */
   0,                                                                 /**< char * id_ */
   0,                                                                 /**< oyCMMapi5_s * api5_ */
   0,                                                                 /**< runtime_context */
   Configs_FromPattern,                                               /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
   Configs_Modify,                                                    /**< oyConfigs_Modify_f oyConfigs_Modify */
   Config_Rank,                                                       /**< oyConfig_Rank_f oyConfig_Rank */

  (oyCMMui_s*)&_api8_ui,                                              /**< device class UI name and help */
  &_api8_icon,                                                        /**< device icon */

   NULL                                                               /**< oyRankMap ** rank_map */
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
const char *GetText(const char *select, oyNAME_e type,
                                       oyStruct_s        * context OY_UNUSED)
{
   if (strcmp(select, "name") == 0) {
      if (type == oyNAME_NICK)
         return CMM_NICK;
      else if (type == oyNAME_NAME)
         return _("Oyranos RAW Image");
      else
         return _("The raw image backend of Oyranos.");
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

/** @brief    CMM_NICK module infos
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
   &_api8_icon,

  CMMinit,             /**< init() */
  CMMreset             /**< reset() */
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
        error = oyOptions_SetFromInt( oyConfig_GetOptions(*config,"backend_core"), \
                                      CMM_BASE_REG OY_SLASH PRFX_LRAW #name, \
                                      params->name[i], i, OY_CREATE_NEW );
#define DFC_OPT_ADD_INT(name) if(!error) \
        error = oyOptions_SetFromInt( oyConfig_GetOptions(*config,"backend_core"), \
                                      CMM_BASE_REG OY_SLASH PRFX_LRAW #name, \
                                      params->name, 0, OY_CREATE_NEW );
#define DFC_OPT_ADD_FLOAT_ARR(name, i, n) if(!error) { \
if (i==n-1) { \
   oyOption_s *opt = oyOption_FromRegistration(CMM_BASE_REG OY_SLASH PRFX_LRAW #name, 0);\
   oyOption_SetFromDouble(opt, params->name[i], i, 0); \
   oyOptions_MoveIn(*oyConfig_GetOptions(*config,"backend_core"), &opt, -1); \
} else { \
   oyOption_s *opt = oyOptions_Find(*oyConfig_GetOptions(*config,"backend_core"), #name, oyNAME_PATTERN); \
   oyOption_SetFromDouble(opt, params->name[i], i, 0); \
} }
#define DFC_OPT_ADD_FLOAT(name) if(!error) { \
        oyOption_s *opt = oyOption_FromRegistration(CMM_BASE_REG OY_SLASH PRFX_LRAW #name, 0); \
        oyOption_SetFromDouble(opt, params->name, 0, 0); \
        oyOptions_MoveIn(*oyConfig_GetOptions(*config,"backend_core"), &opt, -1); \
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

int DeviceFromHandle_opt(oyConfig_s *device, oyOption_s *handle_opt)
{
   Exiv2::Image::AutoPtr device_handle;
   oyAlloc_f allocateFunc = malloc;
   if (handle_opt) {
      char * filename = NULL;
      size_t size = 0;
      const Exiv2::byte *raw_data = (Exiv2::byte*) 
                                           oyOption_GetData( handle_opt, &size,
                                                            allocateFunc );
      if(raw_data)
      {
        if (is_raw(Exiv2::ImageFactory::getType(raw_data, size)))
           device_handle = Exiv2::ImageFactory::open(raw_data, size);
      } else
      {
        filename = oyOption_GetValueText( handle_opt, malloc );
        if(filename)
        {
          if (is_raw(Exiv2::ImageFactory::getType(filename)))
            device_handle = Exiv2::ImageFactory::open(filename);
          if(oy_debug > 2)
          oyRE_msg( oyMSG_DBG, (oyStruct_s *) device, _DBG_FORMAT_
               "filename = %s", _DBG_ARGS_, filename );
        }
        else
          oyRE_msg( oyMSG_WARN, (oyStruct_s *) device, _DBG_FORMAT_
                  "Option \"device_handle\" is of a wrong type", _DBG_ARGS_);
      }

      //The std::auto_ptr::get() method returns the pointer owned by the auto_ptr
      if (device_handle.get() && device_handle->good())
         DeviceFromHandle(oyConfig_GetOptions(device,"backend_core"), device_handle);
      else {
         int level = oyMSG_WARN;
         if(filename && strcmp( filename, "dummy" ) == 0)
           level = oyMSG_DBG;

         oyRE_msg( level, (oyStruct_s *) device, _DBG_FORMAT_
               "Unable to open raw image \"%s\"", _DBG_ARGS_, filename?filename:"");
         return 1;
      }
      if(filename)
      { free(filename); filename = 0; }
   } else
      return 1;

   return 0;
}

bool is_raw( int id )
{
   //using namespace Exiv2::ImageType;
   switch (id) {
      case 3: //crw:
      case 4: //tiff
      case 5: //mrw:
      case 7: //cr2:
      case 8: //raf:
      case 9: //orf:
      case 16: //rw2:
         return true;
         break;
      default:
         if(id == 0)
           return false;
         else
           return true;
   }
}
/**  @} *//* oyre_device */
