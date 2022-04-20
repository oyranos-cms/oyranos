/** @file oyranos_alpha.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    object APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/25
 */

#include "oyranos_config_internal.h"

#include "oyCMMapis_s.h"
#include "oyCMMapi6_s.h"
#include "oyCMMapi8_s.h"
#include "oyCMMapi9_s.h"
#include "oyCMMui_s.h"
#include "oyConfig_s.h"
#include "oyConfigs_s.h"
#include "oyConfig_s_.h"
#include "oyConversion_s.h"
#include "oyObserver_s.h"
#include "oyOption_s.h"
#include "oyOptions_s.h"
#include "oyPointer_s.h"
#include "oyProfile_s_.h"
#include "oyStructList_s_.h"
#include "oyranos_devices.h"

#include "oyranos_types.h"
#include "oyranos_alpha.h"
#include "oyranos_color.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_object_internal.h"
#include "oyranos_generic_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_db.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_icc.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <math.h>
#include <locale.h>   /* LC_NUMERIC */
#include <limits.h>

#define OY_ERR if(l_error != 0) error = l_error;

#ifdef DEBUG_
#define DEBUG_OBJECT 1
#endif

#ifndef NO_OPT
#define OY_USE_OBJECT_POOL_ 0
#endif



/* internal declarations */

/** \addtogroup alpha Alpha API's

 *  @{ *//* alpha */

#define PT_ANY       0    /* Don't check colorspace */
                          /* 1 & 2 are reserved */
#define PT_GRAY      3
#define PT_RGB       4
#define PT_CMY       5
#define PT_CMYK      6
#define PT_YCbCr     7
#define PT_YUV       8      /* Lu'v' */
#define PT_XYZ       9
#define PT_Lab       10
#define PT_YUVK      11     /* Lu'v'K */
#define PT_HSV       12
#define PT_HLS       13
#define PT_Yxy       14
#define PT_HiFi      15

/**
 *  @internal
 *  Function lcmsColorSpace
 *  @brief get lcms notation from icColorSpaceSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/21 (API 0.1.8)
 */
int lcmsColorSpace(icColorSpaceSignature ProfileSpace)
{    
       switch ((unsigned int)ProfileSpace) {

       case icSigGrayData: return  PT_GRAY;
       case icSigRgbData:  return  PT_RGB;
       case icSigCmyData:  return  PT_CMY;
       case icSigCmykData: return  PT_CMYK;
       case icSigYCbCrData:return  PT_YCbCr;
       case icSigLuvData:  return  PT_YUV;
       case icSigXYZData:  return  PT_XYZ;
       case icSigLabData:  return  PT_Lab;
       case icSigLuvKData: return  PT_YUVK;
       case icSigHsvData:  return  PT_HSV;
       case icSigHlsData:  return  PT_HLS;
       case icSigYxyData:  return  PT_Yxy;

       case icSig6colorData:
       case icSigMCH6Data: return PT_HiFi;

       default:  return 0;
       }
}

/** Function oyDumpColorToCGATS
 *  @brief create CGATS text from double value array and a profile
 *
 *  The funtion operates on raw color values. Alpha is not supported.
 *  The intention is to have a small debugging only tool. The values are
 *  required to come prescaled (IE RGB 0...255, XYZ 0...1, CMYK 0...100 ...)
 *
 *  @todo should be merged to some structure like oyImage_s.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/21 (API 0.1.8)
 */
oyChar *     oyDumpColorToCGATS     ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAlloc_f           allocateFunc,
                                       const oyChar      * DESCRIPTOR )
{
  int channels_n = oyProfile_GetChannelsCount( prof );
  int i;
  size_t len = n * channels_n * 16 + 2024;
  oyChar * text = allocateFunc( len );
  const oyChar * prof_name = prof ? ((oyProfile_s_*)prof)->file_name_ : "";
  const char * format = "";
  char * daten = text;
  /*double value = 0;*/

  const char *fomate[17] = {
    "SAMPLE_ID UNKNOWN 0", /* PT_ANY */
    "SAMPLE_ID UNKNOWN 1", /* 1 */
    "SAMPLE_ID UNKNOWN 2", /* 2 */
    "SAMPLE_ID        GRAY", /* 3 PT_GRAY */
    "SAMPLE_ID        RGB_R RGB_G RGB_B", /* 4 PT_RGB */
    "SAMPLE_ID        CMY_C  CMY_M  CMY_Y", /* 5 PT_CMY */
    "SAMPLE_ID        CMYK_C  CMYK_M  CMYK_Y  CMYK_K", /* 6 PT_CMYK */
    "SAMPLE_ID        ", /* 7 PT_YCbCr */
    "SAMPLE_ID        ", /* 8 PT_YUV */
    "SAMPLE_ID        XYZ_X XYZ_Y XYZ_Z", /* 9 PT_XYZ */
    "SAMPLE_ID        LAB_L LAB_A LAB_B", /* 10 PT_Lab */
    "SAMPLE_ID        ", /* 11 PT_YUVK */
    "SAMPLE_ID        ", /* 12 PT_HSV */
    "SAMPLE_ID        ", /* 13 PT_HLS */
    "SAMPLE_ID        ", /* 14 PT_Yxy */
    "SAMPLE_ID        ", /* 15 PT_HiFi */
    0
  };

  icColorSpaceSignature cspace = oyProfile_GetSignature( prof, oySIGNATURE_COLOR_SPACE );

  int lcms_space = lcmsColorSpace( cspace );
  char * save_locale = oyStringCopy_( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_);
  format = fomate[lcms_space];

  n *= channels_n;

  oySprintf_( daten ,
              "OYCGATS\n"
              "\n"
              "ORIGINATOR        \"%s\"\n"
              "DESCRIPTOR        \"%s\"\n"
              "CREATED           \"%s\"\n"
              "\n"
              "NUMBER_OF_FIELDS  %d\n"
              "BEGIN_DATA_FORMAT\n"
              "%s\n"
              "END_DATA_FORMAT\n"
              "\n"
              "NUMBER_OF_SETS    %d\n"
              "BEGIN_DATA\n",
              prof_name?prof_name:"--",
              DESCRIPTOR?DESCRIPTOR:"--",
              "--",
              channels_n + 1,
              format,
              (int)n/(channels_n?channels_n:-1)
            );

  setlocale(LC_NUMERIC, "C");
    for(i = 0; i < (int)n; ++i)
    {
      int modulo_k = i%(channels_n);
      if(modulo_k)
        oySprintf_( &daten[oyStrlen_(daten)], "       ");
      else
      {
        if(i)
          oySprintf_( &daten[oyStrlen_(daten)], "\n");
        oySprintf_( &daten[strlen(daten)], "%d     ", (i/channels_n) + 1);
      }
      oySprintf_( &daten[oyStrlen_(daten)], "%.03f", channels[i] );
    }
    oySprintf_( &daten[oyStrlen_(daten)], "\nEND_DATA\n");

  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );
    
  return text;
}

/** @} *//* alpha */


int      oyByteSwap                  ( void              * data,
                                       int                 Bps, /**< Bytes per sample can be 2, 4 or 8 */
                                       unsigned int        count )
{
  unsigned int i;
  int error = 0;
  unsigned char * c, ct;
  uint16_t * u16 = (uint16_t*) data;
  uint32_t * u32 = (uint32_t*) data;
  uint64_t * u64 = (uint64_t*) data;

  if(!data)
    return -1;

  switch(Bps)
  {
    case 2:
#pragma omp parallel for private(ct,c,i)
        for(i = 0; i < count; ++i)
        {
          c = (unsigned char*) &u16[i];
          // endianess is wonderful stuff
          ct = c[0]; c[0] = c[1]; c[1] = ct;
        }
        break;
    case 4:
#pragma omp parallel for private(ct,c,i)
        for(i = 0; i < count; ++i)
        {
          c = (unsigned char*) &u32[i];
          ct = c[0]; c[0] = c[3]; c[3] = ct;
          ct = c[2]; c[2] = c[1]; c[1] = ct;
        }
        break;
    case 8:
#pragma omp parallel for private(c,i)
        for(i = 0; i < count; ++i)
        {
          unsigned char  uint64_c[8];
          int little = 0,
              big    = 8;
          c = (unsigned char*) &u64[i];
          for (; little < 8 ; little++ )
            uint64_c[little] = c[big--];
          u64[i] = *((uint64_t*) &uint64_c[0]);
        }
        break;
    default:
        // should not happen
        error = 1;
        break;
  }
  return error;
}



/* --- Object handling API's start ------------------------------------ */



/** \addtogroup objects_generic Generic Objects

 *  @{ *//* objects_generic */



/** Function oyCallback_New
 *  @memberof oyCallback_s
 *  @brief   allocate a new Callback object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
OYAPI oyCallback_s * OYEXPORT
           oyCallback_New            ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CALLBACK_S;
# define STRUCT_TYPE oyCallback_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyCallback_s" );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    oyObject_Release( &s_obj );
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );
  if(error)
    WARNc_S( "memset failed" );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCallback_Copy;
  s->release = (oyStruct_Release_f) oyCallback_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
  if(error)
    WARNc_S( "oyObject_SetParent failed" );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** @internal
 *  Function oyCallback_Copy_
 *  @memberof oyCallback_s
 *  @brief   real copy a Callback object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
oyCallback_s * oyCallback_Copy_
                                     ( oyCallback_s      * obj,
                                       oyObject_s          object )
{
  oyCallback_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyCallback_New( object );
  error = !s;

  if(!error)
  {
    s->callback = obj->callback;
    s->data = obj->data;
  }

  if(error)
    oyCallback_Release( &s );

  return s;
}

/** Function oyCallback_Copy
 *  @memberof oyCallback_s
 *  @brief   copy or reference a Callback object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
OYAPI oyCallback_s * OYEXPORT
           oyCallback_Copy           ( oyCallback_s      * obj,
                                       oyObject_s          object )
{
  oyCallback_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CALLBACK_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCallback_Copy_( obj, object );

  return s;
}
 
/** Function oyCallback_Release
 *  @memberof oyCallback_s
 *  @brief   release and possibly deallocate a Callback object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
OYAPI int  OYEXPORT
           oyCallback_Release        ( oyCallback_s     ** obj )
{
  /* ---- start of common object destructor ----- */
  oyCallback_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_CALLBACK_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}




/** @} *//* objects_generic */









/** \addtogroup objects_conversion Conversion API's
 *  @{ *//* objects_conversion */


/** @internal
 *  @brief oyCMMProgress_f
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void             oyCMMProgress_      ( int                 ID,
                                       double              progress )
{
  printf("%s:%d id: %d %0.02f \n",__FILE__,__LINE__, ID, progress);
}



/** @} *//* objects_conversion */






/** \addtogroup monitor_api Monitor API
 *  @brief Hardware detection and profile selection for monitors

 *  The notation of screens for the X Window system is equal for traditional 
 *  and Xinerama screens. \n
 *  The X11 libraries will be loaded at runtime.
 *
 *  The functions can be studied by looking at the oyranos-monitor utiliy.
 *  The code for that tool is in oyranos_gamma.c .
 *
 *  @{ *//* monitor_api */

/** Function: oyGetMonitorInfo
 *  @brief   brief pick up monitor information with Xlib
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param      display_name  the display string
 *  @param[out] manufacturer  the manufacturer of the monitor device
 *  @param[out] model         the model of the monitor device
 *  @param[out] serial        the serial number of the monitor device
 *  @param[out] system_port   the system port name the monitor device is connectted to
 *  @param[out] display_geometry the display geometry description of the monitor
 *  @param[out] edid          the edid data blob of the monitor device
 *  @param      allocate_func the allocator for the above strings
 *  @return     error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
OY_DEPRECATED int      oyGetMonitorInfo            ( const char        * display_name OY_UNUSED,
                                       char             ** manufacturer OY_UNUSED,
                                       char             ** model OY_UNUSED,
                                       char             ** serial OY_UNUSED,
                                       char             ** system_port OY_UNUSED,
                                       char             ** display_geometry OY_UNUSED,
                                       oyBlob_s         ** edid OY_UNUSED,
                                       oyAlloc_f           allocate_func OY_UNUSED )
{
  int error = 0;

  error = 1;

  return error;
}

/** Function: oyGetScreenFromPosition
 *  @brief   screen number from position
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  This function will hit exact results only with Xinerama. \n
 *  a platform specific function
 *
 *  @param      display_name  the display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @return                   screen
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
OY_DEPRECATED int      oyGetScreenFromPosition     ( const char        * display_name OY_UNUSED,
                                       int                 x OY_UNUSED,
                                       int                 y OY_UNUSED )
{
  int screen = 0;

  return screen;
}

/** Function: oyGetDisplayNameFromPosition2
 *  @brief   display name from position
 *
 *  This function will hit exact results only with Xinerama.
 *
 *  @param[in]     device_type         the device type, e.g. OY_TYPE_STD,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        registration ::oyFILTER_REG_APPLICATION
 *                                     part, e.g. "monitor", mandatory
 *  @param      display_name  raw display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @param      allocateFunc  function used to allocate memory for the string
 *  @return                   display name
 *
 *  @version Oyranos: 0.1.13
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2010/11/12
 */
OY_DEPRECATED char *   oyGetDisplayNameFromPosition2(const char        * device_type,
                                       const char        * device_class,
                                       const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAlloc_f           allocateFunc )
{
  int error = 0;
  char * text = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;
  int n, i;
  const char * device_name = 0;

  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  /** we want a fuzzy look at our display, not the narrow "device_name" */
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/display_name",
                                 display_name, OY_CREATE_NEW );

  error = oyConfigs_FromDeviceClass ( device_type, device_class, options, &devices,
                                          0 );

  if(error>0)
    WARNc1_S("oyConfigs_FromDeviceClass() returned: %d", error);

  oyOptions_Release( &options );

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    o = oyConfig_Find( device, "device_rectangle" );

    r = (oyRectangle_s*) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );
    oyOption_Release( &o );

    if(!device_name &&
       r && oyRectangle_PointIsInside( r, x,y ))
    {
      device_name = oyConfig_FindString( device, "device_name", 0 );
      text = oyStringCopy_( device_name, allocateFunc );
    }
    oyRectangle_Release( &r );
    oyConfig_Release( &device );
  }
  oyConfigs_Release( &devices );

  return text;
}


/** Function: oyGetDisplayNameFromPosition
 *  @brief   display name from position
 *
 *  This function will hit exact results only with Xinerama.
 *
 *  @param      display_name  raw display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @param      allocateFunc  function used to allocate memory for the string
 *  @return                   display name
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
OY_DEPRECATED char *   oyGetDisplayNameFromPosition( const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAlloc_f           allocateFunc )
{
  int error = 0;
  char * text = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;
  int n, i;
  const char * device_name = 0;

  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  /** we want a fuzzy look at our display, not the narrow "device_name" */
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/display_name",
                                 display_name, OY_CREATE_NEW );

  error = oyConfigs_FromDeviceClass ( 0, "monitor", options, &devices,
                                          0 );

  if(error>0)
    WARNc1_S("oyConfigs_FromDeviceClass() returned: %d", error);

  oyOptions_Release( &options );

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    o = oyConfig_Find( device, "device_rectangle" );

    r = (oyRectangle_s*) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );
    oyOption_Release( &o );

    if(!device_name &&
       r && oyRectangle_PointIsInside( r, x,y ))
    {
      device_name = oyConfig_FindString( device, "device_name", 0 );
      text = oyStringCopy_( device_name, allocateFunc );
    }
    oyRectangle_Release( &r );
    oyConfig_Release( &device );
  }
  oyConfigs_Release( &devices );

  return text;
}


/** Function: oyGetMonitorProfile
 *  @brief   get the monitor profile from the server
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param      device_name        the display string
 *  @param[out] size                   the size of profile
 *  @param      allocate_func          function used to allocate memory for the profile
 *  @return                            the memory block containing the profile
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2009/02/10
 */
OY_DEPRECATED char *   oyGetMonitorProfile         ( const char        * device_name,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func )
{
  int error = 0;
  oyConfig_s * device = 0;
  oyOptions_s * options = 0;
  oyProfile_s * p = 0;
  char * block = 0;
  const char * device_type = OY_TYPE_STD,
             * device_class = "monitor";

  if(error <= 0)
  {
    error = oyDeviceGet( device_type, device_class, device_name,
                             options, &device );
    error = oyDeviceGetProfile( device, 0, &p );
    oyConfig_Release( &device );
  }

  if(error <= 0)
  {
    block = oyProfile_GetMem( p, size, 0, allocate_func );
    error = !block || !size;
  }

  return block;
}

/** Function: oyMonitorProfileNameFromDB
 *  @brief   get the monitor profile filename from the device
 *           database
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param         device          the device
 *  @param         allocateFunc        user function used to allocate memory 
 *  @return                            the profile filename
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OY_DEPRECATED char *   oyMonitorProfileNameFromDB  ( oyConfig_s        * device,
                                       oyAlloc_f           allocateFunc )
{
  int error = !device;
  oyProfile_s * p = 0;
  char * profile_name = 0;
  char * text = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );
  }

  if(error <= 0)
  {
    if(profile_name && oyStrrchr_( profile_name, OY_SLASH_C ) != 0)
      profile_name = oyStrrchr_( profile_name, OY_SLASH_C ) + 1;

    text = oyStringCopy_( profile_name, allocateFunc );
  }

  if(profile_name)
    oyFree_m_( profile_name );
  oyProfile_Release( &p );

  return text;
}

/** Function: oyGetMonitorProfileNameFromDB
 *  @brief   get the monitor profile filename from the device profile
 *           database
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param      display_name           the display string
 *  @param      allocateFunc  function used to allocate memory for the string
 *  @return                   the profiles filename (if localy available)
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
OY_DEPRECATED char *   oyGetMonitorProfileNameFromDB(const char        * display_name,
                                       oyAlloc_f           allocateFunc )
{
  int error = !display_name || !display_name[0];
  oyOptions_s * options = 0;
  oyConfig_s * device = 0;
  oyProfile_s * p = 0;
  char * profile_name = 0;
  char * text = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error > 0)
  {
    display_name = getenv( "DISPLAY" );
    error = !display_name;
  }

  if(error > 0)
  {
    WARNc_S( "No display_name argument provided. Give up." );
    return 0;
  }

  /** 1. get monitor device */
  if(error <= 0)
    oyDeviceGet( OY_TYPE_STD, "monitor", display_name, options, &device );

  oyOptions_Release( &options );

  /** 2. check for success of device detection */
  error = !device;
  if(error)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open device"), display_name );
    return 0;
  }

  if(error <= 0)
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );

  if(error <= 0)
  {
    if(profile_name && oyStrrchr_( profile_name, OY_SLASH_C ) != 0)
      text = oyStringCopy_( oyStrrchr_( profile_name, OY_SLASH_C ) + 1,
                            allocateFunc );
    else
      text = oyStringCopy_( profile_name, allocateFunc );

  }

  if(profile_name)
    oyFree_m_( profile_name );
  oyProfile_Release( &p );
  oyConfig_Release( &device );

  return text;
}

/** Function: oySetMonitorProfile
 *  @brief   set the monitor profile by filename
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  The profile_name argument does two things. Set to zero the function solely
 *  unsets the graphic card luts and the server stored profile. So pretty all
 *  server side information should go away. \n
 *  With a profile name given the function will lookup the monitor in the 
 *  Oyranos device database and stores the given profile there. \n
 *  To sum up, to set a new profile please call the following sequence:
 *  @code
    // store new settings in the Oyranos data base
    oySetMonitorProfile( display_name, profile_name );
    // remove the server entries
    oySetMonitorProfile( display_name, 0 );
    // update the window server from the newly Oyranos data base settings
    oyActivateMonitorProfiles( display_name );
    @endcode
 *
 *  @param      display_name  the display string
 *  @param      profile_name  the file to use as monitor profile or 0 to unset
 *  @return                   error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2009/01/28
 */
OY_DEPRECATED int      oySetMonitorProfile         ( const char        * display_name,
                                       const char        * profile_name )
{
  int error = !display_name || !display_name[0];
  oyOptions_s * options = 0;
  oyConfig_s * device = 0;

  if(error > 0)
  {
    WARNc1_S( "No display_name argument provided. Give up. %s",
              oyNoEmptyString_m_(profile_name) );
    return error;
  }

  /** 1. obtain detailed and expensive device informations */
  /** 1.1 add "properties" call to module arguments */
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                 "properties", OY_CREATE_NEW );

  /** 1.2 get monitor device */
  if(error <= 0)
    error = oyDeviceGet( OY_TYPE_STD, "monitor", display_name,
                             options, &device );

  oyOptions_Release( &options );

  /** 2. check for success of device detection */
  error = !device;
  if(error)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open device"), display_name );
    return error;
  }

  if(profile_name)
    error = oyDeviceSetProfile( device, oySCOPE_USER, profile_name );
  else
    error = oyDeviceUnset( device );

  oyConfig_Release( &device );

  return error;
}


/** Function: oyActivateMonitorProfiles
 *  @brief   activate the monitor using the stored configuration
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  Activate in case the appropriate profile is not yet setup in the server. \n
 *  To deactivate a profile in the server call 
 *  oySetMonitorProfile( device_name, 0 ).
 *
 *  @see oySetMonitorProfile for permanently configuring a monitor
 *
 *  @param   display_name              the device string
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2009/01/28
 */
OY_DEPRECATED int      oyActivateMonitorProfiles   ( const char        * display_name )
{
  int error = !display_name || !display_name[0];
  oyOptions_s * options = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  const char * device_class = "monitor";
  int i, n;

  if(error > 0)
  {
    WARNc_S( "No device_name argument provided. Give up." );
    return error;
  }

  {
    /* 1. set a general request */
    error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                   "list", OY_CREATE_NEW );
    /* we want a fuzzy look at our display, not as narrow as "device_name"*/
    error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/display_name",
                                   display_name, OY_CREATE_NEW );
    error = oyConfigs_FromDeviceClass ( 0, device_class, options,
                                            &devices, 0 );

    n = oyConfigs_Count( devices );
    for(i = 0; i < n; ++i)
    {
      device = oyConfigs_Get( devices, i );
      oyDeviceSetup( device, options );
      oyConfig_Release( &device );
    }

    oyConfigs_Release( &devices );
    oyOptions_Release( &options );
  }

  return error;
}


/** @} *//* monitor_api */


