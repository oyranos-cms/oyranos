/** @file oyranos_alpha.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    misc alpha APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#include "oyranos_alpha.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_cmms.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_texts.h"
#if !defined(WIN32)
#include <dlfcn.h>
#include <inttypes.h>
#endif
#include <math.h>

/* internal declarations */
oyObject_s   oyObject_SetAllocators_ ( oyObject_s          object,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
int          oyObject_UnRef          ( oyObject_s          obj );
int          oyObject_Ref            ( oyObject_s          obj );
/*oyObject_s   oyObject_SetCMMPtr_     ( oyObject_s          object,
                                       const char        * cmm,
                                       oyPointer           cmm_ptr,
                                       const char        * resource,
                                       oyStructRelease_f   ptrRelease,
                                       oyChar            * func_name );*/
int32_t      oyObject_Hashed_        ( oyObject_s          s );

char **          oyCMMsGetNames_     ( int               * n,
                                       const char        * sub_path,
                                       const char        * ext,
                                       const char        * required_cmm );
char **          oyCMMsGetLibNames_  ( int               * n,
                                       const char        * required_cmm );

/** \addtogroup alpha Alpha API's

 *  @{
 */

/** \addtogroup misc Miscellaneous

 *  @{
 */

/** \addtogroup colour_low Basic colour calculations

 *  @{
 */


/** */
void
oyLab2XYZ (const double *CIELab, double * XYZ)
{
  const double * l = CIELab;
  /* double e = 216./24389.;             // 0.0088565 */
  /* double k = 24389./27.;              // 903.30 */
  double d = 6./29.;                  /* 0.20690 */

  double Xn = 0.964294;
  double Yn = 1.000000;
  double Zn = 0.825104;

  double fy = (l[0] + 16) / 116.;
  double fx = fy + l[1] / 500.;
  double fz = fy - l[2] / 200.;


  if(fy > d)
    XYZ[1] = Yn * pow( fy, 3 );
  else
    XYZ[1] = (fy - 16./116.) * 3 * pow( d, 2 ) * Yn;
  if(fx > d)
    XYZ[0] = Xn * pow( fx, 3 );
  else
    XYZ[0] = (fx - 16./116.) * 3 * pow( d, 2 ) * Xn;
  if(fz > d)
    XYZ[2] = Zn * pow( fz, 3 );
  else
    XYZ[2] = (fz - 16./116.) * 3 * pow( d, 2 ) * Zn;
}

/** */
void
oyXYZ2Lab (const double *XYZ, double * lab)
{
    /* white point D50 [0.964294 , 1.000000 , 0.825104]
     * XYZ->Lab is defined as (found with the help of Marti Maria):
     *
     * L* = 116*f(Y/Yn) - 16                     0 <= L* <= 100
     * a* = 500*[f(X/Xn) - f(Y/Yn)]
     * b* = 200*[f(Y/Yn) - f(Z/Zn)]
     *
     * and
     *
     *        f(t) = t^(1/3)                     1 >= t >  0.008856
     *         7.787*t + (16/116)          0 <= t <= 0.008856
     */

      double gamma = 1.0/3.0; /* standard is 1.0/3.0 */
      double XYZ_[3];
      double K = 24389./27.;
      double k = K/116.;      /* 7.787 */
      double e = 216./24389.; /* 0.008856 */
      int i;

      /* CIE XYZ -> CIE*Lab (D50) */
      XYZ_[0] = XYZ[0] / 0.964294;
      XYZ_[1] = XYZ[1] / 1.000000;
      XYZ_[2] = XYZ[2] / 0.825104;

      for(i = 0; i < 3; ++i)
      {
        if ( XYZ_[i] > e)
           XYZ_[i] = pow (XYZ_[i], gamma);
        else
           XYZ_[i] = k * XYZ_[i] + (16.0/116.0);
      }

      lab[0] = (116.0*  XYZ_[1] -  16.0);
      lab[1] = (500.0*( XYZ_[0] -  XYZ_[1]));
      lab[2] = (200.0*( XYZ_[1] -  XYZ_[2]));
}

/** Function oyCIEabsXYZ2ICCrelXYZ
 *  @brief CIE absolute colourimetric to ICC relative colorimetric
 *
 *  @param[in]     CIEXYZ              CIE absolute colourimetric input
 *  @param[out]    ICCXYZ              ICC relative colourimetric output
 *  @param[in]     XYZmin              CIE absolute black point
 *  @param[in]     XYZmax              CIE absolute media white
 *  @param[in]     XYZwhite            illuminant white
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/20 (API 0.1.8)
 */
void         oyCIEabsXYZ2ICCrelXYZ   ( const double      * CIEXYZ,
                                       double            * ICCXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite )
{
  int i = 0;
  for( ; i < 3; ++i )
    ICCXYZ[i] = (CIEXYZ[i] - XYZmin[i]) / (XYZmax[i] - XYZmin[i]) * XYZwhite[i];
}

/** Function oyICCXYZrel2CIEabsXYZ
 *  @brief ICC relative colourimetric to CIE absolute colorimetric
 *
 *  @param[in]     ICCXYZ              ICC relative colourimetric input
 *  @param[out]    CIEXYZ              CIE absolute colourimetric output
 *  @param[in]     XYZmin              CIE absolute black point
 *  @param[in]     XYZmax              CIE absolute media white
 *  @param[in]     XYZwhite            illuminant white
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/20 (API 0.1.8)
 */
void         oyICCXYZrel2CIEabsXYZ   ( const double      * ICCXYZ,
                                       double            * CIEXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite )
{
  int i = 0;
  for( ; i < 3; ++i )
    CIEXYZ[i] = (ICCXYZ[i] * (XYZmax[i] - XYZmin[i]) + XYZmin[i]) / XYZwhite[i];
}

/**
 *  @} *//* colour_low
 */


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
       switch (ProfileSpace) {

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

/** Function oyDumpColourToCGATS
 *  @brief create CGATS text from double value array and a profile
 *
 *  The funtion operates on raw colour values. Alpha is not supported.
 *  The intention is to have a small debugging only tool. The values are
 *  required to come prescaled (IE RGB 0...255, XYZ 0...1, CMYK 0...100 ...)
 *
 *  @todo should be merged to some structure like oyImage_s.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/21 (API 0.1.8)
 */
oyChar *     oyDumpColourToCGATS     ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAlloc_f           allocateFunc,
                                       const oyChar      * DESCRIPTOR )
{
  int channels_n = oyProfile_GetChannelsCount( prof );
  int i;
  size_t len = n * channels_n * 16 + 2024;
  oyChar * text = allocateFunc( len );
  const oyChar * prof_name = prof?prof->file_name_:"";
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

  icColorSpaceSignature cspace = oyProfile_GetSignature( prof, oySIGNATURE_COLOUR_SPACE );

  int lcms_space = lcmsColorSpace( cspace );
#if USE_GETTEXT
  const char * save_locale = setlocale(LC_NUMERIC, 0 );
#endif
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
              (int)n/channels_n
            );

#if USE_GETTEXT
  setlocale(LC_NUMERIC, "C");
#endif
    for(i = 0; i < n; ++i)
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
#if 0
      switch(lcms_space)
      {
        case PT_ANY:
        case PT_GRAY:
        case PT_YCbCr:
        case PT_YUV:
        case PT_XYZ:
        case PT_YUVK:
        case PT_HSV:
        case PT_HLS:
        case PT_Yxy:
             value = channels[i]; break;
        case PT_RGB:
             value = channels[i] * 255.; break;
        case PT_CMY:
        case PT_CMYK:
        case PT_HiFi:
             value = channels[i] * 100.; break;
        case PT_Lab:
             if(modulo_k == 0)
               value = channels[i] * 100.;
             else
               value = channels[i] * 255. - 127.;
             break;
        default: value = channels[i]; break;
      }
      oySprintf_( &daten[oyStrlen_(daten)], "%.03f", value);
#else
      oySprintf_( &daten[oyStrlen_(daten)], "%.03f", channels[i] );
#endif
    }
    oySprintf_( &daten[oyStrlen_(daten)], "\nEND_DATA\n");

#if USE_GETTEXT
  setlocale(LC_NUMERIC, save_locale);
#endif
    
  return text;
}


/** @brief copy pure colours

    handle colour only, without from set to -1 default
 *
 *  @since Oyranos: version 0.1.8
 *  @date  september 2007 (API 0.1.8)
 */
void
oyCopyColour ( const double * from,
               double       * to,
               int            n,
               oyProfile_s  * ref,
               int            channels_n )
{
  int i, j;
  icColorSpaceSignature sig = 0;
  int c = 0;
  int error = 0;

  if(!n || !to)
    return;

  if(ref)
    sig = oyProfile_GetSignature ( ref, oySIGNATURE_COLOUR_SPACE );
  else
    sig = icSigXYZData;

  c = oyICCColourSpaceGetChannelCount( sig );

  if(from)
    error = !memcpy( to, from, sizeof(double) * (n * c) );

  else
  {
    if(!channels_n && c)
      channels_n = c;
    else if(channels_n && !c)
      c = channels_n;

    if(channels_n)
    switch(sig)
    {
      case icSigLabData:
      case icSigLuvData:
      case icSigHsvData:
      case icSigHlsData:
      case icSigYCbCrData:
           for( i = 0; i < n; ++i )
           {
             to[i*channels_n+0] = -1;
             to[i*channels_n+1] = 0;
             to[i*channels_n+2] = 0;
             for( j = c; j < channels_n; ++j )
               if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
      case icSigXYZData:
      case icSigRgbData:
      default:
           for( i = 0; i < n; ++i )
           {
             for( j = 0; j < channels_n; ++j )
               if(j < c)
                 to[i*channels_n+j] = -1;
               else if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
    }
  }
}

/** @internal
 *  @brief deallocation and resetting
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
int          oyPointerReleaseFunc_   ( oyPointer         * ptr )
{
  oyDeAllocateFunc_( *ptr );
  *ptr = 0;
  return 0;
}

#define oyPointerRelease_m( ptr_ ) oyPointerReleaseFunc_((oyPointer*)ptr_)

/** @} *//* misc */





/* --- Object handling API's start ------------------------------------ */


/** \addtogroup misc Miscellaneous

 *  @{
 */


/** \addtogroup objects_generic Generic Objects

 *  @{
 */


oyPointer    oyStruct_Allocate       ( oyStruct_s        * st,
                                       size_t              size )
{
  oyAlloc_f allocateFunc = oyAllocateFunc_;

  if(st && st->oy_ && st->oy_->allocateFunc_)
    allocateFunc = st->oy_->allocateFunc_;

  return allocateFunc( size );
}

/** Function oyStructTypeToText
 *  @brief   Objects type to small string
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/12/28
 */
const char *     oyStructTypeToText  ( oyOBJECT_e          type )
{
  const char * text = "unknown";

  switch(type) {
    case oyOBJECT_NONE: text = "Zero - none"; break;
    case oyOBJECT_OBJECT_S: text = "oyObject_s"; break;
    case oyOBJECT_MONITOR_S: text = "oyMonitor_s"; break;
    case oyOBJECT_NAMED_COLOUR_S: text = "oyNamedColour_s"; break;
    case oyOBJECT_NAMED_COLOURS_S: text = "oyNamedColours_s"; break;
    case oyOBJECT_PROFILE_S: text = "oyProfile_s"; break;
    case oyOBJECT_PROFILE_TAG_S: text = "oyProfileTag_s"; break;
    case oyOBJECT_PROFILES_S: text = "oyProfiles_s"; break;
    case oyOBJECT_OPTION_S: text = "oyOption_s"; break;
    case oyOBJECT_OPTIONS_S: text = "oyOptions_s"; break;
    case oyOBJECT_REGION_S: text = "oyRegion_s"; break;
    case oyOBJECT_IMAGE_S: text = "oyImage_s"; break;
    case oyOBJECT_ARRAY2D_S: text = "oyArray2d_s"; break;
    case oyOBJECT_COLOUR_CONVERSION_S: text = "oyColourConversion_s";break;
    case oyOBJECT_FILTER_S: text = "oyFilter_s"; break;
    case oyOBJECT_FILTERS_S: text = "oyFilters_s"; break;
    case oyOBJECT_CONVERSION_S: text = "oyConversion_s"; break;
    case oyOBJECT_CONNECTOR_S: text = "oyConnector_s"; break;
    case oyOBJECT_FILTER_PLUG_S: text = "oyFilterPlug_s"; break;
    case oyOBJECT_FILTER_PLUGS_S: text = "oyFilterPlugs_s"; break;
    case oyOBJECT_FILTER_SOCKET_S: text = "oyFilterSocket_s"; break;
    case oyOBJECT_FILTER_NODE_S: text = "oyFilterNode_s"; break;
    case oyOBJECT_PIXEL_ACCESS_S: text = "oyPixelAccess_s"; break;
    case oyOBJECT_CMM_HANDLE_S: text = "oyCMMhandle_s"; break;
    case oyOBJECT_CMM_POINTER_S: text = "oyCMMptr_s"; break;
    case oyOBJECT_CMM_INFO_S: text = "oyCMMInfo_s"; break;
    case oyOBJECT_CMM_API_S: text = "oyCMMapi_s generic"; break;
    case oyOBJECT_CMM_API1_S: text = "oyCMMapi1_s old CMM"; break;
    case oyOBJECT_CMM_API2_S: text = "oyCMMapi2_s Monitors"; break;
    case oyOBJECT_CMM_API3_S: text = "oyCMMapi3_s Profile tags"; break;
    case oyOBJECT_CMM_API4_S: text = "oyCMMapi4_s Filter"; break;
    case oyOBJECT_CMM_API5_S: text = "oyCMMapi5_s MetaFilter"; break;
    case oyOBJECT_CMM_API6_S: text = "oyCMMapi6_s Context convertor"; break;
    case oyOBJECT_CMM_API7_S: text = "oyCMMapi7_s Filter run"; break;
    case oyOBJECT_CMM_API8_S: text = "oyCMMapi8_s Filter run"; break;
    case oyOBJECT_CMM_DATA_TYPES_S: text = "oyCMMDataTypes_s Filter"; break;
    case oyOBJECT_CMM_API_MAX: text = "not defined"; break;
    case oyOBJECT_ICON_S: text = "oyIcon_s"; break;
    case oyOBJECT_MODULE_S: text = "oyModule_s"; break;
    case oyOBJECT_EXTERNFUNC_S: text = "oyExternFunc_s"; break;
    case oyOBJECT_NAME_S: text = "oyName_s"; break;
    case oyOBJECT_COMP_S_: text = "oyComp_s_"; break;
    case oyOBJECT_FILE_LIST_S_: text = "oyFileList_s_"; break;
    case oyOBJECT_HASH_S: text = "oyHash_s"; break;
    case oyOBJECT_STRUCT_LIST_S: text = "oyStructList_s"; break;
    case oyOBJECT_BLOB_S: text = "oyBlob_s"; break;
    case oyOBJECT_CONFIG_S: text = "oyConfig_s"; break;
    case oyOBJECT_CONFIGS_S: text = "oyConfigs_s"; break;
    case oyOBJECT_MAX: text = "Max - none"; break;
  }

  return text;
}

/** @internal
 *  Function oyStruct_TypeToText
 *  @brief   Objects type to small string
 *  @deprecated
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/24
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 */
const char * oyStruct_TypeToText     ( const oyStruct_s  * st )
{ return oyStructTypeToText( st->type_ ); }


/** @brief oyName_s new
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_new              ( oyObject_s          object )
{
  oyAlloc_f allocateFunc = oyAllocateFunc_;
  /* ---- start of object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAME_S;
# define STRUCT_TYPE oyName_s
  int error = 0;
  STRUCT_TYPE * s = 0;

  if(object)
    allocateFunc = object->allocateFunc_;

  s = (STRUCT_TYPE*)allocateFunc(sizeof(STRUCT_TYPE));

  if(!s)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = type;

  s->copy = (oyStruct_Copy_f) oyName_copy;
  s->release = (oyStruct_Release_f) oyName_release;
# undef STRUCT_TYPE
  /* ---- end of object constructor ------- */

  return s;
}

/** @brief oyName_s copy
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/22 (Oyranos: 0.1.10)
 *  @date    2008/12/22
 */
int          oyName_copy_            ( oyName_s          * dest,
                                       oyName_s          * src,
                                       oyObject_s          object )
{
  int error = 0;
  oyName_s * s = dest;
  oyAlloc_f   allocateFunc   = oyAllocateFunc_;
  oyDeAlloc_f deallocateFunc = oyDeAllocateFunc_;

  if(!src || !dest)
    return 0;

  if(object)
  {
    allocateFunc = object->allocateFunc_;
    deallocateFunc = object->deallocateFunc_;
  }

  if(src->name)
    s = oyName_set_ ( s, src->name, oyNAME_NAME, allocateFunc, deallocateFunc );
  if(src->nick)
    s = oyName_set_ ( s, src->nick, oyNAME_NICK, allocateFunc, deallocateFunc );
  if(src->description)
    s = oyName_set_ ( s, src->description, oyNAME_DESCRIPTION, allocateFunc, deallocateFunc );

  if(!s)
    s = oyName_new(0);

  if(!error)
    error = !memcpy( s->lang, src->lang, 8 );

  return error;
}

/** @brief oyName_s copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_copy               ( oyName_s        * obj,
                                         oyObject_s        object )
{
  int error = 0;
  oyName_s * s = 0;

  if(!obj)
    return s;

  s = oyName_new( object );

  error = oyName_copy_( s, obj, object );

  return s;
}

/** @brief oyName_s deallocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
int          oyName_release          ( oyName_s         ** obj )
{
  int error = 0;

  if(!obj || !*obj)
    return 0;

  error = oyName_release_(obj, oyDeAllocateFunc_);

  *obj = 0;

  return error;
}

/** Function oyName_releaseMembers
 *  @memberof oyName_s
 *  @brief   release only members
 *
 *  Useful to release the member strings but not the struct itself, which can
 *  in this case be static. Deallocation uses oyDeAllocateFunc_().
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
int          oyName_releaseMembers   ( oyName_s          * obj,
                                       oyDeAlloc_f         deallocateFunc )
{
  int error = 0;
  oyName_s * s = 0;

  if(!obj)
    return 0;

  if(!deallocateFunc)
    deallocateFunc = oyDeAllocateFunc_;

  s = obj;

  if(s->nick)
    deallocateFunc(s->nick); s->nick = 0;

  if(s->name)
    deallocateFunc(s->name); s->name = 0;

  if(s->description)
    deallocateFunc(s->description); s->description = 0;

  return error;
}

/**
 *  @internal
 *  @brief oyName_s deallocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int          oyName_release_         ( oyName_s         ** obj,
                                       oyDeAlloc_f         deallocateFunc )
{
  /* ---- start of common object destructor ----- */
  oyName_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( s->type != oyOBJECT_NAME_S)
  {
    WARNc_S(("Attempt to release a non oyName_s object."))
    return 1;
  }
  /* ---- end of common object destructor ------- */

  *obj = 0;

  if(!deallocateFunc)
    return 0;

  oyName_releaseMembers( s, deallocateFunc );

  deallocateFunc( s );

  return 0;
}


/**
 *  @internal
 *  @brief naming plus automatic allocation
 *
 *  @param[in]    obj            the oyName_s struct
 *  @param[in]    text           the name should fit into usual labels
 *  @param[in]    type           the kind of name
 *  @param[in]    allocateFunc   memory management (defaults to oyAllocateFunc_)
 *  @param[in]    deallocateFunc optional memory management 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyName_s *   oyName_set_             ( oyName_s          * obj,
                                       const char        * text,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int error = 0;
  oyName_s * s = obj;

  if(obj && obj->type != oyOBJECT_NAME_S)
  {
    WARNc_S(("Attempt to edit a non oyName_s object."))
    return 0;
  }

  if(!s)
    s = oyName_new(0);

  if(!s) return s;

  s->type = oyOBJECT_NAME_S;

  {
#define oySetString_m(n_type)\
    if(!error) { \
      if(s->n_type && deallocateFunc) \
        deallocateFunc( s->n_type ); \
      s->n_type = oyStringCopy_( text, allocateFunc ); \
      if( !s->n_type ) error = 1; \
    } 
    switch (type) {
    case oyNAME_NICK:
         oySetString_m(nick) break;
    case oyNAME_NAME:
         oySetString_m(name) break;
    case oyNAME_DESCRIPTION:
         oySetString_m(description) break;
    default: break;
    }
#undef oySetString_
  }

  return s;
}

/**
 *  @internal
 *  Function oyName_get_
 *  @brief   get name
 *
 *  @param[in,out] obj                 name object
 *  @param         type                type of name
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
const char * oyName_get_             ( const oyName_s    * obj,
                                       oyNAME_e            type )
{
  const char * text = 0;
  const oyName_s * name = obj;
  if(!obj)
    return 0;

  switch(type)
  {
    case oyNAME_NICK:
         text = name->nick; break;
    case oyNAME_DESCRIPTION:
         text = name->description; break;
    case oyNAME_NAME:
    default:
         text = name->name; break;
  }

  return text;
}

/** @brief   test a boolean operator
 *
 *  The function requires to receive proper object arguments and valid ranges.
 *  @todo test
 *
 *  @return                            -1 for undefined, 1 - true, 2 - both,
 *                                     0 - false
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/26 (Oyranos: 0.1.9)
 *  @date    2008/11/26
 */
int          oyTextboolean_          ( const char        * text_a,
                                       const char        * text_b,
                                       oyBOOLEAN_e         type )
{
  int erg = -1;
  int error = !text_a || !text_b ||
              0 > type || type > oyBOOLEAN_UNION;

  if(!error)
  {
    if(type == oyBOOLEAN_INTERSECTION)
    {
      if(text_a && text_b)
        erg = (oyStrcmp_( text_a, text_b ) == 0) ? 2 : 0;
      else
        erg = 0;
    }
    if(type == oyBOOLEAN_SUBSTRACTION)
    {
      if(text_a && text_b)
        erg = oyStrcmp_( text_a, text_b ) != 0;
      else if(text_a)
        erg = 1;
      else
        erg = 0;
    }
    if(type == oyBOOLEAN_DIFFERENZ)
    {
      if(text_a && text_b)
        erg = oyStrcmp_( text_a, text_b ) != 0;
      else if(text_a || text_b)
        erg = 1;
      else
        erg = 0;
    }
    if(type == oyBOOLEAN_UNION)
    {
      if(text_a || text_b)
        erg = 2;
      else
        erg = 0;
    }
  }

  return erg;
}

/** @brief   test a boolean operator
 *
 *  The function requires to receive proper object arguments and valid ranges.
 *
 *  @return                            -1 for undefined, 1 - true, 0 - false
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
int          oyName_boolean          ( oyName_s          * name_a,
                                       oyName_s          * name_b,
                                       oyNAME_e            name_type,
                                       oyBOOLEAN_e         type )
{
  int erg = -1;
  int error = !name_a || !name_b ||
              0 > name_type || name_type > oyNAME_DESCRIPTION ||
              0 > type || type > oyBOOLEAN_UNION;

  const char *text_a = 0;
  const char *text_b = 0;

  if(!error)
  {
    if(name_type == oyNAME_NAME)
    {
      text_a = name_a->name;
      text_b = name_b->name;
    } else
    if(name_type == oyNAME_NICK)
    {
      text_a = name_a->nick;
      text_b = name_b->nick;
    } else
    if(name_type == oyNAME_DESCRIPTION)
    {
      text_a = name_a->description;
      text_b = name_b->description;
    }

    erg = oyTextboolean_( text_a, text_b, type );
  }

  return erg;
}



/** @internal
 *  Function oyStructList_Create
 *  @brief   create a new oyStruct_s list
 *
 *  @param         parent_type         type of parent object
 *  @param         list_name           optional list name
 *  @param         object              the optional object
 *  @return                            a empty list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
oyStructList_s * oyStructList_Create ( oyOBJECT_e          parent_type,
                                       const char        * list_name,
                                       oyObject_s          object )
{
  oyStructList_s * s = oyStructList_New(object);

  if(!s)
    return s;

  s->parent_type_ = parent_type;
  if(list_name)
    s->list_name = oyStringAppend_(0, list_name, s->oy_->allocateFunc_);

  return s;
}

/** @internal
 *  @brief create a new pointer list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
oyStructList_s * oyStructList_New    ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_STRUCT_LIST_S;
# define STRUCT_TYPE oyStructList_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;

  s->copy = (oyStruct_Copy_f) oyStructList_Copy;
  s->release = (oyStruct_Release_f) oyStructList_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  @brief oyStructList_s copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  14 december 2007 (API 0.1.8)
 */
oyStructList_s * oyStructList_Copy   ( oyStructList_s    * list,
                                       oyObject_s          obj )
{
  oyStructList_s * s = 0;
  int error = !list;

  if(!list)
    return s;

  if(list && !obj)
  {
    s = list;
    s->oy_ = oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyStructList_Create( list->type_, list->list_name, obj);

  error = !s;

  oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(list)
  {
    int i;

    s->n_reserved_ = (list->n_ > 10) ? (int)(list->n_ * 1.5) : 10;
    s->n_ = list->n_;
    s->ptr_ = oyAllocateFunc_( sizeof(int*) * s->n_reserved_ );

    for(i = 0; i < list->n_; ++i)
      if(list->ptr_[i]->copy)
        s->ptr_[i] = list->ptr_[i]->copy( list->ptr_[i], 0 );
  }

  oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return s;
}

/** @internal
 *  @brief oyStructList_s pointer add
 *
 *  @param[in]     list                the list
 *  @param[in]     ptr                 the handle this function takes ownership
 *                                     of
 *  @param[in]     pos                 the prefered position in the list. This
 *                                     option has affect of the order in list.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  30 november 2007 (API 0.1.8)
 */
int              oyStructList_MoveIn ( oyStructList_s    * list,
                                       oyStruct_s       ** ptr,
                                       int                 pos)
{
  oyStructList_s * s = list;
  int error = 0;
  int i;
  int set = 0;

  s = list;
  error = !s;

  if(!error)
    if(s->type_ != oyOBJECT_STRUCT_LIST_S)
      error = 1;

  if(!error)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error)
    error = !(ptr && *ptr && (*ptr)->type_);

  if(!error)
    /* search for a empty pointer and set */
    if(0 > pos || pos >= s->n_)
    {
      for(i = 0; i < s->n_; ++i)
        if(s->ptr_[i] == 0)
        {
          s->ptr_[i] = *ptr;
          set = 1;
        }
    }

  if(!error && !set)
  {
    int mult = (s->n_<7) ? 10 : (int)(s->n_ * 1.5);
    int len = 0;
    oyStruct_s ** tmp = 0;
    int real_copy = 0;

    /* reserve new memory */
    if(s->n_ >= s->n_reserved_)
    {
      len =  sizeof(oyPointer) * mult;
      s->n_reserved_ = mult;
      tmp = oyAllocateFunc_(len);

      error = !tmp;

      if(!error)
        error = !memset( tmp, 0, len );
      real_copy = 1;

    } else {

      tmp = s->ptr_;

    }

    /* position the new */
    if(pos < 0 || pos >= s->n_)
      pos = s->n_;

    /* sort the old to the new */
    {
      int j; 

      if(!error)
      if(pos < s->n_ || real_copy)
      for(i = s->n_; i >= 0; --i)
      {
        j = (i >= pos) ? i-1 : i;
        if(i != pos)
          /*if(real_copy)
            tmp[i] = oyHandle_copy_( s->ptr_[j], 0 );
          else*/
            tmp[i] = s->ptr_[j];
      }

      if(!error)
      {
        tmp[pos] = /*oyHandle_copy_(*/ *ptr/*, 0 )*/;
      }

      /* release old data */
      if(!error)
      {
        if(real_copy)
        {
          if(s->ptr_)
            oyDeAllocateFunc_(s->ptr_);
          s->ptr_ = tmp;
        }
      }

      /* set the final count */
      if(!error)
        ++s->n_;
    }

    *ptr = 0;
  }

  if(!error)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** @internal
 *  @brief oyStructList_s release
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyStructList_Release (oyStructList_s   ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyStructList_s * s = 0;

  if(!obj || !*obj)
    return 1;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_STRUCT_LIST_S)
  {
    WARNc_S(("Attempt to release a non oyStructList_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Clear(s);

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->ptr_)
      deallocateFunc(s->ptr_);

    if(s->list_name)
      deallocateFunc(s->list_name);

    oyObject_Release( &s->oy_ );

    s->type_ = 0;

    deallocateFunc( s );
  }

  return error;
}

/**
 *  @internal
 *  Function oyStructList_ReleaseAt
 *  @brief   oyStructList_s pointer release
 *
 *  release and shrink
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/21
 *  @since   2007/11/21 (Oyranos: 0.1.8)
 */
int            oyStructList_ReleaseAt( oyStructList_s    * list,
                                       int                 pos )
{
  oyStructList_s * s = list;
  int error = 0;

  error = !s;

  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(!error)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error && list)
  {
      if(0 <= pos && pos < s->n_)
      {
          if(s->ptr_[pos] && s->ptr_[pos]->release)
            s->ptr_[pos]->release( (oyStruct_s**)&s->ptr_[pos] );

          if(pos < s->n_ - 1)
            error = !memmove( &s->ptr_[pos], &s->ptr_[pos+1],
                              sizeof(oyStruct_s*) * (s->n_ - pos - 1));

          --s->n_;
      }
  }

  if(!error)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_Get_   ( oyStructList_s    * list,
                                       int                 pos )
{
  int n = 0;
  oyStructList_s * s = list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(!error)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(!error)
    n = s->n_;

  if(!error)
  if(pos >= 0 && n > pos && s->ptr_[pos])
    obj = s->ptr_[pos];

  return obj;
}

/**
 *  @internal
 *  Function oyStructList_GetType_
 *  @brief oyStructList_s pointer access
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetType_(oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_e          type )
{
  oyStruct_s * obj = oyStructList_Get_( list, pos );

  if(obj && obj->type_ != type)
    obj = 0;
  return obj;
}

/** @internal
 *  @brief oyStructList_s referenced pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetRef ( oyStructList_s    * list,
                                       int                 pos )
{
  oyStructList_s * s = list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  obj = oyStructList_Get_(list, pos);
  error = !obj;

  if(!error)
    error = oyStructList_ReferenceAt_(list, pos);

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return obj;
}

/** Function oyStructList_GetRefType
 *  @brief oyStructList_s pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetRefType( oyStructList_s * list,
                                       int                 pos,
                                       oyOBJECT_e          type )
{
  oyStruct_s * obj = oyStructList_GetRef( list, pos );

  if(obj && obj->type_ != type)
  {
    if(obj->oy_ && obj->release)
      obj->release( &obj );
    obj = 0;
  }

  return obj;
}

/** @internal
 *  @brief oyStructList_s pointer referencing
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int              oyStructList_ReferenceAt_( oyStructList_s * list,
                                       int                 pos )
{
  int n = 0;
  int error = 0;
  oyStructList_s * s = list;
  oyStruct_s * p = 0;

  if(s)
    n = s->n_;
  else
    error = 1;

  if(!error)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(!error)
  if(pos >= 0 && n > pos && s->ptr_[pos])
  {
    p = s->ptr_[pos];
    error = !(p && p->copy);

    if(!error)
      p = p->copy( p, 0 );
  }

  return !p;
}

/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
/*oyHandle_s **    oyStructList_GetRaw_( oyStructList_s    * list )
{
  int error = 0;
  oyStructList_s * s = list;
  oyHandle_s ** p = 0;

  if(!error)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(!error)
    p = s->ptr_;

  return p;
}*/

/**
 *  @brief oyStructList_s count
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
int              oyStructList_Count ( oyStructList_s   * list )
{
  int n = 0;
  oyStructList_s * s = list;
  int error = 0;

  if(!(s && s->type_ == oyOBJECT_STRUCT_LIST_S))
    error = 1;

  if(!error)
    n = s->n_;

  return n;
}

/**
 *  Function oyStructList_GetID
 *  @memberof oyStructList_s
 *  @brief   eventually build and obtain the lists member names
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/11/04
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 */
const char *     oyStructList_GetID  ( oyStructList_s    * s,
                                       int                 intent_spaces,
                                       uint32_t            flags )
{
  int error = !s;
  const char * text = 0;

  if(!error)
  {
    text = oyObject_GetName( s->oy_, oyNAME_NICK );
    if(!text)
      text = oyStructList_GetText( s, oyNAME_NICK, intent_spaces, flags );
  }

  return text;
}

/**
 *  Function oyStructList_GetText
 *  @memberof oyStructList_s
 *  @brief   build and obtain the lists member names
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/11/04
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 */
const char * oyStructList_GetText    ( oyStructList_s    * s,
                                       oyNAME_e            name_type,
                                       int                 intent_spaces,
                                       uint32_t            flags )
{
  int error = !s, i, n;
  char * hash_text = 0;
  char * text = 0;
  oyStruct_s * oy_struct = 0;

  if(!error)
  {
    oyAllocHelper_m_( text, char, intent_spaces + 1, 0, return 0 );
    for(i = 0; i < intent_spaces; ++i)
      text[i] = ' ';
    text[i] = 0;
    n = oyStructList_Count( s );
    for(i = 0; i < n; ++i)
    {
      oy_struct = oyStructList_Get_( s, i );
      hashTextAdd_m( text );
      /*hashTextAdd_m( text );*/
      hashTextAdd_m( oyObject_GetName( oy_struct->oy_, name_type ) );
      hashTextAdd_m( text );
    }

    oyObject_SetName( s->oy_, hash_text, name_type );

    if(hash_text && s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );
    hash_text = 0;
    oyFree_m_( text );

    hash_text = (oyChar*) oyObject_GetName( s->oy_, name_type );
  }

  return hash_text;
}

/**
 *  Function oyStructList_Clear
 *  @memberof oyStructList_s
 *  @brief   release all listed objects
 *
 *  @version Oyranos: 0.1.9
 *  @date    2008/11/27
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 */
int              oyStructList_Clear  ( oyStructList_s    * s )
{
  int error = !(s && s->type_ == oyOBJECT_STRUCT_LIST_S), i;

  if(!error)
    for(i = s->n_ - 1; i >= 0; --i)
      oyStructList_ReleaseAt( s, i );
  return error;
}

/**
 *  Function oyStructList_CopyFrom
 *  @memberof oyStructList_s
 *  @brief   clean "list" and copy all listed objects from "from" to "list".
 *
 *  @version Oyranos: 0.1.9
 *  @date    2008/11/27
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 */
int              oyStructList_CopyFrom(oyStructList_s    * list,
                                       oyStructList_s    * from,
                                       oyObject_s          object )
{
  oyStructList_s * s = list;
  int error = !(s && s->type_ == oyOBJECT_STRUCT_LIST_S && 
                from && from->type_ == oyOBJECT_STRUCT_LIST_S),
      i;
  int from_n;
  oyStruct_s * o;

  if(!error)
  {
    error = oyStructList_Clear( list );

    from_n = from->n_;
    for(i = 0; i < from_n && !error; ++i)
    {
      o = oyStructList_Get_( from, i );
      o = o->copy( o, object );
      error = !o;
      error = oyStructList_MoveIn( s, &o, -1 );
    }
  }

  return error;
}

/**
 *  @} *//* objects_generic
 */

/** @} *//* misc */



/** \addtogroup cmm_handling

 *  @{
 */


/** @internal
 *  @brief oyCMMptr_s allocator
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyCMMptr_s*
oyCMMptr_New_ ( oyAlloc_f         allocateFunc )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  s = oyAllocateWrapFunc_( sizeof(oyCMMptr_s), allocateFunc );
  error = !s;

  error = !memset(s, 0, sizeof(oyCMMptr_s));

  if(!error)
  {
    s->type = oyOBJECT_CMM_POINTER_S;
    s->copy = (oyStruct_Copy_f) oyCMMptr_Copy_;
    s->release = (oyStruct_Release_f) oyCMMptr_Release_;
  }

  ++s->ref;

  return s;
}

/** @internal
 *  @brief oyCMMptr_s copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  30 november 2007 (API 0.1.8)
 */
oyCMMptr_s *       oyCMMptr_Copy_    ( oyCMMptr_s        * cmm_ptr,
                                       oyAlloc_f           allocateFunc )
{
  oyCMMptr_s * s = cmm_ptr;
  int error = 0;

  error = !s;

  if(!error && s && s->type != oyOBJECT_CMM_POINTER_S)
    error = 1;

  if(!error)
    ++s->ref;
  else
    s = 0;

  return s;
}


/** Function oyCMMptr_Release
 *  @brief   release a oyCMMptr_s
 *  @ingroup backend_api
 *  @memberof oyCMMptr_s
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int                oyCMMptr_Release  ( oyCMMptr_s       ** obj )
{
  return oyCMMptr_Release_( obj );
}

/** @internal
 *  @brief oyCMMptr_s release
 *
 *  @since Oyranos: version 0.1.8
 *  @date  30 november 2007 (API 0.1.8)
 */
int                oyCMMptr_Release_ ( oyCMMptr_s       ** obj )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  if(!obj || !*obj)
    error = 1;

  if(!error)
  s = *obj;

  error = !s;

  if(!error && s && s->type != oyOBJECT_CMM_POINTER_S)
    error = 1;

  if(!error)
    *obj = 0;

  if(!error)
    if(--s->ref)
      return error;

  if(!error)
  {
    s->type = 0;

    if(s->ptr)
    {
      if(s->ptrRelease)
        error = s->ptrRelease( &s->ptr );
      else
        error = oyPointerRelease_m( &s->ptr );

      if(!error)
        error = oyCMMdsoRelease_( s->lib_name );
    }
    s->ptrRelease = 0;
    oyPointerRelease_m( &s );
  }

  return error;
}

/** @internal
 *  @brief set oyCMMptr_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/12/27
 */
int                oyCMMptr_Set_     ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  oyCMMptr_s * s = cmm_ptr;
  int error = !s;

  if(!error && lib_name)
    s->lib_name = oyStringCopy_( lib_name, oyAllocateFunc_ );

  if(!error && func_name)
    if(oyStrlen_(func_name) < 32)
      oySprintf_(s->func_name, func_name); 

  if(!error && ptr)
  {
    if(s->ptrRelease && s->ptr)
      s->ptrRelease( &ptr );
    s->ptr = ptr;
  }

  if(!error && resource)
    if(oyStrlen_(resource) < 5)
      oySprintf_(s->resource, resource); 

  if(!error && ptrRelease)
    s->ptrRelease = ptrRelease;

  return error;
}

/** @internal
 *  @brief   convert between oyCMMptr_s data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int          oyCMMptr_ConvertData    ( oyCMMptr_s        * cmm_ptr,
                                       oyCMMptr_s        * cmm_ptr_out,
                                       oyFilterNode_s    * node )
{
  int error = !cmm_ptr || !cmm_ptr_out;
  oyCMMapi6_s * api6 = 0;
  char * reg = 0, * tmp = 0;

  if(!error)
  {
    reg = oyStringCopy_( "//", oyAllocateFunc_ );
    tmp = oyFilterRegistrationToText( node->filter->registration_,
                                      oyFILTER_REG_TYPE,0);
    STRING_ADD( reg, tmp );
    oyFree_m_( tmp );
    STRING_ADD( reg, "/" );
    STRING_ADD( reg, cmm_ptr->resource );
    STRING_ADD( reg, "_" );
    STRING_ADD( reg, cmm_ptr_out->resource );

    api6 = (oyCMMapi6_s*) oyCMMsGetFilterApi_( 0,0, reg, oyOBJECT_CMM_API6_S );

    error = !api6;
  }

  if(!error && api6->oyCMMdata_Convert)
    error = api6->oyCMMdata_Convert( cmm_ptr, cmm_ptr_out, node );
  else
    error = 1;

  if(error)
    WARNc_S("Could not convert context");

  return error;
}







oyStructList_s * oy_cmm_handles_ = 0;
/** @internal
 *  @brief Oyranos wrapper for dlopen
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
int oyDlclose(oyPointer* handle)
{
  if(handle && *handle)
  {
    dlclose(*handle);
    *handle = 0;
    return 0;
  }
  return 1;
}

/** @internal
 *  @brief register Oyranos CMM dlopen handle
 *
 *  use in pair with oyCMMdsoRelease_
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyCMMdsoReference_    ( const char        * lib_name,
                                     oyPointer           ptr )
{
  int i, n;
  int found = 0;
  int error = 0;

  if(!oy_cmm_handles_)
  {
    oy_cmm_handles_ = oyStructList_New( 0 );
    error = !oy_cmm_handles_;
  }

  if(!error && oy_cmm_handles_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_handles_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_(oy_cmm_handles_, i);
    oyCMMptr_s * s = 0;

    if(obj && obj->type_ == oyOBJECT_CMM_POINTER_S)
      s = (oyCMMptr_s*) obj;

    if( s && s->lib_name && lib_name &&
        !oyStrcmp_( s->lib_name, lib_name ) )
    {
      found = 1;
      oyStructList_ReferenceAt_(oy_cmm_handles_, i);
      if(ptr)
      {
        if(!s->ptr)
          s->ptr = ptr;
        /*else
          WARNc_S(("Attempt to register dso handle multiple times."));*/
      }
    }
  }

  if(!found)
  {
    oyCMMptr_s * s = oyCMMptr_New_(oyAllocateFunc_);
    oyStruct_s * oy_cmm_struct = 0;

    error = !s;

    if(!error)
      error = oyCMMptr_Set_( s, lib_name, 0, ptr, "oyDlclose", oyDlclose );

    if(!error)
      oy_cmm_struct = (oyStruct_s*) s;

    if(!error)
      oyStructList_MoveIn(oy_cmm_handles_, &oy_cmm_struct, -1);
  }

  return error;
}

/** @internal
 *  @brief search a dlopen handle for a Oyranos CMM
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int          oyCMMdsoSearch_         ( const char        * lib_name )
{
  int i, n;
  int pos = -1;
  int error = 0;

  if(!oy_cmm_handles_)
    return 1;

  if(oy_cmm_handles_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_handles_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_(oy_cmm_handles_, i);
    oyCMMptr_s * s = 0;

    if(obj && obj->type_ == oyOBJECT_CMM_POINTER_S)
      s = (oyCMMptr_s*) obj;

    error = !s;

    if(!error)
    if( s->lib_name && lib_name &&
        !oyStrcmp_( s->lib_name, lib_name ) )
      pos = i;
  }

  return pos;
}

/** @internal
 *  @brief release Oyranos CMM dlopen handle
 *
 *  use in pair in oyCMMdsoReference_
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/12/08
 */
int          oyCMMdsoRelease_      ( const char        * lib_name )
{
  int error = 0;

  /* inactive */
#if 0
  int found = -1;

  if(!oy_cmm_handles_)
    return 1;

  if(oy_cmm_handles_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(!error)
    found = oyCMMdsoSearch_(cmm);

  if(found >= 0)
    oyStructList_ReleaseAt(oy_cmm_handles_, found);
#endif
  return error;
}

/** @internal
 *  @brief get Oyranos CMM dlopen handle
 *
 *  Search the cache for a handle and return it. Or dlopen the library and 
 *  store and reference the handle in the cache.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyPointer    oyCMMdsoGet_            ( const char        * cmm,
                                       const char        * lib_name )
{
  int found = -1;
  oyPointer dso_handle = 0;

  if(!lib_name)
    return 0;

  found = oyCMMdsoSearch_(lib_name);

  if(found >= 0)
  {
    oyCMMptr_s * s = (oyCMMptr_s*)oyStructList_GetType_( oy_cmm_handles_, found,
                                                  oyOBJECT_CMM_POINTER_S );

    if(s)
      dso_handle = s->ptr;
  }

  if(!dso_handle)
  {
    dso_handle = dlopen(lib_name, RTLD_LAZY);

    if(!dso_handle)
      WARNc_S((dlerror()))
  }

  if(dso_handle)
    oyCMMdsoReference_( lib_name, dso_handle );

  return dso_handle;
}



/** @internal
 *  @brief new module handle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *    oyCMMhandle_New_    ( oyObject_s        object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_HANDLE_S;
# define STRUCT_TYPE oyCMMhandle_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCMMhandle_Copy_;
  s->release = (oyStruct_Release_f) oyCMMhandle_Release_;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  @brief copy module handle 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *  oyCMMhandle_Copy_     ( oyCMMhandle_s   * handle,
                                         oyObject_s        object )
{
  int error = !handle;
  oyCMMhandle_s * s = 0;

  if(!error && object)
  {
    s = oyCMMhandle_New_(object);
    error = !s;
    if(!error)
    {
      if(handle)
        WARNc1_S("Dont know how to copy CMM[%s] handle.", handle->lib_name);

      error = oyCMMhandle_Set_( s, 0, 0, 0 );
    } else
      WARNc_S("Could not create a new object.");

  } else {

    oyObject_Copy( handle->oy_ );
    s = handle;

  }

  return s;
}

/** @internal
 *  @brief release module handle 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
int              oyCMMhandle_Release_( oyCMMhandle_s    ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyCMMhandle_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_CMM_HANDLE_S)
  {
    WARNc_S("Attempt to release a non oyCMMhandle_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  error = oyCMMdsoRelease_( s->lib_name );

  s->dso_handle = 0;
  s->info = 0;
  oyFree_m_( s->lib_name );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

/** @internal
 */
int              oyCMMhandle_Set_    ( oyCMMhandle_s     * handle,
                                       oyCMMInfo_s       * info,
                                       oyPointer           dso_handle,
                                       const char        * lib_name )
{
  int error = 0;

  if(!handle || !info || !dso_handle)
    error = 1;

  if(!error)
  {
    handle->info = info;
    oyCMMdsoReference_( handle->lib_name, dso_handle );
    handle->dso_handle = dso_handle;
    handle->lib_name = oyStringCopy_( lib_name, handle->oy_->allocateFunc_ );
  }

  return error;
}



/** @internal
 *  @brief internal Oyranos module handle list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_infos_ = 0;

/** @internal
 *  @brief search a Oyranos module handle in a internal list
 *
 *  a intermediate step
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *  oyCMMFromCache_     ( const char        * lib_name )
{
  int error = !lib_name;
  int n, i;
  oyCMMhandle_s * cmm_handle = 0;

  if(!error && !oy_cmm_infos_)
  {
    oy_cmm_infos_ = oyStructList_New( 0 );
    error = !oy_cmm_infos_;
  }

  if(!error && oy_cmm_infos_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_infos_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyCMMhandle_s * cmmh = (oyCMMhandle_s*) oyStructList_GetType_(oy_cmm_infos_,
                                                i, oyOBJECT_CMM_HANDLE_S );
    oyCMMInfo_s * s = 0;

    if(cmmh)
      s = (oyCMMInfo_s*) cmmh->info;

    if( s && s->type == oyOBJECT_CMM_INFO_S &&
        (uint32_t*)&s->cmm &&
        !oyStrcmp_( cmmh->lib_name, lib_name ) )
    {
      cmm_handle = oyCMMhandle_Copy_( cmmh, 0 );
      error = oyStructList_ReferenceAt_( oy_cmm_infos_, i );
    }
  }

  return cmm_handle;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  11 december 2007 (API 0.1.8)
 */
char *           oyCMMnameFromLibName_(const char        * lib_name)
{
  char * cmm = 0;

  if(lib_name && oyStrlen_(lib_name))
  {
    const char * tmp = oyStrstr_( lib_name, OY_MODULE_NAME );

    if(!tmp && oyStrlen_( lib_name ) == 4)
      return oyStringCopy_( lib_name, oyAllocateFunc_ );

    if(tmp)
    {
      cmm = oyAllocateFunc_(5);
      memcpy(cmm, tmp-4, 4);
      cmm[4] = 0;
    }
  }

  return cmm;
}

/** @internal
 *  @brief compare a library name with a Oyranos CMM
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/08 (Oyranos: 0.1.9)
 *  @date    2008/12/08
 */
int              oyCMMlibMatchesCMM  ( const char        * lib_name,
                                       const char        * cmm )
{
  int matches = 0;
  char * tmp = oyCMMnameFromLibName_( lib_name );

  if(oyStrcmp_(tmp, cmm) == 0)
    matches = 1;

  oyFree_m_( tmp );

  return matches;
}

/** @internal
 *  @brief get all module names
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
char **          oyCMMsGetNames_     ( int               * n,
                                       const char        * sub_path,
                                       const char        * ext,
                                       const char        * required_cmm )
{
  int error = !n;
  char ** files = 0,
       ** sub_paths = 0;
  int sub_paths_n = 0;

  if(!error)
  {
    int  files_n = 0, i;
    char * lib_string = oyAllocateFunc_(24);
    const char * cmm = 0;

    if(required_cmm)
    {
      /* assuming a library file can not be smaller than the signature + 
         OY_MODULE_NAME + file extension */
      if(oyStrlen_(required_cmm) < 5)
        cmm = required_cmm;

      if(cmm)
        oySprintf_( lib_string, "%s%s", cmm, OY_MODULE_NAME );
      else
        oyStringListAddStaticString_( &files, &files_n, required_cmm,
                                       oyAllocateFunc_, oyDeAllocateFunc_ );

    } else
      oySprintf_( lib_string, "%s", OY_MODULE_NAME );

    sub_paths = oyStringSplit_( sub_path, ':', &sub_paths_n, 0 );

    /* search for a matching module file */
    for(i = 0; i < sub_paths_n; ++i)
    {
      if(!files)
        files = oyLibFilesGet_( &files_n, sub_paths[i], oyUSER_SYS,
                                0, lib_string, 0, oyAllocateFunc_ );
    }
    error = !files;

    *n = files_n;

    if(sub_paths_n && sub_paths)
      oyStringListRelease_( &sub_paths, sub_paths_n, oyDeAllocateFunc_ );

    if( lib_string )
      oyFree_m_(lib_string);
  }

  return files;
}

/** @internal
 *  @brief get all module names
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/16
 */
char **          oyCMMsGetLibNames_  ( int               * n,
                                       const char        * required_cmm )
{
  return oyCMMsGetNames_(n, OY_METASUBPATH, 0, required_cmm);
}

/** @internal
 *  @brief pick from cache or ldopen a CMM and get the Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
oyCMMInfo_s *    oyCMMOpen_          ( const char        * lib_name )
{
  oyCMMInfo_s * cmm_info = 0;
  oyCMMapi_s * api = 0;
  oyCMMhandle_s * cmm_handle = 0;
  int error = !lib_name;
  char * cmm = oyCMMnameFromLibName_(lib_name);

  if(!error)
  {
    oyPointer dso_handle = 0;

    if(!error)
    {
      if(lib_name)
        dso_handle = oyCMMdsoGet_(cmm, lib_name);

      error = !dso_handle;

      if(error)
        WARNc_S(dlerror());
    }

    /* open the module */
    if(!error)
    {
      char * info_sym = oyAllocateFunc_(24);
      int api_found = 0;

      oySprintf_( info_sym, "%s%s", cmm, OY_MODULE_NAME );

      cmm_info = (oyCMMInfo_s*) dlsym (dso_handle, info_sym);

      if(info_sym)
        oyFree_m_(info_sym);

      error = !cmm_info;

      if(error)
        WARNc_S(dlerror());

      if(!error)
        if(oyCMMapi_Check_( cmm_info->api ))
          api = cmm_info->api;

      if(!error && api)
      {
        error = api->oyCMMMessageFuncSet( oyMessageFunc_p );

        cmm_handle = oyCMMhandle_New_(0);

        /* init */
        if(!error)
        error = api->oyCMMInit();
        if(!error)
          error = oyCMMhandle_Set_( cmm_handle, cmm_info, dso_handle, lib_name);

        api_found = 1;
      }

      /* store */
      if(!error && api_found)
        oyStructList_MoveIn(oy_cmm_infos_, (oyStruct_s**)&cmm_handle, -1);
    }

    oyCMMdsoRelease_( lib_name );
  }

  if(cmm)
    oyDeAllocateFunc_(cmm);
  cmm = 0;

  return cmm_info;
}

/** @internal
 *  @brief pick from cache or ldopen a CMM and get the Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
oyCMMInfo_s *    oyCMMInfoFromLibName_(const char        * lib_name )
{
  oyCMMInfo_s * cmm_info = 0;
  oyCMMhandle_s * cmm_handle = 0;
  int error = !lib_name;
  int found = 0;

  if(!error)
  {
    cmm_handle = oyCMMFromCache_( lib_name );
    if(cmm_handle && cmm_handle->info->api)
    {
      cmm_info = cmm_handle->info;
      found = 1;
    }
  }

  if(!error && !found)
  {
    cmm_info = oyCMMOpen_(lib_name);
  }

  return cmm_info;
}

/** @internal
 */
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info )
{
  char * text = 0, num[48];
  oyCMMapi_s * tmp = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  oyOBJECT_e type = 0;

  if(!cmm_info || cmm_info->type != oyOBJECT_CMM_INFO_S)
    return oyStringCopy_("---", oyAllocateFunc_);

  oySprintf_(num,"%d", cmm_info->oy_compatibility );

  STRING_ADD( text, cmm_info->cmm );
  STRING_ADD( text, " " );
  STRING_ADD( text, cmm_info->backend_version );
  STRING_ADD( text, "/" );
  STRING_ADD( text, num );
  STRING_ADD( text, ":" );

#define CMMINFO_ADD_NAME_TO_TEXT( name_, select ) \
  STRING_ADD( text, "\n  " ); \
  STRING_ADD( text, name_ ); \
  STRING_ADD( text, ":\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_NICK ) ); \
  STRING_ADD( text, "\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_NAME ) ); \
  STRING_ADD( text, "\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_DESCRIPTION ) ); \
  STRING_ADD( text, "\n" );

  CMMINFO_ADD_NAME_TO_TEXT( _("Name"), "name" )
  CMMINFO_ADD_NAME_TO_TEXT( _("Manufacturer"), "manufacturer" )
  CMMINFO_ADD_NAME_TO_TEXT( _("Copyright"), "copyright" )

      if(cmm_info)
      {
        tmp = cmm_info->api;
        STRING_ADD( text, "\n  API(s): " );

        while(tmp)
        {
          type = oyCMMapi_Check_(tmp);

          oySprintf_(num," %d:", type );
          STRING_ADD( text, num );
          STRING_ADD( text, oyStructTypeToText( tmp->type ) );

          if(type == oyOBJECT_CMM_API4_S)
          {
            cmm_api4 = (oyCMMapi4_s*) tmp;
            STRING_ADD( text, "\n    Registration: " );
            STRING_ADD( text, cmm_api4->registration );
            CMMINFO_ADD_NAME_TO_TEXT( "Name", "name" )
          }

          tmp = tmp->next;
        }
      }
  STRING_ADD( text, "\n" );

  return text;
}

/** @internal
 *  Function oyCMMCanHandle_
 *  @brief query a module for certain capabilities
 *
 *  @return                           sum of asked capabilities
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/03 (API 0.1.8)
 */
int              oyCMMCanHandle_    ( oyCMMapi_s         * api,
                                      oyCMMapiQueries_s  * queries )
{
  int capable = 0;
  int error = !api;
  int i, ret = 0;

  if(!error)
  {
    if(queries)
    {
      for(i = 0; i < queries->n; ++i)
      {
        oyCMMapiQuery_s * query = queries->queries[i];
        ret = api->oyCMMCanHandle( query->query, query->value );

        if(ret)
          capable += query->request;
        else if(query->request == oyREQUEST_HARD)
          return 0;
      }

    } else
      capable = 1;
  }

  return capable;
}

/** @internal
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
typedef struct {
  oyOBJECT_e type;
  const char        * registration;
} oyRegistrationData_s;

/** @internal
 *  Function oyCMMapi_selectFilter_
 *  @brief   filter the desired api
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyOBJECT_e   oyCMMapi_selectFilter_  ( oyCMMapi_s        * api,
                                       oyPointer           data )
{
  oyOBJECT_e type = oyOBJECT_NONE;
  oyCMMapiFilter_s * cmm_api = (oyCMMapiFilter_s *) api;
  int error = !data || !api;
  oyRegistrationData_s * reg_filter;
  int found = 0;

  if(!error)
    reg_filter = (oyRegistrationData_s*) data;

  if(!error &&
     api->type == reg_filter->type)
  {
    if(reg_filter->registration)
    {
      if(oyFilterRegistrationMatch( cmm_api->registration,
                                    reg_filter->registration, api->type ))
        found = 1;
    } else
      found = 1;

    if( found )
      type = api->type;
  }

  return type;
}

oyStructList_s * oy_backend_cache_ = 0;

/** @internal
 *  Function oyCMMGetMetaApi_
 *  @brief   get a filter module
 *
 *  This function searches for a modul/CMM API.
 *
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   queries               search for a match to capabilities
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  filter api type
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyCMMapi5_s *oyCMMGetMetaApi_        ( const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       const char        * registration )
{
  oyCMMapi5_s * s = 0;
  int error = 0;
  oyHash_s * entry = 0;
  char * class = 0;
  oyRegistrationData_s reg_filter = {0,0};
  char * lib_name = 0;
  char * api_reg = 0;
  oyOBJECT_e type = oyOBJECT_CMM_API5_S;

  class = oyFilterRegistrationToText( registration, oyFILTER_REG_TYPE,0);

  {
    if(!oy_backend_cache_)
      oy_backend_cache_ = oyStructList_New( 0 );

    entry = oyCacheListGetEntry_ ( oy_backend_cache_, class );

    s = (oyCMMapi5_s*) oyHash_GetPointer_( entry, type );

    if(s)
    {
      oyHash_Release_( &entry );
      return s;
    }
  }

  api_reg = oyStringCopy_("//", oyAllocateFunc_ );
  STRING_ADD( api_reg, class );
  oyFree_m_( class );

  reg_filter.type = type;
  reg_filter.registration = api_reg;

  s = (oyCMMapi5_s*)     oyCMMsGetApi_( type,
                                        cmm_required,
                                        queries,
                                        &lib_name,
                                        oyCMMapi_selectFilter_,
                                        &reg_filter );

  oyFree_m_( api_reg );

  if(s)
  {
    s->id_ = lib_name;
    error = oyHash_SetPointer_( entry, (oyStruct_s*) s );
  }

  oyHash_Release_( &entry );

  return s;
}


/** @internal
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyOBJECT_e   oyCMMapi_CheckWrap_     ( oyCMMapi_s        * api,
                                       oyPointer           data )
{
  return oyCMMapi_Check_( api );
}

/** @internal
 *  Function oyCMMsGetApi__
 *  @brief get a specified module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *
 *  This function allowes to obtain a desired API from a certain library.
 *
 *  @param[in]   type                  the API to return
 *  @param[in]   lib_name              if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   apiCheck              custom api selector
 *  @param[in]   check_pointer         data to pass to apiCheck
 *  @param[in]   num                   position in api chain matching to type and apiCheck/check_pointer starting from zero, -1 means: pick the first match, useful in case the API position is known or to iterate through all matching API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 *  @date    2008/12/23
 */
oyCMMapi_s *     oyCMMsGetApi__      ( oyOBJECT_e          type,
                                       const char        * lib_name,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer,
                                       int                 num )
{
  int error = !type,
      i = 0;
  oyCMMapi_s * api = 0;

  if(!error &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(!error)
  {
    /* open the module */
    oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_( lib_name );

    if(cmm_info)
    {
      oyCMMapi_s * tmp = cmm_info->api;

      if(!apiCheck)
        apiCheck = oyCMMapi_CheckWrap_;

      while(tmp)
      {
        if(apiCheck(tmp, check_pointer) == type)
        {
          if((num >= 0 && num == i) ||
             num < 0 )
            api = tmp;

          ++i;
        }
        tmp = tmp->next;
      }
    }
  }

  return api;
}

/** @internal
 *  Function oyCMMapi4_selectFilter_
 *  @brief   filter the desired api
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyOBJECT_e   oyCMMapi4_selectFilter_ ( oyCMMapi_s        * api,
                                       oyPointer           data )
{
  oyOBJECT_e type = oyOBJECT_NONE,
             searched = oyOBJECT_CMM_API4_S;
  int error = !data || !api;
  oyRegistrationData_s * reg_filter;
  oyCMMapi4_s * cmm_api = (oyCMMapi4_s *) api;
  int found = 0;

  if(!error)
    reg_filter = (oyRegistrationData_s*) data;

  if(!error &&
     api->type == searched &&
     reg_filter->type == searched)
  {
    if(reg_filter->registration)
    {
      if(oyFilterRegistrationMatch( cmm_api->registration,
                                    reg_filter->registration, api->type ))
        found = 1;
    } else
      found = 1;

    if( found )
      type = api->type;
  }

  return type;
}


/** @internal
 *  Function oyCMMsGetFilterApis_
 *  @brief let a oyCMMapi5_s meta backend open a set of modules
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which makes the search for alternatives desireable. So we search
 *  for match to our automatic criteria.\n
 *
 *
 *  This function allowes to obtain API's for matching modules/CMM's.
 *
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   queries               search for a match to capabilities
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  CMM API
 *  @param[out]  rank_list             the ranks matching the returned list;
 *                                     without that only the most matching API  
 *                                     is returned at position 0
 *  @param[out]  count                 count of returned backends
 *  @return                            a zero terminated list of backends
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/19 (Oyranos: 0.1.10)
 *  @date    2008/12/19
 */
oyCMMapiFilter_s**oyCMMsGetFilterApis_(const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       const char        * registration,
                                       oyOBJECT_e          type,
                                       int              ** rank_list,
                                       int               * count )
{
  int error = type != oyOBJECT_CMM_API4_S &&
              type != oyOBJECT_CMM_API6_S &&
              type != oyOBJECT_CMM_API7_S &&
              type != oyOBJECT_CMM_API8_S;
  char prefered_cmm[5] = {0,0,0,0,0};
  oyCMMapiFilter_s ** api = 0;
  int * rank_list_ = 0;

  if(!error && cmm_required)
  {
    if(queries && *(uint32_t*)queries->prefered_cmm)
      error = !memcpy( prefered_cmm, queries->prefered_cmm, 4 );
    else
      error = !memcpy( prefered_cmm, oyModuleGetActual( registration ), 4 );
  }

  /*{
    if(!oy_backend_cache_)
      oy_backend_cache_ = oyStructList_New( 0 );

    entry = oyCacheListGetEntry_ ( oy_backend_cache_, registration );

    api = (oyCMMapiFilter_s*) oyHash_GetPointer_( entry, type );

    if(api)
    {
      oyHash_Release_( &entry );
      return api;
    }
  }*/

  if(!error)
  {
    oyCMMapi5_s * api5 = oyCMMGetMetaApi_( 0, queries, registration );
    char ** files = 0;
    int  files_n = 0;
    int i, j, k = 0, match_j = -1, ret, match_i = -1, rank = 0, old_rank = 0;
    char * match = 0, * reg = 0;
    oyCMMInfo_s * info = 0;
    oyObject_s object = oyObject_New();

    error = !api5;

    if(!error)
    files = oyCMMsGetNames_(&files_n, api5->sub_paths, api5->ext, cmm_required);
    else
      WARNc2_S("%s: %s", _("Could not open meta backend API"),
               oyNoEmptyString_m_( registration ));

    for( i = 0; i < files_n; ++i)
    {
      ret = 0; j = 0;
      ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                   &reg, 0, oyAllocateFunc_, &info, object );
      while(!ret)
      {
        ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                     &reg, 0, oyAllocateFunc_, 0, 0 );
        if(!ret && reg)
        {
          rank = oyFilterRegistrationMatch( reg, registration, type );
          if(rank && rank_list)
          {

            if(!rank_list_ && !api)
            {                                //  TODO @todo error
              oyAllocHelper_m_( *rank_list, int, files_n+1, 0, return 0 );
              oyAllocHelper_m_( api, oyCMMapiFilter_s*, files_n+1, 0, return 0);
              rank_list_ = *rank_list;
            }

            rank_list_[k] = rank;
            api[k] = api5->oyCMMFilterLoad( 0,0, files[i], type, j);
            api[k]->id_ = oyStringCopy_( files[i], oyAllocateFunc_ );
            api[k]->api5_ = api5;
            if(count)
              ++ *count;

          } else
          if(rank > old_rank)
          {
            match = reg;
            match_j = j;
            match_i = i;
            old_rank = rank;
          } else
            oyFree_m_( reg );
        }
        ++j;
      }
    }

    if(match && !rank_list)
    {
      oyAllocHelper_m_( api, oyCMMapiFilter_s*, 2, 0, return 0);
      api[0] = api5->oyCMMFilterLoad( 0,0, files[match_i], type, match_j );
      api[0]->id_ = oyStringCopy_( files[match_i], oyAllocateFunc_ );
      api[0]->api5_ = api5;
      if(count)
        *count = 1;
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );

  }

  /*if(api)
  {
    api->id_ = lib_name;
    error = oyHash_SetPointer_( entry, (oyStruct_s*) s );
  }*/

  return api;
}

/** @internal
 *  Function oyCMMsGetFilterApi_
 *  @brief let a oyCMMapi5_s meta backend open a module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which makes the search for alternatives desireable. So we search
 *  for match to our automatic criteria.\n
 *
 *
 *  This function allowes to obtain a API for a matching modul/CMM.
 *
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   queries               search for a match to capabilities
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  CMM API
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/15 (Oyranos: 0.1.9)
 *  @date    2008/12/15
 */
oyCMMapiFilter_s *oyCMMsGetFilterApi_( const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       const char        * registration,
                                       oyOBJECT_e          type )
{
  oyCMMapiFilter_s * api = 0;
  oyCMMapiFilter_s ** apis = 0;

  apis = oyCMMsGetFilterApis_( cmm_required, queries, registration, type, 0,0 );

  if(apis)
  {
    api = apis[0];
    oyFree_m_( apis );
  }

  return api;
}


/** @internal
 *  Function oyCMMsGetApi_
 *  @brief get a module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which make it desireable. So we have to search for match to our automatic
 *  criteria.\n
 *
 *
 *  This function allowes to obtain a API for a certain modul/CMM.
 *  oyCMMapi4_s is excluded.
 *
 *  @param[in]   type                  the API to return
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   queries               search for a match to capabilities
 *  @param[out]  lib_used              inform about the selected CMM
 *  @param[in]   apiCheck              custom api selector
 *  @param[in]   check_pointer         data to pass to apiCheck
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/12 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       char             ** lib_used,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer )
{
  int error = !type;
  oyCMMapi_s * api = 0,
             * api_fallback = 0;
  char * lib_fallback = 0,
       prefered_cmm[5] = {0,0,0,0,0};
  int  found = 0;

  if(!apiCheck)
    apiCheck = oyCMMapi_CheckWrap_;

  if(!cmm_required)
    if(queries && *(uint32_t*)queries->prefered_cmm)
      error = !memcpy( prefered_cmm, queries->prefered_cmm, 4 );

  if(!error &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(!error)
  {
    char ** files = 0;
    int  files_n = 0;
    int i, oy_compatibility = 0;
    char cmm[5] = {0,0,0,0,0};

    files = oyCMMsGetLibNames_(&files_n, cmm_required);

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api;

        error = !memcpy( cmm, cmm_info->cmm, 4 );

        while(tmp)
        {
          found = 0;

          if(apiCheck(tmp, check_pointer) == type)
          {

              if( /* if we found already a matching version, do not exchange*/
                  oy_compatibility != OYRANOS_VERSION &&
                    /* possibly newly found */
                  ( oy_compatibility = 0 ||
                    /* or a bigger version but not greater than current oy_ */
                    ( cmm_info->oy_compatibility <= OYRANOS_VERSION &&
                      oy_compatibility < cmm_info->oy_compatibility ) ||
                    /* or we select a less greater in case we are above oy_ */
                    ( cmm_info->oy_compatibility > OYRANOS_VERSION &&
                      oy_compatibility > cmm_info->oy_compatibility )
                  )
                )
              {
                if(memcmp( cmm, prefered_cmm, 4 ) == 0)
                {
                  api = tmp;
                  if(lib_used)
                    *lib_used = oyStringCopy_( files[i], oyAllocateFunc_ );

                } else {

                  api_fallback = tmp;
                  lib_fallback = oyStringCopy_( files[i], oyAllocateFunc_ );
                }
                oy_compatibility = cmm_info->oy_compatibility;
              }
          }
          tmp = tmp->next;
        }
      }
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  if(!api)
  {
    if(lib_used)
      *lib_used = oyStringCopy_( lib_fallback, oyAllocateFunc_ );

    if(lib_fallback)
      oyFree_m_( lib_fallback );

    return api_fallback;
  }

  return api;
}


/** @internal
 *  @brief release Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
int              oyCMMRelease_       ( const char        * cmm )
{
  int error = !cmm;
  int n = oyStructList_Count( oy_cmm_infos_ );
  int i;

  n = oyStructList_Count(oy_cmm_infos_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyCMMInfo_s * s = 0;
    oyCMMhandle_s * cmmh = (oyCMMhandle_s *) oyStructList_GetType_(
                                oy_cmm_infos_, i, oyOBJECT_CMM_HANDLE_S );

    if(cmmh)
      s = (oyCMMInfo_s*) cmmh->info;

    if( s && s->type == oyOBJECT_CMM_INFO_S &&
        *(uint32_t*)&s->cmm && cmm &&
        !memcmp( s->cmm, cmm, 4 ) )
    {
      oyCMMhandle_Release_( &cmmh );
      oyStructList_ReleaseAt( oy_cmm_infos_, 0 );
    }
  }

  return error;
}




/** @internal
 *  @memberof oyCMMapi_s
 *  @brief   check for completeness
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyOBJECT_e       oyCMMapi_Check_     ( oyCMMapi_s        * api )
{
  int error = !api;
  oyOBJECT_e type = 0;

  if(!error)
    type = api->type;

  if(!error)
  switch(type)
  {
    case oyOBJECT_CMM_API1_S:
    {
      oyCMMapi1_s * s = (oyCMMapi1_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyCMMCanHandle &&
           s->oyCMMDataOpen &&
           /*s->oyCMMProfile_GetText &&
           s->oyCMMProfile_GetSignature &&*/
           s->oyCMMColourConversion_Create &&
           s->oyCMMColourConversion_FromMem &&
           s->oyCMMColourConversion_ToMem &&
           s->oyCMMColourConversion_Run ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API2_S:
    {
      oyCMMapi2_s * s = (oyCMMapi2_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyGetMonitorInfo &&
           s->oyGetScreenFromPosition &&
           s->oyGetDisplayNameFromPosition &&
           s->oyGetMonitorProfile &&
           s->oyGetMonitorProfileName &&
           s->oySetMonitorProfile &&
           s->oyActivateMonitorProfiles ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API3_S:
    {
      oyCMMapi3_s * s = (oyCMMapi3_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyCMMCanHandle &&
           s->oyCMMProfileTag_GetValues &&
           /*s-> &&*/
           s->oyCMMProfileTag_Create ) )
        error = 1;
    } break;
    default: break;
  }

  return type;
}

/** @internal
 *  @brief get a CMM specific pointer
 *  @memberof oyStruct_s
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/11/05
 */
oyCMMptr_s * oyStruct_GetCMMPtr_      ( oyStruct_s      * data,
                                        const char      * cmm )
{
  oyStruct_s * s = data;
  int error = !s;
  oyCMMptr_s * cmm_ptr = 0;

  if(!error && !cmm)
  {
    cmm = oyModuleGetActual("//colour");
    error = !cmm;
  }

  if(!error)
  {
    const char * tmp = 0;
    char * lib_used = 0;
 
    oyHash_s * entry = 0;
    oyChar * hash_text = 0;

    /** Cache Search
     *  1.     hash from input
     *  2.     query for hash in cache
     *  3.     check
     *  3a.       eighter take cache entry
     *  3b.       or ask CMM
     *  3b.1.                update cache entry
     */

    /* 1. create hash text */
    hashTextAdd_m( cmm );
    hashTextAdd_m( " oyPR:" );
    tmp = oyObject_GetName( s->oy_, oyNAME_NICK );
    hashTextAdd_m( tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );
    if(s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );

    if(!error)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( entry,
                                                  oyOBJECT_CMM_POINTER_S);

      if(!cmm_ptr)
      {
        /* 3b. ask CMM */
        oyCMMDataOpen_f funcP = 0;

        /* TODO update to oyCMMapi4_s::oyCMMDataOpen_f */
        oyCMMapi1_s * api1 = (oyCMMapi1_s*) oyCMMsGetApi_( oyOBJECT_CMM_API1_S,
                                                       cmm, 0, &lib_used, 0,0 );
        if(api1 && *(uint32_t*)&cmm)
          funcP = api1->oyCMMDataOpen;

        if(funcP)
        {
          cmm_ptr = oyCMMptr_New_(s->oy_->allocateFunc_);
          error = !cmm_ptr;

          if(!error)
            error = oyCMMptr_Set_( cmm_ptr, lib_used,
                                   oyCMM_PROFILE, 0, 0, 0 );

          if(!error)
          {
            error = funcP( s, cmm_ptr );

#if 0
            /* We have currently no means to trace all the spread resources. */
            if(!error)
              error = oyCMMRelease_( cmm );
#endif
          }
        }

        if(lib_used)
          oyFree_m_( lib_used );

        error = !cmm_ptr;

        if(!error && cmm_ptr && cmm_ptr->ptr)
          /* 3b.1. update cache entry */
          error = oyHash_SetPointer_( entry,
                                      (oyStruct_s*) oyCMMptr_Copy_(cmm_ptr, 0) );
      }
    }


    oyHash_Release_( &entry );
  }

  return cmm_ptr;
}

/** @internal
 *  @memberof oyStructList_s
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/11/23 (Oyranos: 0.1.8)
 *  @date    2008/11/05
 */
oyCMMptr_s** oyStructList_GetCMMptrs_( oyStructList_s    * list,
                                       const char        * lib_name )
{
  oyCMMptr_s ** obj = 0;
  int n = oyStructList_Count( list );


  if(list && n)
  {
    int i = 0;

    if(n)
      obj = list->oy_->allocateFunc_( sizeof(oyCMMptr_s*) * n); 

    for(i = 0; i < n; ++i)
    {
      oyStruct_s * o = oyStructList_Get_( list, i );

      if(o)
      {
        oyCMMptr_s * cmm_ptr = oyStruct_GetCMMPtr_(o, lib_name);

        if(cmm_ptr && cmm_ptr->type == oyOBJECT_CMM_POINTER_S)
          obj[i] = oyCMMptr_Copy_( cmm_ptr, 0 );
      }
    }
  }

  return obj;
}

/** @} *//* cmm_handling */



/** \addtogroup backend_api CMM API
 *
 *  CMM's in Oyranos are designed to be plugable into a framework of arbitrary
 *  data formats and processing.
 *  @dot
digraph Backends {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=14];
  node [shape=record,fontname=Helvetica, fontsize=11, width=.1];

  subgraph cluster_7 {
    label="Oyranos Module Framework";
    color=white;
    clusterrank=global;

      s [ label="Script Importer - \"oCTL\"\n oyCMMapi5_s"];
      i [ label="Library Importer - \"oyIM\"\n oyCMMapi5_s"];

      node [width = 2.5, style=filled];
      o [ label="External Function Import\n Extendable Functionality\n Additional Dependencies\n ..."];
      p [ label="Meta Backend / Filter Import\n oyFilterNode_s"];

      api7_B [label="//image/root.oyra Processor\n oyCMMapi7_s"];

      api4_A [label="//colour/icc.lcms Context\n oyCMMapi4_s"];
      api6_A [label="//colour/icc.lcms Context Converter\n oyCMMapi6_s"];
      api7_A [label="//colour/icc.lcms Processor\n oyCMMapi7_s"];

      api6_C [label="//colour/icc.octl Context Converter\n oyCMMapi6_s"];
      api7_C [label="//colour/icc.octl Processor\n oyCMMapi7_s"];

      m [label="Config (Device) Functions\n oyCMMapi8_s"];
      icc [label="ICC Profile Functions\n oyCMMapi3_s"];

      subgraph cluster_6 {
        color=gray;
        label="Backends"

        subgraph cluster_0 {
          rank=max;
          color=red;
         style=dashed;
          node [style="filled"];
          api4_A; api6_A; api7_A;
          //api4_A -> api6_A -> api7_A [color=white, arrowhead=none, dirtype=none];
          label="\"lcms\"";
        }

        subgraph cluster_1 {
          color=blue;
          style=dashed;
          node [style="filled"];
          api7_B;
          icc;
          label="\"oyra\"";
        }

        subgraph cluster_2 {
          color=yellow;
          style=dashed;
          node [style="filled"];
          api6_C;
          api7_C;
          label="\"octl\"";
        }

        subgraph cluster_3 {
          color=gray;
          style=dashed;
          m;
          label="\"oyX1\"";
        }

      }

      subgraph cluster_4 {
        color=gray;
        node [style="filled"];
        s;
        i;
        label="Meta Backends - Filter Importing";
      }

      subgraph cluster_5 {
        color=gray;
        node [style="filled"];
        o;
        p;
        label="Oyranos - Abstract Graph/Filter API";
      }

      p -> i [arrowhead="open", color=gray];
      p -> s [arrowhead="open", color=gray];
      i -> api4_A [arrowhead="open", color=red];
      i -> api6_A [arrowhead="open", color=red];
      i -> api7_A [arrowhead="open", color=red];
      i -> api7_B [arrowhead="open", color=blue];
      s -> api6_C [arrowhead="open", color=yellow];
      s -> api7_C [arrowhead="open", color=yellow];
      o -> m [arrowhead="open", color=gray];
      o -> icc [arrowhead="open", color=gray];
  }
} @enddot
 *
 *  The Filter API's are subdivided to allow for automaticaly combining of 
 *  preprocessing and processing stages. Especially in the case of expensive
 *  preprocessing data, like in CMM's, it makes sense to provide the means for
 *  combining general purpose libraries with hardware accelerated modules.
 *  This architecture allowes for combining by just providing enough interface
 *  information about their supported data formats.
 *  The following paragraphs provide a overview.
 *
 *  The oyCMMapi5_s plug-in structure defines a meta backend to load plug-ins,
 *  from a to be defined directory with to be defined naming criterias. This
 *  API defines at the same time allowed input data formats. The meta backend 
 *  loads or constructs all parts of a plug-in, oyCMMapi4_s, oyCMMapi7_s, 
 *  oyCMMapi6_s and oyCMMapi8_s.
 *
 *  oyCMMapi7_s eighter deploys the context created in a oyCMMapi4_s filter, or
 *  simply processes the Oyranos oyFilterNode_s graph element. It is responsible
 *  to request data from the graph and process them.
 *  Members are responsible to describe the filters capabilities for connecting
 *  to other filters in the graph. Processors without context should describe
 *  their own UI. Processors with context must delegate this to oyCMMapi4_s.
 *  oyCMMapi7_s is mandatory.
 *
 *  The oyCMMapi4_s is a structure to create a context for a oyCMMapi7_s
 *  processor. This context is a intermediate processing stage for all of the
 *  context data influencing options and input datas. This structure 
 *  contains as well the GUI. oyCMMapi4_s is mandatory because of its GUI parts.
 *  A oyCMMapi4_s without a oyCMMapi7_s is useless.
 *  oyCMMapi4_s must contain the same basic registration string like the 
 *  according oyCMMapi7_s except some keywords in the application section. This
 *  is explained more below in detail.
 *  It is assumed that the generated context is worth to be cached. Oyranos 
 *  requires therefore a serialised data blob from the context genarator.
 *
 *  In case a oyCMMapi7_s function can not handle a certain provided context
 *  data format, Oyranos will try to convert it for the oyCMMapi7_s API through
 *  a fitting oyCMMapi6_s data convertor. This API is only required for filters,
 *  which request incompatible contexts from a oyCMMapi4_s structure.
 *
 *  The oyCMMapi8_s provides a general interface to backends to export data,
 *  like additional filter input data and options.
 *
 *  @dot
digraph Anatomy_A {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=14];
  node [shape=record,fontname=Helvetica, fontsize=11, width=.1];

  subgraph cluster_7 {
    label="Different Filter Processors - Same Context + UI";
    color=white;
    clusterrank=global;

      n [label="Filter Node A\n oyFilterNode_s" ];
      n2 [label="Filter Node B\n oyFilterNode_s" ];

      node [width = 2.5, style=filled];

      api4_A [label="//colour/icc.lcms\n oyCMMapi4_s | <f>Context Creation \"oyDL\" | <o>Options | <ui>XFORMS GUI"];
      api6_A [label="//colour/icc.lcms Context Converter\n oyCMMapi6_s\n oyDL_lcCC"];
      api7_A [label="//colour/icc.lcms Processor\n oyCMMapi7_s"];

      api6_C [label="//colour/icc.octl Context Converter\n oyCMMapi6_s\n oyDL_oCTL"];
      api7_C [label="//colour/icc.octl Processor\n oyCMMapi7_s"];

      subgraph cluster_0 {
        rank=max;
        color=white;
        style=dashed;
        api4_A; api6_A; api7_A; n;
        label="";
      }
      subgraph cluster_1 {
        rank=max;
        color=white;
        style=dashed;
        api7_C; api6_C; n2;
        label="";
      }

      api4_A:f -> api6_A -> api7_A [arrowhead="open", color=black];
      api4_A:f -> api6_C -> api7_C [arrowhead="open", color=black];
      api4_A:o -> n [arrowhead="open", color=black];
      api4_A:ui -> n [arrowhead="open", color=black];
      api4_A:o -> n2 [arrowhead="open", color=black];
      api4_A:ui -> n2 [arrowhead="open", color=black];
  }
} @enddot
 *  @dot
digraph Anatomy_B {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=14];
  node [shape=record,fontname=Helvetica, fontsize=11, width=.1];

  subgraph cluster_7 {
    label="UI + Processor";
    color=white;
    clusterrank=global;

      n [label="Filter Node\n oyFilterNode_s" ];

      node [width = 2.5, style=filled];

      api4_A [label="//image/root.oyra\n oyCMMapi4_s | <o>Options | <ui>XFORMS GUI"];
      api7_A [label="//image/root.oyra Processor\n oyCMMapi7_s"];

      subgraph cluster_0 {
        rank=max;
        color=white;
        style=dashed;
        n;
        label="";

        subgraph cluster_1 {
          color=gray;
          style=dashed;
          node [style="filled"];
          api4_A; api7_A;
          label="";
        }
      }

      api4_A:o -> n [arrowhead="open", color=black];
      api4_A:ui -> n [arrowhead="open", color=black];
  }
} @enddot
 *
 *  Each filter API provides a \b registration member string.
 *  The registration member provides the means to later sucessfully select 
 *  the according filter. The string is separated into sections by a slash'/'.
 *  The sections can be subdivided by point'.' for additional attributes as 
 *  needed. The sections are to be filled as follows:
 *  - top, e.g. "sw"
 *  - vendor, e.g. "oyranos.org"
 *  - filter type, e.g. "colour" or "tonemap" or "image" or "imaging"
 *  - filter name, e.g. "icc.lcms.NOACCEL.CPU"
 *
 *  The application registration string part should for general purpose modules
 *  contain a convention string. "icc" signals to process colours with the help
 *  of ICC style profiles, which can by convention be inserted into the options
 *  list.
 *
 *  @par Filter registration:
 *  A filter can add keywords but must omit the API number and the following
 *  matching rule sign. Recommended keywords for the application section are:
 *  - ACCEL for acceleration, required
 *  - NOACCEL for no acceleration or plain software, required
 *  - GPU, GLSL, HLSL, MMX, SSE, SSE2, 3DNow and so on for certain hardware
 *    acceleration features \n
 *  
 *  \b Example: a complete module registration: \n
 *  "sw/oyranos.org/colour/icc.lcms.NOACCEL.CPU" registers a plain software CMM
 * 
 *  @par Registration search pattern:
 *  To explicitely select a different processor and context creator the
 *  according registration attribute must have a number and prefix,
 *  e.g. "4_lcms" "7_octl".
 *  A search pattern can add keywords.
 *  - a number followed by underscore, plus or minus signs the according API.
 *    The feature of interesst must then be appended, e.g. "7_GPU" preferes
 *    a GPU interpolator. This is useful to select a certain API of a module.
 *    - underscore '_' means preference
 *    - minus '-' means must skip
 *    - plus '+' means must have
 *  - "4[_,+,-]" - context+UI oyCMMapi4_s
 *  - "6[_,+,-]" - context convertor oyCMMapi6_s
 *  - "7[_,+,-]" - processor oyCMMapi7_s \n
 *  
 *  \b Example: a complete registration search pattern: \n
 *  "//colour/4+icc.7+ACCEL.7_GPU.7_HLSL.7-GLSL" selects a accelerated CMM 
 *  interpolator with prefered GPU and HLSL but no GLSL support together with a
 *  ICC compliant context generator and options.
 *
 *  The oyFilterRegistrationToText() and oyFilterRegistrationMatch() functions
 *  might be useful for canonical processing Oyranos registration text strings.
 *  Many functions allow for passing a registration string. Matching can be 
 *  obtained by omitting sections like in the string "//colour/icc". This string
 *  would result in a match for any ICC compliant colour conversion filter.
 *
 *  See as well <a href="http://www.oyranos.org/wiki/index.php?title=Concepts#Elektra_namespace">Concepts#Elektra_namespace</a> on ColourWiki.
 *  @{
 */


/** Function oyCMMptr_LookUp
 *  @brief   get a CMM specific pointer
 *  @memberof oyCMMptr_s
 *
 *  The returned oyCMMptr_s has to be released after using by the backend with
 *  oyCMMptr_Release().
 *  In case the the oyCMMptr_s::ptr member is empty, it should be set by the
 *  requesting backend.
 *
 *  @see e.g. lcmsCMMData_Open()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyCMMptr_s * oyCMMptr_LookUp          ( oyStruct_s      * data,
                                        const char      * data_type )
{
  oyStruct_s * s = data;
  int error = !s;
  oyCMMptr_s * cmm_ptr = 0;

  if(!error && !data_type)
    error = !data_type;

  if(!error)
  {
    /*oyCMMptr_s *cmm_ptr = 0;*/
    const char * tmp = 0;
 
    oyHash_s * entry = 0;
    oyChar * hash_text = 0;

    /** Cache Search \n
     *  1.     hash from input \n
     *  2.     query for hash in cache \n
     *  3.     check \n
     *  3a.       eighter take cache entry or \n
     *  3b.       update cache entry
     */

    /* 1. create hash text */
    hashTextAdd_m( data_type );
    hashTextAdd_m( ":" );
    tmp = oyObject_GetName( s->oy_, oyNAME_NICK );
    hashTextAdd_m( tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );
    if(s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );

    if(!error)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( entry,
                                                  oyOBJECT_CMM_POINTER_S);

      if(!cmm_ptr)
      {
        cmm_ptr = oyCMMptr_New_(s->oy_->allocateFunc_);
        error = !cmm_ptr;

        if(!error)
          error = oyCMMptr_Set_( cmm_ptr, 0,
                                 data_type, 0, 0, 0 );

        error = !cmm_ptr;

        if(!error && cmm_ptr)
          /* 3b.1. update cache entry */
          error = oyHash_SetPointer_( entry,
                                     (oyStruct_s*) oyCMMptr_Copy_(cmm_ptr, 0) );
      }
    }

    oyHash_Release_( &entry );
  }

  return cmm_ptr;
}

/** @} *//* backend_api */



/** \addtogroup misc Miscellaneous

 *  @{
 */

/** \addtogroup objects_generic

 *  @{
 */


static int oy_object_id_ = 0;


/** @brief   object management 
 *  @memberof oyObject_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s
oyObject_New  ( void )
{
  oyObject_s o = 0;
  int error = 0;
  int len = sizeof(struct oyObject_s_);

  o = oyAllocateFunc_(len);

  if(!o) return 0;

  error = !memset( o, 0, sizeof(len) );
  
  o = oyObject_SetAllocators_( o, oyAllocateFunc_, oyDeAllocateFunc_ );

  o->id_ = oy_object_id_++;
  o->type_ = oyOBJECT_OBJECT_S;
  o->copy = (oyStruct_Copy_f) oyObject_Copy;
  o->release = (oyStruct_Release_f) oyObject_Release;
  o->version_ = oyVersion(0);
  ++o->ref_;

  return o;
}

/** @brief   object management 
 *  @memberof oyObject_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s
oyObject_NewWithAllocators  ( oyAlloc_f         allocateFunc,
                              oyDeAlloc_f       deallocateFunc )
{
  oyObject_s o = 0;
  int error = 0;
  int len = sizeof(struct oyObject_s_);

  o = oyAllocateWrapFunc_( len, allocateFunc );

  if(!o) return 0;

  error = !memset( o, 0, len );
  
  o = oyObject_SetAllocators_( o, allocateFunc, deallocateFunc );

  o->id_ = oy_object_id_++;
  o->type_ = oyOBJECT_OBJECT_S;
  o->copy = (oyStruct_Copy_f) oyObject_Copy;
  o->release = (oyStruct_Release_f) oyObject_Release;
  o->version_ = oyVersion(0);
  ++o->ref_;

  return o;
}

/** @internal
 *  @memberof oyObject_s
 *  @brief   object management 
 *
 *  @param[in]    object         the object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
static oyObject_s
oyObject_NewFrom_( oyObject_s      object )
{
  oyObject_s o = 0;
  int error = 0;

  if(object)
    o = oyObject_NewWithAllocators( object->allocateFunc_,
                                    object->deallocateFunc_ );
  else
    o = oyObject_New( );

  if(!o)
    error = 1;

  if(!error && object && object->name_)
    error = oyObject_SetNames( o, object->name_->nick, object->name_->name,
                              object->name_->description );

  if(!error)
    error = 1;

  return o;
}

/** @brief   object management 
 *  @memberof oyObject_s
 *
 *  @param[in]    object         the object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyObject_s
oyObject_NewFrom ( oyObject_s      object )
{
  return oyObject_NewFrom_( object );
}

/** @brief   object management 
 *  @memberof oyObject_s
 *
 *  @param[in]    object         the object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s
oyObject_Copy ( oyObject_s      object )
{
  if(!object)
    return 0;

  oyObject_Ref( object );

  return object;
}

/** @internal
 *  @memberof oyObject_s
 *  @brief   custom object memory managers
 *
 *  @param[in]    object         the object to modify
 *  @param[in]    allocateFunc   zero for default or user memory allocator
 *  @param[in]    deallocateFunc zero for default or user memory deallocator
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc )
{
  if(!object) return 0;

  /* we require a allocation function to be present */
  if(allocateFunc)
    object->allocateFunc_ = allocateFunc;
  else
    object->allocateFunc_ = oyAllocateFunc_;

  object->deallocateFunc_ = deallocateFunc;

  return object;
}

/** @brief   release an Oyranos object
 *  @memberof oyObject_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_Release         ( oyObject_s      * obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyObject_s s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( s->type_ != oyOBJECT_OBJECT_S)
  {
    WARNc_S("Attempt to release a non oyObject_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s))
    return 0;
  /* ---- end of common object destructor ------- */

  oyName_release_( &s->name_, s->deallocateFunc_ );

  s->id_ = 0;

  if(s->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->deallocateFunc_;
    oyPointer lock = s->lock_;

    s->hash_[0] = 0;

    if(s->backdoor_)
      deallocateFunc( s->backdoor_ ); s->backdoor_ = 0;

    error = oyStructList_Release(&s->handles_);

    deallocateFunc( s );
    oyLockReleaseFunc_( lock, __FILE__, __LINE__ );
  }

  return 0;
}

/**
 *  @internal
 *  Function oyObject_Ref
 *  @memberof oyObject_s
 *  @brief   increase the ref counter and return the above zero ref value
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/07
 *  @since   2008/02/07 (Oyranos: 0.1.8)
 */
int          oyObject_Ref            ( oyObject_s          obj )
{
  oyObject_s s = obj;
  int error = !s;

  if( s->type_ != oyOBJECT_OBJECT_S)
  {
    WARNc2_S("Attempt to manipulate a non oyObject_s object; type: %d ID: %d",
             s->type_, s->id_)
    return 1;
  }

  if(!error)
    oyObject_Lock( s, __FILE__, __LINE__ );

  if(!error)
    ++s->ref_;

  if(obj->parent_type_ == oyOBJECT_NAMED_COLOURS_S)
  {
    int e_a = error;
    error = pow(e_a,2.1);
    error = e_a;
  }

  if(!error)
    oyObject_UnLock( s, __FILE__, __LINE__ );

  return s->ref_;
}

/**
 *  @internal
 *  Function oyObject_UnRef
 *  @memberof oyObject_s
 *  @brief   decrease the ref counter and return the above zero ref value
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/07
 *  @since   2008/02/07 (Oyranos: 0.1.8)
 */
int          oyObject_UnRef          ( oyObject_s          obj )
{
  int ref = 0;
  oyObject_s s = obj;
  int error = !s;

  if( s->type_ != oyOBJECT_OBJECT_S)
  {
    WARNc_S("Attempt to manipulate a non oyObject_s object.")
    return 1;
  }

  if(!error)
  {
    oyObject_Lock( s, __FILE__, __LINE__ );

    if(!error && --s->ref_ > 0)
      ref = s->ref_;

    if(obj->parent_type_ == oyOBJECT_NAMED_COLOURS_S)
    {
      int e_a = error;
      error = pow(e_a,2.1);
      error = e_a;
    }

    oyObject_UnLock( s, __FILE__, __LINE__ );
  }

  return ref;
}

/** @brief   custom object memory managers
 *  @memberof oyObject_s
 *
 *  Normally a sub object should not know about its parent structure, but
 *  follow a hierarchical design. We break this here to allow the observation of
 *  all objects including the parents structures as a feature in oyObject_s.
 *  Then we just need a list of all oyObject_s objects and have an overview.
 *  This facility is intented to work even without debugging tools.
 *
 *  @param[in]    o              the object to modify
 *  @param[in]    type           the parents struct type
 *  @param[in]    parent         a pointer to the parent struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s   oyObject_SetParent       ( oyObject_s        o,
                                        oyOBJECT_e        type,
                                        oyPointer         parent )
{
  int error = 0;
  if(!error && type)
  {
    o->parent_type_ = type;
    if(parent)
      o->parent_ = parent;
  }

  return o;
}

/** @brief   object naming
 *  @memberof oyObject_s
 *
 *  @param[in]    object         the object
 *  @param[in]    text           the name to set
 *  @param[in]    type           the kind of name 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_SetName         ( oyObject_s        object,
                                        const char      * text,
                                        oyNAME_e          type )
{
  object->name_ = oyName_set_( object->name_, text, type,
                               object->allocateFunc_, object->deallocateFunc_ );
  return (text && type && object && !object->name_);
}


/** @brief   object naming
 *  @memberof oyObject_s
 *
 *  @param[in]    object         the object
 *  @param[in]    nick           short name, about 1-4 letters
 *  @param[in]    name           the name should fit into usual labels
 *  @param[in]    description    the description 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_SetNames        ( oyObject_s        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description )
{
  int error = 0;
  

  if(!object) return 1;

  if(!error)
    error = oyObject_SetName( object, nick, oyNAME_NICK );
  if(!error)
    error = oyObject_SetName( object, name, oyNAME_NAME );
  if(!error)
    error = oyObject_SetName( object, description, oyNAME_DESCRIPTION );

  return error;
}

/** Function oyObject_GetName
 *  @memberof oyObject_s
 *  @brief   object get names
 *
 *  @param[in]    obj            the object
 *  @param[in]    type           name type
 *  @return                      the text
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2007/11/00 (API 0.1.8)
 */
const oyChar * oyObject_GetName       ( const oyObject_s        obj,
                                        oyNAME_e                type )
{
  const char * text = 0;
  if(!obj)
    return 0;

  if(!obj->name_)
    return 0;

  text = oyName_get_( obj->name_, type );

  return text;
}

/** Function oyObject_Lock
 *  @memberof oyObject_s
 *  @brief   Lock a object
 *
 *  @see   oyThreadLockingSet
 *
 *  @param[in]     object              the object
 *  @param[in]     marker              debug hints, e.g. __FILE__
 *  @param[in]     line                source line number, e.g. __LINE__
 *  @return                      error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_Lock             ( oyObject_s        object,
                                         const char      * marker,
                                         int               line )
{
  int error = !object;

  if(!error)
  {
    if(!object->lock_)
      object->lock_ = oyStruct_LockCreateFunc_( object->parent_ );

    oyLockFunc_( object->lock_, marker, line );
  }

  return error;
}

/** Function oyObject_UnLock
 *  @memberof oyObject_s
 *  @brief   Unlock a object
 *
 *  @see   oyThreadLockingSet
 *
 *  @param[in]     object              the object
 *  @param[in]     marker              debug hints, e.g. __FILE__
 *  @param[in]     line                source line number, e.g. __LINE__
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_UnLock           ( oyObject_s        object,
                                         const char      * marker,
                                         int               line )
{
  int error = !object;

  if(!error)
  {
    error = !object->lock_;
    if(!error)
      oyUnLockFunc_( object->lock_, marker, line );
  }

  return error;
}

/** Function oyObject_UnSetLocking
 *  @memberof oyObject_s
 *  @brief   remove a object's lock pointer
 *
 *  @see     oyThreadLockingSet
 *
 *  @param[in]     object              the object
 *  @param[in]     marker              debug hints, e.g. __FILE__
 *  @param[in]     line                source line number, e.g. __LINE__
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_UnSetLocking   ( oyObject_s          object,
                                       const char        * marker,
                                       int                 line )
{
  int error = !object;

  if(!error)
  {
    oyLockReleaseFunc_( object->lock_, marker, line );
  }

  return error;
}

/** Function oyObject_GetId
 *  @memberof oyObject_s
 *  @brief   get the identification number of a object 
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2008/07/10
 */
int            oyObject_GetId        ( oyObject_s          obj )
{
  if(obj)
    return obj->id_;

  return -1;
}

/** @internal
 *  Function oyObject_Hashed_
 *  @memberof oyObject_s
 *  @brief   check if a object has a hash sum computed
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/11/02
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 */
int32_t      oyObject_Hashed_        ( oyObject_s          s )
{
  int32_t hashed = 0;
  if(s && s->type_ == oyOBJECT_OBJECT_S)
      if(((uint32_t*)(&s->hash_[0])) ||
         ((uint32_t*)(&s->hash_[4])) ||
         ((uint32_t*)(&s->hash_[8])) ||
         ((uint32_t*)(&s->hash_[12])) )
        hashed = 1;
  return hashed;
}



/** Function oyBlob_New
 *  @memberof oyBlob_s
 *  @brief   allocate a new Blob object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2009/01/06 (Oyranos: 0.1.9)
 *  @date    2009/01/06
 */
OYAPI oyBlob_s * OYEXPORT
                   oyBlob_New        ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_BLOB_S;
# define STRUCT_TYPE oyBlob_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyBlob_Copy;
  s->release = (oyStruct_Release_f) oyBlob_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** Function oyBlob_Copy_
 *  @memberof oyBlob_s
 *  @brief   real copy a Blob object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2009/01/06 (Oyranos: 0.1.9)
 *  @date    2009/01/06
 */
oyBlob_s * oyBlob_Copy_
                                     ( oyBlob_s          * obj,
                                       oyObject_s          object )
{
  oyBlob_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyBlob_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    if(obj->ptr && obj->size)
    {
      s->ptr = allocateFunc_( obj->size );
      error = !s->ptr;
      if(!error)
        error = !memcpy( s->ptr, obj->ptr, obj->size );
    }
  }

  if(!error)
    s->size = obj->size;
  else
    oyBlob_Release( &s );

  return s;
}

/** Function oyBlob_Copy
 *  @memberof oyBlob_s
 *  @brief   copy or reference a Blob object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2009/01/06 (Oyranos: 0.1.9)
 *  @date    2009/01/06
 */
OYAPI oyBlob_s * OYEXPORT
                   oyBlob_Copy       ( oyBlob_s          * obj,
                                       oyObject_s          object )
{
  oyBlob_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_BLOB_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyBlob_Copy_( obj, object );

  return s;
}
 
/** Function oyBlob_Release
 *  @memberof oyBlob_s
 *  @brief   release and possibly deallocate a Blob object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2009/01/06 (Oyranos: 0.1.9)
 *  @date    2009/01/06
 */
OYAPI int  OYEXPORT
               oyBlob_Release     ( oyBlob_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyBlob_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_BLOB_S)
  {
    WARNc_S(("Attempt to release a non oyBlob_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->ptr && s->size)
      deallocateFunc( s->ptr );
    s->size = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyBlob_SetFromData
 *  @memberof oyBlob_s
 *  @brief   set value from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @param[in]     ptr                 data
 *  @param[in]     size                data size
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
int            oyBlob_SetFromData    ( oyBlob_s          * blob,
                                       oyPointer           ptr,
                                       size_t              size )
{
  oyBlob_s * s = blob;
  int error = !s || s->type_ != oyOBJECT_BLOB_S;

  if(error)
  {
    WARNc_S("Attempt to set a non oyBlob_s object.")
    return 1;
  }

  if(!error)
  {
    if(s->ptr)
      s->oy_->deallocateFunc_( s->ptr );
    s->size = 0;

    s->ptr = s->oy_->allocateFunc_( size );
    error = !s->ptr;
  }

  if(!error)
    error = !memcpy( s->ptr, ptr, size );

  if(!error)
    s->size = size;

  return error;
}


/**
 *  @} *//* objects_generic
 */


/** \addtogroup objects_generic

 *  @{
 */

/** @internal
 *  @memberof oyHash_s
 *  @brief   new Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyHash_s *   oyHash_New_             ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_HASH_S;
# define STRUCT_TYPE oyHash_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyHash_Copy_;
  s->release = (oyStruct_Release_f) oyHash_Release_;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  @memberof oyHash_s
 *  @brief   copy a Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s *   oyHash_CopyRef_         ( oyHash_s          * entry,
                                       oyObject_s          object )
{
  oyHash_s * s = entry;
  int error = !s;

  if(!error)
    if(s->type_ != oyOBJECT_HASH_S)
      error = 1;

  if(!error)
    oyObject_Copy( s->oy_ );

  return s;
}

/** @internal
 *  @memberof oyHash_s
 *  @brief   release a Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
int                oyHash_Release_   ( oyHash_s         ** obj )
{
  /* ---- start of common object destructor ----- */
  oyHash_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_HASH_S)
  {
    WARNc_S("Attempt to release a non oyHash_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  /* should not happen */
  if(s->entry && s->entry->release)
    s->entry->release( &s->entry );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @internal
 *  @memberof oyHash_s
 *  @brief   get a new Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s *         oyHash_Get_       ( const char        * hash_text,
                                       oyObject_s          object )
{
  oyHash_s * s = 0;
  int error = !hash_text;
  uint32_t * val = 0;

  if(!error)
  {
    s = oyHash_New_(object);
    error = !s;
  }

  if(!error)
  {
    val = (uint32_t*) &s->oy_->hash_;

    if(oyStrlen_(hash_text) < OY_HASH_SIZE)
      memcpy(s->oy_->hash_, hash_text, oyStrlen_(hash_text)+1);
    else
#if 0
      error = oyMiscBlobGetMD5_( (void*)hash_text, oyStrlen_(hash_text),
                                 s->oy_->hash_ );
#else
      (*val) = oyMiscBlobGetL3_( (void*)hash_text, oyStrlen_(hash_text) );
#endif
  }

  if(!error)
    error = oyObject_SetName(s->oy_, hash_text, oyNAME_NAME);

  return s;
}

/** @internal
 *  @memberof oyHash_s
 *  @brief   copy a Oyranos hash object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
oyHash_s *         oyHash_Copy_      ( oyHash_s          * orig,
                                       oyObject_s          object )
{
  oyHash_s * s = 0; 
  int error = 0;

  if(!orig)
  {
    WARNc_S("Attempt to copy without original.")

    error = 1;
  }

  s = orig;

  if(!error)
    if(s->type_ != oyOBJECT_HASH_S)
      error = 1;

  if(!error) 
  if(!object)
  {
    if(s->entry && s->entry->copy)
      s->entry = s->entry->copy( s->entry, 0 );
    oyObject_Copy( s->oy_ );
    return s;
  }

  if(!error && object)
  {
    s = oyHash_New_( object );

    error = !s;
    if(error)
      WARNc_S("Could not create structure for hash.");
  }

  if(error)
    return 0;
  else
    return s;
}

/** @internal
 *  @memberof oyHash_s
 *  @brief hash is of type
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_IsOf_      ( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  return (hash && hash->entry && hash->entry->type_ == type);
}

/** @internal
 *  @memberof oyHash_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
oyStruct_s *       oyHash_GetPointer_( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  if(oyHash_IsOf_( hash, type))
    return hash->entry;
  else
    return 0;
}

/** @internal
 *  @memberof oyHash_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_SetPointer_( oyHash_s          * hash,
                                       oyStruct_s        * obj )
{
  if(hash)
  {
    hash->entry = obj;
    return 0;
  } else
    return 1;
}




/** @internal
 *  @brief get always a Oyranos cache entry from a cache list
 *
 *  @param[in]     cache_list          the list to search in
 *  @param[in]     hash_text           the text to search for in the cache_list
 *  @return                            the cache entry may not have a entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s *   oyCacheListGetEntry_    ( oyStructList_s    * cache_list,
                                       const char        * hash_text )
{
  oyHash_s * entry = 0;
  oyHash_s * search_key = 0;
  int error = !(cache_list && hash_text);
  int n = 0, i;

  if(!error && cache_list->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(!error)
  {
    search_key = oyHash_Get_(hash_text, 0);
    error = !search_key;
  }

  if(!error)
    n = oyStructList_Count(cache_list);

  for(i = 0; i < n; ++i)
  {
    oyHash_s * compare = (oyHash_s*) oyStructList_GetType_( cache_list, i,
                                                         oyOBJECT_HASH_S );

    if(compare)
    if(memcmp(search_key->oy_->hash_, compare->oy_->hash_, OY_HASH_SIZE) == 0)
      entry = compare;
  }

  if(!error && !entry)
  {
    if(!error)
      entry = oyHash_Copy_( search_key, 0 );

    if(!error)
    {
      error = oyStructList_MoveIn(cache_list, (oyStruct_s**)&search_key, -1);
      search_key = 0;
    }
  }

  oyHash_Release_( &search_key );

  if(entry)
    return oyHash_Copy_( entry, 0 );
  else
    return 0;
}


/**
 *  @} *//* objects_generic
 */





/** @internal
 *  The lists are allocated one time and live until the application quits
 *  It contains the various caches for faster access of CPU intentsive data.\n
 *  We'd need a 3 dimensional table to map\n
 *    A: a function or resource type\n
 *    B: a CMM\n
 *    C: a hash value specifying under which conditions the resource was build\n
 *  The resulting cache entry is the result from the above 3 arguments. With the
 *  much implementation work and expectedly small to no speed advantage it is 
 *  not a good strategy. Even though argument C would be extensible.\n
 *  \n
 *  A different approach would use two values to map the search request to the 
 *  cache entry. The above hash map and the hash or combination of the two other *  values.\
 *  \n
 *  One point to consider is a readable string to end not with a anonymous list
 *  full of anonymous entries, where a user can pretty much nothing know.
 *  A transparent approach has to allow for easy identifying each entry.\n
 *  A help would be a function to compute both a md5 digest and a message from
 *  the 3 arguments outlined above. Probably it would allow much more arguments
 *  to add as we can not know how many optins and other influential parameters
 *  the cache entry depends on.\n
 *  \n
 *  A final implementation would consist of a
 *  - function to convert arbitrary (string) arguments to a single string and a
 *    hash sum 
 *  - a function to create a cache entry struct from above hash and a the 
 *    according description string plus the oyCMMptr_s struct. For simplicity
 *    the cache struct can be identical to the oyCMMptr_s, with the disadvantage
 *    of containing additional data not understandable for a CMM. We need to 
 *    mark these data (hash + description) as internal to Oyranos.
 *  - a list that hold above cache entry stucts
 *  - several functions to reference, release, maps a hash value to the
 *    cached resource
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_cache_ = 0;

/** @internal
 *  @brief get always a Oyranos cache entry from the CMM's cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyHash_s *   oyCMMCacheListGetEntry_ ( const char        * hash_text)
{
  if(!oy_cmm_cache_)
    oy_cmm_cache_ = oyStructList_New( 0 );

  return oyCacheListGetEntry_(oy_cmm_cache_, hash_text);
}

/** @internal
 *  @brief get the Oyranos CMM cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyStructList_s** oyCMMCacheList_()
{
  return &oy_cmm_cache_;
}

/** @internal
 *  @brief get the Oyranos CMM cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyChar* oyCMMCacheListPrint_()
{
  oyStructList_s ** cache_list = oyCMMCacheList_();
  int n = oyStructList_Count( *cache_list ), i;
  oyChar * text = 0;
  oyChar refs[80];

  oySprintf_( refs,"%s:%d Oyranos CMM cache with %d entries:\n", 
              __FILE__,__LINE__, n);
  STRING_ADD( text, refs );

  for(i = 0; i < n ; ++i)
  {
    oyHash_s * compare = (oyHash_s*) oyStructList_GetType_(*cache_list, i,
                                                         oyOBJECT_HASH_S );

    if(compare)
    {
      oySprintf_(refs,"%d: ", compare->oy_->ref_);
      STRING_ADD( text, refs );
      STRING_ADD( text, oyObject_GetName(compare->oy_, oyNAME_NAME) );
      STRING_ADD( text, "\n" );
    }
  }

  return text;
}




/** \addtogroup objects_value Values Handling

 *  @{
 */

/** Function oyValueCopy
 *  @memberof oyValue_u
 *  @brief   copy a oyValue_u union
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
void           oyValueCopy           ( oyValue_u         * to,
                                       oyValue_u         * from,
                                       oyVALUETYPE_e       type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int n = 0, i;

  if(!from || !to)
    return;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_; 

  switch(type)
  {
  case oyVAL_INT: to->int32 = from->int32; break;
  case oyVAL_INT_LIST:
       if(to->int32_list && deallocateFunc)
       {
         deallocateFunc(to->int32_list);
         to->int32_list = 0;
       }

       if(!from->int32_list)
         return;

       n = from->int32_list[0];

       to->int32_list = allocateFunc( (n+1) * sizeof(int32_t) );
       to->int32_list[0] = n;
       for(i = 1; i <= n; ++i)
         to->int32_list[i] = from->int32_list[i];
       break;
  case oyVAL_DOUBLE: to->dbl = from->dbl; break;
  case oyVAL_DOUBLE_LIST:
       if(to->dbl_list && deallocateFunc)
       {
         deallocateFunc(to->dbl_list);
         to->dbl_list = 0;
       }

       if(!from->dbl_list)
         return;

       n = from->dbl_list[0];

       to->dbl_list = allocateFunc( (n+1) * sizeof(double));

       to->dbl_list[0] = n;
       for(i = 1; i <= n; ++i)
         to->dbl_list[i] = from->dbl_list[i];

       break;
  case oyVAL_STRING:
       if(to->string && deallocateFunc)
       {
         deallocateFunc(to->string);
         to->string = 0;
       }

       to->string = oyStringCopy_(from->string, allocateFunc);
       break;
  case oyVAL_STRING_LIST:
       if(to->string_list && deallocateFunc)
       {
         i = 0;
         while(to->string_list[i])
           deallocateFunc(to->string_list[i++]);
         deallocateFunc(to->string_list);
         to->string_list = 0;
       }

       if(!from->string_list)
         return;

       i = 0;
       n = 0;
       while((size_t)from->string_list[i])
         ++n;

       to->string_list = allocateFunc( n * sizeof(char*));
       i = 0;
       while(from->string_list[i])
       {
         to->string_list[i] = oyStringCopy_(from->string_list[i], allocateFunc);
         ++i;
       }
       to->string_list[n] = 0;

       break;
  case oyVAL_STRUCT:
       if(to->oy_struct && deallocateFunc)
       {
         if(to->oy_struct->release)
           to->oy_struct->release( &to->oy_struct );
         to->oy_struct = 0;
       }

       if(!from->oy_struct)
         return;
       if(from->oy_struct->copy)
         to->oy_struct = from->oy_struct->copy( from->oy_struct,
                                                from->oy_struct->oy_ );
       break;
  }
}

/** Function oyValueClear
 *  @memberof oyValue_u
 *  @brief   clear a oyValue_u union
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
void           oyValueClear          ( oyValue_u         * v,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc )
{
  int i;

  if(!v)
    return;

  if(!deallocateFunc)
    return; 

  switch(type)
  {
  case oyVAL_INT:
  case oyVAL_DOUBLE:
       break;
  case oyVAL_INT_LIST:
  case oyVAL_DOUBLE_LIST:
       if(!v->int32_list)
         break;

       if(v->int32_list)
         deallocateFunc(v->int32_list);

       break;
  case oyVAL_STRING:
       deallocateFunc( v->string );
       break;
  case oyVAL_STRING_LIST:
       if(!v->string_list)
         break;

       if(v->string_list)
       {
         i = 0;
         while(v->string_list[i])
           deallocateFunc(v->string_list[i++]);
         deallocateFunc(v->string_list);
       }

       break;
  case oyVAL_STRUCT:
       if(v->oy_struct->release)
         v->oy_struct->release( &v->oy_struct );
       break;
  }
}
/** Function oyValueRelease
 *  @memberof oyValue_u
 *  @brief   release a oyValue_u union
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
void           oyValueRelease        ( oyValue_u        ** v,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc )
{
  if(!v || !*v)
    return;

  if(!deallocateFunc)
    return; 

  oyValueClear( *v, type, deallocateFunc );

  deallocateFunc(*v);
  *v = 0;
}

/** Function oyValueTypeText
 *  @memberof oyValue_u
 *  @brief   obtain a short string about union type
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
const char *   oyValueTypeText       ( oyVALUETYPE_e       type )
{
  switch(type)
  {
  case oyVAL_INT:         return "xs:integer";
  case oyVAL_DOUBLE:      return "xs:double";
  case oyVAL_INT_LIST:    return "xs:integer";
  case oyVAL_DOUBLE_LIST: return "xs:double";
  case oyVAL_STRING:      return "xs:string";
  case oyVAL_STRING_LIST: return "xs:string";
  case oyVAL_STRUCT:      return "struct";
  }
  return 0;
}



static int oy_option_id_ = 0;

/** Function oyOption_New
 *  @memberof oyOption_s
 *  @brief   new option
 *
 *  @param         object              optional user object
 *  @param         registration        no or full qualified registration
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOption_s *   oyOption_New          ( oyObject_s          object,
                                       const char        * registration )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OPTION_S;
# define STRUCT_TYPE oyOption_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyOption_Copy;
  s->release = (oyStruct_Release_f) oyOption_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->id = oy_option_id_++;

  if(registration)
  {
    if(!oyStrlen_(registration) ||
       !oyStrrchr_( registration, OY_SLASH_C ))
    {
      WARNc2_S("%s: %s",
               "passed a incomplete registration string to option creation",
               registration );
      return 0;
    } else
      s->registration = oyStringCopy_( registration, s->oy_->allocateFunc_ );
  }

  return s;
}

/**
 *  @internal
 *  Function oyOption_Copy__
 *  @memberof oyOption_s
 *  @brief   copy the content of a option to an other
 *
 *  @param[out]    to                  resulting options
 *  @param[in]     from                input options
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/11/27
 */
int          oyOption_Copy__         ( oyOption_s        * to,
                                       oyOption_s        * from )
{
  oyOption_s * s = to;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!to || !from)
    return 1;

  allocateFunc_ = s->oy_->allocateFunc_;
  deallocateFunc_ = s->oy_->deallocateFunc_;

  error = oyOption_Clear( s );

  if(!error)
  {
    s->registration = oyStringCopy_( from->registration, allocateFunc_ );
    s->value_type = from->value_type;
    s->value = allocateFunc_(sizeof(oyValue_u));
    oyValueCopy( s->value, from->value, s->value_type,
                 allocateFunc_, deallocateFunc_ );
    s->source = from->source;
    s->flags = from->flags;
  }

  return 0;
}


/**
 *  @internal
 *  Function oyOption_Copy_
 *  @memberof oyOption_s
 *  @brief   real copy a option object
 *
 *  @param[in]     option              option object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/11/27
 */
oyOption_s * oyOption_Copy_          ( oyOption_s        * option,
                                       oyObject_s          object )
{
  oyOption_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!option || !object)
    return s;

  s = oyOption_New( object, 0 );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;
  deallocateFunc_ = s->oy_->deallocateFunc_;

  if(!error)
    oyOption_Copy__( s, option );

  return s;
}
/** Function oyOption_Copy
 *  @memberof oyOption_s
 *  @brief   copy or reference a option
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOption_s *   oyOption_Copy         ( oyOption_s        * option,
                                       oyObject_s          object )
{
  oyOption_s * s = 0;

  if(!option)
    return s;

  if(option && !object)
  {
    s = option;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyOption_Copy_( option, object );

  return s;
}


/** Function oyOption_Release
 *  @memberof oyOption_s
 *  @brief   release a option
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
int            oyOption_Release      ( oyOption_s       ** obj )
{
  /* ---- start of common object destructor ----- */
  oyOption_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_OPTION_S)
  {
    WARNc_S("Attempt to release a non oyOption_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  s->id = 0;

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyOption_Clear( s );

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyOption_Clear
 *  @memberof oyOption_s
 *  @brief   clear a option
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
int            oyOption_Clear        ( oyOption_s        * s )
{
  if(!s)
    return 1;

  s->flags = 0;

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyValueRelease( &s->value, s->value_type, deallocateFunc );

    s->value_type = 0;

    if(s->registration)
      deallocateFunc( s->registration );
    s->registration = 0;
  }

  return 0;
}

/** Function oyOption_GetId
 *  @memberof oyOption_s
 *  @brief   get the identification number of a option 
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2008/07/10
 */
int            oyOption_GetId        ( oyOption_s        * obj )
{
  if(obj)
    return obj->id;

  return -1;
}

/** Function oyOption_GetValueText
 *  @memberof oyOption_s
 *  @brief   get value as a text dump 
 *
 *  @param         obj                 the option
 *  @param         allocateFunc        user allocator
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/05 (Oyranos: 0.1.9)
 *  @date    2008/12/05
 */
char *         oyOption_GetValueText ( oyOption_s        * obj,
                                       oyAlloc_f           allocateFunc )
{
  int error = !obj;
  char * erg = 0;
  oyValue_u * v = 0;
  oyStructList_s * oy_struct_list = 0;

  if(!error)
    v = obj->value;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  error = !v;

  if(!error)
  {
    int n = 1, i = 0;
    char * tmp = oyAllocateFunc_(1024),
         * text = 0;

    switch(obj->value_type)
    {
    case oyVAL_INT_LIST:    n = v->int32_list[0]; break;
    case oyVAL_DOUBLE_LIST: n = (int)v->dbl_list[0]; break;
    case oyVAL_STRING_LIST: while( v->string_list[0] ) ++n; break;
    case oyVAL_INT:
    case oyVAL_DOUBLE:
    case oyVAL_STRING:
    case oyVAL_STRUCT:
         n = 1; break;
    }

    if(obj->value_type == oyVAL_STRUCT)
    {
      oy_struct_list = (oyStructList_s*) v->oy_struct;
      if(oy_struct_list->type_ == oyOBJECT_STRUCT_LIST_S)
        n = oyStructList_Count( oy_struct_list );
    }

    for(i = 0; i < n; ++i)
    {
      if(obj->value_type == oyVAL_INT)
        oySprintf_(tmp, "%d", v->int32);
      if(obj->value_type == oyVAL_DOUBLE)
        oySprintf_(tmp, "%f", v->dbl);
      if(obj->value_type == oyVAL_INT_LIST)
        oySprintf_(tmp, "%d", v->int32_list[i+1]);
      if(obj->value_type == oyVAL_DOUBLE_LIST)
        oySprintf_(tmp, "%f", v->dbl_list[i+1]);

      switch(obj->value_type)
      {
      case oyVAL_INT:
      case oyVAL_DOUBLE:
      case oyVAL_INT_LIST:
      case oyVAL_DOUBLE_LIST: STRING_ADD( text, tmp ); break;
      case oyVAL_STRING:      STRING_ADD( text, v->string ); break;
      case oyVAL_STRING_LIST: STRING_ADD( text, v->string_list[i] ); break;
      case oyVAL_STRUCT:      break;
      }
      if(obj->value_type == oyVAL_STRUCT)
      {
        if(oy_struct_list)
        {
          oyStruct_s * oy_struct = oyStructList_Get_( oy_struct_list, i );
          if(oy_struct && oy_struct->oy_)
            STRING_ADD ( text, oyObject_GetName( oy_struct->oy_, oyNAME_NICK ) );
        } else if(v->oy_struct->oy_)
          STRING_ADD ( text, oyObject_GetName( v->oy_struct->oy_, oyNAME_NICK ));
      }
      if(i)
        STRING_ADD ( text, ":" );
    }

    erg = oyStringCopy_( text, allocateFunc );
  
    oyFree_m_( tmp );
    oyFree_m_( text );
  }


  return erg;
}

/** Function oyOption_GetText
 *  @memberof oyOption_s
 *  @brief   get a text dump 
 *
 *  Only oyOption_s::value is written.
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XFORMS element
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - option registration name with key and without 
 *                         attributes or value 
 *
 *  @param[in,out] obj                 the option
 *  @param         type                oyNAME_NICK is equal to an ID
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
const char *   oyOption_GetText      ( oyOption_s        * obj,
                                       oyNAME_e            type )
{
  int error = !obj;
  const char * erg = 0;
  oyValue_u * v = 0;

  if(!error)
    v = obj->value;

  error = !v;

  if(!error)
    if(type == oyNAME_DESCRIPTION)
    {
      char * text = oyStringCopy_(obj->registration, oyAllocateFunc_),
           * tmp = oyStrrchr_(text, '/');
      if(oyStrrchr_(tmp, '.'))
      {
        tmp = oyStrrchr_(text, '.');
        *tmp = 0;
      }
      error = oyObject_SetName( obj->oy_, text, type );
      oyFree_m_(text);
    }

  if(!error && 
     ( type == oyNAME_NICK || type == oyNAME_NAME ))
  {
    int n = 1, i = 0, j;
    char * tmp = 0,
         * text = 0;
    char ** list = 0;

    if(!oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ))
      oyOption_GetText(obj, oyNAME_DESCRIPTION);


    if(type == oyNAME_NICK)
    {
      STRING_ADD ( text, oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ) );
      STRING_ADD ( text, ":" );
    } else if(type == oyNAME_NAME)
    {
      list = oyStringSplit_( oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ),
                             '/', &n, oyAllocateFunc_);
      for( i = 0; i < n; ++i )
      {
        for(j = 0; j < i; ++j)
          STRING_ADD ( text, " " );
        STRING_ADD ( text, "<" );
        STRING_ADD ( text, list[i] );
        if(i+1==n)
          STRING_ADD ( text, ">" );
        else
          STRING_ADD ( text, ">\n" );
      }
    }

    tmp = oyOption_GetValueText( obj, oyAllocateFunc_ );
    STRING_ADD ( text, tmp );

    if(type == oyNAME_NAME)
    {
      list = oyStringSplit_( oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ),
                             '/', &n, oyAllocateFunc_);
      for( i = n-1; i >= 0; --i )
      {
        if(i+1 < n)
        for(j = 0; j < i; ++j)
          STRING_ADD ( text, " " );
        STRING_ADD ( text, "</" );
        STRING_ADD ( text, list[i] );
        if(i)
          STRING_ADD ( text, ">\n" );
        else
          STRING_ADD ( text, ">" );
      }
    }

    error = oyObject_SetName( obj->oy_, text, type );

    oyFree_m_( tmp );
    oyFree_m_( text );
  }

  erg = oyObject_GetName( obj->oy_, type );

  return erg;
}


/** Function oyOption_SetFromText
 *  @memberof oyOption_s
 *  @brief   set a option value from a string
 *
 *  @param         obj                 the option
 *  @param         text                the text to set
 *  @param         flags               possible is OY_STRING_LIST
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/25 (Oyranos: 0.1.9)
 *  @date    2008/11/25
 */
int            oyOption_SetFromText  ( oyOption_s        * obj,
                                       const char        * text,
                                       uint32_t            flags )
{
  int error = !obj;
  char ** list = 0;
  int n = 0;

  if(!error)
  {
    if(obj->value)
    {
      oyDeAlloc_f deallocateFunc = obj->oy_->deallocateFunc_;

      oyValueRelease( &obj->value, obj->value_type, deallocateFunc );
    }

    obj->value = obj->oy_->allocateFunc_(sizeof(oyValue_u));

    if(oyToStringList_m(flags))
    {
      /** Split for flags & OY_STRING_LIST at newline. */
      list = oyStringSplit_( text, '\n', &n, obj->oy_->allocateFunc_ );
      obj->value->string_list = list; list = 0;
      obj->value_type = oyVAL_STRING_LIST;
    } else
    {
      obj->value->string = oyStringCopy_( text, obj->oy_->allocateFunc_ );
      obj->value_type = oyVAL_STRING;
    }
    obj->flags |= oyOPTIONATTRIBUTE_EDIT;
  }

  return error;
}


/**
 *  @internal
 *  Function oyOption_Match_
 *  @memberof oyOption_s
 *  @brief   two option matches
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
int            oyOption_Match_       ( oyOption_s        * option_a,
                                       oyOption_s        * option_b )
{
  int erg = 0;

  if( option_a && option_b )
  {
    char * a =  oyFilterRegistrationToText( option_a->registration,
                                            oyFILTER_REG_TYPE, 0 );
    char * b =  oyFilterRegistrationToText( option_b->registration,
                                            oyFILTER_REG_TYPE, 0 );

    if( oyStrcmp_( a, b ) == 0 )
      erg = 1;

    oyDeAllocateFunc_(a);
    oyDeAllocateFunc_(b);
  }

  return erg;
}

/**
 *  @internal
 *  Function oyOption_UpdateFlags_
 *  @memberof oyOption_s
 *  @brief   set the ::flags member
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
void         oyOption_UpdateFlags_   ( oyOption_s        * o )
{
  char * tmp = 0;

  if(o && o->registration && oyStrrchr_( o->registration, '/' ))
  {
      tmp = oyStrrchr_( o->registration, '/' );
      if(oyStrstr_( tmp, "front" ))
         o->flags |= oyOPTIONATTRIBUTE_FRONT;
      if(oyStrstr_( tmp, "advanced" ))
         o->flags |= oyOPTIONATTRIBUTE_ADVANCED;
      tmp = 0;
  }
}

/** Function oyOption_SetFromData
 *  @memberof oyOption_s
 *  @brief   set value from a data blob
 *
 *  @param[in]     option              the option
 *  @param[in]     ptr                 data
 *  @param[in]     size                data size
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/04 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
int            oyOption_SetFromData  ( oyOption_s        * option,
                                       oyPointer           ptr,
                                       size_t              size )
{
  int error = !option || option->type_ != oyOBJECT_OPTION_S;
  oyAlloc_f allocateFunc_ = oyAllocateFunc_;
  oyDeAlloc_f deallocateFunc_ = oyDeAllocateFunc_;

  if(error)
  {
    WARNc_S("Attempt to set a non oyOption_s object.")
    return 1;
  }

  if(!error)
  {
    if(option->oy_)
    {
      allocateFunc_ = option->oy_->allocateFunc_;
      deallocateFunc_ = option->oy_->deallocateFunc_;
    }

    oyValueClear( option->value, option->value_type, deallocateFunc_ );
    if(!option->value)
      option->value = allocateFunc_(sizeof(oyValue_u));
    error = !option->value;
  }

  if(!error)
  {
    option->value->oy_struct = (oyStruct_s*) oyBlob_New( 0 );
    error = !option->value->oy_struct;
    option->value_type = oyVAL_STRUCT;
  }

  if(!error)
    error = oyBlob_SetFromData( (oyBlob_s*) option->value->oy_struct,
                                ptr, size );

  return error;
}

/** Function oyOption_GetData
 *  @memberof oyOption_s
 *  @brief   get the data blob
 *
 *  @param[in]     option              the option
 *  @param[out]    size                data size
 *  @param[in]     allocateFunc        user allocator
 *  @return                            data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/04 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
oyPointer      oyOption_GetData      ( oyOption_s        * option,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !option || option->type_ != oyOBJECT_OPTION_S;
  oyPointer ptr = 0;
  size_t size_ = 0;
  oyBlob_s * blob = 0;

  if(error)
  {
    WARNc_S("Attempt to query a non oyOption_s object.")
    return 0;
  }

  if(!error)
  {
    if(!allocateFunc &&
       option->oy_)
      allocateFunc = option->oy_->allocateFunc_;

    if(!option->value || option->value_type != oyVAL_STRUCT ||
       option->value->oy_struct->type_ != oyOBJECT_BLOB_S ||
       !((oyBlob_s*)(option->value->oy_struct))->ptr ||
       ((oyBlob_s*)(option->value->oy_struct))->size == 0)
      error = 1;
  }

  if( !error )
  {
    blob = (oyBlob_s*)option->value->oy_struct;
    size_ = blob->size;
    ptr = oyAllocateWrapFunc_( size_, allocateFunc );
    error = !ptr;
  }

  if(!error)
  {
    error = !memcpy( ptr, blob->ptr, size_ );
    if(!error && size)
      *size = size_;
  }

  if(error)
    ptr = 0;

  return ptr;
}




/** Function oyOptions_New
 *  @memberof oyOptions_s
 *  @brief   new options
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOptions_s *  oyOptions_New         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OPTIONS_S;
# define STRUCT_TYPE oyOptions_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyOptions_Copy;
  s->release = (oyStruct_Release_f) oyOptions_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list = oyStructList_New( 0 );

  return s;
}

/** Function oyOptions_FromBoolean
 *  @memberof oyOptions_s
 *  @brief   boolean operations on two sets of option
 *
 *  @param[in]     set_a               options set A
 *  @param[in]     set_b               options set B
 *  @param[in]     type                the operation to perform
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
oyOptions_s *  oyOptions_FromBoolean ( oyOptions_s       * set_a,
                                       oyOptions_s       * set_b,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object )
{
  int error = !set_a && !set_b;
  oyOptions_s * options = 0;
  oyOption_s * option_a = 0, * option_b = 0;
  int set_an = oyOptions_Count( set_a ),
      set_bn = oyOptions_Count( set_b );
  int i, j,
      found = 0;
  char * txt_1, * txt_2;

  if(!error)
  {
    for(i = 0; i < set_an; ++i)
    {
      option_a = oyOptions_Get( set_a, i );
      txt_1 = oyFilterRegistrationToText( option_a->registration,
                                          oyFILTER_REG_OPTION, 0);

      for(j = 0; j < set_bn; ++j)
      {
        option_b = oyOptions_Get( set_b, j );
        txt_2 = oyFilterRegistrationToText( option_b->registration,
                                            oyFILTER_REG_OPTION, 0);

        found = oyTextboolean_( txt_1, txt_2, type );

        /* add to the list */
        if(found > 0)
        {
          if(!options)
            options = oyOptions_New(0);

          if(option_a->value_type != option_b->value_type)
            found = 0;

          if(found)
          {
            oyOptions_Add( options, option_a, -1, object );
            if(found == 2)
              oyOptions_Add( options, option_b, -1, object );
          }
        }

        oyFree_m_( txt_2 );
        oyOption_Release( &option_b );
      }

      oyFree_m_( txt_1 );
      oyOption_Release( &option_a );
    }

    if(!set_an && set_bn &&
       (type == oyBOOLEAN_UNION || type == oyBOOLEAN_DIFFERENZ))
      options = oyOptions_Copy( set_b, object );
    if(set_an && !set_bn &&
       (type == oyBOOLEAN_UNION))
      options = oyOptions_Copy( set_a, object );
  }

  return options;
}


/**
 *  @internal
 *  Function oyOption_FromStatic_
 *  @memberof oyOption_s
 *  @brief   provide the current state of Oyranos behaviour settings
 *
 *  @todo harmonise the xml key names to the elektra ones to form one namespace
 *  @deprecated we move to the oyCMMapi5_s for static stuff
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/06 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
oyOption_s *   oyOption_FromStatic_  ( oyOption_t_       * opt,
                                       oyObject_s          object )
{
  oyOption_s * s = 0;
  int error = !opt;

  if(error)
    return s;

  s = oyOption_New( object, 0 );
  if(!s)
    return s;

  /*s->id = opt->id;*/
  s->registration = oyStringAppend_( opt->config_string, opt->config_string_xml,
                                     s->oy_->allocateFunc_ );
  s->value = s->oy_->allocateFunc_(sizeof(oyValue_u));

  if(oyWIDGET_BEHAVIOUR_START < opt->id && opt->id < oyWIDGET_BEHAVIOUR_END)
  {
    s->value_type = oyVAL_DOUBLE;
    s->value->dbl = opt->default_value;
    s->source = oyOPTIONSOURCE_FILTER;
  } else
  {
    s->value_type = oyVAL_STRING;
    s->value->string = oyGetDefaultProfileName( (oyPROFILE_e)opt->id,
                                                s->oy_->allocateFunc_ );
    if(!s->value->string)
      s->value->string = oyStringCopy_( opt->default_string, s->oy_->allocateFunc_);
    s->source = oyOPTIONSOURCE_FILTER;
    oyOption_UpdateFlags_( s );
  }

  return s;
}


#include <libxml/parser.h>

/**
 *  @internal
 *  Function oyOptions_ParseXML_
 *  @memberof oyOptions_s
 *  @brief   deserialise a text file to oyOptions_s data
 *
 *  This function is parsing libxml2 structures.
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
void           oyOptions_ParseXML_   ( oyOptions_s       * s,
                                       char            *** texts,
                                       int               * texts_n,
                                       xmlDocPtr           doc,
                                       xmlNodePtr          cur )
{
  oyOption_s * o = 0;
  char * tmp = 0;
  int i;
  xmlChar *key = 0;

  while (cur != NULL)
  {
    if(cur->type == XML_ELEMENT_NODE)
      oyStringListAddStaticString_( texts, texts_n, cur->name,
                                    oyAllocateFunc_, oyDeAllocateFunc_ );

    if(cur->xmlChildrenNode)
    {
      oyOptions_ParseXML_( s, texts, texts_n, doc, cur->xmlChildrenNode );
      *texts_n -= 1;
      oyDeAllocateFunc_( (*texts)[*texts_n] );
    }

    if(cur->type == XML_TEXT_NODE && !cur->children &&
       cur->content && oyStrlen_(cur->content) && cur->content[0] != '\n')
    {
      for( i = 0; i < *texts_n; ++i )
      {
        if(i)
          STRING_ADD( tmp, "/" );
        STRING_ADD( tmp, (*texts)[i] );
      }

      o = oyOption_New(0, tmp);
      o->value = o->oy_->allocateFunc_(sizeof(oyValue_u));

      o->value_type = oyVAL_STRING;

      key = xmlNodeListGetString(doc, cur, 1);
      o->value->string = oyStringCopy_( key, o->oy_->allocateFunc_ );
      xmlFree(key);
      oyFree_m_( tmp );

      o->source = oyOPTIONSOURCE_DATA;

      oyOption_UpdateFlags_( o );

      oyOptions_MoveIn( s, &o, -1 );
    }
    cur = cur->next;
  }
}

/** Function oyOptions_FromText
 *  @memberof oyOptions_s
 *  @brief   deserialise a text file to oyOptions_s data
 *
 *  This function is based on libxml2.
 *
 *  @param[in]     text                the text to process
 *  @param[in]     flags               for future use
 *  @param         object              the optional object
 *  @return                            the data
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
oyOptions_s *  oyOptions_FromText    ( const char        * text,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  int error = !text;
  xmlDocPtr doc = 0;
  xmlNodePtr cur = 0;
  char ** texts = 0;
  int texts_n = 0;
  const char * root_start = "<top>", * root_end = "</top>";
  char * tmp = 0;

  if(!error)
  {
    /* add a root level node - <top> */
    tmp = oyStringAppend_( root_start, text, oyAllocateFunc_ );
    STRING_ADD( tmp, root_end );

    doc = xmlParseMemory( tmp, oyStrlen_( tmp ) );
    error = !doc;

    oyFree_m_( tmp );

    if(doc)
    {
      cur = xmlDocGetRootElement(doc);
      /* skip now the artifically added <top> level */
      cur = cur->xmlChildrenNode;
    }
    error = !cur;
  }

  if(!error)
  {
    s = oyOptions_New(0);

    oyOptions_ParseXML_( s, &texts, &texts_n, doc, cur );

    xmlFreeDoc(doc);
  }

  return s;
}


/** Function oyOptions_DoFilter
 *  @memberof oyOptions_s
 *  @brief   filter the options
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument. advanced options can be 
 *  filtered out by adding oyOPTIONATTRIBUTE_ADVANCED.
 *
 *  Backends should handle the advanced options as well but shall normally
 *  not act upon them. The convention to set them zero, keeps them inactive.
 *  
 *  On the front end side the CMM cache has to include them, as they will 
 *  influence the hash sum generation. The question arrises, whether to include
 *  these options marked as non visible along the path or require the CMM cache
 *  code to check each time for them on cache lookup. The oyOption_s::flags
 *  is already in place. So we use it and do inclusion. Front end options can be
 *  filtered as they do not affect the CMM cache.
 *
 *  @param         s                   the options
 *  @param[in]     flags               for inbuild defaults | oyOPTIONSOURCE_FILTER; for options marked as advanced | oyOPTIONATTRIBUTE_ADVANCED; for front end options | oyOPTIONATTRIBUTE_FRONT
 *  @param         filter_type         the type level from a registration
 *  @return                            options
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
int          oyOptions_DoFilter      ( oyOptions_s       * s,
                                       uint32_t            flags,
                                       const char        * filter_type )
{
  oyOptions_s * opts_tmp = 0;
  oyOption_s * o = 0;
  int error = !s;
  char * text;
  int i,n;

  oyExportStart_(EXPORT_SETTING);
  oyExportEnd_();

  if(!error && (flags || filter_type))
  {
    /*  6. get stored values */
    n = oyOptions_Count( s );
    opts_tmp = oyOptions_New(0);
    for(i = 0; i < n; ++i)
    {
      int skip = 0;

      o = oyOptions_Get( s, i );


      /* usage/type range filter */
      if(filter_type)
      {
        text = oyFilterRegistrationToText( o->registration, oyFILTER_REG_TYPE,
                                           0);
        if(oyStrcmp_( filter_type, text ) != 0)
          skip = 1;

        oyFree_m_( text );
      }

      /* front end options filter */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_FRONT))
      {
        text = oyStrrchr_( o->registration, '/' );

        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "front" ))
            skip = 1;
      }

      /* advanced options mark and zero */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_ADVANCED))
      {
        text = oyStrrchr_( o->registration, '/' );
        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "advanced" ))
          {
            oyOption_SetFromText( o, "0", 0 );
            o->flags = o->flags & (~oyOPTIONATTRIBUTE_EDIT);
          }
      } else
      /* Elektra settings, modify value */
      if(!skip && !(flags & oyOPTIONSOURCE_FILTER))
      {
        text = oyGetKeyString_( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                oyAllocateFunc_ );
        if(text && oyStrlen_(text))
        {
          error = oyOption_SetFromText( o, text, 0 );
          o->flags = o->flags & (~oyOPTIONATTRIBUTE_EDIT);
          o->source = oyOPTIONSOURCE_USER;
          oyFree_m_( text );
        }
      }

      if(!skip)
        oyOptions_Add( opts_tmp, o, -1, s->oy_ );

      oyOption_Release( &o );
    }

    error = oyStructList_CopyFrom( s->list, opts_tmp->list, 0 );

    oyOptions_Release( &opts_tmp );
  }

  return error;
}


/** Function oyOptions_ForFilter_
 *  @memberof oyOptions_s
 *  @brief   provide Oyranos behaviour settings
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all front end options from a meta backend use:@verbatim
 *  flags = oyOPTIONATTRIBUTE_ADVANCED |
 *          oyOPTIONATTRIBUTE_FRONT |
 *          OY_OPTIONSOURCE_META @endverbatim
 *
 *  @param[in]     filter              the filter
 *  @param[in]     flags               for inbuild defaults | oyOPTIONSOURCE_FILTER; for options marked as advanced | oyOPTIONATTRIBUTE_ADVANCED | OY_OPTIONSOURCE_FILTER | OY_OPTIONSOURCE_META
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 *  @date    2008/12/08
 */
oyOptions_s *  oyOptions_ForFilter_  ( oyFilter_s        * filter,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0,
              * opts_tmp = 0,
              * opts_tmp2 = 0;
  oyOption_s * o = 0;
  int error = 0;
  char * type_txt = oyFilterRegistrationToText( filter->registration_,
                                                oyFILTER_REG_TYPE, 0 );
  oyCMMapi5_s * api5 = 0;
  int i,n;

  /* by default we parse both sources */
  if(!(flags & OY_OPTIONSOURCE_FILTER) && !(flags & OY_OPTIONSOURCE_META))
    flags |= OY_OPTIONSOURCE_FILTER | OY_OPTIONSOURCE_META;

  {
    /**
        Programm:
        1. get filter and its type
        2. get implementation for filter type
        3. parse static common options from meta backend
        4. parse static options from filter 
        5. merge both
        6. get stored values from disk
     */

    /*  1. get filter */

    /*  2. get implementation for filter type */
    api5 = filter->api4_->api5_;

    /*  3. parse static common options from meta backend */
    if(api5 && flags & OY_OPTIONSOURCE_META)
      opts_tmp = oyOptions_FromText( api5->options, 0, object );
    /* requires step 2 */

    /*  4. parse static options from filter */
    if(flags & OY_OPTIONSOURCE_FILTER)
      opts_tmp2 = oyOptions_FromText( filter->api4_->options, 0, object );

    /*  5. merge */
    s = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION, object );

    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );

    /*  6. get stored values */
    n = oyOptions_Count( s );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( s, i );
      o->source = oyOPTIONSOURCE_FILTER;
      oyOption_Release( &o );
    }
    error = oyOptions_DoFilter ( s, flags, type_txt );
  }
  oyDeAllocateFunc_( type_txt );

  return s;
}

/** Function oyOptions_ForFilter
 *  @memberof oyOptions_s
 *  @brief   provide Oyranos behaviour settings
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all front end options from a meta backend use:@verbatim
 *  flags = oyOPTIONATTRIBUTE_ADVANCED |
 *          oyOPTIONATTRIBUTE_FRONT |
 *          OY_OPTIONSOURCE_META @endverbatim
 *
 *  @param[in]     registration        the filter registration to search for
 *  @param[in]     cmm                 a CMM to match
 *  @param[in]     flags               for inbuild defaults | oyOPTIONSOURCE_FILTER; for options marked as advanced | oyOPTIONATTRIBUTE_ADVANCED | OY_OPTIONSOURCE_FILTER | OY_OPTIONSOURCE_META
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/10/08 (Oyranos: 0.1.8)
 *  @date    2008/12/09
 */
oyOptions_s *  oyOptions_ForFilter   ( const char        * registration,
                                       const char        * cmm,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  oyFilter_s * filter = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  char * lib_name = 0;
  int error = 0;

  /*  1. get filter */
  filter = oyFilter_New_( object );

  error = !filter;

  if(!error)
    cmm_api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( cmm, 0, registration,
                                                   oyOBJECT_CMM_API4_S );

  lib_name = cmm_api4->id_;

  error = !(cmm_api4 && lib_name);

  if(!error)
    error = oyFilter_SetCMMapi4_( filter, cmm_api4 );

  s = oyOptions_ForFilter_( filter, flags, filter->oy_);

  oyFilter_Release( &filter );

  return s;
}

/**
 *  @internal
 *  Function oyOptions_Copy_
 *  @memberof oyOptions_s
 *  @brief   real copy a options object
 *
 *  @param[in]     options             options object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOptions_s * oyOptions_Copy_        ( oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  int error = 0;

  if(!options || !object)
    return s;

  s = oyOptions_New( object );
  error = !s;

  if(!error)
    s->list = oyStructList_Copy( options->list, s->oy_ );

  if(error)
    oyOptions_Release( &s );

  return s;
}
/** Function oyOptions_Copy
 *  @memberof oyOptions_s
 *  @brief   release options
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOptions_s *  oyOptions_Copy        ( oyOptions_s       * obj,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_OPTIONS_S)
    return s;

  if(!obj)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyOptions_Copy_( obj, object );

  return s;
}

/** Function oyOptions_Release
 *  @memberof oyOptions_s
 *  @brief   release options
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
int            oyOptions_Release     ( oyOptions_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyOptions_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_OPTIONS_S)
  {
    WARNc_S("Attempt to release a non oyOptions_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyOptions_MoveIn
 *  @memberof oyOptions_s
 *  @brief   add a element to a Options list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
OYAPI int  OYEXPORT
                 oyOptions_MoveIn    ( oyOptions_s       * list,
                                       oyOption_s       ** obj,
                                       int                 pos )
{
  oyOptions_s * s = list;
  int error = !s || s->type_ != oyOBJECT_OPTIONS_S;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_OPTION_S)
  {
    if(!s)
    {
      s = oyOptions_New(0);
      error = !s;
    }                                  

    if(!error && !s->list)
    {
      s->list = oyStructList_New( 0 );
      error = !s->list;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list, (oyStruct_s**)obj, pos );
  }   
  
  return error;
}

/** Function oyOptions_ReleaseAt
 *  @memberof oyOptions_s
 *  @brief   release a element from a Options list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
OYAPI int  OYEXPORT
                  oyOptions_ReleaseAt ( oyOptions_s * list,
                                       int                 pos )
{ 
  int error = !list;

  if(!error && list->type_ != oyOBJECT_OPTIONS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list, pos );

  return error;
}

/** Function oyOptions_Get
 *  @memberof oyOptions_s
 *  @brief   get a element of a Options list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
OYAPI oyOption_s * OYEXPORT
                 oyOptions_Get       ( oyOptions_s       * list,
                                       int                 pos )
{       
  if(list && list->type_ == oyOBJECT_OPTIONS_S)
    return (oyOption_s *) oyStructList_GetRefType( list->list, pos, oyOBJECT_OPTION_S ); 
  else  
    return 0;
}   

int            oyOptions_Count       ( oyOptions_s       * list )
{
  if(list)
    return oyStructList_Count( list->list );
  else return 0;
}

/** Function oyOptions_Add
 *  @memberof oyOptions_s
 *  @brief   add a element to a Options list
 *
 *  We must not add any already listed option. 
 *  A "shared" key has higher priority and substitutes a non "shared" one.
 *  (oyFILTER_REG_TOP)
 *
 *  Adding a new element without any checks is as simple as following code:
 *  @verbatim
 *     tmp = oyOption_Copy( option, object );
 *     oyOptions_MoveIn( options, &tmp, -1 ); @endverbatim
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
int            oyOptions_Add         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object )
{
  oyOption_s *tmp = 0;
  int error = !options || !option;
  int n, i, skip = 0;
  char * o_opt,
       * o_top,
       * l_opt,  /* l - list */
       * l_top;

  if(!error)
  {
    o_opt = oyFilterRegistrationToText( option->registration,
                                        oyFILTER_REG_OPTION, 0 );
    o_top = oyFilterRegistrationToText( option->registration,
                                        oyFILTER_REG_TOP, 0 );
    n = oyOptions_Count( options );

    for(i = 0; i < n; ++i)
    {
      tmp = oyOptions_Get( options, i );
      l_opt = oyFilterRegistrationToText( tmp->registration,
                                          oyFILTER_REG_OPTION, 0 );
      l_top = oyFilterRegistrationToText( tmp->registration,
                                          oyFILTER_REG_TOP, 0 );
      if(oyStrcmp_(l_opt, o_opt) == 0)
        skip = 2;

      /* replace as we priorise the "shared" namespace */
      if(skip == 2)
      {
        if(oyStrcmp_(o_top, OY_TOP_SHARED) == 0 &&
           oyStrcmp_(l_top, OY_TOP_SHARED) != 0)
          oyOption_Copy__( tmp, option );
        -- skip;
      }

      oyFree_m_( l_opt );
      oyFree_m_( l_top );
      oyOption_Release( &tmp );
    }

    if(skip == 0)
    {
      tmp = oyOption_Copy( option, object );
      oyOptions_MoveIn( options, &tmp, -1 );
    }

    oyFree_m_( o_opt );
    oyFree_m_( o_top );
  }

  return error;
}


/** Function oyOptions_GetText
 *  @memberof oyOptions_s
 *  @brief   dump options to text
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XFORMS data model
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - option registration name with key and without value
 *
 *  @todo streamline output and group, avoid writing all levels for each key
 *
 *  @param[in,out] options             the option
 *  @param         type                oyNAME_NICK is equal to an ID
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/25 (Oyranos: 0.1.9)
 *  @date    2008/11/25
 */
const char *   oyOptions_GetText     ( oyOptions_s       * options,
                                       oyNAME_e            type )
{
  int error = !options;
  const char * erg = 0;
  char * text = 0;
  oyOption_s * o = 0;
  int i, n;


  if(!error)
  {
    n = oyOptions_Count( options );

    for( i = 0; i < n; ++i )
    {
      o = oyOptions_Get( options, i );
      STRING_ADD ( text, oyOption_GetText( o, type) );
      STRING_ADD ( text, "\n" );

      oyOption_Release( &o );
    }

    error = oyObject_SetName( options->oy_, text, type );

    oyFree_m_( text );
  }

  if(!error)
    erg = oyObject_GetName( options->oy_, type );

  return erg;
}

/** Function oyOptions_Find
 *  @memberof oyOptions_s
 *  @brief   search for a certain option key
 *
 *  This function returns the first found option for a given key.
 *  The key is represented by a registrations option level.
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/05 (Oyranos: 0.1.9)
 *  @date    2008/11/05
 */
oyOption_s *   oyOptions_Find        ( oyOptions_s       * options,
                                       const char        * key )
{
  int error = !options || !key;
  oyOption_s * option_a = 0,
             * option = 0;
  char * tmp = 0;
  const char * opt_key = key;

  if(!error && options && options->type_ == oyOBJECT_OPTIONS_S)
  {
    oyOptions_s * set_a = options;
    int set_an = oyOptions_Count( set_a ), i;

    for(i = 0; i < set_an; ++i)
    {
      option_a = oyOptions_Get( set_a, i );

      if(option_a && option_a->type_ == oyOBJECT_OPTION_S)
      {
        tmp = oyFilterRegistrationToText( option_a->registration,
                                          oyFILTER_REG_OPTION, 0 );
        if(oyStrchr_( key, OY_SLASH_C ))
        {
          opt_key = oyStrrchr_( key, OY_SLASH_C );
          ++ opt_key;
        }

        if(oyStrcmp_( tmp, opt_key) == 0)
          option = oyOption_Copy( option_a, 0 );
        oyFree_m_( tmp );
      }

      oyOption_Release( &option_a );

    }
  }

  return option;
}

/** Function oyOptions_FindString
 *  @memberof oyOptions_s
 *  @brief   search for a certain option key and possibly value
 *
 *  This function returns the first found value for a given key.
 *  The key is represented by the oyOption_s::name::nick
 *  The returned string is valid along the livetime of the particular 
 *  option value.
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/10/07 (Oyranos: 0.1.8)
 *  @date    2008/12/03
 */
const char *   oyOptions_FindString  ( oyOptions_s       * options,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = !options;

  if(!error)
  if(options->type_ == oyOBJECT_OPTIONS_S)
  {
    oyOptions_s * set_a = options;
    int set_an = oyOptions_Count( set_a ), i,j;
    oyOption_s * option_a = 0;
    int found = 0;

    for(i = 0; i < set_an; ++i)
    {
      option_a = oyOptions_Get( set_a, i );

      if(option_a && option_a->type_ == oyOBJECT_OPTION_S)
      {
        char * tmp = oyFilterRegistrationToText( option_a->registration,
                                                 oyFILTER_REG_OPTION, 0 );
        if(oyStrcmp_( tmp, key) == 0)
        {
          if(option_a->value_type == oyVAL_STRING)
          {
            text = option_a->value->string;

            if(text && oyStrlen_( text ))
              if(!value ||
                 (value && oyStrstr_(value, text)))
                found = 1;
          } else if(option_a->value_type == oyVAL_STRING_LIST)
          {
            j = 0;

            while(option_a->value->string_list[j])
            {
              text = option_a->value->string_list[j];

              if(text && oyStrlen_( text ))
                if(!value ||
                   (value && oyStrstr_(value, text)))
                  found = 1;

              if(found) break;

              ++j;
            }
          }
        }
        oyFree_m_( tmp );
      }

      oyOption_Release( &option_a );

      error = !found;
    }
  }

  return text;
}

/** Function oyOptions_SetFromText
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param         obj                 the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                  "share/freedesktop.org/colour/my_app/my_opt"
 *  @param         value               the value to set
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     or OY_STRING_LIST
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2009/01/20
 */
int            oyOptions_SetFromText ( oyOptions_s       * obj,
                                       const char        * registration,
                                       const char        * value,
                                       uint32_t            flags )
{
  int error = !obj || obj->type_ != oyOBJECT_OPTIONS_S || !registration ||
              !value;
  oyOption_s * o = 0;

  if(!error)
  {
    o = oyOptions_Find( obj, registration );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if(!o && oyToCreateNew_m(flags))
    {
      o = oyOption_New( obj->oy_, registration );
      error = !o;

      if(!error)
        /** Flags are passed on to oyOption_SetFromText, e.g. OY_STRING_LIST. */
        error = oyOption_SetFromText( o, value, flags );

      oyOptions_MoveIn( obj, &o, -1 );
    }

    oyOption_SetFromText( o, value, 0 );
    oyOption_Release( &o );
  }

  return error;
}

/**
 *  @} *//* objects_value
 */


/** @} *//* misc */


/** \addtogroup objects_profile Profile API

 *  @{
 */


/** @internal
 *  @memberof oyProfile_s
 *  @brief   create a empty profile
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyProfile_s *
oyProfile_New_ ( oyObject_s        object)
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PROFILE_S;
# define STRUCT_TYPE oyProfile_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyProfile_Copy;
  s->release = (oyStruct_Release_f) oyProfile_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->tags_ = oyStructList_New( 0 );

  return s;
}

/** @internal
 *  @memberof oyProfile_s
 *  @brief   hash for oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
static int
oyProfile_GetHash_( oyProfile_s * s )
{
  int error = 1;
  if(s->block_ && s->size_)
  {
    memset( s->oy_->hash_, 0, OY_HASH_SIZE*2 );
    error = oyProfileGetMD5( s->block_, s->size_, s->oy_->hash_ );
    if(error)
      memset(s->oy_->hash_, 0, OY_HASH_SIZE*2 );
  }
  return error;
}


oyProfile_s ** oy_profile_s_std_cache_ = 0;

/** @brief   create from default colour space settings
 *  @memberof oyProfile_s
 *
 *  @param[in]    type           default colour space or zero to detect display?
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyProfile_s *
oyProfile_FromStd     ( oyPROFILE_e       type,
                        oyObject_s        object)
{
  oyProfile_s * s = 0;
  char * name = 0;
  oyAlloc_f allocateFunc = 0;
  int pos = type - oyDEFAULT_PROFILE_START;

  if(!oy_profile_s_std_cache_)
  {
    int len = sizeof(oyProfile_s*) *
                            (oyDEFAULT_PROFILE_END - oyDEFAULT_PROFILE_START);
    oy_profile_s_std_cache_ = oyAllocateFunc_( len );
    memset( oy_profile_s_std_cache_, 0, len );
  }

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    if(oy_profile_s_std_cache_[pos])
      return oyProfile_Copy( oy_profile_s_std_cache_[pos], 0 );

  if(object)
    allocateFunc = object->allocateFunc_;

  if(type)
    name = oyGetDefaultProfileName ( type, allocateFunc );

  s = oyProfile_FromFile_( name, 0, object );

  if(s)
    s->use_default_ = type;

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    oy_profile_s_std_cache_[pos] = oyProfile_Copy( s, 0 );

  oyProfile_GetID( s );

  return s;
}


oyStructList_s * oy_profile_s_file_cache_ = 0;

/**
 *  @internal
 *  @brief   create from file
 *  @memberof oyProfile_s
 *
 *  @param[in]    name           profile file name or zero to detect display?
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  flags supports OY_NO_CACHE_READ and OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *
 *  @since Oyranos: version 0.1.9
 *  @date  november 2007 (API 0.1.9)
 */
oyProfile_s *  oyProfile_FromFile_   ( const char        * name,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyProfile_s * s = 0;
  int error = 0;
  size_t size = 0;
  oyPointer block = 0;
  oyAlloc_f allocateFunc = 0;
  oyHash_s * entry = 0;
  char * file_name = 0;

  if(object)
    allocateFunc = object->allocateFunc_;

  if(!oyToNoCacheRead_m(flags) || !oyToNoCacheWrite_m(flags))
  {
    if(!oy_profile_s_file_cache_)
      oy_profile_s_file_cache_ = oyStructList_New( 0 );

    if(!object)
    {
      entry = oyCacheListGetEntry_ ( oy_profile_s_file_cache_, name );

      if(!oyToNoCacheRead_m(flags))
      {
        s = (oyProfile_s*) oyHash_GetPointer_( entry, oyOBJECT_PROFILE_S);
        s = oyProfile_Copy( s, 0 );
        if(s)
          return s;
      }
    }
  }

  if(!error && name && !s)
  {
    file_name = oyFindProfile_( name );
    block = oyGetProfileBlock( file_name, &size, allocateFunc );
    if(!block || !size)
      error = 1;
  }

  if(!error)
  {
    s = oyProfile_FromMemMove_( size, &block, 0, object );

    if(!s)
      error = 1;

    /* We expect a incomplete filename attached to s and try to correct this. */
    if(!error && !file_name && s->file_name_)
    {
      file_name = oyFindProfile_( s->file_name_ );
      if(file_name && s->oy_->deallocateFunc_)
      {
        s->oy_->deallocateFunc_( s->file_name_ );
        s->file_name_ = 0;
      }
    }

    if(!error && file_name)
    {
      s->file_name_ = oyStringCopy_( file_name, s->oy_->allocateFunc_ );
      oyDeAllocateFunc_( file_name ); file_name = 0;
    }

    if(!error && !s->file_name_)
      error = 1;
  }

  if(!error && s && entry)
  {
    if(!oyToNoCacheWrite_m(flags))
    {
      /* 3b.1. update cache entry */
      error = oyHash_SetPointer_( entry, (oyStruct_s*) oyProfile_Copy( s, 0 ) );
#if 0
    } else {
      int i = 0, n = 0, pos = -1;

      n = oyStructList_Count( oy_profile_s_file_cache_ );
      for( i = 0; i < n; ++i )
        if((oyStruct_s*)entry == oyStructList_Get_( oy_profile_s_file_cache_,i))
          pos = i;
      if(pos >= 0)
        oyStructList_ReleaseAt( oy_profile_s_file_cache_, pos );
#endif
    }
  }

  oyHash_Release_( &entry );

  return s;
}

/** @brief   create from file
 *  @memberof oyProfile_s
 *
 *  @param[in]    name           profile file name or zero to detect display?
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  flags supports OY_NO_CACHE_READ and OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromFile            ( const char      * name,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;

  s = oyProfile_FromFile_( name, flags, object );

  oyProfile_GetID( s );

  return s;
}

/**
 *  @internal
 *  @brief   create from in memory blob
 *  @memberof oyProfile_s
 *
 *  @param[in]    size           buffer size
 *  @param[in]    block          pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyProfile_s* oyProfile_FromMemMove_  ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       oyObject_s          object)
{
  oyProfile_s * s = oyProfile_New_( object );
  int error = 0;
  oyGetMonitorProfile_f funcP = 0;
  oyGetMonitorProfileName_f funcP2 = 0;
  char cmm[] = {"oyX1"};

  if(block  && *block && size)
  {
    s->block_ = *block;
    *block = 0;
    if(!s->block_)
      error = 1;
    else
      s->size_ = size;
  }

  if (!s->block_)
  {
    if(!error)
    {
      oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
      if(api && *(uint32_t*)&cmm)
      {
        oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
        funcP = api2->oyGetMonitorProfile;
        funcP2= api2->oyGetMonitorProfileName;
      }
      /*error = !funcP || !funcP2;*/
    }

    if(funcP)
      s->block_ = funcP( NULL, &s->size_, s->oy_->allocateFunc_ );

    if(!error && (!s->block_ || !s->size_))
    {
      s->file_name_ = oyGetDefaultProfileName( oyASSUMED_WEB,
                                               s->oy_->allocateFunc_ );
      if(!error && s->file_name_)
      {
        s->block_ = oyGetProfileBlock( s->file_name_, &s->size_,
                                       s->oy_->allocateFunc_ );
        if(!s->block_ || !s->size_)
        {
          error = 1;
          WARNc_S("Did not find monitor profile; nor a substitute.")
        } else
          WARNc_S("Did not find monitor profile; use sRGB instead.")
      }

    } else if(funcP2)
      s->file_name_ = funcP2( NULL, s->oy_->allocateFunc_ );
  }

  /* Comparision strategies
      A
       - search for similiar arguments in the structure
       - include the affect of the arguments (resolve type -> filename)
       - exclude paralell structure elements (filename -> ignore blob)
      B
       - use always the ICC profiles md5
      C
       - always the flattened Oyranos profile md5

       - A higher level API can maintain its own cache depending on costs.
   */

  if(!error)
    error = oyProfile_GetHash_( s );

  if(error)
  {
    WARNc1_S( "hash error %d", error )
    return 0;
  }

  if(!error)
    error = !oyProfile_GetSignature ( s, oySIGNATURE_COLOUR_SPACE );

  if(error)
  {
    WARNc1_S( "signature error %d", error )
    return 0;
  }

  if(!error)
  {
    s->names_chan_ = 0;
    s->channels_n_ = 0;
    s->channels_n_ = oyProfile_GetChannelsCount( s );
    error = (s->channels_n_ <= 0);
  }

  if(error)
  {
    icHeader *h = 0;
    icSignature sig = 0;

    h = (icHeader*) s->block_;

    sig = oyValueCSpaceSig( h->colorSpace );

    WARNc3_S("Channels <= 0 %d %s %s", s->channels_n_, oyICCColourSpaceGetName(sig), oyICCColourSpaceGetName(h->colorSpace))
  }

  return s;
}

/** @brief   create from in memory blob
 *  @memberof oyProfile_s
 *
 *  @param[in]    size           buffer size
 *  @param[in]    block          pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromMem             ( size_t            size,
                                const oyPointer   block,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;
  int error = 0;
  oyPointer block_ = 0;
  size_t size_ = 0;

  if(block && size)
  {
    block_ = oyAllocateWrapFunc_( size, object ? object->allocateFunc_:0 );
    if(!block_)
      error = 1;
    else
    {
      size_ = size;
      error = !memcpy( block_, block, size );
    }
  }

  s = oyProfile_FromMemMove_( size_, &block_, flags, object );

  oyProfile_GetID( s );

  return s;
}


/** @brief   create a fractional profile from signature
 *  @memberof oyProfile_s
 *
 *  @param[in]    sig            signature
 *  @param[in]    type           type of signature to set
 *  @param[in]    object         the optional base
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
             oyProfile_FromSignature(  icSignature         sig,
                                       oySIGNATURE_TYPE_e  type,
                                       oyObject_s          object )
{
  oyProfile_s * s = oyProfile_New_( object );
  int error = !s;

  if(!error)
    error = oyProfile_SetSignature( s, sig, type );

  return s;
}

/**
 *  @internal
 *  @brief   create new from existing profile struct
 *  @memberof oyProfile_s
 *
 *  @param[in]    profile        other profile
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_Copy_            ( const oyProfile_s  * profile,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc = 0;

  if(!profile)
    return s;

  s = oyProfile_New_( object );
  allocateFunc = s->oy_->allocateFunc_;

  if(profile->block_ && profile->size_)
  {
    s->block_ = s->oy_->allocateFunc_( profile->size_ );
    if(!s->block_)
      error = 1;
    else
    {
      s->size_ = profile->size_;
      error = !memcpy( s->block_, profile->block_, profile->size_ );
    }
  }

  if(!error)
  {
    if(!oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s );
  }

  if(!error)
  {
    if(profile->sig_)
      s->sig_ = profile->sig_;
    else
      error = !oyProfile_GetSignature ( s, oySIGNATURE_COLOUR_SPACE );
  }

  if(!error)
    s->file_name_ = oyStringCopy_( profile->file_name_, allocateFunc );

  if(!error)
    s->use_default_ = profile->use_default_;

  if(!error)
  {
    s->channels_n_ = oyProfile_GetChannelsCount( s );
    error = (s->channels_n_ <= 0);
  }

  if(!error)
    oyProfile_SetChannelNames( s, profile->names_chan_ );

  if(error)
  {
    WARNc_S("Could not create structure for profile.")
  }

  return s;
}

/** @brief   copy from existing profile struct
 *  @memberof oyProfile_s
 *
 *  @param[in]    profile        other profile
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_Copy                ( oyProfile_s     * profile,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;

  if(!profile)
    return s;

  if(profile && !object)
  {
    s = profile;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyProfile_Copy_( profile, object );
  /* TODO cache */
  return s;
}

/** @brief   release correctly
 *  @memberof oyProfile_s
 *
 *  set pointer to zero
 *
 *  @param[in]     obj                 address of Oyranos struct pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int 
oyProfile_Release( oyProfile_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyProfile_s * s = 0;
  int i;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_PROFILE_S)
  {
    WARNc_S("Attempt to release a non oyProfile_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->names_chan_)
    for(i = 0; i < s->channels_n_; ++i)
      if(s->names_chan_[i])
        oyObject_Release( &s->names_chan_[i] );
  /*oyOptions_Release( s->options );*/

  s->sig_ = (icColorSpaceSignature)0;

  oyStructList_Release(&s->tags_);

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->names_chan_)
      deallocateFunc( s->names_chan_ ); s->names_chan_ = 0;

    s->oy_->hash_[0] = 0;

    if(s->block_)
      deallocateFunc( s->block_ ); s->block_ = 0; s->size_ = 0;

    if(s->file_name_)
      deallocateFunc( s->file_name_ ); s->file_name_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @brief   number of channels in a colour space
 *  @memberof oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int
oyProfile_GetChannelsCount( oyProfile_s * profile )
{
  if(!profile)
    return 0;

  if(profile->channels_n_)
    return profile->channels_n_;

  profile->channels_n_ = oyICCColourSpaceGetChannelCount( profile->sig_ );

  return profile->channels_n_;
}

/** @brief   get ICC colour space signature
 *  @memberof oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI icSignature OYEXPORT
oyProfile_GetSignature ( oyProfile_s * s,
                         oySIGNATURE_TYPE_e type )
{
  icHeader *h = 0;
  icSignature sig = 0;

  if(!s)
    return 0;

  if(s->sig_ && type == oySIGNATURE_COLOUR_SPACE)
    return s->sig_;

  if(!s->block_)
  {
    if(type == oySIGNATURE_COLOUR_SPACE)
      sig = s->sig_ = icSigXYZData;
    return sig;
  }

  h = (icHeader*) s->block_;

  switch(type)
  {
  case oySIGNATURE_COLOUR_SPACE:       /* colour space */
       sig = s->sig_ = oyValueCSpaceSig( h->colorSpace ); break;
  case oySIGNATURE_PCS:                /* profile connection space */
       sig = oyValueCSpaceSig( h->pcs ); break;
  case oySIGNATURE_SIZE:               /* internal stored size */
       sig = oyValueUInt32( h->size ); break;
  case oySIGNATURE_CMM:                /* prefered CMM */
       sig = oyValueUInt32( h->cmmId ); break;
  case oySIGNATURE_VERSION:            /* version */
       sig = oyValueUInt32( h->version ); break;
  case oySIGNATURE_CLASS:              /* usage class 'mntr' ... */
       sig = oyValueUInt32( h->deviceClass ); break;
  case oySIGNATURE_MAGIC:              /* magic; ICC: 'acsp' */
       sig = oyValueUInt32( h->magic ); break;
  case oySIGNATURE_PLATFORM:           /* operating system */
       sig = oyValueUInt32( h->platform ); break;
  case oySIGNATURE_OPTIONS:            /* various ICC header flags */
       sig = oyValueUInt32( h->flags ); break;
  case oySIGNATURE_MANUFACTURER:       /* device manufacturer */
       sig = oyValueUInt32( h->manufacturer ); break;
  case oySIGNATURE_MODEL:              /* device modell */
       sig = oyValueUInt32( h->model ); break;
  case oySIGNATURE_INTENT:             /* seldom used profile claimed intent*/
       sig = oyValueUInt32( h->renderingIntent ); break;
  case oySIGNATURE_CREATOR:            /* profile creator ID */
       sig = oyValueUInt32( h->creator ); break;
  case oySIGNATURE_DATETIME_YEAR:      /* creation time in UTC */
       sig = oyValueUInt16( h->date.year ); break;
  case oySIGNATURE_DATETIME_MONTH:     /* creation time in UTC */
       sig = oyValueUInt16( h->date.month ); break;
  case oySIGNATURE_DATETIME_DAY:       /* creation time in UTC */
       sig = oyValueUInt16( h->date.day ); break;
  case oySIGNATURE_DATETIME_HOURS:     /* creation time in UTC */
       sig = oyValueUInt16( h->date.hours ); break;
  case oySIGNATURE_DATETIME_MINUTES:   /* creation time in UTC */
       sig = oyValueUInt16( h->date.minutes ); break;
  case oySIGNATURE_DATETIME_SECONDS:   /* creation time in UTC */
       sig = oyValueUInt16( h->date.seconds ); break;
  case oySIGNATURE_MAX: break;
  }

  return sig;
}

/** @brief   set signature
 *  @memberof oyProfile_s
 *
 *  @param[in]     profile             the profile
 *  @param[in]     sig                 signature
 *  @param[in]     type                type of signature to set
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type )
{
  oyProfile_s * s = profile;
  int error = 0;
  oyPointer block_ = 0;
  size_t size_ = 128;
  icHeader *h = 0;

  if(!error)
    error = !s;

  if(!error && type == oySIGNATURE_COLOUR_SPACE)
  {
    if(sig)
      s->sig_ = sig;
    else
      s->sig_ = icSigXYZData;
    return error;
  }

  if(!error && !s->block_)
  {
    block_ = oyAllocateWrapFunc_( size_, s->oy_ ? s->oy_->allocateFunc_:0 );
    if(!block_)
      error = 1;

    if(!error)
      error = !memset( block_, 0, size_ );

    if(!error)
    {
      s->block_ = block_;
      s->size_ = size_;
    }
  }

  if(!error)
    h = (icHeader*) s->block_;

  if(!error)
  switch(type)
  {
  case oySIGNATURE_COLOUR_SPACE:       /* colour space */
       h->colorSpace = oyValueCSpaceSig( s->sig_ ); break;
  case oySIGNATURE_PCS:                /* profile connection space */
       h->pcs = oyValueCSpaceSig( sig ); break;
  case oySIGNATURE_SIZE:               /* internal stored size */
       h->size = oyValueUInt32( sig ); break;
  case oySIGNATURE_CMM:                /* prefered CMM */
       h->cmmId = oyValueUInt32( sig ); break;
  case oySIGNATURE_VERSION:            /* version */
       h->version = oyValueUInt32( sig ); break;
  case oySIGNATURE_CLASS:              /* usage class 'mntr' ... */
       h->deviceClass = oyValueUInt32( sig ); break;
  case oySIGNATURE_MAGIC:              /* magic; ICC: 'acsp' */
       h->magic = oyValueUInt32( sig ); break;
  case oySIGNATURE_PLATFORM:           /* operating system */
       h->platform = oyValueUInt32( sig ); break;
  case oySIGNATURE_OPTIONS:            /* various ICC header flags */
       h->flags = oyValueUInt32( sig ); break;
  case oySIGNATURE_MANUFACTURER:       /* device manufacturer */
       h->manufacturer = oyValueUInt32( sig ); break;
  case oySIGNATURE_MODEL:              /* device modell */
       h->model = oyValueUInt32( sig ); break;
  case oySIGNATURE_INTENT:             /* seldom used profile claimed intent*/
       h->renderingIntent = oyValueUInt32( sig ); break;
  case oySIGNATURE_CREATOR:            /* profile creator ID */
       h->creator = oyValueUInt32( sig ); break;
  case oySIGNATURE_DATETIME_YEAR:      /* creation time in UTC */
       h->date.year = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_MONTH:     /* creation time in UTC */
       h->date.month = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_DAY:       /* creation time in UTC */
       h->date.day = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_HOURS:     /* creation time in UTC */
       h->date.hours = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_MINUTES:   /* creation time in UTC */
       h->date.minutes = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_SECONDS:   /* creation time in UTC */
       h->date.seconds = oyValueUInt16( sig ); break;
  case oySIGNATURE_MAX: break;
  }

  return error;
}

/** @brief   set channel names
 *  @memberof oyProfile_s
 *
 *  The function should be used to specify extra channels or unusual colour
 *  layouts like CMYKRB. The number of elements in names_chan should fit to the
 *  channels count or to the colour space signature.
 *
 *  You can let single entries empty if they are understandable by the
 *  colour space signature. Oyranos will set them for you on request.
 *
 *  @param[in]     profile             profile
 *  @param[in]     names_chan          pointer to channel names 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
void
oyProfile_SetChannelNames            ( oyProfile_s       * profile,
                                       oyObject_s        * names_chan )
{
  oyProfile_s * c = profile;
  int n = oyProfile_GetChannelsCount( profile );

  if(names_chan && n)
  {
    int i = 0;
    c->names_chan_ = c->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    c->names_chan_[ n ] = NULL;
    for( ; i < n; ++i )
      if(names_chan[i])
        c->names_chan_[i] = oyObject_Copy( names_chan[i] );
  }
}

/** @brief   get a channels name
 *  @memberof oyProfile_s
 *
 *  A convinience function to get a single name with a certain type.
 *
 *  @param[in] profile  address of a Oyranos named colour structure
 *  @param[in] pos      position of channel 
 *  @param[in] type     sort of text 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
const oyChar *
oyProfile_GetChannelName           ( oyProfile_s   * profile,
                                        int                pos,
                                        oyNAME_e           type )
{
  oyProfile_s * s = profile;
  int n = oyProfile_GetChannelsCount( s );
  const oyChar * text = 0;

  if(!s)
    return 0;

  if( 0 <= pos && pos < n )
    return "-";

  if(!s->names_chan_)
    oyProfile_GetChannelNames ( s );

  if(s->names_chan_ && s->names_chan_[pos])
    text = oyObject_GetName( s->names_chan_[pos], type );

  return text;
}

/** @brief   get channel names
 *  @memberof oyProfile_s
 *
 *  @param[in]     profile             the profile
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
const oyObject_s *
oyProfile_GetChannelNames           ( oyProfile_s   * profile )
{
  oyProfile_s * s = profile;
  int n = oyProfile_GetChannelsCount( profile );
  int error = 0;
  icColorSpaceSignature sig = oyProfile_GetSignature( profile, oySIGNATURE_COLOUR_SPACE );

  if(!profile)
    return 0;

  if(!s->names_chan_ && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    if(!s->names_chan_)
      error = 1;
    if(!error)
    {
      s->names_chan_[ n ] = NULL;
      for( ; i < n; ++i )
      {
        s->names_chan_[i] = oyObject_NewFrom( s->oy_ );
        if(!s->names_chan_[i])
          error = 1;
        else
          error = oyObject_SetNames( s->names_chan_[i],
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_NICK ),
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_NAME ),
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_DESCRIPTION )
                      );
      }
    }
  }

  if(!error && s->names_chan_)
    return (const oyObject_s*) s->names_chan_;
  else
    return 0;
}

/** @brief   get unique name
 *  @memberof oyProfile_s
 *
 *  the returned string is identical to oyNAME_ID
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * s )
{
  int error = !s;
  const oyChar * text = 0;

  if(!error)
    text = oyObject_GetName( s->oy_, oyNAME_NICK );

  if(!error && !text)
  {
    oyChar * temp = 0;
    int found = 0;

    oyAllocHelper_m_( temp, oyChar, 1024, 0, error = 1 );

    /* A short number represents a default profile,
     * The length should not exceed OY_HASH_SIZE.
     */
    if(s->use_default_ && !found && !error)
    {
      oySprintf_(temp, "%d", s->use_default_);
      if(oyStrlen_(temp) < OY_HASH_SIZE)
        found = 1;
    }

    /* Do we have a file_name_? */
    if(s->file_name_ && !found && !error)
    {
      oySprintf_(temp, "%s", s->file_name_);
      if(oyStrlen_(temp))
        found = 1;
    }

    /* Do we have a hash_? */
    if(!found && !error)
    {
      if(!oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s );

      if(!error)
      {
        uint32_t * i = (uint32_t*)&s->oy_->hash_[0];
        oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
        if(oyStrlen_(temp))
          found = 1;
      }
    }

    if(!error && !found)
      error = 1;

    if(!error)
      error = oyObject_SetName( s->oy_, temp, oyNAME_NICK );

    oyFree_m_( temp );

    if(!error)
      text = oyObject_GetName( s->oy_, oyNAME_NICK );
  }

  return text;
}

/** @brief   get a presentable name
 *  @memberof oyProfile_s
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XML element
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - profile internal name (icSigProfileDescriptionTag)
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/06/23
 */
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetText ( oyProfile_s       * s,
                                       oyNAME_e            type )
{
  int error = !s;
  const char * text = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  oyProfileTag_s * tag = 0;


  if(!error && type == oyNAME_NICK)
    text = oyProfile_GetID( s );

  if(!error && !text)
    if(type <= oyNAME_DESCRIPTION)
      text = oyObject_GetName( s->oy_, type );

  if(!error && !(text && oyStrlen_(text)))
  {
    char * temp = 0,
         * tmp2 = 0;
    int found = 0;

    oyAllocHelper_m_( temp, char, 1024, 0, error = 1 );

    /* Ask the CMM? */
    if(!found && !error &&
       type == oyNAME_DESCRIPTION)
    {
      {
        tag = oyProfile_GetTagById( s, icSigProfileDescriptionTag );
        texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);

        if(texts_n && texts[0] && oyStrlen_(texts[0]))
        {
          memcpy(temp, texts[0], oyStrlen_(texts[0]));
          temp[oyStrlen_(texts[0])] = 0;
          found = 1;

          oyStringListRelease_( &texts, texts_n, tag->oy_->deallocateFunc_ );
        } else
          /* we try to get something as oyNAME_NAME */
        if(s->file_name_ && oyStrlen_(s->file_name_))
        {
          size_t len = oyStrlen_(s->file_name_);
          if(strrchr(s->file_name_,'/'))
          {
            tmp2 = oyStrrchr_(s->file_name_,'/')+1;
            len = oyStrlen_( tmp2 );
            memcpy( temp, tmp2, len );
          } else
            memcpy( temp, s->file_name_, len );
          temp[len] = 0;
          found = 1;
        }
      }
    }

    if(type == oyNAME_NAME)
    {
      uint32_t * i = (uint32_t*)&s->oy_->hash_[0];
      char * file_name = oyProfile_GetFileName_r( s, oyAllocateFunc_ );

      if(oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s );

      if(s->use_default_ && !error)
        oyWidgetTitleGet( (oyWIDGET_e)s->use_default_, 0, &text, 0, 0 );

      oySprintf_( temp, "<profile use_default=\"%s\" file_name=\"%s\" hash=\"%x%x%x%x\" />",
             oyNoEmptyName_m_(text),
             oyNoEmptyName_m_(file_name),
             i[0], i[1], i[2], i[3] );

      if(file_name) free(file_name); file_name = 0;
      found = 1;
    }

    if(!found)
    {
      text = oyProfile_GetID( s );
      if(oyStrlen_(text))
        found = 1;
    }

    /* last rescue */
    if(!found && oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s );

    if(!found && !error)
    {
      uint32_t * i = (uint32_t*)&s->oy_->hash_[0];
      oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
      if(oyStrlen_(temp))
        found = 1;
    }

    if(!error && !found)
      error = 1;

    if(!error)
      error = oyObject_SetName( s->oy_, temp, type );

    oyFree_m_( temp );

    if(!error)
      text = oyObject_GetName( s->oy_, type );
  }

  return text;
}

/** Function oyProfile_GetMem
 *  @memberof oyProfile_s
 *  @brief   get the ICC profile in memory
 *
 *  The prefered memory comes from a previously created tag list.
 *  Otherwise the memory block is copied.
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer block = 0;
  oyProfile_s * s = profile;
  int error = !s;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(oyStructList_Count( s->tags_ ))
    {
      block = oyProfile_TagsToMem_ ( profile, size, allocateFunc );

    } else
    if(s->size_ && s->block_)
    {

      block = oyAllocateWrapFunc_( s->size_, allocateFunc );
      error = !block;
      if(!error)
        error = !memcpy( block, s->block_, s->size_ );
      if(!error && size)
        *size = s->size_;
    }
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return block;
}

/**
 *  @internal
 *  Function oyProfile_GetFileName_r
 *  @memberof oyProfile_s
 *  @brief   get the ICC profile location in the filesystem
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or as a
 *  from Oyranos defaults the associated filename will simply be retuned.
 *
 *  @param         profile             the profile
 *  @param         allocateFunc        memory allocator           
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
char *       oyProfile_GetFileName_r ( oyProfile_s       * profile,
                                       oyAlloc_f           allocateFunc )
{
  char * name = 0;
  oyProfile_s * s = profile, * tmp = 0;
  int error = !s;
  oyChar ** names = 0;
  uint32_t count = 0, i = 0;
  oyChar *  hash = 0;
  oyChar    tmp_hash[34];

  if(!error && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(s->file_name_ && !hash)
    {
      name = s->file_name_;
    } else {

      names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

      for(i = 0; i < count; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );

          if(hash && tmp)
          {
            uint32_t * h = (uint32_t*)&tmp->oy_->hash_[0];
            oySprintf_(tmp_hash, "%x%x%x%x", h[0], h[1], h[2], h[3]);
            if(memcmp( hash, tmp_hash, 2*OY_HASH_SIZE ) == 0 )
            {
              name = names[i];
              break;
            }
          } else
          if(oyProfile_Equal( s, tmp ))
          {
            name = names[i];
            break;
          }

          oyProfile_Release( &tmp );
        }
      }

      name = oyFindProfile_( name );
      oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }
  }

  name = oyStringCopy_( s->file_name_, allocateFunc );

  return name;
}
/** Function oyProfile_GetFileName
 *  @memberof oyProfile_s
 *  @brief   get the ICC profile location in the filesystem
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or as a
 *  from Oyranos defaults the associated filename will simply be retuned.
 *
 *  @param         profile             the profile
 *  @param         dl_pos              -1, or the position in a device links source chain
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
const oyChar *     oyProfile_GetFileName (
                                       oyProfile_s       * profile,
                                       int                 dl_pos )
{
  const oyChar * name = 0;
  oyProfile_s * s = profile, * tmp = 0;
  int error = !s;
  oyChar ** names = 0;
  uint32_t count = 0, i = 0;
  oyProfileTag_s * psid = 0;
  oyChar ** texts = 0;
  int32_t   texts_n = 0;
  oyChar *  hash = 0;
  oyChar    tmp_hash[34];
  int       dl_n = 0;

  if(!error && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(dl_pos >= 0)
    {
      psid = oyProfile_GetTagById( s, icSigProfileSequenceIdentifierTag );
      texts = oyProfileTag_GetText( psid, &texts_n, 0,0,0,0);

      if(texts && texts_n > 1+dl_pos*5+2)
      {
        dl_n = 1 + dl_pos*5+2;
        hash = texts[dl_n];
      }
    }


    if(s->file_name_ && !hash)
    {
      name = s->file_name_;
    } else
    {
      names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

      for(i = 0; i < count; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );

          if(hash && tmp)
          {
            uint32_t * h = (uint32_t*)&tmp->oy_->hash_[0];
            oySprintf_(tmp_hash, "%x%x%x%x", h[0], h[1], h[2], h[3]);
            if(memcmp( hash, tmp_hash, 2*OY_HASH_SIZE ) == 0 )
            {
              name = names[i];
              break;
            }
          } else
          if(oyProfile_Equal( s, tmp ))
          {
            name = names[i];
            break;
          }

          oyProfile_Release( &tmp );
        }
      }
      s->file_name_ = oyFindProfile_( name );
      name = oyStringCopy_( s->file_name_, s->oy_->allocateFunc_ );
      oyDeAllocateFunc_( s->file_name_ );
      s->file_name_ = (char*)name;

      if(names)
        oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }

    if(texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  return name;
}


#if 0
/** @brief get a CMM specific pointer
 *  @memberof oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
/*
oyChar *       oyProfile_GetCMMText_ ( oyProfile_s       * profile,
                                       oyNAME_e            type,
                                       const char        * language,
                                       const char        * country )
{
  oyProfile_s * s = profile;
  int error = !s;
  oyChar * name = 0;
  char cmm_used[] = {0,0,0,0,0};

  if(!error)
  {
    oyCMMProfile_GetText_t funcP = 0;
    oyCMMptr_s  * cmm_ptr = 0;


    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API1_S,
                                      0, 0, cmm_used );
    if(api && *(uint32_t*)&cmm_used)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMProfile_GetText;
    }

    if(*(uint32_t*)&cmm_used)
      cmm_ptr = oyProfile_GetCMMPtr_( s, cmm_used );

    if(funcP && cmm_ptr)
    {
      name = funcP(cmm_ptr, type, language, country, s->oy_->allocateFunc_);

      oyCMMdsoRelease_( cmm_used );
    }
  }

  return name;
}
*/
#endif

/**
 *  @internal
 *  Function oyProfile_Equal
 *  @memberof oyProfile_s
 *  @brief   check if two profiles are qual by their hash sum
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/03
 *  @since   2008/02/03 (Oyranos: 0.1.8)
 */
OYAPI int OYEXPORT
                   oyProfile_Equal   ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB )
{
  int equal = 0;

  if(profileA && profileB)
  {
    equal = memcmp( profileA->oy_->hash_, profileB->oy_->hash_, OY_HASH_SIZE )
            == 0;
  }

  return equal;
}

/** @internal
 *  Function oyProfile_Match_
 *  @memberof oyProfile_s
 *  @brief   check if a profiles matches by some properties
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Match_        ( oyProfile_s       * pattern,
                                       oyProfile_s       * profile )
{
  int32_t match = 0;
  int i;
  icSignature pattern_sig, profile_sig;

  if(pattern && profile)
  {
    /*match = oyProfile_Equal_(pattern, profile);*/ /* too expensive */
    if(!match)
    {
      match = 1;
      for( i = 0; i < (int)oySIGNATURE_MAX; ++i)
      {
        pattern_sig = oyProfile_GetSignature( pattern, (oySIGNATURE_TYPE_e) i );
        profile_sig = oyProfile_GetSignature( profile, (oySIGNATURE_TYPE_e) i );

        if(pattern_sig && profile_sig && pattern_sig != profile_sig)
          match = 0;
      }
    }
  }

  return match;
}

/** @internal
 *  Function oyProfile_Hashed_
 *  @memberof oyProfile_s
 *  @brief   check if a profile has a hash sum computed
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Hashed_       ( oyProfile_s       * s )
{
  if(s && s->type_ == oyOBJECT_PROFILE_S)
    return oyObject_Hashed_( s->oy_ );
  else
    return 0;
}

/** @internal
 *  @memberof oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  20 december 2007 (API 0.1.8)
 */
int          oyProfile_ToFile_       ( oyProfile_s       * profile,
                                       const char        * file_name )
{
  oyProfile_s * s = profile;
  int error = !s || !file_name;
  oyPointer buf = 0;
  size_t size = 0;

  if(!error)
  {
    buf = oyProfile_GetMem ( s, &size, 0, 0 );
    if(buf && size)
    error = oyWriteMemToFile_( file_name, buf, size );

    if(buf) oyDeAllocateFunc_(buf);
    size = 0;
  }

  return error;
}

/**
 *  @internal
 *  Function oyProfile_WriteHeader_
 *  @memberof oyProfile_s
 *  @brief   get the parsed ICC profile back into memory
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteHeader_  ( oyProfile_s       * profile,
                                       size_t            * size )
{
  oyPointer block = 0;
  char h[5] = {"head"};
  uint32_t * hi = (uint32_t*)&h;

  if(profile && profile->block_ && profile->size_ > 132 &&
     profile->tags_)
  {
    int n = oyProfile_GetTagCount( profile );
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( profile, 0 );

    if(n && tag->use == *hi && tag->block_ && tag->size_ >= 128)
    {
      block = oyAllocateFunc_ (132);
      if(block)
        memcpy( block, tag->block_, 128 );
    }

    oyProfileTag_Release( &tag );
  }

  return block;
}

/**
 *  @internal
 *  Function oyProfile_WriteTagTable_
 *  @memberof oyProfile_s
 *  @brief   get the parsed ICC profile back into memory
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTagTable_( oyProfile_s       * profile,
                                       size_t            * size )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(!error)
  {
    int n = oyProfile_GetTagCount( profile );
    size_t size = 0;

    size = sizeof (icTag) * n;
    error = !size;

    if(!error)
    {
      block = oyAllocateFunc_( size );
      error = !block;
    }

    if(!error)
      error = !memset( block, 0, size );
  }

  return block;
}

/**
 *  @internal
 *  Function oyProfile_WriteTags_
 *  @memberof oyProfile_s
 *  @brief   get the parsed ICC profile back into memory
 *
 *  Call in following order:
 *         -  oyProfile_WriteHeader_
 *         -  oyProfile_WriteTagTable_
 *         -  oyProfile_WriteTags_
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTags_    ( oyProfile_s       * profile,
                                       size_t            * size,
                                       oyPointer           icc_header,
                                       oyPointer           icc_list,
                                       oyAlloc_f           allocateFunc )
{
  char * block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size &&
                icc_header && icc_list);

  if(!error)
  {
    int n = 0, i;
    size_t len = 0;

    n = oyProfile_GetTagCount( profile );
    block = (char*) oyAllocateFunc_(132 + n * sizeof(icTag));
    error = !block;

    if(!error)
      error = !memcpy( block, icc_header, 132 );

    len = 132;

    if(!error)
      error = !memcpy( &block[len], icc_list, (n-1) * sizeof(icTag) );

    len += sizeof(icTag) * (n-1);

    for(i = 0; i < n - 1; ++i)
    {
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char * temp = 0;
      icTagList* list = (icTagList*) &block[128];
      oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( profile, i + 1 );
      size_t size = 0;

      if(!error)
        error = !tag;

      if(!error)
        size = tag->size_;

      if(!error && tag->use == *hi)
      {
        oyProfileTag_Release( &tag );
        continue;
      }

      if(!error)
      {
        list->tags[i].sig = oyValueUInt32( (icTagSignature)tag->use );
        list->tags[i].offset = oyValueUInt32( (icUInt32Number)len );
        list->tags[i].size = oyValueUInt32( (icUInt32Number)size );
        temp = (char*) oyAllocateFunc_ ( len + size + 
                                               (size%4 ? 4 - size%4 : 0));
      }

      if(temp)
        error = !memcpy( temp, block, len );
      if(!error)
      {
        error = !memcpy( &temp[len], tag->block_, tag->size_);
        len += size + (size%4 ? 4 - size%4 : 0);
      }

      if(!error)
      {
        oyDeAllocateFunc_(block);
        block = temp;

        oyProfileTag_Release( &tag );
      }
      temp = 0;
    }

    if(!error)
    {
      char h[5] = {OY_MODULE_NICK};
      uint32_t * hi = (uint32_t*)&h;
      icProfile* p = 0;
      icHeader* header = 0;
      oyPointer temp = oyAllocateWrapFunc_( len, allocateFunc );

      error = !temp;
      if(!error)
        error = !memcpy( temp, block, len );

      oyDeAllocateFunc_( block );
      block = temp; temp = 0;

      p = (icProfile*) block;
      p->count = oyValueUInt32( (icUInt32Number) n - 1);

      header = (icHeader*) block;
      header->size = oyValueUInt32( (icUInt32Number) len);
#if 0 /* we dont override the CMM's id */
      header->creator = *hi;
#endif
#if defined(__APPLE__)
      oySprintf_( h, "APPL" );
#elif defined(WIN32)
      oySprintf_( h, "MSFT" );
#else
      oySprintf_( h, "*nix" );
#endif
      header->platform = *hi;
      *size = len;
    }
  }

  return block;
}

/**
 *  @internal
 *  Function oyProfile_TagsToMem_
 *  @memberof oyProfile_s
 *  @brief   get the parsed ICC profile back into memory
 *
 *  non thread save
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s       * profile,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(!error)
  {
    size_t size_ = 0;

    oyPointer icc_header = 0;
    oyPointer icc_tagtable = 0;

    /* 1. header */
    icc_header = oyProfile_WriteHeader_( profile, &size_ );

    error = !icc_header;

    /* 2. tag table */
    if(!error)
    {
      icc_tagtable = oyProfile_WriteTagTable_( profile, &size_ );
      error = !icc_tagtable;
    }

    /* 3. tags */
    if(!error)
    {
      block = oyProfile_WriteTags_( profile, &size_, icc_header, icc_tagtable,
                                    allocateFunc );
      error = !block;
    }

    if(!error)
    {
      oyDeAllocateFunc_(icc_header);
      oyDeAllocateFunc_(icc_tagtable);
      *size = size_;
    }
  }

  return block;
}

/** Function oyProfile_GetTagById
 *  @memberof oyProfile_s
 *  @brief   get a profile tag by its tag signature
 *
 *  @param[in]     profile             the profile
 *  @param[in]     id                  icTagSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2 january 2008 (API 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagById( oyProfile_s       * profile,
                                       icTagSignature      id )
{
  oyProfile_s * s = profile;
  int error = !s;
  oyProfileTag_s * tag = 0,
                 * tmp = 0;
  int i = 0, n = 0;
  icTagSignature tag_id_ = 0;

  if(!error && profile->type_ != oyOBJECT_PROFILE_S)
    error = 1;

  if(!error)
    n = oyProfile_GetTagCount( s );

  if(!error && n)
  {
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );
    for(i = 0; i < n; ++i)
    {
      tmp = oyProfile_GetTagByPos_( s, i );
      tag_id_ = 0;

      if(tmp)
        tag_id_ = tmp->use;

      if(tag_id_ == id)
      {
        tag = tmp; tmp = 0;
        break;
      } else
        oyProfileTag_Release( &tmp );
    }
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
  }

  return tag;
}


/** Function oyProfile_GetTag
 *  @memberof oyProfile_s
 *  @brief   get a profile tag
 *
 *  non thread save
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s     * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s * s = 0;
  int error = !profile;
  int n = 0;

  if(!error && profile->type_ != oyOBJECT_PROFILE_S)
    error = 1;

  if(!error)
  {
    s = profile;
    n = oyStructList_Count( profile->tags_ );
  }

  if(!error && n)
  {
    tag = (oyProfileTag_s*) oyStructList_GetRef( profile->tags_, pos );
    return tag;
  }

  /* parse the ICC profile struct */
  if(!error && s->block_)
  {
    icSignature magic = oyProfile_GetSignature( s, oySIGNATURE_MAGIC );
    icSignature profile_cmmId = oyProfile_GetSignature( s, oySIGNATURE_CMM );
    char profile_cmm[5] = {0,0,0,0,0};
    icProfile * ic_profile = s->block_;
    int min_icc_size = 132 + sizeof(icTag);

    error = (magic != icMagicNumber);

    profile_cmmId = oyValueUInt32( profile_cmmId );
    error = !memcpy( profile_cmm, &profile_cmmId, 4 );
    profile_cmmId = 0;

    if(!error && s->size_ > min_icc_size)
    {
      uint32_t size = 0;
      uint32_t tag_count = 0;
      icTag *tag_list = 0;
      int i = 0;
      oyProfileTag_s * tag_ = oyProfileTag_New( 0 );
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char *tag_block = 0;

      tag_block = oyAllocateFunc_( 132 );
      error = !memcpy( tag_block, s->block_, 132 );
      error = oyProfileTag_Set( tag_, (icTagSignature)*hi,
                                (icTagTypeSignature)*hi,
                                oyOK, 132, tag_block );
      if(!error)
        error = !memcpy( tag_->profile_cmm_, profile_cmm, 4 );

      if(0 == pos)
        tag = oyProfileTag_Copy( tag_, 0 );
      error = oyProfile_TagMoveIn_( s, &tag_, -1 );


      size = oyProfile_GetSignature( s, oySIGNATURE_SIZE );
      tag_count = oyValueUInt32( ic_profile->count );

      tag_list = (icTag*)&((char*)s->block_)[132];

      /* parse the profile and add tags to the oyProfile_s::tags_ list */
      for(i = 0; i < tag_count; ++i)
      {
        icTag *ic_tag = &tag_list[i];
        size_t offset = oyValueUInt32( ic_tag->offset );
        size_t tag_size = oyValueUInt32( ic_tag->size );
        char *tmp = 0;
#ifdef DEBUG
        oyChar **texts = 0;
        int32_t texts_n = 0;
        int j;
#endif
        oySTATUS_e status = oyOK;
        icTagSignature sig = oyValueUInt32( ic_tag->sig );
        icTagTypeSignature tag_type = 0;

        oyProfileTag_s * tag_ = oyProfileTag_New( 0 );

        tag_block = 0;

        if((offset+tag_size) > s->size_)
          status = oyCORRUPTED;
        else
        {
          icTagBase * tag_base = 0;

          tag_block = oyAllocateFunc_( tag_size );
          tmp = &((char*)s->block_)[offset];
          error = !memcpy( tag_block, tmp, tag_size );

          tag_base = (icTagBase*) tag_block; 
          tag_type = oyValueUInt32( tag_base->sig );
        }

        error = oyProfileTag_Set( tag_, sig, tag_type,
                                  status, tag_size, tag_block );
        tag_->offset_orig = offset;
        if(!error)
          error = !memcpy( tag_->profile_cmm_, profile_cmm, 4 );

#ifdef DEBUG
        if(oy_debug > 3)
        {
          DBG_PROG5_S("%d[%d @ %d]: %s %s", 
            i, (int)tag_->size_, (int)tag_->offset_orig,
            oyICCTagTypeName( tag_->tag_type_ ),
            oyICCTagDescription( tag_->use ) );
          texts = oyProfileTag_GetText(tag_,&texts_n,0,0,0,0);
          for(j = 0; j < texts_n; ++j)
            DBG_PROG2_S("%s: %s", tag_->last_cmm_, texts[j]?texts[j]:"");
          if(texts_n && texts)
            oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
        }
#endif

        if(i == pos-1)
          tag = oyProfileTag_Copy( tag_, 0 );

        if(!error)
          error = oyProfile_TagMoveIn_( s, &tag_, -1 );
      }
    }
  }

  return tag;
}

/** Function oyProfile_GetTagByPos
 *  @memberof oyProfile_s
 *  @brief   get a profile tag
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/06
 *  @since   2008/02/06 (Oyranos: 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagByPos(oyProfile_s       * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s * s = profile;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  tag = oyProfile_GetTagByPos_( s, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return tag;
}

/** Function oyProfile_GetTagCount
 *  @memberof oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
int                oyProfile_GetTagCount( oyProfile_s    * profile )
{
  int n = 0;
  oyProfile_s *s = profile;
  int error = !s;

  if(!error && !(s && s->type_ == oyOBJECT_PROFILE_S && s->tags_))
    error = 1;

  if(!error)
    n = oyStructList_Count( s->tags_ );

  if(!error && !n)
  {
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );
  }

  return n;
}

/** @internal
 *  Function oyProfile_TagMoveIn_
 *  @memberof oyProfile_s
 *  @brief   add a tag to a profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
int          oyProfile_TagMoveIn_    ( oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  int error = !s;

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );


  if(!error)
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function oyProfile_TagMoveIn
 *  @memberof oyProfile_s
 *  @brief   add a tag to a profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2009/01/06
 */
int                oyProfile_TagMoveIn(oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  int error = !s, i,n;
  oyProfileTag_s * tag = 0;

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );


  if(!error)
  {
    /** Initialise tag list. */
    n = oyProfile_GetTagCount( s );

    /** Avoid double occurencies of tags. */
    for( i = 0; i < n; ++i )
    {
      tag = oyProfile_GetTagByPos( s, i );
      if(tag->use == (*obj)->use)
      {
        oyProfile_TagReleaseAt(s, i);
        n = oyProfile_GetTagCount( s );
      }
      oyProfileTag_Release( &tag );
    }
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos );
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function oyProfile_TagReleaseAt
 *  @memberof oyProfile_s
 *  @brief   remove a tag from a profile
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/03/11
 *  @since   2008/03/11 (Oyranos: 0.1.8)
 */
int                oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  int error = 0;

  if(!(s && s->type_ == oyOBJECT_PROFILE_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error)
    error = oyStructList_ReleaseAt ( s->tags_, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}





/** Function oyProfileTag_New
 *  @memberof oyProfileTag_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                   oyProfileTag_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PROFILE_TAG_S;
# define STRUCT_TYPE oyProfileTag_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyProfileTag_Copy;
  s->release = (oyStruct_Release_f) oyProfileTag_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** Function oyProfileTag_Create
 *  @memberof oyProfileTag_s
 *
 *  The API relies on an generic arguments inside a list. The arguments are not
 *  specified here but in the appropriate moduls. This allowes flixibility, 
 *  which needs more understanding.
 *
 *  For the effect of the parameters look at the appropriate module
 *  documentation and the function infos.
 *  @see oyraProfileTag_Create
 *  @see oyraFunctionGetInfo
 *
 *  @param[in]     list                a list of arguments
 *  @param[in]     tag_type            type to create
 *  @param[in]     version             version as supported
 *  @param[in,out] required_cmm        in: CMM to create the tag; out: used CMM
 *  @param[in]     object              the user object for the tag creation
 *  @return                            a profile tag
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/08 (Oyranos: 0.1.8)
 *  @date    2008/01/08
 */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Create   ( oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version,
                                       const char        * required_cmm,
                                       oyObject_s          object)
{
  oyProfileTag_s * s = 0, * tag = 0;
  int error = !list;
  oyCMMProfileTag_Create_f funcP = 0;
  char cmm[] = {0,0,0,0,0};
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_WRITE, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0};

  if(!error && list->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(!error)
  {
    query.value = tag_type;
    query_[0] = &query;
    queries.queries = query_;
    if(required_cmm)
      error = !memcpy( queries.prefered_cmm, required_cmm, 4 ); 

    if(!error && required_cmm)
      error = !memcpy( cmm, required_cmm, 4 );
  }

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, cmm, &queries, 0,
                                      0,0 );
    if(api)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
      funcP = api3->oyCMMProfileTag_Create;
    }
    error = !funcP;
  }

  if(!error)
  {
    tag = oyProfileTag_New( object );
    error = !tag;
    
    if(!error)
      error = funcP( tag, list, tag_type, version );

    if(!error)
      error = !memcpy( tag->last_cmm_, cmm, 4 );
    if(!error)
      s = tag;
  }

  return s;
}

/** Function oyProfileTag_CreateFromText
 *  @memberof oyProfileTag_s
 *
 *  @param[in]     text                a string
 *  @param[in]     tag_type            type to create, e.g. icSigTextDescriptionType or icSigTextType
 *  @param[in]     tag_usage           signature, e.g. icSigCopyrightTag
 *  @param[in]     object              the user object for the tag creation
 *  @return                            a profile tag
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_CreateFromText (
                                       const char        * text,
                                       icTagTypeSignature  tag_type,
                                       icTagSignature      tag_usage,
                                       oyObject_s          object )
{
  int error = !text;
  oyProfileTag_s * tag = 0;
  oyName_s * name = 0;
  oyStructList_s * list = 0;

  if(!error)
  {
    name = oyName_set_ ( name, text, oyNAME_NAME,
                         oyAllocateFunc_, oyDeAllocateFunc_ );
    error = !name;
  }

  if(!error)
  {
    memcpy( name->lang, "en_GB", 5 );
    list = oyStructList_New(0);
    error = oyStructList_MoveIn( list, (oyStruct_s**) &name, 0 );
  }

  if(!error)
  {
    tag = oyProfileTag_Create( list, tag_type, 0, OY_MODULE_NICK, object);
    error = !tag;
  }

  if(!error)
  tag->use = tag_usage;

  oyStructList_Release( &list );

  return tag;
}

/** Function oyProfileTag_Copy
 *  @memberof oyProfileTag_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                   oyProfileTag_Copy   ( oyProfileTag_s  * obj,
                                         oyObject_s        object)
{
  oyProfileTag_s * s = 0;
  int error = 0;

  if(!obj)
    return s;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error && !object && obj->oy_)
  {
    oyObject_Ref( obj->oy_ );
    return obj;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return s;
}

/** Function oyProfileTag_Release
 *  @memberof oyProfileTag_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI int  OYEXPORT
                   oyProfileTag_Release(oyProfileTag_s  ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyProfileTag_s * s = 0;

  if(!obj || !*obj)
    return error;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_PROFILE_TAG_S)
  {
    WARNc_S("Attempt to release a non oyProfileTag_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return error;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->block_ && s->size_)
      deallocateFunc( s->block_ );
    s->block_ = 0; s->size_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

/** Function oyProfileTag_Set
 *  @memberof oyProfileTag_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI int  OYEXPORT
                   oyProfileTag_Set  ( oyProfileTag_s    * tag,
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer           tag_block )
{
  oyProfileTag_s * s = tag;
  int error = !s;

  if(!error && s->type_ != oyOBJECT_PROFILE_TAG_S)
    error = 1;

  if(!error)
  {
    s->use = sig;
    s->tag_type_ = type;
    s->status_ = status;
    s->size_ = tag_size;
    s->block_ = tag_block;
  }

  return error;
}

/** Function oyProfileTag_GetText
 *  @memberof oyProfileTag_s
 *
 *  For the affect of the parameters look at the appropriate module.
 *  @see oyIMProfileTag_GetValues
 *
 *  Hint: to select a certain module use the oyProfileTag_s::required_cmm
 *  element from the tag parameter.
 *
 *  For localised strings, e.g. icSigMultiLocalizedUnicodeType:<br>
 *    - zero language and country args: all localisation strings are returned 
 *    - with language and/or country args: return appropriate matches
 *    - for language != "", the string starts with language code, the text follows after a colon ":"
 *    - a non zero but empty language argument == "", 
 *      returns the pure string in the actual Oyranos locale, no language code
 *
 *  @param[in]     tag                 the tag to read
 *  @param[out]    n                   the number of returned strings
 *  @param[in]     language            2 byte language code, or "" for current
 *  @param[in]     country             2 byte country code
 *  @param[out]    tag_size            the processed tag size
 *  @param[in]     allocateFunc        the user allocator for the returned list
 *  @return                            a list of strings
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/03 (Oyranos: 0.1.8)
 *  @date    2008/06/19
 */
char **        oyProfileTag_GetText  ( oyProfileTag_s    * tag,
                                       int32_t           * n,
                                       const char        * language,
                                       const char        * country,
                                       int32_t           * tag_size,
                                       oyAlloc_f           allocateFunc )
{
  oyProfileTag_s * s = tag;
  int error = !s;
  oyCMMProfileTag_GetValues_f funcP = 0;
  char cmm[] = {0,0,0,0,0}, 
       t_l[8], t_c[8], *t_ptr;
  int implicite_i18n = 0;
  char ** texts = 0, * text = 0, * text_tmp = 0, * temp = 0;
  oyStructList_s * values = 0;
  oyName_s * name = 0;
  oyBlob_s * blob = 0;
  size_t size = 0;
  int values_n = 0, i = 0, k;
  int32_t texts_n = 0;
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_READ, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0};

  *n = 0;

  if(!error && s->type_ != oyOBJECT_PROFILE_TAG_S)
    error = 1;

  if(!error)
  {
    query.value = tag->tag_type_;
    query_[0] = &query;
    queries.queries = query_;
    error = !memcpy( queries.prefered_cmm, tag->profile_cmm_, 4 ); 

    if(!error)
      error = !memcpy( cmm, tag->required_cmm, 4 );
  }

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, cmm, &queries, 0,
                                      0,0 );
    if(api)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
      funcP = api3->oyCMMProfileTag_GetValues;
    }
    error = !funcP;
  }

  if(!error)
  {
    /* check for a "" in the lang variable -> want the best i18n match */
    if(language && !oyStrlen_(language))
    {
      implicite_i18n = 1;
      language = oyLanguage() ? oyLanguage() : "";
      country  = oyCountry() ? oyCountry() : "";
    }

    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    values = funcP( tag );

      if(oyStructList_Count( values ) )
      {
        name = 0;
        blob = 0;
        values_n = oyStructList_Count( values );

        for(k = 0; k < 4; ++k)
        {
          for(i = 0; i < values_n; ++i)
          {
            text = 0;
            name = (oyName_s*) oyStructList_GetRefType( values, i,
                                                        oyOBJECT_NAME_S );
            if(!name)
            blob = (oyBlob_s*) oyStructList_GetRefType( values, i,
                                                        oyOBJECT_BLOB_S );
            if(name)
            {
              memcpy(t_l, name->lang, 8); t_c[0] = 0;
              t_ptr = oyStrchr_(t_l, '_');
              if(t_ptr)
              {
                memcpy(t_c, t_ptr+1, 3);
                *t_ptr = 0;
              }
            }

            if(name)
              text = name->name;
            else if(blob && blob->ptr && blob->size)
            {
              error = oyStringFromData_( blob->ptr, blob->size, &text_tmp,
                      &size, oyAllocateFunc_ );
              if(!error && size && text_tmp)
                text = text_tmp;
            }

            if(text)
            /* select by language and/or country or best i18n match or all */
            if(
               (k == 0 && language && oyStrlen_( language ) &&
                          oyStrcmp_( language, t_l ) == 0 &&
                          country  && oyStrlen_( country ) &&
                          oyStrcmp_( country, t_c ) )       ||
               (k == 1 && language && oyStrlen_( language ) &&
                          oyStrcmp_( language, t_l ) == 0 &&
                          (!country || implicite_i18n ))                ||
               (k == 2 && country  && oyStrlen_( country ) &&
                          oyStrcmp_( country, t_c ) == 0  &&
                          (!language || implicite_i18n ))               ||
               (k == 3 && ((!language && !country) || implicite_i18n))
              )
            {
              if(name && oyStrlen_(name->lang) && !implicite_i18n)
              {
                /* string with i18n infos -> "de_DE:Licht" */
                temp = oyStringAppend_(name->lang, ":", oyAllocateFunc_);
                temp = oyStringAppend_(temp, text, oyAllocateFunc_);
                oyStringListAddString_( &texts, &texts_n, &temp,
                                            oyAllocateFunc_, oyDeAllocateFunc_);

              } else {
                /* pure string -> "Licht" */
                oyStringListAddStaticString_( &texts, &texts_n, text,
                                            oyAllocateFunc_, oyDeAllocateFunc_);
                /* no selection for best i18n match and no lang: take all */
                if(k == 3 && implicite_i18n)
                {
                  implicite_i18n = 0;
                  language = 0;
                  country = 0;
                }
              }
            }

            /* best i18n match found -> end */
            if(implicite_i18n && texts_n)
            {
              k = 4;
              break;
            }

            if(text_tmp)
              oyFree_m_( text_tmp );
          }
        }

        *n = texts_n;
      }
    oyStructList_Release( &values );

    error = !memcpy( tag->last_cmm_, cmm, 4 );
  }

  return texts;
}




/**
 *  @memberof oyProfiles_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
OYAPI oyProfiles_s * OYEXPORT
                   oyProfiles_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PROFILES_S;
# define STRUCT_TYPE oyProfiles_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyProfiles_Copy;
  s->release = (oyStruct_Release_f) oyProfiles_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/** @internal
 *  @memberof oyProfiles_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
OYAPI oyProfiles_s * OYEXPORT
                   oyProfiles_Copy  ( oyProfiles_s * obj,
                                         oyObject_s        object)
{
  oyProfiles_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc = 0;

  if(!obj)
    return s;

  s = oyProfiles_New( object );
  error = !s;

  if(!error)
    allocateFunc = s->oy_->allocateFunc_;

  if(!error)
  {
    if(obj->list_)
    {
      s->list_ = oyStructList_Copy(obj->list_, object);
      error = !s->list_;
    }
  }

  if(error)
  {
    WARNc_S("Could not create structure for profile.")
    return 0;
  }

  return s;
}

oyProfiles_s * oy_profile_list_cache_ = 0;

/** Function oyProfiles_Create
 *  @memberof oyProfiles_s
 *  @brief   get a list of installed profiles
 *
 *  @param[in]     patterns            a list properties, e.g. classes
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 *  @date    2008/06/20
 */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create( oyProfiles_s   * patterns,
                                       oyObject_s          object)
{
  oyProfiles_s * s = 0;
  int error = 0;

  oyProfile_s * tmp = 0, * pattern = 0;
  char  ** names = 0, * full_name = 0;
  oyPointer block = 0;
  uint32_t names_n = 0, i = 0, j = 0, n = 0,
           patterns_n = oyProfiles_Count(patterns);
  size_t   size = 128;

  s = oyProfiles_New( object );
  error = !s;

  if(!error)
  {
    names = oyProfileListGet_ ( NULL, &names_n );

    for(j = 0; j < patterns_n; ++j)
    {
      pattern = oyProfiles_Get(patterns, j);

      if(pattern->size_ > 132)
        size = 0;

      oyProfile_Release( &pattern );
    }

    if(oyProfiles_Count( oy_profile_list_cache_ ) != names_n)
    {
      for(i = 0; i < names_n; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          {
            if(size && 0)
            { /* TODO short readings */
              full_name = oyFindProfile_(names[i]);
              block = oyReadFileToMem_ (full_name, &size, oyAllocateFunc_);
              tmp = oyProfile_FromMemMove_( size, &block, 0, 0 );
            }
            else
            {
              tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );
              oy_profile_list_cache_ = oyProfiles_MoveIn(oy_profile_list_cache_,
                                                         &tmp, -1);
              error = !oy_profile_list_cache_;
            }
          }
        }
      }
    }

    n = oyProfiles_Count( oy_profile_list_cache_ );
    if(oyProfiles_Count( oy_profile_list_cache_ ) != names_n)
      WARNc2_S("updated oy_profile_list_cache_ differs: %d %d",n, names_n);
    oyStringListRelease_( &names, names_n, oyDeAllocateFunc_ ); names_n = 0;

    for(i = 0; i < n; ++i)
    {
        tmp = oyProfiles_Get( oy_profile_list_cache_, i );

        if(patterns_n > 0)
        {
          for(j = 0; j < patterns_n; ++j)
          {
            if(tmp)
              pattern = oyProfiles_Get(patterns, j);

            if(oyProfile_Match_( pattern, tmp ))
            {
              s = oyProfiles_MoveIn( s, &tmp, -1);
              error = !s;
              break;
            }
          }

        } else {

          s = oyProfiles_MoveIn( s, &tmp, -1);
          error = !s;
        }

        if(tmp)
          oyProfile_Release( &tmp );
    }
  }

  return s;
}

/** Function oyProfiles_ForStd
 *  @memberof oyProfiles_s
 *  @brief   get a list of installed profiles
 *
 *  Allow for a special case with oyDEFAULT_PROFILE_START in the colour_space
 *  argument, to select all possible standard colour profiles, e.g. for 
 *  typical colour conversions.
 *
 *  @param[in]     std_profile_class  standard profile class, e.g. oyEDITING_RGB
 *  @param[out]    current             get the colour_space profile position
 *  @param         object              a optional object
 *  @return                            the profile list
 *
 *  @par Example - get all standard RGB profiles:
 *  @verbatim
    oyPROFILE_e type = oyEDITING_RGB;
    int current = 0,
        size, i;
    oyProfile_s * temp_prof = 0;
    oyProfiles_s * iccs = 0;

    iccs = oyProfiles_ForStd( type, &current, 0 );

    size = oyProfiles_Count(iccs);
    for( i = 0; i < size; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      printf("%s %d: \"%s\" %s\n", i == current ? "*":" ", i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, 0));
      oyProfile_Release( &temp_prof );
    } @endverbatim
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/25 (Oyranos: 0.1.8)
 *  @date    2008/08/06
 */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         std_profile_class,
                                       int               * current,
                                       oyObject_s          object)
{
  oyPROFILE_e type = std_profile_class;
    char * default_p = 0;
    int i, val = -1;

    char  * temp = 0,
          * text = 0;
    uint32_t size = 0;
    oyProfiles_s * iccs = 0, * patterns = 0;
    oyProfile_s * profile = 0, * temp_prof = 0;
    icSignature csp;


    if(type == oyEDITING_XYZ ||
       type == oyASSUMED_XYZ ||
       type == oyEDITING_LAB ||
       type == oyASSUMED_LAB ||
       type == oyEDITING_RGB ||
       type == oyASSUMED_RGB ||
       type == oyEDITING_CMYK ||
       type == oyASSUMED_CMYK ||
       type == oyPROFILE_PROOF ||
       type == oyEDITING_GRAY ||
       type == oyASSUMED_GRAY)
      default_p = oyGetDefaultProfileName( (oyPROFILE_e)type, oyAllocateFunc_);

    /* prepare the patterns according to the profile type */
    if(type == oyEDITING_XYZ ||
       type == oyASSUMED_XYZ)
    {
      csp = icSigXYZData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_LAB ||
       type == oyASSUMED_LAB)
    {
      csp = icSigLabData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_RGB ||
       type == oyASSUMED_RGB)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_CMYK ||
       type == oyASSUMED_CMYK ||
       type == oyPROFILE_PROOF)
    {
      csp = icSigCmykData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_GRAY ||
       type == oyASSUMED_GRAY)
    {
      csp = icSigGrayData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyDEFAULT_PROFILE_START)
    {
      profile = oyProfile_FromSignature( icSigColorSpaceClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigInputClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigOutputClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigDisplayClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }

    /* get the profile list */
    iccs = oyProfiles_Create( patterns, 0 );

    /* detect the default profile position in our list */
    size = oyProfiles_Count(iccs);
    if(default_p)
    for( i = 0; i < size; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      text = oyStringCopy_( oyProfile_GetFileName(temp_prof, 0),
                            oyAllocateFunc_ );
      temp = oyStrrchr_( text, '/' );
      if(temp)
        ++temp;
      else
        temp = text;

      if(oyStrstr_( temp, default_p) &&
         oyStrlen_( temp ) == oyStrlen_(default_p))
      {
        val = i;
        break;
      }

      oyProfile_Release( &temp_prof );
      oyDeAllocateFunc_( text );
    }

    if(current)
      *current          = val;

    if(default_p)
      oyFree_m_( default_p );
    oyProfiles_Release( &patterns );

  return iccs;
}

/**
 *  @memberof oyProfiles_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
OYAPI int  OYEXPORT
                   oyProfiles_Release(oyProfiles_s** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyProfiles_s * s = 0;

  if(!obj || !*obj)
    return error;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_PROFILES_S)
  {
    WARNc_S("Attempt to release a non oyProfiles_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return error;
  /* ---- end of common object destructor ------- */

  if(!error && s->list_)
  error = oyStructList_Release(&s->list_);

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

/**
 *  @memberof oyProfiles_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
oyProfiles_s* oyProfiles_MoveIn( oyProfiles_s   * list,
                                       oyProfile_s      ** obj,
                                       int                 pos )
{
  int error = 0;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_S)
  {
    if(!list)
      list = oyProfiles_New(0);

    if(list && list->list_)
        error = oyStructList_MoveIn( list->list_, (oyStruct_s**) obj, pos );
  }

  return list;
}

/**
 *  @memberof oyProfiles_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyProfiles_ReleaseAt( oyProfiles_s * list,
                                       int                 pos )
{
  int error = 0;

  if(list && list->list_)
    error = oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/**
 *  @memberof oyProfiles_s
 *
 *  @param[in] list                    the profile list to use
 *  @param[in] pos                     the position in list
 *  @return                            a copy of the profile owned by the caller
 *
 *  @since Oyranos: version 0.1.8  2007/11/22
 *  @date  20 december 2007 (API 0.1.8)
 */
oyProfile_s *    oyProfiles_Get   ( oyProfiles_s   * list,
                                       int                 pos )
{
  oyProfile_s * obj = 0;

  if(list && list->list_)
  {
    oyProfile_s * p = (oyProfile_s*) oyStructList_GetType_( list->list_,
                                                 pos, oyOBJECT_PROFILE_S );

    if(p)
      obj = oyProfile_Copy(p, 0);
  }

  return obj;
}

/**
 *  @memberof oyProfiles_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyProfiles_Count ( oyProfiles_s   * list )
{
  int n = 0;

  if(list && list->list_)
    n = oyStructList_Count( list->list_ );

  return n;
}

/** @} *//* objects_profile */




/** \addtogroup misc Miscellaneous

 *  @{
 */

/** \addtogroup objects_region Region Handling

 *  @{
 */


/** @internal
 *  @brief   new
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_New_         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_REGION_S;
# define STRUCT_TYPE oyRegion_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyRegion_Copy;
  s->release = (oyStruct_Release_f) oyRegion_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** 
 *  @brief   new with geometry
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_NewWith      ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object )
{
  oyRegion_s * s = oyRegion_New_( object );
  if(s)
    oyRegion_SetGeo( s, x, y, width, height );
  return s;
}

/**
 *  @brief   new from other region
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_NewFrom      ( oyRegion_s        * ref,
                                       oyObject_s          object )
{
  oyRegion_s * s = oyRegion_New_( object );
  if(s)
    oyRegion_SetByRegion(s, ref);
  return s;
}

/**
 *  @brief   copy/reference from other region
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_Copy         ( oyRegion_s        * orig,
                                       oyObject_s          object )
{
  oyRegion_s * s = 0;

  if(!orig)
    return s;

  if(object)
  {
    s = oyRegion_NewFrom( orig, object );

  } else {

    s = orig;
    oyObject_Copy( s->oy_ );
  }

  return s;
}

/**
 *  @brief   release
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_Release      ( oyRegion_s       ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyRegion_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_REGION_S)
  {
    WARNc_S("Attempt to release a non oyRegion_s object.")
    return 1;
  }

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

  return error;
}

/**
 *  @brief   set geometry
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_SetGeo       ( oyRegion_s        * edit_region,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height )
{
  oyRegion_s * s = edit_region;
  if(!s)
    return;
  s->x = x;
  s->y = y;
  s->width = width;
  s->height = height;
}

/**
 *  @brief   copy values
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_SetByRegion  ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref )
{
  oyRegion_s * s = edit_region;
  if(!s || !ref)
    return;

  oyRegion_SetGeo( s, ref->x, ref->y, ref->width, ref->height );
}

/**
 *  @brief   trim edit_region to ref extents
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Trim         ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  if(!s)
    return;

  if (r->x < ref->x)
  { 
    r->width -= ref->x - r->x;
    r->x = ref->x;
  }
  if (r->x + r->width > ref->x + ref->width)
    r->width -= (r->x + r->width) - (ref->x + ref->width);
  if( r->width < 0 )
    r->width = 0;
  
  if (r->y < ref->y)
  {
    r->height -= ref->y - r->y;
    r->y = ref->y;
  }
  if (r->y + r->height > ref->y + ref->height)
    r->height -= (r->y + r->height) - (ref->y + ref->height);
  if( r->height < 0 )
    r->height = 0;

  oyRegion_Normalise( r );
}

/**
 *  @brief   trim edit_region to ref extents
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_MoveInside   ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * a = ref;

  if(!s)
    return;

  oyRegion_Normalise( s );

  if (s->x < a->x)
    s->x = a->x;
  if (s->x+s->width > a->x+a->width)
  { if (s->width > a->width)
      ; /* Lassen */
    else
      s->x = a->x+a->width - s->width;
  }
  if (s->y < a->y)
    s->y = a->y;
  if (s->y+s->height  > a->y+a->height)
  { if (s->height > a->height)
      ; /* Lassen */
    else
      s->y = a->y+a->height - s->height;
  }
}

/**
 *  @brief   scale with origin in the top left corner
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Scale        ( oyRegion_s        * edit_region,
                                       double              factor )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  
  if(!s)
    return;

  r->x *= factor;
  r->y *= factor;
  r->width *= factor;
  r->height *= factor;
}

/**
 *  @brief   normalise swapped values for width and height
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Normalise    ( oyRegion_s        * edit_region )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  
  if(!s)
    return;

  if(r->width < 0) {
    r->x += r->width;
    r->width = fabs(r->width);
  }
  if(r->height < 0) {
    r->y += r->height;
    r->height = fabs(r->height);
  }
}

/**
 *  @brief   scale with origin in the top left corner
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Round        ( oyRegion_s        * edit_region )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  
  if(!s)
    return;

  r->x = (int)OY_ROUND(r->x);
  r->y = (int)OY_ROUND(r->y);
  r->width = (int)OY_ROUND(r->width);
  r->height = (int)OY_ROUND(r->height);
}

/**
 *  @brief   compare
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_IsEqual      ( oyRegion_s        * region1,
                                       oyRegion_s        * region2 )
{
  int gleich = TRUE;
  oyRegion_s * r1 = region1;
  oyRegion_s * r2 = region2;
  
  if(!r1 || !r2)
    return FALSE;

  if (r1->x != r2->x) gleich = FALSE;
  if (r1->y != r2->y) gleich = FALSE;
  if (r1->width != r2->width) gleich = FALSE;
  if (r1->height != r2->height) gleich = FALSE;
  return gleich;
}

/**
 *  @brief   compare
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_IsInside     ( oyRegion_s        * region,
                                       double              x,
                                       double              y )
{
  oyRegion_s * s = region;
  oyRegion_s * r = s;
  int in = TRUE;
  
  if(!s)
    return FALSE;

  if (x < r->x) return FALSE;
  if (y < r->y) return FALSE;
  if (x > (r->x + r->width)) return FALSE;
  if (y > (r->y + r->height)) return FALSE;
  return in;
}

/**
 *  @brief   count number of points covered by this region
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_CountPoints  ( oyRegion_s        * region )
{
  oyRegion_s * s = region;
  oyRegion_s * r = s;
  int w,h;
  
  if(!s)
    return FALSE;

  w = (int)OY_ROUND(r->width);
  h = (int)OY_ROUND(r->height);
  return (w) * (h);
}

/** @brief   return position inside region, assuming region size
 *  @memberof oyRegion_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_Index        ( oyRegion_s        * region,
                                       double              x,
                                       double              y )
{
  oyRegion_s * s = region;
  oyRegion_s * r = s;
  
  if(!s)
    return FALSE;

  return (int)OY_ROUND((y - r->y) * r->width + (x - r->x));
}

/**
 *  @memberof oyRegion_s
 *  @brief   debug text
 *  not so threadsafe
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyChar*        oyRegion_Show         ( oyRegion_s        * r )
{
  static oyChar *text = 0;

  if(!text)
    text = oyAllocateFunc_(sizeof(oyChar) * 512);

  if(r)
    oySprintf_(text, "%.02f+%.02f:%.02fx%.02f",r->x , r->y, r->width , r->height);
  else
    oySprintf_(text, "no region");

  return text;

}


/**
 *  @} *//* objects_region
 */



/**
 *  @brief oyDATATYPE_e to byte mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int
oySizeofDatatype (oyDATATYPE_e t)
{
  int n = 0;
  switch(t)
  {
    case oyUINT8:
         n = 1; break;
    case oyUINT16:
    case oyHALF:
         n = 2; break;
    case oyUINT32:
    case oyFLOAT:
         n = 4; break;
    case oyDOUBLE:
         n = 8; break;
  }
  return n;
}

/**
 *  @brief oyDATATYPE_e to string mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */

const oyChar * oyDatatypeToText      ( oyDATATYPE_e        t)
{
  const oyChar * text = 0;
  switch(t)
  {
    case oyUINT8:
         text = "oyUINT8"; break;
    case oyUINT16:
         text = "oyUINT16"; break;
    case oyHALF:
         text = "oyHALF"; break;
    case oyUINT32:
         text = "oyUINT32"; break;
    case oyFLOAT:
         text = "oyFLOAT"; break;
    case oyDOUBLE:
         text = "oyDOUBLE"; break;
  }
  return text;
}


/** @} *//* misc */



/** \addtogroup objects_image Image API
 *
 *  @{
 */


/** Function oyArray2d_New
 *  @memberof oyArray2d_s
 *  @brief   allocate a new Array2d object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_ARRAY2D_S;
# define STRUCT_TYPE oyArray2d_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyArray2d_Copy;
  s->release = (oyStruct_Release_f) oyArray2d_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/**
 *  @internal
 *  Function oyArray2d_Create_
 *  @memberof oyArray2d_s
 *  @brief   allocate and initialise a oyArray2d_s object widthout pixel
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/10/03
 */
OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create_ ( int                 width,
                                       int                 height,
                                       oyDATATYPE_e        type,
                                       oyObject_s          object )
{
  oyArray2d_s * s = 0;
  int error = 0;

  if(!width || !height || !type)
    return s;

  s = oyArray2d_New( object );
  error = !s;

  if(!error)
  {
    int y_len = sizeof(unsigned char *) * (height + 1);

    s->width = width;
    s->height = height;
    s->t = type;
    s->data_area = oyRegion_NewWith( 0,0, width, height, s->oy_ );
    s->array2d = s->oy_->allocateFunc_( y_len );
    error = !memset( s->array2d, 0, y_len );
    s->own_lines = 0;
  }

  return s;
}

/** Function oyArray2d_Create
 *  @memberof oyArray2d_s
 *  @brief   allocate and initialise a oyArray2d_s object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        type,
                                       oyObject_s          object )
{
  oyArray2d_s * s = 0;
  int error = 0;

  if(!width || !height || !type || !data)
    return s;

  s = oyArray2d_Create_( width, height, type, object );
  error = !s;

  if(!error)
    error = oyArray2d_DataSet( s, data );

  return s;
}

/**
 *  @internal
 *  Function oyArray2d_Copy_
 *  @memberof oyArray2d_s
 *  @brief   real copy a Array2d object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
oyArray2d_s * oyArray2d_Copy_
                                     ( oyArray2d_s       * obj,
                                       oyObject_s          object )
{
  oyArray2d_s * s = 0;
  int error = 0;
  int i;
  oyAlloc_f allocateFunc_ = 0;
  size_t size = 0;

  if(!obj || !object)
    return s;

  s = oyArray2d_Create( 0, obj->height, 0, obj->t, object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;
    s->own_lines = 1;
    for(i = 0; i < s->height; ++i)
    {
      size = s->width * oySizeofDatatype( s->t );
      oyAllocHelper_m_( s->array2d[i], unsigned char, size, allocateFunc_,
                        error = 1; break );
      error = !memcpy( s->array2d[i], obj->array2d[i], size );
    }
  }

  if(error)
    oyArray2d_Release( &s );

  return s;
}

/** Function oyArray2d_Copy
 *  @memberof oyArray2d_s
 *  @brief   copy or reference a Array2d object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Copy    ( oyArray2d_s       * obj,
                                       oyObject_s          object )
{
  oyArray2d_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_ARRAY2D_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyArray2d_Copy_( obj, object );

  return s;
}
 
/**
 *  @internal
 *  Function oyArray2d_Release
 *  @memberof oyArray2d_s
 *  @brief   release and possibly deallocate a Array2d object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI int  OYEXPORT
               oyArray2d_Release     ( oyArray2d_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyArray2d_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_ARRAY2D_S)
  {
    WARNc_S(("Attempt to release a non oyArray2d_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int y;
    size_t dsize = oySizeofDatatype( s->t );

    for( y = s->data_area->y; y < s->data_area->height; ++y )
    {
      if(s->own_lines)
        deallocateFunc( s->array2d[y] );
      s->array2d[y] = 0;
    }
    deallocateFunc( s->array2d - (size_t)(dsize * -s->data_area->y) );

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/**
 *  @internal
 *  Function oyArray2d_DataSet
 *  @memberof oyArray2d_s
 *  @brief   set the data blob and (re-)initialise the object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     data                the data
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI int  OYEXPORT
                 oyArray2d_DataSet   ( oyArray2d_s       * obj,
                                       oyPointer           data )
{
  oyArray2d_s * s = 0;
  int error = 0;

  if(!data)
    return error;

  if(!obj || obj->type_ != oyOBJECT_ARRAY2D_S)
    return 1;

  s = obj;

  {
    int y_len = sizeof(unsigned char *) * (s->height + 1),
        y;
    uint8_t * u8 = data;

    error = !s->array2d;

    if(!error)
      error = !memset( s->array2d, 0, y_len );

    s->own_lines = 0;

    if(!error)
      for( y = 0; y < s->height; ++y )
        s->array2d[y] = &u8[oySizeofDatatype( s->t ) * s->width * y];
  }

  return error;
}


/**
 *  @internal
 *  Function oyImage_CombinePixelLayout2Mask_
 *  @memberof oyImage_s
 *  @brief   describe a images channel and pixel layout
 *
 *  - gather informations about the pixel layout
 *  - describe the colour channels characteristic into oyImage_s::channel_layout
 *  - store some text in the images nick name as a ID
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/09/20
 */
int
oyImage_CombinePixelLayout2Mask_ (
                             oyImage_s   * image,
                             oyPixel_t     pixel_layout,
                             oyProfile_s * profile )
{
  int n     = oyToChannels_m( pixel_layout );
  int cchan_n = oyProfile_GetChannelsCount( profile );
  int coff_x = oyToColourOffset_m( pixel_layout );
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  int swap  = oyToSwapColourChannels_m( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  oyPixel_t *mask = image->oy_->allocateFunc_( sizeof(oyPixel_t*) * (oyCHAN0 + 
                    OY_MAX(n,cchan_n) + 1));
  int error = !mask;
  int so = oySizeofDatatype( t );
  int w = image->width;
  int h = image->height;
  int i;
  char * text = oyAllocateFunc_(512);
  char * hash_text = 0;
  oyImage_s * s = image;
  oyCHANNELTYPE_e * clayout = 0; /* non profile described channels */

  if(!s)
    return 0;

  /* describe the pixel layout and access */
  if(!error)
  {
    error = !memset( mask, 0, sizeof(mask) * sizeof(oyPixel_t*));
    if(oyToPlanar_m( pixel_layout ))
    {
      mask[oyPOFF_X] = 1;
      mask[oyCOFF] = w*h*n;
    } else {
      mask[oyPOFF_X] = n;
      mask[oyCOFF] = 1;
    }
    mask[oyPOFF_Y] = mask[oyPOFF_X] * w;
    mask[oyDATA_SIZE] = so;
    mask[oyLAYOUT] = pixel_layout;
    mask[oyCHANS] = n;

    if(swap)
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + cchan_n - i - 1;
    else
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + i;
  }

  /* describe the channels characters */
  if(!s->channel_layout)
  {
    clayout = image->oy_->allocateFunc_( sizeof(int) * ( OY_MAX(n,cchan_n)+ 1));
    /* we dont know about the content */
    for(i = 0; i < n; ++i)
      clayout[i] = oyCHANNELTYPE_OTHER;
    /* describe profile colours */
    for(i = coff_x; i < coff_x + cchan_n; ++i)
      clayout[i] = oyICCColourSpaceToChannelLayout( profile->sig_, i - coff_x );
    /* place a end marker */
    clayout[n] = oyCHANNELTYPE_UNDEFINED;
      s->channel_layout = clayout;
  }

  /* describe the image */
  oySprintf_( text, 
                  "  <oyImage_s id=\"%d\" width=\"%d\" height=\"%d\" resolution=\"%.02f,%.02f\">\n",
                  oyObject_GetId(image->oy_),
                  image->width,
                  image->height,
                  image->resolution_x,
                  image->resolution_y);
  hashTextAdd_m( text );
  oySprintf_( text, "    %s\n", oyProfile_GetText(profile, oyNAME_NAME));
  hashTextAdd_m( text );
  oySprintf_( text, "    <channels all=\"%d\" colour=\"%d\" />\n", n, cchan_n );
  hashTextAdd_m( text );
  oySprintf_( text,
              "    <offsets first_colour_sample=\"%d\" next_pixel=\"%d\" />\n"
              /*"  next line = %d\n"*/,
              coff_x, mask[oyPOFF_X]/*, mask[oyPOFF_Y]*/ );
  hashTextAdd_m( text );

  if(swap || oyToByteswap_m( pixel_layout ))
  {
    hashTextAdd_m( "    <swap" );
    if(swap)
      hashTextAdd_m( " colourswap=\"yes\"" );
    if( oyToByteswap_m( pixel_layout ) )
      hashTextAdd_m( " byteswap=\"yes\"" );
    hashTextAdd_m( " />\n" );
  }

  if( oyToFlavor_m( pixel_layout ) )
  {
    oySprintf_( text, "    <flawor value=\"yes\" />\n" );
    hashTextAdd_m( text );
  }
  oySprintf_( text, "    <sample_type value=\"%s[%dByte]\" />\n",
                    oyDatatypeToText(t), so );
  hashTextAdd_m( text );
  oySprintf_( text, "  </oyImage_s>");
  hashTextAdd_m( text );

  if(!error)
    error = oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );


  oyDeAllocateFunc_(text);
  oyDeAllocateFunc_(hash_text);

  if(s->oy_->deallocateFunc_)
  {
    if(s->layout_)
      s->oy_->deallocateFunc_(s->layout_);
    s->layout_ = 0;
  }
  s->layout_ = mask;


  return 0;
}


/** Function oyImage_GetPointContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout pixel accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyPointer oyImage_GetArray2dPointContinous (
                                         oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  int pos = (point_x * image->layout_[oyCHANS]
             + image->layout_[oyCHAN0+channel])
            * image->layout_[oyDATA_SIZE];
  if(is_allocated) *is_allocated = 0;
  return &array2d[ point_y ][ pos ]; 

}

/** Function oyImage_GetLineContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout line accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
oyPointer oyImage_GetArray2dLineContinous (
                                         oyImage_s       * image,
                                         int               point_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  if(height) *height = 1;
  if(is_allocated) *is_allocated = 0;
  return &array2d[ point_y ][ 0 ]; 
}

/** Function oyImage_GetPointPlanar
 *  @memberof oyImage_s
 *  @brief   standard planar layout pixel accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/08/24
 */
oyPointer oyImage_GetArray2dPointPlanar( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated )
{
  WARNc_S("planar pixel access not implemented")
  return 0;
#if 0
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  if(is_allocated) *is_allocated = 0;
  return &array2d[ point_y ][ (point_x + image->layout_[oyCOFF]
                               * image->layout_[oyCHAN0+channel])
                              * image->layout_[oyDATA_SIZE]       ]; 
#endif
}

/** Function oyImage_GetLinePlanar
 *  @memberof oyImage_s
 *  @brief   standard continus layout line accessor
 *
 *  We assume a channel after channel behaviour without line interweaving.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/24
 */
oyPointer oyImage_GetArray2dLinePlanar ( oyImage_s       * image,
                                         int               point_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated )
{
  if(height) *height = 1;
  WARNc_S("planar pixel access not implemented")
  return 0;
#if 0 /* SunC: warning: statement not reached */
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  if(is_allocated) *is_allocated = 0;
  /* it makes no sense to use more than one line */                   
  return &array2d[ 0 ][   image->width
                        * image->layout_[oyCOFF]
                        * image->layout_[oyCHAN0+channel]
                        * image->layout_[oyDATA_SIZE] ];
#endif
}


/** @brief   collect infos about a image
 *  @memberof oyImage_s
 *
 *  Create a image description and access object. The passed channels pointer
 *  remains in the responsibility of the user. The image is a in memory blob.
 *
    @param[in]    width        image width
    @param[in]    height       image height
    @param[in]    channels     pointer to the data buffer
    @param[in]    pixel_layout i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]    profile      colour space description
    @param[in]    object       the optional base
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
oyImage_s *    oyImage_Create         ( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        oyObject_s        object)
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_IMAGE_S;
# define STRUCT_TYPE oyImage_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyImage_Copy;
  s->release = (oyStruct_Release_f) oyImage_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->width = width;
  s->height = height;
  {
    oyArray2d_s * a = oyArray2d_Create( channels,
                                        s->width * oyToChannels_m(pixel_layout),
                                        s->height,
                                        oyToDataType_m(pixel_layout),
                                        s_obj );
    oyImage_DataSet ( s, (oyStruct_s**) &a, 0,0,0 );
  }
  s->profile_ = oyProfile_Copy( profile, 0 );
  s->viewport = oyRegion_NewWith( 0, 0, 1.0, s->height/s->width, s->oy_ );

  error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout, profile );

  if(s->pixel_data && s->layout_[oyCOFF] == 1)
    oyImage_DataSet( s, 0, oyImage_GetArray2dPointContinous,
                           oyImage_GetArray2dLineContinous, 0 );
  else if(s->pixel_data)
    oyImage_DataSet( s, 0, oyImage_GetArray2dPointPlanar,
                           oyImage_GetArray2dLinePlanar, 0 );

  return s;
}

/** @brief   collect infos about a image for showing one a display
 *  @memberof oyImage_s

    @param[in]    width        image width
    @param[in]    height       image height
    @param[in]    channels     pointer to the data buffer
    @param[in]    pixel_layout i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]    display_name  display name
    @param[in]    display_pos_x left image position on display
    @param[in]    display_pos_y top image position on display
    @param[in]    object       the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_CreateForDisplay(int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        oyPixel_t         pixel_layout,
                                        const char      * display_name,
                                        int               display_pos_x,
                                        int               display_pos_y,
                                        oyObject_s        object)
{
  oyImage_s * s = oyImage_Create( width, height, channels, pixel_layout,
                                  0, object );
  int error = !s;

  if(!error)
  {
    oyProfile_Release( &s->profile_ );
    s->profile_ = oyProfile_FromFile (0,0,0);
    if(!s->profile_)
      error = 1;

    if(!error)
      error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout, s->profile_ );

    s->display_pos_x = display_pos_x;
    s->display_pos_y = display_pos_y;
  }

  return s;
}

/**
 *  @internal
 *  @brief   copy a image
 *  @memberof oyImage_s
 *
 *  @todo  implement
 * 
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_Copy_          ( oyImage_s       * image,
                                        oyObject_s        object )
{
  oyImage_s * s = 0;

  if(!image)
    return s;

  /* TODO */

  s = image;
  s->oy_ = oyObject_Copy( s->oy_ );

  return s;
}

/** @brief   copy a image
 *  @memberof oyImage_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_Copy           ( oyImage_s       * image,
                                        oyObject_s        object )
{
  oyImage_s * s = image;

  if(!s)
    return s;

  if(s->type_ != oyOBJECT_IMAGE_S)
  {
    WARNc_S("Attempt to copy a non oyImage_s object.")
    return 0;
  }

  if(!object)
  {
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyImage_Copy_( image, object );
  /* TODO cache */
  return s;
}

/** @brief   release a image
 *  @memberof oyImage_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int            oyImage_Release        ( oyImage_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyImage_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_IMAGE_S)
  {
    WARNc_S("Attempt to release a non oyImage_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  s->width = 0;
  s->height = 0;
  if(s->pixel_data && s->pixel_data->release)
    s->pixel_data->release( &s->pixel_data );

  if(s->user_data && s->user_data->release)
    s->user_data->release( &s->user_data );

  oyProfile_Release( &s->profile_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->layout_)
      deallocateFunc( s->layout_ ); s->layout_ = 0;

    if(s->channel_layout)
      deallocateFunc( s->channel_layout ); s->channel_layout = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @brief   set a image
 *  @memberof oyImage_s
 *
 *  set critical options
 *
 *  @since Oyranos: version 0.1.8
 *  @date  19 december 2007 (API 0.1.8)
 */
int            oyImage_SetCritical    ( oyImage_s       * image,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        oyOptions_s     * options )
{
  oyImage_s * s = image;
  int error = !s;

  if(profile)
    s->profile_ = oyProfile_Copy( profile, 0 );

  if(pixel_layout)
    error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout, s->profile_ );

  if(options)
    s->options_ = oyOptions_Copy( options, s->oy_ );

  return error;
}

/** Function oyImage_SetData
 *  @memberof oyImage_s
 *  @brief   set a custom image data backend
 *
 *  This function allowes for exchanging of all the backend components. 
 *
 *  The pixel_data structure can hold in memory or mmap representations or file
 *  pointers. The according point, line and/or tile functions shall use
 *  the oyImage_s::pixel_data member to access the data and provide in this
 *  interface.
 *
 *  @param         image               the image
 *  @param         pixel_data          data struct will be moved in
 *  @param         getPoint            interface function
 *  @param         getLine             interface function
 *  @param         getTile             interface function
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
int            oyImage_DataSet       ( oyImage_s         * image,
                                       oyStruct_s       ** pixel_data,
                                       oyImage_GetPoint_f  getPoint,
                                       oyImage_GetLine_f   getLine,
                                       oyImage_GetTile_f   getTile )
{
  oyImage_s * s = image;
  int error = 0;

  if(!s)
    return 1;

  if(s->type_ != oyOBJECT_IMAGE_S)
  {
    WARNc_S("Attempt to manipulate a non oyImage_s object.")
    return 1;
  }

  if(pixel_data)
  {
    if(s->pixel_data && s->pixel_data->release)
      s->pixel_data->release( &s->pixel_data );
    s->pixel_data = *pixel_data;
    *pixel_data = 0;
  }

  if(getPoint)
    s->getPoint = getPoint;

  if(getLine)
    s->getLine = getLine;

  if(getTile)
    s->getTile = getTile;

  return error;
}

/** Function oyImage_FillArray
 *  @memberof oyImage_s
 *  @brief   creata a array from a image and fill with data
 *
 *  The region will be considered relative to the data.
 *  A given array will be filled. do_copy makes the distinction to reuse a 
 *  available array2d. If do_copy is set to false, a owned array is freed and
 *  newly allocated or a the new array is set according to the get* API
 *  in oyImage_s. A empty array will be completely allocated.
 *  Pixel layouts should be normalised within this function. So it works like a
 *  mediator.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/02 (Oyranos: 0.1.8)
 *  @date    2008/10/05
 */
int            oyImage_FillArray     ( oyImage_s         * image,
                                       oyRegion_s        * region,
                                       int                 do_copy,
                                       oyArray2d_s      ** array,
                                       oyObject_s          obj )
{
  int error = 0;
  oyArray2d_s * s = *array;
  oyRegion_s * pixel_region = 0;
  oyDATATYPE_e data_type = oyUINT8;
  int is_allocated = 0;
  int size = 0, channel_n;

  if(!image)
    return 0;

  if(image->type_ != oyOBJECT_IMAGE_S)
  {
    WARNc_S("Attempt to access a non oyImage_s object.")
    return 0;
  }

  data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  size = oySizeofDatatype( data_type );
  channel_n = image->layout_[oyCHANS];
  pixel_region = oyRegion_NewFrom( region, 0 );
  if( !region )
  {
    pixel_region->width = 1.0;
    pixel_region->height = image->height / (double)image->width;
  }

  oyRegion_Scale( pixel_region, image->width );
  pixel_region->x *= channel_n;
  pixel_region->width *= channel_n;

  if( s && s->width != pixel_region->width )
  {
    WARNc_S("s->width != pixel_region->width  not yet implemented");
    oyArray2d_Release( &s );
  }

  if(!s)
    s = oyArray2d_Create_( pixel_region->width, pixel_region->height,
                           data_type, obj );

  if(image->getTile)
  {
  }

  if(image->getLine)
  {
    unsigned char * data = 0;
    int x = OY_ROUND( pixel_region->x ),
        y = OY_ROUND( pixel_region->y ),
        w = OY_ROUND( pixel_region->width ),
        h = OY_ROUND( pixel_region->height ),
        i,j, height;
    size_t len;

    s->data_area->x = -x;
    s->data_area->y = 0;
    s->data_area->width = image->width;
    s->data_area->height = h;

    for( i = 0; i < h; )
    {
      height = is_allocated = 0;
      data = image->getLine( image, y+i, &height, -1, &is_allocated );
      len = size * w;

      for( j = 0; j < height; ++j )
      {
        if(do_copy)
        {
          if(!s->array2d[i+j])
            oyAllocHelper_m_( s->array2d[i+j], unsigned char, len,
                              s->oy_ ? s->oy_->allocateFunc_ : 0,
                              error = 1; break );

          error = !memcpy( s->array2d[i+j], &data[j * size * s->width], len );

        } else
          s->array2d[i+j] = &data[j * size * s->width];

        s->array2d[i+j] = &s->array2d[i+j][size * x];
      }

      i += height;

      if(error) break;
    }
  }

  if(image->getPoint)
  {
  }

  if(error)
    oyArray2d_Release( &s );

  *array = s;

  return error;
}

/** @} *//* objects_image */ 



/** \addtogroup objects_conversion Conversion API's
 *  Colour conversion front end API's.
 *
 *  Colour conversions are realised by structures called acyclic graphs.
 *
 *  \b About \b Graphs: \n
 *  The top object a user will handle is of type oyConversion_s. This 
 *  contains oyFilterNode_s objects, which describe the connections of each
 *  processing element, such as images with attached data and profile, and the
 *  filters. The Oyranos node connection concept is splitted into various
 *  structures.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded,filled"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="Oyranos Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowhead=none, arrowtail=normal];
    b:socket -> d:plug [arrowhead=none, arrowtail=normal];
    a:socket -> c:plug [arrowhead=none, arrowtail=normal];
    c:socket -> d:plug [arrowhead=none, arrowtail=normal];
  }
}
 \enddot
 *
 *  \b Connectors \b have \b tree \b missions:
 *  - The first is to tell others to about the 
 *  filters intention to provide a connection endity. This is done by the pure
 *  existence of the oyConnector_s inside the backend filter structure
 *  (oyCMMapi4_s) and the oyFilterNode_s::sockets and 
 *  oyFilterNode_s::plugs members. \n
 *  - The second is to tell about the connectors capabilities, to allow for 
 *  automatic checking for fittness to other connectors. \n
 *  - The thierd task of this struct is to differenciate between \a input or
 *  \a plug and \a output or \a socket. This is delegated to the
 *  oyFilterSocket_s and oyFilterPlug_s structures.
 *  
 *
 *  \b Routing: \n
 *  The connector output, called socket, side is primarily passive. The data
 *  stream is requested or viewed by the input side, called plug. 
 *  Changes are propagated by events (?). This turns the acyclic graph into a 
 *  looped one. The event use the same graph just in the other direction.
 *  Events and data requests are distingt.
 *  A \a plug local to the filter or filter node can be connected to a remote
 *  \a socket connector and vice versa.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter Node A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter Node B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [label=request];
} 
 \enddot
 * Status information can be passed from the input side to the output side by
 * callbacks.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter Node A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter Node B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowhead=none, arrowtail=normal, label=callback];
} 
 \enddot
 *
 * The data flows from the socket to the plug.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter Node A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter Node B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [label=data];
} 
 \enddot
 *
 * A oyFilterNode_s can have various oyFilterPlug_s ' to obtain data from
 * different sources. The required number is described in the oyCMMapi4_s 
 * structure, which is part of oyFilter_s. But each plug can only connect to
 * one socket.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  b [ label="{<plug> | Filter Node 2 |<socket>}"];
  c [ label="{<plug> | Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 |<socket>}"];

  b:socket -> d:plug [arrowtail=normal, arrowhead=none];
  c:socket -> d:plug [arrowtail=normal, arrowhead=none];
}
 \enddot
 *
 * oyFilterSocket_s is designed to accept arbitrary numbers of connections 
 * to allow for viewing on a filters data output or observe its state changes.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> | Filter Node 1 |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 1| Filter Node 4 |<socket>}"];
  e [ label="{<plug> 1| Filter Node 5 |<socket>}"];

  a:socket -> b:plug [arrowtail=normal, arrowhead=none];
  a:socket -> c:plug [arrowtail=normal, arrowhead=none];
  a:socket -> d:plug [arrowtail=normal, arrowhead=none];
  a:socket -> e:plug [arrowtail=normal, arrowhead=none];
}
 \enddot

 *  @{
 */

/** Function oyConnector_New
 *  @memberof oyConnector_s
 *  @brief   allocate a new Connector object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyConnector_s * OYEXPORT
                   oyConnector_New   ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONNECTOR_S;
# define STRUCT_TYPE oyConnector_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConnector_Copy;
  s->release = (oyStruct_Release_f) oyConnector_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->name.type = oyOBJECT_NAME_S;

  s->is_plug = -1;
  s->max_colour_offset = -1;
  s->min_channels_count = -1;
  s->max_channels_count = -1;
  s->min_colour_count = -1;
  s->max_colour_count = -1;
  s->can_planar = -1;
  s->can_interwoven = -1;
  s->can_swap = -1;
  s->can_swap_bytes = -1;
  s->can_revert = -1;
  s->can_premultiplied_alpha = -1;
  s->can_nonpremultiplied_alpha = -1;
  s->can_subpixel = -1;

  return s;
}

/**
 *  @internal
 *  Function oyConnector_Copy_
 *  @memberof oyConnector_s
 *  @brief   real copy a Connector object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
oyConnector_s * oyConnector_Copy_    ( oyConnector_s     * obj,
                                       oyObject_s          object )
{
  oyConnector_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyConnector_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    s->name.nick = oyStringCopy_( obj->name.nick, allocateFunc_);
    s->name.name = oyStringCopy_( obj->name.name, allocateFunc_);
    s->name.description = oyStringCopy_( obj->name.description, allocateFunc_);

    s->connector_type = obj->connector_type;
    s->is_plug = obj->is_plug;
    if(obj->data_types_n)
    {
      s->data_types = allocateFunc_( obj->data_types_n * sizeof(oyDATATYPE_e) );
      error = !s->data_types;
      error = !memcpy( s->data_types, obj->data_types,
                       obj->data_types_n * sizeof(oyDATATYPE_e) );
      if(!error)
        s->data_types_n = obj->data_types_n;
    }
    s->max_colour_offset = obj->max_colour_offset;
    s->min_channels_count = obj->min_channels_count;
    s->max_channels_count = obj->max_channels_count;
    s->min_colour_count = obj->min_colour_count;
    s->max_colour_count = obj->max_colour_count;
    s->can_planar = obj->can_planar;
    s->can_interwoven = obj->can_interwoven;
    s->can_swap = obj->can_swap;
    s->can_swap_bytes = obj->can_swap_bytes;
    s->can_revert = obj->can_revert;
    s->can_premultiplied_alpha = obj->can_premultiplied_alpha;
    s->can_nonpremultiplied_alpha = obj->can_nonpremultiplied_alpha;
    s->can_subpixel = obj->can_subpixel;
    if(obj->channel_types_n)
    {
      int n = obj->channel_types_n;

      s->channel_types = allocateFunc_( n * sizeof(oyCHANNELTYPE_e) );
      error = !s->channel_types;
      error = !memcpy( s->channel_types, obj->channel_types,
                       n * sizeof(oyCHANNELTYPE_e) );
      if(!error)
        s->channel_types_n = n;
    }
    s->is_mandatory = obj->is_mandatory;
  }

  if(error)
    oyConnector_Release( &s );

  return s;
}

/** Function oyConnector_Copy
 *  @memberof oyConnector_s
 *  @brief   copy or reference a Connector object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyConnector_s * OYEXPORT
                   oyConnector_Copy  ( oyConnector_s     * obj,
                                       oyObject_s          object )
{
  oyConnector_s * s = 0;

  if(!obj)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConnector_Copy_( obj, object );

  return s;
}
 
/** Function oyConnector_Release
 *  @memberof oyConnector_s
 *  @brief   release and possibly deallocate a Connector object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI int  OYEXPORT
               oyConnector_Release   ( oyConnector_s    ** obj )
{
  /* ---- start of common object destructor ----- */
  oyConnector_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_CONNECTOR_S)
  {
    WARNc_S("Attempt to release a non oyConnector_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->name.nick) deallocateFunc( s->name.nick );
    if(s->name.name) deallocateFunc( s->name.name );
    if(s->name.description) deallocateFunc( s->name.description );

    if(s->data_types)
      deallocateFunc( s->data_types ); s->data_types = 0;
    s->data_types_n = 0;

    if(s->channel_types)
      deallocateFunc( s->channel_types ); s->channel_types = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

const char *       oyConnectorEventToText (
                                       oyCONNECTOR_EVENT_e e )
{
  const char * text = "unknown";
  switch(e)
  {
    case oyCONNECTOR_EVENT_OK: text = "oyCONNECTOR_EVENT_OK: kind of ping"; break;
    case oyCONNECTOR_EVENT_CONNECTED: text = "oyCONNECTOR_EVENT_CONNECTED: connection established"; break;
    case oyCONNECTOR_EVENT_RELEASED: text = "oyCONNECTOR_EVENT_RELEASED: released the connection"; break;
    case oyCONNECTOR_EVENT_DATA_CHANGED: text = "oyCONNECTOR_EVENT_DATA_CHANGED: call to update image views"; break;
    case oyCONNECTOR_EVENT_STORAGE_CHANGED: text = "oyCONNECTOR_EVENT_STORAGE_CHANGED: new data accessors"; break;
    case oyCONNECTOR_EVENT_INCOMPATIBLE_DATA: text = "oyCONNECTOR_EVENT_INCOMPATIBLE_DATA: can not process data"; break;
    case oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION: text = "oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION: can not handle option"; break;
    case oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT: text = "oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT: can not handle context"; break;
    case oyCONNECTOR_EVENT_INCOMPLETE_GRAPH: text = "oyCONNECTOR_EVENT_INCOMPLETE_GRAPH: can not completely process"; break;
  }
  return text;
}

/** Function oyFilterSocket_Callback
 *  @memberof oyFilterSocket_s
 *  @brief   tell about a oyConversion_s event
 *
 *  @param[in,out] c                   the connector
 *  @param         e                   the event type
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2008/07/28
 */
OYAPI int  OYEXPORT
                 oyFilterSocket_Callback(
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e )
{
  /* currently catch nothing */

  WARNc2_S("oyFilterSocket_s event: id : %d\n  event: \"%s\"",
            c?oyObject_GetId(c->oy_):-1, oyConnectorEventToText(e) );

  return 0;
}

/** Function oyFilterPlug_Callback
 *  @memberof oyFilterPlug_s
 *  @brief   tell about a oyConversion_s event
 *
 *  @param[in,out] c                   the connector
 *  @param         e                   the event type
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2008/07/28
 */
OYAPI int  OYEXPORT
                 oyFilterPlug_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e )
{
  /* currently catch nothing */

  WARNc2_S("oyFilterPlug_s event: id : %d\n  event: \"%s\"",
            c?oyObject_GetId(c->oy_):-1, oyConnectorEventToText(e) );

  return 0;
}


/** Function oyFilterSocket_New
 *  @memberof oyFilterSocket_s
 *  @brief   allocate a new FilterSocket object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyFilterSocket_s * OYEXPORT
                   oyFilterSocket_New( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_SOCKET_S;
# define STRUCT_TYPE oyFilterSocket_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterSocket_Copy;
  s->release = (oyStruct_Release_f) oyFilterSocket_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->relatives_ = 0;

  return s;
}

/**
 *  @internal
 *  Function oyFilterSocket_Copy_
 *  @memberof oyFilterSocket_s
 *  @brief   real copy a FilterSocket object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
oyFilterSocket_s * oyFilterSocket_Copy_
                                     ( oyFilterSocket_s  * obj,
                                       oyObject_s          object )
{
  oyFilterSocket_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyFilterSocket_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    s->pattern = oyConnector_Copy( obj->pattern, s->oy_ );
    s->node = oyFilterNode_Copy( obj->node, 0 );
    if(obj->data && obj->data->copy)
      s->data = obj->data->copy( obj->data, s->oy_ );
  }

  if(error)
    oyFilterSocket_Release( &s );

  return s;
}

/** Function oyFilterSocket_Copy
 *  @memberof oyFilterSocket_s
 *  @brief   copy or reference a FilterSocket object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyFilterSocket_s * OYEXPORT
                   oyFilterSocket_Copy(oyFilterSocket_s  * obj,
                                       oyObject_s          object )
{
  oyFilterSocket_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_FILTER_SOCKET_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterSocket_Copy_( obj, object );

  return s;
}
 
/** Function oyFilterSocket_Release
 *  @memberof oyFilterSocket_s
 *  @brief   release and possibly deallocate a FilterSocket object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI int  OYEXPORT
               oyFilterSocket_Release( oyFilterSocket_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterSocket_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_FILTER_SOCKET_S)
  {
    WARNc_S("Attempt to release a non oyFilterSocket_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyFilterNode_Release( &s->node );

  {
    int count = oyFilterPlugs_Count( s->requesting_plugs_ ),
        i;
    oyFilterPlug_s * c = 0;
    for(i = 0; i < count; ++i)
    {
      c = oyFilterPlugs_Get( s->requesting_plugs_, i );
      oyFilterPlug_Callback( c, oyCONNECTOR_EVENT_RELEASED );
      oyFilterPlug_Release( &c );
    }
  }

  oyConnector_Release( &s->pattern );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->relatives_)
      deallocateFunc( s->relatives_ );
    s->relatives_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyFilterPlug_ConnectIntoSocket
 *  @memberof oyFilterPlug_s
 *  @brief   connect a oyFilterPlug_s with a oyFilterSocket_s
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/31
 */
OYAPI int  OYEXPORT
                 oyFilterPlug_ConnectIntoSocket (
                                       oyFilterPlug_s   ** p,
                                       oyFilterSocket_s ** s )
{
  oyFilterPlug_s * tp = *p;
  oyFilterSocket_s * ts = *s;
  if(tp->remote_socket_)
    oyFilterSocket_Callback( tp->remote_socket_, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( &tp->remote_socket_ );

  tp->remote_socket_ = *s; *s = 0;
  return !(ts->requesting_plugs_ =
                          oyFilterPlugs_MoveIn( ts->requesting_plugs_, p, -1 ));
}


/** Function oyFilterPlug_New
 *  @memberof oyFilterPlug_s
 *  @brief   allocate a new FilterPlug object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyFilterPlug_s * OYEXPORT
                   oyFilterPlug_New  ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_PLUG_S;
# define STRUCT_TYPE oyFilterPlug_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterPlug_Copy;
  s->release = (oyStruct_Release_f) oyFilterPlug_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->relatives_ = 0;

  return s;
}

/**
 *  @internal
 *  Function oyFilterPlug_Copy_
 *  @memberof oyFilterPlug_s
 *  @brief   real copy a FilterPlug object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
oyFilterPlug_s * oyFilterPlug_Copy_
                                     ( oyFilterPlug_s    * obj,
                                       oyObject_s          object )
{
  oyFilterPlug_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyFilterPlug_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    s->pattern = oyConnector_Copy( obj->pattern, s->oy_ );
    s->node = oyFilterNode_Copy( obj->node, 0 );
  }

  if(error)
    oyFilterPlug_Release( &s );

  return s;
}

/** Function oyFilterPlug_Copy
 *  @memberof oyFilterPlug_s
 *  @brief   copy or reference a FilterPlug object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyFilterPlug_s * OYEXPORT
                   oyFilterPlug_Copy ( oyFilterPlug_s    * obj,
                                       oyObject_s          object )
{
  oyFilterPlug_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_FILTER_PLUG_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterPlug_Copy_( obj, object );

  return s;
}
 
/** Function oyFilterPlug_Release
 *  @memberof oyFilterPlug_s
 *  @brief   release and possibly deallocate a FilterPlug object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI int  OYEXPORT
               oyFilterPlug_Release  ( oyFilterPlug_s   ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterPlug_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_FILTER_PLUG_S)
  {
    WARNc_S(("Attempt to release a non oyFilterPlug_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyFilterNode_Release( &s->node );

  oyFilterSocket_Callback( s->remote_socket_, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( &s->remote_socket_ );

  oyConnector_Release( &s->pattern );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->relatives_)
      deallocateFunc( s->relatives_ );
    s->relatives_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}




/** Function oyFilterPlugs_New
 *  @memberof oyFilterPlugs_s
 *  @brief   allocate a new FilterPlugs list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI oyFilterPlugs_s * OYEXPORT
                   oyFilterPlugs_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_PLUGS_S;
# define STRUCT_TYPE oyFilterPlugs_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterPlugs_Copy;
  s->release = (oyStruct_Release_f) oyFilterPlugs_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/**
 *  @internal
 *  Function oyFilterPlugs_Copy_
 *  @memberof oyFilterPlugs_s
 *  @brief   real copy a FilterPlugs object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
oyFilterPlugs_s * oyFilterPlugs_Copy_
                                     ( oyFilterPlugs_s       * obj,
                                       oyObject_s          object )
{
  oyFilterPlugs_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyFilterPlugs_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );
  }

  if(error)
    oyFilterPlugs_Release( &s );

  return s;
}

/** Function oyFilterPlugs_Copy
 *  @memberof oyFilterPlugs_s
 *  @brief   copy or reference a FilterPlugs list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI oyFilterPlugs_s * OYEXPORT
                   oyFilterPlugs_Copy ( oyFilterPlugs_s       * obj,
                                       oyObject_s          object )
{
  oyFilterPlugs_s * s = 0;

  if(!obj)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterPlugs_Copy_( obj, object );

  return s;
}
 
/** Function oyFilterPlugs_Release
 *  @memberof oyFilterPlugs_s
 *  @brief   release and possibly deallocate a FilterPlugs list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI int  OYEXPORT
               oyFilterPlugs_Release     ( oyFilterPlugs_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterPlugs_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_FILTER_PLUGS_S)
  {
    WARNc_S(("Attempt to release a non oyFilterPlugs_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyFilterPlugs_MoveIn
 *  @memberof oyFilterPlugs_s
 *  @brief   add a element to a FilterPlugs list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI oyFilterPlugs_s * OYEXPORT
                 oyFilterPlugs_MoveIn    ( oyFilterPlugs_s   * list,
                                       oyFilterPlug_s   ** obj,
                                       int                 pos )
{
  oyFilterPlugs_s * s = list;
  int error = !s || s->type_ != oyOBJECT_FILTER_PLUGS_S;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_FILTER_PLUG_S)
  {
    if(!s)
    {
      s = oyFilterPlugs_New(0);
      error = !s;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos );
  }   
  
  return s;
}

/** Function oyFilterPlugs_ReleaseAt
 *  @memberof oyFilterPlugs_s
 *  @brief   release a element from a FilterPlugs list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI int  OYEXPORT
                 oyFilterPlugs_ReleaseAt ( oyFilterPlugs_s * list,
                                       int                 pos )
{ 
  int error = !list;

  if(!error && list->type_ != oyOBJECT_FILTER_PLUGS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function oyFilterPlugs_Get
 *  @memberof oyFilterPlugs_s
 *  @brief   get a element of a FilterPlugs list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterPlugs_Get   ( oyFilterPlugs_s   * list,
                                       int                 pos )
{       
  if(list && list->type_ == oyOBJECT_FILTER_PLUGS_S)
    return (oyFilterPlug_s *) oyStructList_GetRefType( list->list_, pos, oyOBJECT_FILTER_PLUG_S ); 
  else  
    return 0;
}   

/** Function oyFilterPlugs_Count
 *  @memberof oyFilterPlugs_s
 *  @brief   count the elements in a FilterPlugs list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI int  OYEXPORT
                 oyFilterPlugs_Count     ( oyFilterPlugs_s   * list )
{       
  if(list)
    return oyStructList_Count( list->list_ );
  else return 0;
}






/** Function oyFilterRegistrationToText
 *  @brief   analyse registration string
 *
 *  @param         registration        registration string to analyse
 *  @param[in]     type                kind of answere in return
 *  @param[in]     allocateFunc        use this or Oyranos standard allocator
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/11/17
 */
char *         oyFilterRegistrationToText (
                                       const char        * registration,
                                       oyFILTER_REG_e      type,
                                       oyAlloc_f           allocateFunc )
{
  char  * text = 0, * tmp = 0;
  char ** texts = 0;
  int     texts_n = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(registration)
  {
    texts = oyStringSplit_( registration, OY_SLASH_C, &texts_n,oyAllocateFunc_);
    if(texts_n >= type && type == oyFILTER_REG_TOP)
    {
      text = oyStringCopy_( texts[oyFILTER_REG_TOP-1], allocateFunc );

      /** We can not allow attributes in the oyFILTER_TOP_TYPE section, as this
       *  would conflict with the Elektra namespace policy. */
      tmp = oyStrchr_( text, '.' );
      if(tmp)
      {
        /* i18n hint: a string "with '.' is not allowed" */
        WARNc3_S( "oyFILTER_TOP_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        return 0;
      }
    }
    if(texts_n >= type && type == oyFILTER_REG_DOMAIN)
      text = oyStringCopy_( texts[oyFILTER_REG_DOMAIN-1], allocateFunc );
    if(texts_n >= type && type == oyFILTER_REG_TYPE)
    {
      text = oyStringCopy_( texts[oyFILTER_REG_TYPE-1], allocateFunc );

      /** We can not allow attributes in the oyFILTER_REG_TYPE section, as this
       *  would conflict with robust module cache lookup. */
      tmp = oyStrchr_( text, '.' );
      if(tmp)
      {
        WARNc3_S( "oyFILTER_REG_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        return 0;
      }
    }
    if(texts_n >= type && type == oyFILTER_REG_APPLICATION)
      text = oyStringCopy_( texts[oyFILTER_REG_APPLICATION-1], allocateFunc );
    if(texts_n >= type && type == oyFILTER_REG_OPTION)
      text = oyStringCopy_( texts[oyFILTER_REG_OPTION-1], allocateFunc );
    if(text && type == oyFILTER_REG_OPTION)
    {
      tmp = oyStrchr_( text, '.' );
      if(tmp)
        tmp[0] = 0;
    }

    oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  return text;
}

/** Function oyFilterRegistrationMatch
 *  @brief   analyse registration string and compare with a given pattern
 *
 *  The rules are described in the @ref backend_api overview.
 *
 *  @param         registration        registration string to analise
 *  @param         pattern             pattern to compare with
 *  @param         api_number          select object type
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/12/31
 */
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number )
{
  char ** reg_texts = 0;
  int     reg_texts_n = 0;
  char ** regc_texts = 0;
  int     regc_texts_n = 0;
  char  * reg_text = 0;
  char ** p_texts = 0;
  int     p_texts_n = 0;
  char ** pc_texts = 0;
  int     pc_texts_n = 0;
  char  * pc_text = 0;
  int     match = 0, match_tmp = 0, i,j,k, api_num = oyOBJECT_NONE, pc_api_num;
  char    pc_match_type = '+';

  if(registration && pattern)
  {
    if(api_number == oyOBJECT_CMM_API4_S)
      api_num = '4';
    else if(api_number == oyOBJECT_CMM_API5_S)
      api_num = '5';
    else if(api_number == oyOBJECT_CMM_API6_S)
      api_num = '6';
    else if(api_number == oyOBJECT_CMM_API7_S)
      api_num = '7';
    else if(api_number == oyOBJECT_CMM_API8_S)
      api_num = '8';


    match_tmp = 1;
    reg_texts = oyStringSplit_( registration, OY_SLASH_C, &reg_texts_n,
                                oyAllocateFunc_);
    p_texts = oyStringSplit_( pattern, OY_SLASH_C, &p_texts_n, oyAllocateFunc_);

    for( i = 0; i < reg_texts_n && i < p_texts_n; ++i)
    {
      regc_texts_n = 0;
      regc_texts = oyStringSplit_( reg_texts[i],'.',&regc_texts_n,
                                   oyAllocateFunc_);
      pc_texts = oyStringSplit_( p_texts[i],'.',&pc_texts_n, oyAllocateFunc_);

      if(match_tmp && pc_texts_n && regc_texts_n)
      {
        for( j = 0; j < pc_texts_n; ++j)
        {
          match_tmp = 0;
          pc_api_num = 0;
          pc_match_type = '+';
          pc_text = pc_texts[j];

          if(pc_text[0] == '4' ||
             pc_text[0] == '5' ||
             pc_text[0] == '6' ||
             pc_text[0] == '7')
          {
            pc_api_num = pc_text[0];
            ++ pc_text;
            pc_match_type = pc_text[0];
            ++ pc_text;
          }

          for( k = 0; k < regc_texts_n; ++k )
          {
            reg_text = regc_texts[k];
            if((!pc_api_num || (pc_api_num && api_num == pc_api_num)) &&
               oyStrstr_( reg_text, pc_text ))
            {
              if(pc_match_type == '+' ||
                 pc_match_type == '_')
              {
                ++ match;
                match_tmp = 1;
              } else /* if(pc_match_type == '-') */
                goto clean_up;
            }
          }

          if(pc_match_type == '+' && !match_tmp)
            goto clean_up;
        }
      }

      oyStringListRelease_( &pc_texts, pc_texts_n, oyDeAllocateFunc_ );
      oyStringListRelease_( &regc_texts, regc_texts_n, oyDeAllocateFunc_ );
    }
    oyStringListRelease_( &reg_texts, reg_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &p_texts, p_texts_n, oyDeAllocateFunc_ );
  }

  return match;


  clean_up:
      oyStringListRelease_( &pc_texts, pc_texts_n, oyDeAllocateFunc_ );
      oyStringListRelease_( &regc_texts, regc_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &reg_texts, reg_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &p_texts, p_texts_n, oyDeAllocateFunc_ );
  return 0;
}

/**
 *  @internal
 *  Function oyFilter_New_
 *  @memberof oyFilter_s
 *  @brief   allocate and initialise a new filter object
 *
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/24
 */
oyFilter_s * oyFilter_New_           ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_S;
# define STRUCT_TYPE oyFilter_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilter_Copy;
  s->release = (oyStruct_Release_f) oyFilter_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/**
 *  @internal
 *  Function oyFilter_SetCMMapi4_
 *  @memberof oyFilter_s
 *  @brief   lookup and initialise a new filter object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
int          oyFilter_SetCMMapi4_    ( oyFilter_s        * s,
                                       oyCMMapi4_s       * cmm_api4 )
{
  int error = !s;
  oyAlloc_f allocateFunc_ = 0;
  static const char * lang = 0;
  int update = 1;

  if(!error)
    allocateFunc_ = s->oy_->allocateFunc_;

  error = !(cmm_api4 && cmm_api4->type == oyOBJECT_CMM_API4_S);

  if(!error)
  {
    s->registration_ = oyStringCopy_( cmm_api4->registration,
                                      allocateFunc_);
    s->name_ = oyName_copy( &cmm_api4->name, s->oy_ );

    s->category_ = oyStringCopy_( cmm_api4->category, allocateFunc_ );

    /* we lock here as cmm_api4->oyCMMuiGet might not be thread save */
    {
      if(!lang)
        lang = oyLanguage();

      oyObject_Lock( s->oy_, __FILE__, __LINE__ );
      if(oyStrcmp_( oyNoEmptyName_m_(oyLanguage()), lang ) == 0)
        update = 0;

      if(cmm_api4->oyCMMuiGet)
        error = cmm_api4->oyCMMuiGet( s->options_, &s->opts_ui_, allocateFunc_);
      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
    }

    s->api4_ = cmm_api4;
  }

  if(error && s)
    oyFilter_Release( &s );

  return error;
}

/** Function oyFilter_New
 *  @memberof oyFilter_s
 *  @brief   lookup and initialise a new filter object
 *
 *  back end selection: \n
 *  - the user knows, which kind of filter is requested -> registration, e.g. "//color"
 *  - the user probably knows, which special CMM to use (e.g. lcms, icc, shiva)
 *  - the user passes options, which might decide about a type of CMM (e.g. ICC)
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/12/16
 */
oyFilter_s * oyFilter_New            ( const char        * registration,
                                       const char        * cmm_required,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyFilter_s * s = oyFilter_New_( object );
  int error = !s;
  uint32_t ret = 0;
  oyOptions_s * opts_tmp = 0;
  oyCMMapi4_s * api4 = 0;
  oyCMMapiQueries_s * queries = 0;
  char * lib_name = 0;

  if(!error)
  {
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( cmm_required, queries,
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4;
  }

  if(!error)
    error = oyFilter_SetCMMapi4_( s, api4 );

  if(!error)
  {
    opts_tmp = oyOptions_ForFilter_( s, 0, s->oy_);
#if 0
    s->options_ = api4->oyCMMFilter_ValidateOptions( s, options, 0, &ret );
#endif
    error = ret;
    
    /* @todo test oyBOOLEAN_SUBSTRACTION for correctness */
    s->options_ = oyOptions_FromBoolean( opts_tmp, options,
                                         oyBOOLEAN_SUBSTRACTION, s->oy_ );
  }

  if(lib_name)
   oyFree_m_( lib_name );

  if(error && s)
  {
    oyFilter_Release( &s );
    WARNc2_S("could not create filter: \"%s\" \"%s\"",
            oyNoEmptyName_m_(cmm_required), oyNoEmptyName_m_(registration));
  }

  return s;
}

/**
 *  @internal
 *  Function oyFilter_Copy_
 *  @memberof oyFilter_s
 *  @brief   real copy a filter object
 *
 *  @param[in]     filter              filter object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
oyFilter_s * oyFilter_Copy_          ( oyFilter_s        * filter,
                                       oyObject_s          object )
{
  oyFilter_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!filter || !object)
    return s;

  s = oyFilter_New_( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(!error)
  {
    s->registration_ = oyStringCopy_( filter->registration_, allocateFunc_ );
    s->name_ = oyName_copy( filter->name_, s->oy_ );
    s->category_ = oyStringCopy_( filter->category_, allocateFunc_ );
    s->options_ = oyOptions_Copy( filter->options_, s->oy_ );
    s->opts_ui_ = oyStringCopy_( filter->opts_ui_, allocateFunc_ );
    s->api4_ = filter->api4_;
  }

  return s;
}

/** Function oyFilter_Copy
 *  @memberof oyFilter_s
 *  @brief   copy or reference a filter object
 *
 *  @param[in]     filter              filter object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
oyFilter_s * oyFilter_Copy           ( oyFilter_s        * filter,
                                       oyObject_s          object )
{
  oyFilter_s * s = 0;

  if(!filter)
    return s;

  if(filter && !object)
  {
    s = filter;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilter_Copy_( filter, object );

  return s;
}
/** Function oyFilter_Release
 *  @memberof oyFilter_s
 *  @brief   release and zero a filter object
 *
 *  @todo    complete the implementation
 *
 *  @param[in,out] obj                 filter object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
int          oyFilter_Release        ( oyFilter_s       ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilter_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_FILTER_S)
  {
    WARNc_S(("Attempt to release a non oyFilter_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  s->registration_ = 0;

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->category_)
      deallocateFunc( s->category_ ); s->category_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyFilter_GetText
 *  @memberof oyFilter_s
 *  @brief   get text
 *
 *  oyNAME_NAME provides a XML element with child elements and attributes
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
const char * oyFilter_GetText        ( oyFilter_s        * filter,
                                       oyNAME_e            name_type )
{
  char * text = 0;
  oyFilter_s * s = filter;
  int error = !s;

  if(error)
    return 0;

  if(name_type == oyNAME_NAME && !oyObject_GetName(s->oy_, name_type))
  {
    text = oyAllocateWrapFunc_( 512, s->oy_ ? s->oy_->allocateFunc_ : 0 );
    if(!text)
      error = 1;
    sprintf(text, "<oyFilter_s registration=\"%s\" category=\"%s\" version=\"%d.%d.%d\"/>\n",
                  s->registration_,
                  s->category_,
                  s->api4_->version[0],
                  s->api4_->version[1],
                  s->api4_->version[2]
           );

#if 0
    if(!error && filter->profiles_)
    {
      int i = 0, n = oyProfiles_Count(filter->profiles_);
      oyProfile_s * profile = 0;
      for(i = 0; i < n; ++i)
      {
        profile = oyProfiles_Get( filter->profiles_, i );
        sprintf( &text[oyStrlen_(text)], "    %s\n",
        oyNoEmptyName_m_(oyProfile_GetText( profile, oyNAME_NAME)) );
      }
    }
#endif

    if(!error)
      error = oyObject_SetName( s->oy_, text, name_type );

    if(error)
      WARNc_S(("MEM Error."))
  }

  return oyObject_GetName(filter->oy_, name_type);
}

/** Function oyFilter_GetName
 *  @memberof oyFilter_s
 *  @brief   get name
 *
 *  provides the original filter names
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
const char * oyFilter_GetName        ( oyFilter_s        * filter,
                                       oyNAME_e            name_type )
{
  oyFilter_s * s = filter;

  if(!s)
    return 0;

  return oyNoEmptyName_m_( oyName_get_( filter->name_, name_type ) );
}
/** Function oyFilter_CategoryGet
 *  @memberof oyFilter_s
 *  @brief   get category string
 *
 *  @param[in,out] filter              filter object
 *  @param         nontranslated       switch for translation
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
const char * oyFilter_CategoryGet    ( oyFilter_s        * filter,
                                       int                 nontranslated )
{
  if(!filter)
    return 0;

  return filter->category_;
}

oyOptions_s* oyFilter_OptionsSet     ( oyFilter_s        * filter,
                                       oyOptions_s       * options,
                                       int                 flags );
/** Function: oyFilter_OptionsGet
 *  @memberof oyFilter_s
 *  @brief   get filter options
 *
 *  @param[in,out] filter              filter object
 *  @param         flags               possible: OY_FILTER_GET_DEFAULT | oyOPTIONSOURCE_FILTER | oyOPTIONATTRIBUTE_ADVANCED
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOptions_s* oyFilter_OptionsGet     ( oyFilter_s        * filter,
                                       int                 flags )
{
  if(!filter)
    return 0;

  if(flags)
    return oyOptions_ForFilter_( filter, flags, filter->oy_ );
  else
    return oyOptions_Copy( filter->options_, 0 );
}
const char * oyFilter_WidgetsSet     ( oyFilter_s        * filter,
                                       const char        * widgets,
                                       int                 flags );
const char * oyFilter_WidgetsGet     ( oyFilter_s        * filter,
                                       int                 flags );

/** Function: oyFilterNode_ShowConnectorCount
 *  @memberof oyFilterNode_s
 *  @brief   get the connector count from a filter backend
 *
 *  The path to obtain a new connector.
 *  The filter can say it has more connectors to provide for a certain kind of 
 *  static connector eigther described in oyCMMapi4_s::inputs or
 *  oyCMMapi4_s::outputs.
 *
 *  @param       node                the backend filter node
 *  @param       is_plug             select from 0 - plugs or 1 - sockets
 *  @param[out]  last_adds           maximal copies of last connector as suggested by the filter backend
 *  @return                          count of static connectors
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2008/07/28
 */
OYAPI int  OYEXPORT
             oyFilterNode_ShowConnectorCount( 
                                       oyFilterNode_s    * node,
                                       int                 is_plug,
                                       uint32_t          * last_adds )
{
  int n = 0;

  if(!node || node->type_ != oyOBJECT_FILTER_NODE_S ||
     !node->api7_)
    return n;

  if(is_plug)
  {
    n = node->api7_->plugs_n;
    if(last_adds)
      *last_adds = node->api7_->plugs_last_add;
  } else {
    n = node->api7_->sockets_n;
    if(last_adds)
      *last_adds = node->api7_->sockets_last_add;
  }

  return n;
}



/** Function: oyFilters_New
 *  @memberof oyFilters_s
 *  @brief   allocate a new Filters list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
OYAPI oyFilters_s * OYEXPORT
                   oyFilters_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTERS_S;
# define STRUCT_TYPE oyFilters_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilters_Copy;
  s->release = (oyStruct_Release_f) oyFilters_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/**
 *  @internal
 *  Function: oyFilters_Copy_
 *  @memberof oyFilters_s
 *  @brief   real copy a Filters object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
oyFilters_s * oyFilters_Copy_
                                     ( oyFilters_s       * obj,
                                       oyObject_s          object )
{
  oyFilters_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyFilters_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );
  }

  if(error)
    oyFilters_Release( &s );

  return s;
}

/** Function: oyFilters_Copy
 *  @memberof oyFilters_s
 *  @brief   copy or reference a Filters list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
OYAPI oyFilters_s * OYEXPORT
                   oyFilters_Copy    ( oyFilters_s       * obj,
                                       oyObject_s          object )
{
  oyFilters_s * s = 0;

  if(!obj)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilters_Copy_( obj, object );

  return s;
}
 
/** Function: oyFilters_Release
 *  @memberof oyFilters_s
 *  @brief   release and possibly deallocate a Filters list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
OYAPI int  OYEXPORT
               oyFilters_Release     ( oyFilters_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilters_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_FILTERS_S)
  {
    WARNc_S(("Attempt to release a non oyFilters_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function: oyFilters_MoveIn
 *  @memberof oyFilters_s
 *  @brief   add a element to a Filters list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
OYAPI oyFilters_s * OYEXPORT
                 oyFilters_MoveIn    ( oyFilters_s   * list,
                                       oyFilter_s   ** obj,
                                       int                 pos )
{
  oyFilters_s * s = list;
  int error = !s || s->type_ != oyOBJECT_FILTERS_S;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_FILTER_S)
  {
    if(!s)
    {
      s = oyFilters_New(0);
      error = !s;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos );
  }   
  
  return s;
}

/** Function: oyFilters_ReleaseAt
 *  @memberof oyFilters_s
 *  @brief   release a element from a Filters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
OYAPI int  OYEXPORT
                  oyFilters_ReleaseAt( oyFilters_s       * list,
                                       int                 pos )
{ 
  int error = !list;

  if(!error && list->type_ != oyOBJECT_FILTERS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function: oyFilters_Get
 *  @memberof oyFilters_s
 *  @brief   get a element of a Filters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
OYAPI oyFilter_s * OYEXPORT
                 oyFilters_Get       ( oyFilters_s       * list,
                                       int                 pos )
{       
  if(list && list->type_ == oyOBJECT_FILTERS_S)
    return (oyFilter_s *) oyStructList_GetRefType( list->list_, pos, oyOBJECT_FILTER_S ); 
  else  
    return 0;
}   

/** Function: oyFilters_Count
 *  @memberof oyFilters_s
 *  @brief   count the elements in a Filters list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
OYAPI int  OYEXPORT
                 oyFilters_Count     ( oyFilters_s       * list )
{       
  if(list)
    return oyStructList_Count( list->list_ );
  else return 0;
}



/** Function: oyFilterNode_New
 *  @memberof oyFilterNode_s
 *  @brief   allocate and initialise a new filter node object
 *
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
oyFilterNode_s *   oyFilterNode_New  ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_NODE_S;
# define STRUCT_TYPE oyFilterNode_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterNode_Copy;
  s->release = (oyStruct_Release_f) oyFilterNode_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->relatives_ = 0;

  if( s->backend_data && s->backend_data->release )
    s->backend_data->release( (oyStruct_s**) & s->backend_data );
  s->backend_data = 0;


  if(error)
    oyFilterNode_Release( &s );

  return s;
}

/** Function: oyFilterNode_Create
 *  @memberof oyFilterNode_s
 *  @brief   initialise a new filter node object properly
 *
 *  @param         filter              the mandatory filter
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/30
 TODO select oyCMMapi7_s over registration string */
oyFilterNode_s *   oyFilterNode_Create(oyFilter_s        * filter,
                                       oyObject_s          object )
{
  oyFilterNode_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!filter)
    return s;

  s = oyFilterNode_New( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(!error)
  {
    s->filter = oyFilter_Copy( filter, object );

    s->api7_ = (oyCMMapi7_s*) oyCMMsGetFilterApi_( 0, 0,
                                   filter->registration_, oyOBJECT_CMM_API7_S );

    if(s->filter)
    {
      size_t len = sizeof(oyFilterSocket_s*) *
             (s->api7_->sockets_n + s->api7_->sockets_last_add
              + 1);
      len = len?len:sizeof(oyFilterSocket_s*);
      s->sockets = allocateFunc_( len );
      memset( s->sockets, 0, len );

      len = sizeof(oyFilterSocket_s*) *
            (s->api7_->plugs_n + s->api7_->plugs_last_add + 1);
      len = len?len:sizeof(oyFilterSocket_s*);
      s->plugs = allocateFunc_( len );
      memset( s->plugs, 0, len );

      s->relatives_ = allocateFunc_( oyStrlen_(filter->category_) + 24 );
      oySprintf_( s->relatives_, "%d: %s", oyObject_GetId(filter->oy_), s->filter->category_);
    }
  }

  return s;
}

/**
 *  @internal
 *  Function: oyFilterNode_Copy_
 *  @memberof oyFilterNode_s
 *  @brief   real copy a filter node object
 *
 *  @param[in]     node                node filter object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
oyFilterNode_s *   oyFilterNode_Copy_( oyFilterNode_s    * node,
                                       oyObject_s          object )
{
  oyFilterNode_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!node || !object)
    return s;

  s = oyFilterNode_Create( node->filter, object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;


  if(!error)
  {
    if(!error && node->backend_data && node->backend_data->copy)
      s->backend_data = (oyCMMptr_s*) node->backend_data->copy( (oyStruct_s*)
                                                  node->backend_data , s->oy_ );
  }

  return s;
}

/** Function: oyFilterNode_Copy
 *  @memberof oyFilterNode_s
 *  @brief   copy or reference a filter node object
 *
 *  @param[in]     node                node object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
oyFilterNode_s * oyFilterNode_Copy   ( oyFilterNode_s    * node,
                                       oyObject_s          object )
{
  oyFilterNode_s * s = 0;

  if(!node)
    return s;

  if(node && !object)
  {
    s = node;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterNode_Copy_( node, object );

  return s;
}
/** Function: oyFilterNode_Release
 *  @memberof oyFilterNode_s
 *  @brief   release and zero a filter node object
 *
 *  @param[in,out] obj                 node object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
int          oyFilterNode_Release    ( oyFilterNode_s   ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterNode_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_FILTER_NODE_S)
  {
    WARNc_S(("Attempt to release a non oyFilterNode_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    uint32_t i;

    if(s->sockets)
    for(i = 0;
        i < s->api7_->sockets_n + s->api7_->sockets_last_add;
        ++i)
      oyFilterSocket_Release( &s->sockets[i] );

    for(i = 0;
        i < s->api7_->plugs_n + s->api7_->plugs_last_add;
        ++i)
      oyFilterPlug_Release( &s->plugs[i] );

    if(s->relatives_)
      deallocateFunc( s->relatives_ );
    s->relatives_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function: oyFilterNode_ShowConnector
 *  @memberof oyFilterNode_s
 *  @brief   get a connector description from a filter backend
 *
 *  The path to obtain a new connector.
 *  The filter can say it has more connectors to provide for a certain kind of 
 *  static connector eigther described in oyCMMapi4_s::inputs or
 *  oyCMMapi4_s::outputs.
 *
 *  @param[in]   node                the backend filter node
 *  @param[in]   as_pos              the according oyConnector_s
 *  @param[in]   is_plug             select from 0 - plugs or 1 - sockets
 *  @return                          the new oyConnector_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2008/12/27
 */
OYAPI oyConnector_s * OYEXPORT
             oyFilterNode_ShowConnector (
                                       oyFilterNode_s    * node,
                                       int                 as_pos,
                                       int                 is_plug )
{
  oyConnector_s * pattern = 0;
  oyObject_s object = 0;

  if(!node || !node->filter || node->type_ != oyOBJECT_FILTER_NODE_S ||
     !node->api7_)
    return 0;

  object = oyObject_New ();

  if(node->api7_->plugs_n < as_pos &&
     as_pos < node->api7_->plugs_n + node->api7_->plugs_last_add )
    as_pos = node->api7_->plugs_n - 1;

  {
    if(is_plug)
    {
      if(node->api7_->plugs_n > as_pos)
        pattern = oyConnector_Copy( node->api7_->plugs[as_pos], object );
    } else {
      if(node->api7_->sockets_n > as_pos)
        pattern = oyConnector_Copy( node->api7_->sockets[as_pos], object );
    }
  }

  oyObject_Release( &object );

  return pattern;
}

/** Function: oyFilterNode_ConnectorMatch
 *  @memberof oyFilterNode_s
 *  @brief   check if a connector match to a FilterNode
 *
 *  @param         node_first          first node
 *  @param         pos_first           position of connector from first filter
 *  @param         connector_second    second connector
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI int  OYEXPORT
                 oyFilterNode_ConnectorMatch (
                                       oyFilterNode_s    * node_first,
                                       int                 pos_first,
                                       oyConnector_s     * connector_second )
{
  int match = 0;
  oyConnector_s * a = 0,  * b = connector_second;
  oyImage_s * image = 0;
  int colours_n = 0, n, i, j;
  int coff = 0;
  oyDATATYPE_e data_type = 0;

  if(node_first && node_first->type_ == oyOBJECT_FILTER_NODE_S &&
     node_first->filter)
    a = oyFilterNode_ShowConnector( node_first, pos_first, 0 );

  if(a && b && b->type_ == oyOBJECT_CONNECTOR_S)
  {
    oyFilterSocket_s * sock_first = oyFilterNode_GetSocket( node_first, pos_first );
    match = 1;
    image = oyImage_Copy( (oyImage_s*)sock_first->data, 0 );

    if(!b->is_plug)
      match = 0;

    if(!image) match = 0;
    else if(match)
    {
      coff = oyToColourOffset_m( image->layout_[oyLAYOUT] );

      /* channel counts */
      colours_n = oyProfile_GetChannelsCount( image->profile_ );
      if(image->layout_[oyCHANS] < b->min_channels_count ||
         image->layout_[oyCHANS] > b->max_channels_count ||
         colours_n < b->min_colour_count ||
         colours_n > b->max_colour_count)
        match = 0;

      /* data types */
      if(match)
      {
        data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
        n = b->data_types_n;
        match = 0;
        for(i = 0; i < n; ++i)
          if(b->data_types[i] == data_type)
            match = 1;
      }

      /* planar and interwoven capabilities */
      if(b->max_colour_offset < image->layout_[oyCOFF] ||
         (!b->can_planar && oyToPlanar_m(image->layout_[oyCOFF])) ||
         (!b->can_interwoven && !oyToPlanar_m(image->layout_[oyCOFF])))
        match = 0;

      /* swap and byteswapping capabilities */
      if((!b->can_swap && oyToSwapColourChannels_m(image->layout_[oyCOFF])) ||
         (!b->can_swap_bytes && oyToByteswap_m(image->layout_[oyCOFF])))
        match = 0;

      /* revert or chockolat and vanilla */
      if((!b->can_revert && oyToFlavor_m(image->layout_[oyCOFF])))
        match = 0;

      /* channel types */
      if(match && b->channel_types)
      {
        n = image->layout_[oyCHANS];
        for(i = 0; i < b->channel_types_n; ++i)
        {
          match = 0;
          for(j = 0; j < n; ++j)
            if(b->channel_types[i] == image->channel_layout[j] &&
               !(!b->can_nonpremultiplied_alpha &&
                 image->channel_layout[j] == oyCHANNELTYPE_COLOUR_LIGHTNESS) &&
               !(!b->can_premultiplied_alpha &&
                 image->channel_layout[j] == oyCHANNELTYPE_COLOUR_LIGHTNESS_PREMULTIPLIED))
              match = 1;
          if(!match)
            break;
        }
      }

      /* subpixels */
      if(image->sub_positioning && !b->can_subpixel)
        match = 0;
    }
  }

  oyImage_Release( &image );
  oyConnector_Release( &a );

  return match;
}

/** Function: oyFilterNode_GetSocket
 *  @memberof oyFilterNode_s
 *  @brief   get a oyFilterSocket_s of type from a FilterNode
 *
 *  @param         node                filter node
 *  @param         pos                 position of connector from filter
 *  @return                            the socket - no copy!
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/30
 */
OYAPI oyFilterSocket_s * OYEXPORT
                 oyFilterNode_GetSocket (
                                       oyFilterNode_s    * node,
                                       int                 pos )
{
  oyFilterSocket_s * s = 0;

  if(node && node->type_ == oyOBJECT_FILTER_NODE_S &&
     pos < node->api7_->sockets_n + node->api7_->sockets_last_add)
  {
    oyAlloc_f allocateFunc_ = node->oy_->allocateFunc_;

    if(!node->sockets)
    {
      size_t len = sizeof(oyFilterSocket_s*) *
       (node->api7_->sockets_n + node->api7_->sockets_last_add
        + 1);
      node->sockets = allocateFunc_( len );
      memset( node->sockets, 0, len );
    }

    if(!node->sockets[pos])
    {
      s = oyFilterSocket_New( node->oy_ );
      s->pattern = oyFilterNode_ShowConnector( node, pos, 0 );
      s->node = oyFilterNode_Copy( node, 0 );
      s->relatives_ = oyStringCopy_( node->relatives_, allocateFunc_ );
      node->sockets[pos] = s;
    }

    s = node->sockets[pos];
  }

  return s;
}

/** Function: oyFilterNode_GetPlug
 *  @memberof oyFilterNode_s
 *  @brief   get a oyFilterPlug_s of type from a FilterNode
 *
 *  @param         node                filter node
 *  @param         pos                 position of connector from filter
 *  @return                            the plug - no copy
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/30
 */
OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterNode_GetPlug( oyFilterNode_s    * node,
                                       int                 pos )
{
  oyFilterPlug_s * s = 0;

  if(node && node->type_ == oyOBJECT_FILTER_NODE_S &&
     pos < node->api7_->plugs_n + node->api7_->plugs_last_add)
  {
    oyAlloc_f allocateFunc_ = node->oy_->allocateFunc_;

    if(!node->plugs)
    {
      size_t len = sizeof(oyFilterPlug_s*) *
           (node->api7_->plugs_n + node->api7_->plugs_last_add
            + 1);
      node->plugs = allocateFunc_( len );
      memset( node->plugs, 0, len );
    }

    if(!node->plugs[pos])
    {
      s = oyFilterPlug_New( node->oy_ );
      s->pattern = oyFilterNode_ShowConnector( node, pos, 1 );
      s->node = oyFilterNode_Copy( node, 0 );
      s->relatives_ = oyStringCopy_( node->relatives_, allocateFunc_ );
      node->plugs[pos] = s;
    }

    s = node->plugs[pos];
  }

  return s;
}

/** Function oyFilterNode_GetText
 *  @memberof oyFilterNode_s
 *  @brief   serialise filter node to text
 *
 *  Serialise into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML
 *  - oyNAME_DESCRIPTION: ??
 *
 *  This function provides a complete description of the context. It might be 
 *  more adequate to use only a subset for hashing as not all data and options
 *  might have an effect to the context data result. 
 *  The oyCMMapi4_s::oyCMMFilterNode_GetText() function provides a way to let a
 *  module decide about what to place into a hash text.
 *
 *  @param[in,out] node                filter node
 *  @param[out]    name_type           the type
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
const char * oyFilterNode_GetText    ( oyFilterNode_s    * node,
                                       oyNAME_e            name_type )
{
  const char * tmp = 0;
  char * hash_text = 0;
  oyFilterNode_s * s = node;

  oyStructList_s * in_datas = 0,
                 * out_datas = 0;

  if(!node)
    return 0;

  /* 1. create hash text */
  hashTextAdd_m( "<oyFilterNode_s>\n  " );

  /* the filter text */
  hashTextAdd_m( oyFilter_GetText( node->filter, oyNAME_NAME ) );

  /* pick all plug (input) data */
  in_datas = oyFilterNode_DataGet_( node, 1 );

  /* pick all sockets (output) data */
  out_datas = oyFilterNode_DataGet_( node, 0 );

  /* make a description */
  tmp = oyContextCollectData_( (oyStruct_s*)node, s->filter->options_,
                               in_datas, out_datas );
  hashTextAdd_m( tmp );

  hashTextAdd_m( "</oyFilterNode_s>\n" );


  oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );

  if(s->oy_->deallocateFunc_)
    s->oy_->deallocateFunc_( hash_text );
  hash_text = 0;

  hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NICK );

  return hash_text;
}

/** 
 *  @internal
 *  Info profilbody */
char info_profile_data[320] =
  {
/*0*/    0,0,1,64, 'o','y','r','a',
    2,48,0,0, 'n','o','n','e',
    'R','G','B',32, 'L','a','b',32,
    0,0,0,0,0,0,0,0,
/*32*/    0,0,0,0,97,99,115,112,
    '*','n','i','x',0,0,0,0,
    110,111,110,101,110,111,110,101,
    -64,48,11,8,-40,-41,-1,-65,
/*64*/    0,0,0,0,0,0,-10,-42,
    0,1,0,0,0,0,-45,45,
    'o','y','r','a',0,0,0,0,
    0,0,0,0,0,0,0,0,
/*96*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
/*128*/    0,0,0,3,'d','e','s','c',
    0,0,0,-88,0,0,0,33,
    'c','p','r','t',0,0,0,-52,
    0,0,0,29,'I','n','f','o',
/*160*/    0,0,0,-20,0,0,0,0,
    't','e','x','t',0,0,0,0,
    'F','i','l','t','e','r',' ','I',
    'n','f','o',' ','X','M','L',0,
/*192*/    0,0,0,0,0,0,0,0,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,110,111,116,32,
    99,111,112,121,114,105,103,104,
/*224*/    116,101,100,32,100,97,116,97,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,'s','t','a','r',
    't',0,0,0,0,0,0,0,
/*256*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
  };

/** Function oyFilterNode_TextToInfo
 *  @memberof oyFilterNode_s
 *  @brief   serialise filter node to binary
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" text tag.
 *  Not useable for binary contexts.
 *
 *  @param[in,out] node                filter node
 *  @param[out]    size                output size
 *  @param[in]     allocateFunc        memory allocator
 *  @return                            the profile container
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyPointer    oyFilterNode_TextToInfo ( oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer ptr = 0;
  icHeader * header = 0;
  size_t len = 244, text_len = 0;
  char * text = 0;
  const char * temp = 0;
  uint32_t * mem = 0;

  if(!node)
    return 0;

  temp = oyFilterNode_GetText( node, oyNAME_NAME );

  text_len = strlen(temp) + 1;
  len += text_len + 1;
  len = len > 320 ? len : 320;
  ptr = allocateFunc(len);
  header = ptr;
  
  if(ptr)
  { 
    *size = len;
    memset(ptr,0,len);
    memcpy(ptr, info_profile_data, 320);

    text = ((char*)ptr)+244;
    sprintf(text, "%s", temp);
    header->size = oyValueUInt32( len );
    mem = ptr;
    mem[41] = oyValueUInt32( text_len + 8 );
  }

  return ptr;
}

/**
 *  @internal
 *  Function: oyFilterNode_GetNextFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief   get next node from a linear graph 
 *
 *  @param[in]     first               filter
 *  @return                            next node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
oyFilterNode_s *   oyFilterNode_GetNextFromLinear_ (
                                       oyFilterNode_s    * first )
{
      oyFilterNode_s * next = 0;
      oyFilterSocket_s * socket = 0;
      oyFilterPlug_s * plug = 0;

      {
        socket = first->sockets[0];

        if(socket)
          plug = oyFilterPlugs_Get( socket->requesting_plugs_, 0 );
        if(plug)
          next = plug->node;
        else
          next = 0;
        oyFilterPlug_Release( &plug );
      }

  return next;
}

/**
 *  @internal
 *  Function: oyFilterNode_GetLastFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief   get last node from a linear graph 
 *
 *  @param[in]     first               filter
 *  @return                            last node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/16 (Oyranos: 0.1.8)
 *  @date    2008/07/16
 */
oyFilterNode_s *   oyFilterNode_GetLastFromLinear_ (
                                       oyFilterNode_s    * first )
{
  oyFilterNode_s * next = 0,
                 * last = 0;

      next = last = first;

      while(next)
      {
        next = oyFilterNode_GetNextFromLinear_( next );

        if(next)
          last = next;
      }

  return last;
}

/**
 *  @internal
 *  Function oyFilterNode_DataGet_
 *  @memberof oyFilterNode_s
 *  @brief   get the processing data from a filter node
 *
 *  @param[in]     node                filter
 *  @param[in]     get_plug            1 get input, 0 get output data
 *  @return                            the data list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 *  @date    2008/11/04
 */
oyStructList_s * oyFilterNode_DataGet_(oyFilterNode_s    * node,
                                       int                 get_plug )
{
  int error = !node;
  oyStructList_s * datas = 0;
  oyStruct_s * data = 0;
  int i;

  if(!error)
  {
    datas = oyStructList_New(0);

    if(get_plug)
    {
          /* pick all plug (input) data */
          i = 0;
          while( node->plugs[i] && !error )
          {
            data = 0;
            if(node->plugs[i]->remote_socket_->data)
              data = node->plugs[i]->remote_socket_->data->copy( node->plugs[i]->remote_socket_->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0, 0);
            error = oyStructList_MoveIn( datas, &data, -1 );
            ++i;
          }
    } else
    {
          /* pick all sockets (output) data */
          i = 0;
          while( node->sockets[i] && !error )
          {
            data = 0;
            if(node->sockets[i]->data)
              data = node->sockets[i]->data->copy( node->sockets[i]->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0, 0);
            error = oyStructList_MoveIn( datas, &data, -1 );
            ++i;
          }

    }
  }

  return datas;
}

/** @internal
 *  @brief   wrapper for oyDeAllocateFunc_
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
int oyPointerRelease                 ( oyPointer         * ptr )
{
  if(ptr && *ptr)
  {
    oyDeAllocateFunc_(*ptr);
    *ptr = 0;
    return 0;
  }
  return 1;
}


/**
 *  @internal
 *  Function oyFilterNode_ContextSet_
 *  @memberof oyFilterNode_s
 *  @brief   set backend context in a filter 
 *
 *  The api4 data is passed to a interpolator specific transformer. The result
 *  of this transformer will on request be cached by Oyranos as well.
 *
 *  @param[in]     node                filter
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
int          oyFilterNode_ContextSet_( oyFilterNode_s    * node )
{
  int error = 0;
  oyFilter_s * s = node->filter;

  if(!error)
  {
          size_t size = 0;
          oyHash_s * hash = 0;
          const char * hash_text_ = 0;
          char * hash_text = 0,
               * hash_temp = 0;
          oyPointer ptr = 0;
          oyCMMptr_s * cmm_ptr = 0,
                     * cmm_ptr_out = 0;


          /*  Cache Search
           *  1.     hash from input
           *  2.     query for hash in cache
           *  3.     check
           *  3a.       eighter take cache entry
           *  3b.       or ask CMM
           *  3b.1.                update cache entry
           */

          /* 1. create hash text */
          if(s->api4_->oyCMMFilterNode_GetText)
          {
            hash_temp = s->api4_->oyCMMFilterNode_GetText( node, oyNAME_NICK,
                                                           oyAllocateFunc_ );
            hash_text_ = hash_temp;
          } else
            hash_text_ = oyFilterNode_GetText( node, oyNAME_NICK );

          hash_text = oyStringCopy_( s->api4_->context_type, oyAllocateFunc_ );
          STRING_ADD( hash_text, ":" );
          STRING_ADD( hash_text, hash_text_ );

          if(oy_debug > 3)
          {
            size = 0;
            ptr = oyFilterNode_TextToInfo ( node, &size, oyAllocateFunc_ );
            if(ptr)
              oyWriteMemToFile_( "test_dbg_colour.icc", ptr, size );
          }

          /* 2. query in cache */
          hash = oyCMMCacheListGetEntry_( hash_text );

          if(!error)
          {
            /* 3. check and 3.a take*/
            cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( hash,
                                                        oyOBJECT_CMM_POINTER_S);

            if(!cmm_ptr)
            {
              size = 0;
              cmm_ptr = oyCMMptr_New_(oyAllocateFunc_);

              /* 3b. ask CMM */
              ptr = s->api4_->oyCMMFilterNode_ContextToMem( node, &size,
                                                            oyAllocateFunc_ );

              error = oyCMMptr_Set_( cmm_ptr, s->api4_->id_,
                                     s->api4_->context_type,
                                     ptr, "oyPointerRelease", oyPointerRelease);
              cmm_ptr->size = size;

              if(!error && cmm_ptr && cmm_ptr->ptr)
              {
                if( oyStrcmp_( node->api7_->context_type,
                               s->api4_->context_type ) != 0 )
                {
                  cmm_ptr_out = oyCMMptr_New_(oyAllocateFunc_);
                  error = oyCMMptr_Set_( cmm_ptr_out, node->api7_->id_,
                                         node->api7_->context_type, 0, 0, 0);

                  /* search for a convertor and convert */
                  oyCMMptr_ConvertData( cmm_ptr, cmm_ptr_out, node );
                  node->backend_data = cmm_ptr_out;
                } else
                  node->backend_data = oyCMMptr_Copy_( cmm_ptr, 0 );
              }

              /* 3b.1. update cache entry */
              error = oyHash_SetPointer_( hash, (oyStruct_s*) cmm_ptr);
              cmm_ptr = 0;
            }
          }

          if(oy_debug > 3)
          {
            if(ptr && size && node->backend_data)
              oyWriteMemToFile_( "test_dbg_colour_dl.icc", ptr, size );
          }

  }

  return error;
}



/** Function oyConfig_New
 *  @memberof oyConfig_s
 *  @brief   allocate a new Config object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/15 (Oyranos: 0.1.10)
 *  @date    2009/01/15
 */
OYAPI oyConfig_s * OYEXPORT
                   oyConfig_New      ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIG_S;
# define STRUCT_TYPE oyConfig_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConfig_Copy;
  s->release = (oyStruct_Release_f) oyConfig_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->options = oyOptions_New( s->oy_ );

  return s;
}

/** @internal
 *  Function oyConfig_Copy_
 *  @memberof oyConfig_s
 *  @brief   real copy a Config object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/15 (Oyranos: 0.1.10)
 *  @date    2009/01/15
 */
oyConfig_s * oyConfig_Copy_          ( oyConfig_s        * obj,
                                       oyObject_s          object )
{
  oyConfig_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyConfig_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    s->options = oyOptions_Copy( obj->options, s->oy_ );
  }

  if(error)
    oyConfig_Release( &s );

  return s;
}

/** Function oyConfig_Copy
 *  @memberof oyConfig_s
 *  @brief   copy or reference a Config object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/15 (Oyranos: 0.1.10)
 *  @date    2009/01/15
 */
OYAPI oyConfig_s * OYEXPORT
                   oyConfig_Copy     ( oyConfig_s        * obj,
                                       oyObject_s          object )
{
  oyConfig_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_CONFIG_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConfig_Copy_( obj, object );

  return s;
}
 
/** Function oyConfig_Release
 *  @memberof oyConfig_s
 *  @brief   release and possibly deallocate a Config object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/15 (Oyranos: 0.1.10)
 *  @date    2009/01/15
 */
OYAPI int  OYEXPORT
               oyConfig_Release      ( oyConfig_s       ** obj )
{
  /* ---- start of common object destructor ----- */
  oyConfig_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_CONFIG_S)
  {
    WARNc_S(("Attempt to release a non oyConfig_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyOptions_Release( &s->options );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyConfigs_New
 *  @memberof oyConfigs_s
 *  @brief   allocate a new Configs list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI oyConfigs_s * OYEXPORT
                   oyConfigs_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIGS_S;
# define STRUCT_TYPE oyConfigs_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConfigs_Copy;
  s->release = (oyStruct_Release_f) oyConfigs_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_New( 0 );

  return s;
}

/** Function oyConfigs_NewFromDomain
 *  @memberof oyConfigs_s
 *  @brief   allocate a new Config object with empty options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI oyConfigs_s * OYEXPORT
               oyConfigs_NewFromDomain(const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyConfigs_s * s = 0;
  int error = !registration_domain;
  oyCMMapi8_s * cmm_api8 = 0;

  if(!error)
  {
    s = oyConfigs_New( object );
    error = !s;
  }
  /**
   *  1. first we search for oyCMMapi8_s complex config support matching to our
   *     registration_domain
   *  2. if we find a backend, we ask for the options
   *  3. add the options to the config (in the backend)
   */
  if(!error)
  {
    cmm_api8 = (oyCMMapi8_s*) oyCMMsGetFilterApi_( 0,0, registration_domain,
                                                     oyOBJECT_CMM_API8_S );
    error = !cmm_api8;
  }

  if(!error)
    s = cmm_api8->oyConfigs_FromPattern( registration_domain, options );

  return s;
}

/** @internal
 *  Function oyConfigs_Copy_
 *  @memberof oyConfigs_s
 *  @brief   real copy a Configs object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
oyConfigs_s * oyConfigs_Copy_
                                     ( oyConfigs_s       * obj,
                                       oyObject_s          object )
{
  oyConfigs_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyConfigs_New( object );
  error = !s;

  if(!error)
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );

  if(error)
    oyConfigs_Release( &s );

  return s;
}

/** Function oyConfigs_Copy
 *  @memberof oyConfigs_s
 *  @brief   copy or reference a Configs list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI oyConfigs_s * OYEXPORT
                   oyConfigs_Copy ( oyConfigs_s       * obj,
                                       oyObject_s          object )
{
  oyConfigs_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_CONFIGS_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConfigs_Copy_( obj, object );

  return s;
}
 
/** Function oyConfigs_Release
 *  @memberof oyConfigs_s
 *  @brief   release and possibly deallocate a Configs list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
               oyConfigs_Release     ( oyConfigs_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyConfigs_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_CONFIGS_S)
  {
    WARNc_S(("Attempt to release a non oyConfigs_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyConfigs_MoveIn
 *  @memberof oyConfigs_s
 *  @brief   add a element to a Configs list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
                 oyConfigs_MoveIn    ( oyConfigs_s   * list,
                                       oyConfig_s   ** obj,
                                       int                 pos )
{
  oyConfigs_s * s = list;
  int error = !s || s->type_ != oyOBJECT_CONFIGS_S;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_CONFIG_S)
  {
    if(!s)
    {
      s = oyConfigs_New(0);
      error = !s;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_New( 0 );
      error = !s->list_;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos );
  }   
  
  return error;
}

/** Function oyConfigs_ReleaseAt
 *  @memberof oyConfigs_s
 *  @brief   release a element from a Configs list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
                  oyConfigs_ReleaseAt ( oyConfigs_s * list,
                                       int                 pos )
{ 
  int error = !list;

  if(!error && list->type_ != oyOBJECT_CONFIGS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function oyConfigs_Get
 *  @memberof oyConfigs_s
 *  @brief   get a element of a Configs list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI oyConfig_s * OYEXPORT
                 oyConfigs_Get       ( oyConfigs_s   * list,
                                       int                 pos )
{       
  if(list && list->type_ == oyOBJECT_CONFIGS_S)
    return (oyConfig_s *) oyStructList_GetRefType( list->list_, pos, oyOBJECT_CONFIG_S ); 
  else  
    return 0;
}   

/** Function oyConfigs_Count
 *  @memberof oyConfigs_s
 *  @brief   count the elements in a Configs list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
                 oyConfigs_Count     ( oyConfigs_s   * list )
{       
  if(list && list->type_ == oyOBJECT_CONFIGS_S)
    return oyStructList_Count( list->list_ );
  else return 0;
}

/** Function oyConfigDomainList
 *  @memberof oyConfigs_s
 *  @brief   count and show the global oyConfigs_s suppliers
 *
 *  @param[in]     registration_pattern a optional filter
 *  @param[out]    list                the list
 *  @param[out]    count               the list count
 *  @param[out]    rank_list           the rank fitting to list
 *  @param[in]     allocateFunc        the user allocator for list
 *  @return                            0 - good, >= 1 - error, <= -1 unknown
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
                 oyConfigDomainList  ( const char        * registration_pattern,
                                       char            *** list,
                                       int               * count,
                                       int              ** rank_list,
                                       oyAlloc_f           allocateFunc )
{
  oyCMMapiFilter_s ** apis = 0;
  int error = !list || !count;
  char ** reg_lists = 0;
  int i = 0,
      reg_list_n = 0,
      apis_n = 0;

  if(!error)
  {
    apis = oyCMMsGetFilterApis_( 0, 0, registration_pattern,
                                 oyOBJECT_CMM_API8_S,
                                 rank_list, &apis_n);
    error = !apis;
  }

  if(!error)
  {
    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    for(i = 0; i < apis_n; ++i)
      oyStringListAddStaticString_( &reg_lists, &reg_list_n,
                                    oyNoEmptyString_m_(apis[i]->registration),
                                    oyAllocateFunc_, oyDeAllocateFunc_ );
    if(reg_list_n && reg_lists)
      *list = oyStringListAppend_( (const char**)reg_lists, reg_list_n, 0,0,
                                   &reg_list_n, allocateFunc );

    oyStringListRelease_( &reg_lists, reg_list_n, oyDeAllocateFunc_ );
  }

  if(count)
    *count = reg_list_n;

  return error;
}




/** @internal
 *  @brief   create and possibly precalculate a transform for a given image
 *  @memberof oyColourConversion_s

 *  @param[in]     opts                conversion opts
 *  @param[in]     in                  input image
 *  @param[in]     out                 output image
 *  @param         object              the optional object
 *  @return        conversion
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyColourConversion_s* oyColourConversion_Create (
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s        object)
{
  oyColourConversion_s * s = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CMMS);

  s = oyColourConversion_Create_(opts, in, out, object);

  oyExportEnd_();
  DBG_PROG_ENDE
  return s;
}

/**
 *  @internal
 *  Function: oyConcatenateImageProfiles_
 *  @brief   oyCMMColourConversion_ToMem_t implementation
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/06/26
 */
oyProfiles_s * oyConcatenateImageProfiles_ (
                                        oyProfiles_s    * list,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s        obj )
{
  int error = 0;
  oyProfiles_s * p_list = 0;

  if(!error)
  {
    int i, n;

    /* collect profiles */
    if(!error)
    {
      int p_list_n = 0;
      oyProfile_s * tmp = 0;

      if(obj)
        p_list = oyProfiles_New( obj );
      else
        p_list = oyProfiles_New( 0 );
      error = !p_list;

      if(!error)
      {
        tmp = oyProfile_Copy( in->profile_, 0);
        p_list = oyProfiles_MoveIn( p_list, &tmp, 0 );
        error = !p_list;
      }

      p_list_n = oyProfiles_Count( p_list );

      if(!error && list && oyProfiles_Count(list))
      {
        n = oyProfiles_Count(list);
        for(i = 0; i < n; ++i)
        {
          tmp = oyProfiles_Get( list,i );
          p_list = oyProfiles_MoveIn( p_list, &tmp, i + p_list_n);
          error = !p_list;
        }
      }

      if(!error)
      {
        tmp = oyProfile_Copy(out->profile_, 0);
        p_list = oyProfiles_MoveIn( p_list, &tmp, p_list_n);
        error = !p_list;
      }
    }
  }
  
  return p_list;
}


/** @internal 
 *  @memberof oyColourConversion_s
 *             precalculate a transform for a given image by the CMM
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyCMMptr_s *       oyColourConversion_CallCMM_ (
                                        const char      * cmm,
                                        oyColourConversion_s * s,
                                        oyProfiles_s * list,
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyProfileTag_s ** psid,
                                        oyObject_s        obj)
{
  oyCMMptr_s * cmm_ptr = 0;
  oyCMMColourConversion_Create_f funcP = 0;
  int error = !s;
  char *lib_used = 0;

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API1_S, cmm, 0, &lib_used,
                                      0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMColourConversion_Create;
    }
    error = !funcP;
  }

  if(!error)
  {
    oyProfiles_s * p_list = 0;
    int i, n;

    if(obj)
      cmm_ptr = oyCMMptr_New_(obj->allocateFunc_);
    else
      cmm_ptr = oyCMMptr_New_(s->oy_->allocateFunc_);
    error = !cmm_ptr;

    if(!error)
      error = oyCMMptr_Set_( cmm_ptr, lib_used, oyCMM_COLOUR_CONVERSION,0,0,0 );

    /* collect profiles */
    if(!error)
    {
      p_list = oyConcatenateImageProfiles_( list, in, out, obj ? obj : s->oy_ );

      error = !p_list;
    }

    if(!error)
    {
      oyCMMptr_s ** p = oyStructList_GetCMMptrs_( p_list->list_, lib_used );
      int layout_in = in->layout_[oyLAYOUT];
      int layout_out = out->layout_[oyLAYOUT];

      if(!opts)
        opts = oyOptions_ForFilter( "//colour", "lcms",
                                            0/* oyOPTIONATTRIBUTE_ADVANCED |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_OPTIONSOURCE_META */, 0 );

      n = oyProfiles_Count(p_list);

      error = funcP( p, n, layout_in, layout_out, opts, cmm_ptr );

      for(i = 0; i < n; ++i)
        if(!error)
          error = oyCMMptr_Release_(&p[i]);
      p_list->oy_->deallocateFunc_(p);

      oyCMMdsoRelease_( lib_used );

      if(psid)
        *psid = oyProfileTag_Create( p_list->list_,
                     icSigProfileSequenceIdentifierType, 0, OY_MODULE_NICK, 0 );

      oyProfiles_Release( &p_list );
    }
  }

  if(lib_used)
    oyFree_m_(lib_used);

  return cmm_ptr;
}

/**
 *  @internal
 *  Function oyContextCollectData_
 *  @brief   describe a transform uniquely
 *
 *  @param[in,out] s                   the context's object 
 *  @param[in]     opts                options
 *  @param[in]     ins                 input datas
 *  @param[in]     outs                output datas
 *  @return                            the objects ID text
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
const char *   oyContextCollectData_ ( oyStruct_s        * s,
                                       oyOptions_s       * opts,
                                       oyStructList_s    * ins,
                                       oyStructList_s    * outs )
{
  int error = !s;
  int i, n;

  char * hash_text = 0;

  if(!error)
  {
    /* input data */
    hashTextAdd_m( "<data_in>\n" );
    hashTextAdd_m( oyStructList_GetID( ins, 0, 0 ) );
    hashTextAdd_m( "</data_in>\n" );

    /* options -> xforms */
    n = oyOptions_Count( opts );
    for(i = 0; i < n; ++i)
    {
      oyOption_s * o = oyOptions_Get( opts, i );
      hashTextAdd_m( "  <options name=\"" );
      hashTextAdd_m( o->registration );
      hashTextAdd_m( "\" type=\"" );
      hashTextAdd_m( oyValueTypeText( o->value_type ) );
      hashTextAdd_m( "\"" );
      hashTextAdd_m( oyOption_GetText( o, oyNAME_NAME ) );
      oyOption_Release( &o );
      hashTextAdd_m( ">\n" );
    }

    /* output data */
    hashTextAdd_m( "<data_out>\n" );
    hashTextAdd_m( oyStructList_GetID( outs, 0, 0 ) );
    hashTextAdd_m( "</data_out>\n" );

    oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );

    if(hash_text && s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );
    hash_text = 0;
  }

  hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NICK );

  return hash_text;
}

const char *   oyColourContextGetID_ ( oyStruct_s      * s,
                                       oyOptions_s     * opts,
                                       oyImage_s       * in,
                                       oyImage_s       * out)
{
  const char * hash_text = 0;
  oyStructList_s * ins = oyStructList_New(0),
                 * outs = oyStructList_New(0);
  oyStruct_s * s_in = (oyStruct_s*)oyImage_Copy( in, 0 ),
             * s_out = (oyStruct_s*)oyImage_Copy( out, 0 );

  oyStructList_MoveIn( ins, &s_in, -1 );
  oyStructList_MoveIn( outs, &s_out, -1 );

  hash_text = oyContextCollectData_( s, opts, ins, outs );

  oyStructList_Release( &ins );
  oyStructList_Release( &outs );

  return hash_text;
}


/** @internal create and possibly precalculate a transform for a given image
 *  @memberof oyColourConversion_s

 *  @param[in]  opts   conversion opts
 *  @param[in]  in     input image
 *  @param[in]  out    output image
 *  @param[in]  object the optional object
 *  @return            conversion
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyColourConversion_s* oyColourConversion_Create_ (
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s        object)
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_COLOUR_CONVERSION_S;
# define STRUCT_TYPE oyColourConversion_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyColourConversion_Copy;
  s->release = (oyStruct_Release_f) oyColourConversion_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(!error)
  {
    oyCMMptr_s *cmm_ptr = 0;
    const char *cmm = oyModuleGetActual( "//colour" );
    const oyChar * tmp = 0;
 
    oyHash_s * entry = 0;
    oyChar * hash_text = 0;

    /*  Cache Search
     *  1.     hash from input
     *  2.     query for hash in cache
     *  3.     check
     *  3a.       eighter take cache entry
     *  3b.       or ask CMM
     *  3b.1.                update cache entry
     */

    /* 1. create hash text */
    hashTextAdd_m( cmm );
    tmp = oyColourContextGetID_( (oyStruct_s*)s, opts, in, out );
    hashTextAdd_m( tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );
    /*WARNc_S(("%s\n", hash_text);)*/
    if(s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );

    if(!error)
      s->cmms_ = oyStructList_New( s->oy_ );

    if(!error)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( entry,
                                                  oyOBJECT_CMM_POINTER_S);

      if(!cmm_ptr)
      {
        /* 3b. ask CMM */
        cmm_ptr = oyColourConversion_CallCMM_( cmm, s, 0, opts, in, out,
                                               0, entry ? entry->oy_ : 0);
        error = !cmm_ptr;

        /* 3b.1. update cache entry */
        error = oyHash_SetPointer_( entry,
                                   (oyStruct_s*) oyCMMptr_Copy_(cmm_ptr, 0) );
      }

      if(!error && cmm_ptr && cmm_ptr->ptr)
      {
        oyHash_s * c = oyHash_Copy_(entry, 0);
        error = oyStructList_MoveIn( s->cmms_, (oyStruct_s**) &c, -1 );
      }

      oyCMMptr_Release_( &cmm_ptr );

      if(!error)
      {
        s->image_in_ = oyImage_Copy( in, 0 );
        s->image_out_ = oyImage_Copy( out, 0 );
      }

#if 0
    {
      const char * fn = 0;
      size_t size = 0;
      oyPointer mem = 0;
      /*oyStruct_s * h = 0;
      oyCMMptr_s * cmm_ptr_orig = 0;*/

      /*if(in)
        fn = in->profile_->file_name_;*/
      if(!fn) fn = "oy_dbg_in.icc";
      oyProfile_ToFile_(in->profile_, fn);

      mem = oyColourConversion_ToMem_( s, &size, oyAllocateFunc_ );
      error = oyWriteMemToFile_( "oy_dbg_dl.icc", mem, size );
      if(mem)
        oyDeAllocateFunc_(mem); mem = 0;

      fn = 0;
      /*if(out && out->profile_)
        fn = out->profile_->file_name_;*/
      if(!fn) fn = "oy_dbg_out.icc";
      oyProfile_ToFile_(in->profile_, fn);

#if 0
        cmm_ptr = oyColourConversion_CallCMM_( cmm, s, list, opts, in, out,
                                            &psid, entry ? entry->oy_ : 0);
        error = !cmm_ptr;
        h = oyStructList_Get_( s->cmms_, 0 );

      oyCMMptr_Release_( &cmm_ptr );
#endif
      /*cmm_ptr_orig = (oyCMMptr_s*) oyHash_GetPointer_(entry, oyOBJECT_CMM_POINTER_S);

      cmm_ptr_orig->ptr = cmm_ptr->ptr;*/
    }
#endif
    }

    oyHash_Release_( &entry );
  }

  return s;
}

/** @internal
 *  Function: oyColourConversion_Copy
 *  @memberof oyColourConversion_s
 *  @brief   copy the struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
oyColourConversion_s* oyColourConversion_Copy (
                                       oyColourConversion_s * obj,
                                       oyObject_s          object )
{
  oyColourConversion_s * s = 0;
  int error = !obj;

  if(!error && obj->type_ != oyOBJECT_COLOUR_CONVERSION_S)
    error = 1;

  if(!error && obj->oy_)
  {
    oyObject_Ref( obj->oy_ );
    s = obj;
  }

  return s;
}

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


/** @internal
 *  @memberof oyColourConversion_s
 *  @brief   run
 *
 *  @todo TODO: What to do when the CMM changes?
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int        oyColourConversion_Run    ( oyColourConversion_s * s )
{
  int error = !s;
  oyHash_s   * hash = 0;
  oyCMMptr_s * cmm_ptr = 0;
  int pos = 0;
  oyCMMColourConversion_Run_f funcP = 0;

  if(!error)
  {
    hash = (oyHash_s*) oyStructList_GetRefType( s->cmms_, pos,
                                                 oyOBJECT_HASH_S);
    error = !hash;

    if(!error)
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( hash,
                                                  oyOBJECT_CMM_POINTER_S);

    error = !cmm_ptr;

    oyHash_Release_( &hash );
  }

  if(!error)
  {
    oyCMMapi_s *api = oyCMMsGetApi__( oyOBJECT_CMM_API1_S, cmm_ptr->lib_name,
                                      0,0, -1 );
    if(api)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMColourConversion_Run;
    }
    error = !funcP;
  }

  if(!error)
  {
    if(!error)
    {
      int count = 0;
      oyArray2d_s * in, * out; 

      if(s->image_in_ && s->image_in_->pixel_data)
      {
        in = (oyArray2d_s*) s->image_in_->pixel_data;

        /*intptr_t off_x = s->image_in_->layout_[oyPOFF_X],
                 sample_size = s->image_in_->layout_[oyDATA_SIZE];

        in += (intptr_t) oyRegion_Index( s->image_in_->region, 0,0 ) *
                         off_x * sample_size;*/

        count = s->image_in_->width * s->image_in_->height;

      } else
        error = 1;

      if(!error)
      {
        if(s->image_out_)
        {
          if(s->image_out_->pixel_data)
            /* deprecated */
            out = (oyArray2d_s*) s->image_out_->pixel_data;
          else
            error = 1;
        } else
          /* deprecated */
          out = (oyArray2d_s*) s->image_in_->pixel_data;
      }

      if(!error)
      {
        oyCMMProgress_f progress = oyCMMProgress_;
        int channel_n = s->image_in_->layout_[oyCHANS], i;

        for(i = 0; i < in->height; ++i)
          if(!error)
            error = funcP( cmm_ptr, in->array2d[i], out->array2d[i],
                           in->width / channel_n, progress );

        oyCMMdsoRelease_( cmm_ptr->lib_name );
      }
    }
  }

  if(error)
  {
    if(!s->flags & 0x01)
    {
      WARNc2_S("Can not run %s %d", oyStructTypeToText( s->type_ ),
                                    oyObject_GetId( s->oy_ ));
      s->flags |= 0x01;
    }
  }

  return error;
}

/** @internal
 *  Function: oyColourConversion_Release
 *  @memberof oyColourConversion_s
 *  @brief   release a oyColourConversion_Release
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
int        oyColourConversion_Release( oyColourConversion_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyColourConversion_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_COLOUR_CONVERSION_S)
  {
    WARNc_S(("Attempt to release a non oyColourConversion_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyImage_Release( &s->image_in_ );
  oyImage_Release( &s->image_out_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->profiles_)
      deallocateFunc( s->profiles_ ); s->profiles_ = 0;

    if(s->cmms_)
      oyStructList_Release( &s->cmms_ );

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/**
 *  @internal
 *  Function: oyColourConversion_ToMem_ 
 *  @memberof oyColourConversion_s
 *  @brief   save the profile nodeing data as a ICC device node to memory
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2008/03/11
 */
oyPointer    oyColourConversion_ToMem_( oyColourConversion_s * s,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !s;
  oyHash_s   * hash = 0;
  oyCMMptr_s * cmm_ptr = 0;
  int pos = 0;
  oyCMMColourConversion_ToMem_f funcP = 0;
  oyPointer block = 0;
  size_t size_ = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(!error)
  {
    hash = (oyHash_s*) oyStructList_GetRefType( s->cmms_, pos,
                                                 oyOBJECT_HASH_S );
    error = !hash;

    if(!error)
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( hash,
                                                  oyOBJECT_CMM_POINTER_S);

    error = !cmm_ptr;

    oyHash_Release_( &hash );
  }

  if(!error)
  {
    oyCMMapi_s *api = oyCMMsGetApi__( oyOBJECT_CMM_API1_S, cmm_ptr->lib_name,
                                      0,0, -1 );
    if(api)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMColourConversion_ToMem;
    }
    error = !funcP;
  }

  if(!error)
  {
    oyProfile_s * prof = 0;
    /*int n = 0;*/
    oyProfileTag_s * psid = 0,
                   * info = 0,
                   * cprt = 0;

    if(!error)
    {
      if(!error)
        block = funcP( cmm_ptr, &size_, oyAllocateFunc_ );
      error = !block;

      oyCMMdsoRelease_( cmm_ptr->lib_name );
    }

    if(!error && size)
    {
      *size = size_;

      prof = oyProfile_FromMem( size_, block, 0, 0 );
      psid = oyProfile_GetTagById( prof, icSigProfileSequenceIdentifierTag );

      /* icSigProfileSequenceIdentifierType */
      if(!psid)
      {
        oyProfiles_s * p_list = 0;

        p_list = oyConcatenateImageProfiles_( s->profiles_,
                            s->image_in_, s->image_out_, s->oy_ );

        psid = oyProfileTag_Create( p_list->list_,
                     icSigProfileSequenceIdentifierType, 0, OY_MODULE_NICK, 0 );

        oyProfiles_Release( &p_list );

        if(psid)
          error = oyProfile_TagMoveIn_( prof, &psid, -1 );
      }

      /* Info tag */
      if(!error)
      {
        oyStructList_s * list = 0;
        char h[5] = {"Info"};
        uint32_t * hi = (uint32_t*)&h;
        const char * cc_name = oyObject_GetName( s->oy_, oyNAME_NICK );
        oyName_s * name = oyName_new(0);
        char * tmp = oyCMMnameFromLibName_( cmm_ptr->lib_name );

        name = oyName_set_ ( name, cc_name, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        name = oyName_set_ ( name, tmp, oyNAME_NICK,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        oyFree_m_( tmp );
        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list,  (oyStruct_s**) &name, 0 );

        if(!error)
        {
          info = oyProfileTag_Create( list, icSigTextType, 0,OY_MODULE_NICK, 0);
          error = !info;
        }

        if(!error)
          info->use = (icTagSignature)oyValueUInt32(*hi);

        oyStructList_Release( &list );

        if(info)
          error = oyProfile_TagMoveIn_( prof, &info, -1 );
      }

      if(!error)
        cprt = oyProfile_GetTagById( prof, icSigCopyrightTag );

      /* icSigCopyrightTag */
      if(!error && !cprt)
      {
        oyStructList_s * list = 0;
        const char * c_text = "no copyright; use freely";
        oyName_s * name = oyName_new(0);

        name = oyName_set_ ( name, c_text, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list, (oyStruct_s**) &name, 0 );

        if(!error)
        {
          cprt = oyProfileTag_Create( list, icSigTextType, 0,OY_MODULE_NICK, 0);
          error = !cprt;
        }

        if(!error)
          cprt->use = icSigCopyrightTag;

        oyStructList_Release( &list );

        if(cprt)
          error = oyProfile_TagMoveIn_( prof, &cprt, -1 );
      }

      if(block)
        oyDeAllocateFunc_( block ); block = 0; size_ = 0;

      block = oyProfile_TagsToMem_( prof, &size_, allocateFunc );

      *size = size_;
      oyProfile_Release( &prof );
    }
  }

  return block;
}

/**
 *  @internal
 *  Function: oyColourConversion_ToProfile
 *  @memberof oyColourConversion_s
 *  @brief   convert a oyColourConversion_Release to a profile
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 */
oyProfile_s* oyColourConversion_ToProfile ( oyColourConversion_s * cc )
{
  oyProfile_s * s = 0;
  int error = !cc;

  if(!error)
  {
    oyPointer block = 0;
    size_t size = 0;

    block = oyColourConversion_ToMem_( cc, &size, 0 );

    s = oyProfile_FromMem( size, block, 0, 0 );

    if(block && size)
      oyDeAllocateFunc_(block);
  }

  return s;
}




/**
 *  @internal
 *  Function: oyPixelAccess_New_
 *  @memberof oyPixelAccess_s
 *  @brief   allocate and initialise a new oyPixelAccess_s object
 *
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
oyPixelAccess_s *  oyPixelAccess_New_( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PIXEL_ACCESS_S;
# define STRUCT_TYPE oyPixelAccess_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = type;
  s->copy = (oyStruct_Copy_f) oyPixelAccess_Copy;
  s->release = (oyStruct_Release_f) oyPixelAccess_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** Function: oyPixelAccess_Create
 *  @memberof oyPixelAccess_s
 *  @brief   allocate iand initialise a basic oyPixelAccess_s object
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/07/07
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 */
oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterSocket_s  * sock,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object )
{
  oyPixelAccess_s * s = oyPixelAccess_New_( object );
  int error = !s || !sock;
  int w = 0;

  if(!error)
  {
    oyImage_s * image = (oyImage_s*)sock->data;

    s->start_xy[0] = s->start_xy_old[0] = start_x;
    s->start_xy[1] = s->start_xy_old[1] = start_y;

    /* make shure the filter->image_ is set, e.g. 
       error = oyFilter_ImageSet ( filter, image );
     
    s->data_in = filter->image_->data; */
    w = image->width;

    if(type == oyPIXEL_ACCESS_POINT)
    {
      s->array_xy = s->oy_->allocateFunc_(sizeof(int32_t) * 2);
      s->array_xy[0] = s->array_xy[1] = 0;
      s->array_n = 1;
      s->pixels_n = 1;
    } else
    if(type == oyPIXEL_ACCESS_LINE)
    {
      s->array_xy = s->oy_->allocateFunc_(sizeof(int32_t) * 2);
      /* set relative advancements from one pixel to the next */
      s->array_xy[0] = 1;
      s->array_xy[1] = 0;
      s->array_n = 1;
      s->pixels_n = w;       /* the total we want */
    } else
    /* if(type == oyPIXEL_ACCESS_IMAGE) */
    {
      /** @todo how can we know about the various backend capabilities
       *  - back report the processed number of pixels in the passed pointer
       *  - restrict for a line interface only, would fit to oyArray2D_s
       *  - + handle inside an to be created function oyConversion_Run()
       */
    }
  }

  if(error)
    oyPixelAccess_Release ( &s );

  return s;
}

/**
 *  @internal
 *  Function: oyPixelAccess_Copy_
 *  @memberof oyPixelAccess_s
 *  @brief   real copy a oyPixelAccess_s object
 *
 *  @param[in]     obj                 oyPixelAccess_s object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
oyPixelAccess_s * oyPixelAccess_Copy_( oyPixelAccess_s   * obj,
                                       oyObject_s          object )
{
  oyPixelAccess_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  int len = 0;

  if(!obj || !object)
    return s;

  s = oyPixelAccess_New_( object );
  error = !s;
  if(!error && s->oy_)
    allocateFunc_ = s->oy_->allocateFunc_;
  else
    allocateFunc_ = oyAllocateFunc_;

  if(!error)
  {
    s->start_xy[0] = s->start_xy_old[0] = obj->start_xy[0];
    s->start_xy[1] = s->start_xy_old[1] = obj->start_xy[1];
    s->array_n = obj->array_n;
    if(obj->array_xy && obj->array_n)
    {
      len = sizeof(int32_t) * 2 * obj->array_n;
      s->array_xy = allocateFunc_(len);
      error = !s->array_xy;
      if(!error)
        error = !memcpy(s->array_xy, obj->array_xy, len);
    }
    /* reset to properly initialise the new iterator */
    s->index = 0;
    s->pixels_n = obj->pixels_n;
    s->workspace_id = obj->workspace_id;
  }

  if(error)
    oyPixelAccess_Release( &s );

  return s;
}

/** Function: oyPixelAccess_Copy
 *  @memberof oyPixelAccess_s
 *  @brief   copy or reference a oyPixelAccess_s object
 *
 *  @param[in]     obj                 oyPixelAccess_s object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
oyPixelAccess_s *  oyPixelAccess_Copy( oyPixelAccess_s   * obj,
                                       oyObject_s          object )
{
  oyPixelAccess_s * s = 0;

  if(!obj)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyPixelAccess_Copy_( obj, object );

  return s;
}

/** Function: oyPixelAccess_Release
 *  @memberof oyPixelAccess_s
 *  @brief   release and zero a oyPixelAccess_s object
 *
 *  @param[in,out] obj                 oyPixelAccess_s object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2008/07/07
 */
int          oyPixelAccess_Release   ( oyPixelAccess_s  ** obj )
{
  /* ---- start of common object destructor ----- */
  oyPixelAccess_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( s->type != oyOBJECT_PIXEL_ACCESS_S)
  {
    WARNc_S(("Attempt to release a non oyPixelAccess_s object."))
    return 1;
  }

  if( !s->oy_ )
  {
    DBG_PROG_S(("oyPixelAccess_s object is not from Oyranos. Skip"))
    return 0;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_ && s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->array_xy)
      deallocateFunc( s->array_xy ); s->array_xy = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function: oyPixelAccess_CalculateNextStartPixel
 *  @memberof oyPixelAccess_s
 *  @brief   predict the next start position
 *
 *  @param[in,out] obj                 oyPixelAccess_s object
 *  @param[in,out] requestor_plug      plug
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/21 (Oyranos: 0.1.8)
 *  @date    2008/10/21
 */
int                oyPixelAccess_CalculateNextStartPixel (
                                       oyPixelAccess_s   * obj,
                                       oyFilterPlug_s    * requestor_plug )
{
  oyPixelAccess_s * pixel_access = obj;
  int x = pixel_access->start_xy[0], sx = x;
  int y = pixel_access->start_xy[1], sy = y;
  int max = 0, i, n;

  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyImage_s * image = (oyImage_s*)socket->data;

  /* calculate the pixel position we want */
  if(pixel_access->array_xy)
  { 
    /* we have a iteration description - use it */
    n = pixel_access->pixels_n;
    if(pixel_access->array_n < pixel_access->pixels_n)
      n = pixel_access->array_n;
  
    /* x direction progress of this iteration request */
    for( i = 0; i < n; ++i )
      max += pixel_access->array_xy[i*2+0];

    sx += max * pixel_access->pixels_n / pixel_access->array_n;

    /* y direction progress of this iteration request */
    max = 0;
    for( i = 0; i < n; ++i )
      max += pixel_access->array_xy[i*2+1];

    sy += max * pixel_access->pixels_n / pixel_access->array_n;
    pixel_access->start_xy[0] = sx;
    pixel_access->start_xy[1] = sy;
  } else
  {
    /* fall back to a one by one pixel access */
    x = pixel_access->start_xy[0];
    y = pixel_access->start_xy[1];

    if(pixel_access->start_xy[0] >= image->width)
    {
      x = 0; pixel_access->start_xy[0] = 1;
      y = ++pixel_access->start_xy[1];
    } else
      ++pixel_access->start_xy[0];

    if(pixel_access->start_xy[1] >= image->height)
    {
      return -1;
    }
  }

  return 0;
}



/**
 *  @internal
 *  Function: oyConversion_New_
 *  @memberof oyConversion_s
 *  @brief   allocate and initialise a new oyConversion_s object
 *
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/24
 */
oyConversion_s *   oyConversion_New_ ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONVERSION_S;
# define STRUCT_TYPE oyConversion_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S("MEM Error.")
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConversion_Copy;
  s->release = (oyStruct_Release_f) oyConversion_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** Function: oyConversion_CreateBasic
 *  @memberof oyConversion_s
 *  @brief   allocate initialise a basic oyConversion_s object
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/26
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 */
oyConversion_s   * oyConversion_CreateBasic (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyConversion_s * s = 0;
  int error = !input || !output;
  oyFilter_s * filter = 0;

  if(!error)
  {
    s = oyConversion_CreateInput ( input, 0 );

    filter = oyFilter_New( "//colour/icc", 0,0, 0 );

    error = oyConversion_FilterAdd( s, filter );
    if(error)
      WARNc1_S( "could not add  filter: %s\n", "//colour" );

    error = oyConversion_OutputAdd( s, output );
  }

  if(error)
    oyConversion_Release ( &s );

  return s;
}

/** Function: oyConversion_CreateInput
 *  @memberof oyConversion_s
 *  @brief   initialise from a input image for later adding more filters
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/07/07
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 */
oyConversion_s   * oyConversion_CreateInput (
                                       oyImage_s         * input,
                                       oyObject_s          object )
{
  oyConversion_s * s = oyConversion_New_( object );
  int error = !s;
  oyFilter_s * filter = 0;
  oyFilterSocket_s * sock = 0;

  if(!error)
  {
    filter = oyFilter_New( "//image/root", 0,0, 0 );
    s->input = oyFilterNode_Create( filter, s->oy_ );
    oyFilter_Release( &filter );

    error = !s->input;

    if(!error)
    {
      sock = oyFilterNode_GetSocket ( s->input, 0 );
      sock->data = (oyStruct_s*)oyImage_Copy( input, 0 );
    }
  }

  if(error)
    oyConversion_Release ( &s );

  return s;
}

/**
 *  @internal
 *  Function: oyConversion_Copy_
 *  @memberof oyConversion_s
 *  @brief   real copy a oyConversion_s object
 *
 *  @param[in]     conversion          conversion object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyConversion_s * oyConversion_Copy_  ( oyConversion_s    * conversion,
                                       oyObject_s          object )
{
  oyConversion_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!conversion || !object)
    return s;

  s = oyConversion_New_( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(!error)
  {
    s->input = oyFilterNode_Copy( conversion->input, object );
  }

  return s;
}

/** Function: oyConversion_Copy
 *  @memberof oyConversion_s
 *  @brief   copy or reference a oyConversion_s object
 *
 *  @param[in]     conversion          conversion object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyConversion_s * oyConversion_Copy   ( oyConversion_s    * conversion,
                                       oyObject_s          object )
{
  oyConversion_s * s = 0;

  if(!conversion)
    return s;

  if(conversion && !object)
  {
    s = conversion;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConversion_Copy_( conversion, object );

  return s;
}
/** Function: oyConversion_Release
 *  @memberof oyConversion_s
 *  @brief   release and zero a conversion object
 *
 *  @todo    complete the implementation
 *
 *  @param[in,out] obj                 conversion object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
int          oyConversion_Release    ( oyConversion_s   ** obj )
{
  /* ---- start of common object destructor ----- */
  oyConversion_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_CONVERSION_S)
  {
    WARNc_S(("Attempt to release a non oyConversion_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyFilterNode_Release( &s->input );
  oyFilterNode_Release( &s->out_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyConversion_FilterAdd
 *  @memberof oyConversion_s
 *  @brief   add a filter to a oyConversion_s filter list
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     filter              filter
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/09 (Oyranos: 0.1.8)
 *  @date    2008/07/10
 */
int                oyConversion_FilterAdd (
                                       oyConversion_s    * conversion,
                                       oyFilter_s        * filter )
{
  oyConversion_s * s = conversion;
  int error = !s;
  oyFilterNode_s * node = 0,
                 * temp = 0,
                 * last = 0;
  oyConnector_s  * node_plug_connector = 0;
  oyFilterPlug_s * node_plug = 0;
  oyFilterSocket_s * socket_last = 0,
                 * node_socket = 0;

  if(!error)
  {
    node = oyFilterNode_Create( filter, s->oy_ );

    if(!error && 
       (!node || !node->filter))
    {
      WARNc2_S( "%s: %d", _("No filter"), oyObject_GetId( s->oy_ ) );
      error = 1;
    }

    if(!error &&
       (!s->input &&
        !oyFilterRegistrationMatch( filter->registration_, "//image",
                                    oyOBJECT_CMM_API4_S )))
    {
      WARNc2_S( "%s: %s",
      _("Please add first a image node to the graph before adding other filters"),
                s->oy_->id_ );
      error = 1;
    }
    if(!error &&
       (!node->filter || !node->filter->api4_))
    {
      WARNc2_S( "%s: %s",
      _("attempt to add a incomplete filter"),
                oyFilter_GetName( filter, oyNAME_NAME) );
      error = 1;
    }
    if(!error &&
       (node->api7_->sockets_n > 1 ||
        node->api7_->plugs_n > 1))
    {
      WARNc2_S( "%s: %s",
      _("attempt to add a non linear filter to a linear graph"),
                oyFilter_GetName( node->filter, oyNAME_NAME) );
      error = 1;
    }

    if(!error)
    {
      last = oyFilterNode_GetLastFromLinear_( s->input );

      if(!error)
        error = !last;

      if(last)
      {
        temp = node;
        node_socket = oyFilterNode_GetSocket( node, 0 );
        node_plug = oyFilterNode_GetPlug( node, 0 );
        node_plug_connector = node_plug->pattern;

        if(oyFilterNode_ConnectorMatch( last, 0, node_plug_connector ))
        {
          socket_last = oyFilterNode_GetSocket( last, 0 );
        } else
          error = 1;

        if(!error && node_socket && !node_socket->data)
          node_socket->data = socket_last->data->copy( socket_last->data, 0 );

        if(!error)
          oyFilterPlug_ConnectIntoSocket( &node_plug, &socket_last );

        temp = 0;
      }
      else
        WARNc2_S( "%s: %d", _("?? Nothing to add ??"), oyObject_GetId(s->oy_));
    }
  }

  if(error)
    oyFilterNode_Release( &node );

  return error;
}

/** Function oyConversion_OutputAdd
 *  @memberof oyConversion_s
 *  @brief   close a oyConversion_s with a target image
 *
 *  Internally the image will be attached to the socket at which the last node
 *  points through a plug.
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     output              output image
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/06 (Oyranos: 0.1.8)
 *  @date    2008/07/06
 */
int                oyConversion_OutputAdd (
                                       oyConversion_s    * conversion,
                                       oyImage_s         * output )
{
  oyConversion_s * s = conversion;
  int error = !s;
  oyFilter_s * filter = 0;
  oyFilterNode_s * last = 0;
  oyFilterPlug_s * plug_last = 0;

  if(!error)
  {
    filter = oyFilter_New( "//image/output", 0,0, 0);

    if(!error)
      error = oyConversion_FilterAdd( conversion, filter );

    last = oyFilterNode_GetLastFromLinear_( s->input );
    plug_last = oyFilterNode_GetPlug( last, 0 );

    /* oyConversion_FilterAdd references the input image in the new filter */
    oyImage_Release( (oyImage_s**) &plug_last->remote_socket_->data );
    plug_last->remote_socket_->data = (oyStruct_s*)oyImage_Copy( output, 0 );

    if(!error)
      s->out_ = oyFilterNode_Copy( last, 0 );

    oyFilterNode_Release( &last );

    if(!error)
    {
      oyFilterNode_s * node = s->input;

      while((node = oyFilterNode_GetNextFromLinear_( node )) != 0)
        if(!error &&
           node->filter->api4_->oyCMMFilterNode_ContextToMem &&
           oyStrlen_(node->api7_->context_type))
          oyFilterNode_ContextSet_( node );
    }
  }

  if(error && s)
    oyFilterNode_Release( &s->out_ );

  return error;
}

/** Function: oyConversion_Run
 *  @memberof oyConversion_s
 *  @brief   iterate over a conversion graph
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in,out] pixel_access        pixel iterator configuration
 *  @param[in]     region              the region relative to the output image viewport to render
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/06 (Oyranos: 0.1.8)
 *  @date    2008/10/02
 */
int                oyConversion_Run  ( oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access,
                                       oyRegion_s        * region )
{
  oyFilterPlug_s * plug = 0;
  oyFilter_s * filter = 0;
  oyImage_s * image = 0;
  oyArray2d_s * array = 0;
  int error = 0, result;

  /* conversion->out_ has to be linear, so we access only the first socket */
  plug = conversion->out_->plugs[0];
  /* should be the same as conversion->out_->filter */
  filter = conversion->out_->filter;
  image = (oyImage_s*) plug->remote_socket_->data;

  result = oyImage_FillArray( image, region, 0, &array, 0 );
  error = ( result != 0 );

  if(!error)
  {
    error = conversion->out_->api7_->oyCMMFilterPlug_Run( plug, pixel_access,
                                                          &array );
  }

  if(!error)
    error = oyArray2d_Release( &array );

  /* @todo write data back to image in case we obtained copies only */

  return error;
}

/** Function: oyConversion_GetOnePixel
 *  @memberof oyConversion_s
 *  @brief   compute one pixel at the given position
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     x                   position x
 *  @param[in]     y                   position y
 *  @param[out]    feedback            -1 end; 0 on success; error > 1
 *  @return                            the pixel pointer
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/14 (Oyranos: 0.1.8)
 *  @date    2008/07/14
 */
oyPointer        * oyConversion_GetOnePixel (
                                       oyConversion_s    * conversion,
                                       int32_t             x,
                                       int32_t             y,
                                       int32_t           * feedback )
{
  oyPixelAccess_s * pixel_access = 0;
  oyFilterPlug_s * plug = 0;
  oyFilterSocket_s * sock = 0;
  oyPointer pixel = 0;
  int error = 0;

  /* conversion->out_ has to be linear, so we access only the first socket */
  plug = (oyFilterPlug_s*) ((oyFilterSocket_s *)conversion->out_->sockets[0])->requesting_plugs_->list_->ptr_[0];
  sock = plug->remote_socket_;

  pixel_access = oyPixelAccess_Create ( x, y, sock, oyPIXEL_ACCESS_POINT, 0 );
  /* @todo */
  error = sock->node->api7_->oyCMMFilterPlug_Run( plug, pixel_access, 0 );

  return pixel;
}

oyProfile_s      * oyConversion_ToProfile (
                                       oyConversion_s    * conversion );
/** Function: oyConversion_GetAdjazenzlist
 *  @memberof oyConversion_s
 *  @brief   adjazenzliste of a conversion graph
 *
 *  
 *
 *  @param[in]     conversion          conversion object
 *  @param[in]     allocateFunc        allocation function
 *  @return                            the adjazenzlist
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/09 (Oyranos: 0.1.8)
 *  @date    2008/07/09
 */
int             ** oyConversion_GetAdjazenzlist (
                                       oyConversion_s    * conversion,
                                       oyAlloc_f           allocateFunc )
{
  int ** adjazenzliste = 0;
  /*int nodes = 0,
      edges = 0;*/

  /* count nodes and edges */
  /* allocate adjazenzliste */
  /* fill adjazenzliste */
  /* TODO */

  return adjazenzliste;
}

void               oyConversion_ToTextShowNode_ (
                                       oyFilterNode_s    * node,
                                       char             ** stream,
                                       int                 reserved,
                                       int                 sub_format,
                                       int               * counter,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  uint32_t last_adds = 0;
  int n = oyFilterNode_ShowConnectorCount( node, 1, &last_adds );
  char text[256];
  char name = 'A' + *counter;

  ++*counter;

  if(sub_format == 0)
  {
    oySprintf_(text, "  %c [ label=\"{<plug> %d| Filter Node %c\\n Category: \\\"%s\\\"\\n CMM: \\\"%s\\\"\\n Type: \\\"%s\\\"|<socket>}\"];\n", name, n, name,
    node->filter->category_, node->filter->api4_->id_,
    node->filter->registration_ );
    STRING_ADD( *stream, text );
  }

  {
    int i = 0;

    while( node->sockets[i] )
    {
      int j = 0,
          np = oyFilterPlugs_Count( node->sockets[i]->requesting_plugs_ );

      for(j = 0; j < np; ++j )
      {
        oyFilterPlug_s * remote_plug = 0;

        remote_plug = oyFilterPlugs_Get( node->sockets[i]->requesting_plugs_, j );

        if(remote_plug)
        {
          if(sub_format == 1)
          {
            oySprintf_(text, "    %c:socket -> %c:plug [arrowhead=crow, arrowtail=box];\n", name, name+1+j );
            STRING_ADD( *stream, text );
          }
 
          oyConversion_ToTextShowNode_( remote_plug->node, stream,
                                        reserved, sub_format, counter,
                                        allocateFunc, deallocateFunc );
        }
      }
      ++i;
    }
  }
}

/** Function: oyConversion_ToText
 *  @memberof oyConversion_s
 *  @brief   text description of a conversion graph
 *
 *  @todo Should this function generate XFORMS compatible output? How?
 *
 *  @param[in]     conversion          conversion object
 *  @param[in]     head_line           text for inclusion
 *  @param[in]     reserved            future format selector (dot, xml ...)
 *  @param[in]     allocateFunc        allocation function
 *  @return                            the adjazenzlist
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/04 (Oyranos: 0.1.8)
 *  @date    2008/10/04
 */
char             * oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc )
{
  char * text = 0, temp[80];
  oyConversion_s * s = conversion;
  const char * save_locale = 0;
  int counter = 0;

  if(!s || s->type_ != oyOBJECT_CONVERSION_S)
  {
    WARNc_S(("Attempt to read a non oyConversion_s object."))
    return 0;
  }

#if USE_GETTEXT
  save_locale = setlocale(LC_NUMERIC, 0 );
#endif


  STRING_ADD( text, "digraph Graph {\n" );
  STRING_ADD( text, "bgcolor=\"transparent\"\n" );
  STRING_ADD( text, "  rankdir=LR\n" );
  STRING_ADD( text, "  graph [fontname=Helvetica, fontsize=12];\n" );
  STRING_ADD( text, "  node [shape=record, fontname=Helvetica, fontsize=10, style=\"filled,rounded\"];\n" );
  STRING_ADD( text, "  edge [fontname=Helvetica, fontsize=10];\n" );
  STRING_ADD( text, "\n" );
  STRING_ADD( text, "  conversion [shape=plaintext, label=<\n" );
  STRING_ADD( text, "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n" );
  STRING_ADD( text, "  <tr><td>oyConversions_s</td></tr>\n" );
  STRING_ADD( text, "  <tr><td>\n" );
  STRING_ADD( text, "     <table border=\"0\" cellborder=\"0\" align=\"left\">\n" );
  STRING_ADD( text, "       <tr><td align=\"left\">...</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\" port=\"in\">+input</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\" port=\"out\">+out_</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\">...</td></tr>\n" );
  STRING_ADD( text, "     </table>\n" );
  STRING_ADD( text, "     </td></tr>\n" );
  STRING_ADD( text, "  <tr><td> </td></tr>\n" );
  STRING_ADD( text, "</table>>,\n" );
  STRING_ADD( text, "                    style=\"\", color=black];\n" );
  STRING_ADD( text, "\n" );

  /* add more node descriptions */
  oyConversion_ToTextShowNode_( s->input, &text,
                                reserved, 0, &counter,
                                oyAllocateFunc_, oyDeAllocateFunc_ );

  STRING_ADD( text, "\n" );
  STRING_ADD( text, "  subgraph cluster_0 {\n" );
  STRING_ADD( text, "    label=\"" );
  STRING_ADD( text, head_line );
  STRING_ADD( text, "\"\n" );
  STRING_ADD( text, "    color=gray;\n" );
  STRING_ADD( text, "\n" );

  counter = 0;
  /* add more node placements */
  oyConversion_ToTextShowNode_( s->input, &text,
                                reserved, 1, &counter,
                                oyAllocateFunc_, oyDeAllocateFunc_ );

  STRING_ADD( text, "\n" );
  STRING_ADD( text, "    conversion:in -> A [arrowhead=none, arrowtail=normal];\n" );
  oySprintf_( temp, "    conversion:out -> %c;\n", 'A' + counter - 1 );
  STRING_ADD( text, temp );
  STRING_ADD( text, "  }\n" );
  STRING_ADD( text, "\n" );
  STRING_ADD( text, "  conversion\n" );
  STRING_ADD( text, "}\n" );
  STRING_ADD( text, "\n" );

#if USE_GETTEXT
  setlocale(LC_NUMERIC, "C");
  /* sensible printing */
  setlocale(LC_NUMERIC, save_locale);
#endif

  STRING_ADD( text, "" );

  return text;
}



/** @} objects_conversion */


/** \addtogroup objects_single_colour Named Colour API
 *  Named Colour API's.

 *  @{
 */


/** @brief manage complex oyNamedColour_s inside Oyranos
 *  @memberof oyNamedColour_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 *
 *  @param[in]     chan                pointer to channel data with a number of elements specified in sig or channels_n
 *  @param[in]     blob                CGATS or other reference data
 *  @param[in]     blob_len            length of the data blob
 *  @param[in]     ref                 possibly a ICC profile, allowed abreviations are 'sRGB' 'XYZ' 'Lab'(D50)
 *  @param         object              the optional object
 * 
 */
oyNamedColour_s*
oyNamedColour_Create( const double      * chan,
                      const char        * blob,
                      int                 blob_len,
                      oyProfile_s       * ref,
                      oyObject_s          object )
{
  int n = 0;
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAMED_COLOUR_S;
# define STRUCT_TYPE oyNamedColour_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyNamedColour_Copy;
  s->release = (oyStruct_Release_f) oyNamedColour_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(!error)
    s->profile_  = oyProfile_Copy( ref, 0 );

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(n)
    s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
  oyCopyColour( chan, &s->channels_[0], 1, ref, n );
  oyCopyColour( 0, &s->XYZ_[0], 1, 0, 0 );

  if(!error && blob && blob_len)
  {
    s->blob_ = s->oy_->allocateFunc_( blob_len );
    if(!s->blob_) error = 1;

    if(!error)
      error = !memcpy( s->blob_, blob, blob_len );

    if(!error)
      s->blob_len_ = blob_len;
  }

  return s;
}

/** Function: oyNamedColour_CreateWithName
 *  @memberof oyNamedColour_s
 *  @brief   create a oyNamedColour_s struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 *
 *  @param[in]  nick        very short, only a few letters
 *  @param[in]  name        of the colour
 *  @param[in]  description description
 *  @param[in]  chan        pointer to channel data with a number of elements specified in sig or channels_n
 *  @param[in]  XYZ         reference values
 *  @param[in]  blob        CGATS or other reference data
 *  @param[in]  blob_len    length of the data blob
 *  @param[in]  profile_ref profile
 *  @param[in]  object      memory management
 *  @return                 a oyNamedColour_s
 * 
 */
oyNamedColour_s*  oyNamedColour_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object )
{
  int error = 0;
  oyNamedColour_s * s = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );

  error = !s_obj;

  if(!error)
    error = oyObject_SetNames( s_obj, nick, name, description );

  s = oyNamedColour_Create( chan, blob, blob_len, profile_ref, s_obj );
  error =!s;

  oyObject_Release( &s_obj );

  if(!error && XYZ)
    oyCopyColour( XYZ, s->XYZ_, 1, 0, 0 );

  return s;
}

/** @brief   copy
 *  @memberof oyNamedColour_s
 *
 *  
 *
 *  @param[in]     colour              address of Oyranos colour struct
 *  @param[in]     object              the optional base object
 *  @return                            copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyNamedColour_s*
oyNamedColour_Copy ( oyNamedColour_s   * colour,
                     oyObject_s          object )
{
  oyNamedColour_s * c = NULL,
                  * s = colour;

  if(!colour)
    return c;

  if(!object)
  {
    oyObject_Ref( s->oy_ );
    return s;
  }

  c = oyNamedColour_Create(
                     colour->channels_,
                     colour->blob_,
                     colour->blob_len_,
                     colour->profile_,
                     object );

  return c;
}

/** @brief release correctly
 *  @memberof oyNamedColour_s
 *
 *  set pointer to zero
 *
 *  @param[in]     obj                 address of Oyranos colour struct pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int 
oyNamedColour_Release( oyNamedColour_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyNamedColour_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_NAMED_COLOUR_S)
  {
    WARNc_S(("Attempt to release a non oyNamedColour_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyProfile_Release( &s->profile_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->channels_)
      deallocateFunc( s->channels_ ); s->channels_ = 0;

    if(s->blob_) /* s->blob_len */
      deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/*  @return                 pointer to D50 Lab doubles with L 0.0...1.0 a/b -1.27...1.27 */

/** Function: oyNamedColour_GetSpaceRef
 *  @memberof oyNamedColour_s
 *  @brief   get a colour space reference
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
oyProfile_s *     oyNamedColour_GetSpaceRef ( oyNamedColour_s  * colour )
{
  if(!colour)
    return 0;

  return oyProfile_Copy( colour->profile_, 0 );
}


/** Function: oyNamedColour_SetChannels
 *  @memberof oyNamedColour_s
 *  @brief   set colour channels
 *
 *
 *  @param[in]     colour              Oyranos colour struct pointer
 *  @param[in]     channels            pointer to channel data
 *  @param[in]     flags               reserved
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
void
oyNamedColour_SetChannels( oyNamedColour_s  * colour,
                           const double     * channels,
                                       uint32_t            flags )
{
  int i, n;
  if(!colour)
    return;

  n = oyProfile_GetChannelsCount( colour->profile_ );
  if(channels)
    for(i = 0; i < n; ++i)
      colour->channels_[i] = channels[i];
}

/**
 *  @internal
 *  Function: oyColour_Convert
 *  @memberof oyNamedColour_s
 *  @brief   convert one colour
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int  oyColourConvert_ ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out )
{
  oyImage_s * in  = NULL,
            * out = NULL;
  oyColourConversion_s * conv = NULL;
  int error = 0;

  in    = oyImage_Create( 1,1, 
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  out   = oyImage_Create( 1,1, 
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  conv   = oyColourConversion_Create( 0, in,out, 0 );
  error  = oyColourConversion_Run( conv );

  oyColourConversion_Release( &conv );
  oyImage_Release( &in );
  oyImage_Release( &out );

  return error;
}

/** Function: oyNamedColour_GetColour
 *  @memberof oyNamedColour_s
 *  @brief   convert a named colour to a standard colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int          oyNamedColour_GetColour ( oyNamedColour_s   * colour,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags )
{
  int error = !colour || !profile || !buf || !buf_type;
  oyProfile_s * p_in = colour->profile_;

  /* XYZ has priority */
  if(!error &&
     colour->XYZ_[0] != -1 && colour->XYZ_[1] != -1 && colour->XYZ_[2] != -1)
  {
    p_in = oyProfile_FromStd ( oyEDITING_XYZ, NULL );
    if(!profile)
      return 1;
 
    error = oyColourConvert_( p_in, profile,
                              colour->XYZ_, buf,
                              oyDOUBLE, buf_type);

    oyProfile_Release ( &p_in );

  } else if(!error)
    error = oyColourConvert_( p_in, profile,
                              colour->channels_, buf,
                              oyDOUBLE, buf_type);

  return error;
}

/** Function: oyNamedColour_SetColourStd
 *  @memberof oyNamedColour_s
 *  @brief   set colour channels
 *
 *
 *  @param[in]     colour              Oyranos colour struct pointer
 *  @param[in]     colour_space        Oyranos standard colour space
 *  @param[in]     channels            pointer to channel data
 *  @param[in]     channels_type       data type
 *  @param[in]     flags               reserved for future use
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColour_SetColourStd ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags )
{
  oyNamedColour_s * s = colour;
  int error = !s || !colour_space || !channels;
  oyProfile_s * p_in = 0;

  /* abreviate */
  if(!error && channels_type == oyDOUBLE)
  {
    if     (colour_space == oyEDITING_LAB)
    {
      oyLab2XYZ( (double*)channels, s->XYZ_ );
      return error;

    }
    else if(colour_space == oyEDITING_XYZ)
    {
      oyCopyColour( (double*)channels, s->XYZ_, 1, 0, 0 );
      return error;
    }
  }

  if(!error)
  {
    p_in = oyProfile_FromStd ( colour_space, NULL );
    error = !p_in;
  }

  /* reset and allocate */
  if(!error)
  {
    int n = oyProfile_GetChannelsCount( p_in );

    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(n > oyProfile_GetChannelsCount( s->profile_ ))
    {
      if(s->channels_)
        deallocateFunc(s->channels_); s->channels_ = 0;

      s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
    }

    error = !memset( s->channels_, 0, sizeof(double) * n );

    s->XYZ_[0] = s->XYZ_[1] = s->XYZ_[2] = -1;

    if(deallocateFunc && s->blob_)
      deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0;
  }

  /* convert */
  if(!error)
  {
    oyProfile_s * p_out = s->profile_;
    oyColourConvert_( p_in, p_out,
                      channels, s->channels_,
                      channels_type , oyDOUBLE );
    oyProfile_Release ( &p_out );

    p_out = oyProfile_FromStd( oyEDITING_XYZ, 0 );
    oyColourConvert_( p_in, p_out,
                      channels, s->XYZ_,
                      channels_type , oyDOUBLE );
    oyProfile_Release ( &p_out );
  }

  return error;
}

/** Function: oyNamedColour_GetChannelsConst
 *  @memberof oyNamedColour_s
 *  @brief   get colour channels
 *
 *
 *  @param[in]     colour              Oyranos colour struct pointer
 *  @param[in]     flags               reserved
 *  @return                            pointer channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
const double *    oyNamedColour_GetChannelsConst ( oyNamedColour_s * colour,
                                       uint32_t          * flags )
{
  static double l[3] = {-1.f,-1.f,-1.f};
  if(colour)
    return colour->channels_;
  else
    return l;
}

/** Function: oyNamedColour_GetiXYZConst
 *  @memberof oyNamedColour_s
 *  @brief   get XYZ channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @return                 pointer XYZ channels
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/18
 *  @since   2008/02/18 (Oyranos: 0.1.8)
 */
const double *    oyNamedColour_GetXYZConst      ( oyNamedColour_s * colour)
{
  static double l[3] = {-1.f,-1.f,-1.f};
  if(colour)
    return colour->XYZ_;
  else
    return l;
}

/** Function: oyNamedColour_GetColourStd
 *  @memberof oyNamedColour_s
 *  @brief   convert a named colour to a standard colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColour_GetColourStd     ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t          * flags )
{                        
  int ret = 0;
  oyProfile_s * profile;

  if(!colour)
    return 1;

  /* abreviate */
  if(buf_type == oyDOUBLE &&
     colour->XYZ_[0] != -1 && colour->XYZ_[1] != -1 && colour->XYZ_[2] != -1)
  {
    if(colour_space == oyEDITING_LAB)
    {
      oyXYZ2Lab( colour->XYZ_, (double*)buf );
      return 0;
    }

    if(colour_space == oyEDITING_XYZ)
    {
      oyCopyColour( colour->XYZ_, (double*)buf, 1, 0, 0 );
      return 0;
    }
  }

  profile = oyProfile_FromStd ( colour_space, NULL );
  if(!profile)
    return 1;

  ret = oyNamedColour_GetColour ( colour, profile, buf, buf_type, 0 );
  oyProfile_Release ( &profile );

  return ret;
}

/** Function: oyNamedColour_GetName
 *  @memberof oyNamedColour_s
 *  @brief   get colour channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
const oyChar *    oyNamedColour_GetName( oyNamedColour_s * s,
                                       oyNAME_e            type,
                                       uint32_t            flags )
{
  const oyChar * text = 0;
  if(!s)
    return 0;

  if(!s->oy_)
    return 0;

  text = oyObject_GetName( s->oy_, type );

  if(!text && flags)
  {
    const oyChar * tmp = 0;
    oyChar * txt = 0;
    double l[3];
    int i;
    icSignature sig = oyProfile_GetSignature( s->profile_,
                                              oySIGNATURE_COLOUR_SPACE );

    oyAllocHelper_m_( txt, oyChar, 1024, 0, return 0 );
    oyNamedColour_GetColourStd( s, oyEDITING_LAB, l, oyDOUBLE, 0 );

    switch(type)
    {
      case oyNAME_DESCRIPTION:
           tmp = oyObject_GetName( s->oy_, oyNAME_NAME );
           if(!tmp)
             tmp = oyObject_GetName( s->oy_, oyNAME_NICK );

           if(tmp)
             oySprintf_(txt, "%s: CIE*Lab: ", tmp );
           else
             oySprintf_(txt, "CIE*Lab: ");
           for(i = 0; i < 3; ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", l[i] );

           tmp = oyICCColourSpaceGetName( sig );
           if(tmp)
             oySprintf_( &txt[ oyStrlen_(txt) ], "; %s:", tmp );

           if( s->channels_ )
           for(i = 0; i < oyICCColourSpaceGetChannelCount( sig ); ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", s->channels_[i] );
           break;
      default:
           break;
    }

    oyObject_SetName ( s->oy_, txt, type );

    oyFree_m_( txt );
    
    text = oyObject_GetName( s->oy_, type );
  }

  if(!text && type > oyNAME_NICK)
    text = oyObject_GetName( s->oy_, type - 2 );
  if(!text && type > oyNAME_NAME)
    text = oyObject_GetName( s->oy_, type - 1 );
  if(!text && type < oyNAME_NICK )
    text = oyObject_GetName( s->oy_, type + 2 );
  if(!text && type < oyNAME_DESCRIPTION )
    text = oyObject_GetName( s->oy_, type + 1 );
  if(!text)
    text = _("---");

  return text;
}









/** Function: oyNamedColours_New
 *  @memberof oyNamedColours_s
 *  @brief   create a list object for oyNamedColour_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s* oyNamedColours_New ( oyObject_s       object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAMED_COLOURS_S;
# define STRUCT_TYPE oyNamedColours_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyNamedColours_Copy;
  s->release = (oyStruct_Release_f) oyNamedColours_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(!error)
    s->list_ = oyStructList_Create( s->type_, 0, object );

  return s;
}

/** Function: oyNamedColours_Copy
 *  @memberof oyNamedColours_s
 *  @brief   Copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s* oyNamedColours_Copy( oyNamedColours_s  * colours,
                                       oyObject_s          obj )
{
  oyNamedColours_s * s = 0;
  int error = !colours;

  if(error)
    return s;

  if(colours && !obj)
  {
    s = colours;
    s->oy_ = oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyNamedColours_New( obj );

  s->list_ = oyStructList_Copy( colours->list_, 0 );

  return s;
}

/** Function: oyNamedColours_Release
 *  @memberof oyNamedColours_s
 *  @brief   release and probably free a named colour object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_Release ( oyNamedColours_s** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyNamedColours_s * s = 0;

  if(!obj || !*obj)
    return 1;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_NAMED_COLOURS_S)
  {
    WARNc_S(("Attempt to release a non oyNamedColours_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    s->type_ = 0;

    deallocateFunc( s );
  }

  return error;
}

/** Function: oyNamedColours_Count
 *  @memberof oyNamedColours_s
 *  @brief   count in oyNamedColours_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_Count( oyNamedColours_s * obj )
{
  if(obj && obj->type_ == oyOBJECT_NAMED_COLOURS_S)
    return oyStructList_Count( obj->list_ );
  else
    return -1;
}

/** Function: oyNamedColours_Get
 *  @memberof oyNamedColours_s
 *  @brief   obtain a referenced patch from the list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColour_s*  oyNamedColours_Get ( oyNamedColours_s  * obj,
                                       int                 position)
{
  int error = !obj;
  oyNamedColour_s * s = 0;

  if(!error && obj->type_ != oyOBJECT_NAMED_COLOURS_S)
    error = 1;

  if(!error)
    s = (oyNamedColour_s*) oyStructList_GetRefType( obj->list_, position,
                                                 oyOBJECT_NAMED_COLOUR_S );

 
  return s;
}

/** Function: oyNamedColours_MoveIn
 *  @memberof oyNamedColours_s
 *  @brief   add a patch to the colours list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s * oyNamedColours_MoveIn ( oyNamedColours_s  * list,
                                       oyNamedColour_s  ** obj,
                                       int                 pos )
{
  int error = 0;
  oyNamedColours_s * s = list;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_NAMED_COLOUR_S)
  {
    if(!s)
    {
      s = oyNamedColours_New(0);
      error = !s;
    }

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }

    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos );
  }

  return s;
}

/** Function: oyNamedColours_ReleaseAt
 *  @memberof oyNamedColours_s
 *  @brief   release a patch from the list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_ReleaseAt ( oyNamedColours_s * obj,
                                       int                 position )
{
  int error = !obj;

  if(!error && obj->type_ != oyOBJECT_NAMED_COLOURS_S)
    error = 1;

  if(!error)
    oyStructList_ReleaseAt( obj->list_, position );

  return error; 
}


/** @} *//* objects_single_colour */


/** \addtogroup cmm_handling CMM Handling API

 *  @{
 */
const char *   oyModuleGetActual     ( const char        * type )
{
  oyExportStart_(EXPORT_CMMS);
  oyExportEnd_();

  if(oyFilterRegistrationMatch( type, "//colour", oyOBJECT_CMM_API4_S ))
  return "lcms";
  else
  return OY_PROFILE_NONE;
}

uint32_t     oyCMMtoId               ( const char        * cmm )
{
  if(cmm)
    return oyValueUInt32( *(uint32_t*)cmm );
  else
    return 0;
}
int          oyIdToCMM               ( uint32_t            cmmId,
                                       char              * cmm )
{
  cmmId = oyValueUInt32( cmmId );

  if(cmm)
    return !memcpy( cmm, &cmmId, 4 );
  else
    return 0;
}

/** @} *//* cmm_handling */



/** \addtogroup backend_api

 *  @{
 */

/** Function oyCMMInfo_New
 *  @memberof oyCMMInfo_s
 *  @brief   allocate a new CMMInfo object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
OYAPI oyCMMInfo_s * OYEXPORT
                   oyCMMInfo_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_INFO_S;
# define STRUCT_TYPE oyCMMInfo_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = type;
  s->copy_ = (oyStruct_Copy_f) oyCMMInfo_Copy;
  s->release_ = (oyStruct_Release_f) oyCMMInfo_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  Function oyCMMInfo_Copy_
 *  @memberof oyCMMInfo_s
 *  @brief   real copy a CMMInfo object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
oyCMMInfo_s * oyCMMInfo_Copy_
                                     ( oyCMMInfo_s       * obj,
                                       oyObject_s          object )
{
  oyCMMInfo_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyCMMInfo_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    error = !memcpy(s->cmm, obj->cmm, 8);

    if(obj->backend_version)
      s->backend_version = oyStringCopy_( obj->backend_version, allocateFunc_ );

    s->getText = obj->getText;
      
    s->oy_compatibility = obj->oy_compatibility;
  }

  if(error)
    oyCMMInfo_Release( &s );

  return s;
}

/** Function oyCMMInfo_Copy
 *  @memberof oyCMMInfo_s
 *  @brief   copy or reference a CMMInfo object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
OYAPI oyCMMInfo_s * OYEXPORT
                   oyCMMInfo_Copy ( oyCMMInfo_s       * obj,
                                       oyObject_s          object )
{
  oyCMMInfo_s * s = 0;

  if(!obj || obj->type != oyOBJECT_CMM_INFO_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMInfo_Copy_( obj, object );

  return s;
}
 
/** Function oyCMMInfo_Release
 *  @memberof oyCMMInfo_s
 *  @brief   release and possibly deallocate a CMMInfo object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
OYAPI int  OYEXPORT
               oyCMMInfo_Release     ( oyCMMInfo_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyCMMInfo_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type != oyOBJECT_CMM_INFO_S)
  {
    WARNc_S(("Attempt to release a non oyCMMInfo_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->backend_version)
      deallocateFunc( s->backend_version );

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @} *//* backend_api */


/** \addtogroup misc Miscellaneous

 *  @{
 */

int                oyBigEndian       ( void )
{
  int big = 0;
  char testc[2] = {0,0};
  uint16_t *testu = (uint16_t*)testc;
  *testu = 1;
  big = testc[1];
  return big;
}


/** @brief MSB<->LSB */
icUInt16Number
oyValueUInt16 (icUInt16Number val)
{
  if(!oyBigEndian())
  {
  # define BYTES 2
  # define KORB  4
    unsigned char        *temp  = (unsigned char*) &val;
    unsigned char  korb[KORB];
    int i;
    for (i = 0; i < KORB ; i++ )
      korb[i] = (int) 0;  /* empty */

    {
    int klein = 0,
        gross = BYTES - 1;
    for (; klein < BYTES ; klein++ ) {
      korb[klein] = temp[gross--];
    }
    }

    {
    unsigned int *erg = (unsigned int*) &korb[0];

  # undef BYTES
  # undef KORB
    return (long)*erg;
    }
  } else
  return (long)val;
}

icUInt32Number
oyValueUInt32 (icUInt32Number val)
{
  if(!oyBigEndian())
  {
    unsigned char        *temp = (unsigned char*) &val;

    unsigned char  uint32[4];

    uint32[0] = temp[3];
    uint32[1] = temp[2];
    uint32[2] = temp[1];
    uint32[3] = temp[0];

    {
    unsigned int *erg = (unsigned int*) &uint32[0];


    return (icUInt32Number) *erg;
    }
  } else
    return (icUInt32Number)val;
}

unsigned long
oyValueUInt64 (icUInt64Number val)
{
  if(!oyBigEndian())
  {
    unsigned char        *temp  = (unsigned char*) &val;

    unsigned char  uint64[8];
    int little = 0,
        big    = 8;

    for (; little < 8 ; little++ ) {
      uint64[little] = temp[big--];
    }

    {
    unsigned long *erg = (unsigned long*) &uint64[0];

    return (long)*erg;
    }
  } else
  return (long)val;
}

#define icValue_to_icUInt32Number_m(funkname, typ) \
typ \
funkname (typ val) \
{ \
  icUInt32Number i = (icUInt32Number) val; \
  return (typ) oyValueUInt32 (i); \
}

icValue_to_icUInt32Number_m( oyValueCSpaceSig, icColorSpaceSignature )
icValue_to_icUInt32Number_m( oyValuePlatSig, icPlatformSignature )
icValue_to_icUInt32Number_m( oyValueProfCSig, icProfileClassSignature )
icValue_to_icUInt32Number_m( oyValueTagSig, icTagSignature )


oyPointer  oyStruct_LockCreateDummy_   ( oyStruct_s      * obj )    {return 0;}
void       oyLockReleaseDummy_         ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyLockDummy_                ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyUnLockDummy_              ( oyPointer         look,
                                         const char      * marker,
                                         int               line ) {;}


oyStruct_LockCreate_f   oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
oyLockRelease_f         oyLockReleaseFunc_ = oyLockReleaseDummy_;
oyLock_f                oyLockFunc_        = oyLockDummy_;
oyUnLock_f              oyUnLockFunc_      = oyUnLockDummy_;



/** Function: oyThreadLockingSet
 *  @brief set locking functions for threaded applications
 *
 *  @since Oyranos: version 0.1.8
 *  @date  14 january 2008 (API 0.1.8)
 */
void         oyThreadLockingSet        ( oyStruct_LockCreate_f  createLockFunc,
                                         oyLockRelease_f   releaseLockFunc,
                                         oyLock_f          lockFunc,
                                         oyUnLock_f        unlockFunc )
{
  oyStruct_LockCreateFunc_ = createLockFunc;
  oyLockReleaseFunc_ = releaseLockFunc;
  oyLockFunc_ = lockFunc;
  oyUnLockFunc_ = unlockFunc;

  if(!oyStruct_LockCreateFunc_ && !oyLockReleaseFunc_ &&
     !oyLockFunc_ && !oyUnLockFunc_)
  {
    oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
    oyLockReleaseFunc_ = oyLockReleaseDummy_;
    oyLockFunc_ = oyLockDummy_;
    oyUnLockFunc_ = oyUnLockDummy_;
  }
}


/** @} *//*misc */

/** @} *//*alpha
 */




/** \addtogroup monitor_api Monitor API
 *  @brief Hardware detection and profile selection for monitors

 *  The notation of screens for the X Window system is equal for traditional 
 *  and Xinerama screens. \n
 *  The X11 libraries will be loaded at runtime.
 *
 *  The functions can be studied by looking at the oyranos-monitor utiliy.
 *  The code for that tool is in oyranos_gamma.c .
 *
 *  @{
 */

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
int      oyGetMonitorInfo            ( const char        * display_name,
                                       char             ** manufacturer,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** system_port,
                                       char             ** display_geometry,
                                       oyBlob_s         ** edid,
                                       oyAlloc_f           allocate_func )
{
  int error = 0;
  oyGetMonitorInfo_f funcP = 0;
  char cmm[] = {"oyX1"};

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
      funcP = api2->oyGetMonitorInfo;
    }
  }

  if(funcP)
    error = funcP( display_name, manufacturer, model, serial, 
                   display_geometry, system_port,
                   edid, allocate_func );

  return error;
}

/** Function: oyGetScreenFromPosition
 *  @brief   screen number from position
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
int      oyGetScreenFromPosition     ( const char        * display_name,
                                       int                 x,
                                       int                 y )
{
  int error = 0;
  oyGetScreenFromPosition_f funcP = 0;
  char cmm[] = {"oyX1"};

  int screen = 0;

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
      funcP = api2->oyGetScreenFromPosition;
    }
  }

  if(funcP)
    screen = funcP( display_name, x, y );

  return screen;
}

/** Function: oyGetDisplayNameFromPosition
 *  @brief   display name from position
 *
 *  This function will hit exact results only with Xinerama.
 *
 *  @param      display_name  raw display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @param      allocate_func function used to allocate memory for the string
 *  @return                   display name
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
char *   oyGetDisplayNameFromPosition( const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAlloc_f           allocate_func )
{
  int error = 0;
  oyGetDisplayNameFromPosition_f funcP = 0;
  char cmm[] = {"oyX1"};

  char * text = 0;

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
      funcP = api2->oyGetDisplayNameFromPosition;
    }
  }

  if(funcP)
    text = funcP( display_name, x, y, allocate_func );

  return text;
}


/** Function: oyGetMonitorProfile
 *  @brief   get the monitor profile from the server
 *
 *  @param      display_name           the display string
 *  @param[out] size                   the size of profile
 *  @param      allocate_func          function used to allocate memory for the profile
 *  @return                            the memory block containing the profile
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
char *   oyGetMonitorProfile         ( const char        * display_name,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func )
{
  int error = 0;
  oyGetMonitorProfile_f funcP = 0;
  char cmm[] = {"oyX1"};

  char * block = 0;

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
      funcP = api2->oyGetMonitorProfile;
    }
  }

  if(funcP)
    block = funcP( display_name, size, allocate_func );

  return block;
}

/** Function: oyGetMonitorProfileNameFromDB
 *  @brief   get the monitor profile filename from the device profile database
 *
 *  @param      display_name           the display string
 *  @param      allocate_func function used to allocate memory for the string
 *  @return                   the profiles filename (if localy available)
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
char *   oyGetMonitorProfileNameFromDB(const char        * display_name,
                                       oyAlloc_f           allocate_func )
{
  int error = 0;
  oyGetMonitorProfileName_f funcP = 0;
  char cmm[] = {"oyX1"};

  char * text = 0;

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
      funcP = api2->oyGetMonitorProfileName;
    }
  }

  if(funcP)
    text = funcP( display_name, allocate_func );

  return text;
}

/** Function: oySetMonitorProfile
 *  @brief   set the monitor profile by filename
 *
 *  The profile_name argument does two things. Set to zero the function solely
 *  unsets the graphic card luts and the server stored profile. So pretty all
 *  server side informatin should go away. \n
 *  With a profile name given the function will lookup the monitor in the 
 *  Oyranos device database and stores the given profile there. \n
 *  To sum up, to set a new profile please call the following sequence:
 *  @verbatim
    // store new settings in the Oyranos data base
    oySetMonitorProfile( display_name, profil_name );
    // remove the server entries
    oySetMonitorProfile( display_name, 0 );
    // update the window server from the newly Oyranos data base settings
    oyActivateMonitorProfiles( display_name );
    @endverbatim
 *
 *  @param      display_name  the display string
 *  @param      profil_name   the file to use as monitor profile or 0 to unset
 *  @return                   error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
int      oySetMonitorProfile         ( const char        * display_name,
                                       const char        * profil_name )
{
  int error = 0;
  oySetMonitorProfile_f funcP = 0;
  char cmm[] = "oyX1";

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
      funcP = api2->oySetMonitorProfile;
    }
  }

  if(funcP)
    error = funcP( display_name, profil_name );

  return error;
}

/** Function: oyActivateMonitorProfiles
 *  @brief   activate the monitor using the stored configuration
 *
 *  Activate in case the appropriate profile is not yet setup in the server. \n
 *  To deactivate a profile in the server call 
 *  oySetMonitorProfile( display_name, 0 ).
 *
 *  @see oySetMonitorProfile for permanently configuring a monitor
 *
 *  @param   display_name              the display string
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
int      oyActivateMonitorProfiles   ( const char        * display_name )
{
  int error = 0;
  oyActivateMonitorProfiles_f funcP = 0;
  char cmm[] = "oyX1";

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API2_S, cmm, 0, 0, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
      funcP = api2->oyActivateMonitorProfiles;
    }
  }

  if(funcP)
    error = funcP( display_name );

  return error;
}


/** @} *//* monitor_api */

