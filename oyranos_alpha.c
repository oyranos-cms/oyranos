/** @file oyranos_alpha.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    object APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include "oyPointer_s.h"
#include "oyName_s.h"
#include "oyObserver_s.h"
#include "oyOption_s.h"
#include "oyOptions_s.h"

#include "oyranos_types.h"
#include "oyranos_alpha.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#ifdef HAVE_POSIX
#include <dlfcn.h>
#endif
#include <math.h>
#include <locale.h>   /* LC_NUMERIC */
#include <limits.h>

#define OY_ERR if(l_error != 0) error = l_error;

#ifdef DEBUG_
#define DEBUG_OBJECT 1
#endif

#ifdef NO_OPT
/* speed comparision test */
#define USE_OLD_STRING_API 1
#else
#define OY_USE_OBJECT_POOL_ 0
#endif



/* internal declarations */

char **          oyCMMsGetNames_     ( uint32_t          * n,
                                       const char        * sub_path,
                                       const char        * ext,
                                       const char        * required_cmm,
                                       oyPATH_TYPE_e       path_type );
char **          oyCMMsGetLibNames_  ( uint32_t          * n,
                                       const char        * required_cmm );
oyOBJECT_e   oyCMMapi3_Query_        ( oyCMMInfo_s       * cmm_info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank );

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
              (int)n/channels_n
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

  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );
    
  return text;
}


/** @brief copy pure colours
 *
 *  Handle colour only.
 *  With a empty \b from variable set -1 as default in \b to.
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
  {
    error = !memcpy( to, from, sizeof(double) * (n * c) );
    if(error)
      WARNc_S("Problem with memcpy.");

  } 
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


/** Function oyStruct_Allocate
 *  @brief   let a object allocate some memory
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/00 (Oyranos: 0.1.10)
 *  @date    2008/12/00
 */
oyPointer    oyStruct_Allocate       ( oyStruct_s        * st,
                                       size_t              size )
{
  oyAlloc_f allocateFunc = oyAllocateFunc_;

  if(st && st->oy_ && st->oy_->allocateFunc_)
    allocateFunc = st->oy_->allocateFunc_;

  return allocateFunc( size );
}



int          oyStruct_GetId          ( oyStruct_s        * st )
{
  int id = -1;

  if(st && st->oy_)
    id = oyObject_GetId(st->oy_);

  return id;
}



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
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCallback_Copy;
  s->release = (oyStruct_Release_f) oyCallback_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
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

  if(error <= 0)
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

  if(error <= 0)
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





/**
 *  @} *//* objects_generic
 */

/** @} *//* misc */



/** \addtogroup module_api

 *  @{
 */



/** @internal
 *  @brief   convert between oyPointer_s data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int          oyPointer_ConvertData   ( oyPointer_s       * cmm_ptr,
                                       oyPointer_s       * cmm_ptr_out,
                                       oyFilterNode_s    * node )
{
  int error = !cmm_ptr || !cmm_ptr_out;
  oyCMMapi6_s * api6 = 0;
  char * reg = 0, * tmp = 0;

  if(error <= 0)
  {
    reg = oyStringCopy_( "//", oyAllocateFunc_ );
    tmp = oyFilterRegistrationToText( node->core->registration_,
                                      oyFILTER_REG_TYPE,0);
    STRING_ADD( reg, tmp );
    oyFree_m_( tmp );
    STRING_ADD( reg, "/" );
    STRING_ADD( reg, oyPointer_GetResourceName( cmm_ptr ) );
    STRING_ADD( reg, "_" );
    STRING_ADD( reg, oyPointer_GetResourceName( cmm_ptr_out ) );

    api6 = (oyCMMapi6_s*) oyCMMsGetFilterApi_( 0, reg, oyOBJECT_CMM_API6_S );

    error = !api6;
  }

  if(error <= 0 && api6->oyModuleData_Convert)
    error = api6->oyModuleData_Convert( cmm_ptr, cmm_ptr_out, node );
  else
    error = 1;

  if(error)
    WARNc_S("Could not convert context");

  return error;
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



/** Function oyCMMapis_New
 *  @memberof oyCMMapis_s
 *  @brief   allocate a new CMMapis list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI oyCMMapis_s * OYEXPORT
           oyCMMapis_New             ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_APIS_S;
# define STRUCT_TYPE oyCMMapis_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCMMapis_Copy;
  s->release = (oyStruct_Release_f) oyCMMapis_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/** @internal
 *  Function oyCMMapis_Copy_
 *  @memberof oyCMMapis_s
 *  @brief   real copy a CMMapis object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
oyCMMapis_s * oyCMMapis_Copy_
                                     ( oyCMMapis_s       * obj,
                                       oyObject_s          object )
{
  oyCMMapis_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyCMMapis_New( object );
  error = !s;

  if(!error)
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );

  if(error)
    oyCMMapis_Release( &s );

  return s;
}

/** Function oyCMMapis_Copy
 *  @memberof oyCMMapis_s
 *  @brief   copy or reference a CMMapis list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI oyCMMapis_s * OYEXPORT
           oyCMMapis_Copy            ( oyCMMapis_s       * obj,
                                       oyObject_s          object )
{
  oyCMMapis_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapis_Copy_( obj, object );

  return s;
}
 
/** Function oyCMMapis_Release
 *  @memberof oyCMMapis_s
 *  @brief   release and possibly deallocate a CMMapis list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI int  OYEXPORT
           oyCMMapis_Release         ( oyCMMapis_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyCMMapis_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 1 )

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


/** Function oyCMMapis_MoveIn
 *  @memberof oyCMMapis_s
 *  @brief   add a element to a CMMapis list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI int  OYEXPORT
           oyCMMapis_MoveIn          ( oyCMMapis_s       * list,
                                       oyCMMapi_s       ** obj,
                                       int                 pos )
{
  oyCMMapis_s * s = list;
  int error = 0;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_APIS_S, return 1 )

  if(obj && *obj &&
     (*obj)->type >= oyOBJECT_CMM_API1_S &&
     (*obj)->type <= oyOBJECT_CMM_API_MAX)
  {
    if(!s)
    {
      s = oyCMMapis_New(0);
      error = !s;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
  }   
  
  return error;
}

/** Function oyCMMapis_ReleaseAt
 *  @memberof oyCMMapis_s
 *  @brief   release a element from a CMMapis list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI int  OYEXPORT
           oyCMMapis_ReleaseAt       ( oyCMMapis_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyCMMapis_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_CMM_APIS_S, return 1 )

  if(!error && list->type_ != oyOBJECT_CMM_APIS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function oyCMMapis_Get
 *  @memberof oyCMMapis_s
 *  @brief   get a element of a CMMapis list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI oyCMMapi_s * OYEXPORT
           oyCMMapis_Get             ( oyCMMapis_s       * list,
                                       int                 pos )
{       
  int error = !list;
  oyCMMapis_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_CMM_APIS_S, return 0 )

  if(!error)
    return (oyCMMapi_s *) oyStructList_GetRef( list->list_, pos ); 
  else  
    return 0;
}   

/** Function oyCMMapis_Count
 *  @memberof oyCMMapis_s
 *  @brief   count the elements in a CMMapis list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI int  OYEXPORT
           oyCMMapis_Count           ( oyCMMapis_s       * list )
{       
  int error = !list;
  oyCMMapis_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_CMM_APIS_S, return 0 )

  if(!error)
    return oyStructList_Count( list->list_ );
  else return 0;
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

  if(error <= 0 && oy_cmm_handles_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_handles_);
  if(error <= 0)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_(oy_cmm_handles_, i);
    oyPointer_s * s;

    if(obj && obj->type_ == oyOBJECT_POINTER_S)
      s = (oyPointer_s*) obj;
    else
      s = 0;

    if( s && oyPointer_GetLibName( s ) && lib_name &&
        !oyStrcmp_( oyPointer_GetLibName( s ), lib_name ) )
    {
      found = 1;
      oyStructList_ReferenceAt_(oy_cmm_handles_, i);
      if(ptr)
      {
        if(!oyPointer_GetPointer( s ))
          oyPointer_Set( s, 0, 0, ptr, 0, 0 );
        /*else
          WARNc_S(("Attempt to register dso handle multiple times."));*/
      }
    }
  }

  if(!found)
  {
    oyPointer_s * s = oyPointer_New(0);
    oyStruct_s * oy_cmm_struct = 0;

    error = !s;

    if(error <= 0)
      error = oyPointer_Set( s, lib_name, 0, ptr, "oyDlclose", oyDlclose );

    if(error <= 0)
      oy_cmm_struct = (oyStruct_s*) s;

    if(error <= 0)
      oyStructList_MoveIn(oy_cmm_handles_, &oy_cmm_struct, -1, 0);
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
  if(error <= 0)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_(oy_cmm_handles_, i);
    oyPointer_s * s = 0;

    if(obj && obj->type_ == oyOBJECT_POINTER_S)
      s = (oyPointer_s*) obj;

    error = !s;

    if(error <= 0)
    if( oyPointer_GetLibName(s) && lib_name &&
        !oyStrcmp_( oyPointer_GetLibName(s), lib_name ) )
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

  if(error <= 0)
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

#ifdef HAVE_POSIX
  found = oyCMMdsoSearch_(lib_name);

  if(found >= 0)
  {
    oyPointer_s * s = (oyPointer_s*)oyStructList_GetType_( oy_cmm_handles_, found,
                                                  oyOBJECT_POINTER_S );

    if(s)
      dso_handle = oyPointer_GetPointer(s);
  }

  if(!dso_handle)
  {
    dso_handle = dlopen( lib_name, RTLD_LAZY );

    if(!dso_handle)
    {
      WARNc2_S( "\n  dlopen( %s, RTLD_LAZY):\n  \"%s\"", lib_name, dlerror() );
      system("  echo $LD_LIBRARY_PATH");
    }
  }

  if(dso_handle)
    oyCMMdsoReference_( lib_name, dso_handle );

  return dso_handle;
#endif
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0 && object)
  {
    s = oyCMMhandle_New_(object);
    error = !s;
    if(error <= 0)
    {
      if(handle)
        WARNc1_S("Dont know how to copy CMM[%s] handle.", handle->lib_name);

      error = oyCMMhandle_Set_( s, 0, 0, 0 );
      if(!error)
        oyCMMhandle_Release_( &s );

    } else
      WARNc_S("Could not create a new object.");

  } else if(handle)
  {
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

  oyCheckType__m( oyOBJECT_CMM_HANDLE_S, return 1 )

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

  if(error <= 0)
  {
    handle->info = info;
    handle->dso_handle = dso_handle;
    handle->lib_name = oyStringCopy_( lib_name, handle->oy_->allocateFunc_ );
    oyCMMdsoReference_( handle->lib_name, dso_handle );
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

  if(error <= 0 && !oy_cmm_infos_)
  {
    oy_cmm_infos_ = oyStructList_New( 0 );
    error = !oy_cmm_infos_;
  }

  if(error <= 0 && oy_cmm_infos_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_infos_);
  if(error <= 0)
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
      if(!error)
        break;
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

  if(lib_name && lib_name[0])
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
 *  @brief get all CMM/module/script names
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2010/06/28
 */
char **          oyCMMsGetNames_     ( uint32_t          * n,
                                       const char        * sub_path,
                                       const char        * ext,
                                       const char        * required_cmm,
                                       oyPATH_TYPE_e       path_type )
{
  int error = !n;
  char ** files = 0,
       ** sub_paths = 0;
  int sub_paths_n = 0;

  if(error <= 0)
  {
    int  files_n = 0, i;
    char * lib_string = oyAllocateFunc_(24);
    const char * cmm = 0;

    lib_string[0] = 0;
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
      {
        if(path_type == oyPATH_MODULE)
          files = oyLibFilesGet_( &files_n, sub_paths[i], oyUSER_SYS,
                                  0, lib_string, 0, oyAllocateFunc_ );
        else if(path_type == oyPATH_SCRIPT)
          files = oyDataFilesGet_( &files_n, sub_paths[i], oyYES, oyUSER_SYS,
                                   0, lib_string, ext, oyAllocateFunc_ );
      }
    }
    error = !files;

    if(!error)
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
char **          oyCMMsGetLibNames_  ( uint32_t          * n,
                                       const char        * required_cmm )
{
#ifdef HAVE_POSIX
  return oyCMMsGetNames_(n, OY_METASUBPATH, 0, required_cmm, oyPATH_MODULE);
#else
  *n = 0;
  return 0;
#endif
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
  const char * error_text = 0;

  if(error <= 0)
  {
    oyPointer dso_handle = 0;

    if(error <= 0)
    {
      if(lib_name)
        dso_handle = oyCMMdsoGet_(cmm, lib_name);

      error = !dso_handle;

      if(error)
      {
        error_text = dlerror();
        WARNc2_S( "\n  error while dlopen'ing lib:\n    %s\n  dlerror(): %s",
                  lib_name, error_text ? error_text : "no text" );
      }
    }

    /* open the module */
    if(error <= 0)
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

      if(error <= 0)
        if(oyCMMapi_Check_( cmm_info->api ))
          api = cmm_info->api;

      if(error <= 0 && api)
      {
        error = api->oyCMMMessageFuncSet( oyMessageFunc_p );

        cmm_handle = oyCMMhandle_New_(0);

        /* init */
        if(error <= 0)
        error = api->oyCMMInit( (oyStruct_s*) api );
        if(error <= 0)
          error = oyCMMhandle_Set_( cmm_handle, cmm_info, dso_handle, lib_name);

        api_found = 1;
      }

      /* store */
      if(error <= 0 && api_found)
        oyStructList_MoveIn(oy_cmm_infos_, (oyStruct_s**)&cmm_handle, -1, 0);
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

  if(error <= 0)
  {
    cmm_handle = oyCMMFromCache_( lib_name );
    if(cmm_handle && cmm_handle->info->api)
    {
      cmm_info = cmm_handle->info;
      found = 1;
    }
  }

  if(error <= 0 && !found)
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
  STRING_ADD( text, cmm_info->getText( select, oyNAME_NICK, (oyStruct_s*) cmm_info ) ); \
  STRING_ADD( text, "\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_NAME, (oyStruct_s*) cmm_info ) ); \
  STRING_ADD( text, "\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_DESCRIPTION, (oyStruct_s*) cmm_info)); \
  STRING_ADD( text, "\n" );

  CMMINFO_ADD_NAME_TO_TEXT( _("Name"), "name" )
  CMMINFO_ADD_NAME_TO_TEXT( _("Manufacturer"), "manufacturer" )
  CMMINFO_ADD_NAME_TO_TEXT( _("Copyright"), "copyright" )

      if(cmm_info)
      {
        tmp = cmm_info->api;
        STRING_ADD( text, "\n  API(s):\n" );

        while(tmp)
        {
          type = oyCMMapi_Check_(tmp);

          oySprintf_(num,"    %d:", type );
          STRING_ADD( text, num );
          STRING_ADD( text, oyStructTypeToText( tmp->type ) );

          if(type == oyOBJECT_CMM_API4_S)
          {
            cmm_api4 = (oyCMMapi4_s*) tmp;
            STRING_ADD( text, "\n    Registration: " );
            STRING_ADD( text, cmm_api4->registration );
            CMMINFO_ADD_NAME_TO_TEXT( "Name", "name" )
          }
          STRING_ADD( text, "\n" );

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
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/03 (Oyranos: 0.1.8)
 *  @date    2009/09/02
 */
int              oyCMMCanHandle_    ( oyCMMapi3_s        * api,
                                      oyCMMapiQueries_s  * queries )
{
  int capable = 0;
  int error = !api;
  int i, ret = 0;

  if(error <= 0)
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
oyOBJECT_e   oyCMMapi_selectFilter_  ( oyCMMInfo_s       * info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank )
{
  oyOBJECT_e type = oyOBJECT_NONE;
  oyCMMapiFilter_s * cmm_api = (oyCMMapiFilter_s *) api;
  int error = !data || !api;
  oyRegistrationData_s * reg_filter;
  int found = 0;

  if(error <= 0)
    reg_filter = (oyRegistrationData_s*) data;

  if(error <= 0 &&
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
    {
      type = api->type;
      if(rank)
        *rank = 1;
      else
        *rank = 0;
    }
  }

  return type;
}

static oyStructList_s * oy_meta_module_cache_ = 0;

/** @internal
 *  Function oyCMMGetMetaApis_
 *  @brief   get module loaders
 *
 *  This function searches for a meta modul API.
 *
 *  @param[in]   cmm                   the four byte module ID, optional
 *
 *  @version Oyranos: 0.1.11
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2010/09/14
 */
oyCMMapis_s* oyCMMGetMetaApis_       ( const char        * cmm )
{
  oyCMMapis_s * meta_apis = 0;
  meta_apis = oyCMMsGetMetaApis_( cmm );
  return meta_apis;
}


/** @internal
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/16 (Oyranos: 0.1.9)
 *  @date    2009/09/02
 */
oyOBJECT_e   oyCMMapi_CheckWrap_     ( oyCMMInfo_s       * cmm_info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank )
{
  oyOBJECT_e type = oyCMMapi_Check_( api );
  if(rank)
  {
    if(type)
      *rank = 1;
    else
      *rank = 0;
  }
  return type;
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
 *  @param[in]   num                   position in api chain matching to type
 *                                     and apiCheck/check_pointer starting from
 *                                     zero,
 *                                     -1 means: pick the first match, useful 
 *                                     in case the API position is known or to
 *                                     iterate through all matching API's
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
  uint32_t rank = 0;

  if(error <= 0 &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(error <= 0)
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
        if(apiCheck(cmm_info, tmp, check_pointer, &rank) == type)
        {
          if((num >= 0 && num == i) ||
             num < 0 )
            api = tmp;

          ++i;
        }
        tmp = tmp->next;
      }
    }

    oyCMMInfo_Release( &cmm_info );
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

  if(error <= 0)
    reg_filter = (oyRegistrationData_s*) data;

  if(error <= 0 &&
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
 *  @brief let a oyCMMapi5_s meta module open a set of modules
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *    - select complementing API's
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
 *  The modules are filtered for double occurencies. The highest rank or
 *  version is selected.
 *
 *  @param[in]   cmm_meta              the selected meta API will be used,
 *                                     can be Zero or at least 4 bytes long,
 *                                     e.g. "oyIM" for the library module loader
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  CMM API
 *  @param[in]   flags                 supported is 
 *                                     - oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR 
 *                                       for omitting implementation attributes
 *  @param[out]  rank_list             the ranks matching the returned list;
 *                                     without that only the most matching API  
 *                                     is returned at position 0
 *  @param[out]  count                 count of returned modules
 *  @return                            a zero terminated list of modules
 *
 *  @version Oyranos: 0.1.11
 *  @since   2008/12/19 (Oyranos: 0.1.10)
 *  @date    2010/08/12
 */
oyCMMapiFilters_s*oyCMMsGetFilterApis_(const char        * cmm_meta,
                                       const char        * cmm_required,
                                       const char        * registration,
                                       oyOBJECT_e          type,
                                       uint32_t            flags,
                                       uint32_t         ** rank_list,
                                       uint32_t          * count )
{
  int error = !oyIsOfTypeCMMapiFilter( type );
  oyCMMapiFilters_s * apis = 0, * apis2 = 0;
  oyCMMapiFilter_s * api = 0,
                   * api2 = 0;
  uint32_t * rank_list_ = 0, * rank_list2_ = 0;
  int rank_list_n = 5, count_ = 0;
  oyObject_s object = oyObject_New();

  /*{
    if(!oy_module_cache_)
      oy_module_cache_ = oyStructList_New( 0 );

    entry = oyCacheListGetEntry_ ( oy_module_cache_, registration );

    api = (oyCMMapiFilter_s*) oyHash_GetPointer_( entry, type );

    if(api)
    {
      oyHash_Release_( &entry );
      return api;
    }
  }*/

  if(error <= 0)
  {
    oyCMMapi5_s * api5 = 0;
    oyCMMapis_s * meta_apis = oyCMMGetMetaApis_( cmm_meta );
    int meta_apis_n = 0;
    char ** files = 0;
    uint32_t  files_n = 0;
    int i, j, k = 0, match_j = -1, ret, match_i = -1, rank = 0, old_rank = 0,
        n, accept;
    char * match = 0, * reg = 0;
    oyCMMInfo_s * info = 0;

    meta_apis_n = oyCMMapis_Count( meta_apis );
    for(k = 0; k < meta_apis_n; ++k)
    {
      api5 = (oyCMMapi5_s*) oyCMMapis_Get( meta_apis, k );

      files_n = 0;

      if(error <= 0)
      files = oyCMMsGetNames_(&files_n, api5->sub_paths, api5->ext,
                              cmm_required, 
                              api5->data_type == 0 ? oyPATH_MODULE :
                                                     oyPATH_SCRIPT);
      else
        WARNc2_S("%s: %s", _("Could not open meta module API"),
                 oyNoEmptyString_m_( registration ));

      if(files)
      for( i = 0; i < files_n; ++i)
      {
        ret = 0; j = 0;
        ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                     0, 0, oyAllocateFunc_, &info, object );
        while(!ret)
        {
          ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                       &reg, 0, oyAllocateFunc_, 0, 0 );
          if(!ret && reg)
          {
            rank = oyFilterRegistrationMatch( reg, registration, type );
            if(rank && OYRANOS_VERSION == info->oy_compatibility)
              ++rank;

            if(rank && rank_list)
            {

              if(!rank_list_ && !apis)
              {
                *rank_list = 0;
                oyAllocHelper_m_( *rank_list, uint32_t, rank_list_n+1, 0,
                                  goto clean );
                rank_list_ = *rank_list;
                apis = oyCMMapiFilters_New(0);
              } else
              if(count_ >= rank_list_n)
              {
                rank_list_n *= 2;
                rank_list_ = 0;
                oyAllocHelper_m_( rank_list_, uint32_t, rank_list_n+1, 0,
                                  goto clean );
                error = !memcpy( rank_list_, *rank_list,
                                 sizeof(uint32_t) * rank_list_n/2 );
                oyFree_m_(*rank_list);
                *rank_list = rank_list_;
              }

              rank_list_[k++] = rank;
              api = api5->oyCMMFilterLoad( 0,0, files[i], type, j);
              if(!api)
                continue;

              if(!api->id_)
                api->id_ = oyStringCopy_( files[i], oyAllocateFunc_ );
              api->api5_ = api5;
              if(!apis)
                apis = oyCMMapiFilters_New( 0 );
              oyCMMapiFilters_MoveIn( apis, &api, -1 );
              ++ count_;
              if(count)
                *count = count_;

            } else
            if(rank > old_rank)
            {
              match = reg;
              match_j = j;
              match_i = i;
              old_rank = rank;
            }
          }
          ++j;
          if(reg) oyFree_m_( reg );
        }
      }

      oyCMMInfo_Release( &info );
#if 0
      if(api5->release)
        api5->release( (oyStruct_s**)&api5 );
#endif
    }

    oyCMMapis_Release( &meta_apis );

    if(match && !rank_list)
    {
      apis2 = oyCMMapiFilters_New( 0 );
      api = api5->oyCMMFilterLoad( 0,0, files[match_i], type, match_j );
      if(!api->id_)
        api->id_ = oyStringCopy_( files[match_i], oyAllocateFunc_ );
      api->api5_ = api5;
      oyCMMapiFilters_MoveIn( apis2, &api, -1 );
      if(count)
        *count = 1;
    }

    if(rank_list_)
    {
      /* filter doubled entries */
      n = oyCMMapiFilters_Count( apis );
      oyAllocHelper_m_( rank_list2_, uint32_t, rank_list_n+1, 0, goto clean );
      k = 0;
      for(i = 0 ; i < n; ++i)
      {
        char * apir = 0;

        api = oyCMMapiFilters_Get( apis, i );
        if(flags | oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR)
          oyFilterRegistrationModify( api->registration,
                                    oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                      &apir, 0 );
        accept = 1;

        for(j = i+1; j < n; ++j)
        {
          char * api2r = 0;

          api2 = oyCMMapiFilters_Get( apis, j );
          if(flags | oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR)
            oyFilterRegistrationModify( api2->registration,
                                    oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                        &api2r, 0 );

          /* for equal registration compare rank and version */
          if(oyStrcmp_( apir,  api2r ) == 0 &&
             rank_list_[i] <= rank_list_[j])
            accept = 0;

          oyFree_m_(api2r);

          if(api2->release)
            api2->release( (oyStruct_s**)&api2 );
        }

        if(!apis2)
          apis2 = oyCMMapiFilters_New( 0 );

        if(accept)
        {
          oyCMMapiFilters_MoveIn( apis2, &api, -1 );
          rank_list2_[k++] = rank_list_[i];
        }

        oyFree_m_(apir);

        if(api && api->release)
          api->release( (oyStruct_s**)&api );
      }

      if(rank_list)
        *rank_list = rank_list2_;
      if(rank_list_)
        oyFree_m_(rank_list_);
      if(count)
        *count = k;
    }
      
    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  /*if(api)
  {
    api->id_ = lib_name;
    error = oyHash_SetPointer_( entry, (oyStruct_s*) s );
  }*/

  clean:
    oyObject_Release( &object );

  return apis2;
}

oyStructList_s * oy_cmm_filter_cache_ = 0;

/** @internal
 *  Function oyCMMsGetFilterApi_
 *  @brief let a oyCMMapi5_s meta module open a module
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
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  CMM API
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/15 (Oyranos: 0.1.9)
 *  @date    2008/06/02
 */
oyCMMapiFilter_s *oyCMMsGetFilterApi_( const char        * cmm_required,
                                       const char        * registration,
                                       oyOBJECT_e          type )
{
  oyCMMapiFilter_s * api = 0;
  oyCMMapiFilters_s * apis = 0;
  oyHash_s * entry = 0;
#ifndef NO_OPT
  int len = oyStrlen_(registration);
  char * hash_text = oyAllocateFunc_( len + 10 );
  char api_char = oyCMMapiNumberToChar( type );

  oySprintf_( hash_text, "%s.%c_", registration, api_char ? api_char:' ');
  entry = oyCMMCacheListGetEntry_( hash_text );
  oyDeAllocateFunc_( hash_text ); hash_text = 0;
  api = (oyCMMapiFilter_s*) oyHash_GetPointer( entry, type );
  if(api)
    return api;
#endif

  apis = oyCMMsGetFilterApis_( 0,cmm_required, registration, type, 
                               oyFILTER_REG_MODE_NONE, 0,0 );

  if(apis)
  {
    api = oyCMMapiFilters_Get( apis, 0 );
    oyCMMapiFilters_Release( &apis );
#ifndef NO_OPT
#ifdef NOT_YET
    if(api->copy)
      api = api->copy
#endif
    oyHash_SetPointer( entry, (oyStruct_s*) api );
#endif
  }

  return api;
}


/** @internal
 *  Function oyCMMsGetMetaApis_
 *  @brief get oyranos modules
 *
 *  @param[in]   cmm                  the selected module, optional
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/09/14
 */
oyCMMapis_s *    oyCMMsGetMetaApis_  ( const char        * cmm )
{
  int error = 0;
  oyCMMapis_s * apis = 0;
  int  found = 0;
  oyCMMapi_Check_f apiCheck = oyCMMapi_CheckWrap_;

  if(error <= 0)
  {
    char ** files = 0;
    uint32_t  files_n = 0;
    int i = 0;

    files = oyCMMsGetLibNames_(&files_n, cmm);

    if(!files_n)
      WARNc_S(_("Could not find any meta module. "
                "Did you set the OY_MODULE_PATHS variable,"
                " to point to a Oyranos module loader library?"));

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api,
                   * copy = 0;

        while(tmp)
        {
          found = 0;

          if(apiCheck(cmm_info, tmp, 0, 0) == oyOBJECT_CMM_API5_S)
          {
            if(!apis)
              apis = oyCMMapis_New(0);

            copy = tmp;
              
            oyCMMapis_MoveIn( apis, &copy, -1 );
          }

          tmp = tmp->next;
        }
      }

      oyCMMInfo_Release( &cmm_info );
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  return apis;
}

/** @internal
 *  Function oyCMMsGetApi_
 *  @brief get a oyranos module
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
 *  @param[in]   type                  the API type to return
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[out]  lib_used              inform about the selected CMM
 *  @param[in]   apiCheck              custom API selector
 *  @param[in]   check_pointer         data to pass to apiCheck
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/12 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       char             ** lib_used,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer )
{
  int error = !type;
  oyCMMapi_s * api = 0;
  int  found = 0;

  if(!apiCheck)
    apiCheck = oyCMMapi_CheckWrap_;

  if(error <= 0 &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(error <= 0)
  {
    char ** files = 0;
    uint32_t  files_n = 0;
    int i, oy_compatibility = 0;
    uint32_t rank = 0,
             max_rank = 0;
    int max_pos = -1;

    files = oyCMMsGetLibNames_(&files_n, cmm_required);

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api;

        while(tmp)
        {
          found = 0;

          if(apiCheck(cmm_info, tmp, check_pointer, &rank) == type)
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
                if(cmm_info->oy_compatibility == OYRANOS_VERSION)
                  ++rank;
                if(rank > max_rank)
                {
                  api = tmp;
                  max_rank = rank;
                  max_pos = i;

                }
                oy_compatibility = cmm_info->oy_compatibility;
              }
          }
          tmp = tmp->next;
        }
      }

      oyCMMInfo_Release( &cmm_info );
    }

    if(max_rank >= 0 && lib_used)
    {
      if(*lib_used)
        oyFree_m_(*lib_used);
      if(files && files_n)
        *lib_used = oyStringCopy_( files[max_pos], oyAllocateFunc_ );
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
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
  if(error <= 0)
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
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/06 (Oyranos: 0.1.8)
 *  @date    2009/09/02
 */
oyOBJECT_e       oyCMMapi_Check_     ( oyCMMapi_s        * api )
{
  int error = !api;
  oyOBJECT_e type = oyOBJECT_NONE;
  int module_api = api->module_api[0]*10000 + api->module_api[1]*100
                    + api->module_api[2];

  if(api->type <= oyOBJECT_CMM_API_S ||
     api->type >= oyOBJECT_CMM_API_MAX)
    error = 1;
  else
  {
    if(module_api < 300 ||  /* last API break */
       OYRANOS_VERSION < module_api)
    {
      error = 1;
      DBG_MEM3_S("Wrong API for: %s %s %d", oyStructTypeToText(api->type),
                 oyNoEmptyString_m_(api->registration), module_api);
      return type;
    }
  }

  if(error <= 0)
  switch(api->type)
  {
    case oyOBJECT_CMM_API1_S:
    {
      /* This module type is obsolete and should be replaced by oyCMMapi4_s. */
      error = 1;
    } break;
    case oyOBJECT_CMM_API2_S:
    {
      /* This module type is obsolete and should be replaced by oyCMMapi8_s. */
      error = 1;
    } break;
    case oyOBJECT_CMM_API3_S:
    {
      oyCMMapi3_s * s = (oyCMMapi3_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyCMMProfileTag_GetValues &&
           /*s-> &&*/
           s->oyCMMProfileTag_Create ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API4_S:
    {
      oyCMMapi4_s * s = (oyCMMapi4_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2])))
      {
        error = 1;
        WARNc2_S("Incomplete module header: %s %s",
                 oyStructTypeToText(api->type),
                 oyNoEmptyString_m_(api->registration));
      }
      if(!(s->oyCMMInit && s->ui))
      {
        int ui_module_api = s->ui->module_api[0]*10000
                            + s->ui->module_api[1]*100
                            + s->ui->module_api[2];

        if(ui_module_api < 110 ||  /* last API break */
          OYRANOS_VERSION < ui_module_api)
        {
          error = 1;
          WARNc2_S("Wrong UI API for: %s %s", oyStructTypeToText(api->type),
                   oyNoEmptyString_m_(api->registration));
          return type;
        }
      }
      if(s->ui && s->ui->oyCMMFilter_ValidateOptions &&
         !s->ui->oyWidget_Event)
      {
        error = 1;
        WARNc2_S("Incomplete module UI function set: %s %s",
                 oyStructTypeToText(api->type),
                 oyNoEmptyString_m_(api->registration));
      }
      if(s->context_type && s->context_type[0] &&
         !s->oyCMMFilterNode_ContextToMem)
      {
        error = 1;
        WARNc2_S("context_type provided but no oyCMMFilterNode_ContextToMem: %s %s",
                 oyStructTypeToText(api->type),
                 oyNoEmptyString_m_(api->registration));
      }
      if(!(s->ui && s->ui->texts &&
           s->ui->getText && s->ui->getText("name", oyNAME_NAME, (oyStruct_s*)
                                            s->ui)))
      {
        error = 1;
        WARNc2_S("Missed module name: %s %s",
                 oyStructTypeToText(api->type),
                 oyNoEmptyString_m_(api->registration));
      }
      if(!(s->ui && s->ui->category && s->ui->category[0]))
      {
        error = 1;
        WARNc2_S("Missed module category: %s %s",
                 oyStructTypeToText(api->type),
                 oyNoEmptyString_m_(api->registration));
      }
      if(s->ui && s->ui->options && s->ui->options[0] && !s->ui->oyCMMuiGet)
      {
        error = 1;
        WARNc2_S("options provided without oyCMMuiGet: %s %s",
                 oyStructTypeToText(api->type),
                 oyNoEmptyString_m_(api->registration));
      }
    } break;
    case oyOBJECT_CMM_API5_S:
    {
      oyCMMapi5_s * s = (oyCMMapi5_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->oyCMMFilterLoad &&
           s->oyCMMFilterScan
            ) )
      {
        int module_api = 10000*s->module_api[0] + 100*s->module_api[1] + 1*s->module_api[2];
        if(module_api < 110) /* last API break */
          error = 1;
      }
    } break;
    case oyOBJECT_CMM_API6_S:
    {
      oyCMMapi6_s * s = (oyCMMapi6_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->data_type_in && s->data_type_in[0] &&
           s->data_type_out && s->data_type_out[0] &&
           s->oyModuleData_Convert
            ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API7_S:
    {
      oyCMMapi7_s * s = (oyCMMapi7_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->oyCMMFilterPlug_Run &&
           ((s->plugs && s->plugs_n) || (s->sockets && s->sockets_n))
            ) )
      {
        int module_api = 10000*s->module_api[0] + 100*s->module_api[1] + 1*s->module_api[2];
        if(module_api < 110) /* last API break */
          error = 1;
      }
    } break;
    case oyOBJECT_CMM_API8_S:
    {
      oyCMMapi8_s * s = (oyCMMapi8_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           s->oyConfigs_FromPattern &&
           s->oyConfigs_Modify &&
           s->oyConfig_Rank &&
           s->rank_map
            ) )
        error = 1;
    } break;
    case oyOBJECT_CMM_API9_S:
    {
      oyCMMapi9_s * s = (oyCMMapi9_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           (!s->options ||
            (s->options && s->oyCMMFilter_ValidateOptions &&
             s->oyCMMuiGet && s->oyWidget_Event))&&
           (!s->texts ||
            ((s->texts || s->getText)
              && s->texts[0] && s->texts[0][0] && s->getText)) &&
           s->pattern && s->pattern[0]
            ) )
      {
        int module_api = 10000*s->module_api[0] + 100*s->module_api[1] + 1*s->module_api[2];
        if(module_api < 110) /* last API break */
          error = 1;
      }
    } break;
    case oyOBJECT_CMM_API10_S:
    {
      oyCMMapi10_s * s = (oyCMMapi10_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->registration && s->registration[0] &&
           (s->version[0] || s->version[1] || s->version[2]) &&
           (!s->texts ||
            ((s->texts || s->getText)
              && s->texts[0] && s->texts[0][0] && s->getText)) &&
           s->oyMOptions_Handle
            ) )
        error = 1;
    } break;
    default: break;
  }

  if(error <= 0)
    type = api->type;
  else
    WARNc2_S("Found problems with: %s %s", oyStructTypeToText(api->type),
              oyNoEmptyString_m_(api->registration));


  return type;
}


/** @} *//* module_api */



/** \addtogroup module_api Module APIs
 *
 *  Oyranos C modules provide support for data formats, data processing and
 *  process control, as well as configuration.
 *  The module architecture covers three basic layers. There are the Module 
 *  APIs for the real stuff, the Meta Module APIs for language support and the
 *  final Oyranos objects with their User APIs.\n
 *  @dot
digraph Modules {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  //rankdir=LR;
  graph [fontname=Helvetica, fontsize=14];
  node [shape=record,fontname=Helvetica, fontsize=11, width=.1];

  subgraph cluster_0 {
    label="Oyranos Module Framework";
    color=white;
    clusterrank=global;

      node [width = 1.5, style=filled];
      i [URL="structoyCMMapi5__s.html#_details", label="Module Importer\n oyCMMapi5_s"];

      p [ label="Filter nodes / Graph handling APIs\n oyFilterNode_s"];


      api4_A [URL="structoyCMMapi4__s.html#_details",label="Context Module + UI\n oyCMMapi4_s"];
      api6_A [URL="structoyCMMapi6__s.html#_details",label="Context Converter\n oyCMMapi6_s"];
      api7_A [URL="structoyCMMapi7__s.html#_details",label="Node Module\n oyCMMapi7_s"];

      m [URL="structoyCMMapi8__s.html#_details", label="Config (Device) Functions\n oyCMMapi8_s"];
      policy [URL="structoyCMMapi9__s.html#_details", label="Policy Functions\n oyCMMapi9_s"];


      subgraph cluster_1 {
        color=gray;
        label="Module APIs"

        subgraph cluster_2 {
          rank=max;
          color=red;
         style=dashed;
          node [style="filled"];
          api4_A; api6_A; api7_A;
          label="Graph Modules and Helpers";
        }

        subgraph cluster_6 {
          color=green;
          style=dashed;
          m;
          label="Device Modules";
        }

        subgraph cluster_9 {
          color=blue;
          style=dashed;
          policy;
          label="Policy Modules";
        }

      }

      subgraph cluster_10 {
        color=gray;
        node [style="filled"];
        i;
        label="Meta Module API";
      }

      subgraph cluster_11 {
        color=gray;
        node [style="filled"];
        p;
        label="User APIs";
      }

      p -> i [arrowhead="open", color=gray];
      i -> api4_A [arrowhead="open", color=red];
      i -> api6_A [arrowhead="open", color=red];
      i -> api7_A [arrowhead="open", color=red];
      i -> m [arrowhead="open", color=green];
      i -> policy [arrowhead="open", color=blue];
  }
} @enddot
 *
 *  \b User \b API:
 *  The user API allowes to contruct a filter or configuration
 *  object. Filters can be chained to directed acyclic graphs (DAGs) and data
 *  can be processed through graphs or they are deployed by higher level APIs,
 *  like named colour API.
 *
 *  \b Meta \b Modules:
 *  The modules are loaded into Oyranos by meta modules. A Meta module can
 *  support different formats of modules. The basic format are the native C
 *  structures and function declarations, which allow very detailed access and 
 *  control of Oyranos objects. These C data structures for building a module
 *  are complex and need some expertise to handle. However the meta module 
 *  interface allows to write support for modules which might be written even
 *  in scripting languages. Such filters should then be loadable as normal
 *  modules and are, depending on the choosen language, very easy to understand
 *  and to write.
 *  Beside reduced access to Oyranos native C data types, script filters might
 *  become highly interchangeable outside of Oyranos.
 *
 *  \b Module \b APIs:
 *  Several interfaces allow to write different module types. They can have
 *  access to Oyranos' configuration system, build data dependent contexts,
 *  provide access to user defined data types, create custom UIs via XFORMS and
 *  possibly SVG, simply process data and connect as node into a Oyranos DAG or
 *  expose to users as policy tool for a DAG. Different module APIs expose as
 *  different user APIs.
 *  Most module authors will want to write for one of these interfaces.
 *
 *  \b High \b Abstraction: Most module interfaces have no idea themselve about
 *  what kind of data they handle. They follow very generic and abstract ideas
 *  and rules on how to do data processing in a directed acyclic graph. 
 *  The overal idea of Oyranos' graphs can be read in the 
 *  @ref objects_conversion. 
 *  The module interfaces can implement different processing stages and tell 
 *  how to combine them in a graph by Oyranos. E.g. it is possible for on module
 *  to build a cacheable context, which can be used by different modules to
 *  process data.
 *  Most of the processing logic is inside Oyranos's core. But for efficiency
 *  and flexibility modules have access to their connected neighbour plug-ins.
 *  For instance they have to call their forerunner to request for data.
 *
 *  \b Examples: For learning how modules can
 *  do useful work see the delivered modules like the lcms and oyIM ones in
 *  files like "oyranos_cmm_xxxx.c". They are linked as libraries and are
 *  installed in the "$cmmdir" and "$libdir/oyranos" paths. These paths are
 *  shown during the configuration process or through te provided oyranos-config
 *  tool.
 *
 *  The Filter API's are subdivided to allow for automatical combining of 
 *  preprocessing and processing stages. Especially in the case of expensive
 *  preprocessing data, like in CMM's, it makes sense to provide the means for
 *  combining general purpose libraries with hardware accelerated modules.
 *  This architecture allowes for combining by just providing enough interface
 *  information about their supported data formats.
 *  The following paragraphs provide a overview.
 *
 *  The oyCMMapi5_s module structure defines a meta module to load modules,
 *  from a to be defined directory with to be defined naming criterias. The 
 *  meta module loads or constructs all parts of a module, oyCMMapi4_s,
 *  oyCMMapi7_s, oyCMMapi6_s, oyCMMapi8_s and oyCMMapi9_s.
 *
 *  oyCMMapi7_s eighter deploys the context created in a oyCMMapi4_s filter, or
 *  simply processes the data of a oyFilterNode_s graph element. It is 
 *  responsible to request data from the graph and process them.
 *  Members are responsible to describe the filters capabilities for connecting
 *  to other filters in the graph. Modules can describe their own UI in 
 *  oyCMMapi4_s.
 *  oyCMMapi7_s is mandatory.
 *
 *  The oyCMMapi4_s is a structure to create a context for a oyCMMapi7_s
 *  processor. This context is a intermediate processing stage for all of the
 *  context data influencing options and input datas. The idea for tight 
 *  integration of the context functionality is to provide a well defined way
 *  of interaction for node modules with context modules. 
 *  The oyCMMapi4_s structure contains as well the GUI. oyCMMapi4_s is 
 *  mandatory because of its GUI parts.
 *  A oyCMMapi4_s without a oyCMMapi7_s is useless.
 *  oyCMMapi4_s must contain the same basic registration string like the 
 *  according oyCMMapi7_s except some keywords in the application section. This
 *  is explained more below in detail.
 *  It is assumed that a generated context is worth to be cached. If Oyranos 
 *  obtains a serialised data blob from the context generator it can be
 *  automatically cached.
 *  @dot
digraph Anatomy_A {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=14];
  node [shape=record,fontname=Helvetica, fontsize=11, width=.1];

  subgraph cluster_7 {
    label="One Context - Different Data Processors";
    color=white;
    clusterrank=global;

      node [width = 2.5, style=filled];

      api4_A [label="lcms ICC colour profile Module\n oyCMMapi4_s | <f>Context Creation \"oyDL\" | ... | XFORMS UI"];

      api6_A [label="lcms Context Converter\n oyCMMapi6_s\n \"oyDL\"-\>\"lcCC\" \(oyDL_lcCC\)"];
      api7_A [label="lcms colour conversion Data Processor\n oyCMMapi7_s\n C implementation needs \"lcCC\" context" width = 3.5];

      api6_C [label="ctl Context Converter\n oyCMMapi6_s\n \"oyDL\"-\>\"oCTL\" \(oyDL_oCTL\)"];
      api7_C [label="ctl colour conversion Data Processor\n oyCMMapi7_s\n LLVM implementation needs \"oCTL\" context" width = 3.5];

      api4_A; api6_A; api7_A;
      api7_C; api6_C;

      api4_A:f -> api6_A -> api7_A [arrowhead="open", color=black];
      api4_A:f -> api6_C -> api7_C [arrowhead="open", color=black];
  }
} @enddot
 *
 *  In case a oyCMMapi7_s function can not handle a certain provided context
 *  data format, Oyranos will try to convert it for the oyCMMapi7_s API through
 *  a fitting oyCMMapi6_s data convertor. oyCMMapi6_s is only required for 
 *  filters, which request incompatible contexts from a oyCMMapi4_s structure.
 *
 *  The oyCMMapi8_s handles configurations, like external module data
 *  and options. The @ref devices_handling deployes these modules.
 *
 *  oyCMMapi9_s can be used to plug in new object types, policy settings and
 *  a way to enforce the policies.
 *
 *  @section registration Registration
 *  Each filter API provides a \b registration member string.
 *  The registration member provides the means to later successfully select 
 *  the according filter. The string is separated into sections by a slash'/'.
 *  The sections can be subdivided by point'.' for additional attributes as 
 *  needed. This pattern follows the scheme of directories with attributes or
 *  XML elements with attributes.
 *  The sections are to be filled as follows:
 *  - top, e.g. "shared" (::oyFILTER_REG_TOP)
 *  - vendor, e.g. "oyranos.org" (::oyFILTER_REG_DOMAIN)
 *  - filter type, e.g. "imaging"
 *    (::oyFILTER_REG_TYPE)
 *  - filter name, e.g. "icc.lcms._NOACCEL._CPU" (::oyFILTER_REG_APPLICATION)
 *
 *  After that the options section follows (::oyFILTER_REG_OPTION).
 *
 *  The application registration string part should for general purpose modules
 *  contain a convention string. "icc" signals to process colours with the help
 *  of ICC style profiles, which can by convention be inserted into the options
 *  list.
 *
 *  @par Filter registration:
 *  A filter can add keywords but must omit the API number and the following
 *  matching rule sign. Recommended keywords for the application section are:
 *  - _ACCEL for acceleration, required
 *  - _NOACCEL for no acceleration or plain software, required
 *  - _GPU, _GLSL, _HLSL, _MMX, _SSE, _SSE2, _3DNow and so on for certain 
 *    hardware acceleration features \n
 *  
 *  \b Example: a complete module registration: \n
 *  "shared/oyranos.org/imaging/icc.lcms._NOACCEL._CPU" registers a plain
 *  software CMM
 * 
 *  A underscore in front of a attribute makes the attribute optional during
 *  the matching process in oyFilterRegistrationMatch(). This is needed in case
 *  a registration string is used itself as a search pattern.
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
 *  By default all attributes in a search pattern are considered mandatory.
 *  A level can be omitted, "//", or a attribute can be tagged by a '_'
 *  underscore in front for making it optional, or a '-' to indicate a 
 *  attibute must not match.
 *
 *  \b Example: a complete registration search pattern: \n
 *  "//imaging/4+icc.7+ACCEL.7_GPU.7_HLSL.7-GLSL" selects a accelerated CMM 
 *  interpolator with prefered GPU and HLSL but no GLSL support together with a
 *  ICC compliant context generator and options.
 *
 *  The oyFilterRegistrationToText() and oyFilterRegistrationMatch() functions
 *  might be useful for canonical processing Oyranos registration text strings.
 *  Many functions allow for passing a registration string. Matching can be 
 *  obtained by omitting sections like in the string "//imaging/icc", where the
 *  elements between slashes is o,itted. This string would result in a match 
 *  for any ICC compliant colour conversion filter.
 *
 *  The registration attributes ".front", ".advanced" and more are described in
 *  the objects_value::oyOPTIONATTRIBUTE_e enum.
 *
 *  See as well <a href="http://www.oyranos.org/wiki/index.php?title=Concepts#Elektra_namespace">Concepts#Elektra_namespace</a> on ColourWiki.
 *  @{
 */


/** Function oyPointerLookUpFromObject
 *  @brief   get a module specific pointer from cache
 *  @memberof oyPointer_s
 *
 *  The returned oyPointer_s has to be released after using by the module with
 *  oyPointer_Release().
 *  In case the the oyPointer_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see oyPointerLookUpFromText()
 *
 *  @param[in]     data                 object to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyPointer_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyPointer_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyPointer_s  * oyPointerLookUpFromObject (
                                       oyStruct_s        * data,
                                       const char        * data_type )
{
  oyStruct_s * s = data;
  int error = !s;
  oyPointer_s * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    const char * tmp = 0;
    tmp = oyObject_GetName( s->oy_, oyNAME_NICK );
    cmm_ptr = oyPointerLookUpFromText( tmp, data_type );
  }

  return cmm_ptr;
}

/** Function oyPointerLookUpFromText
 *  @brief   get a module specific pointer from cache
 *  @memberof oyPointer_s
 *
 *  The returned oyPointer_s has to be released after using by the module with
 *  oyPointer_Release().
 *  In case the the oyPointer_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see e.g. lcmsCMMData_Open()
 *
 *  @param[in]     text                 hash text to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyPointer_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyPointer_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/05 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyPointer_s * oyPointerLookUpFromText( const char        * text,
                                       const char        * data_type )
{
  int error = !text;
  oyPointer_s * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    /*oyPointer_s *cmm_ptr = 0;*/
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
    STRING_ADD( hash_text, data_type );
    STRING_ADD( hash_text, ":" );
    tmp = text;
    STRING_ADD( hash_text, tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );

    if(error <= 0)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyPointer_s*) oyHash_GetPointer( entry,
                                                  oyOBJECT_POINTER_S);

      if(!cmm_ptr)
      {
        cmm_ptr = oyPointer_New( 0 );
        error = !cmm_ptr;

        if(error <= 0)
          error = oyPointer_Set( cmm_ptr, 0,
                                 data_type, 0, 0, 0 );

        error = !cmm_ptr;

        if(error <= 0 && cmm_ptr)
          /* 3b.1. update cache entry */
          error = oyHash_SetPointer( entry,
                                     (oyStruct_s*) cmm_ptr );
      }
    }

    oyHash_Release( &entry );
  }

  return cmm_ptr;
}

/** @} *//* module_api */



/** \addtogroup misc Miscellaneous

 *  @{
 */


/** \addtogroup objects_generic

 *  @{
 */


/** @internal
 *  @brief get always a Oyranos cache entry from a cache list
 *
 *  @param[in]     cache_list          the list to search in
 *  @param[in]     flags               0 - assume text, 1 - assume sized hash
 *  @param[in]     hash_text           the text to search for in the cache_list
 *  @return                            the cache entry may not have a entry
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/24 (Oyranos: 0.1.8)
 *  @date    2009/06/04
 */
oyHash_s *   oyCacheListGetEntry_    ( oyStructList_s    * cache_list,
                                       uint32_t            flags,
                                       const char        * hash_text )
{
  oyHash_s * entry = 0,
           * search_key = 0;
  int error = !(cache_list && hash_text);
  int n = 0, i;
  uint32_t search_int[8] = {0,0,0,0,0,0,0,0};
  char hash_text_copy[32];
  const char * search_ptr = (const char*)search_int;

  if(error <= 0 && cache_list->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  {
    if(flags & 0x01)
      search_ptr = hash_text;
    else
    if(oyStrlen_(hash_text) < OY_HASH_SIZE*2-1)
    {
      memset( hash_text_copy, 0, OY_HASH_SIZE*2 );
      memcpy( hash_text_copy, hash_text, oyStrlen_(hash_text) );
      search_ptr = hash_text_copy;
    } else
      search_int[0] = oyMiscBlobGetL3_( (void*)hash_text, oyStrlen_(hash_text));
  }

  if(error <= 0)
    n = oyStructList_Count(cache_list);

  for(i = 0; i < n; ++i)
  {
    oyHash_s * compare = (oyHash_s*) oyStructList_GetType_( cache_list, i,
                                                         oyOBJECT_HASH_S );

    if(compare )
    if(memcmp(search_ptr, compare->oy_->hash_ptr_, OY_HASH_SIZE*2) == 0)
    {
      entry = compare;
      return oyHash_Copy( entry, 0 );
    }
  }

  if(error <= 0 && !entry)
  {
    search_key = oyHash_Get(hash_text, 0);
    error = !search_key;

    if(error <= 0)
      entry = oyHash_Copy( search_key, 0 );

    if(error <= 0)
    {
      error = oyStructList_MoveIn(cache_list, (oyStruct_s**)&search_key, -1, 0);
      search_key = 0;
    }

    oyHash_Release( &search_key );
  }


  if(entry)
    return oyHash_Copy( entry, 0 );
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
 *    according description string plus the oyPointer_s struct. For simplicity
 *    the cache struct can be identical to the oyPointer_s, with the disadvantage
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

  return oyCacheListGetEntry_(oy_cmm_cache_, 0, hash_text);
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




/** Function oyRankMapCopy
 *  @memberof oyConfig_s
 *  @brief   copy a rank map
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/01/27
 */
oyRankPad *        oyRankMapCopy     ( const oyRankPad   * rank_map,
                                       oyAlloc_f           allocateFunc )
{
  oyRankPad * map = 0;
  int error = !rank_map;
  int n = 0, i;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    while( rank_map[n++].key ) {}

    oyAllocHelper_m_( map, oyRankPad, n + 1, allocateFunc, error = 1 );
  }

  if(error <= 0)
  {
    for(i = 0; i < n; ++i)
    {
      map[i].key = oyStringCopy_( rank_map[i].key, allocateFunc );
      map[i].match_value = rank_map[i].match_value;
      map[i].none_match_value = rank_map[i].none_match_value;
      map[i].not_found_value = rank_map[i].not_found_value;
    }
  }

  return map;
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
                   oyConfig_New      ( const char        * registration,
                                       oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIG_S;
# define STRUCT_TYPE oyConfig_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(registration)
    s->registration = oyStringCopy_( registration, s->oy_->allocateFunc_ );

  s->db = oyOptions_New( 0 );
  s->backend_core = oyOptions_New( 0 );
  s->data = oyOptions_New( 0 );

  return s;
}

/** Function oyConfig_GetDB
 *  @brief   search a configuration in the DB for a configuration from module
 *  @memberof oyConfig_s
 *
 *  @param[in]     device              the to be checked configuration from
 *                                     oyConfigs_FromPattern_f
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/01/26
 */
OYAPI int  OYEXPORT
               oyConfig_GetDB        ( oyConfig_s        * device,
                                       int32_t           * rank_value )
{
  int error = !device;
  int rank = 0, max_rank = 0, i, n;
  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0, * max_config = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    error = oyConfigs_FromDB( device->registration, &configs, 0 );

    n = oyConfigs_Count( configs );

    for( i = 0; i < n; ++i )
    {
      config = oyConfigs_Get( configs, i );

      error = oyConfig_Compare( device, config, &rank );
      DBG_PROG1_S("rank: %d\n", rank);
      if(max_rank < rank)
      {
        max_rank = rank;
        oyConfig_Release( &max_config );
        max_config = oyConfig_Copy( config, 0 );
      }

      oyConfig_Release( &config );
    }
  }

  if(error <= 0 && rank_value)
    *rank_value = max_rank;

  if(error <= 0 && max_config)
  {
    oyOptions_Release( &device->db );
    device->db = oyOptions_Copy( max_config->db, 0 );
    oyConfig_Release( &max_config );
  }

  return error;
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

  s = oyConfig_New( obj->registration, object );
  error = !s;

  if(error <= 0)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    s->db = oyOptions_Copy( obj->db, s->oy_ );
    s->backend_core = oyOptions_Copy( obj->backend_core, s->oy_ );
    s->data = oyOptions_Copy( obj->data, s->oy_ );
    error = !memcpy( s->version, obj->version, 3*sizeof(int) );

    s->rank_map = oyRankMapCopy( obj->rank_map, allocateFunc_ );
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
  int i = 0;
  /* ---- start of common object destructor ----- */
  oyConfig_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyOptions_Release( &s->db );
  oyOptions_Release( &s->backend_core );
  oyOptions_Release( &s->data );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->rank_map)
    {
      while(s->rank_map[i].key)
        deallocateFunc( s->rank_map[i++].key );
      deallocateFunc( s->rank_map );
    }

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyConfig_AddDBData
 *  @memberof oyConfig_s
 *  @brief   add a key value pair to a oyConfig_s::db
 *
 *  This functions handles canonical user side settings. The keys added with
 *  this function can later be stored in the DB. A call to oyConfig_GetDB() or
 *  oyConfig_ClearDBData() overwrite the added entries. \n
 *  Modules should add informations to oyConfig_s::data.
 *
 *  @param[in]     config              the configuration
 *  @param[in]     key                 a key name, e.g. "my_key"
 *  @param[in]     value               a value, e.g. "my_value"
 *  @param[in]     flags               see oyOptions_s::oyOptions_SetFromText(.., flags,..)
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI int  OYEXPORT
               oyConfig_AddDBData    ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags )
{
  int error = !config || !key;
  char * tmp = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    STRING_ADD( tmp, config->registration );
    if(tmp[oyStrlen_(tmp)-1] != OY_SLASH_C)
      STRING_ADD( tmp, OY_SLASH );

    if(oyStrstr_( key, config->registration ) != 0)
    {
      oyFree_m_(tmp);
      STRING_ADD( tmp, key );
    }
    else if(oyStrrchr_( key, OY_SLASH_C ) != 0)
      STRING_ADD( tmp, oyStrrchr_( key, OY_SLASH_C )+1 );
    else
      STRING_ADD( tmp, key );

    /** We provide basically a wrapper for oyOptions_SetFromText(). */
    error = oyOptions_SetFromText( &config->db, tmp, value, flags );

    oyFree_m_( tmp );
  }

  return error;
}

/** Function oyConfig_ClearDBCache
 *  @memberof oyConfig_s
 *  @brief   remove all additional data from the oyConfig_s::db object cache
 *
 *  Clear the local DB cache. The function will be called as well from
 *  oyConfig_GetDB().
 *
 *  @param[in]     config              the configuration
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI int  OYEXPORT
               oyConfig_ClearDBData    ( oyConfig_s        * config )
{
  int error = !config;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    error = oyOptions_Release( &config->db );
    config->db = oyOptions_New( config->oy_ );
  }

  return error;
}

/** Function oyConfig_SaveToDB
 *  @memberof oyConfig_s
 *  @brief   store a oyConfig_s in DB
 *
 *  The new key set name is stored inside the key "key_set_name".
 *
 *  @param[in]     config              the configuration
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2011/01/29
 */
OYAPI int  OYEXPORT
               oyConfig_SaveToDB     ( oyConfig_s        * config )
{
  int error = !config;
  oyOptions_s * opts = 0;
  oyConfig_s * s = config;
  char * new_reg = 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  DBG_PROG_START

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );
    oyOptions_AppendOpts( opts, config->db );
    oyOptions_AppendOpts( opts, config->backend_core );

    error = oyOptions_SaveToDB( opts, config->registration, &new_reg, 0 );

    /* add information about the data's origin */
    oyConfig_AddDBData( config, "key_set_name", new_reg, OY_CREATE_NEW );

    oyFree_m_( new_reg );
    oyOptions_Release( &opts );
  }

  DBG_PROG_ENDE
  return error;
}

/** Function oyRegistrationEraseFromDB
 *  @memberof oyConfig_s
 *  @brief   remove a registration config from DB
 *
 *  The configs registration needs to be a config group, one with 4 slashes.
 *  The last level is typical a number.
 *
 *  @param[in]     config_registration the configuration
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2009/01/27 (Oyranos: 0.3.0)
 *  @date    2011/01/29
 */
int          oyRegistrationEraseFromDB(const char        * registration )
{
  int error = !registration;
  int i;
  const char * text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    i = 0;
    text = registration;
    if(text)
      while( (text = oyStrchr_(++text, OY_SLASH_C)) != 0)
        ++i;

    if(i == 4)
      text = registration;

    error = oyEraseKey_( text );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Function oyConfig_EraseFromDB
 *  @memberof oyConfig_s
 *  @brief   remove a oyConfig_s from DB
 *
 *  @param[in]     config              the configuration
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/01/27
 */
OYAPI int  OYEXPORT
               oyConfig_EraseFromDB  ( oyConfig_s        * config )
{
  int error = !config;
  oyOption_s * o = 0;
  int i;
  const char * text = 0;
  char * t = 0,
       * tmp = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    i = 0;
    text = config->registration;
    if(text)
      while( (text = oyStrchr_(++text, OY_SLASH_C)) != 0)
        ++i;

    if(i != 4)
    {
      o = oyOptions_Get( config->db, 0 );
      i = 0;
      text = 0;
      if(o)
        text = oyOption_GetRegistration( o );
      if(text)
        while( (text = oyStrchr_(++text, OY_SLASH_C)) != 0)
          ++i;

      /* A key has one slash more. Cut the last slash off.  */
      if(i == 5)
      {
        tmp = oyStringCopy_( oyOption_GetRegistration( o ), oyAllocateFunc_ );
        t = oyStrrchr_(tmp, OY_SLASH_C);
        t[0] = 0;
        text = tmp;
      }

    }
    else
      text = config->registration;

    error = oyEraseKey_( text );

    if(tmp)
      oyFree_m_( tmp );
    oyOption_Release( &o );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Function oyConfig_Compare
 *  @brief   check for matching to a given pattern
 *  @memberof oyConfig_s
 *
 *  @param[in]     module_device       the to be checked configuration from
 *                                     oyConfigs_FromPattern_f;
 *                                     Additional allowed are DB configs.
 *  @param[in]     db_pattern          the to be compared configuration from
 *                                     elsewhere, e.g. ICC dict tag
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.1.13
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2010/11/21
 */
int            oyConfig_Compare      ( oyConfig_s        * module_device,
                                       oyConfig_s        * db_pattern,
                                       int32_t           * rank_value )
{
  int error = !module_device || !db_pattern;
  int domain_n, pattern_n, i, j, k, l,
      rank = 0,
      d_rank = 0,
      has_opt;
  oyOption_s * d = 0,
             * p = 0,
             * check = 0;
  oyOptions_s * dopts = 0; /* device options */
  char * d_opt = 0, * d_val = 0, /* device variables */
       * p_opt = 0, * p_val = 0, /* pattern variables */
       * check_opt = 0, * check_val = 0;
  oyConfig_s * pattern = db_pattern,
             * device = module_device;
  oyRankPad  * rank_map = 0;
  oyConfig_s * s = module_device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    if(device->rank_map)
      rank_map = device->rank_map;
    else
      rank_map = pattern->rank_map;

    domain_n = oyOptions_Count( device->backend_core );
    if(domain_n)
      dopts = device->backend_core;
    else
    {
      domain_n = oyOptions_Count( device->db );
      if(domain_n)
        /* fall back for pure DB contructed oyConfig_s */
        dopts = device->db;
#ifdef DEBUG
      else if(oy_debug > 2)
        WARNc1_S("No key/values pairs found in %s", device->registration);
#endif
    }

    pattern_n = oyOptions_Count( pattern->db );
    for(i = 0; i < domain_n; ++i)
    {
      d = oyOptions_Get( dopts, i );
      d_opt = oyFilterRegistrationToText( oyOption_GetRegistration(d),
                                          oyFILTER_REG_MAX, 0);
      d_val = oyOption_GetValueText( d, oyAllocateFunc_ );
      has_opt = 0;

      /* check for double occurences */
      for(l = 0; l < i; ++l)
      {
        check = oyOptions_Get( dopts, l );
        check_opt = oyFilterRegistrationToText( oyOption_GetRegistration(check),
                                                oyFILTER_REG_MAX, 0);
        if(oyStrcmp_(d_opt, check_opt) == 0)
        {
          check_val = oyOption_GetValueText( check, oyAllocateFunc_ );
          WARNc4_S( "%d %s occured twice with: %s %s", i, d_opt, check_val,
                    check_val ? check_val : "" );
          if(check_val) oyFree_m_( check_val );
        }

        oyOption_Release( &check );
        oyFree_m_( check_opt );
      }

      d_rank = oyConfig_DomainRank( device );
      if(d_rank > 0 && d_val && d_opt)
      for( j = 0; j < pattern_n; ++j )
      {
        p = oyOptions_Get( pattern->db, j );

        p_opt = oyFilterRegistrationToText( oyOption_GetRegistration(p),
                                            oyFILTER_REG_MAX,
                                            0 );

        if(p_opt && oyStrstr_(p_opt, d_opt))
        {
          p_val = oyOption_GetValueText( p, oyAllocateFunc_ );
          has_opt = 1;

          /** Option name is equal and and value matches : increase rank value
           *  
           *        we need a comparision mechanism here. The pattern value
           *        should be expandable to several values.
           *        Do we need more than the ICC dict style syntax here?
           */
          if(p_val && oyTextIccDictMatch( d_val, p_val ))
          {
            if(rank_map)
            {
              k = 0;
              while(rank_map[k].key)
              {
                if(oyStrcmp_(rank_map[k].key, d_opt) == 0)
                {
                  rank += rank_map[k].match_value;
                  break;
                }
                ++k;
              }
            } else
              ++rank;

            oyFree_m_(p_val);
          } else if(rank_map)
          {
            k = 0;
            while(rank_map[k].key)
            {
              if(oyStrcmp_(rank_map[k].key, d_opt) == 0)
              {
                rank += rank_map[k].none_match_value;
                break;
              }
              ++k;
            }
          }
          break;
        }
        /*
        rank += oyFilterRegistrationMatch( d->registration, p->registration,
                                           oyOBJECT_CMM_API8_S); */

        oyOption_Release( &p );
        if(p_opt) oyFree_m_( p_opt );
        if(p_val) oyFree_m_( p_val );
      }

      if(!has_opt && rank_map)
      {
        k = 0;
          while(rank_map[k].key)
          {
            if(oyStrcmp_(rank_map[k].key, d_opt) == 0)
            {
              rank += rank_map[k].not_found_value;
              break;
            }
            ++k;
          }
      }

      oyOption_Release( &d );
      oyFree_m_( d_opt );
      oyFree_m_( d_val );
    }
  }

  if(rank_value)
    *rank_value = rank;

  return error;
}

/** Function oyConfig_DomainRank
 *  @brief   check for being recognised by a given module
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @return                            0 - indifferent, <= -1 - no fit
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/01/26
 */
OYAPI int  OYEXPORT
               oyConfig_DomainRank   ( oyConfig_s        * config )
{
  oyCMMapiFilters_s * apis = 0;
  int error = !config;
  int i = 0,
      rank = 0;
  uint32_t * rank_list = 0,
           max_rank = 0;
  uint32_t apis_n = 0;
  oyCMMapi8_s * cmm_api8 = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    apis = oyCMMsGetFilterApis_( 0,0, config->registration,
                                 oyOBJECT_CMM_API8_S, 
                                 oyFILTER_REG_MODE_NONE,
                                 &rank_list, &apis_n);
    error = !apis;
  }

  if(error <= 0)
  {
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api8 = (oyCMMapi8_s*) oyCMMapiFilters_Get( apis, i );

      error = !cmm_api8->oyConfig_Rank;
      if(error <= 0)
      /** Ask the module if it wants later on to accept this configuration. */
        rank = cmm_api8->oyConfig_Rank( config ) * rank_list[i];

      if(max_rank < rank)
        max_rank = rank;

      if(cmm_api8->release)
        cmm_api8->release( (oyStruct_s**)&cmm_api8 );
    }
  }

  oyCMMapiFilters_Release( &apis );

  if(error && max_rank >= 0)
    rank = -1;
  else
    rank = max_rank;

  oyExportEnd_();
  return rank;
}

/** Function oyConfig_FindString
 *  @brief   search in data sets for a key/value
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @param[in]     key                 the key name
 *  @param[in]     value               the optional value
 *  @return                            the found value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI const char * OYEXPORT
               oyConfig_FindString   ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value )
{
  const char * text = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )


  text = oyOptions_FindString( config->data, key, value );
  if(!text)
    text = oyOptions_FindString( config->backend_core, key, value );
  if(!text)
    text = oyOptions_FindString( config->db, key, value );

  return text;
}

/** Function oyConfig_Find
 *  @brief   search in data sets for a key
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @param[in]     key                 the key name
 *  @return                            the found value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI oyOption_s * OYEXPORT
               oyConfig_Find         ( oyConfig_s        * config,
                                       const char        * key )
{
  oyOption_s * o = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  o = oyOptions_Find( config->data, key );
  if(!o)
    o = oyOptions_Find( config->backend_core, key );
  if(!o)
    o = oyOptions_Find( config->db, key );

  return o;
}

/** Function oyConfig_Has
 *  @brief   search in data sets for a key
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @param[in]     key                 the key name
 *  @return                            0 - not found; 1 - key found
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/10 (Oyranos: 0.1.10)
 *  @date    2009/12/10
 */
OYAPI int  OYEXPORT
               oyConfig_Has          ( oyConfig_s        * config,
                                       const char        * key )
{
  oyOption_s * o = 0;
  int has_option = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  o = oyOptions_Find( config->data, key );
  if(!o)
    o = oyOptions_Find( config->backend_core, key );
  if(!o)
    o = oyOptions_Find( config->db, key );

  if(o)
    has_option = 1;

  oyOption_Release( &o );

  return has_option;
}

/** Function oyConfig_Count
 *  @brief   number of all options
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration
 *  @return                            the options count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI int  OYEXPORT
               oyConfig_Count        ( oyConfig_s        * config )
{
  int error = !config;
  int n = 0;
  oyOptions_s * opts = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );

    oyOptions_AppendOpts( opts, config->db );
    oyOptions_AppendOpts( opts, config->backend_core );
    oyOptions_AppendOpts( opts, config->data );
    n = oyOptions_Count( opts );
    oyOptions_Release( &opts );
  }

  return n;
}

/** Function oyConfig_Get
 *  @brief   get one option
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration
 *  @param[in]     pos                 option position
 *  @return                            the selected option
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI oyOption_s * OYEXPORT
               oyConfig_Get          ( oyConfig_s        * config,
                                       int                 pos )
{
  int error = !config;
  oyOption_s * o = 0;
  oyOptions_s * opts = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );

    oyOptions_AppendOpts( opts, config->db );
    oyOptions_AppendOpts( opts, config->backend_core );
    oyOptions_AppendOpts( opts, config->data );

    o = oyOptions_Get( opts, pos );

    oyOptions_Release( &opts );
  }

  return o;
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/** Function oyConfigs_FromDomain
 *  @memberof oyConfigs_s
 *  @brief   send a request to a configuration module
 *
 *  The convention an empty options argument should be send an Warning message
 *  containing intructions on how to talk with the module as a fallback for
 *  programmers. Otherwise the calls are pure convention and depend on the usage
 *  and agreement of the partners.
 *
 *  For the convention to call to colour devices
 *  @see oyX1Configs_FromPatternUsage().
 *
 *  @param[in]     registration_domain                     the module to call to
 *  @param[in]     options                                 options to specify the calling into modules
 *                                                         messages are bound to this object
 *  @param[out]    configs                                 the returned configurations
 *  @param[in]     object                                  a optional user object
 *  @return                                                0 - good, 1 <= error, -1 >= issues,
 *                                                         look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
               oyConfigs_FromDomain  ( const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object )
{
  oyConfigs_s * s = 0;
  oyConfig_s * config = 0;
  int error = !registration_domain,
      l_error = 0;
  oyCMMapi8_s * cmm_api8 = 0;
  int i, n;

  oyExportStart_(EXPORT_CHECK_NO);

  /**
   *  1. first we search for oyCMMapi8_s complex config support matching to our
   *     registration_domain
   *  2. if we find a module, we ask for the options
   *  3. add the options to the config (in the module)
   */
  if(error <= 0)
  {
    cmm_api8 = (oyCMMapi8_s*) oyCMMsGetFilterApi_( 0, registration_domain,
                                                   oyOBJECT_CMM_API8_S );
    error = !cmm_api8;
  }

  if(error <= 0)
    error = !cmm_api8->oyConfigs_FromPattern;

  if(error <= 0)
    error = cmm_api8->oyConfigs_FromPattern( registration_domain, options, &s );

  if(error <= 0)
  {
    n = oyConfigs_Count( s );
    for(i = 0; i < n && error <= 0; ++i)
    {
      config = oyConfigs_Get( s, i );

      l_error = oyOptions_SetSource( config->backend_core,
                                     oyOPTIONSOURCE_FILTER); OY_ERR
      l_error = oyOptions_SetSource( config->data,
                                     oyOPTIONSOURCE_FILTER ); OY_ERR

      oyConfig_Release( &config );
    }
  }

  if(error <= 0 && configs)
  {
    *configs = s;
    s = 0;
  } else
    oyConfigs_Release( &s );

  oyExportEnd_();
  return error;
}

/** Function oyConfigs_FromDeviceClass
 *  @brief   ask a module for device informations or other direct calls
 *  @memberof oyConfigs_s
 *
 *  @param[in]     device_type     the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class    the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     options             options to pass to the module, for zero
 *                                     the usage instructions are requested,
 *                                     a option "device_name" can be used 
 *                                     as filter
 *  @param[out]    devices         the devices
 *  @param[in]     object              the optional object
 *  @return                            0 - good, >= 1 - error
 *
 *  @verbatim
    // pass empty options to the module to get a usage message
    oyOptions_s * options = 0;
    int error = oyConfigs_FromDeviceClass( OY_TYPE_STD, "monitor",
                                              options, 0, 0 );
    @endverbatim
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI int  OYEXPORT
             oyConfigs_FromDeviceClass (
                                       const char        * device_type,
                                       const char        * device_class,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** devices,
                                       oyObject_s          object )
{
  int error = !device_class || !device_class[0];
  oyConfig_s * device = 0;
  oyConfigs_s * configs = 0;
  int i, j, j_n;
  uint32_t count = 0,
         * rank_list = 0;
  char ** texts = 0,
        * device_class_registration = 0;
  const char * tmp = 0,
             * device_name = 0;

  if(error > 0)
  {
    WARNc_S( "\n  No device_class argument provided. Give up" );
    return 0;
  }

  /** 1. obtain detailed and expensive device informations */
  if(options)
  {
    options = oyOptions_Copy( options, 0 );
    device_name = oyOptions_FindString( options, "device_name", 0 );
  }

  /** 1.2.1 build a device class registration string */
  if(error <= 0)
  {
    device_class_registration = oyDeviceRegistrationCreate_(
                                          device_type, device_class,
                                          device_name,
                                          device_class_registration );
    error = !device_class_registration;
  }

  /** 1.2.2 get all device class module names */
  if(error <= 0)
    error = oyConfigDomainList  ( device_class_registration, &texts, &count,
                                  &rank_list, 0 );

  if(devices && !*devices)
    *devices = oyConfigs_New( object );

  /** 1.3 ask each module */
  for( i = 0; i < count; ++i )
  {
    const char * registration_domain = texts[i];

    /** 1.3.1 call into module */
    error = oyConfigs_FromDomain( registration_domain, options, &configs,
                                  object);

    if(devices && *devices)
      j_n = oyConfigs_Count( configs );
    else
      j_n = 0;
    for( j = 0; j < j_n; ++j )
    {
      device = oyConfigs_Get( configs, j );

      if(device_name)
      {
        /** 1.3.1.1 Compare the device_name with the device_name option
         *          and collect the matching devices. */
        tmp = oyConfig_FindString( device, "device_name", 0 );
        if(tmp && oyStrcmp_( tmp, device_name ) == 0)
          oyConfigs_MoveIn( *devices, &device, -1 );
      } else
        /** 1.3.1.2 ... or collect all device configurations */
        oyConfigs_MoveIn( *devices, &device, -1 );

      oyConfig_Release( &device );
    }

    oyConfigs_Release( &configs );
  }

  if(devices)
    j_n = oyConfigs_Count( *devices );
  else
    j_n = 0;

  for( j = 0; j < j_n; ++j )
  {
    device = oyConfigs_Get( *devices, j );

    /** The basic call on how to obtain the configuration is added here as
     *  the objects name. "properties" and "list" are known. */
    if(oyOptions_FindString( options, "command", "properties" ) ||
       oyOptions_FindString( options, "oyNAME_DESCRIPTION", 0 ))
      oyObject_SetName( device->oy_, "properties", oyNAME_NAME );
    else if(oyOptions_FindString( options, "list", 0 ))
      oyObject_SetName( device->oy_, "list", oyNAME_NAME );

    oyConfig_Release( &device );
  }

  oyOptions_Release( &options );

  return error;
}


/** Function oyConfigs_Modify
 *  @brief   ask a module for device informations or other direct calls
 *  @memberof oyConfigs_s
 *
 *
 *  @param[in,out] configs             The passed configs first member is used
 *                                     to obtain a registration string and
 *                                     select a appropriate module.
 *                                     Regarding the module the
 *                                     configs need to be homogenous.
 *                                     All configs are passed at once to the
 *                                     module. Mixing configs from different
 *                                     modules is not defined.
 *  @param[in]     options             options to pass to the module; With zero
 *                                     the usage instructions are requested.
 *  @return                            0 - good, >= 1 - error, issue <= -1 
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/21 (Oyranos: 0.1.10)
 *  @date    2009/08/25
 */
OYAPI int  OYEXPORT
                 oyConfigs_Modify    ( oyConfigs_s       * configs,
                                       oyOptions_s       * options )
{
  int error = !oyConfigs_Count( configs );
  oyConfig_s * config = 0;
  oyConfigs_s * s = configs;
  int i;
  uint32_t count = 0,
         * rank_list = 0;
  char ** texts = 0,
        * registration_domain = 0;
  oyCMMapi8_s * cmm_api8 = 0;

  if(error > 0)
  {
    WARNc_S( "\n  No devices provided. Give up" );
    return 0;
  }
  oyCheckType__m( oyOBJECT_CONFIGS_S, return 1 )


  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    /** 1.  pick the first device to select a registration */
    config = oyConfigs_Get( configs, 0 );
    /** 1.2 get all device class module names from the firsts oyConfig_s
      *     registration */
    error = oyConfigDomainList  ( config->registration, &texts, &count,
                                  &rank_list, 0 );
    oyConfig_Release( &config );
  }


  /** 2. call each modules oyCMMapi8_s::oyConfigs_Modify */
  for( i = 0; i < count; ++i )
  {
    registration_domain = texts[i];

    if(error <= 0)
    {
      cmm_api8 = (oyCMMapi8_s*) oyCMMsGetFilterApi_( 0, registration_domain,
                                                     oyOBJECT_CMM_API8_S );
      error = !cmm_api8;
    }

    if(error <= 0)
      error = !cmm_api8->oyConfigs_Modify;

    if(error <= 0)
      error = cmm_api8->oyConfigs_Modify( configs, options );
  }

  oyStringListRelease_( &texts, count, oyDeAllocateFunc_ );
  if(rank_list)
    oyDeAllocateFunc_( rank_list );

  oyExportEnd_();
  return error;
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

  if(error <= 0)
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

  oyCheckType__m( oyOBJECT_CONFIGS_S, return 1 )

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
                 oyConfigs_MoveIn    ( oyConfigs_s       * list,
                                       oyConfig_s       ** obj,
                                       int                 pos )
{
  oyConfigs_s * s = list;
  int error = !s;

  if(!error)
    oyCheckType__m( oyOBJECT_CONFIGS_S, return 1 )

  if(obj && *obj && (*obj)->type_ == oyOBJECT_CONFIG_S)
  {
    if(!s)
    {
      s = oyConfigs_New(0);
      error = !s;
    }                                  

    if(error <= 0 && !s->list_)
    {
      s->list_ = oyStructList_New( 0 );
      error = !s->list_;
    }
      
    if(error <= 0)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos,
                                   OY_OBSERVE_AS_WELL );
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
                 oyConfigs_ReleaseAt ( oyConfigs_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyConfigs_s * s = list;

  if(s)
    oyCheckType__m( oyOBJECT_CONFIGS_S, return 1 )

  if(error <= 0)
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
                 oyConfigs_Get       ( oyConfigs_s       * list,
                                       int                 pos )
{       
  oyConfigs_s * s = list;

  if(s)
    oyCheckType__m( oyOBJECT_CONFIGS_S, return 0 )

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
  oyConfigs_s * s = list;

  if(!s)
    return 0;

  if(s)
    oyCheckType__m( oyOBJECT_CONFIGS_S, return 0 )

  return oyStructList_Count( list->list_ );
}

/** Function oyConfigs_SelectSimiliars
 *  @memberof oyConfigs_s
 *  @brief   filter similiar configs compared by a pattern
 *
 *  This is a simple convenience function to select from a list existing 
 *  configurations.
 *
 *  @verbatim
    const char * pattern[][2] = {{"device_name",0},
                                 {"manufacturer",0},
                                 {"model",0},
                                 {"serial",0},
                                 {0,0}};
    oyConfigs_s * devices = 0, * filtered = 0;
    int error = oyConfigs_FromDB( registration, &devices, 0 );
    error = oyConfigs_SelectSimiliars( devices, pattern, &filtered )
 *  @endverbatim
 *
 *  @param[in]     list                the list
 *  @param[in]     pattern             user supplied zero terminated pattern
 *                                     list; The first element is a mandadory
 *                                     key to meet. The second element is the 
 *                                     optional value. Each 
 *  @param[out]    filtered            the result
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/26 (Oyranos: 0.1.10)
 *  @date    2009/02/26
 */
OYAPI int  OYEXPORT
                 oyConfigs_SelectSimiliars (
                                       oyConfigs_s       * list,
                                       const char        * pattern[][2],
                                       oyConfigs_s      ** filtered )
{
  int error = !list;
  int i,j, matches = 0, n, required = 0;
  oyConfigs_s * s = list,
              * result = 0;
  oyConfig_s * device = 0;
  const char * ct = 0;

  if(!s)
    return 0;

  if(s)
    oyCheckType__m( oyOBJECT_CONFIGS_S, return 0 )

  /** 0. setup Elektra */
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  result = oyConfigs_New(0);
                 
  n = oyConfigs_Count( list );
  for(i = 0; i < n; ++i)
  {
    device = oyConfigs_Get( list, i );
    j = 0;
    matches = 0;
    required = 0;

    while(pattern[j][0])
    {
      ct = oyConfig_FindString( device, pattern[j][0], 0);
      if(pattern[j][1])
      {
        ++required;

        if(ct && strcmp(ct,pattern[j][1]) == 0)
          ++matches;

      } else
      {
        ++required;
        ++matches;
      }

      ++j;
    }

    if(required == matches)
      error = oyConfigs_MoveIn( result, &device, -1 );

    oyConfig_Release( &device );
  }

  *filtered = result;

  return error;
}

/** Function oyConfigs_FromDB
 *  @memberof oyConfigs_s
 *  @brief   get all oyConfigs_s from DB
 *
 *  @param[in]     registration        the filter
 *  @param[out]    configs             the found configuration list
 *  @param[in]     object              a optional user object
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/23 (Oyranos: 0.1.10)
 *  @date    2010/02/25
 */
OYAPI int OYEXPORT
                 oyConfigs_FromDB    ( const char        * registration,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object )
{
  oyConfigs_s * s = 0;
  oyConfig_s * config = 0;
  oyOption_s * o = 0;
  char ** texts = 0,
       ** key_set_names = 0,
       ** config_key_names = 0;
  uint32_t count = 0,
         * d_rank_list = 0;
  int error = !registration;
  int i, j, k, n = 0, k_n = 0;
  oyCMMapi8_s * cmm_api8 = 0;

  /** 0. setup Elektra */
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    /** 1. get all module names for the registration pattern */
    error = oyConfigDomainList( registration, &texts, &count, &d_rank_list, 0 );
    if(count)
      s = oyConfigs_New( 0 );

    if(error <= 0 && count && texts)
      cmm_api8 = (oyCMMapi8_s*) oyCMMsGetFilterApi_( 0, texts[0],
                                                     oyOBJECT_CMM_API8_S );

    for(i = 0; i < count; ++i)
    {
      /** 2. obtain the directory structure for configurations */
      key_set_names = oyKeySetGetNames_( texts[i], &n );

      if(error <= 0)
      for(j = 0; j < n; ++j)
      {
        /** 3. obtain all keys from one configuration directory */
        config_key_names = oyKeySetGetNames_( key_set_names[j], &k_n );

        config = oyConfig_New( texts[i], object );
        error = !config;

        for(k = 0; k < k_n; ++k)
        {
          /** 4. create a oyOption_s for each Elektra DB key/value pair */
          if(error <= 0)
            o = oyOption_FromDB( config_key_names[k], object );
          error = !o;
          if(error <= 0)
            error = oyOptions_Add( config->db, o, -1, 0 );
          else
          {
            WARNcc1_S( (oyStruct_s*) object, "Could not generate key %s",
                       config_key_names[k] );
            break;
          }
          oyOption_Release( &o );
        }

        /* add information about the data's origin */
        oyConfig_AddDBData( config, "key_set_name", key_set_names[j],
                            OY_CREATE_NEW );

        /* add a rank map to allow for comparisions */
        if(cmm_api8)
          config->rank_map = oyRankMapCopy( cmm_api8->rank_map,
                                            config->oy_->allocateFunc_ );

        oyConfigs_MoveIn( s, &config, -1 );
      }
    }

    oyStringListRelease_( &texts, count, oyDeAllocateFunc_ );
  }

  if(configs)
    *configs = s;
  else
    oyConfigs_Release( &s );

  oyExportEnd_();
  return error;
}

/** Function oyConfigDomainList
 *  @memberof oyConfigs_s
 *  @brief   count and show the global oyConfigs_s suppliers
 *
 *  @verbatim
    uint32_t count = 0,
           * rank_list = 0;
    char ** texts = 0,
          * temp = 0,
         ** attributes = 0,
          * device_class = 0;
    int i,j, attributes_n;

    // get all configuration filters
    oyConfigDomainList("//"OY_TYPE_STD"/config", &texts, &count,&rank_list ,0 );
    for( i = 0; i < count; ++i )
    {
      attributes_n = 0;

      // pick the filters name and remove the common config part
      temp = oyFilterRegistrationToText( texts[i], oyFILTER_REG_APPLICATION,
                                         malloc );
      attributes = oyStringSplit_( temp, '.', &attributes_n, malloc );
      free(temp);
      temp = malloc(1024); temp[0] = 0;
      for(j = 0; j < attributes_n; ++j)
      {
        if(strcmp(attributes[j], "config") == 0)
          continue;

        if(j && temp[0])
          sprintf( &temp[strlen(temp)], "." );
        sprintf( &temp[strlen(temp)], "%s", attributes[j]);
      }

      // The string in temp can be passed as the device_class argument to 
      // oyDevicesGet()
      printf("%d: %s \"%s\"\n", i, texts[i], temp);

      oyStringListRelease_( &attributes, attributes_n, free );
      free (device_class);
      free(temp);
    }
    @endverbatim
 *
 *  @param[in]     registration_pattern a optional filter
 *  @param[out]    list                the list with full filter registrations
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
                                       uint32_t          * count,
                                       uint32_t         ** rank_list,
                                       oyAlloc_f           allocateFunc )
{
  oyCMMapiFilters_s * apis = 0;
  oyCMMapiFilter_s * api = 0;
  int error = !list || !count;
  char ** reg_lists = 0;
  int i = 0,
      reg_list_n = 0;
  uint32_t apis_n = 0;

  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    apis = oyCMMsGetFilterApis_( 0,0, registration_pattern,
                                 oyOBJECT_CMM_API8_S, 
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 rank_list, &apis_n);
    error = !apis;
  }

  if(error <= 0)
  {
    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    for(i = 0; i < apis_n; ++i)
    {
      api = oyCMMapiFilters_Get( apis, i );
      oyStringListAddStaticString_( &reg_lists, &reg_list_n,
                                    oyNoEmptyString_m_( api->registration ),
                                    oyAllocateFunc_, oyDeAllocateFunc_ );

      if(api->release)
        api->release( (oyStruct_s**)&api );
    }

    if(reg_list_n && reg_lists)
      *list = oyStringListAppend_( (const char**)reg_lists, reg_list_n, 0,0,
                                   &reg_list_n, allocateFunc );

    oyStringListRelease_( &reg_lists, reg_list_n, oyDeAllocateFunc_ );
  }

  if(count)
    *count = reg_list_n;

  oyCMMapiFilters_Release( &apis );

  oyExportEnd_();
  return error;
}

/** @internal
 *  @struct  oyConfDomain_s_
 *  @brief   a ConfDomain object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
typedef struct {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CONF_DOMAIN_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyCMMapi8_s        * api8;
} oyConfDomain_s_;

oyConfDomain_s_ *
           oyConfDomain_New_         ( oyObject_s          object );
oyConfDomain_s_ *
           oyConfDomain_FromReg_     ( const char        * registration,
                                       oyObject_s          object );
oyConfDomain_s_ *
           oyConfDomain_Copy_        ( oyConfDomain_s_   * obj,
                                       oyObject_s          object);
int
           oyConfDomain_Release_     ( oyConfDomain_s_   **obj );

const char * oyConfDomain_GetText_   ( oyConfDomain_s_   * obj,
                                       const char        * name,
                                       oyNAME_e            type );
const char **oyConfDomain_GetTexts_  ( oyConfDomain_s_   * obj );

/* --- Public_API Begin --- */

/** Function oyConfDomain_New
 *  @memberof oyConfDomain_s
 *  @brief   allocate a new ConfDomain object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_FromReg      ( const char        * registration_domain,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * obj = 0;

  obj = oyConfDomain_FromReg_( registration_domain, object );

  return (oyConfDomain_s*) obj;
}

/** Function oyConfDomain_Copy
 *  @memberof oyConfDomain_s
 *  @brief   copy or reference a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_Copy         ( oyConfDomain_s    * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;

  if(s)
    oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 0 );

  s = oyConfDomain_Copy_( s, (oyObject_s) object );

  return (oyConfDomain_s*) s;
}
 
/** Function oyConfDomain_Release
 *  @memberof oyConfDomain_s
 *  @brief   release and possibly deallocate a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI int  OYEXPORT
           oyConfDomain_Release      ( oyConfDomain_s    **obj )
{
  oyConfDomain_s_ * s = 0;

  if(!obj || !*obj)
    return 0;

  s = (oyConfDomain_s_*) *obj;

  oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 1 )

  *obj = 0;

  return oyConfDomain_Release_( &s );
}

/** Function oyConfDomain_GetText
 *  @memberof oyConfDomain_s
 *  @brief   obtain a UI text from a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     name                the category to return
 *  @param[in]     type                the type of string
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI const char * OYEXPORT
           oyConfDomain_GetText      ( oyConfDomain_s    * obj,
                                       const char        * name,
                                       oyNAME_e            type )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;
  const char * text = 0;

  if(s)
    oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 0 );

  text = oyConfDomain_GetText_( s, name, type );

  return text;
}

/** Function oyConfDomain_GetTexts
 *  @memberof oyConfDomain_s
 *  @brief   obtain a list of possible UI text from a ConfDomain object
 *
 *  @return                            zero terminated list of strings,
 *                                     Each string is a "name" option to
 *                                     oyConfDomain_GetText().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI const char ** OYEXPORT
           oyConfDomain_GetTexts     ( oyConfDomain_s    * obj )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;
  const char ** texts = 0;

  if(s)
    oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 0 );

  texts = oyConfDomain_GetTexts_( s );

  return texts;
}

/* --- Public_API End --- */


/** @internal
 *  Function oyConfDomain_New_
 *  @memberof oyConfDomain_s_
 *  @brief   allocate a new ConfDomain object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_New_  ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONF_DOMAIN_S;
# define STRUCT_TYPE oyConfDomain_s_
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConfDomain_Copy;
  s->release = (oyStruct_Release_f) oyConfDomain_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->api8 = 0;

  return s;
}

oyConfDomain_s_ * oyConfDomain_FromReg_(
                                       const char        * registration,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = oyConfDomain_New_( object );
  int error = !s;
  oyCMMapi8_s * cmm_api8 = 0;

  if(error <= 0)
  {
    cmm_api8 = (oyCMMapi8_s*) oyCMMsGetFilterApi_( 0, registration,
                                                   oyOBJECT_CMM_API8_S );
    error = !cmm_api8;
  }

  if(error <= 0)
    s->api8 = cmm_api8;

  return s;
}

/** @internal
 *  Function oyConfDomain_Copy__
 *  @memberof oyConfDomain_s_
 *  @brief   real copy a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_Copy__ (
                                       oyConfDomain_s_   * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyConfDomain_New_( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    if(obj->api8)
    {
      if(obj->api8->copy)
        s->api8 = (oyCMMapi8_s*) obj->api8->copy( (oyStruct_s*)s->api8,
                                                  object );
      else
        s->api8 = obj->api8;
    }
  }

  if(error)
    oyConfDomain_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfDomain_Copy_
 *  @memberof oyConfDomain_s_
 *  @brief   copy or reference a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_Copy_ ( oyConfDomain_s_   * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = obj;

  if(!obj)
    return 0;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConfDomain_Copy__( obj, object );

  return s;
}
 
/** @internal
 *  Function oyConfDomain_Release_
 *  @memberof oyConfDomain_s_
 *  @brief   release and possibly deallocate a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
int        oyConfDomain_Release_     ( oyConfDomain_s_   **obj )
{
  /* ---- start of common object destructor ----- */
  oyConfDomain_s_ * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->api8)
    {
      if(s->api8->release)
        s->api8->release( (oyStruct_s**) &s->api8 );
      else
        s->api8 = 0;
    }

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @internal
 *  Function oyConfDomain_GetText_
 *  @memberof oyConfDomain_s
 *  @brief   obtain a UI text from a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     name                the category to return
 *  @param[in]     type                the type of string
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
const char * oyConfDomain_GetText_   ( oyConfDomain_s_   * obj,
                                       const char        * name,
                                       oyNAME_e            type )
{
  const char * text = 0;
  oyConfDomain_s_ * s = obj;

  if(s->api8 && s->api8->ui && s->api8->ui->getText)
    text = s->api8->ui->getText( name, type, (oyStruct_s*)s->api8->ui );

  return text;
}

/** @internal
 *  Function oyConfDomain_GetTexts
 *  @memberof oyConfDomain_s
 *  @brief   obtain a list of possible UI text from a ConfDomain object
 *
 *  @return                            zero terminated list of strings,
 *                                     Each string is a "name" option to
 *                                     oyConfDomain_GetText().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
const char **oyConfDomain_GetTexts_  ( oyConfDomain_s_   * obj )
{
  oyConfDomain_s_ * s = obj;
  const char ** texts = 0;

  if(s->api8 && s->api8->ui)
    texts = s->api8->ui->texts;

  return texts;
}


/**
 *  @} *//* objects_value
 */

/** @} *//* misc */


/** \addtogroup devices_handling Device API
 *
 *  Devices are a special form of configurations. Their access is grouped
 *  for effective performance. Known devices are queried with
 *  oyDevicesGet(). oyConfigDomainList() provides a list of known device
 *  modules.
 *  A single device can be obtained by oyDeviceGet(). The \a
 *  device_type argument defaults to OY_TYPE_STD and can be omitted for this
 *  group. The \a device_class argument specifies a subgroup, e.g. 
 *  "monitor".
 *
 *  All other functions return a handle to the device. With this handle it is
 *  possible to get informations (oyDeviceGetInfo()), query it's current,
 *  possibly remote profile (oyDeviceAskProfile2()) or typical used get a 
 *  profile with fallbacks including the DB through (oyDeviceGetProfile()), 
 *  set the profile persistent (oyDeviceSetProfile()) or query the persistent
 *  stored profile (oyDeviceProfileFromDB()).
 *
 *  @{
 */


/** @internal
 *  Function oyDeviceRegistrationCreate_
 *  @brief   combine a device registration
 *
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     old_text            string to reuse
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
char * oyDeviceRegistrationCreate_   ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       char              * old_text )
{
  char * text = old_text;
  const char * device_type_ = device_type;

  if(!device_type_)
    device_type_ = OY_TYPE_STD;

  STRING_ADD( text, "//" );
  STRING_ADD( text, device_type_ );
  STRING_ADD( text, "/config" );
  if(device_class)
  {
    STRING_ADD( text, "." );
    STRING_ADD( text, device_class );
  }
  if(key)
  {
    STRING_ADD( text, "/" );
    STRING_ADD( text, key );
  }

  return text;
}

/** Function oyOption_SetValueFromDB
 *  @memberof oyOption_s
 *  @brief   value filled from DB if available
 *
 *  @param         option              the option
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 *  @date    2009/05/25
 */
int            oyOption_SetValueFromDB  ( oyOption_s        * option )
{
  int error = !option || !oyOption_GetRegistration(option);
  char * text = 0;
  oyPointer ptr = 0;
  size_t size = 0;
  oyOption_s * s = option;

  if(error)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 1 )

  oyExportStart_(EXPORT_SETTING);

  if(error <= 0)
    text = oyGetKeyString_( oyOption_GetText( option, oyNAME_DESCRIPTION),
                            oyAllocateFunc_ );

  if(error <= 0)
  {
    /** Change the option value only if something was found in the DB. */
    if(text && text[0])
    {
      oyOption_SetFromText( option, text, 0 );
      oyOption_SetSource( s, oyOPTIONSOURCE_DATA );
    }
    else
    {
      ptr = oyGetKeyBinary_( oyOption_GetRegistration(s), &size, oyAllocateFunc_ );
      if(ptr && size)
      {
        oyOption_SetFromData( option, ptr, size );
        oyOption_SetSource( s, oyOPTIONSOURCE_DATA );
        oyFree_m_( ptr );
      }
    }
  }

  if(text)
    oyFree_m_( text );

  oyExportEnd_();

  return error;
}
/** Function oyOption_FromDB
 *  @memberof oyOption_s
 *  @brief   new option with registration and value filled from DB if available
 *
 *  @param         registration        no or full qualified registration
 *  @param         object              the optional object
 *  @return                            the option
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 *  @date    2009/01/24
 */
oyOption_s *   oyOption_FromDB       ( const char        * registration,
                                       oyObject_s          object )
{
  int error = !registration;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    /** This is merely a wrapper to oyOption_New() and
     *  oyOption_SetValueFromDB(). */
    o = oyOption_FromRegistration( registration, object );
    error = oyOption_SetFromText( o, 0, 0 );
    error = oyOption_SetValueFromDB( o );
    oyOption_SetSource( o, oyOPTIONSOURCE_DATA );
  }

  return o;
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
 *  Modules should handle the advanced options as well but shall normally
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
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED;
 *                                     for front end options |
 *                                     oyOPTIONATTRIBUTE_FRONT
 *  @param         filter_type         the type level from a registration
 *  @return                            options
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
int          oyOptions_DoFilter      ( oyOptions_s       * opts,
                                       uint32_t            flags,
                                       const char        * filter_type )
{
  oyOptions_s * s = opts;
  oyOptions_s * opts_tmp = 0;
  oyOption_s * o = 0;
  int error = !s;
  char * text;
  int i,n;

  oyExportStart_(EXPORT_SETTING);
  oyExportEnd_();

  if(error <= 0 && (flags || filter_type))
  {
    /*  6. get stored values */
    n = oyOptions_Count( opts );
    opts_tmp = oyOptions_New(0);
    for(i = 0; i < n; ++i)
    {
      int skip = 0;

      o = oyOptions_Get( opts, i );


      /* usage/type range filter */
      if(filter_type)
      {
        text = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                           oyFILTER_REG_TYPE, 0);
        if(oyStrcmp_( filter_type, text ) != 0)
          skip = 1;

        oyFree_m_( text );
      }

      /* front end options filter */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_FRONT))
      {
        text = oyStrrchr_( oyOption_GetRegistration(o), '/' );

        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "front" ))
            skip = 1;
      }

      /* advanced options mark and zero */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_ADVANCED))
      {
        text = oyStrrchr_( oyOption_GetRegistration(o), '/' );
        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "advanced" ))
          {
            oyOption_SetFromText( o, "0", 0 );
            oyOption_SetFlags( o,
                              oyOption_GetFlags(o) & (~oyOPTIONATTRIBUTE_EDIT));
          }
      } else
      /* Elektra settings, modify value */
      if(!skip && !(flags & oyOPTIONSOURCE_FILTER))
      {
        text = oyGetKeyString_( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                oyAllocateFunc_ );
        if(text && text[0])
        {
          error = oyOption_SetFromText( o, text, 0 );
          oyOption_SetFlags(o,oyOption_GetFlags(o) & (~oyOPTIONATTRIBUTE_EDIT));
          oyOption_SetSource( o, oyOPTIONSOURCE_USER );
          oyFree_m_( text );
        }
      }

      if(!skip)
        oyOptions_Add( (oyOptions_s*)opts_tmp, o, -1, s->oy_ );

      oyOption_Release( &o );
    }

    n = oyOptions_Count( opts_tmp );
    error = oyOptions_Clear(s);
    for( i = 0; i < n && !error; ++i )
    {
      o = oyOptions_Get( opts_tmp, i );
      error = oyOptions_MoveIn( s, &o, -1 );
    }
    oyOptions_Release( &opts_tmp );
  }

  return error;
}

/** @internal
 *  Function oyOptions_ForFilter_
 *  @memberof oyOptions_s
 *  @brief   provide Oyranos behaviour settings
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all front end options from a meta module use: @verbatim
    flags = oyOPTIONATTRIBUTE_ADVANCED |
            oyOPTIONATTRIBUTE_FRONT |
            OY_SELECT_COMMON @endverbatim
 *
 *  @param[in]     filter              the filter
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 *  @date    2009/07/27
 */
oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s    * filter,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0,
              * opts_tmp = 0,
              * opts_tmp2 = 0;
  oyOption_s * o = 0;
  int error = !filter || !filter->api4_;
  char * type_txt = oyFilterRegistrationToText( filter->registration_,
                                                oyFILTER_REG_TYPE, 0 );
  oyCMMapi5_s * api5 = 0;
  int i,n;

  /* by default we parse both sources */
  if(!(flags & OY_SELECT_FILTER) && !(flags & OY_SELECT_COMMON))
    flags |= OY_SELECT_FILTER | OY_SELECT_COMMON;

  if(!error)
  {
    /*
        Programm:
        1. get filter and its type
        2. get implementation for filter type
        3. parse static common options from meta module
        4. parse static options from filter 
        5. merge both
        6. get stored values from disk
     */

    /*  1. get filter */

    /*  2. get implementation for filter type */
    api5 = filter->api4_->api5_;

    /*  3. parse static common options from a policy module */
    if(api5 && flags & OY_SELECT_COMMON)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0;
      uint32_t         * rank_list = 0;
      oyCMMapi9_s * cmm_api9 = 0;
      char * klass, * api_reg;

      klass = oyFilterRegistrationToText( filter->registration_,
                                          oyFILTER_REG_TYPE, 0 );
      api_reg = oyStringCopy_("//", oyAllocateFunc_ );
      STRING_ADD( api_reg, klass );
      oyFree_m_( klass );

      s = oyOptions_New( 0 );

      apis = oyCMMsGetFilterApis_( 0,0, api_reg,
                                   oyOBJECT_CMM_API9_S, 
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   &rank_list, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );
        if(oyFilterRegistrationMatch( filter->registration_, cmm_api9->pattern,
                                      oyOBJECT_NONE ))
        {
          opts_tmp = oyOptions_FromText( cmm_api9->options, 0, object );
          oyOptions_AppendOpts( s, opts_tmp );
          oyOptions_Release( &opts_tmp );
        }
        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );
      }
      oyCMMapiFilters_Release( &apis );
      oyFree_m_( api_reg );
      opts_tmp = s; s = 0;
    }
    /* requires step 2 */

    /*  4. parse static options from filter */
    if(flags & OY_SELECT_FILTER)
      opts_tmp2 = oyOptions_FromText( filter->api4_->ui->options, 0, object );

    /*  5. merge */
    s = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION, object );

    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );

    /*  6. get stored values */
    n = oyOptions_Count( s );
    for(i = 0; i < n && error <= 0; ++i)
    {
      o = oyOptions_Get( s, i );
      oyOption_SetSource( o, oyOPTIONSOURCE_FILTER );
      /* ask Elektra */
      if(!(flags & oyOPTIONSOURCE_FILTER))
        error = oyOption_SetValueFromDB( o );
      oyOption_Release( &o );
    }
    error = oyOptions_DoFilter ( s, flags, type_txt );
  }

  if(type_txt)
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
 *  To obtain all advanced front end options from a meta module use:@verbatim
 *  flags = oyOPTIONATTRIBUTE_ADVANCED |
 *          oyOPTIONATTRIBUTE_FRONT |
 *          OY_SELECT_COMMON @endverbatim
 *
 *  @see OY_SELECT_FILTER OY_SELECT_COMMON oyOPTIONATTRIBUTE_e
 *
 *  @param[in]     registration        the filter registration to search for
 *  @param[in]     cmm                 a CMM to match
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/10/08 (Oyranos: 0.1.8)
 *  @date    2011/01/29
 */
oyOptions_s *  oyOptions_ForFilter   ( const char        * registration,
                                       const char        * cmm,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  oyFilterCore_s * filter = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  char * lib_name = 0;
  int error = 0;

  /*  1. get filter */
  filter = oyFilterCore_New_( object );

  error = !filter;

  if(error <= 0)
    cmm_api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( cmm, registration,
                                                   oyOBJECT_CMM_API4_S );

  if(cmm_api4)
    lib_name = cmm_api4->id_;

  error = !(cmm_api4 && lib_name);

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( filter, cmm_api4 );

  s = oyOptions_ForFilter_( filter, flags, object);

  oyFilterCore_Release( &filter );

  return s;
}

/** Function oyOptions_SaveToDB
 *  @memberof oyOptions_s
 *  @brief   store a oyOptions_s in DB
 *
 *  @param[in]     options             the options
 *  @param[in]     registration        the registration
 *  @param[out]    new_reg             the new registration; optional
 *  @param[in]     alloc               the user allocator for new_reg; optional
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2011/01/29
 */
OYAPI int  OYEXPORT
               oyOptions_SaveToDB    ( oyOptions_s       * options,
                                       const char        * registration,
                                       char             ** new_reg,
                                       oyAlloc_f           allocateFunc )
{
  int error = !options || !registration;
  oyOption_s * o = 0;
  int n,i;
  char * key_base_name = 0,
       * key_name = 0,
       * key_top = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    key_base_name = oySearchEmptyKeyname_( registration );
    error = !key_base_name;
    if(error <= 0)
    {
      STRING_ADD( key_base_name, OY_SLASH );
    }

    n = oyOptions_Count( options );
    for( i = 0; i < n; ++i )
    {
      o = oyOptions_Get( options, i );
      key_top = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                            oyFILTER_REG_MAX, 0 );


      STRING_ADD( key_name, key_base_name );
      STRING_ADD( key_name, key_top );
      if(oyOption_GetValueString(o,0))
        error = oyAddKey_valueComment_( key_name, oyOption_GetValueString(o,0),
                                        0 );
# if 0
      else if(o->value_type == oyVAL_STRUCT &&
              o->value && o->value->oy_struct->type_ == oyOBJECT_BLOB_S)
        error = 0;/*oyAddKeyBlobComment_();*/
#endif
      else
        WARNcc_S( (oyStruct_s*)o,
                    "Could not save non string / non binary option" );

      oyOption_Release( &o );
      oyFree_m_( key_name );
    }

    if(error <= 0 && new_reg && key_base_name)
    {
      key_base_name[strlen(key_base_name)-1] = '\000';
      *new_reg = oyStringCopy_(key_base_name, allocateFunc);
    }
    oyFree_m_( key_base_name );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** @internal
 *  Function oyOptions_SetDeviceTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s       * options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = 0;

  text = oyDeviceRegistrationCreate_( device_type, device_class,
                                          key, text );
  error = oyOptions_SetFromText( &options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );

  return error;
}

/** @internal
 *  Function oyOptions_SetRegistrationTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     registration        the registration string
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/09 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s       * options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = 0;

  STRING_ADD( text, registration );
  STRING_ADD( text, "/" );
  STRING_ADD( text, key );

  error = oyOptions_SetFromText( &options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );

  return error;
}

/** Function oyOptions_SetDriverContext
 *  @memberof oyOptions_s
 *  @brief   set a device option from a given external context
 *
 *  The options will be created in case they do not exist. The 
 *  driver_context_type accepts "xml". The data in driver_context will be
 *  converted to a options set following the Oyranos options XML schemes with 
 *  the help oyOptions_FromText().
 *  Any other pointer will be converted to a oyBlob_s object. The name of that
 *  object will come from driver_context_type.
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     driver_context      driver context
 *  @param[in]     driver_context_type "xml" or something related to the driver
 *  @param[in]     driver_context_size size of driver_context
 *  @param[in]     object              a optional object
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/18 (Oyranos: 0.1.10)
 *  @date    2009/05/18
 */
OYAPI int  OYEXPORT
           oyOptions_SetDriverContext( oyOptions_s      ** options,
                                       oyPointer           driver_context,
                                       const char        * driver_context_type,
                                       size_t              driver_context_size,
                                       oyObject_s          object )
{
  int error = !options || !driver_context_type,
      l_error = 0;
  oyOptions_s * s = 0,
              * opts_tmp = 0;
  oyOption_s * o = 0;
  char * key = 0;

  if(options && *options)
  {
    s = *options;
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 );
  }

  if(!error)
  {
    if(!s)
      s = oyOptions_New( 0 );
    error = !s;

    key = oyStringAppend_( "driver_context.", driver_context_type,
                           oyAllocateFunc_ );
  }

  if(!error)
  {
    o = oyOption_FromRegistration( key, object );

    if(oyFilterRegistrationMatch( driver_context_type, "xml", 0 ))
    {
      opts_tmp = oyOptions_FromText( (char*)driver_context, 0, object );
      error = oyOption_StructMoveIn ( o, (oyStruct_s**) &opts_tmp );
    }
    else
      error = oyOption_SetFromData( o, driver_context, driver_context_size );

    if(error <= 0)
      l_error = oyOptions_MoveIn( s, &o, -1 ); OY_ERR

    oyFree_m_( key );
  }

  if(!error)
    *options = s;
  else
    oyOptions_Release( &s );

  return error;
}


/** Function oyDevicesGet
 *  @brief   get all devices matching to a device class and type
 *
 *  @verbatim
    // "list" all monitors
    oyConfig_s * monitors = 0;
    int error = oyDevicesGet( 0, "monitor", 0, &monitors );
    // see how many are included
    int n = oyConfigs_Count( monitors );
    // release them
    oyConfigs_Release( &monitors );
    @endverbatim
 *
 *  For obtaining expensive "properties" informations at once, add the according
 *  option.
 *  @verbatim
    // get all monitors the expensive way
    oyConfig_s * monitors = 0;
    oyOptions_s * options = oyOptions_New( 0 );
    int error = 0;

    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );
    error = oyDevicesGet( 0, "monitor", 0, &monitors );
    oyOptions_Release( &options );

    // see how many are included
    int n = oyConfigs_Count( monitors );

    // release them
    oyConfigs_Release( &monitors );
    @endverbatim
 *
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     options             options for the device
 *  @param[out]    devices             the found devices
 *  @return                            0 - good, >= 1 - error, <= -1 unknown
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/02 (Oyranos: 0.1.10)
 *  @date    2009/02/02
 */
OYAPI int  OYEXPORT
           oyDevicesGet              ( const char        * device_type,
                                       const char        * device_class,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** devices )
{
  int error = !device_class || !device_class[0];
  static char * num = 0;

  if(error > 0)
  {
    WARNc_S( "Argument(s) incorrect. Giving up" );
    return error;
  }

  if(!num)
    oyAllocHelper_m_( num, char, 80, 0, error = 1; return error );

  /** 1. obtain detailed and expensive device informations */

  if(!options)
  {
    options = oyOptions_New( 0 );
    /** 1.1 add "list" call to module arguments */
    error = oyOptions_SetDeviceTextKey_( options, device_type,
                                             device_class,
                                             "command", "list" );
  }

  /** 1.2 ask each module */
  if(error <= 0)
    error = oyConfigs_FromDeviceClass( device_type, device_class,
                                           options, devices, 0 );


  return error;
}

/** Function oyDeviceGet
 *  @brief   ask a module for device informations or other direct calls
 *
 *  @verbatim
    oyConfig_s * device = 0;
    int error = oyDeviceGet( 0, "monitor", ":0.0", 0, &device );
    oyConfig_Release( &device );
    @endverbatim
 *
 *  @verbatim
    // pass empty options to the module to get a usage message
    oyOptions_s * options = oyOptions_New( 0 );
    oyDeviceGet( OY_TYPE_STD, "monitor", ":0.0", options, 0 );
    @endverbatim
 *
 *  @param[in]     device_type         the device type, e.g. OY_TYPE_STD,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        registration ::oyFILTER_REG_APPLICATION
 *                                     part, e.g. "monitor", mandatory
 *  @param[in]     device_name         the device name as returned by
 *                                     oyConfigs_FromPattern_f, mandatory,
                                       ::oyFILTER_REG_OPTION
 *  @param[in]     options             options to pass to the module, for zero
 *                                     the verbose and expensive "properties"
 *                                     call is assumed
 *  @param[out]    device              the returned device
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
OYAPI int  OYEXPORT
           oyDeviceGet               ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device )
{
  int error = !device_name || !device_name[0] ||
              !device_class || !device_class[0];
  oyConfigs_s * devices = 0;
  oyConfig_s * s = 0;

  if(error > 0)
  {
    WARNc2_S( "No device_name/device_class argument provided. Give up: %s/%s",
              oyNoEmptyString_m_(device_name),
              oyNoEmptyString_m_(device_class) );
    return 0;
  }

  /** 1. obtain basic device informations */

  if(!options)
  {
    options = oyOptions_New( 0 );
    error = !options;
    /** 1.1 add "list" call to module arguments */
    if(error <= 0)
    error = oyOptions_SetDeviceTextKey_( options, device_type,
                                             device_class,
                                             "command", "list" );
  }

  /** 1.1.2 set device filter */
  if(error <= 0)
    error = oyOptions_SetDeviceTextKey_( options, device_type,
                                             device_class,
                                             "device_name",device_name);

  /** 2. get the device */
  error = oyConfigs_FromDeviceClass( device_type, device_class,
                                         options, &devices, 0 );

  s = oyConfigs_Get( devices, 0 );

  oyConfigs_Release( &devices );


  /** 3. check for success of device detection */
  error = !s;
  if(error)
    WARNc2_S( "%s: \"%s\"", _("Could not open device"), device_name );

  if(device)
    *device = s;
  else
    oyConfig_Release( &s );

  return error;
}

/** Function oyDeviceBackendCall
 *  @brief   get device answere from options
 *
 *  @param[in]     device          the device
 *  @param[in]     options             options for the device
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/02 (Oyranos: 0.1.10)
 *  @date    2009/08/25
 */
OYAPI int  OYEXPORT
               oyDeviceBackendCall   ( oyConfig_s        * device,
                                       oyOptions_s       * options )
{
  int error = !device,
      l_error = 0;
  oyConfigs_s * devices = 0;
  oyConfig_s * s = device;
  const char * device_name = 0;
  int new_options = 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error <= 0)
  {
    device_name = oyConfig_FindString( device, "device_name", 0);
  }

  if(!options)
  {
    /** 1. obtain detailed and expensive device informations for a
     *     zero options argument through the "properties" command. */
    options = oyOptions_New( 0 );
    l_error = !options; OY_ERR
    /** 1.1 add "properties" call to module arguments */
    if(error <= 0)
    l_error = oyOptions_SetRegistrationTextKey_( options,
                                                 device->registration,
                                                 "command", "properties" ); OY_ERR
    new_options = 1;
  }

  devices = oyConfigs_New( 0 );
  error = !devices;
  
  if(error <= 0)
  {
    /* Keep a reference to config as devices will later be released. */
    s = oyConfig_Copy( device, 0 );

    oyConfigs_MoveIn( devices, &device, -1 );
  }

  /** 3. talk to the module */
  l_error = oyConfigs_Modify( devices, options ); OY_ERR

  oyConfigs_Release( &devices );
  if(new_options)
    oyOptions_Release( &options );

  return error;
}


icProfileClassSignature oyDeviceSigGet(oyConfig_s        * device )
{
  icProfileClassSignature deviceSignature = 0;
  if(oyFilterRegistrationMatch( device->registration, "monitor", 0 ))
    deviceSignature = icSigDisplayClass;
  else if(oyFilterRegistrationMatch( device->registration, "scanner", 0 ))
    deviceSignature = icSigInputClass;
  else if(oyFilterRegistrationMatch( device->registration, "raw-image", 0 ))
    deviceSignature = icSigInputClass;
  else if(oyFilterRegistrationMatch( device->registration, "printer", 0 ))
    deviceSignature = icSigOutputClass;

  return deviceSignature;
}

/** Function oyDeviceSetup
 *  @brief   activate the device using the stored configuration
 *
 *  @param[in]     device              the device
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/02/07
 */
OYAPI int  OYEXPORT
         oyDeviceSetup               ( oyConfig_s        * device )
{
  int error = !device;
  oyOptions_s * options = 0;
  oyProfile_s * p = 0;
  char * profile_name = 0,
       * profile_name_temp = 0;
  const char * device_name = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  {
    /* 1. ask for the profile the device is setup with */
    error = oyDeviceAskProfile2( device, 0, &p );
    if(p)
    {
      oyProfile_Release( &p );
      /** We ignore a device, which already has its profile setup. */
      return error;
    }

    /* 2. query the full device information */
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );

    /* 2.1 select best match to device from installed profiles */
    if(!profile_name)
    {
      int size;
      oyProfile_s * profile = 0;
      oyProfiles_s * patterns = 0, * iccs = 0;
      icProfileClassSignature device_signature = oyDeviceSigGet(device);
      int32_t * rank_list = 0;
      double clck;

      profile = oyProfile_FromSignature( device_signature, oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      clck = oyClock();
      iccs = oyProfiles_Create( patterns, 0 );
      clck = oyClock() - clck;
      DBG_NUM1_S("oyProfiles_Create(): %g", clck/1000000.0 );
      oyProfiles_Release( &patterns );
 
      size = oyProfiles_Count(iccs);
      oyAllocHelper_m_( rank_list, int32_t, oyProfiles_Count(iccs), 0, error = 1; return error );
      if(error <= 0)
      {
        clck = oyClock();
        oyProfiles_DeviceRank( iccs, device, rank_list );
        clck = oyClock() - clck;
        DBG_NUM1_S("oyProfiles_DeviceRank(): %g", clck/1000000.0 );
      }
      if(error <= 0 && size && rank_list[0] > 0)
      {
        p = oyProfiles_Get( iccs, 0 );
        profile_name = oyStringCopy_( oyProfile_GetFileName(p, -1),
                                      oyAllocateFunc_ );
        WARNc1_S( "implicitely selected %s", oyNoEmptyString_m_(profile_name) );
        oyFree_m_( rank_list );
      }

      oyProfile_Release( &p );
      oyProfiles_Release( &iccs );
    }

    /* 2.2 for no profile name: skip the "setup" call */
    if(!profile_name)
    {
      oyOptions_s * fallback = oyOptions_New( 0 );
      error = oyOptions_SetRegistrationTextKey_( fallback,
                                                 device->registration,
                                                 "icc_profile.fallback","true");
      /* 2.2.1 try fallback for rescue */
      error = oyDeviceAskProfile2( device, fallback, &p );
      oyOptions_Release( &fallback );
      if(p)
      {
        profile_name = oyStringCopy_( oyProfile_GetFileName(p, -1),
                                      oyAllocateFunc_ );
        if(!profile_name)
        {
          char * data = 0;
          size_t size = 0;
          data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
          if(data && size)
          {
            char * fn = 0;
            const char * t = 0;
            STRING_ADD( fn, OY_USERCOLORDATA OY_SLASH OY_ICCDIRNAME OY_SLASH );
            STRING_ADD( fn, "devices" OY_SLASH );
            STRING_ADD( fn, oyICCDeviceClassDescription(
                             oyProfile_GetSignature( p, oySIGNATURE_CLASS ) ) );
            STRING_ADD( fn, OY_SLASH );
            if((t = oyProfile_GetText( p, oyNAME_DESCRIPTION )) != 0)
              STRING_ADD( fn, t );
            STRING_ADD( fn, ".icc" );
            
            error = oyWriteMemToFile_ ( fn, data, size );
            if(!error)
              profile_name = fn;
            else
              error = oyWriteMemToFile2_( "oyranos_tmp.icc", data, size,
                                        OY_FILE_NAME_SEARCH | OY_FILE_TEMP_DIR,
                                        &profile_name_temp, oyAllocateFunc_ );
          } else
          {
            error = 1;
            WARNc1_S( "%s",_("Could not open profile") );
          }

          if(profile_name_temp)
            profile_name = profile_name_temp;
          else if( !profile_name )
          {
            error = 1;
            WARNc2_S("%s: \"%s\"(oyranos_tmp.icc)",_("Could not write to file"),
                      oyNoEmptyString_m_(profile_name_temp) );
          }
        }
      }

      if(!profile_name)
        return error;
    }

    /* 2.3 get device_name */
    device_name = oyConfig_FindString( device, "device_name", 0);

    /* 3. setup the device through the module */
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "setup", OY_CREATE_NEW );
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_name",
                                   device_name, OY_CREATE_NEW );
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/profile_name",
                                   profile_name, OY_CREATE_NEW );
    /* 3.1 send the query to a module */
    error = oyDeviceBackendCall( device, options );

    if(profile_name_temp)
      oyRemoveFile_( profile_name_temp );
    profile_name_temp = 0;
    oyOptions_Release( &options );
    if(profile_name)
      oyFree_m_( profile_name );
  }

  return error;
}

/** Function oyDeviceUnset
 *  @brief   unset the device profile
 *
 *  The function solely calls \a unset in the module, e.g. unset graphic card
 *  luts and server stored profile. So pretty all device/server side 
 *  informatin should go away. \n
 *
 *  @param         device          the device
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/12 (Oyranos: 0.1.10)
 *  @date    2009/02/12
 */
int      oyDeviceUnset               ( oyConfig_s        * device )
{
  int error = !device;
  oyOptions_s * options = 0;
  char * profile_name = 0;
  const char * device_name = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  {
    /* 1. query the full device information */
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );

    /* 1.1 get device_name */
    device_name = oyConfig_FindString( device, "device_name", 0);

    /* 2. unset the device through the module */
    /** 2.1 set a general request */
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "unset", OY_CREATE_NEW );
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_name",
                                   device_name, OY_CREATE_NEW );

    /** 2.2 send the query to a module */
    error = oyConfigs_FromDomain( device->registration, options, 0, 0 );

    oyOptions_Release( &options );
    /* 3.1 send the query to a module */
    error = oyDeviceBackendCall( device, options );

    oyOptions_Release( &options );
    if(profile_name)
      oyFree_m_( profile_name );
  }

  return error;
}


/** Function oyDeviceGetInfo
 *  @brief   get all devices matching to a device class and type
 *
 *  @verbatim
    // print all properties
    int error = oyDeviceGetInfo( device, oyNAME_DESCRIPTION, 0, &text,
                                     malloc );
    char * list = text, * tmp = 0, * line = malloc(128);
    int even = 1;

        tmp = list;
        while(list && list[0])
        {
          snprintf( line, 128, "%s", list );
          if(strchr( line, '\n' ))
          {
            tmp = strchr( line, '\n' );
            tmp[0] = 0;
          }
          if(even)
            printf( "%s\n", line );
          else
            printf( "  %s\n", line );
          list = strchr( list, '\n' );
          if(list) ++list;
          even = !even;
        }

    if(line) free(line);
    if(text) free(text);
    @endverbatim
 *
 *  To obtain a certain single pice of information you do not need 
 *  oyDeviceGetInfo. See the following example:
 *  @verbatim
    char * device_name = ":0.0"; // a typical device
    char * text = 0;
    oyConfig_s * device = 0;
    oyOptions_s * options = 0;
    int error = 0;

    // tell the module with the "properties" call to add all informations
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD
                                   "/config/command",
                                   "properties", OY_CREATE_NEW );

    oyDeviceGet( OY_TYPE_STD, "monitor", device_name, options, &device );
    text = oyConfig_FindString( device, "manufacturer", 0 );
    @endverbatim
 *
 *  @param[in]     device          the device
 *  @param[in]     type                influences the info_text output
 *                                     - oyNAME_NAME - a short one line text,
 *                                     - oyNAME_NICK - one word,
 *                                     - oyNAME_DESCRIPTION - expensive text,
 *                                     even lines contain the property key name,
 *                                     odd lines contain the value,
 *                                     lines are separated by newline '\\n'
 *  @param[in]     options             defaults to command=properties
 *  @param[out]    info_text           the text
 *  @param[in]     allocateFunc        the user allocator for info_text
 *  @return                            0 - good, 1 >= error, -1 <= issue(s)
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/02 (Oyranos: 0.1.10)
 *  @date    2009/03/27
 */
OYAPI int  OYEXPORT
           oyDeviceGetInfo           ( oyConfig_s        * device,
                                       oyNAME_e            type,
                                       oyOptions_s       * options,
                                       char             ** info_text,
                                       oyAlloc_f           allocateFunc )
{
  int error = !device || !info_text;
  oyOption_s * o = 0;
  oyConfig_s * config = 0;
  const char * tmp = 0;
  static char * num = 0;
  char * text = 0, * t = 0;
  int i, n,
      own_options = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error > 0)
  {
    WARNc_S( "Argument(s) incorrect. Giving up" );
    return error;
  }

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(!options)
    own_options = 1;

  if(type == oyNAME_NICK)
  {
    tmp = oyOptions_FindString( device->backend_core,"device_name", 0 );
    *info_text = oyStringCopy_( tmp, allocateFunc );
    return error;
  }

  if(type == oyNAME_DESCRIPTION)
  {
    /* get expensive infos */
    if(oyOptions_Count( device->backend_core ) < 2)
    {
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );

      if(error <= 0)
        error = oyDeviceBackendCall( device, options );
    }

    if(error <= 0)
    {
      n = oyOptions_Count( device->backend_core );
      for( i = 0; i < n; ++i )
      {
        o = oyOptions_Get( device->backend_core, i );
        
        STRING_ADD( text, oyStrrchr_( oyOption_GetRegistration(o),
                          OY_SLASH_C ) + 1 );
        STRING_ADD( text, ":\n" );
        t = oyOption_GetValueText(o,oyAllocateFunc_);
        if(t)
        {
          STRING_ADD( text, t );
          oyDeAllocateFunc_(t); t = 0;
        }
        STRING_ADD( text, "\n" );
             
        oyOption_Release( &o );
      }
    }
    *info_text = oyStringCopy_( text, allocateFunc );
    oyFree_m_(text);
    return error;
  }


  if(!num)
    oyAllocHelper_m_( num, char, 80, 0, error = 1; return error );

  if(!options)
  {
    options = oyOptions_New( 0 );

    error = !options;
  }

  if(error <= 0)
  {
    /* add "list" call to module arguments */
    error = oyOptions_SetRegistrationTextKey_( options,
                                               device->registration,
                                               "command", "list" );
  }

  if(error <= 0)
  {
    if(type == oyNAME_NAME)
    error = oyOptions_SetRegistrationTextKey_( options,
                                               device->registration,
                                               "oyNAME_NAME", "true" );
  }


  /** 1.2 ask each module */
  if(error <= 0)
    error = oyDeviceBackendCall( device, options );

  if(error <= 0 && device->backend_core)
  {
    /** 1.2.1 add device_name to the string list */
    if(type == oyNAME_NICK)
      tmp = oyOptions_FindString( device->backend_core,"device_name",0);
    else if(type == oyNAME_NAME)
      tmp = oyOptions_FindString( device->data, "oyNAME_NAME", 0 );
    else if(type == oyNAME_DESCRIPTION)
      tmp = oyOptions_FindString( device->data, "oyNAME_DESCRIPTION", 0 );
  }

  *info_text = oyStringCopy_( tmp, allocateFunc );

  if(own_options)
    oyOptions_Release( &options );
  oyConfig_Release( &config );

  return error;
}

/** Function oyDeviceGetProfile
 *  @brief   order a device profile
 *
 *  This function is designed to satisfy most users as it tries to deliver
 *  a profile all the time. 
 *  Following code can almost allways expect some profile to go with.
 *  It tries hard to get a current profile or set the system up and retry or
 *  get at least one basic profile.
 *
 *  For a basic and thus weaker call to the device use
 *  oyDeviceAskProfile2() instead.
 *
 *  @param         device              the device
 *  @param         options             options passed to the backend
 *  @param         profile             the device's ICC profile
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
OYAPI int  OYEXPORT
           oyDeviceGetProfile        ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyProfile_s      ** profile )
{
  int error = !device,
      l_error = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )


  l_error = oyDeviceAskProfile2( device, options, profile ); OY_ERR

  /** This function does a device setup in case no profile is delivered
   *  by the according module. */
  if(error != 0 && !*profile)
    error = oyDeviceSetup( device );

  if(error <= 0) 
    l_error = oyDeviceAskProfile2( device, options, profile ); OY_ERR

  /** As a last means oyASSUMED_WEB is delivered. */
  if(!*profile)
  {
    *profile = oyProfile_FromStd( oyASSUMED_WEB, 0 );
    if(error == 0)
      error = -1;
  }

  return error;
}

/** Function oyDeviceAskProfile2
 *  @brief   ask for the device profile
 *
 *  Ask for a profile associated with the device. A device capable to
 *  hold a profile. Only the held profile will be checked and returned.
 *  In case this profile is not found a "icc_profile" of oyVAL_STRUCT should be
 *  included.
 *
 *  The device might not be able to hold a profile, then just the DB profile
 *  will be returned from here without an issue. For interessted users, the
 *  source of the profile keeps transparent, as it can be checked if the
 *  device contains a "icc_profile" option which contains a oyProfile_s object.
 *
 *  @param[in]     device              the device
 *  @param[in]     options             additional options
 *  @param[out]    profile             the device's ICC profile
 *  @return                            0 - good, 1 >= error, -1 <= issue(s)
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/10 (Oyranos: 0.1.10)
 *  @date    2009/12/10
 */
OYAPI int  OYEXPORT
           oyDeviceAskProfile2       ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyProfile_s      ** profile )
{
  int error = !device;
  oyOption_s * o = 0;
  oyConfig_s * s = device;
  int own_options = 0;
  oyProfile_s * p = 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )


  if(!options)
  {
    options = oyOptions_New( 0 );
    own_options = 1;
    error = !options;
  }

  if(error <= 0)
  {
    /* add "list" call to module arguments */
    error = oyOptions_SetRegistrationTextKey_( options,
                                               device->registration,
                                               "command", "list" );
  }

  if(error <= 0)
  {
    error = oyOptions_SetRegistrationTextKey_( options,
                                                 device->registration,
                                                 "icc_profile", "true" );
  }

  if(error <= 0)
    error = oyDeviceBackendCall( device, options );

  /* The backend shows with the existence of the "icc_profile" response that it
   * can handle device profiles through the driver. */
  if(error <= 0)
    o = oyConfig_Find( device, "icc_profile" );

  p = (oyProfile_s*) oyOption_StructGet( o, oyOBJECT_PROFILE_S );
  if(oyProfile_GetSignature( p, oySIGNATURE_MAGIC ) == icMagicNumber)
    *profile = p;
  else if(!error)
    error = -1;
  p = 0;

  /* The backend can not handle device driver profiles. Switch back to DB. */
  if(error <= 0 && !(*profile) && !o)
  {
    char * profile_name = 0;
    oyDeviceProfileFromDB( device, &profile_name, 0 );
    if(profile_name)
    {
      *profile = oyProfile_FromFile( profile_name, 0,0 );
      oyDeAllocateFunc_( profile_name );
    }
  }

  if(own_options)
    oyOptions_Release( &options );
  oyOption_Release( &o );

  return error;
}


/** Function oyDeviceSetProfile
 *  @brief   set the device profile
 *
 *  The function will lookup the monitor in the Oyranos device database
 *  and stores the given profile there.
 *
 *  To set a new profile und update the device please call the following
 *  sequence:
 *  @verbatim
    // store new settings in the Oyranos data base
    oyDeviceSetProfile( device, profile );
    // remove any device entries
    oyDeviceUnset( device );
    // update the device from the newly added Oyranos data base settings
    oyDeviceSetup( device );
    @endverbatim
 *
 *  @param         device              the device
 *  @param         profile_name        the device's ICC profile or zero to
 *                                     unset
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/07 (Oyranos: 0.1.10)
 *  @date    2009/02/12
 */
int      oyDeviceSetProfile          ( oyConfig_s        * device,
                                       const char        * profile_name )
{
  int error = !device || !profile_name || !profile_name[0];
  oyOption_s * od = 0;
  oyOptions_s * options = 0;
  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0,
             * device_tmp = 0;
  oyProfile_s * p = 0;
  int i, j, n, j_n, equal;
  char * d_opt = 0;
  const char * device_name = 0,
             * o_val = 0,
             * d_val = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error > 0)
  {
    WARNc1_S( "No profile argument provided. Give up. %s",
              oyNoEmptyString_m_(profile_name) );
    return error;
  }


  /** 1. obtain detailed and expensive device informations */
  if(oyOptions_Count( device->backend_core ) < 2)
  { 
    /** 1.1 add "properties" call to module arguments */
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );

    /** 1.2 get monitor device */
    if(error <= 0)
      error = oyDeviceBackendCall( device, options );

    oyOptions_Release( &options );
  }

  if(error <= 0)
    error = !oyOptions_Count( device->backend_core );

  if(error <= 0)
  {
    device_name = oyConfig_FindString( device, "device_name", 0);
    error = !device_name;
  }

  /** 2. check for success of device detection */
  if(error)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open device"), device_name );
    goto cleanup;
  }

  /** 3 load profile from file name argument */
  p = oyProfile_FromFile( profile_name, 0, 0 );

  /** 3.1 check for success of profile loading */
  error = !p;
  if(error)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open profile"), profile_name );
    goto cleanup;
  }

  /** 4. Now remove all those DB configurations fully matching the selected
   *     device.  */
  if(error <= 0)
  {
    /** 4.1 get stored DB's configurations */
    error = oyConfigs_FromDB( device->registration, &configs, 0 );

    n = oyConfigs_Count( configs );
    for( i = 0; i < n; ++i )
    {
      config = oyConfigs_Get( configs, i );

      equal = 0;

      j_n = oyOptions_Count( device->backend_core );
      for(j = 0; j < j_n; ++j)
      {
        od = oyOptions_Get( device->backend_core, j );
        d_opt = oyFilterRegistrationToText( oyOption_GetRegistration(od),
                                            oyFILTER_REG_MAX, 0 );
        d_val = oyConfig_FindString( device, d_opt, 0 );

        o_val = oyConfig_FindString( config, d_opt, 0 );

        /** 4.1.1 compare if each device key matches to one configuration
         *          key */
        if( (d_val && o_val &&
             oyStrcmp_( d_val, o_val ) == 0 ) ||
            (!d_val && !o_val) )
          ++equal;
        else
          if(oyStrcmp_( d_opt, "profile_name") == 0)
            ++equal;

        oyOption_Release( &od );
        oyFree_m_( d_opt );
      }

      /** 4.1.2 if the 4.1.1 condition is true remove the configuration */
      if(equal == j_n)
        oyConfig_EraseFromDB( config );

      oyConfig_Release( &config );
    }
    oyConfigs_Release( &configs );
  }

  /** 5. save the new configuration with a associated profile \n
   *  5.1 add the profile simply to the device configuration */
  if(error <= 0)
  {
    error = oyConfig_ClearDBData( device );
    error = oyConfig_AddDBData( device, "profile_name", profile_name,
                                OY_CREATE_NEW );
  }

  /** 5.2 save the configuration to DB (Elektra) */
  if(error <= 0)
    error = oyConfig_SaveToDB( device );
  /** 5.3 reload the DB part */
  if(error <= 0)
    error = oyConfig_GetDB( device, 0 );

  cleanup:
  oyConfig_Release( &device_tmp );

  return error;
}

/** Function oyDeviceProfileFromDB
 *  @brief   look up a profile of a device from DB
 *
 *  The function asks the module for a detailed and possible expensive list
 *  of device information and tries to find a matching configuration in the
 *  DB. The device informations are the same as for saving to DB.
 *
 *  @param[in]     device          a device
 *  @param[in]     profile_name        profile's name in DB
 *  @param[in]     allocateFunc        user allocator
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
OYAPI int OYEXPORT oyDeviceProfileFromDB
                                     ( oyConfig_s        * device,
                                       char             ** profile_name,
                                       oyAlloc_f           allocateFunc )
{
  oyOption_s * o = 0;
  oyOptions_s * options = 0;
  int error = !device || !profile_name;
  const char * device_name = 0;
  char * tmp = 0, * tmp2 = 0;
  int32_t rank_value = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    o = oyConfig_Find( device, "profile_name" );
    device_name = oyConfig_FindString( device, "device_name", 0);

    /* 1. obtain detailed and expensive device informations */
    if( !oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0) )
    { 
      /* 1.1 add "properties" call to module arguments */
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_name",
                                     device_name, OY_CREATE_NEW );

      device_name = 0;

      /* 1.2 get device */
      if(error <= 0)
        error = oyDeviceBackendCall( device, options );

      oyOptions_Release( &options );

      /* renew outdated string */
      o = oyConfig_Find( device, "profile_name" );
      device_name = oyConfig_FindString( device, "device_name", 0);
      oyOption_Release( &o );
    }

    if(!o)
    {
      error = oyConfig_GetDB( device, &rank_value );
      o = oyConfig_Find( device, "profile_name" );
    }

    if(!o)
    {
      o = oyOptions_Get( device->db, 0 );
      if(o)
        tmp = oyStringCopy_(oyOption_GetRegistration(o), oyAllocateFunc_);
      if(tmp && oyStrrchr_( tmp, OY_SLASH_C))
      {
        tmp2 = oyStrrchr_( tmp, OY_SLASH_C);
        tmp2[0] = 0;
      }
      WARNc3_S( "\n Could not get a \"profile_name\" from %s\n"
                " registration: \"%s\" rank: %d", 
                oyNoEmptyString_m_(device_name), oyNoEmptyString_m_(tmp),
                (int)rank_value )
      if(tmp)
        oyFree_m_(tmp); tmp2 = 0;
      oyOption_Release( &o );
      error = -1;
    } else if(!oyOption_GetValueString(o,0))
    {
      WARNc1_S( "Could not get \"profile_name\" data from %s", 
                oyNoEmptyString_m_(device_name) )
      error = -1;
    } else
      *profile_name = oyOption_GetValueText( o, allocateFunc );

  } else
    WARNc_S( "missed argument(s)" );

  return error;
}

/** Function oyDeviceSelectSimiliar
 *  @brief   get similiar devices by a pattern from a list
 *
 *  The function takes a device and tries to find exact matches, which can be
 *  considered as belonging to the same device. The comparision can be 
 *  influenced by the flags.
 *  The option "profile_name" is ignored during the comparision.
 *
 *  @param[in]     pattern             Pass a device used as reference. String
 *                                     options of this object are compared to
 *                                     the objects in the heap argument
 *                                     depending on the flags argument.
 *                                     "profile_name" and other
 *                                     options from heap objects are ignored.
 *  @param[in]     heap                a list of device objects
 *  @param[in]     flags               - 0 yields exact match
 *                                     - 1 compare manufacturer model and serial
 *                                     - 2 compare only manufacturer and model
 *                                     - 4 compare only device_name
 *  @param[out]    matched_devices     the devices selected from heap
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/27 (Oyranos: 0.1.10)
 *  @date    2009/08/27
 */
OYAPI int OYEXPORT oyDeviceSelectSimiliar
                                     ( oyConfig_s        * pattern,
                                       oyConfigs_s       * heap,
                                       uint32_t            flags,
                                       oyConfigs_s      ** matched_devices )
{
  oyOption_s * odh = 0,
             * od = 0;
  int error  = !pattern || !matched_devices;
  char * od_key = 0;
  const char * od_val = 0,
             * odh_val = 0;
  oyConfig_s * s = pattern,
             * dh = 0;
  oyConfigs_s * matched = 0;
  int i,j,n,j_n;
  int match = 1;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error <= 0)
  {
    n = oyConfigs_Count( heap );

    /* Make shure the pattern has as well manufacturer, model included.
     * If not try a "properties" command. */
    if((flags == 0 || flags == 1 || flags == 2) &&
       (!oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0)))
    {
      oyOptions_s * options = 0;
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );
      oyDeviceBackendCall( s, options );
      oyOptions_Release( &options );
    }

    if((flags == 1 || flags == 2) &&
       (!oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0)))
    {
      return 0;
    }

    matched = oyConfigs_New( 0 );

    for(i = 0; i < n; ++i)
    {
      match = 0;
      dh = oyConfigs_Get( heap, i );

      j_n = oyConfig_Count( pattern );
      for(j = 0; j < j_n; ++j)
      {
        match = 1;
        od = oyConfig_Get( pattern, j );
        od_key = oyFilterRegistrationToText( oyOption_GetRegistration(od),
                                             oyFILTER_REG_MAX, 0);

        od_val = oyOption_GetValueString( od, 0 );
        if(!od_val)
          /* ignore non text options */
          continue;

        /* handle selective flags */
        if(flags == 4 &&
           oyStrcmp_(od_key,"device_name") != 0
          )
          continue;
        else
        if(flags == 2 &&
           oyStrcmp_(od_key,"manufacturer") != 0 &&
           oyStrcmp_(od_key,"model") != 0
          )
          continue;
        else
        if(flags == 1 &&
           oyStrcmp_(od_key,"manufacturer") != 0 &&
           oyStrcmp_(od_key,"model") != 0 &&
           oyStrcmp_(od_key,"serial") != 0
          )
          continue;

        /* ignore a "profile_name" option */
        if(oyStrcmp_(od_key,"profile_name") == 0)
          continue;

        odh = oyOptions_Find( dh->db, od_key );

        odh_val = oyOption_GetValueString( odh, 0 );
        if( !odh_val )
          /* ignore non text options */
          match = 0;

        if(match && oyStrcmp_( od_val, odh_val ) != 0)
          match = 0;

        /*printf("pruefe: %s=%s match = %d flags=%d\n", od_key, od_val, match, flags);*/


        oyOption_Release( &od );

        oyOption_Release( &odh );

        if(match == 0)
          break;
      }

      if(match)
        oyConfigs_MoveIn( matched, &dh, -1 );
      else
        oyConfig_Release( &dh );
    }

    if(oyConfigs_Count( matched ))
      *matched_devices = matched;
    else
      oyConfigs_Release( &matched );

  } else
    WARNc_S( "missed argument(s)" );

  return error;
}


/**
 *  @} *//* devices_handling
 */


/** \addtogroup objects_profile Profile API
 *
 *  To open a profile exist several methods in the oyProfile_Fromxxx APIs.
 *  oyProfile_FromStd(), oyProfile_FromFile() and oyProfile_FromMem() are basic
 *  profile open functions. oyProfile_FromSignature() is used for creating a
 *  dummy profile during profile filtering.
 *  oyProfile_FromMD5() is a lookup function from a profile hash, e.g. as
 *  provided by a 'psid' tag. \n
 *  Profile properties can be obtained from oyProfile_GetSignature(), like
 *  colour spaces, date, magic number and so on.
 *  oyProfile_GetChannelNames(), oyProfile_GetText() and oyProfile_GetID()
 *  provide additional informations. \n
 *  The profile element functions have Tag in their names. They work together
 *  with the oyProfileTag_s APIs. \n
 *  oyProfile_GetFileName is a reverse lookup to obtain the name of a installed
 *  file from a profile, e.g. find the name of a reachable display profile.

 *  @{
 */

static int oy_profile_first = 0;
static
const char * oyProfile_Message_      ( oyPointer           profile,
                                       int                 flags )
{
  oyStruct_s*s = profile;
  return oyProfile_GetText( (oyProfile_s*)s, oyNAME_DESCRIPTION );
}

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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  s->tags_ = oyStructList_Create( s->type_, 0, 0 );
  s->tags_modified_ = 0;

  if(oy_profile_first)
  {
    oy_profile_first = 1;
    oyStruct_RegisterStaticMessageFunc( oyOBJECT_PROFILE_S,
                                        oyProfile_Message_ );
  }

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

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(s->block_ && s->size_)
  {
    oyObject_HashSet( s->oy_, 0 );
    error = oyProfileGetMD5( s->block_, s->size_, s->oy_->hash_ptr_ );
    if(error)
      oyObject_HashSet( s->oy_, 0 );
  }
  return error;
}


oyProfile_s ** oy_profile_s_std_cache_ = 0;

/** @brief   create from default colour space settings
 *  @memberof oyProfile_s
 *
 *  @param[in]    type           default colour space
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
  else
  {
    if(strcmp("XYZ.icc",name) == 0 ||
       strcmp("Lab.icc",name) == 0 ||
       strcmp("LStar-RGB.icc",name) == 0 ||
       strcmp("sRGB.icc",name) == 0 ||
       strcmp("ISOcoated_v2_bas.ICC",name) == 0
      )
    {
      oyMessageFunc_p( oyMSG_ERROR,(oyStruct_s*)object,
                       OY_DBG_FORMAT_"\n\t%s: \"%s\"\n\t%s\n\t%s", OY_DBG_ARGS_,
                _("Could not open default ICC profile"),name,
                _("You can get them from http://sf.net/projects/openicc"),
                _("install in the OpenIccDirectory icc path") );
    } else
      oyMessageFunc_p( oyMSG_ERROR,(oyStruct_s*)object,
                       OY_DBG_FORMAT_"\n\t%s: \"%s\"\n\t%s", OY_DBG_ARGS_,
                _("Could not open default ICC profile"), name,
                _("install in the OpenIccDirectory icc path") );
  }

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
 *  @param[in]    name           profile file name
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  flags supports OY_NO_CACHE_READ and OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 *  @date    2010/05/18
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
      entry = oyCacheListGetEntry_ ( oy_profile_s_file_cache_, 0, name );

      if(!oyToNoCacheRead_m(flags))
      {
        s = (oyProfile_s*) oyHash_GetPointer( entry, oyOBJECT_PROFILE_S);
        s = oyProfile_Copy( s, 0 );
        if(s)
          return s;
      }
    }
  }

  if(error <= 0 && name && !s)
  {
    file_name = oyFindProfile_( name );
    block = oyGetProfileBlock( file_name, &size, allocateFunc );
    if(!block || !size)
      error = 1;
  }

  if(error <= 0)
  {
    s = oyProfile_FromMemMove_( size, &block, 0, object );

    if(!s)
      error = 1;

    /* We expect a incomplete filename attached to s and try to correct this. */
    if(error <= 0 && !file_name && s->file_name_)
    {
      file_name = oyFindProfile_( s->file_name_ );
      if(file_name && s->oy_->deallocateFunc_)
      {
        s->oy_->deallocateFunc_( s->file_name_ );
        s->file_name_ = 0;
      }
    }

    if(error <= 0 && file_name)
    {
      s->file_name_ = oyStringCopy_( file_name, s->oy_->allocateFunc_ );
      oyDeAllocateFunc_( file_name ); file_name = 0;
    }

    if(error <= 0 && !s->file_name_)
      error = 1;
  }

  if(error <= 0 && s && entry)
  {
    if(!oyToNoCacheWrite_m(flags))
    {
      /* 3b.1. update cache entry */
      error = oyHash_SetPointer( entry, (oyStruct_s*)s );
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

  oyHash_Release( &entry );

  return s;
}

/** @brief   create from file
 *  @memberof oyProfile_s
 *
 *  Supported are profiles with absolute path names, profiles in OpenICC 
 *  profile paths and profiles relative to the current working path. 
 *  Search will occure in this order.
 *
 *  @param[in]    name           profile file name
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  flags supports OY_NO_CACHE_READ and OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 *  @date    2010/05/18
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
    WARNc1_S( "%s", "no data" )
    return 0;
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

  if(error <= 0)
    error = oyProfile_GetHash_( s );

  if(error)
  {
    WARNc1_S( "hash error %d", error )
    return 0;
  }

  if(error <= 0)
    error = !oyProfile_GetSignature ( s, oySIGNATURE_COLOUR_SPACE );

  if(error)
  {
    WARNc1_S( "signature error %d", error )
    return 0;
  }

  if(error <= 0)
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

    WARNc3_S("Channels <= 0 %d %s %s", s->channels_n_,
             oyICCColourSpaceGetName(sig),
             oyICCColourSpaceGetName(h->colorSpace))
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
    oyAllocHelper_m_( block_, char, size, object ? object->allocateFunc_:0,
                      error = 1 );

    if(!error)
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

  if(error <= 0)
    error = oyProfile_SetSignature( s, sig, type );

  return s;
}

/** @brief   look up a profile from it's md5 hash sum
 *  @memberof oyProfile_s
 *
 *  @param[in]    md5            hash sum
 *  @param[in]    object         the optional base
 *  @return                      a profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/20 (Oyranos: 0.1.10)
 *  @date    2009/03/20
 */
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMD5(  uint32_t          * md5,
                                       oyObject_s          object )
{
  oyProfile_s * s = 0, * tmp = 0;
  int error = !md5,
      equal = 0;
  char ** names = 0;
  uint32_t count = 0, i = 0;

  if(error)
    return 0;

  if(error <= 0)
  {
    names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

    for(i = 0; i < count; ++i)
    {
      if(names[i])
      {
        if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          tmp = oyProfile_FromFile( names[i], 0, 0 );

        if(tmp->oy_->hash_ptr_)
          equal = memcmp( md5, tmp->oy_->hash_ptr_, OY_HASH_SIZE );
        else
          equal = 1;
        if(equal == 0)
          {
            s = tmp;
            break;
          }

          oyProfile_Release( &tmp );
        }
      }

      oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
  }

  return s;
}

/**
 *  @internal
 *  @brief   create new from existing profile struct
 *  @memberof oyProfile_s
 *
 *  @param[in]    profile        other profile
 *  @param[in]    object         the optional base
 *  @return                      the profile copy
 *
 *  @version Oyranos: 0.1.10
 *  @date    2010/06/03
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_Copy_                      ( const oyProfile_s * profile,
                                       oyObject_s          object)
{
  oyProfile_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc = 0;

  if(!profile)
    return s;

  {
    const oyProfile_s * s = profile;
    oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )
  }

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

  if(error <= 0)
  {
    if(!oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s );
  }

  if(error <= 0)
  {
    if(profile->sig_)
      s->sig_ = profile->sig_;
    else
      error = !oyProfile_GetSignature ( s, oySIGNATURE_COLOUR_SPACE );
  }

  if(error <= 0)
    s->file_name_ = oyStringCopy_( profile->file_name_, allocateFunc );

  if(error <= 0)
    s->use_default_ = profile->use_default_;

  if(error <= 0)
  {
    s->channels_n_ = oyProfile_GetChannelsCount( s );
    error = (s->channels_n_ <= 0);
  }

  if(error <= 0)
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
  oyProfile_s * s = profile;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  s = 0;

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

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

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
  oyProfile_s * s = profile;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(profile->channels_n_)
    return profile->channels_n_;

  profile->channels_n_ = oyICCColourSpaceGetChannelCount( profile->sig_ );

  return profile->channels_n_;
}

/** @brief   get ICC colour space signature
 *  @memberof oyProfile_s
 *
 *  \verbatim
    // show some profile properties
    oyProfile_s * p = ...; // get from somewhere
    icSignature vs = oyValueUInt32( oyProfile_GetSignature(p,oySIGNATURE_VERSION) );      
    char * v = (char*)&vs;
    printf("  created %d.%d.%d %d:%d:%d\n", 
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_YEAR),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MONTH),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_DAY),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_HOURS),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MINUTES),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_SECONDS)
          );
    printf("  pcs: %s  colour space: %s version: %d.%d.%d\n", 
          oyICCColourSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_PCS) ),
          oyICCColourSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_COLOUR_SPACE) ),
          (int)v[0], (int)v[1]/16, (int)v[1]%16
          );
    \endverbatim
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

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

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
  int error = !s;
  oyPointer block_ = 0;
  size_t size_ = 128;
  icHeader *h = 0;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(error <= 0 && type == oySIGNATURE_COLOUR_SPACE)
  {
    if(sig)
      s->sig_ = sig;
    else
      s->sig_ = icSigXYZData;
  }

  if(error <= 0 && !s->block_)
  {
    oyAllocHelper_m_( block_, char, size_, s->oy_ ? s->oy_->allocateFunc_:0,
                      error = 1 );

    if(error <= 0)
    {
      s->block_ = block_;
      s->size_ = size_;
    }
  }

  if(error <= 0)
    h = (icHeader*) s->block_;

  if(error <= 0)
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
  oyProfile_s * s = profile;
  int n = oyProfile_GetChannelsCount( profile );
  int error = !s;

  if(error)
    return;

  oyCheckType__m( oyOBJECT_PROFILE_S, return )

  if(names_chan && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    s->names_chan_[ n ] = NULL;
    for( ; i < n; ++i )
      if(names_chan[i])
        s->names_chan_[i] = oyObject_Copy( names_chan[i] );
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

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

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

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!s->names_chan_ && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    if(!s->names_chan_)
      error = 1;
    if(error <= 0)
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

  if(error <= 0 && s->names_chan_)
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

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
    text = oyObject_GetName( s->oy_, oyNAME_NICK );

  if(error <= 0 && !text)
  {
    char * temp = 0;
    int found = 0;

    oyAllocHelper_m_( temp, oyChar, 1024, 0, error = 1 );

    /* A short number represents a default profile,
     * The length should not exceed OY_HASH_SIZE.
     */
    if(s->use_default_ && !found && error <= 0)
    {
      oySprintf_(temp, "%d", s->use_default_);
      if(oyStrlen_(temp) < OY_HASH_SIZE)
        found = 1;
    }

    /* Do we have a file_name_? */
    if(s->file_name_ && !found && error <= 0)
    {
      oySprintf_(temp, "%s", s->file_name_);
      if(temp[0])
        found = 1;
    }

    /* Do we have a hash_? */
    if(!found && error <= 0)
    {
      if(!oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s );

      if(error <= 0)
      {
        uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
        if(i)
          oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
        else
          oySprintf_(temp, "                " );
        if(temp[0])
          found = 1;
      }
    }

    if(error <= 0 && !found)
      error = 1;

    if(error <= 0)
      error = oyObject_SetName( s->oy_, temp, oyNAME_NICK );

    oyFree_m_( temp );

    if(error <= 0)
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

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && type == oyNAME_NICK)
    text = oyProfile_GetID( s );

  if(error <= 0 && !text)
    if(type <= oyNAME_DESCRIPTION)
      text = oyObject_GetName( s->oy_, type );

  if(error <= 0 && !(text && text[0]))
  {
    char * temp = 0,
         * tmp2 = 0;
    int found = 0;

    oyAllocHelper_m_( temp, char, 1024, 0, error = 1 );

    /* Ask the CMM? */
    if(!found && error <= 0 &&
       type == oyNAME_DESCRIPTION)
    {
      {
        tag = oyProfile_GetTagById( s, icSigProfileDescriptionTag );
        texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);

        if(texts_n && texts[0] && texts[0][0])
        {
          memcpy(temp, texts[0], oyStrlen_(texts[0]));
          temp[oyStrlen_(texts[0])] = 0;
          found = 1;

          oyStringListRelease_( &texts, texts_n, tag->oy_->deallocateFunc_ );
        } else
          /* we try to get something as oyNAME_NAME */
        if(s->file_name_ && s->file_name_[0])
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
      uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
      char * file_name = oyProfile_GetFileName_r( s, oyAllocateFunc_ );

      if(oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s );

      if(s->use_default_ && error <= 0)
        oyWidgetTitleGet( (oyWIDGET_e)s->use_default_, 0, &text, 0, 0 );

      oySprintf_( temp, "<profile use_default=\"%s\" file_name=\"%s\" hash=\"",
             oyNoEmptyName_m_(text),
             oyNoEmptyName_m_(file_name) );
      if(i)
        oySprintf_( &temp[oyStrlen_(temp)], "%x%x%x%x\" />",
             i[0], i[1], i[2], i[3] );
      else
        oySprintf_( &temp[oyStrlen_(temp)], "                \" />" );

      if(file_name) free(file_name); file_name = 0;
      found = 1;
    }

    if(!found)
    {
      text = oyProfile_GetID( s );
      if(text[0])
        found = 1;
    }

    /* last rescue */
    if(!found && oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s );

    if(!found && error <= 0)
    {
      uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
      oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
      if(temp[0])
        found = 1;
    }

    if(error <= 0 && !found)
      error = 1;

    if(error <= 0)
      error = oyObject_SetName( s->oy_, temp, type );

    oyFree_m_( temp );

    if(error <= 0)
      text = oyObject_GetName( s->oy_, type );
  }

  return text;
}

/** Function oyProfile_GetMem
 *  @memberof oyProfile_s
 *  @brief   get the ICC profile in memory
 *
 *  The prefered memory comes from the unmodified original memory.
 *  Otherwise a previously modified tag list is serialised into memory.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 *  @date    2010/04/16
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

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(error <= 0 && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(s->size_ && s->block_ && !s->tags_modified_)
    {

      block = oyAllocateWrapFunc_( s->size_, allocateFunc );
      error = !block;
      if(error <= 0)
        error = !memcpy( block, s->block_, s->size_ );
      if(error <= 0 && size)
        *size = s->size_;

    } else
    if( oyStructList_Count( s->tags_ ))
    {
      block = oyProfile_TagsToMem_ ( profile, size, allocateFunc );
      profile->tags_modified_ = 0;
      profile->use_default_ = 0;
      profile->oy_->deallocateFunc_( profile->file_name_ );
      profile->file_name_ = 0;
      oyProfile_GetHash_(profile);
      oyObject_SetNames( profile->oy_, 0,0,0 );
      oyProfile_GetText(profile, oyNAME_NICK);
      oyProfile_GetText(profile, oyNAME_NAME);
      oyProfile_GetText(profile, oyNAME_DESCRIPTION);
    }
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return block;
}

/** Function oyProfile_GetMD5
 *  @memberof oyProfile_s
 *  @brief   get the ICC profile md5 hash sum
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/30 (Oyranos: 0.3.0)
 *  @date    2011/01/30
 */
int                oyProfile_GetMD5  ( oyProfile_s       * profile,
                                       uint32_t          * md5 )
{
  oyProfile_s * s = profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!oyProfile_Hashed_(s))
    error = oyProfile_GetHash_( s );

  if(oyProfile_Hashed_(s))
    memcpy( md5, s->oy_->hash_ptr_, OY_HASH_SIZE );
  else
    error += 1;

  return error;
}

/**
 *  @internal
 *  Function oyProfile_GetFileName_r
 *  @memberof oyProfile_s
 *  @brief   get the ICC profile location in the filesystem
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or 
 *  from a Oyranos default profile, the associated filename will simply be
 *  retuned.
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
  char ** names = 0;
  uint32_t count = 0, i = 0;
  char *  hash = 0;
  char    tmp_hash[34];

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
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
            uint32_t * h = (uint32_t*)s->oy_->hash_ptr_;
            if(h)
              oySprintf_(tmp_hash, "%x%x%x%x", h[0], h[1], h[2], h[3]);
            else
              oySprintf_(tmp_hash, "                " );
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
      if(name)
      {
        s->file_name_ = oyStringCopy_( name, s->oy_->allocateFunc_ );
        oyDeAllocateFunc_( name );
      }
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
 *  @param         dl_pos              -1, or the position in a device links
 *                                     source chain
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
const char *       oyProfile_GetFileName (
                                       oyProfile_s       * profile,
                                       int                 dl_pos )
{
  const char * name = 0;
  oyProfile_s * s = profile, * tmp = 0;
  int error = !s;
  char ** names = 0;
  uint32_t count = 0, i = 0;
  oyProfileTag_s * psid = 0;
  char ** texts = 0;
  int32_t   texts_n = 0;
  char *  hash = 0,
       *  txt = 0;
  char    tmp_hash[34];
  int       dl_n = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
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

          if(tmp)
          {
            if(hash)
            {
              uint32_t * h = (uint32_t*)tmp->oy_->hash_ptr_;
              if(h)
                oySprintf_(tmp_hash, "%x%x%x%x", h[0], h[1], h[2], h[3]);
              else
                oySprintf_(tmp_hash, "                " );
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
      }

      if(hash)
      {
        char * key = oyAllocateFunc_(80);
        txt = oyFindProfile_( name );
        sprintf( key, "//imaging/profile.icc/psid_%d", dl_pos );
        oyOptions_SetFromText( &s->oy_->handles_,
                               key,
                               txt,
                               OY_CREATE_NEW );
        oyDeAllocateFunc_( txt );
        name = oyOptions_FindString( s->oy_->handles_,
                                     key, 0 );
        oyFree_m_( key );
      } else
      {
        s->file_name_ = oyFindProfile_( name );
        name = oyStringCopy_( s->file_name_, s->oy_->allocateFunc_ );
        if(s->file_name_)
          oyDeAllocateFunc_( s->file_name_ );
        s->file_name_ = (char*)name;
      }

      if(names)
        oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }

    if(texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  return name;
}

/**
 *  Function oyProfile_DeviceAdd
 *  @memberof oyProfile_s
 *  @brief   add device and driver informations to a profile
 *
 *  oyProfile_DeviceAdd() is for storing device/driver informations in a 
 *  ICC profile. So the profile can be sent over internet and Oyranos, or 
 *  an other CMS, can better match to a device/driver on the new host.
 *  The convention what to place into the ICC profile is dependent on each
 *  device class and its actual driver or driver type.
 *  The meta data is stored in the ICC 'meta' tag of type 'dict'.
 *
 *  @param[in,out] profile             the profile
 *  @param[in]     device              device and driver informations
 *  @param[in]     options             - "key_prefix_required" : prefix
 *                                       accept only key names with the prefix
 *
 *  @version Oyranos: 0.1.13
 *  @since   2009/05/18 (Oyranos: 0.1.10)
 *  @date    2010/11/09
 */
#if 0
TODO find a general form. Do we want to support the mluc type or is that better
up to a specialised GUI?
int                oyProfile_DeviceAdd(oyProfile_s       * profile,
                                       oyConfig_s        * device )
{
  int error = !profile;
  oyProfile_s * s = profile;
  oyProfileTag_s * pddt = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
      pddt = oyProfile_GetTagById( s, icSigProfileDetailDescriptionTag_ );

      /* icSigProfileDetailDescriptionTag_ */
      if(error <= 0 && !pddt)
      {
        oyStructList_s * list = 0;

        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list, (oyStruct_s**) &device, 0,
                                     OY_OBSERVE_AS_WELL );

        if(error <= 0)
        {
          pddt = oyProfileTag_Create( list, icSigProfileDetailDescriptionTag_,
                                      0, OY_MODULE_NICK, 0);
          error = !pddt;
        }

        if(error <= 0)
          pddt->use = icSigProfileDetailDescriptionTag_;

        oyStructList_Release( &list );

        if(pddt)
        {
          error = oyProfile_TagMoveIn_( s, &pddt, -1 );
          ++s->tags_modified_;
        }
      }
  }

  return error;
}
#else
int                oyProfile_DeviceAdd(oyProfile_s       * profile,
                                       oyConfig_s        * device,
                                       oyOptions_s       * options )
{
  int error = 0;
  int i, len, size, block_size, pos;

  char ** keys,
       ** values,
        * key;
  const char * val,
             * r;
  void * string = 0;
  const char * key_prefix_required = oyOptions_FindString( options,
                                            "key_prefix_required", 0 );
  int key_prefix_required_len = 0;

  /* get just some device */
  oyOption_s * o = 0;
  oyConfig_s * d = device;

  oyProfile_s * p = profile;
  oyProfileTag_s * dict_tag;
  icDictTagType * dict;
  icNameValueRecord * record;

  int n = oyConfig_Count( d );
  int count = 0;

  if(key_prefix_required)
    key_prefix_required_len = strlen(key_prefix_required);

  /* count valid entries */
  for(i = 0; i < n; ++i)
  {
    char * reg = 0;
    o = oyConfig_Get( d, i );
    r = oyOption_GetRegistration(o);
    reg = oyFilterRegistrationToText( r, oyFILTER_REG_OPTION, oyAllocateFunc_ );
    val = oyOption_GetValueString( o, 0 );
    if(val)
    {
      int pass = 1;

      if(key_prefix_required)
      {
        int len = strlen( reg );
        pass = 0;
        if(len > key_prefix_required_len &&
           memcmp( key_prefix_required, reg, key_prefix_required_len) == 0)
          pass = 1;
      }

      if(val && pass)
      {
        DBG_PROG2_S("%s: %s", reg, val );
        ++count;
      }
    }
    if(reg) oyDeAllocateFunc_(reg);
  }

  /* collect data */
  size = 16 /* or 24 or 32*/
         * n + sizeof(icDictTagType),
  block_size = size;
  pos = 0;

  keys = oyAllocateFunc_( 2 * count * sizeof(char*));
  values = oyAllocateFunc_( 2 * count * sizeof(char*));
  for(i = 0; i < n; ++i)
  {
    o = oyConfig_Get( d, i );
    r = oyOption_GetRegistration(o);
    key = oyFilterRegistrationToText( r, oyFILTER_REG_OPTION,
                                             oyAllocateFunc_ );
    val = oyOption_GetValueString( o, 0 );
    if(val)
    {
      int pass = 1;

      if(key_prefix_required)
      {
        int len = strlen( key );
        pass = 0;
        if(len > key_prefix_required_len &&
           memcmp( key_prefix_required, key, key_prefix_required_len) == 0)
          pass = 1;
      }

      if(pass)
      {
        keys[pos] = key;
        values[pos] = oyStringCopy_(val,oyAllocateFunc_);
        DBG_PROG2_S("%s: %s", key, val );
        block_size += (strlen(key) + strlen(val)) * 2 + 2;
        ++pos;
        key = 0;
      }
    }
    if(key) oyDeAllocateFunc_( key ); key = 0;
  }

  dict = calloc(sizeof(char), block_size);
  dict->sig = oyValueUInt32( icSigDictType );
  dict->number = oyValueUInt32( count );
  dict->size = oyValueUInt32( 16 );

  pos = size;
  for(i = 0; i < count; ++i)
  {
    record = (icNameValueRecord*)((char*)dict + sizeof(icDictTagType) + 16 * i);

    len = 0;
    string = NULL;
    error = oyIconvGet( keys[i], &string, &len, "UTF-8", "UTF-16BE",
                        oyAllocateFunc_ );
    record->name_string_offset = oyValueUInt32( pos );
    len = strlen( keys[i] ) * 2;
    len = len + (len%4 ? 4 - len%4 : 0);
    record->name_string_size =  oyValueUInt32( len );
    memcpy(((char*)dict)+pos, string, len );
    pos += len;

    len = 0;
    string = NULL;
    error = oyIconvGet( values[i], &string, &len, "UTF-8", "UTF-16BE", 
                        oyAllocateFunc_ );
    record->value_string_offset =  oyValueUInt32( pos );
    len = strlen( values[i] ) * 2;
    len = len + (len%4 ? 4 - len%4 : 0);
    record->value_string_size =  oyValueUInt32( len );
    memcpy(((char*)dict)+pos, string, len );
    pos += len;
  }

  dict_tag = oyProfileTag_New(NULL);
  error = oyProfileTag_Set( dict_tag, icSigMetaDataTag, icSigDictType,
                            oyOK, block_size, dict );
  error = oyProfile_TagMoveIn( p, &dict_tag, -1 );

  oyStringListRelease_( &keys, count, oyDeAllocateFunc_ );
  oyStringListRelease_( &values, count, oyDeAllocateFunc_ );

  return 0;
}
#endif

/** Function oyProfile_DeviceGet
 *  @memberof oyProfile_s
 *  @brief   obtain device informations from a profile
 *
 *  @verbatim
    oyConfig_s * device = oyConfig_New( "//" OY_TYPE_STD "/config", object );
    oyProfile_DeviceGet( profile, device ); @endverbatim
 *
 *  @param[in]     profile             the profile
 *  @param[in,out] device              the device description
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2010/10/26
 */
int              oyProfile_DeviceGet ( oyProfile_s       * profile,
                                       oyConfig_s        * device )
{
  int error = !profile, l_error = 0;
  oyProfile_s * s = profile;
  oyProfileTag_s * tag = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  int i,
      dmnd_found = 0, dmdd_found = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!error)
  {
    tag = oyProfile_GetTagById( s, icSigMetaDataTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts_n > 0)
      for(i = 2; i+1 < texts_n && error <= 0; i+=2)
      {
        if(strcmp(texts[i+0],"model") == 0) dmdd_found = 1;
        if(strcmp(texts[i+0],"manufacturer") == 0) dmnd_found = 1;

        error = oyOptions_SetRegistrationTextKey_( device->backend_core,
                                                 device->registration,
                                                 texts[i+0], texts[i+1] );
      }
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( s, icSigDeviceModelDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmdd_found)
      error = oyOptions_SetRegistrationTextKey_( device->backend_core, 
                                                 device->registration,
                                                 "model", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( s, icSigDeviceMfgDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmnd_found)
      error = oyOptions_SetRegistrationTextKey_( device->backend_core, 
                                                 device->registration,
                                                 "manufacturer", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  l_error = oyOptions_SetSource( device->backend_core,
                                     oyOPTIONSOURCE_FILTER); OY_ERR

  return error;
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

  if(error <= 0)
  {
    oyCMMProfile_GetText_t funcP = 0;
    oyPointer_s  * cmm_ptr = 0;


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
 *  @since   2008/02/03 (Oyranos: 0.1.8)
 *  @date    2009/04/16
 */
OYAPI int OYEXPORT
                   oyProfile_Equal   ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB )
{
  int equal = 0;

  if(profileA && profileB)
    equal = oyObject_HashEqual( profileA->oy_, profileB->oy_ );

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

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(error <= 0)
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
    int n = oyProfile_GetTagCount_( profile );
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( profile, 0 );

    if(n && tag->use == *hi && tag->block_ && tag->size_ >= 128)
    {
      block = oyAllocateFunc_ (132);
      if(block)
      {
        memset( block, 0, 132 );
        memcpy( block, tag->block_, 128 );
      }
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

  if(error <= 0)
  {
    int n = oyProfile_GetTagCount_( profile );
    size_t size = 0;

    size = sizeof (icTag) * n;
    error = !size;

    if(error <= 0)
    {
      block = oyAllocateFunc_( size );
      error = !block;
    }

    if(error <= 0)
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

  if(error <= 0)
  {
    int n = 0, i, j;
    size_t len = 0;

    n = oyProfile_GetTagCount_( profile );
    block = (char*) oyAllocateFunc_(132 + n * sizeof(icTag));
    error = !block;

    if(error <= 0)
    {
      memset( block, 0, 132 + n * sizeof(icTag) );
      error = !memcpy( block, icc_header, 132 );
    }

    len = 132;

    if(error <= 0)
      error = !memcpy( &block[len], icc_list, (n-1) * sizeof(icTag) );

    len += sizeof(icTag) * (n-1);

    /* write tags */
    for(i = 0; i < n - 1; ++i)
    {
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char * temp = 0;
      icTagList* list = (icTagList*) &block[128];
      oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( profile, i + 1 );
      size_t size = 0;
      uint32_t dup_offset = 0;

      if(error <= 0)
        error = !tag;

      if(error <= 0)
        size = tag->size_;

      if(error <= 0 && tag->use == *hi)
      {
        oyProfileTag_Release( &tag );
        continue;
      }

      /* detect duplicate tag contents */
      for(j = 0; j < i; ++j)
      {
        oyProfileTag_s * comp = oyProfile_GetTagByPos_( profile, j + 1 );
        if(tag->size_ == comp->size_ &&
           tag->offset_orig == comp->offset_orig &&
           memcmp(tag->block_, comp->block_, tag->size_) == 0)
          dup_offset = list->tags[j].offset;
      }

      if(error <= 0)
      {
        list->tags[i].sig = oyValueUInt32( (icTagSignature)tag->use );
        if(dup_offset)
          list->tags[i].offset = dup_offset;
        else
        {
          list->tags[i].offset = oyValueUInt32( (icUInt32Number)len );
          temp = (char*) oyAllocateFunc_ ( len + size + 
                                               (size%4 ? 4 - size%4 : 0));
        }
        list->tags[i].size = oyValueUInt32( (icUInt32Number)size );
        if(temp)
          memset( temp, 0, len + size + (size%4 ? 4 - size%4 : 0));
      }

      if(temp)
        error = !memcpy( temp, block, len );
      if(temp && error <= 0)
      {
        error = !memcpy( &temp[len], tag->block_, tag->size_);
        len += size + (size%4 ? 4 - size%4 : 0);
      }

      if(temp && error <= 0)
      {
        oyDeAllocateFunc_(block);
        block = temp;
      }
      if(error <= 0)
      {
        oyProfileTag_Release( &tag );
      }
      temp = 0;
    }

    /* modify header, e.g. size, platform signature  */
    if(error <= 0)
    {
      char h[5] = {OY_MODULE_NICK};
      uint32_t * hi = (uint32_t*)&h;
      icProfile* p = 0;
      icHeader* header = 0;
      oyPointer temp = oyAllocateWrapFunc_( len, allocateFunc );

      error = !temp;
      if(error <= 0)
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

  if(error <= 0)
  {
    size_t size_ = 0;

    oyPointer icc_header = 0;
    oyPointer icc_tagtable = 0;

    /* 1. header */
    icc_header = oyProfile_WriteHeader_( profile, &size_ );

    error = !icc_header;

    /* 2. tag table */
    if(error <= 0)
    {
      icc_tagtable = oyProfile_WriteTagTable_( profile, &size_ );
      error = !icc_tagtable;
    }

    /* 3. tags */
    if(error <= 0)
    {
      block = oyProfile_WriteTags_( profile, &size_, icc_header, icc_tagtable,
                                    allocateFunc );
      error = !block;
    }

    if(error <= 0)
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

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
    n = oyProfile_GetTagCount_( s );

  if(error <= 0 && n)
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


/** @internal
 *  Function oyProfile_GetTagByPos_
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
  oyProfile_s * s = profile;
  int error = !profile;
  int n = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && profile->type_ != oyOBJECT_PROFILE_S)
    error = 1;

  if(error <= 0)
  {
    s = profile;
    n = oyStructList_Count( profile->tags_ );
  }

  if(error <= 0 && n)
  {
    tag = (oyProfileTag_s*) oyStructList_GetRef( profile->tags_, pos );
    return tag;
  }

  /* parse the ICC profile struct */
  if(error <= 0 && s->block_)
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

    if(error <= 0 && s->size_ > min_icc_size)
    {
      uint32_t size = 0;
      uint32_t tag_count = 0;
      icTag *tag_list = 0;
      int i = 0;
      oyProfileTag_s * tag_ = oyProfileTag_New( 0 );
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char *tag_block = 0;

      oyAllocHelper_m_( tag_block, char, 132, 0, return 0 );
      error = !memcpy( tag_block, s->block_, 132 );
      error = oyProfileTag_Set( tag_, (icTagSignature)*hi,
                                (icTagTypeSignature)*hi,
                                oyOK, 132, tag_block );
      if(error <= 0)
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

          oyAllocHelper_m_( tag_block, char, tag_size, 0, return 0 );
          tmp = &((char*)s->block_)[offset];
          error = !memcpy( tag_block, tmp, tag_size );

          tag_base = (icTagBase*) tag_block; 
          tag_type = oyValueUInt32( tag_base->sig );
        }

        error = oyProfileTag_Set( tag_, sig, tag_type,
                                  status, tag_size, tag_block );
        tag_->offset_orig = offset;
        if(error <= 0)
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

        if(error <= 0)
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

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  tag = oyProfile_GetTagByPos_( s, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return tag;
}

/** |internal
 *  Function oyProfile_GetTagCount_
 *  @memberof oyProfile_s
 *
 *  non thread save
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int                oyProfile_GetTagCount_ (
                                       oyProfile_s       * profile )
{
  int n = 0;
  oyProfile_s *s = profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && !s->tags_)
    error = 1;

  if(error <= 0)
    n = oyStructList_Count( s->tags_ );

  if(error <= 0 && !n)
  {
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );
  }

  return n;
}

/** Function oyProfile_GetTagCount
 *  @memberof oyProfile_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int                oyProfile_GetTagCount( oyProfile_s    * profile )
{
  int n = 0;
  oyProfile_s *s = profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && !s->tags_)
    error = 1;

  if(error <= 0)
    n = oyStructList_Count( s->tags_ );

  if(error <= 0 && !n)
  {
    oyProfileTag_s * tag = 0;
    if(s)
      oyObject_Lock( s->oy_, __FILE__, __LINE__ );

    tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );

    if(s)
      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
  }

  return n;
}

/** @internal
 *  Function oyProfile_TagMoveIn_
 *  @memberof oyProfile_s
 *  @brief   add a tag to a profile
 *
 *  non thread save
 *
 *  The profile is needs probably be marked as modified after calling this
 *  function.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/12/29
 */
int          oyProfile_TagMoveIn_    ( oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  int error = !s;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(error <= 0)
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos,
                                  OY_OBSERVE_AS_WELL );

  return error;
}

/** Function oyProfile_TagMoveIn
 *  @memberof oyProfile_s
 *  @brief   add a tag to a profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int                oyProfile_TagMoveIn(oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  int error = !s, i,n;
  oyProfileTag_s * tag = 0;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );


  if(error <= 0)
  {
    /** Initialise tag list. */
    n = oyProfile_GetTagCount_( s );

    /** Avoid double occurencies of tags. */
    for( i = 0; i < n; ++i )
    {
      tag = oyProfile_GetTagByPos_( s, i );
      if(tag->use == (*obj)->use)
      {
        oyProfile_TagReleaseAt_(s, i);
        n = oyProfile_GetTagCount_( s );
      }
      oyProfileTag_Release( &tag );
    }
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos,
                                  OY_OBSERVE_AS_WELL );
    ++s->tags_modified_;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** @internal
 *  Function oyProfile_TagReleaseAt_
 *  @memberof oyProfile_s
 *  @brief   remove a tag from a profile
 *
 *  non thread save
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/03/11 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int          oyProfile_TagReleaseAt_ ( oyProfile_s       * profile,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  return oyStructList_ReleaseAt ( s->tags_, pos );
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
  int error = !s;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(s && s->type_ == oyOBJECT_PROFILE_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(error <= 0)
  {
    error = oyStructList_ReleaseAt ( s->tags_, pos );
    ++s->tags_modified_;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function oyProfile_AddTagText
 *  @memberof oyProfile_s
 *  @brief   add a text tag
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/10/18
 *  @since   2009/10/18 (Oyranos: 0.1.10)
 */
int                oyProfile_AddTagText (
                                       oyProfile_s       * profile,
                                       icSignature         signature,
                                       const char        * text )
{
  oyStructList_s * list = 0;
  oyName_s * name = oyName_new(0);
  int error = 0;
  oyProfileTag_s * tag = 0;
  icTagTypeSignature tt = icSigTextType;
  icSignature vs = oyValueUInt32( oyProfile_GetSignature( profile,
                                                         oySIGNATURE_VERSION) );
  char * v = (char*)&vs;
  int version_A = (int)v[0]/*,
      version_B = (int)v[1]/16,
      version_C =  (int)v[1]%16*/;

  if(version_A <= 3 &&
     (signature == icSigProfileDescriptionTag ||
      signature == icSigDeviceMfgDescTag ||
      signature == icSigDeviceModelDescTag ||
      signature == icSigScreeningDescTag ||
      signature == icSigViewingCondDescTag))
    tt = icSigTextDescriptionType;
  else if(version_A >= 4 &&
     (signature == icSigProfileDescriptionTag ||
      signature == icSigDeviceMfgDescTag ||
      signature == icSigDeviceModelDescTag ||
      signature == icSigCopyrightTag ||
      signature == icSigViewingCondDescTag))
    tt = icSigMultiLocalizedUnicodeType;

  name = oyName_set_ ( name, text, oyNAME_NAME,
                       oyAllocateFunc_, oyDeAllocateFunc_ );
  list = oyStructList_New(0);
  error = oyStructList_MoveIn( list, (oyStruct_s**) &name, 0, 0 );

  if(!error)
  {
    tag = oyProfileTag_Create( list, tt, 0,OY_MODULE_NICK, 0);
    error = !tag;
  }

  if(!error)
    tag->use = signature;

  oyStructList_Release( &list );

  if(tag)
    error = oyProfile_TagMoveIn ( profile, &tag, -1 );

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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0 && list->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  {
    query.value = tag_type;
    query_[0] = &query;
    queries.queries = query_;
    if(required_cmm)
      error = !memcpy( queries.prefered_cmm, required_cmm, 4 ); 

    if(error <= 0 && required_cmm)
      error = !memcpy( cmm, required_cmm, 4 );
  }

  if(error <= 0)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, cmm, 0,
                                      oyCMMapi3_Query_, &queries );
    if(api)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
      funcP = api3->oyCMMProfileTag_Create;
    }
    error = !funcP;
  }

  if(error <= 0)
  {
    tag = oyProfileTag_New( object );
    error = !tag;
    
    if(error <= 0)
      error = funcP( tag, list, tag_type, version );

    if(error <= 0)
      error = !memcpy( tag->last_cmm_, cmm, 4 );
    if(error <= 0)
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

  if(error <= 0)
  {
    name = oyName_set_ ( name, text, oyNAME_NAME,
                         oyAllocateFunc_, oyDeAllocateFunc_ );
    error = !name;
  }

  if(error <= 0)
  {
    memcpy( name->lang, "en_GB", 5 );
    list = oyStructList_New(0);
    error = oyStructList_MoveIn( list, (oyStruct_s**) &name, 0,
                                 OY_OBSERVE_AS_WELL );
  }

  if(error <= 0)
  {
    tag = oyProfileTag_Create( list, tag_type, 0, OY_MODULE_NICK, object);
    error = !tag;
  }

  if(error <= 0)
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

  if(error <= 0 && !object && obj->oy_)
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

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

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
 *  The function is a simple setter for the object elements.
 *
 *  @param[in,out] tag                 the to be manipulated ICC profile object
 *  @param[in]     sig                 usage signature
 *  @param[in]     type                content type
 *  @param[in]     status              to be set
 *  @param[in]     tag_size            memory size of tag_block
 *  @param[in]     tag_block           the block to be moved into the object;
 *                                     The pointer is owned by the object. Its
 *                                     memory should be allocated as with the
 *                                     same allocators as the object.
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/11/06
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

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  if(error <= 0)
  {
    s->use = sig;
    s->tag_type_ = type;
    s->status_ = status;
    s->size_ = tag_size;
    if(s->block_)
      s->oy_->deallocateFunc_( s->block_ );
    s->block_ = tag_block;
  }

  return error;
}

/** @internal
 *  Function oyCMMapi3_Query_
 *
 *  implements oyCMMapi_Check_f
 *  The data argument is expected to be oyCMMapiQueries_s.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/02 (Oyranos: 0.1.10)
 *  @date    2009/09/02
 */
oyOBJECT_e   oyCMMapi3_Query_        ( oyCMMInfo_s       * cmm_info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank )
{
  oyCMMapiQueries_s * queries = data;
  uint32_t rank_ = 0;
  int prefered = 1;
  oyCMMapi3_s * api3 = 0;

  if(api->type == oyOBJECT_CMM_API3_S)
    api3 = (oyCMMapi3_s*) api;

  if(memcmp( queries->prefered_cmm, cmm_info->cmm, 4 ) == 0)
    prefered = 10;

  rank_ = oyCMMCanHandle_( api3, queries );

  if(rank)
    *rank = rank_ * prefered;

  if(rank_)
    return api->type;
  else
    return oyOBJECT_NONE;
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
 *  For localised strings, e.g. icSigMultiLocalizedUnicodeType: \n
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
       t_l[8] = {0,0,0,0,0,0,0,0}, t_c[8] = {0,0,0,0,0,0,0,0}, *t_ptr;
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

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  if(error <= 0)
  {
    query.value = tag->tag_type_;
    query_[0] = &query;
    queries.queries = query_;
    error = !memcpy( queries.prefered_cmm, tag->profile_cmm_, 4 ); 

    if(error <= 0)
      error = !memcpy( cmm, tag->required_cmm, 4 );
  }

  if(error <= 0)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, cmm, 0,
                                      oyCMMapi3_Query_, &queries );
    if(api)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
      funcP = api3->oyCMMProfileTag_GetValues;
    }
    error = !funcP;
  }

  if(error <= 0)
  {
    /* check for a "" in the lang variable -> want the best i18n match */
    if(language && !language[0])
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
            else if(blob && oyBlob_GetPointer(blob) && oyBlob_GetSize(blob))
            {
              error = oyStringFromData_( oyBlob_GetPointer(blob),
                                         oyBlob_GetSize(blob), &text_tmp,
                      &size, oyAllocateFunc_ );
              if(error <= 0 && size && text_tmp)
                text = text_tmp;
            }

            /* select by language and/or country or best i18n match or all */
            if(
               (k == 0 && language && language[0] &&
                          oyStrcmp_( language, t_l ) == 0 &&
                          country  && country[0] &&
                          oyStrcmp_( country, t_c ) == 0 )              ||
               (k == 1 && language && language[0] &&
                          oyStrcmp_( language, t_l ) == 0 &&
                          (!country || implicite_i18n ))                ||
               (k == 2 && country  && country[0] &&
                          oyStrcmp_( country, t_c ) == 0  &&
                          (!language || implicite_i18n ))               ||
               (k == 3 && ((!language && !country) || implicite_i18n))
              )
            {
              if(name && name->lang[0] && !implicite_i18n)
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

/** Function oyProfileTag_GetBlock
 *  @memberof oyProfileTag_s
 *
 *  Get the raw memory block of the tag.
 *
 *  @param[in]     tag                 the tag to read
 *  @param[out]    tag_block           the raw data owned by the user; on success the block if it has a size; else undefined
 *  @param[out]    tag_size            the data size; mandatory arg; on success the size returned in tag_block else undefined
 *  @param[in]     allocateFunc        the user allocator
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.2.0
 *  @since   2010/01/31 (Oyranos: 0.2.0)
 *  @date    2010/06/31
 */
int            oyProfileTag_GetBlock ( oyProfileTag_s    * tag,
                                       oyPointer         * tag_block,
                                       size_t            * tag_size,
                                       oyAlloc_f           allocateFunc )
{
  oyProfileTag_s * s = tag;
  int error = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  if(error <= 0)
  {
    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    if(tag->size_ && tag->block_ && tag_block)
    {
      *tag_block = allocateFunc( tag->size_ + 1 );
      memcpy( *tag_block, tag->block_, tag->size_ );
    }
    if(tag_size)
      *tag_size = tag->size_;
  }

  return error;
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/22 (Oyranos: 0.1.8)
 *  @date    2011/01/28
 */
OYAPI oyProfiles_s * OYEXPORT
                   oyProfiles_Copy  ( oyProfiles_s * obj,
                                         oyObject_s        object)
{
  oyProfiles_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc = 0;

  if(!obj)
    return 0;

  s = obj;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  s = oyProfiles_New( object );
  error = !s;

  if(error <= 0)
    allocateFunc = s->oy_->allocateFunc_;

  if(error <= 0)
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

  if(error <= 0)
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

            oyProfile_Release( &pattern );
          }

        } else {

          s = oyProfiles_MoveIn( s, &tmp, -1);
          error = !s;
        }

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
 *  oyASSUMED_WEB will result in exactly one profile added as long as it is
 *  available in the file paths.
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
             oyProfile_GetFileName(temp_prof, -1));
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

    if(type == oyASSUMED_WEB)
    {
      profile = oyProfile_FromStd( type, object );
      iccs = oyProfiles_New( object );
      if(current)
      {
        if(profile)
          *current          = 0;
        else
          *current          = -1;
      }
      oyProfiles_MoveIn( iccs, &profile, 0 );
      return iccs;
    }

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
    /* support typical output Rgb device for cinema and print proofing */
    if(type == oyPROFILE_PROOF)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
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
      text = oyStringCopy_( oyProfile_GetFileName(temp_prof, -1),
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

  oyCheckType__m( oyOBJECT_PROFILES_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return error;
  /* ---- end of common object destructor ------- */

  if(error <= 0 && s->list_)
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
oyProfiles_s* oyProfiles_MoveIn      ( oyProfiles_s      * list,
                                       oyProfile_s      ** obj,
                                       int                 pos )
{
  int error = 0;
  oyProfiles_s * s = list;

  if(s)
    oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  if(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_S)
  {
    if(!list)
      list = oyProfiles_New(0);

    if(list && list->list_)
        error = oyStructList_MoveIn( list->list_, (oyStruct_s**) obj, pos,
                                     OY_OBSERVE_AS_WELL );
  }

  return list;
}

/**
 *  @memberof oyProfiles_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyProfiles_ReleaseAt( oyProfiles_s      * list,
                                       int                 pos )
{
  int error = 0;
  oyProfiles_s * s = list;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 1 )

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
oyProfile_s *    oyProfiles_Get      ( oyProfiles_s      * list,
                                       int                 pos )
{
  oyProfile_s * obj = 0;
  oyProfiles_s * s = list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

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
  oyProfiles_s * s = list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  if(list && list->list_)
    n = oyStructList_Count( list->list_ );

  return n;
}

/** Function oyProfiles_DeviceRank
 *  @memberof oyProfiles_s
 *  @brief   sort a profile list according to a given device
 *
 *  Profiles which match the device will placed according to a rank value on 
 *  top of the list followed by the zero ranked profiles.
 *
 *  @verbatim
    oyProfiles_s * p_list = oyProfiles_ForStd( oyASSUMED_RGB, 0,0 );
    int32_t * rank_list = (int32_t*) malloc( oyProfiles_Count(p_list) *
                                             sizeof(int32_t) );
    oyProfiles_DeviceRank( p_list, oy_device, rank_list );
    n = oyProfiles_Count( p_list );
    for(i = 0; i < n; ++i)
    {
      temp_prof = oyProfiles_Get( p_list, i );
      printf("%d %d: \"%s\" %s\n", rank_list[i], i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, 0));
      oyProfile_Release( &temp_prof );
    } @endverbatim
 *
 *  @param[in,out] list                the to be sorted profile list
 *  @param[in]     device              filter pattern
 *  @param[in,out] rank_list           list of rank levels for the profile list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2009/05/22
 */
int              oyProfiles_DeviceRank ( oyProfiles_s    * list,
                                       oyConfig_s        * device,
                                       int32_t           * rank_list )
{
  int error = !list || !device || !rank_list;
  oyProfiles_s * s = list;
  int i,n,rank;
  oyProfile_s * p = 0;
  oyConfig_s * p_device = 0;
  oyOptions_s * old_db = 0;

  if(!list)
    return 0;

  if(error)
    return error;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  p_device = oyConfig_New( device->registration, 0 );
  n = oyProfiles_Count( list );

  error = !memset( rank_list, 0, sizeof(int32_t) * n );

  /* oyConfig_Compare assumes its options in device->db, so it is filled here.*/
  if(!oyOptions_Count( device->db ))
  {
    old_db = device->db;
    device->db = device->backend_core;
  }

  for(i = 0; i < n; ++i)
  {
    p = oyProfiles_Get( list, i );

    oyProfile_DeviceGet( p, p_device );
    rank = 0;

    error = oyConfig_Compare( p_device, device, &rank );
    rank_list[i] = rank;

    oyOptions_Clear( p_device->backend_core );
    oyProfile_Release( &p );
  }

  if(!error)
    error = oyStructList_Sort( list->list_, rank_list );

  if(old_db)
    device->db = old_db;

  return error;
}

/** @} *//* objects_profile */




/** \addtogroup misc Miscellaneous

 *  @{
 */

/** \addtogroup objects_rectangle Rectangle Handling

 *  @{
 */


/** @internal
 *  @brief   new
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_New_      ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_RECTANGLE_S;
# define STRUCT_TYPE oyRectangle_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyRectangle_Copy;
  s->release = (oyStruct_Release_f) oyRectangle_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** 
 *  @brief   new with geometry
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_NewWith   ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object )
{
  oyRectangle_s * s = oyRectangle_New_( object );
  if(s)
    oyRectangle_SetGeo( s, x, y, width, height );
  return s;
}

/**
 *  @brief   new from other rectangle
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_NewFrom   ( oyRectangle_s     * ref,
                                       oyObject_s          object )
{
  oyRectangle_s * s = oyRectangle_New_( object );
  if(s)
    oyRectangle_SetByRectangle(s, ref);
  return s;
}

/** Function oyRectangle_SamplesFromImage
 *  @memberof oyRectangle_s
 *  @brief   new from image
 *
 *  @param[in]     image               a image
 *  @param[in]     image_rectangle     optional rectangle from image
 *  @param[in,out] pixel_rectangle     mandatory rectangle for pixel results
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/13 (Oyranos: 0.1.10)
 *  @date    2009/06/08
 */
int            oyRectangle_SamplesFromImage (
                                       oyImage_s         * image,
                                       oyRectangle_s     * image_rectangle,
                                       oyRectangle_s     * pixel_rectangle )
{
  int error = !image,
      channel_n = 0;

  if(!error && image->type_ != oyOBJECT_IMAGE_S)
    return 0;

  channel_n = image->layout_[oyCHANS];

  if(!image_rectangle)
  {
    oyRectangle_SetGeo( pixel_rectangle, 0,0, image->width, image->height );
    pixel_rectangle->width *= channel_n;

  } else
  {
    oyRectangle_SetByRectangle( pixel_rectangle, image_rectangle );
    oyRectangle_Scale( pixel_rectangle, image->width );
    pixel_rectangle->x *= channel_n;
    pixel_rectangle->width *= channel_n;
    oyRectangle_Round( pixel_rectangle );
  }

  return error;
}

/**
 *  @brief   copy/reference from other rectangle
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_Copy      ( oyRectangle_s     * orig,
                                       oyObject_s          object )
{
  oyRectangle_s * s = 0;

  if(!orig)
    return s;

  if(!orig)
    return 0;

  s = orig;
  oyCheckType__m( oyOBJECT_RECTANGLE_S, return 0 )

  if(object)
  {
    s = oyRectangle_NewFrom( orig, object );

  } else {

    s = orig;
    oyObject_Copy( s->oy_ );
  }

  return s;
}

/**
 *  @brief   release
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRectangle_Release   ( oyRectangle_s    ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyRectangle_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_RECTANGLE_S, return 1 )

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
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_SetGeo    ( oyRectangle_s     * edit_rectangle,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height )
{
  oyRectangle_s * s = edit_rectangle;
  if(!s)
    return;

  s->x = x;
  s->y = y;
  s->width = width;
  s->height = height;
}

/**
 *  @brief   copy values
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_SetByRectangle (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s * s = edit_rectangle;
  if(!s || !ref)
    return;

  oyRectangle_SetGeo( s, ref->x, ref->y, ref->width, ref->height );
}

/**
 *  @brief   trim edit_rectangle to ref extents
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Trim      ( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;
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

  oyRectangle_Normalise( r );
}

/**
 *  @brief   trim edit_rectangle to ref extents
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_MoveInside( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * a = ref;

  if(!s)
    return;

  oyRectangle_Normalise( s );

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
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Scale     ( oyRectangle_s     * edit_rectangle,
                                       double              factor )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;
  
  if(!s)
    return;

  r->x *= factor;
  r->y *= factor;
  r->width *= factor;
  r->height *= factor;
}

/**
 *  @brief   normalise swapped values for width and height
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Normalise ( oyRectangle_s     * edit_rectangle )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;
  
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
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Round     ( oyRectangle_s     * edit_rectangle )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;
  
  if(!s)
    return;

  r->x = (int)OY_ROUND(r->x);
  r->y = (int)OY_ROUND(r->y);
  r->width = (int)OY_ROUND(r->width);
  r->height = (int)OY_ROUND(r->height);
}

/**
 *  @brief   compare
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRectangle_IsEqual   ( oyRectangle_s     * rectangle1,
                                       oyRectangle_s     * rectangle2 )
{
  int gleich = TRUE;
  oyRectangle_s * r1 = rectangle1;
  oyRectangle_s * r2 = rectangle2;
  
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
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/02/23
 */
int            oyRectangle_IsInside  ( oyRectangle_s     * test,
                                       oyRectangle_s     * ref )
{
  return oyRectangle_PointIsInside( ref, test->x, test->y ) &&
         oyRectangle_PointIsInside( ref, test->x + test->width - 1, test->y ) &&
         oyRectangle_PointIsInside( ref, test->x + test->width - 1,
                                      test->y + test->height - 1) &&
         oyRectangle_PointIsInside( ref, test->x, test->y + test->height - 1 );
}

/**
 *  @brief   compare
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2009/02/23
 */
int            oyRectangle_PointIsInside (
                                       oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y )
{
  oyRectangle_s * s = rectangle;
  oyRectangle_s * r = s;
  int in = TRUE;
  
  if(!s)
    return FALSE;

  if (x < r->x) return FALSE;
  if (y < r->y) return FALSE;
  if (x >= (r->x + r->width)) return FALSE;
  if (y >= (r->y + r->height)) return FALSE;
  return in;
}

/**
 *  @brief   count number of points covered by this rectangle
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2009/02/23
 */
double         oyRectangle_CountPoints(oyRectangle_s     * rectangle )
{
  oyRectangle_s * s = rectangle;
  oyRectangle_s * r = s;
  
  if(!s)
    return FALSE;

  return r->width * r->height;
}

/** @brief   return position inside rectangle, assuming rectangle size
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRectangle_Index     ( oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y )
{
  oyRectangle_s * s = rectangle;
  oyRectangle_s * r = s;
  
  if(!s)
    return FALSE;

  return (int)OY_ROUND((y - r->y) * r->width + (x - r->x));
}

/**
 *  @memberof oyRectangle_s
 *  @brief   debug text
 *  not so threadsafe
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
const char*    oyRectangle_Show      ( oyRectangle_s     * r )
{
  static oyChar *text = 0;

  if(!text)
    text = oyAllocateFunc_(sizeof(char) * 512);

  if(r)
    oySprintf_(text, "%.02fx%.02f%s%.02f%s%.02f", r->width,r->height,
                     r->x<0?"":"+", r->x, r->y<0?"":"+", r->y);
  else
    oySprintf_(text, "no rectangle");

  return text;

}


/**
 *  @} *//* objects_rectangle
 */



/**
 *  @brief oyDATATYPE_e to byte mapping
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2007/11/00
 */
int      oySizeofDatatype            ( oyDATATYPE_e        t )
{
  int n = 0;
  switch(t)
  {
    case oyUINT8:
         return 1;
    case oyUINT16:
    case oyHALF:
         return 2;
    case oyUINT32:
    case oyFLOAT:
         return 4;
    case oyDOUBLE:
         return 8;
  }
  return n;
}

/**
 *  @brief oyDATATYPE_e to string mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */

const char *   oyDatatypeToText      ( oyDATATYPE_e        t)
{
  const char * text = 0;
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object )
{
  oyArray2d_s * s = 0;
  int error = 0;

  if(!width || !height)
    return s;

  s = oyArray2d_New( object );
  error = !s;

  if(error <= 0)
  {
    int y_len = sizeof(unsigned char *) * (height + 1);

    s->width = width;
    s->height = height;
    s->t = data_type;
    oyRectangle_SetGeo( &s->data_area, 0,0, width, height );
    s->array2d = s->oy_->allocateFunc_( y_len );
    error = !memset( s->array2d, 0, y_len );
    s->own_lines = oyNO;
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
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object )
{
  oyArray2d_s * s = 0;
  int error = 0;

  if(!width || !height)
    return s;

  s = oyArray2d_Create_( width, height, data_type, object );
  error = !s;

  if(error <= 0)
  {
    if(data)
      error = oyArray2d_DataSet( s, data );
    else
    {
      data = s->oy_->allocateFunc_( width * height *
                                    oySizeofDatatype( data_type ) );
      error = oyArray2d_DataSet( s, data );
      s->own_lines = oyYES;
    }
  }

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

  if(error <= 0)
  {
    allocateFunc_ = s->oy_->allocateFunc_;
    s->own_lines = 2;
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

/** Function oyArray2d_ReleaseArray
 *  @memberof oyArray2d_s
 *  @brief   release Array2d::array member
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/07 (Oyranos: 0.1.11)
 *  @date    2010/09/07
 */
int            oyArray2d_ReleaseArray( oyArray2d_s       * obj )
{
  int error = 0;
  oyArray2d_s * s = obj;
  if(s && s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int y;
    size_t dsize = oySizeofDatatype( s->t );

    for( y = s->data_area.y; y < s->data_area.height; ++y )
    {
      if((s->own_lines == 1 && y == 0) ||
         s->own_lines == 2)
        deallocateFunc( s->array2d[y] );
      s->array2d[y] = 0;
    }
    deallocateFunc( s->array2d - (size_t)(dsize * -s->data_area.y) );
    s->array2d = 0;
  }
  return error;
}
 
/**
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

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyArray2d_ReleaseArray( s );

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
 *  @param[in]     data                the data, remains in the property of the
 *                                     caller
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
    return 1;

  if(!obj || obj->type_ != oyOBJECT_ARRAY2D_S)
    return 1;

  s = obj;

  {
    int y_len = sizeof(unsigned char *) * (s->height + 1),
        y;
    uint8_t * u8 = data;

    error = !s->array2d;

    if(error <= 0)
      error = !memset( s->array2d, 0, y_len );

    s->own_lines = oyNO;

    if(error <= 0)
      for( y = 0; y < s->height; ++y )
        s->array2d[y] = &u8[oySizeofDatatype( s->t ) * s->width * y];
  }

  return error;
}

/**
 *  @internal
 *  Function oyArray2d_RowsSet
 *  @memberof oyArray2d_s
 *  @brief   set the data and (re-)initialise the object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     data                the data
 *  @param[in]     do_copy             - 0 : take the memory as is
 *                                     - 1 : copy the memory monolithic
 *                                     - 2 : copy the memory chunky
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/07 (Oyranos: 0.1.11)
 *  @date    2010/09/07
 */
OYAPI int  OYEXPORT
                 oyArray2d_RowsSet   ( oyArray2d_s       * obj,
                                       oyPointer         * rows,
                                       int                 do_copy )
{
  oyArray2d_s * s = obj;
  int error = 0;

  if(!rows)
    return 1;

  if(!obj || obj->type_ != oyOBJECT_ARRAY2D_S)
    return 1;

  {
    int y_len = sizeof(unsigned char *) * (s->height + 1),
        y;
    size_t size = 0;
    oyAlloc_f allocateFunc_ = s->oy_->allocateFunc_;

    error = !s->array2d;

    size = s->width * oySizeofDatatype( s->t );

    oyArray2d_ReleaseArray( s );

    /* allocate the base array */
    oyAllocHelper_m_( s->array2d, unsigned char *, s->height+1, allocateFunc_,
                      error = 1; return 1 );
    if(error <= 0)
      error = !memset( s->array2d, 0, y_len );

    s->own_lines = do_copy;

    if(error <= 0 && s->own_lines == 2)
    {
      /* allocate each line separately */
      for(y = 0; y < s->height; ++y)
      {
        oyAllocHelper_m_( s->array2d[y], unsigned char, size, allocateFunc_,
                          error = 1; break );
        error = !memcpy( s->array2d[y], rows[y], size );
      }

    } else
    if(error <= 0 && s->own_lines == 1)
    {
      /* allocate all lines at once */
      unsigned char * u8 = 0;
      oyAllocHelper_m_( u8, unsigned char, size * s->height, allocateFunc_,
                        error = 1; return 1 );

      s->own_lines = do_copy;
      if(error <= 0)
      for( y = 0; y < s->height; ++y )
      {
        s->array2d[y] = &u8[size * y];
        error = !memcpy( s->array2d[y], rows[y], size );
      }

    } else
    if(error <= 0 && s->own_lines == 0)
    {
      /* just assign */
      for( y = 0; y < s->height; ++y )
        s->array2d[y] = rows[y];
    }
  }

  return error;
}

#if 0
/**
 *  Function oyArray2d_DataCopy
 *  @memberof oyArray2d_s
 *  @brief   copy data
 *
 *  @todo just refere the other arrays, with refs_ and refered_ members,
 *        reuse memory
 *
 *  @param[in,out] obj                 the array to fill in
 *  @param[in]     roi_obj             rectangle of interesst in samples
 *  @param[in]     source              the source data
 *  @param[in]     roi_source          rectangle of interesst in samples
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/12 (Oyranos: 0.1.10)
 *  @date    2009/03/12
 */
OYAPI int  OYEXPORT
                 oyArray2d_DataCopy  ( oyArray2d_s      ** obj,
                                       oyRectangle_s     * obj_source,
                                       oyArray2d_s       * source,
                                       oyRectangle_s     * roi_source )
{
  oyArray2d_s * s = *obj,
              * src = source;
  int error = 0;
  int new_roi = !roi;

  if(!src || !s)
    return 1;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  if(error <= 0)
  {
    if(!roi)
      roi = oyRectangle_NewWith( 0,0, s->width, s->height, s->oy_ );
    error = !roi;
  }

  /* possibly expensive hack */
  if(*obj)
    oyArray2d_Release( obj );

  if(!(*obj))
  {
    *obj = oyArray2d_Create( 0, roi->height, roi->width, src->t, src->oy_ );
    error = !*obj;
  }

  if(error <= 0)
  {
    oyAlloc_f allocateFunc_ = s->oy_->allocateFunc_;
    int i, size;

    s->own_lines = 2;
    for(i = 0; i < roi->height; ++i)
    {
      size = roi->width * oySizeofDatatype( s->t );
      if(!s->array2d[i])
        oyAllocHelper_m_( s->array2d[i], unsigned char, size, allocateFunc_,
                          error = 1; break );
      error = !memcpy( s->array2d[i], src->array2d[i], size );
    }
  }

  if(error)
    oyArray2d_Release( obj );

  if(new_roi)
    oyRectangle_Release( &roi );

  return error;
}
#endif

/** @internal
 *  Function oyArray2d_ToPPM_
 *  @memberof oyArray2d_s
 *  @brief   dump array to a netppm file 
 *
 *  @param[in]     array               the array to fill read from
 *  @param[in]     file_name           rectangle of interesst in samples
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/18 (Oyranos: 0.1.10)
 *  @date    2009/06/18
 */
int              oyArray2d_ToPPM_    ( oyArray2d_s       * array,
                                       const char        * file_name )
{
  oyArray2d_s * s = array;
  int error = 0, i,j, len, shift;
  size_t size,
         byteps;
  char * buf,
       * data;

  if(!array || !file_name || !file_name[0])
    return 1;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  if(!error)
  {
    if(s->array2d[0] == NULL)
    {
      printf("oyArray2d_s[%d] is not yet used/allocated\n",
             oyObject_GetId(s->oy_));
      return 1;
    }

    byteps = oySizeofDatatype(s->t); /* byte per sample */
    size = s->width * s->height * byteps;
    buf = oyAllocateFunc_(size + 1024);

    if(buf && size)
    {
      switch(s->t) {
      case oyUINT8:     /*  8-bit integer */
           sprintf( buf, "P5\n#%s:%d oyArray2d_s[%d]\n%d %d\n255\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height );
           break;
      case oyUINT16:    /* 16-bit integer */
      case oyUINT32:    /* 32-bit integer */
           sprintf( buf, "P5\n#%s:%d oyArray2d_s[%d]\n%d %d\n65535\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height );
           break;
      case oyHALF:      /* 16-bit floating point number */
      case oyFLOAT:     /* IEEE floating point number */
      case oyDOUBLE:    /* IEEE double precission floating point number */
           sprintf( buf, "Pf\n#%s:%d oyArray2d_s[%d]\n%d %d\n%s\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height,
                    oyBigEndian()? "1.0" : "-1.0" );
           break;
      default: return 1;
      }

      len = oyStrlen_(buf);
      data = &buf[len];
      shift = oyBigEndian() ? 0 : 1;

      switch(s->t) {
      case oyUINT8:     /*  8-bit integer */
      case oyFLOAT:     /* IEEE floating point number */
           for(i = 0; i < s->height; ++i)
             memcpy( &data[i * s->width * byteps],
                     s->array2d[i],
                     s->width * byteps );
           break;
      case oyUINT16:    /* 16-bit integer */
           for(i = 0; i < s->height; ++i)
             memcpy( &data[i * s->width * byteps + shift],
                     s->array2d[i],
                     s->width * byteps );
           break;
      case oyUINT32:    /* 32-bit integer */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((uint16_t*)&data[i*s->width*2])[j] =
                                       *((uint32_t*)&s->array2d[i][j*byteps]) /
                                                     65537;
           break;
      case oyHALF:      /* 16-bit floating point number */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((uint16_t*)&data[i*s->width*2])[j] = 
                                       *((uint16_t*)&s->array2d[i][j*byteps]);
           break;
      case oyDOUBLE:    /* IEEE double precission floating point number */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((float*)&data[i*s->width*2])[j] =
                                       *((double*)&s->array2d[i][j*byteps]);
           break;
      default: return 1;
      }

      error = oyWriteMemToFile_( file_name, buf, len + size );
    }

    if(buf) oyDeAllocateFunc_(buf);
      size = 0;
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
                             oyPixel_t     pixel_layout )
{
  int n     = oyToChannels_m( pixel_layout );
  oyProfile_s * profile = image->profile_;
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

  if(!n && cchan_n)
    n = cchan_n;

  /* describe the pixel layout and access */
  if(error <= 0)
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
  /*if(oy_debug)*/
    oySprintf_( text, "    %s\n", oyProfile_GetText(profile, oyNAME_NAME));
  /*else
    oySprintf_( text, "    %s\n", oyProfile_GetText(profile, oyNAME_NICK));*/
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

  if(error <= 0)
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
  if(point_y >= a->height)
    WARNc2_S("point_y < a->height failed(%d/%d)", point_y, a->height)
  return &array2d[ point_y ][ 0 ]; 
}

/** Function oyImage_SetPointContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout pixel accessor
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/22 (Oyranos: 0.1.10)
 *  @date    2009/02/22
 */
int       oyImage_SetArray2dPointContinous (
                                         oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         oyPointer         data )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  int pos = (point_x * image->layout_[oyCHANS]
             + image->layout_[oyCHAN0+channel])
            * image->layout_[oyDATA_SIZE];
  oyDATATYPE_e data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  int byteps = oySizeofDatatype( data_type );
  int channels = 1;

  if(channel < 0)
    channels = oyToChannels_m( image->layout_[oyLAYOUT] );

  memcpy( &array2d[ point_y ][ pos ], data, byteps * channels );

  return 0;

}

/** Function oyImage_SetLineContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout line accessor
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/22 (Oyranos: 0.1.10)
 *  @date    2009/02/22
 */
int       oyImage_SetArray2dLineContinous (
                                         oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               pixel_n,
                                         int               channel,
                                         oyPointer         data )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  oyDATATYPE_e data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  int byteps = oySizeofDatatype( data_type );
  int channels = 1;
  int offset = point_x;

  if(pixel_n < 0)
    pixel_n = image->width - point_x;

  if(channel < 0)
  {
    channels = oyToChannels_m( image->layout_[oyLAYOUT] );
    offset *= channels;
  }

  memcpy( &array2d[ point_y ][ offset ], data, pixel_n * byteps * channels );

  return 0; 
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
  oyRectangle_s * display_rectangle = 0;
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_IMAGE_S;
# define STRUCT_TYPE oyImage_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(!profile)
  {
    oyImage_Release( &s );
    return s;
  }

  s->width = width;
  s->height = height;
  {
    oyArray2d_s * a = oyArray2d_Create( channels,
                                        s->width * oyToChannels_m(pixel_layout),
                                        s->height,
                                        oyToDataType_m(pixel_layout),
                                        s_obj );
    oyImage_DataSet ( s, (oyStruct_s**) &a, 0,0,0,0,0,0 );
  }
  s->profile_ = oyProfile_Copy( profile, 0 );
  if(s->width != 0.0)
    s->viewport = oyRectangle_NewWith( 0, 0, 1.0,
                                   (double)s->height/(double)s->width, s->oy_ );

  error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout );

  if(s->pixel_data && s->layout_[oyCOFF] == 1)
    oyImage_DataSet( s, 0, oyImage_GetArray2dPointContinous,
                           oyImage_GetArray2dLineContinous, 0,
                           oyImage_SetArray2dPointContinous,
                           oyImage_SetArray2dLineContinous, 0 );
  else if(s->pixel_data)
    oyImage_DataSet( s, 0, oyImage_GetArray2dPointPlanar,
                           oyImage_GetArray2dLinePlanar, 0, 0,0,0 );

  if(error <= 0)
  {
    display_rectangle = oyRectangle_New_( 0 );

    error = !display_rectangle;
    if(error <= 0)
      oyOptions_MoveInStruct( &s->tags,
                              "//imaging/output/display_rectangle",
                              (oyStruct_s**)&display_rectangle, OY_CREATE_NEW );
  }

  return s;
}

/** @brief   collect infos about a image for showing one a display
 *  @memberof oyImage_s

    @param[in]     width               image width
    @param[in]     height              image height
    @param[in]     channels            pointer to the data buffer
    @param[in]     pixel_layout        i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]     display_name        display name
    @param[in]     display_pos_x       left image position on display
    @param[in]     display_pos_y       top image position on display
    @param[in]     display_width       width to show in window
    @param[in]     display_height      height to show in window
    @param[in]     object              the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_CreateForDisplay ( int              width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       const char        * display_name,
                                       int                 display_pos_x,
                                       int                 display_pos_y,
                                       int                 display_width,
                                       int                 display_height,
                                       oyObject_s          object)
{
  oyProfile_s * p = oyProfile_FromFile ("XYZ.icc",0,0);
  oyImage_s * s = oyImage_Create( width, height, channels, pixel_layout,
                                  p, object );
  int error = !s;
  oyRectangle_s * display_rectangle = 0;

  oyProfile_Release( &p );

  if(error <= 0)
  {
    if(!s->profile_)
      error = 1;

    if(error <= 0)
      error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout );

    if(error <= 0)
    {
      display_rectangle = (oyRectangle_s*) oyOptions_GetType( s->tags, -1,
                                    "display_rectangle", oyOBJECT_RECTANGLE_S );
      oyRectangle_SetGeo( display_rectangle, display_pos_x, display_pos_y,
                                             display_width, display_height );
    }
    error = !display_rectangle;
    
    if(error <= 0 && display_name)
      error = oyOptions_SetFromText( &s->tags, "//imaging/output/display_name",
                                     display_name, OY_CREATE_NEW );

    if(error > 0)
    {
      oyImage_Release( &s );
      WARNc1_S("Could not create image %d", oyObject_GetId( object ));
    }
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

  s = oyImage_Create( image->width, image->height,
                      0, image->layout_[oyLAYOUT],
                      image->profile_, object );

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

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

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

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

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
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/19 (Oyranos: 0.1.8)
 *  @date    2009/03/01
 */
int            oyImage_SetCritical    ( oyImage_s       * image,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        oyOptions_s     * options )
{
  oyImage_s * s = image;
  int error = !s;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

  if(profile)
  {
    oyProfile_Release( &s->profile_ );
    s->profile_ = oyProfile_Copy( profile, 0 );
  }

  if(options)
  {
    oyOptions_Release( &s->tags );
    s->tags = oyOptions_Copy( options, s->oy_ );
  }

  if(pixel_layout)
    error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout );
  else
    /* update to new ID for possible new context hashing */
    error = oyImage_CombinePixelLayout2Mask_ ( s, s->layout_[oyLAYOUT] );

  /* Not shure whether it is a good idea to have automatic image data
     allocation here. Anyway this is intented as a fallback for empty images, 
     like a unspecified output image to be catched here. */
  if((!s->setLine || !s->getLine) &&
     (!s->setPoint || !s->getPoint) &&
     s->width && s->height)
  {
    oyPixel_t pixel_layout = s->layout_[oyLAYOUT];
    oyPointer channels = 0;

    oyArray2d_s * a = oyArray2d_Create( channels,
                                        s->width * oyToChannels_m(pixel_layout),
                                        s->height,
                                        oyToDataType_m(pixel_layout),
                                        s->oy_ );
      
    oyImage_DataSet( s,    (oyStruct_s**) &a,
                           oyImage_GetArray2dPointContinous,
                           oyImage_GetArray2dLineContinous, 0,
                           oyImage_SetArray2dPointContinous,
                           oyImage_SetArray2dLineContinous, 0 );
  }

  return error;
}

/** Function oyImage_SetData
 *  @memberof oyImage_s
 *  @brief   set a custom image data module
 *
 *  This function allowes for exchanging of all the module components. 
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
 *  @param         setPoint            interface function
 *  @param         setLine             interface function
 *  @param         setTile             interface function
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
                                       oyImage_GetTile_f   getTile,
                                       oyImage_SetPoint_f  setPoint,
                                       oyImage_SetLine_f   setLine,
                                       oyImage_SetTile_f   setTile )
{
  oyImage_s * s = image;
  int error = 0;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

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

  if(setPoint)
    s->setPoint = setPoint;

  if(setLine)
    s->setLine = setLine;

  if(setTile)
    s->setTile = setTile;

  return error;
}

/** Function oyImage_FillArray
 *  @memberof oyImage_s
 *  @brief   creata a array from a image and fill with data
 *
 *  The rectangle will be considered relative to the data.
 *  A given array will be filled. do_copy makes the distinction to reuse a 
 *  available array2d.
 *
 *  @param[in]     image               the image
 *  @param[in]     rectangle           the image rectangle in a relative unit
 *                                     a rectangle in the source image
 *  @param[in]     do_copy
 *                                     - 0 assign the rows without copy
 *                                     - 1 do copy into the array
 *                                     - 2 allocate empty rows
 *                                     - 3 only skelet, no copy/assignment
 *  @param[out]    array               array to fill
 *  @param[in]     array_rectangle     the array rectangle in samples
 *                                     For NULL the image rectangle will be
 *                                     placed to the top left corner in array.
 *                                     If array_rectangle is provided, image
 *                                     samples will be placed in array at 
 *                                     array_rectangle's position and size.
 *                                     The unit is relative to the image.
 *  @param[in]     obj                 the optional user object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/02 (Oyranos: 0.1.8)
 *  @date    2009/06/19
 */
int            oyImage_FillArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       int                 do_copy,
                                       oyArray2d_s      ** array,
                                       oyRectangle_s     * array_rectangle,
                                       oyObject_s          obj )
{
  int error;
  oyArray2d_s * a = *array;
  oyImage_s * s = image;
  oyRectangle_s pixel_rectangle = {oyOBJECT_RECTANGLE_S,0,0,0},
                r = {oyOBJECT_RECTANGLE_S,0,0,0};
  oyDATATYPE_e data_type = oyUINT8;
  int is_allocated = 0;
  int size;
  oyRectangle_s array_rectangle_ = {oyOBJECT_RECTANGLE_S,0,0,0};
  double a_orig_width = 0, a_orig_height = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!image)
    return 1;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

  data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  error = oyRectangle_SamplesFromImage( image, rectangle, &pixel_rectangle );

  if(!error && array_rectangle)
    error = oyRectangle_SamplesFromImage( image, array_rectangle,
                                          &array_rectangle_ );
  else
  {
    oyRectangle_SetGeo( &r, 0,0, rectangle->width, rectangle->height );
    error = oyRectangle_SamplesFromImage( image, &r, &array_rectangle_ );
  }

  if(!error && 
     (!a ||
      (a && ( pixel_rectangle.width > a->width ||
              pixel_rectangle.height > a->height )) ||
      pixel_rectangle.width != array_rectangle_.width ||
      pixel_rectangle.height != array_rectangle_.height)
    )
  {
    oyArray2d_Release( array );

    /* array creation is not possible */
    if(!(array_rectangle_.width && array_rectangle_.height))
      error = -1;

    if(!error)
    {
      a = oyArray2d_Create_( array_rectangle_.width, array_rectangle_.height,
                             data_type, obj );
      error = !a;
      if(!error)
      {
        if(array_rectangle_.x)
          a->data_area.x = -array_rectangle_.x;
        /* allocate each single line */
        if(do_copy == 1 || do_copy == 2)
          a->own_lines = 2;
      }
    }
  }

  /* a array should have been created */
  if( !a && array_rectangle_.width && array_rectangle_.height )
  {
    WARNc_S("Could not create array.")
  }

  if( !error && a )
  {
    /* change intermediately and store old values for later recovering */
    if(a && a->width > pixel_rectangle.width)
    {
      a_orig_width = a->width;
      a->width = pixel_rectangle.width;
    }
    if(a && a->height > pixel_rectangle.height)
    {
      a_orig_height = a->height;
      a->height = pixel_rectangle.height;
    }
  }

  if(!error && do_copy != 3)
  {
  if(image->getLine)
  {
    unsigned char * data = 0;
    int i,j, height;
    size_t len, wlen, ay;
    oyPointer src, dst;

    if(a->oy_)
      allocateFunc_ = a->oy_->allocateFunc_;
    size = oySizeofDatatype( data_type );
    len = (array_rectangle_.width + array_rectangle_.x) * size;
    wlen = pixel_rectangle.width * size;

    if(do_copy == 2)
    for( i = 0; i < pixel_rectangle.height; ++i )
    {
      if(!a->array2d[i])
            oyAllocHelper_m_( a->array2d[i],
                              unsigned char, len,
                              allocateFunc_,
                              error = 1; break );
    } else
    for( i = 0; i < pixel_rectangle.height; )
    {
      height = is_allocated = 0;
      data = image->getLine( image, pixel_rectangle.y + i, &height, -1,
                             &is_allocated );

      for( j = 0; j < height; ++j )
      {
        if( i + j >= array_rectangle_.height )
          break;

        ay = array_rectangle_.y + i + j;

        if(do_copy == 1)
        {
          if(!a->array2d[ay])
            oyAllocHelper_m_( a->array2d[ay], 
                              unsigned char,
                              a_orig_width ? a_orig_width * size : len,
                              allocateFunc_,
                              error = 1; break );

          dst = &a->array2d[ay][(int)array_rectangle_.x * size];
          src = &data[(j
                       * (int)OY_ROUND(image->width * image->layout_[oyCHANS])
                       + (int)pixel_rectangle.x)
                      * size];

          if(dst != src)
            error = !memcpy( dst, src, wlen );

        } else /*if(do_copy == 0)*/
        {
          a->array2d[ay] = 
                    &data[j * size * (int)OY_ROUND(a->data_area.width)];

          a->array2d[ay] = &a->array2d[i+j]
                        [size * (int)(array_rectangle_.x + pixel_rectangle.x)];
        }
      }

      i += height;

      if(error) break;
    }

    /* allocate a complete array */
    if(do_copy != 0)
    for( ; i < a_orig_height; ++i )
    {
      ay = array_rectangle_.y + i;

      if(!a->array2d[ay])
            oyAllocHelper_m_( a->array2d[ay], 
                              unsigned char, len,
                              allocateFunc_,
                              error = 1; break );
    }

  } else
  if(image->getPoint)
  {
    WARNc_S("image->getPoint  not yet supported")
  } else
  if(image->getTile)
  {
    WARNc_S("image->getTile  not yet supported")
    error = 1;
  }
  }

  if(error)
    oyArray2d_Release( &a );
  else
  {
    /* set back, what was changed intermediately above */
    if(a_orig_width != 0)
      a->width = a_orig_width;
    if(a_orig_height != 0)
      a->height = a_orig_height;
  }

  *array = a;

  return error;
}

/** Function oyImage_ReadArray
 *  @memberof oyImage_s
 *  @brief   read a array into a image
 *
 *  The rectangle will be considered relative to the image.
 *  The given array should match that rectangle.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
int            oyImage_ReadArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * array_rectangle )
{
  oyImage_s * s = image;
  int error = !image || !array;
  oyRectangle_s pixel_rectangle = {oyOBJECT_RECTANGLE_S,0,0,0};
  oyRectangle_s new_array_rectangle_ = {oyOBJECT_RECTANGLE_S,0,0,0};
  oyDATATYPE_e data_type = oyUINT8;
  int size = 0, channel_n, i, offset, width;

  if(error)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

  data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  size = oySizeofDatatype( data_type );
  channel_n = image->layout_[oyCHANS];

  error = oyRectangle_SamplesFromImage( image, rectangle, &pixel_rectangle );
  width = pixel_rectangle.width / channel_n;

  if(array->width < pixel_rectangle.width ||
     array->height < pixel_rectangle.height)
  {
    WARNc3_S( "array (%dx%d) is too small for rectangle %s",
               (int)array->width, (int)array->height,
               oyRectangle_Show( &pixel_rectangle ) );
    error = 1;
  }

  if(!error & !image->setLine)
  {
    WARNc1_S( "only the setLine() interface is yet supported; image[%d]",
              oyObject_GetId( image->oy_ ) );
    error = 1;
  }

  if(!error && !array_rectangle)
  {
    array_rectangle = &new_array_rectangle_;
    oyRectangle_SetGeo( array_rectangle, 0,0, array->width, array->height );
    error = !array_rectangle;
  }

  if(!error)
  {
    offset = pixel_rectangle.x / channel_n;
    for(i = array_rectangle->y; i < array_rectangle->height; ++i)
    {
      image->setLine( image, offset, pixel_rectangle.y + i, width, -1,
                      &array->array2d
                              [i][(int)OY_ROUND(array_rectangle->x) * size] );
    }
  }

  return error;
}

/** Function oyImage_PixelLayoutGet
 *  @memberof oyImage_s
 *  @brief   get the pixel layout
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
oyPixel_t      oyImage_PixelLayoutGet( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->layout_[oyLAYOUT];
}

/** Function oyImage_ProfileGet
 *  @memberof oyImage_s
 *  @brief   get the image profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/05 (Oyranos: 0.1.10)
 *  @date    2009/08/05
 */
oyProfile_s *  oyImage_ProfileGet    ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return oyProfile_Copy( s->profile_, 0 );
}

/** Function oyImage_TagsGet
 *  @memberof oyImage_s
 *  @brief   get object tags
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
oyOptions_s *  oyImage_TagsGet       ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return oyOptions_Copy( s->tags, 0 );
}

/** @} *//* objects_image */ 



/** \addtogroup objects_conversion Conversion API's
 *  Colour conversion front end API's.
 *
 *  Colour conversions are realised by structures called acyclic graphs. A graph
 *  combines several processing elements and their according stages into one
 *  process model. A very simple graph will link elements just one by one to
 *  form a chain. More complex graphs are possible by branching and rejoining of
 *  data processing arms.
 *  Datas are requested always from the end. This makes it simple to create
 *  views at data. A observer tells the to be viewed plugin that it wants
 *  to connect and can request the data stream.
 *  In the other direction plug-ins can send events along the pipe through
 *  callbacks, e.g. for reporting errors or requesting updating of parameters.
 *  The @ref module_api explains how to create modules to plug into Oyranos.
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
 *  existence of the oyConnector_s inside the module filter structure
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
 *  Changes are propagated by events. This turns the acyclic graph into a 
 *  dual one. The events use the same graph just in the other direction.
 *  Events and data requests are distinct.
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

  b:p->a:s [arrowhead=none, arrowtail=normal, label=data];
} 
 \enddot
 *
 * A oyFilterNode_s can have various oyFilterPlug_s ' to obtain data from
 * different sources. The required number is described in the oyCMMapi4_s 
 * structure, which is part of oyFilterCore_s. But each plug can only connect to
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  s->is_plug = -1;

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
 *  @version Oyranos: 0.3.0
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2011/01/31
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

  if(error <= 0)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    error = oyObject_CopyNames( s->oy_, obj->oy_ );

    s->connector_type = oyStringCopy_( obj->connector_type, allocateFunc_ );
    s->is_plug = obj->is_plug;
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

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->connector_type)
      deallocateFunc( s->connector_type ); s->connector_type = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyConnector_SetName
 *  @memberof oyConnector_s
 *  @brief   set the names in a connector
 *
 *  These are UI strings, e.g. "Img", "Image", "Image Socket" .
 *
 *  @param[in,out] obj                 Connector object
 *  @param[in]     string              the name to set
 *  @param[in]     type                the names type
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_SetName ( oyConnector_s     * obj,
                                       const char        * string,
                                       oyNAME_e            type )
{
  oyConnector_s * s = obj;
  int error = 0;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 1 )

  error = oyObject_SetName( obj->oy_, string, type );

  return error;
}

/** Function oyConnector_GetName
 *  @memberof oyConnector_s
 *  @brief   set the names in a connector
 *
 *  Get UI strings.
 *
 *  @param[in]     obj                 Connector object
 *  @param[in]     type                the names type
 *  @return                            the name string
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
const char *     oyConnector_GetName ( oyConnector_s     * obj,
                                       oyNAME_e            type )
{
  oyConnector_s * s = obj;
  const char * string = 0;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 0 )

  string = oyObject_GetName( obj->oy_, type );

  return string;
}

/** Function oyConnector_IsPlug
 *  @memberof oyConnector_s
 *  @brief   is this connector a plug or a socket
 *
 *  @param[in]     obj                 Connector object
 *  @return                            boolean; 0 - socket; 1 - plug
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_IsPlug  ( oyConnector_s     * obj )
{
  oyConnector_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return obj->is_plug;
}

/** Function oyConnector_SetIsPlug
 *  @memberof oyConnector_s
 *  @brief   Set this connector as a plug or a socket
 *
 *  @param[in,out] obj                 Connector object
 *  @param[in]     is_plug             boolean; 0 - socket; 1 - plug
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_SetIsPlug(oyConnector_s     * obj,
                                       int                 is_plug )
{
  oyConnector_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 1 )

  obj->is_plug = is_plug;

  return 0;
}

/** Function oyConnector_GetReg
 *  @memberof oyConnector_s
 *  @brief   Get the registration for the connection type
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in]     obj                 Connector object
 *  @return                            registration string
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
const char *     oyConnector_GetReg  ( oyConnector_s     * obj )
{
  oyConnector_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return obj->connector_type;
}

/** Function oyConnector_SetReg
 *  @memberof oyConnector_s
 *  @brief   Set this connectors type string
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in,out] obj                 Connector object
 *  @param[in]     type_registration   the registration string to describe the
 *                                     type
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_SetReg  ( oyConnector_s     * obj,
                                       const char        * type_registration )
{
  oyConnector_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 1 )

  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyAlloc_f allocateFunc = s->oy_->allocateFunc_;

    if(s->connector_type)
      deallocateFunc( s->connector_type ); s->connector_type = 0;

    obj->connector_type = oyStringCopy_( type_registration, allocateFunc );
  }

  return 0;
}

/** Function oyConnector_SetMatch
 *  @memberof oyConnector_s
 *  @brief   Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in,out] obj                 Connector object
 *  @param[in]     func                the check function
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_SetMatch( oyConnector_s     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func )
{
  oyConnector_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 1 )

  obj->filterSocket_MatchPlug = func;

  return 0;
}

/** Function oyConnector_GetMatch
 *  @memberof oyConnector_s
 *  @brief   Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in]     obj                 Connector object
 *  @return                            the check function
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
oyCMMFilterSocket_MatchPlug_f oyConnector_GetMatch (
                                       oyConnector_s     * obj )
{
  oyConnector_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return obj->filterSocket_MatchPlug;
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
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2009/02/19
 */
OYAPI int  OYEXPORT
                 oyFilterSocket_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e )
{
  int n, i;
  oyFilterSocket_s * s;
  oyFilterPlug_s * p;

  if(e != oyCONNECTOR_EVENT_OK && oy_debug_signals)
  {
    WARNc5_S("\n  oyFilterNode_s[%d]->oyFilterSocket_s[%d]\n"
             "  event: \"%s\" plug[%d/node%d]",
            (c && c->remote_socket_ && c->remote_socket_->node) ?
                   oyObject_GetId(c->remote_socket_->node->oy_) : -1,
            (c && c->remote_socket_) ? oyObject_GetId(c->remote_socket_->oy_)
                                     : -1,
            oyConnectorEventToText(e),
            c ? oyObject_GetId( c->oy_ ) : -1,
            c ? (c->node ? oyObject_GetId( c->node->oy_ ) : -1) : -1
          );
  }

  if(!c)
    return 1;

  s = c->remote_socket_;

  if(!s)
    return 0;

  n = oyFilterPlugs_Count( s->requesting_plugs_ );

  if(e == oyCONNECTOR_EVENT_RELEASED)
    for(i = 0; i < n; ++i)
    {
      p = oyFilterPlugs_Get( s->requesting_plugs_, i );
      if(p == c)
      {
        oyFilterPlugs_ReleaseAt( s->requesting_plugs_, i );
        break;
      }
    }

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
  if(oy_debug_signals)
    WARNc4_S("oyFilterNode_s[%d]->oyFilterPlug_s[%d]\n"
             "  event: \"%s\" socket[%d]",
            (c && c->node) ? oyObject_GetId(c->node->oy_) : -1,
            c ? oyObject_GetId(c->oy_) : -1,
            oyConnectorEventToText(e),
            (c && c->remote_socket_) ?
                                   oyObject_GetId( c->remote_socket_->oy_ ) : -1
          );

  if(c && e == oyCONNECTOR_EVENT_RELEASED)
    c->remote_socket_ = 0;

  return 0;
}


/** Function oyFilterSocket_SignalToGraph
 *  @memberof oyFilterSocket_s
 *  @brief   send a signal through the graph
 *
 *  The traversal direction is defined as from the starting node to the output.
 *
 *  @return                            1 if handled or zero
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/27 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
OYAPI int  OYEXPORT
                 oyFilterSocket_SignalToGraph (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e )
{
  int result = 0;
  oySIGNAL_e sig = oySIGNAL_OK;
  int n, i, j_n,j, k,k_n, handled = 0, pos;
  oyFilterPlug_s * p, * sp;
  oyFilterGraph_s * graph = 0;

  switch(e)
  {
    case oyCONNECTOR_EVENT_OK:                /**< kind of ping */
    case oyCONNECTOR_EVENT_CONNECTED:         /**< connection established */
    case oyCONNECTOR_EVENT_RELEASED:          /**< released the connection */
    case oyCONNECTOR_EVENT_DATA_CHANGED:      /**< call to update image views */
    case oyCONNECTOR_EVENT_STORAGE_CHANGED:   /**< new data accessors */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_DATA: /**< can not process image */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION:/**< can not handle option */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT:/**< can not handle profile */
    case oyCONNECTOR_EVENT_INCOMPLETE_GRAPH:  /**< can not completely process */
         sig = (oySIGNAL_e) e; break;
  }

  result = oyStruct_ObserverSignal( (oyStruct_s*) c->node, sig, 0 );

  if(e != oyCONNECTOR_EVENT_OK && oy_debug_signals)
  {
    WARNc4_S("oyFilterNode_s[%d]->oyFilterSocket_s[%d]\n"
             "  event: \"%s\" socket[%d]",
            (c && c->node) ? oyObject_GetId(c->node->oy_) : -1,
            c ? oyObject_GetId(c->oy_) : -1,
            oyConnectorEventToText(e),
            (c) ? oyObject_GetId( c->oy_ ) : -1
          );
  }

  n = oyFilterPlugs_Count( c->requesting_plugs_ );

  for(i = 0; i < n; ++i)
  {
    p = oyFilterPlugs_Get( c->requesting_plugs_, i );
    result = oyStruct_ObserverSignal( (oyStruct_s*) p->node, sig, 0 );

    /* get all nodes in the output direction */
    graph = oyFilterGraph_FromNode( p->node, OY_INPUT );

    j_n = oyFilterNodes_Count( graph->nodes );
    for( j = 0; j < j_n; ++j )
    {
      oyFilterNode_s * node = oyFilterNodes_Get( graph->nodes, j );

      /* iterate over all node outputs */
      k_n = oyFilterNode_EdgeCount( node, 1, OY_FILTEREDGE_CONNECTED );
      for(k = 0; k < k_n; ++k)
      {
        pos = oyFilterNode_GetConnectorPos( node, 1, "///", k,
                                            OY_FILTEREDGE_CONNECTED );
        sp = oyFilterNode_GetPlug( node, pos );
        oyFilterSocket_Callback( sp, e );
      }

      oyFilterNode_Release( &node );
      if(handled)
        break;
    }

    oyFilterGraph_Release( &graph );
    if(handled)
      break;
  }

  return handled;
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0)
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

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyObject_Ref(s->oy_);

  oyFilterNode_Release( &s->node );

  {
    int count = 0,
        i;
    oyFilterPlug_s * c = 0;

    count = oyFilterPlugs_Count( s->requesting_plugs_ );
    for(i = 0; i < count; ++i)
    {
      c = oyFilterPlugs_Get( s->requesting_plugs_, i );
      oyFilterPlug_Callback( c, oyCONNECTOR_EVENT_RELEASED );
      oyFilterPlug_Release( &c );
    }
  }

  oyObject_UnRef(s->oy_);
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
  oyFilterPlug_s * tp = 0;
  oyFilterSocket_s * ts = 0;

  if(!p || !*p || !s || !*s)
    return 1;

  tp = *p;
  ts = *s;

  if(tp->remote_socket_)
    oyFilterSocket_Callback( tp, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( &tp->remote_socket_ );

# if DEBUG_OBJECT
      WARNc6_S("%s Id: %d -> %s Id: %d\n  %s -> %s",
             oyStructTypeToText( (*p)->type_ ), oyObject_GetId((*p)->oy_),
             oyStructTypeToText( (*s)->type_ ), oyObject_GetId((*s)->oy_),
             (*p)->node->relatives_,
             (*s)->node->relatives_ )
#endif

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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0)
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

  oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyFilterNode_Release( &s->node );

  oyFilterSocket_Callback( s, oyCONNECTOR_EVENT_RELEASED );
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

/** Function oyFilterPlug_ResolveImage
 *  @memberof oyFilterPlug_s
 *  @brief   resolve processing data during a filter run
 *
 *  The function is a convenience function to use inside a filters
 *  oyCMMFilterPlug_Run_f call. The function makes only sense for non root
 *  filters.
 *
 *  @param[in,out] plug                the filters own plug
 *  @param[in,out] socket              the filters own socket
 *  @param[in,out] ticket              the actual ticket
 *  @return                            the input image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/01 (Oyranos: 0.1.10)
 *  @date    2009/05/01
 */
OYAPI oyImage_s * OYEXPORT
             oyFilterPlug_ResolveImage(oyFilterPlug_s    * plug,
                                       oyFilterSocket_s  * socket,
                                       oyPixelAccess_s   * ticket )
{
  int error = !plug || !plug->remote_socket_ ||
              !ticket ||
              !socket || !socket->node;
  oyImage_s * image_input = 0, * image = 0;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node;
  oyPixel_t pixel_layout = 0;
  oyOptions_s * options = 0,
              * requests = 0,
              * ticket_orig;
  int32_t n = 0;

  if(error)
    return 0;

  image_input = oyImage_Copy( (oyImage_s*)plug->remote_socket_->data, 0 );
  input_node = plug->remote_socket_->node;

 
  if(!image_input)
  {
    /* get options */
    options = oyFilterNode_OptionsGet( node, 0 );

    /* store original queue */
    ticket_orig = ticket->request_queue;
    ticket->request_queue = 0;

    /* select only resolve requests */
    error = oyOptions_Filter( &requests, &n, 0,
                              oyBOOLEAN_INTERSECTION,
                              "////resolve", options );
    oyOptions_Release( &options );

    /* combine old queue and requests from the current node */
    oyOptions_CopyFrom( &ticket->request_queue, requests, oyBOOLEAN_UNION, 0,0);
    oyOptions_CopyFrom( &ticket->request_queue, ticket_orig, oyBOOLEAN_UNION,
                        0, 0 );

    /* filter again, (really needed?) */
    oyOptions_Filter( &ticket->request_queue, &n, 0,
                      oyBOOLEAN_INTERSECTION, "////resolve", requests );
    oyOptions_Release( &requests );
 
    /* try to obtain the processing data from a generator filter */
    input_node->api7_->oyCMMFilterPlug_Run( node->plugs[0], ticket );
    image_input = oyImage_Copy( (oyImage_s*)plug->remote_socket_->data, 0 );

    /* clean up the queue */
    oyOptions_Release( &ticket->request_queue );

    /* restore old queue */
    ticket->request_queue = ticket_orig; ticket_orig = 0;

    if(!image_input)
      return 0;
  }

  if(!socket->data)
  {
    /* Copy a root image or link to a non root image. */
    if(!plug->remote_socket_->node->api7_->plugs_n)
    {
      options = oyFilterNode_OptionsGet( node, 0 );
      error = oyOptions_Filter( &requests, &n, 0,
                                oyBOOLEAN_INTERSECTION,
                                "////resolve", options );
      oyOptions_Release( &options );
      oyOptions_CopyFrom( &requests, ticket->request_queue,oyBOOLEAN_UNION,0,0);

      error = oyOptions_FindInt( requests, "pixel_layout", 0,
                                 (int32_t*)&pixel_layout );
      oyOptions_Release( &requests );

      if(error == 0)
      {
        /* possibly complete the pixel layout information */
        int n = oyToChannels_m( pixel_layout );
        int cchan_n = oyProfile_GetChannelsCount( image_input->profile_ );
        oyPixel_t layout = oyDataType_m( oyToDataType_m(pixel_layout) ) |
                           oyChannels_m( OY_MAX(n, cchan_n) );
        /* create a new image */
        image = oyImage_Create( image_input->width, image_input->height,
                                0, layout,
                                image_input->profile_, node->oy_ );

      } else
        image = oyImage_Copy( (oyImage_s*) image_input, node->oy_ );


    } else
      image = oyImage_Copy( (oyImage_s*) image_input, 0 );

    error = oyFilterNode_DataSet( node, (oyStruct_s*)image, 0, 0 );
    oyImage_Release( &image );
  }

  if(!ticket->output_image)
    ticket->output_image = oyImage_Copy( (oyImage_s*) socket->data, 0 );

  oyOptions_Release( &requests );

  return image_input;
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0)
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

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 1 )

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

    if(error <= 0 && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(error <= 0)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
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

  if(error <= 0 && list->type_ != oyOBJECT_FILTER_PLUGS_S)
    error = 1;
  
  if(error <= 0)
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


/** Function oyFilterRegistrationToSTextField
 *  @brief   analyse registration string
 *
 *  @param         registration        registration string to analyse
 *  @param[in]     field               kind of answere in return, only one field
 *  @param[out]    len                 text length
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/05/27
 */
char *         oyFilterRegistrationToSTextField (
                                       const char        * registration,
                                       oyFILTER_REG_e      field,
                                       int               * len )
{
  char  * text = 0, * erg = 0, * t = 0;
  int     texts_n = 0;
  int pos = 0, single = 0;

  if(registration)
  {
    text = (char*) registration;

         if(field & oyFILTER_REG_TOP)
      pos = 1;
    else if(field & oyFILTER_REG_DOMAIN)
      pos = 2;
    else if(field & oyFILTER_REG_TYPE)
      pos = 3;
    else if(field & oyFILTER_REG_APPLICATION)
      pos = 4;
    else if(field & oyFILTER_REG_OPTION)
      pos = 5;
    else if(field & oyFILTER_REG_MAX)
      pos = 6;

    if(field == oyFILTER_REG_TOP ||
       field == oyFILTER_REG_DOMAIN ||
       field == oyFILTER_REG_TYPE ||
       field == oyFILTER_REG_APPLICATION ||
       field == oyFILTER_REG_OPTION ||
       field == oyFILTER_REG_MAX)
      single = 1;

    texts_n = oyStringSegments_( registration, OY_SLASH_C );
    if(texts_n >= pos && field == oyFILTER_REG_TOP)
    {
      erg = oyStringSegment_( text, OY_SLASH_C, 0, len );

      /** We can not allow attributes in the oyFILTER_TOP_TYPE section, as this
       *  would conflict with the Elektra namespace policy. */
      t = oyStrnchr_( erg, '.', *len );
      if(t)
      {
        text = oyStringCopy_( erg, oyAllocateFunc_ );
        text[*len] = 0;
        /* i18n hint: a string "with '.' is not allowed" */
        WARNc3_S( "oyFILTER_TOP_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        goto clean;
      }
    }
    if(texts_n >= pos && field == oyFILTER_REG_DOMAIN)
      erg = oyStringSegment_( text, OY_SLASH_C, 1, len );
    if(texts_n >= pos && field == oyFILTER_REG_TYPE)
    {
      erg = oyStringSegment_( text, OY_SLASH_C, 2, len );

      /** We can not allow attributes in the oyFILTER_REG_TYPE section, as this
       *  would conflict with robust module cache lookup. */
      t = oyStrnchr_( erg, '.', *len );
      if(t)
      {
        text = oyStringCopy_( erg, oyAllocateFunc_ );
        text[*len] = 0;
        WARNc3_S( "oyFILTER_REG_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        goto clean;
      }
    }
    if(texts_n >= pos && field == oyFILTER_REG_APPLICATION)
      erg = oyStringSegment_( text, OY_SLASH_C, 3, len );
    if(texts_n >= pos && field == oyFILTER_REG_OPTION)
      erg = oyStringSegment_( text, OY_SLASH_C, 4, len );
#if 0
    if(erg && field == oyFILTER_REG_OPTION)
    {
      erg = oyStrnchr_( erg, '.', len );
      if(erg)
        erg[0] = 0;
    }
#endif
    /** oyFILTER_REG_MAX returns the last level which is the key name. */
    if(field == oyFILTER_REG_MAX)
      erg = oyStringSegment_( text, OY_SLASH_C, texts_n-1, len );


    /** For several oyFILTER_REG bits we compose a new registration string. */
    if(!single && field)
    {
      WARNc_S("Multiple fields are not allowed.\n"
              "  Use oyFilterRegistrationToText instead!");
    } else if( !field )
    {
      if(erg)
        WARNc1_S("text variable should be zero, found: %s", erg);
    }
  }

  return erg;

  clean:
  if(text)
    oyDeAllocateFunc_( text );
  return 0;
}

/** Function oyFilterRegistrationToText
 *  @brief   analyse registration string
 *
 *  @param         registration        registration string to analyse
 *  @param[in]     fields              kind of answere in return
 *  @param[in]     allocateFunc        use this or Oyranos standard allocator
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/05/27
 */
char *         oyFilterRegistrationToText (
                                       const char        * registration,
                                       oyFILTER_REG_e      fields,
                                       oyAlloc_f           allocateFunc )
{
  char  * text = 0, * tmp = 0;
  int     texts_n = 0,
          pos = 0, single = 0, len = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(registration)
  {
         if(fields & oyFILTER_REG_TOP)
      pos = 1;
    else if(fields & oyFILTER_REG_DOMAIN)
      pos = 2;
    else if(fields & oyFILTER_REG_TYPE)
      pos = 3;
    else if(fields & oyFILTER_REG_APPLICATION)
      pos = 4;
    else if(fields & oyFILTER_REG_OPTION)
      pos = 5;
    else if(fields & oyFILTER_REG_MAX)
      pos = 6;

    if(fields == oyFILTER_REG_TOP ||
       fields == oyFILTER_REG_DOMAIN ||
       fields == oyFILTER_REG_TYPE ||
       fields == oyFILTER_REG_APPLICATION ||
       fields == oyFILTER_REG_OPTION ||
       fields == oyFILTER_REG_MAX)
      single = 1;

#if USE_OLD_STRING_API
    char ** texts = oyStringSplit_( registration, OY_SLASH_C, &texts_n,oyAllocateFunc_);
    if(texts_n >= pos && fields == oyFILTER_REG_TOP)
    {
      text = oyStringCopy_( texts[0], allocateFunc );

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
    if(texts_n >= pos && fields == oyFILTER_REG_DOMAIN)
      text = oyStringCopy_( texts[1], allocateFunc );
    if(texts_n >= pos && fields == oyFILTER_REG_TYPE)
    {
      text = oyStringCopy_( texts[2], allocateFunc );

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
    if(texts_n >= pos && fields == oyFILTER_REG_APPLICATION)
      text = oyStringCopy_( texts[3], allocateFunc );
    if(texts_n >= pos && fields == oyFILTER_REG_OPTION)
      text = oyStringCopy_( texts[4], allocateFunc );
    if(text && fields == oyFILTER_REG_OPTION)
    {
      tmp = oyStrchr_( text, '.' );
      if(tmp)
        tmp[0] = 0;
    }
    /** oyFILTER_REG_MAX returns the last level which is the key name. */
    if(fields == oyFILTER_REG_MAX)
      text = oyStringCopy_( texts[texts_n-1], allocateFunc );

    oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
#else
    if(single)
      tmp = oyFilterRegistrationToSTextField ( registration, fields, &len );
    if(tmp)
    {
      text = allocateFunc( len + 1 );
      memcpy( text, tmp, len );
      text[len] = 0;
    }
#endif

    /** For several oyFILTER_REG bits we compose a new registration string. */
    if(!single && fields)
    {
      text = 0;

      if(fields & oyFILTER_REG_TOP)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_TOP, 0 );
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_DOMAIN)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_DOMAIN, 0);
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_TYPE)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_TYPE, 0 );
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_APPLICATION)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_APPLICATION, 0 );
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_OPTION)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_OPTION, 0);
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(texts_n > 5 && fields & oyFILTER_REG_MAX)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_OPTION, 0);
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      }

      tmp = text; 
      text = oyStringCopy_( tmp, allocateFunc );
      oyFree_m_(tmp);

    } else if( !fields )
    {
      if(text)
        WARNc1_S("text variable should be zero, found: %s", text);
      text = oyStringCopy_( registration, allocateFunc );
    }
  }

  return text;
}


#if USE_OLD_STRING_API
/** Function oyFilterRegistrationMatch
 *  @brief   analyse registration string and compare with a given pattern
 *
 *  The rules are described in the @ref module_api overview.
 *  The function is intensively used.
 *
 *  @param         registration        registration string to analise
 *  @param         pattern             pattern or key name to compare with
 *  @param         api_number          select object type
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/04/13
 */
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number )
{
  char ** reg_texts = 0;
  int     reg_texts_n = 0;
  char ** regc_texts = 0;
  int     regc_texts_n = 0;
  int     regc_texts_pos[3] = {0,0,0};
  char  * reg_text = 0;
  char ** p_texts = 0;
  int     p_texts_n = 0;
  char ** pc_texts = 0;
  int     pc_texts_n = 0;
  char  * pc_text = 0;
  int     match = 0, match_tmp = 0, i,j,k, api_num = oyOBJECT_NONE, pc_api_num;
  char    pc_match_type = '+';
  char  * key_tmp = 0,
        * max_segment = 0;
 
  if(registration && pattern)
  {
    api_num = oyCMMapiNumberToChar(api_number);
    match_tmp = 1;
    reg_texts = oyStringSplit_( registration, OY_SLASH_C, &reg_texts_n,
                                oyAllocateFunc_);
    p_texts = oyStringSplit_( pattern, OY_SLASH_C, &p_texts_n, oyAllocateFunc_);

    for( i = 0; i < reg_texts_n && i < p_texts_n; ++i)
    {
      regc_texts_n = 0;

      /* allow a key only in *pattern to filter from *registration */
      if(p_texts_n == 1)
      {
        key_tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_MAX,0);
        regc_texts = oyStringSplit_( key_tmp,'.',&regc_texts_n,
                                     oyAllocateFunc_);
        /*if(oyStringSegment_(key_tmp, '.', &regc_texts_n, &regc_texts_pos,
                            &max_segment ))
          return 0;*/
        oyFree_m_( key_tmp );
        pc_texts = oyStringSplit_( p_texts[i],'.',&pc_texts_n, oyAllocateFunc_);
        i = reg_texts_n;
      } else
      /* level by level comparision */
      {
        regc_texts = oyStringSplit_( reg_texts[i],'.',&regc_texts_n,
                                     oyAllocateFunc_);
        pc_texts = oyStringSplit_( p_texts[i],'.',&pc_texts_n, oyAllocateFunc_);
      }

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
               oyStrcmp_( reg_text, pc_text ) == 0)
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

  if(match_tmp == 1 && !match)
    match = 1;

  return match;


  clean_up:
    oyStringListRelease_( &pc_texts, pc_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &regc_texts, regc_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &reg_texts, reg_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &p_texts, p_texts_n, oyDeAllocateFunc_ );
  return 0;
}
#else
/** Function oyFilterRegistrationMatch 
 *  @brief   analyse registration string and compare with a given pattern
 *
 *  The rules are described in the @ref module_api overview.
 *  The function is intensively used.
 *
 *  @param         registration        registration string to analise
 *  @param         pattern             pattern or key name to compare with
 *  @param         api_number          select object type
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/07/16
 */
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number )
{
  char  * reg_text = 0;
  int     reg_n = 0;
  int     reg_len = 0;
  char  * regc_text = 0;
  int     regc_n = 0;
  int     regc_len = 0;
  char  * p_text = 0;
  int     p_n = 0;
  int     p_len = 0;
  char  * pc_text = 0;
  int     pc_n = 0;
  int     pc_len = 0;
  int     match = 0, match_tmp = 0, i,j,k, api_num = oyOBJECT_NONE, pc_api_num;
  char    pc_match_type = '+';
 
  if(registration && pattern)
  {
    api_num = oyCMMapiNumberToChar(api_number);
    match_tmp = 1;
    reg_n = oyStringSegments_(registration, OY_SLASH_C);
    p_n = oyStringSegments_(pattern, OY_SLASH_C);

    for( i = 0; i < reg_n && i < p_n; ++i)
    {
      regc_n = 0;

      /* allow a key only in *pattern to filter from *registration */
      if(p_n == 1)
      {
        reg_text = oyFilterRegistrationToSTextField( registration,
                                                    oyFILTER_REG_MAX, &reg_len);
        regc_n = oyStringSegmentsN_( reg_text, reg_len, '.' );
        /*if(oyStringSegment_(key_tmp, '.', &regc_n, &regc_texts_pos,
                              &max_segment ))
          return 0;*/
        p_text = oyStringSegment_( (char*)pattern, OY_SLASH_C, i, &p_len );
        pc_n = oyStringSegmentsN_( p_text, p_len, '.' );
        i = reg_n;
      } else
      /* level by level comparision */
      {
        reg_text = oyStringSegment_( (char*)registration,
                                        OY_SLASH_C, i, &reg_len );
        regc_n = oyStringSegmentsN_( reg_text, reg_len, '.' );
        p_text = oyStringSegment_( (char*)pattern, OY_SLASH_C, i, &p_len );
        pc_n = oyStringSegmentsN_( p_text, p_len, '.' );
      }

      if(match_tmp && pc_n && regc_n)
      {
        for( j = 0; j < pc_n; ++j)
        {
          match_tmp = 0;
          pc_api_num = 0;
          pc_match_type = '+';
          pc_text = oyStringSegmentN_( p_text, p_len, '.', j, &pc_len );

          if(pc_text[0] == '4' ||
             pc_text[0] == '5' ||
             pc_text[0] == '6' ||
             pc_text[0] == '7' ||
             pc_text[0] == '8' ||
             pc_text[0] == '9')
          {
            pc_api_num = pc_text[0];
            ++ pc_text;
            pc_match_type = pc_text[0];
            ++ pc_text;
            pc_len -= 2;
          } else
          if(pc_text[0] == '_' ||
             pc_text[0] == '-')
          {
            pc_match_type = pc_text[0];
            ++ pc_text;
            -- pc_len;
          }

          for( k = 0; k < regc_n; ++k )
          {
            regc_text = oyStringSegmentN_( reg_text, reg_len, '.', k,
                                              &regc_len );
            if(regc_text[0] == '4' ||
               regc_text[0] == '5' ||
               regc_text[0] == '6' ||
               regc_text[0] == '7' ||
               regc_text[0] == '8' ||
               regc_text[0] == '9')
            {
              ++ regc_text;
              ++ regc_text;
              regc_len -= 2;
            } else
            if(regc_text[0] == '_' ||
               regc_text[0] == '-')
            {
              ++ regc_text;
              -- regc_len;
            }

            if((!pc_api_num || (pc_api_num && api_num == pc_api_num)) &&
               memcmp( regc_text, pc_text, OY_MIN(regc_len,pc_len) ) == 0 &&
	       (regc_len == pc_len || !regc_len || !pc_len))
            {
              if(pc_match_type == '+' ||
                 pc_match_type == '_')
              {
                ++ match;
                match_tmp = 1;
              } else /* if(pc_match_type == '-') */
                return 0;
            }
          }

          if(pc_match_type == '+' && !match_tmp)
            return 0;
        }
      }
    }
  }

  if(match_tmp == 1 && !match)
    match = 1;

  return match;
}
#endif

/** Function oyFilterRegistrationMatchKey
 *  @brief   compare two registration strings, skip key attributes
 *
 *  The rules are described in the @ref module_api overview.
 *  The rules in this function map especially to key storage rules for 
 *  Oyranos DB.
 *
 *  The non key part is handled as namespace and should match in order to
 *  consider the two keys from the same namespace.
 *  The second condition is the key name is stripped from all attributes.
 *  This means the string part after the last slash '/' is taken into account
 *  only until the first point '.' or end of string appears.
 *
 *  @param         registration_a      registration key
 *  @param         registration_b      registration key
 *  @param         api_number          select object type
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/03 (Oyranos: 0.1.10)
 *  @date    2010/06/03
 */
int    oyFilterRegistrationMatchKey  ( const char        * registration_a,
                                       const char        * registration_b,
                                       oyOBJECT_e          api_number )
{
  char * tmp1 = 0,
       * tmp2 = 0,
       * tmp = 0;
  int match = 0;

  if(registration_a && oyStrrchr_( registration_a, '/' ))
    tmp1 = oyStringCopy_( registration_a, oyAllocateFunc_ );
  if(registration_b && oyStrrchr_( registration_b, '/' ))
    tmp2 = oyStringCopy_( registration_b, oyAllocateFunc_ );

  if(!tmp1 || !tmp2)
    return match;

  if(tmp1)
  {
    tmp = oyStrrchr_( tmp1, '/' );
    tmp[0] = 0;
  }
  if(tmp2)
  {
    tmp = oyStrrchr_( tmp2, '/' );
    tmp[0] = 0;
  }

  match = oyFilterRegistrationMatch( tmp1, tmp2, api_number );

  if(match)
  {
    oyFree_m_(tmp1);
    oyFree_m_(tmp2);
    tmp1 = oyFilterRegistrationToText( registration_a,
                                       oyFILTER_REG_OPTION, 0 );
    tmp = oyStrchr_( tmp1, '.' );
    if(tmp)
      tmp[0] = 0;
    tmp2 = oyFilterRegistrationToText( registration_b,
                                       oyFILTER_REG_OPTION, 0 );
    tmp = oyStrchr_( tmp2, '.' );
    if(tmp)
      tmp[0] = 0;

    if(oyStrcmp_( tmp1, tmp2 ) == 0)
      ++match;
    else
      match = 0;

    oyFree_m_(tmp1);
    oyFree_m_(tmp2);
  }

  return match;
}

/** Function oyTextIccDictMatch
 *  @brief   analyse a string and compare with a given pattern
 *
 *  The rules are described in the ICC meta tag dict type at
 *  http://www.color.org    ICCSpecRevision_25-02-10_dictType.pdf
 *
 *  @param         text                value string
 *  @param         pattern             pattern to compare with
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.1.13
 *  @since   2010/11/21 (Oyranos: 0.1.13)
 *  @date    2010/11/21
 */
int    oyTextIccDictMatch            ( const char        * text,
                                       const char        * pattern )
{
  int match = 0;
  int n = 0, p_n = 0, i, j;
  char ** texts = 0, * t;
  char ** patterns = 0, * p;
  long num[2];
  int num_valid[2];
  double dbl[2];
  int dbl_valid[2]; 

  if(text && pattern)
  {
    texts = oyStringSplit_(text, ',', &n, oyAllocateFunc_ );
    patterns = oyStringSplit_(pattern, ',', &p_n, oyAllocateFunc_ );

    for( i = 0; i < n; ++i)
    {
      t = texts[i];
      num_valid[0] = !oyStringToLong(t,&num[0]);
      dbl_valid[0] = !oyStringToDouble(t,&dbl[0]);
      for( j = 0; j < p_n; ++j )
      {
        p = patterns[j];
        num_valid[1] = !oyStringToLong(p,&num[1]);
        dbl_valid[1] = !oyStringToDouble(p,&dbl[1]);

        if((strcmp( t, p ) == 0) ||
           (num_valid[0] && num_valid[1] && num[0] == num[1]) ||
           (dbl_valid[0] && dbl_valid[1] && dbl[0] == dbl[1]) )
        {
          match = 1;
          goto clean_oyTextIccDictMatch;
        }
      }
    }
    clean_oyTextIccDictMatch:
      oyStringListRelease_( &texts, n, oyDeAllocateFunc_ );
      oyStringListRelease_( &patterns, p_n, oyDeAllocateFunc_ );
  }

  return match;
}


/** Function oyFilterRegistrationModify
 *  @brief   process a registration string
 *
 *  A semantical overview is given in @ref module_api.
 *
 *  @param[in]     registration        registration key
 *  @param[in]     mode                the processing rule
 *  @param[out]    result              allocated by allocateFunc
 *  @param[in]     allocateFunc        optional user allocator; defaults to 
 *                                     oyAllocateFunc_
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/08/12 (Oyranos: 0.1.11)
 *  @date    2010/08/12
 */
char   oyFilterRegistrationModify    ( const char        * registration,
                                       oyFILTER_REG_MODE_e mode,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc )
{
  if(!result) return 1;
  if(!registration) return -1;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  switch(mode)
  {
  case oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR:
  {
    char ** reg_texts = 0;
    int     reg_texts_n = 0;
    char ** regc_texts = 0;
    int     regc_texts_n = 0;
    char  * reg_text = 0;
    int     i,j;
 
    reg_texts = oyStringSplit_( registration, OY_SLASH_C, &reg_texts_n,
                                oyAllocateFunc_);

    for( i = 0; i < reg_texts_n; ++i)
    {
      regc_texts_n = 0;

      /* level by level */
      regc_texts = oyStringSplit_( reg_texts[i],'.',&regc_texts_n,
                                   oyAllocateFunc_);

      if(i > 0)
        STRING_ADD( reg_text, OY_SLASH );

      for( j = 0; j < regc_texts_n; ++j)
      {
        /* '_' underbar is the feature char; omit such attributes */
        if(regc_texts[j][0] != '_')
        {
          if(j > 0)
            STRING_ADD( reg_text, "." );
          STRING_ADD( reg_text, regc_texts[j] );
        }
      }

      oyStringListRelease_( &regc_texts, regc_texts_n, oyDeAllocateFunc_ );
    }
    oyStringListRelease_( &reg_texts, reg_texts_n, oyDeAllocateFunc_ );


    if(allocateFunc != oyAllocateFunc_)
      *result = oyStringCopy_( reg_text, allocateFunc );
    else
      *result = reg_text;
  }
  break;

  default: return -1;
  }

  return 0;
}

/**
 *  @internal
 *  Function oyFilterCore_New_
 *  @memberof oyFilterCore_s
 *  @brief   allocate and initialise a new filter object
 *
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
oyFilterCore_s * oyFilterCore_New_   ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_CORE_S;
# define STRUCT_TYPE oyFilterCore_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterCore_Copy;
  s->release = (oyStruct_Release_f) oyFilterCore_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/**
 *  @internal
 *  Function oyFilterCore_SetCMMapi4_
 *  @memberof oyFilterCore_s
 *  @brief   lookup and initialise a new filter object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2009/02/28
 */
int          oyFilterCore_SetCMMapi4_( oyFilterCore_s    * s,
                                       oyCMMapi4_s       * cmm_api4 )
{
  int error = !s;
  oyAlloc_f allocateFunc_ = 0;
  static const char * lang = 0;
  int update = 1;

  if(error <= 0)
    allocateFunc_ = s->oy_->allocateFunc_;

  error = !(cmm_api4 && cmm_api4->type == oyOBJECT_CMM_API4_S);

  if(error <= 0)
  {
    s->registration_ = oyStringCopy_( cmm_api4->registration,
                                      allocateFunc_);

    s->category_ = oyStringCopy_( cmm_api4->ui->category, allocateFunc_ );

    /* we lock here as cmm_api4->oyCMMuiGet might not be thread save */
    {
      if(!lang)
        lang = oyLanguage();

      oyObject_Lock( s->oy_, __FILE__, __LINE__ );
      if(lang &&
         oyStrcmp_( oyNoEmptyName_m_(oyLanguage()), lang ) == 0)
        update = 0;

      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
    }

    s->api4_ = cmm_api4;
  }

  if(error && s)
    oyFilterCore_Release( &s );

  return error;
}

/** Function oyFilterCore_New
 *  @memberof oyFilterCore_s
 *  @brief   lookup and initialise a new filter object
 *
 *  back end selection: \n
 *  - the user knows, which kind of filter is requested -> registration, e.g. "//color"
 *  - the user probably knows, which special CMM to use (e.g. lcms, icc, shiva)
 *
 *  @param[in]     registration        the filter registration pattern
 *  @param[in]     options             the supplied filter options
 *  @param[in]     object              the optional object
 *  @return                            a filter core
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/07/27
 */
oyFilterCore_s * oyFilterCore_New    ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyFilterCore_s * s = oyFilterCore_New_( object );
  int error = !s;
  oyCMMapi4_s * api4 = 0;

  if(error <= 0)
  {
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( 0,
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4;
  }

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( s, api4 );

  if(error <= 0 && !options)
  {
#if 0
    s->options_ = api4->oyCMMFilter_ValidateOptions( s, options, 0, &ret );
#endif
  }

  if(error <= 0 && options)
    s->options_ = oyOptions_Copy( options, 0 );

  if(error && s)
  {
    oyFilterCore_Release( &s );
    WARNc1_S("could not create filter: \"%s\"", oyNoEmptyName_m_(registration));
  }

  return s;
}

/**
 *  @internal
 *  Function oyFilterCore_Copy_
 *  @memberof oyFilterCore_s
 *  @brief   real copy a filter object
 *
 *  @param[in]     filter              filter object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
oyFilterCore_s * oyFilterCore_Copy_  ( oyFilterCore_s    * filter,
                                       oyObject_s          object )
{
  oyFilterCore_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!filter || !object)
    return s;

  s = oyFilterCore_New_( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(error <= 0)
  {
    s->registration_ = oyStringCopy_( filter->registration_, allocateFunc_ );
    s->category_ = oyStringCopy_( filter->category_, allocateFunc_ );
    s->options_ = oyOptions_Copy( filter->options_, s->oy_ );
    s->api4_ = filter->api4_;
  }

  return s;
}

/** Function oyFilterCore_Copy
 *  @memberof oyFilterCore_s
 *  @brief   copy or reference a filter object
 *
 *  @param[in]     filter              filter object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
oyFilterCore_s * oyFilterCore_Copy   ( oyFilterCore_s    * filter,
                                       oyObject_s          object )
{
  oyFilterCore_s * s = 0;

  if(!filter)
    return s;

  if(filter && !object)
  {
    s = filter;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterCore_Copy_( filter, object );

  return s;
}
/** Function oyFilterCore_Release
 *  @memberof oyFilterCore_s
 *  @brief   release and zero a filter object
 *
 *  @todo    complete the implementation
 *
 *  @param[in,out] obj                 filter object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
int          oyFilterCore_Release    ( oyFilterCore_s   ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterCore_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FILTER_CORE_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->registration_)
      deallocateFunc( s->registration_ );

    if(s->category_)
      deallocateFunc( s->category_ ); s->category_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyFilterCore_GetText
 *  @memberof oyFilterCore_s
 *  @brief   get text
 *
 *  oyNAME_NAME provides a XML element with child elements and attributes
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetText    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type )
{
  char * text = 0;
  oyFilterCore_s * s = filter;
  int error = !s;

  if(error)
    return 0;

  if(name_type == oyNAME_NAME && !oyObject_GetName(s->oy_, name_type))
  {
    text = oyAllocateWrapFunc_( 512, s->oy_ ? s->oy_->allocateFunc_ : 0 );
    if(!text)
      error = 1;
    sprintf(text, "<oyFilterCore_s registration=\"%s\" category=\"%s\" version=\"%d.%d.%d\"/>\n",
                  s->registration_,
                  s->category_,
                  s->api4_->version[0],
                  s->api4_->version[1],
                  s->api4_->version[2]
           );

#if 0
    if(error <= 0 && filter->profiles_)
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

    if(error <= 0)
      error = oyObject_SetName( s->oy_, text, name_type );

    if(error)
      WARNc_S(_("MEM Error."));
  }

  return oyObject_GetName(filter->oy_, name_type);
}

/** Function oyFilterCore_GetName
 *  @memberof oyFilterCore_s
 *  @brief   get name
 *
 *  provides the original filter names
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetName    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type )
{
  oyFilterCore_s * s = filter;

  if(!s)
    return 0;

  return oyNoEmptyName_m_( filter->api4_->ui->getText( "name", name_type,
                           (oyStruct_s*)filter->api4_->ui ) );
}
/** Function oyFilterCore_CategoryGet
 *  @memberof oyFilterCore_s
 *  @brief   get category string
 *
 *  @param[in,out] filter              filter object
 *  @param         nontranslated       switch for translation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_CategoryGet( oyFilterCore_s    * filter,
                                       int                 nontranslated )
{
  if(!filter)
    return 0;

  return filter->category_;
}

const char * oyFilterCore_WidgetsSet ( oyFilterCore_s    * filter,
                                       const char        * widgets,
                                       int                 flags );
const char * oyFilterCore_WidgetsGet ( oyFilterCore_s    * filter,
                                       int                 flags );



/** Function: oyFilterCores_New
 *  @memberof oyFilterCores_s
 *  @brief   allocate a new Filters list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
OYAPI oyFilterCores_s * OYEXPORT
                   oyFilterCores_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_CORES_S;
# define STRUCT_TYPE oyFilterCores_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterCores_Copy;
  s->release = (oyStruct_Release_f) oyFilterCores_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/**
 *  @internal
 *  Function: oyFilterCores_Copy_
 *  @memberof oyFilterCores_s
 *  @brief   real copy a Filters object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
oyFilterCores_s * oyFilterCores_Copy_
                                     ( oyFilterCores_s   * obj,
                                       oyObject_s          object )
{
  oyFilterCores_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyFilterCores_New( object );
  error = !s;

  if(error <= 0)
  {
    allocateFunc_ = s->oy_->allocateFunc_;
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );
  }

  if(error)
    oyFilterCores_Release( &s );

  return s;
}

/** Function: oyFilterCores_Copy
 *  @memberof oyFilterCores_s
 *  @brief   copy or reference a Filters list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
OYAPI oyFilterCores_s * OYEXPORT
                   oyFilterCores_Copy( oyFilterCores_s   * obj,
                                       oyObject_s          object )
{
  oyFilterCores_s * s = 0;

  if(!obj)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterCores_Copy_( obj, object );

  return s;
}
 
/** Function: oyFilterCores_Release
 *  @memberof oyFilterCores_s
 *  @brief   release and possibly deallocate a Filters list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
               oyFilterCores_Release ( oyFilterCores_s  ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterCores_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FILTER_CORES_S, return 1 )

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


/** Function: oyFilterCores_MoveIn
 *  @memberof oyFilterCores_s
 *  @brief   add a element to a Filters list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
OYAPI oyFilterCores_s * OYEXPORT
                 oyFilterCores_MoveIn( oyFilterCores_s   * list,
                                       oyFilterCore_s   ** obj,
                                       int                 pos )
{
  oyFilterCores_s * s = list;
  int error = !s || s->type_ != oyOBJECT_FILTER_CORES_S;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_FILTER_CORE_S)
  {
    if(!s)
    {
      s = oyFilterCores_New(0);
      error = !s;
    }                                  

    if(error <= 0 && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(error <= 0)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
  }   
  
  return s;
}

/** Function: oyFilterCores_ReleaseAt
 *  @memberof oyFilterCores_s
 *  @brief   release a element from a Filters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
                  oyFilterCores_ReleaseAt( oyFilterCores_s   * list,
                                       int                 pos )
{ 
  int error = !list;

  if(error <= 0 && list->type_ != oyOBJECT_FILTER_CORES_S)
    error = 1;
  
  if(error <= 0)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function: oyFilterCores_Get
 *  @memberof oyFilterCores_s
 *  @brief   get a element of a Filters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
OYAPI oyFilterCore_s * OYEXPORT
                 oyFilterCores_Get   ( oyFilterCores_s   * list,
                                       int                 pos )
{       
  if(list && list->type_ == oyOBJECT_FILTER_CORES_S)
    return (oyFilterCore_s *) oyStructList_GetRefType( list->list_, pos, oyOBJECT_FILTER_CORE_S ); 
  else  
    return 0;
}   

/** Function: oyFilterCores_Count
 *  @memberof oyFilterCores_s
 *  @brief   count the elements in a Filters list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
                 oyFilterCores_Count ( oyFilterCores_s   * list )
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

/** Function oyFilterNode_Create
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
oyFilterNode_s *   oyFilterNode_Create(oyFilterCore_s    * filter,
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

  if(error <= 0)
  {
    s->core = oyFilterCore_Copy( filter, object );
    if(!s->core)
    {
      WARNc2_S("Could not copy filter: %s %s",
               filter->registration_, filter->category_)
      error = 1;
    }

    if(error <= 0)
      s->api7_ = (oyCMMapi7_s*) oyCMMsGetFilterApi_( 0,
                                s->core->registration_, oyOBJECT_CMM_API7_S );
    if(error <= 0 && !s->api7_)
    {
      WARNc2_S("Could not obtain filter api7 for: %s %s",
               filter->registration_, filter->category_)
      error = 1;
    }

    if(s->api7_)
    {
      s->plugs_n_ = s->api7_->plugs_n + s->api7_->plugs_last_add;
      if(s->api7_->plugs_last_add)
        --s->plugs_n_;
      s->sockets_n_ = s->api7_->sockets_n + s->api7_->sockets_last_add;
      if(s->api7_->sockets_last_add)
        --s->sockets_n_;
    }

    if(s->core)
    {
      size_t len = sizeof(oyFilterSocket_s*) *
             (oyFilterNode_EdgeCount( s, 0, 0 ) + 1);
      len = len?len:sizeof(oyFilterSocket_s*);
      s->sockets = allocateFunc_( len );
      memset( s->sockets, 0, len );

      len = sizeof(oyFilterSocket_s*) * (oyFilterNode_EdgeCount( s, 1, 0 ) + 1);
      len = len?len:sizeof(oyFilterSocket_s*);
      s->plugs = allocateFunc_( len );
      memset( s->plugs, 0, len );

      s->relatives_ = allocateFunc_( oyStrlen_(filter->category_) + 24 );
      oySprintf_( s->relatives_, "%d: %s", oyObject_GetId(s->oy_), s->core->category_);
    }
  }

  if(error)
    oyFilterNode_Release( &s );

  return s;
}

/** Function oyFilterNode_NewWith
 *  @memberof oyFilterNode_s
 *  @brief   initialise a new filter node object properly
 *
 *  @param         registration        a registration string, @see module_api
 *  @param         options             options for the filter
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/06/24
 */
oyFilterNode_s *   oyFilterNode_NewWith (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyFilterCore_s * core = oyFilterCore_New( registration, options, object);
  oyFilterNode_s * node = oyFilterNode_Create( core, object );

  oyFilterCore_Release( &core );
  return node;
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

  s = oyFilterNode_Create( node->core, object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;


  if(error <= 0)
  {
    if(error <= 0 && node->backend_data && node->backend_data->copy)
      s->backend_data = (oyPointer_s*) node->backend_data->copy( (oyStruct_s*)
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
/** Function oyFilterNode_Release
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
  uint32_t s_n = 0, p_n = 0, i, n;
  /* ---- start of common object destructor ----- */
  oyFilterNode_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  *obj = 0;

  /* ---- end of common object destructor ------- */

  oyObject_UnRef(s->oy_);

  if(s->sockets)
  {
    n = oyFilterNode_EdgeCount( s, 0, 0 );
    for(i = 0; i < n; ++i)
      if(s->sockets[i]) ++s_n;
  }

  if(s->plugs)
  {
    n = oyFilterNode_EdgeCount( s, 1, 0 );
    for(i = 0; i < n; ++i)
      if(s->plugs[i]) ++p_n;
  }

  if(oyObject_GetRefCount( s->oy_ ) > s_n + p_n)
    return 0;

  oyObject_Ref(s->oy_);

  oyOptions_Release( &s->tags );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->sockets)
    {
      n = oyFilterNode_EdgeCount( s, 0, 0 );
      for(i = 0; i < n; ++i)
        oyFilterSocket_Release( &s->sockets[i] );
    }

    if(s->plugs)
    {
      n = oyFilterNode_EdgeCount( s, 1, 0 );
      for(i = 0; i < n; ++i)
        oyFilterPlug_Release( &s->plugs[i] );
    }

    oyObject_UnRef(s->oy_);

    if(s->relatives_)
      deallocateFunc( s->relatives_ );
    s->relatives_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyFilterNode_EdgeCount
 *  @memberof oyFilterNode_s
 *  @brief   count real and potential connections to a filter node object
 *
 *  @param         node                the node
 *  @param         is_input            1 - plugs; 0 - sockets
 *  @param         flags               specify which number to return
 *                                     - oyranos::OY_FILTEREDGE_FREE: count available
 *                                     - oyranos::OY_FILTEREDGE_CONNECTED: count used
 *                                     - oyranos::OY_FILTEREDGE_LASTTYPE: account only
 *                                       for the last connector type
 *  @return                            the number of possible edges
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/27
 */
int            oyFilterNode_EdgeCount( oyFilterNode_s    * node,
                                       int                 is_input,
                                       int                 flags )
{
  oyFilterNode_s * s = node;
  int n = 0, start, i,
      possible = 0,
      connected = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  if(!node->core || !node->api7_)
    return 0;

  /* plugs */
  if(is_input)
  {
    if(oyToFilterEdge_LastType_m(flags))
    {
      possible = s->api7_->plugs_last_add + 1;
      start = s->api7_->plugs_n - 1;
    } else
    {
      possible = node->plugs_n_;
      start = 0;
    }

    if(node->plugs)
      for(i = start; i < possible; ++i)
        if(node->plugs[i] && node->plugs[i]->remote_socket_)
          ++connected;

    if(oyToFilterEdge_Free_m(flags))
      n = possible - connected;
    else if(oyToFilterEdge_Connected_m(flags))
      n = connected;
    else
      n = possible;

  } else
  /* ... sockets */
  {
    if(oyToFilterEdge_LastType_m(flags))
    {
      possible = s->api7_->sockets_last_add + 1;
      start = s->api7_->sockets_n - 1;
    } else
    {
      possible = node->sockets_n_;
      start = 0;
    }

    if(node->sockets)
      for(i = 0; i < possible; ++i)
        if(node->sockets[i])
          connected += oyFilterPlugs_Count(node->sockets[i]->requesting_plugs_);

    if(oyToFilterEdge_Free_m(flags))
      n = possible ? INT32_MAX : 0;
    else if(oyToFilterEdge_Connected_m(flags))
      n = connected;
    else
      n = possible;
  }


  return n;
}

/** Function oyFilterNode_Connect
 *  @memberof oyFilterNode_s
 *  @brief   connect two nodes by a edge
 *
 *  @param         input               the node to provide a socket
 *  @param         socket_nick         name of socket
 *  @param         output              the node providing a plug
 *  @param         plug_nick           name of plug
 *  @param         flags               unused
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/26 (Oyranos: 0.1.10)
 *  @date    2009/02/26
 */
int            oyFilterNode_Connect  ( oyFilterNode_s    * input,
                                       const char        * socket_nick,
                                       oyFilterNode_s    * output,
                                       const char        * plug_nick,
                                       int                 flags )
{
  oyFilterNode_s * s = input;
  int error = !s;
  oyFilterPlug_s * out_plug = 0;
  oyFilterSocket_s * output_socket = 0,
                   * in_socket = 0;
  int pos, out_pos;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )
  s = output;
  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  if(error <= 0)
  {
    if(error <= 0 &&
       (!s->core || !s->core->api4_))
    {
      WARNc2_S( "%s: %s",
      _("attempt to add a incomplete filter"), s->relatives_ );
      error = 1;
    }
    if(error <= 0 &&
       !oyFilterNode_EdgeCount( input, 0, OY_FILTEREDGE_FREE ))
    {
      WARNc2_S( "%s: %s", "input node has no free socket",
                oyFilterCore_GetName( input->core, oyNAME_NAME) );
      error = 1;
    }

    if(error <= 0)
    {
      if(socket_nick)
        pos = oyFilterNode_GetConnectorPos( input, 0, socket_nick, 0,
                                            0 );
      else
        pos = 0;
      if(pos >= 0)
      {
        in_socket = oyFilterNode_GetSocket( input, pos );
        in_socket = oyFilterSocket_Copy( in_socket, 0 );
      }

      if(plug_nick)
        out_pos = oyFilterNode_GetConnectorPos( output, 1, plug_nick, 0,
                                                OY_FILTEREDGE_FREE );
      else
        out_pos = 0;
      if(out_pos >= 0)
      {
        out_plug = oyFilterNode_GetPlug( output, out_pos );
        out_plug = oyFilterPlug_Copy( out_plug, 0 );
      }

      if(!out_plug)
      {
        WARNc3_S( "\n  %s: \"%s\" %s", "Could not find plug for filter",
                  oyFilterCore_GetName( output->core, oyNAME_NAME), socket_nick );
        error = 1;
      }

      if(error <= 0)
      {
        if(oyFilterNode_ConnectorMatch( input, pos, out_plug ))
          output_socket = oyFilterNode_GetSocket( output, 0 );
        else
        {
          WARNc3_S( "\n  %s: %s -> %s", "Filter connectors do not match",
                    input->relatives_, output->relatives_ );
          error = 1;
        }
      }

      if(error <= 0 && output_socket && !output_socket->data && 
         in_socket && in_socket->data)
        output_socket->data = in_socket->data->copy( in_socket->data, 0 );

      if(error <= 0)
        oyFilterPlug_ConnectIntoSocket( &out_plug, &in_socket );

    } else
      WARNc2_S( "%s: %d", _("?? Nothing to add ??"),
                oyObject_GetId(input->oy_));
  }

  return error;
}

/** Function oyFilterNode_Disconnect
 *  @memberof oyFilterNode_s
 *  @brief   disconnect two nodes by a edge
 *
 *  @param         edge                plug
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int            oyFilterNode_Disconnect(oyFilterPlug_s    * edge )
{
  oyFilterPlug_s * s = edge;
  oyFilterSocket_Callback( s, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( &s->remote_socket_ );
  return 0;
}

/** Function: oyFilterNode_ShowConnector
 *  @memberof oyFilterNode_s
 *  @brief   get a connector description from a filter module
 *
 *  The path to obtain a new connector.
 *  The filter can say it has more connectors to provide for a certain kind of 
 *  static connector eigther described in oyCMMapi4_s::inputs or
 *  oyCMMapi4_s::outputs.
 *
 *  @param[in]   node                the module filter node
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

  if(!node || !node->core || node->type_ != oyOBJECT_FILTER_NODE_S ||
     !node->api7_)
    return 0;

  object = oyObject_New ();

  if(is_plug)
  {
    if(node->api7_->plugs_n <= as_pos &&
       as_pos < oyFilterNode_EdgeCount( node, 1, 0 ))
      as_pos = node->api7_->plugs_n - 1;

    if(node->api7_->plugs_n > as_pos)
      pattern = oyConnector_Copy( node->api7_->plugs[as_pos], object );
  } else {
    if(node->api7_->sockets_n <= as_pos &&
       as_pos < oyFilterNode_EdgeCount( node, 0, 0 ))
      as_pos = node->api7_->sockets_n - 1;

    if(node->api7_->sockets_n > as_pos)
      pattern = oyConnector_Copy( node->api7_->sockets[as_pos], object );
  }

  oyObject_Release( &object );

  return pattern;
}

/** Function: oyFilterNode_ConnectorMatch
 *  @memberof oyFilterNode_s
 *  @brief   check if a connector match to a FilterNode
 *
 *  @param         node_first          first node
 *  @param         pos_first           position of connector from first node
 *  @param         plug                second connector
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI int  OYEXPORT
                 oyFilterNode_ConnectorMatch (
                                       oyFilterNode_s    * node_first,
                                       int                 pos_first,
                                       oyFilterPlug_s    * plug )
{
  int match = 0;
  oyConnector_s * a = 0,  * b = plug->pattern;
  char * reg = 0,
       * tmp = 0;

  if(node_first && node_first->type_ == oyOBJECT_FILTER_NODE_S &&
     node_first->core)
    a = oyFilterNode_ShowConnector( node_first, pos_first, 0 );

  if(a && b)
  {
    oyFilterSocket_s * sock_first = oyFilterNode_GetSocket( node_first, pos_first );
    match = 1;

    if(!b->is_plug)
      match = 0;

    if(match)
    {
      /** Check if basic types match. */
      reg = oyStringCopy_( "//", oyAllocateFunc_ );
      tmp = oyFilterRegistrationToText( oyConnector_GetReg(a),
                                        oyFILTER_REG_TYPE, 0 );
      STRING_ADD( reg, tmp );
      if(tmp) oyFree_m_( tmp );
      match = oyFilterRegistrationMatch( reg, oyConnector_GetReg(b),
                                         0 );
      if(reg) oyFree_m_(reg);
    }

    /** More detailed checking is done in oyCMMapi5_s. */
    if(match && oyConnector_GetMatch(sock_first->pattern))
      oyConnector_GetMatch(sock_first->pattern)( sock_first, plug );
  }

  oyConnector_Release( &a );

  return match;
}

/** Function oyFilterNode_GetConnectorPos
 *  @memberof oyFilterNode_s
 *  @brief   get a oyFilterSocket_s or oyFilterPlug_s position from a FilterNode
 *
 *  @param         node                filter node
 *  @param         is_input            1 - plugs; 0 - sockets
 *  @param         pattern             the pattern to be found in the
 *                                     oyConnector_s::connector_type of the
 *                                     searched plug or socket. Its a
 *                                     @ref registration string. E.g. a typical
 *                                     data connection: "//" OY_TYPE_STD "/data"
 *                                     See as well oyranos::oyCONNECTOR_e.
 *  @param         nth_of_type         the position in the group of the
 *                                     connector type for this filter; Note
 *                                     this parameter makes only sense for the
 *                                     last filter defined connector, as only
 *                                     this one can occure multiple times.
 *  @param         flags               specify which status to return
 *                                     - zero means: take all into account
 *                                     - oyranos::OY_FILTEREDGE_FREE: next free available
 *                                     - oyranos::OY_FILTEREDGE_CONNECTED: consider used
 *  @return                            the absolute position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/26 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT
               oyFilterNode_GetConnectorPos (
                                       oyFilterNode_s    * node,
                                       int                 is_input,
                                       const char        * pattern,
                                       int                 nth_of_type,
                                       int                 flags )
{
  oyFilterNode_s * s = node;
  int pos = -1,
      i, j, n, n2,
      nth = -1;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return pos )

  if(!pattern)
  {
    WARNc1_S("no ID argument given %s", s->relatives_ );
    return pos;
  }

  if(nth_of_type == -1)
    nth_of_type = 0;

  /* plugs */
  if(is_input)
  {
    n = node->api7_->plugs_n;
    for( i = 0; i < n; ++i )
    {
      if(oyFilterRegistrationMatch( oyConnector_GetReg(node->api7_->plugs[i]),
                                    pattern, 0))
      {
        if( i == n - 1 && node->api7_->plugs_last_add)
          n2 = node->api7_->plugs_last_add;
        else
          n2 = 0;

        for( j = 0; j <= n2; ++j )
        {
          if(oyToFilterEdge_Free_m(flags))
          {
            if( node->plugs[i + j] && node->plugs[i + j]->remote_socket_ )
              continue;
            else
              ++nth;

          } else
          if(oyToFilterEdge_Connected_m(flags))
          {
            if( node->plugs[i + j] && node->plugs[i + j]->remote_socket_ )
              ++nth;
            else
              continue;

          } else
            ++nth;

          if( nth == nth_of_type )
          {
            pos = i + j;
            return pos;
          }
        }
      }
    }
  } else
  /* ... or sockets */
  {
    /* 1. count possible connectors */
    n = node->api7_->sockets_n;
    for( i = 0; i < n; ++i )
    {
      /* 2. compare pattern argument with the socket type */
      if(oyFilterRegistrationMatch( oyConnector_GetReg(node->api7_->sockets[i]),
                                    pattern, 0))
      {

        /* 3. iterate through at least connectors or connectors that where added
              to the last one */
        if( i == n - 1 && node->api7_->sockets_last_add)
          n2 = node->api7_->sockets_last_add;
        else
          n2 = 0;

        for( j = 0; j <= n2; ++j )
        {
          /* 3.1 check only unused connectors */
          if(oyToFilterEdge_Free_m(flags))
          {
            if( node->sockets[i + j] &&
                oyFilterPlugs_Count( node->sockets[i + j]->requesting_plugs_ ) )
              continue;
            else
              ++nth;

          } else
          /* 3.2 check only used connectors */
          if(oyToFilterEdge_Connected_m(flags))
          {
            if( node->sockets[i + j] &&
                oyFilterPlugs_Count( node->sockets[i + j]->requesting_plugs_ ) )
              ++nth;
            else
              continue;

          } else
          /* 3.3 count all connectors */
            ++nth;

          /* 4. check the type relative positional parameter */
          if( nth == nth_of_type )
          {
            /* 4.1 return as we otherwise would need to leave two loops */
            pos = i + j;
            return pos;
          }
        }
      }
    }
  }

  return pos;
}


/** Function oyFilterNode_GetSocket
 *  @memberof oyFilterNode_s
 *  @brief   get a oyFilterSocket_s of type from a FilterNode
 *
 *  @param         node                filter node
 *  @param         pos                 absolute position of connector
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
     pos < oyFilterNode_EdgeCount( node, 0, 0 ))
  {
    oyAlloc_f allocateFunc_ = node->oy_->allocateFunc_;

    if(!node->sockets)
    {
      size_t len = sizeof(oyFilterSocket_s*) *
                   (oyFilterNode_EdgeCount( node, 0, 0 ) + 1);
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

/** Function oyFilterNode_GetPlug
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
     pos < oyFilterNode_EdgeCount( node, 1, 0 ))
  {
    oyAlloc_f allocateFunc_ = node->oy_->allocateFunc_;

    if(!node->plugs)
    {
      size_t len = sizeof(oyFilterPlug_s*) *
                   (oyFilterNode_EdgeCount( node, 1, 0 ) + 1);
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

oyOptions_s* oyFilterNode_OptionsSet ( oyFilterNode_s    * node,
                                       oyOptions_s       * options,
                                       int                 flags );
/**
 *  @internal
 *  Function oyFilterNodeObserve_
 *  @memberof oyFilterNode_s
 *  @brief   observe filter options
 *
 *  Implements oyObserver_Signal_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int      oyFilterNodeObserve_        ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  int i,n;
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s * node = 0;
  oyObserver_s * obs = observer;

  if(observer && observer->model &&
     observer->model->type_ == oyOBJECT_OPTIONS_S &&
     observer->observer && observer->observer->type_== oyOBJECT_FILTER_NODE_S)
  {
    if(oy_debug_signals)
      WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    node = (oyFilterNode_s*)observer->observer;

    /* invalidate the context */
    if(node->backend_data)
      node->backend_data->release( (oyStruct_s**)&node->backend_data );

    n = oyFilterNode_EdgeCount( node, 0, 0 );
    for(i = 0; i < n; ++i)
    {
      socket = oyFilterNode_GetSocket( node, i );
      /* forward to the downward graph */
      oyFilterSocket_SignalToGraph( socket, signal_type );
    }
  }

  return handled;
}

/** Function oyFilterNode_OptionsGet
 *  @memberof oyFilterNode_s
 *  @brief   get filter options
 *
 *  @param[in,out] node                filter object
 *  @param         flags               see oyOptions_s::oyOptions_ForFilter()
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/06/26
 */
oyOptions_s* oyFilterNode_OptionsGet ( oyFilterNode_s    * node,
                                       int                 flags )
{
  oyOptions_s * options = 0;
  oyFilterNode_s * s = node;
  int error = 0;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  if(flags || !node->core->options_)
  {
    options = oyOptions_ForFilter_( node->core, flags, node->core->oy_ );
    if(!node->core->options_)
      node->core->options_ = oyOptions_Copy( options, 0 );
    else
      error = oyOptions_Filter( &node->core->options_, 0, 0,
                                oyBOOLEAN_UNION,
                                0, options );
    if(!node->core->options_)
      node->core->options_ = oyOptions_New( 0 );
  }

  options = oyOptions_Copy( node->core->options_, 0 );

  /** Observe exported options for changes and propagate to a existing graph. */
  error = oyOptions_ObserverAdd( options, (oyStruct_s*)node,
                                 0, oyFilterNodeObserve_ );

  return options;
}

/** Function oyFilterNode_UiGet
 *  @memberof oyFilterNode_s
 *  @brief   get filter options XFORMS
 *
 *  @param[in,out] node                filter object
 *  @param[out]    ui_text             XFORMS fitting to the node Options
 *  @param[out]    namespaces          additional XML namespaces
 *  @param         allocateFunc        optional user allocator
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/29 (Oyranos: 0.1.10)
 *  @date    2009/08/31
 */
int            oyFilterNode_UiGet    ( oyFilterNode_s     * node,
                                       char              ** ui_text,
                                       char             *** namespaces,
                                       oyAlloc_f            allocateFunc )
{
  int error = 0;
  oyFilterNode_s * s = node;
  oyOptions_s * options = 0;
  char * text = 0,
       * tmp = 0;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(!error)
    options = oyFilterNode_OptionsGet( node, 0 );

  if(!error)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i,j = 0;
    oyCMMapi9_s * cmm_api9 = 0;
    char * class, * api_reg;
    const char * reg = node->core->registration_;

    class = oyFilterRegistrationToText( reg, oyFILTER_REG_TYPE, 0 );
    api_reg = oyStringCopy_("//", oyAllocateFunc_ );
    STRING_ADD( api_reg, class );
    oyFree_m_( class );

    apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S, 
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 0,0 );
    apis_n = oyCMMapiFilters_Count( apis );
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

      if(oyFilterRegistrationMatch( reg, cmm_api9->pattern, 0 ))
      {
        if(cmm_api9->oyCMMuiGet)
          error = cmm_api9->oyCMMuiGet( options, &tmp, oyAllocateFunc_ );

        if(error)
        {
          WARNc2_S( "%s %s",_("error in module:"), cmm_api9->registration );
          return 1;

        } else
        if(tmp)
        {
          STRING_ADD( text, tmp );
          STRING_ADD( text, "\n" );
          oyFree_m_(tmp);

          if(namespaces && cmm_api9->xml_namespace)
          {
            if(j == 0)
            {
              size_t len = (apis_n - i + 1) * sizeof(char*);
              *namespaces = allocateFunc( len );
              memset(*namespaces, 0, len);
            }
            *namespaces[j] = oyStringCopy_( cmm_api9->xml_namespace,
                                            allocateFunc );
            ++j;
            namespaces[j] = 0;
          }
        }
      }

      if(cmm_api9->release)
        cmm_api9->release( (oyStruct_s**)&cmm_api9 );
    }
    oyCMMapiFilters_Release( &apis );
  }

  if(!error && node->core->api4_->ui->oyCMMuiGet)
  {
    /* @todo and how to mix in the values? */
    error = node->core->api4_->ui->oyCMMuiGet( options, &tmp, oyAllocateFunc_ );
    if(tmp)
    {
      STRING_ADD( text, tmp );
      oyFree_m_(tmp);
    }
  }

  oyOptions_Release( &options );

  if(error <= 0 && text)
  {
    *ui_text = oyStringCopy_( text, allocateFunc );
  }

  return error;
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
  hashTextAdd_m( oyFilterCore_GetText( node->core, oyNAME_NAME ) );

  /* pick all plug (input) data */
  in_datas = oyFilterNode_DataGet_( node, 1 );

  /* pick all sockets (output) data */
  out_datas = oyFilterNode_DataGet_( node, 0 );

  /* make a description */
  tmp = oyContextCollectData_( (oyStruct_s*)node, s->core->options_,
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

/** Function oyFilterNode_GetId
 *  @memberof oyFilterNode_s
 *  @brief   get the object Id
 *
 *  @param[in]     node                filter node
 *  @return                            the object Id
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/02/25
 */
OYAPI int  OYEXPORT
               oyFilterNode_GetId    ( oyFilterNode_s    * node )
{
  oyFilterNode_s * s = node;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return -1 )

  return oyObject_GetId( node->oy_ );
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

/** Function oyFilterNode_TextToInfo_
 *  @memberof oyFilterNode_s
 *  @brief   serialise filter node to binary
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" text tag.
 *  Not useable for binary contexts.
 *
 *  This function is currently a ICC only thing and yet just for debugging
 *  useful.
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
oyPointer    oyFilterNode_TextToInfo_( oyFilterNode_s    * node,
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

/** Function oyFilterNode_DataGet
 *  @memberof oyFilterNode_s
 *  @brief   get process data from a filter socket
 *
 *  @param[in]     node                filter node
 *  @param[in]     socket_pos          position of socket
 *  @return                            the data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
oyStruct_s *   oyFilterNode_DataGet  ( oyFilterNode_s    * node,
                                       int                 socket_pos )
{
  oyFilterNode_s * s = node;
  oyStruct_s * data = 0;
  oyFilterSocket_s * socket = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  socket = oyFilterNode_GetSocket( node, socket_pos );
  if(socket)
    data = socket->data;

  return data;
}

/** Function oyFilterNode_DataSet
 *  @memberof oyFilterNode_s
 *  @brief   Set process data to a filter socket
 *
 *  @param[in,out] node                filter node
 *  @param[in]     data                data
 *  @param[in]     socket_pos          position of socket
 *  @param[in]     object              a object to not only set a reference
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int            oyFilterNode_DataSet  ( oyFilterNode_s    * node,
                                       oyStruct_s        * data,
                                       int                 socket_pos,
                                       oyObject_s        * object )
{
  oyFilterNode_s * s = node;
  oyFilterSocket_s * socket = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  socket = oyFilterNode_GetSocket( node, socket_pos );
  
  if(socket)
  {
    if(socket->data && socket->data->release)
      socket->data->release( &socket->data );

    if(data && data->copy)
      socket->data = data->copy( data, object );
    else
      socket->data = data;
  }

  return 0;
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
  int i, n;

  if(error <= 0)
  {
    datas = oyStructList_New(0);

    if(get_plug)
    {
          /* pick all plug (input) data */
          n = oyFilterNode_EdgeCount( node, 1, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->plugs[i])
          {
            data = 0;
            if(node->plugs[i]->remote_socket_->data)
              data = node->plugs[i]->remote_socket_->data->copy( node->plugs[i]->remote_socket_->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
            ++i;
          }
    } else
    {
          /* pick all sockets (output) data */
          n = oyFilterNode_EdgeCount( node, 0, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->sockets[i])
          {
            data = 0;
            if(node->sockets[i]->data)
              data = node->sockets[i]->data->copy( node->sockets[i]->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
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
 *  @brief   set module context in a filter 
 *
 *  The api4 data is passed to a interpolator specific transformer. The result
 *  of this transformer will on request be cached by Oyranos as well.
 *
 *  @param[in]     node                filter
 *  @param[in,out] blob                context to fill
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 *  @date    2009/06/12
 */
int          oyFilterNode_ContextSet_( oyFilterNode_s    * node,
                                       oyBlob_s          * blob )
{
  int error = 0;
  oyFilterCore_s * s = node->core;

  if(error <= 0)
  {
          size_t size = 0;
          oyHash_s * hash = 0,
                   * hash_out = 0;
          const char * hash_text_ = 0;
          char * hash_text = 0,
               * hash_temp = 0;
          int hash_text_len;
          oyPointer ptr = 0;
          oyPointer_s * cmm_ptr = 0,
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

          hash_text_len = oyStrlen_( hash_text_ );

          hash_text = oyAllocateFunc_(hash_text_len + 16);
          oySprintf_( hash_text, "%s:%s", node->api7_->context_type,
                                          hash_text_ );

          if(oy_debug == 1)
          {
            size = 0;
            ptr = oyFilterNode_TextToInfo_( node, &size, oyAllocateFunc_ );
            if(ptr)
              oyWriteMemToFile_( "test_dbg_colour.icc", ptr, size );
          }

          /* 2. query in cache for api7 */
          hash_out = oyCMMCacheListGetEntry_( hash_text );

          if(error <= 0)
          {
            /* 3. check and 3.a take*/
            cmm_ptr_out = (oyPointer_s*) oyHash_GetPointer( hash_out,
                                                        oyOBJECT_POINTER_S);

            if(!(cmm_ptr_out && oyPointer_GetPointer(cmm_ptr_out)) || blob)
            {
              oySprintf_( hash_text, "%s:%s", s->api4_->context_type, 
                                              hash_text_ );
              /* 2. query in cache for api4 */
              hash = oyCMMCacheListGetEntry_( hash_text );
              cmm_ptr = (oyPointer_s*) oyHash_GetPointer( hash,
                                                        oyOBJECT_POINTER_S);

              if(!cmm_ptr)
              {
                size = 0;
                cmm_ptr = oyPointer_New(0);
              }

              /* write the context to memory */
              if(blob)
              {

                error = oyOptions_SetFromText( &node->tags, "////verbose",
                                               "true", OY_CREATE_NEW );

                /* oy_debug is used to obtain a complete data set */
                ptr = s->api4_->oyCMMFilterNode_ContextToMem( node, &size,
                                                              oyAllocateFunc_ );
                oyBlob_SetFromData( blob, ptr, size, s->api4_->context_type );
                error = oyOptions_SetFromText( &node->tags, "////verbose",
                                               "false", 0 );

                goto clean;
              }

              if(!oyPointer_GetPointer(cmm_ptr))
              {
                /* 3b. ask CMM */
                ptr = s->api4_->oyCMMFilterNode_ContextToMem( node, &size,
                                                              oyAllocateFunc_ );

                if(!ptr || !size)
                {
                  oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*) node, 
                  OY_DBG_FORMAT_ "no device link for caching", OY_DBG_ARGS_);
                  error = 1;
                  oyPointer_Release( &cmm_ptr );
                }

                if(!error)
                {
                  error = oyPointer_Set( cmm_ptr, s->api4_->id_,
                                         s->api4_->context_type,
                                    ptr, "oyPointerRelease", oyPointerRelease);
                  oyPointer_SetSize( cmm_ptr, size );

                  /* 3b.1. update cache entry */
                  error = oyHash_SetPointer( hash, (oyStruct_s*) cmm_ptr);
                }
              }


              if(error <= 0 && cmm_ptr && oyPointer_GetPointer(cmm_ptr))
              {
                if(node->backend_data && node->backend_data->release)
                node->backend_data->release( (oyStruct_s**)&node->backend_data);

                if( oyStrcmp_( node->api7_->context_type,
                               s->api4_->context_type ) != 0 )
                {
                  cmm_ptr_out = oyPointer_New(0);
                  error = oyPointer_Set( cmm_ptr_out, node->api7_->id_,
                                         node->api7_->context_type, 0, 0, 0);

                  /* search for a convertor and convert */
                  oyPointer_ConvertData( cmm_ptr, cmm_ptr_out, node );
                  node->backend_data = cmm_ptr_out;
                  /* 3b.1. update cache entry */
                  error = oyHash_SetPointer( hash_out,
                                              (oyStruct_s*) cmm_ptr_out);

                } else
                  node->backend_data = oyPointer_Copy( cmm_ptr, 0 );
              }

              if(oy_debug == 1)
              {
                if(ptr && size && node->backend_data)
                  oyWriteMemToFile_( "test_dbg_colour_dl.icc", ptr, size );
              }

            } else
              node->backend_data = cmm_ptr_out;

          }


    clean:
    if(hash_temp) oyDeAllocateFunc_(hash_temp);
    if(hash_text) oyDeAllocateFunc_(hash_text);
  }

  return error;
}


/** Function oyFilterNodes_New
 *  @memberof oyFilterNodes_s
 *  @brief   allocate a new FilterNodes list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterNodes_s * OYEXPORT
           oyFilterNodes_New         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_NODES_S;
# define STRUCT_TYPE oyFilterNodes_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterNodes_Copy;
  s->release = (oyStruct_Release_f) oyFilterNodes_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/** @internal
 *  Function oyFilterNodes_Copy_
 *  @memberof oyFilterNodes_s
 *  @brief   real copy a FilterNodes object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
oyFilterNodes_s * oyFilterNodes_Copy_
                                     ( oyFilterNodes_s   * obj,
                                       oyObject_s          object )
{
  oyFilterNodes_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyFilterNodes_New( object );
  error = !s;

  if(!error)
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );

  if(error)
    oyFilterNodes_Release( &s );

  return s;
}

/** Function oyFilterNodes_Copy
 *  @memberof oyFilterNodes_s
 *  @brief   copy or reference a FilterNodes list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterNodes_s * OYEXPORT
           oyFilterNodes_Copy        ( oyFilterNodes_s   * obj,
                                       oyObject_s          object )
{
  oyFilterNodes_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterNodes_Copy_( obj, object );

  return s;
}
 
/** Function oyFilterNodes_Release
 *  @memberof oyFilterNodes_s
 *  @brief   release and possibly deallocate a FilterNodes list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_Release     ( oyFilterNodes_s  ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterNodes_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

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


/** Function oyFilterNodes_MoveIn
 *  @memberof oyFilterNodes_s
 *  @brief   add a element to a FilterNodes list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_MoveIn      ( oyFilterNodes_s   * list,
                                       oyFilterNode_s   ** obj,
                                       int                 pos )
{
  oyFilterNodes_s * s = list;
  int error = 0;

  if(s)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  if(obj && *obj && (*obj)->type_ == oyOBJECT_FILTER_NODE_S)
  {
    if(!s)
    {
      s = oyFilterNodes_New(0);
      error = !s;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos,
                                   OY_OBSERVE_AS_WELL );
  }   
  
  return error;
}

/** Function oyFilterNodes_ReleaseAt
 *  @memberof oyFilterNodes_s
 *  @brief   release a element from a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_ReleaseAt   ( oyFilterNodes_s   * list,
                                       int                 pos )
{ 
  int error = !list;
  oyFilterNodes_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  if(!error && list->type_ != oyOBJECT_FILTER_NODES_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function oyFilterNodes_Get
 *  @memberof oyFilterNodes_s
 *  @brief   get a element of a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterNodes_Get         ( oyFilterNodes_s   * list,
                                       int                 pos )
{       
  int error = !list;
  oyFilterNodes_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  if(!error)
    return (oyFilterNode_s *) oyStructList_GetRefType( list->list_, pos, oyOBJECT_FILTER_NODE_S ); 
  else  
    return 0;
}   

/** Function oyFilterNodes_Count
 *  @memberof oyFilterNodes_s
 *  @brief   count the elements in a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_Count       ( oyFilterNodes_s   * list )
{       
  int error = !list;
  oyFilterNodes_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  if(!error)
    return oyStructList_Count( list->list_ );
  else return 0;
}


/** Function oyFilterGraph_New
 *  @memberof oyFilterGraph_s
 *  @brief   allocate a new FilterGraph object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_New         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_GRAPH_S;
# define STRUCT_TYPE oyFilterGraph_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterGraph_Copy;
  s->release = (oyStruct_Release_f) oyFilterGraph_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->options = oyOptions_New( 0 );

  return s;
}

/** @internal
 *  Function oyAdjacencyListAdd_
 *  @brief   get a graphs adjazency list
 *
 *  Try to add an edge, if not yet found in the scanned graph.
 *  If the edge/plug was added, return success.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/03/06
 */
int    oyAdjacencyListAdd_           ( oyFilterPlug_s    * plug,
                                       oyFilterNodes_s   * nodes,
                                       oyFilterPlugs_s   * edges,
                                       const char        * selector,
                                       int                 flags )
{
  int added = 0, found = 0,
      i,n;
  oyFilterPlug_s * p = 0;
  oyFilterNode_s * node = 0;

  if(selector &&
     (oyOptions_FindString( plug->node->tags, selector, 0 ) == 0 &&
      oyOptions_FindString( plug->remote_socket_->node->tags, selector,0) == 0))
    return added;

  n = oyFilterPlugs_Count( edges );
  for(i = 0; i < n; ++i)
  {
    p = oyFilterPlugs_Get( edges, i );
    if(oyObject_GetId( p->oy_ ) == oyObject_GetId( plug->oy_ ))
      found = 1;
    oyFilterPlug_Release( &p );
  }

  if(!found)
  {
    p = oyFilterPlug_Copy(plug, 0);
    oyFilterPlugs_MoveIn( edges, &p, -1 );
    added = !found;


    found = 0;
    {
      n = oyFilterNodes_Count( nodes );
      for(i = 0; i < n; ++i)
      {
        node = oyFilterNodes_Get( nodes, i );
        if(oyObject_GetId( plug->node->oy_ ) == oyObject_GetId( node->oy_))
          found = 1;
        oyFilterNode_Release( &node );
      }
      if(!found)
      {
        node = oyFilterNode_Copy( plug->node, 0 );
        oyFilterNodes_MoveIn( nodes, &node, -1 );
      }
    }

    {
      found = 0;
      n = oyFilterNodes_Count( nodes );
      for(i = 0; i < n; ++i)
      {
        node = oyFilterNodes_Get( nodes, i );
        if(plug->remote_socket_ && plug->remote_socket_->node)
        {
          if(oyObject_GetId( plug->remote_socket_->node->oy_ ) ==
             oyObject_GetId( node->oy_ ))
            found = 1;

        } else
          found = 1;

        oyFilterNode_Release( &node );
      }
      if(!found)
      {
        node = oyFilterNode_Copy( plug->remote_socket_->node, 0 );
        oyFilterNodes_MoveIn( nodes, &node, -1 );
      }
    }
  }

  return added;
}

/** @internal
 *  Function oyFilterNode_AddToAdjacencyLst_
 *  @brief   get a graphs adjazency list
 *
 *  @param[in]     s                   the start node
 *  @param[in,out] nodes               the collected nodes
 *  @param[in,out] edges               the collected edges
 *  @param[in]     mark                a search string to get sub graphs
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int  oyFilterNode_AddToAdjacencyLst_ ( oyFilterNode_s    * s,
                                       oyFilterNodes_s   * nodes,
                                       oyFilterPlugs_s   * edges,
                                       const char        * mark,
                                       int                 flags )
{
  int n, i, j, p_n;
  oyFilterPlug_s * p = 0;

  /* Scan the input/plug side for unknown nodes, add these and continue in
   * the direction of previous unknown edges...
   */
  if(!(flags & OY_INPUT))
  {
    n = oyFilterNode_EdgeCount( s, 1, 0 );
    for( i = 0; i < n; ++i )
    {
      if( s->plugs[i] && s->plugs[i]->remote_socket_ )
        if(oyAdjacencyListAdd_( s->plugs[i], nodes, edges, mark, flags ))
          oyFilterNode_AddToAdjacencyLst_( s->plugs[i]->remote_socket_->node, 
                                           nodes, edges, mark, flags );
    }
  }

  /* ... same on the output/socket side */
  if(!(flags & OY_OUTPUT))
  {
    n = oyFilterNode_EdgeCount( s, 0, 0 );
    for( i = 0; i < n; ++i )
    {
      if( s->sockets[i] && s->sockets[i]->requesting_plugs_ )
      {
        p_n = oyFilterPlugs_Count( s->sockets[i]->requesting_plugs_ );
        for( j = 0; j < p_n; ++j )
        {
          p = oyFilterPlugs_Get( s->sockets[i]->requesting_plugs_, j );

          if(oyAdjacencyListAdd_( p, nodes, edges, mark, flags ))
            oyFilterNode_AddToAdjacencyLst_( p->node,
                                             nodes, edges, mark, flags );
        }
      }
    }
  }

  return 0;
}

/** Function oyFilterGraph_FromNode
 *  @memberof oyFilterNode_s
 *  @brief   get a graphs adjazency list
 *
 *  @param[in]     node                filter node
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *  @return                            the graph
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_FromNode    ( oyFilterNode_s    * node,
                                       int                 flags )
{
  oyFilterNode_s * s = node;
  oyFilterGraph_s * graph = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  {
    graph = oyFilterGraph_New( 0 );
    oyFilterGraph_SetFromNode( graph, s, 0, flags );
  }

  return graph;
}


/** @internal
 *  Function oyFilterGraph_Copy_
 *  @memberof oyFilterGraph_s
 *  @brief   real copy a FilterGraph object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
oyFilterGraph_s * oyFilterGraph_Copy_
                                     ( oyFilterGraph_s   * obj,
                                       oyObject_s          object )
{
  oyFilterGraph_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyFilterGraph_New( object );
  error = !s;

  s->nodes = oyFilterNodes_Copy( obj->nodes, 0 );
  s->edges = oyFilterPlugs_Copy( obj->edges, 0 );
  s->options = oyOptions_Copy( obj->options, object );

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;
  }

  if(error)
    oyFilterGraph_Release( &s );

  return s;
}

/** Function oyFilterGraph_Copy
 *  @memberof oyFilterGraph_s
 *  @brief   copy or reference a FilterGraph object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_Copy        ( oyFilterGraph_s   * obj,
                                       oyObject_s          object )
{
  oyFilterGraph_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterGraph_Copy_( obj, object );

  return s;
}
 
/** Function oyFilterGraph_Release
 *  @memberof oyFilterGraph_s
 *  @brief   release and possibly deallocate a FilterGraph object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterGraph_Release     ( oyFilterGraph_s  ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterGraph_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyFilterNodes_Release( &s->nodes );
  oyFilterPlugs_Release( &s->edges );
  oyOptions_Release( &s->options );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyFilterGraph_PrepareContexts
 *  @memberof oyFilterGraph_s
 *  @brief   iterate over a filter graph and possibly prepare contexts
 *
 *  @param[in,out] graph               a filter graph
 *  @param[in]     flags               1 - enforce a context preparation
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/03/01
 */
OYAPI int  OYEXPORT
           oyFilterGraph_PrepareContexts (
                                       oyFilterGraph_s   * graph,
                                       int                 flags )
{
  oyOption_s * o = 0;
  oyFilterNode_s * node = 0;
  oyFilterGraph_s * s = graph;
  int i, n, do_it;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < n; ++i)
  {
    node = oyFilterNodes_Get( s->nodes, i );

    if(flags || !node->backend_data)
      do_it = 1;
    else
      do_it = 0;

    if(do_it &&
       node->core->api4_->oyCMMFilterNode_ContextToMem &&
       strlen(node->api7_->context_type))
      oyFilterNode_ContextSet_( node, 0 );

    oyFilterNode_Release( &node );
  }

  /* clean the graph */
  o = oyOptions_Find( s->options, "dirty" );
  oyOption_SetFromText( o, "false", 0 );

  return 0;
}

/** Function oyFilterGraph_GetNode
 *  @memberof oyFilterGraph_s
 *  @brief   select a node
 *
 *  @param[in]     graph               a filter graph
 *  @param[in]     pos                 the position in a matching list,
                                       or -1 to select the first match
 *  @param[in]     registration        criterium to generate the matching list,
                                       or zero for no criterium
 *  @param[in]     mark                marking string to use as a selector
 *  @return                            the filter node
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/01
 */
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterGraph_GetNode     ( oyFilterGraph_s   * graph,
                                       int                 pos,
                                       const char        * registration,
                                       const char        * mark )
{
  oyFilterNode_s * node = 0;
  oyFilterGraph_s * s = graph;
  int i, n, m = -1, found;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < n; ++i)
  {
    node = oyFilterNodes_Get( s->nodes, i );

    found = 1;

    if(found && registration &&
       !oyFilterRegistrationMatch( node->core->api4_->registration,
                                   registration, 0 ))
      found = 0;

    if(found && mark &&
       oyOptions_FindString( node->tags, mark, 0 ) == 0 )
      found = 0;

    if(found)
    if(pos == -1 || ++m == pos)
      break;

    oyFilterNode_Release( &node );
  }

  return node;
}

/** Function oyFilterGraph_SetFromNode
 *  @memberof oyFilterNode_s
 *  @brief   get a graphs adjazency list
 *
 *  @param[in]     graph               a graph object
 *  @param[in]     node                filter node
 *  @param[in]     mark                a selection
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *  @return                            the graph
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
OYAPI int  OYEXPORT
           oyFilterGraph_SetFromNode ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * node,
                                       const char        * mark,
                                       int                 flags )
{
  oyFilterGraph_s * s = graph;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  oyFilterNodes_Release( &s->nodes );
  oyFilterPlugs_Release( &s->edges );

  {
    s->nodes = oyFilterNodes_New( 0 );
    s->edges = oyFilterPlugs_New( 0 );

    oyFilterNode_AddToAdjacencyLst_( node, s->nodes, s->edges, mark, flags );
  }

  return 0;
}

/** Function oyFilterGraph_ToText
 *  @memberof oyFilterGraph_s
 *  @brief   text description of a graph
 *
 *  @todo Should this function generate XFORMS compatible output? How?
 *
 *  @param[in]     graph               graphy object
 *  @param[in]     input               start node of a oyConversion_s
 *  @param[in]     output              end node and if present a switch
 *                                     to interprete input and output as start
 *                                     and end node of a oyConversion_s
 *  @param[in]     head_line           text for inclusion
 *  @param[in]     reserved            future format selector (dot, xml ...)
 *  @param[in]     allocateFunc        allocation function
 *  @return                            the graph description
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/10/04 (Oyranos: 0.1.8)
 *  @date    2009/03/05
 */
OYAPI char * OYEXPORT
           oyFilterGraph_ToText      ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc )
{
  char * text = 0, 
       * temp = oyAllocateFunc_(1024),
       * tmp = 0, * txt = 0, * t = 0;
  oyFilterNode_s * node = 0;
  char * save_locale = 0;
  oyFilterGraph_s * s = graph;

  oyFilterPlug_s * p = 0;
  int i, n,
      nodes_n = 0;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  save_locale = oyStringCopy_( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_);


  STRING_ADD( text, "digraph G {\n" );
  STRING_ADD( text, "bgcolor=\"transparent\"\n" );
  STRING_ADD( text, "  rankdir=LR\n" );
  STRING_ADD( text, "  graph [fontname=Helvetica, fontsize=12];\n" );
  STRING_ADD( text, "  node [shape=record, fontname=Helvetica, fontsize=10, style=\"filled,rounded\"];\n" );
  STRING_ADD( text, "  edge [fontname=Helvetica, fontsize=10];\n" );
  STRING_ADD( text, "\n" );
  if(input && output)
  {
  STRING_ADD( text, "  conversion [shape=plaintext, label=<\n" );
  STRING_ADD( text, "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n" );
  STRING_ADD( text, "  <tr><td>oyConversion_s</td></tr>\n" );
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
  }
  STRING_ADD( text, "\n" );

  /* add more node descriptions */
  nodes_n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < nodes_n; ++i)
  {
    node = oyFilterNodes_Get( s->nodes, i );
    n = oyFilterNode_EdgeCount( node, 1, 0 );

    /** The function is more verbose with the oy_debug variable set. */
    if(!oy_debug && 
       oyStrchr_( node->core->api4_->id_, OY_SLASH_C ))
    {
      STRING_ADD( tmp, node->core->api4_->id_ );
      t = oyStrrchr_( tmp, OY_SLASH_C );
      *t = 0;
      STRING_ADD( txt, t+1 );
      oyFree_m_(tmp);
    } else
      STRING_ADD( txt, node->core->api4_->id_ );

    oySprintf_(temp, "  %d [ label=\"{<plug> %d| Filter Node %d\\n"
                     " Category: \\\"%s\\\"\\n CMM: \\\"%s\\\"\\n"
                     " Type: \\\"%s\\\"|<socket>}\"];\n",
                     oyFilterNode_GetId( node ), n,
                     node->oy_->id_,
                     node->core->category_,
                     txt,
                     node->core->registration_ );
    STRING_ADD( text, temp );
    oyFree_m_(txt);

    oyFilterNode_Release( &node );
  }


  STRING_ADD( text, "\n" );
  STRING_ADD( text, "  subgraph cluster_0 {\n" );
  STRING_ADD( text, "    label=\"" );
  STRING_ADD( text, head_line );
  STRING_ADD( text, "\"\n" );
  STRING_ADD( text, "    color=gray;\n" );
  STRING_ADD( text, "\n" );

  /* add more node placements */
  n = oyFilterPlugs_Count( s->edges );
  for(i = 0; i < n; ++i)
  {
    p = oyFilterPlugs_Get( s->edges, i );

    oySprintf_( temp,
                "    %d:socket -> %d:plug [arrowhead=crow, arrowtail=box];\n",
                oyFilterNode_GetId( p->remote_socket_->node ),
                oyFilterNode_GetId( p->node ) );
    STRING_ADD( text, temp );

    oyFilterPlug_Release( &p );
  }

  STRING_ADD( text, "\n" );
  if(input && output)
  {
  oySprintf_( temp, "    conversion:in -> %d [arrowhead=none, arrowtail=normal];\n", oyFilterNode_GetId( input ) );
  STRING_ADD( text, temp );
  oySprintf_( temp, "    conversion:out -> %d;\n",
                    oyFilterNode_GetId( output ) );
  STRING_ADD( text, temp );
  }
  STRING_ADD( text, "  }\n" );
  STRING_ADD( text, "\n" );
  if(input && output)
  STRING_ADD( text, "  conversion\n" );
  STRING_ADD( text, "}\n" );
  STRING_ADD( text, "\n" );

  setlocale(LC_NUMERIC, "C");
  /* sensible printing */
  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );

  STRING_ADD( text, "" );

  oyFree_m_( temp );

  return text;
}

void oyShowGraph__( oyFilterGraph_s * s )
{
  char * ptr = 0;
  int error = 0;
  oyFilterGraph_s * adjacency_list = s;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return )
  /*return;*/

  ptr = oyFilterGraph_ToText( adjacency_list, 0, 0,
                              "Oyranos Test Graph", 0, malloc );
  oyWriteMemToFile_( "test.dot", ptr, strlen(ptr) );
  error = system("dot -Tps test.dot -o test.ps; gv -spartan -antialias test.ps &");

  free(ptr); ptr = 0;
}
void oyShowGraph_( oyFilterNode_s * s, const char * selector )
{
  oyFilterGraph_s * adjacency_list = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return )
  /*return;*/

  adjacency_list = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( adjacency_list, s, selector, 0 );

  oyShowGraph__(adjacency_list);

  oyFilterGraph_Release( &adjacency_list );
}

#if 0
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

  if(error <= 0)
  {
    int i, n;

    /* collect profiles */
    if(error <= 0)
    {
      int p_list_n = 0;
      oyProfile_s * tmp = 0;

      if(obj)
        p_list = oyProfiles_New( obj );
      else
        p_list = oyProfiles_New( 0 );
      error = !p_list;

      if(error <= 0)
      {
        tmp = oyProfile_Copy( in->profile_, 0);
        p_list = oyProfiles_MoveIn( p_list, &tmp, 0 );
        error = !p_list;
      }

      p_list_n = oyProfiles_Count( p_list );

      if(error <= 0 && list && oyProfiles_Count(list))
      {
        n = oyProfiles_Count(list);
        for(i = 0; i < n; ++i)
        {
          tmp = oyProfiles_Get( list,i );
          p_list = oyProfiles_MoveIn( p_list, &tmp, i + p_list_n);
          error = !p_list;
        }
      }

      if(error <= 0)
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

  if(error <= 0)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API1_S, cmm, &lib_used,
                                      0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMColourConversion_Create;
    }
    error = !funcP;
  }

  if(error <= 0)
  {
    oyProfiles_s * p_list = 0;
    int i, n;

    if(obj)
      cmm_ptr = oyCMMptr_New(0);
    else
      cmm_ptr = oyCMMptr_New(0);
    error = !cmm_ptr;

    if(error <= 0)
      error = oyCMMptr_Set( cmm_ptr, lib_used, oyCMM_COLOUR_CONVERSION,0,0,0 );

    /* collect profiles */
    if(error <= 0)
    {
      p_list = oyConcatenateImageProfiles_( list, in, out, obj ? obj : s->oy_ );

      error = !p_list;
    }

    if(error <= 0)
    {
      oyCMMptr_s ** p = oyStructList_GetCMMptrs_( p_list->list_, lib_used );
      int layout_in = in->layout_[oyLAYOUT];
      int layout_out = out->layout_[oyLAYOUT];

      if(!opts)
        opts = oyOptions_ForFilter( "//" OY_TYPE_STD, "lcms",
                                            0/* oyOPTIONATTRIBUTE_ADVANCED |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON */, 0 );

      n = oyProfiles_Count(p_list);

      error = funcP( p, n, layout_in, layout_out, opts, cmm_ptr );

      for(i = 0; i < n; ++i)
        if(error <= 0)
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
#endif

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
  const char * model = 0;

  char * hash_text = 0;

  if(error <= 0)
  {
    /* input data */
    hashTextAdd_m(   " <data_in>\n" );
    hashTextAdd_m( oyStructList_GetID( ins, 0, 0 ) );
    hashTextAdd_m( "\n </data_in>\n" );

    /* options -> xforms */
    hashTextAdd_m(   " <oyOptions_s>\n" );
    model = oyOptions_GetText( opts, oyNAME_NAME );
    hashTextAdd_m( model );
    hashTextAdd_m( "\n </oyOptions_s>\n" );

    /* output data */
    hashTextAdd_m(   " <data_out>\n" );
    hashTextAdd_m( oyStructList_GetID( outs, 0, 0 ) );
    hashTextAdd_m( "\n </data_out>\n" );

    oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );

    if(hash_text && s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );
    hash_text = 0;
  }

  hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NICK );

  return hash_text;
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  s->output_image_roi = oyRectangle_NewFrom( 0, 0 );

  return s;
}

/** Function oyPixelAccess_Create
 *  @memberof oyPixelAccess_s
 *  @brief   allocate iand initialise a basic oyPixelAccess_s object
 *
 *  @verbatim
  // conversion->out_ has to be linear, so we access only the first plug
  plug = oyFilterNode_GetPlug( conversion->out_, 0 );

  // create a very simple pixel iterator
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                         oyPIXEL_ACCESS_IMAGE, 0 );
@endverbatim
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2009/06/10
 */
oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterPlug_s    * plug,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object )
{
  oyPixelAccess_s * s = oyPixelAccess_New_( object );
  oyFilterSocket_s * sock = 0;
  int error = !s || !plug || !plug->remote_socket_;
  int w = 0;
  oyImage_s * image = 0;
  int32_t n = 0;

  if(error <= 0)
  {
    sock = plug->remote_socket_;
    image = (oyImage_s*)sock->data;

    s->start_xy[0] = s->start_xy_old[0] = start_x;
    s->start_xy[1] = s->start_xy_old[1] = start_y;

    /* make shure the filter->image_ is set, e.g. 
       error = oyFilterCore_ImageSet ( filter, image );
     
    s->data_in = filter->image_->data; */
    if(image)
    w = image->width;

    /** The filters have no obligation to pass end to end informations.
        The ticket must hold all pices of interesst.
     */
    s->output_image_roi->width = 1.0;
    if(image)
      s->output_image_roi->height = image->height / (double)image->width;
    s->output_image = oyImage_Copy( image, 0 );
    s->graph = oyFilterGraph_FromNode( sock->node, 0 );

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
      /** @todo how can we know about the various module capabilities
       *  - back report the processed number of pixels in the passed pointer
       *  - restrict for a line interface only, would fit to oyArray2d_s
       *  - + handle inside an to be created function oyConversion_RunPixels()
       */
    }

    /* Copy requests, which where attached to the node, to the ticket. */
    if(plug->node->core->options_)
      error = oyOptions_Filter( &s->request_queue, &n, 0,
                                oyBOOLEAN_INTERSECTION,
                                "////resolve", plug->node->core->options_ );
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
  if(error <= 0 && s->oy_)
    allocateFunc_ = s->oy_->allocateFunc_;
  else
    allocateFunc_ = oyAllocateFunc_;

  if(error <= 0)
  {
    s->start_xy[0] = s->start_xy_old[0] = obj->start_xy[0];
    s->start_xy[1] = s->start_xy_old[1] = obj->start_xy[1];
    s->array_n = obj->array_n;
    if(obj->array_xy && obj->array_n)
    {
      len = sizeof(int32_t) * 2 * obj->array_n;
      s->array_xy = allocateFunc_(len);
      error = !s->array_xy;
      if(error <= 0)
        error = !memcpy(s->array_xy, obj->array_xy, len);
    }
    /* reset to properly initialise the new iterator */
    s->index = 0;
    s->pixels_n = obj->pixels_n;
    s->workspace_id = obj->workspace_id;
    s->output_image_roi = oyRectangle_Copy( obj->output_image_roi, s->oy_ );
    s->output_image = oyImage_Copy( obj->output_image, 0 );
    s->array = oyArray2d_Copy( obj->array, 0 );
    if(obj->user_data && obj->user_data->copy)
      s->user_data = obj->user_data->copy( obj->user_data, 0 );
    else
      s->user_data = obj->user_data;
    s->graph = oyFilterGraph_Copy( obj->graph, 0 );
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

  oyCheckType_m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  if( !s->oy_ )
  {
    DBG_PROG_S(("oyPixelAccess_s object is not from Oyranos. Skip"))
    return 0;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyArray2d_Release( &s->array );
  oyRectangle_Release( &s->output_image_roi );
  oyImage_Release( &s->output_image );
  oyFilterGraph_Release( &s->graph );
  if(s->user_data && s->user_data->release)
      s->user_data->release( &s->user_data );

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
 *  @date    2009/05/01
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

  if(!image)
    return 0;

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
      if(image->width > 0)
        y = ++pixel_access->start_xy[1];
      else
        y = pixel_access->start_xy[1];
    } else if(image->width > 0)
      ++pixel_access->start_xy[0];

    if(pixel_access->start_xy[1] >= image->height && image->height)
    {
      return -1;
    }
  }

  return 0;
}



/**
 *  Function oyConversion_New
 *  @memberof oyConversion_s
 *  @brief   allocate and initialise a new oyConversion_s object
 *
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/02/18
 */
oyConversion_s *   oyConversion_New  ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONVERSION_S;
# define STRUCT_TYPE oyConversion_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

/** Function oyConversion_CreateBasicPixels
 *  @memberof oyConversion_s
 *  @brief   allocate initialise a basic oyConversion_s object
 *
 *  Provided options will be passed to oyFilterNode_NewWith(). There for no 
 *  options defaults will be selected.
 *
 *  @param[in]     input               the input image data
 *  @param[in]     output              the output image data
 *  @param[in]     options             options to be passed to oyFilterCore_s
 *                                     Each option should be carefully named
 *                                     to be correctly selected by the intented
 *                                     filter (root, icc and output).
 *                                     The available options are particial
 *                                     defined by plugable policy modules,
 *                                     e.g. "oicc", and a filters own options.
 *  @param         object              the optional object
 *  @return                            the conversion context
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/08/01
 */
oyConversion_s   * oyConversion_CreateBasicPixels (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyConversion_s * s = 0;
  int error = !input || !output;
  oyFilterNode_s * in = 0, * out = 0;

  if(error <= 0)
  {
    s = oyConversion_New ( 0 );
    error = !s;    

    if(error <= 0)
      in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", options, 0 );
    if(error <= 0)
      error = oyConversion_Set( s, in, 0 );
    if(error <= 0)
      error = oyFilterNode_DataSet( in, (oyStruct_s*)input, 0, 0 );

    if(error <= 0)
      out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/icc", options, 0 );
    if(error <= 0)
      error = oyFilterNode_DataSet( out, (oyStruct_s*)output, 0, 0 );
    if(error <= 0)
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );

    in = out; out = 0;

    if(error <= 0)
      out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", options, 0 );
    if(error <= 0)
    {
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );
      if(error)
        WARNc1_S( "could not add  filter: %s\n", "//" OY_TYPE_STD "/output" );
    }
    if(error <= 0)
      error = oyConversion_Set( s, 0, out );
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

  s = oyConversion_New( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(error <= 0)
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

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

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



/** Function oyConversion_Set
 *  @memberof oyConversion_s
 *  @brief   set input and output of a conversion graph
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     input               input node
 *  @param[in]     output              output node
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int                oyConversion_Set  ( oyConversion_s    * conversion,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output )
{
  oyConversion_s * s = conversion;
  int error = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  if(input)
    s->input = input;

  if(output)
    s->out_ = output;

  return error;
}

/** Function oyConversion_RunPixels
 *  @memberof oyConversion_s
 *  @brief   iterate over a conversion graph
 *
 *  @verbatim
    // use the output
    oyImage_s * image = oyConversion_GetImage( context, OY_OUTPUT );
    // get the data and draw the image
    for(i = 0; i < image->height; ++i)
    {
      image_data = image->getLine( image, i, &height, -1, &is_allocated );

      // ...

      if(is_allocated)
        free( image_data );
    }
@endverbatim
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in,out] pixel_access        optional pixel iterator configuration
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/06 (Oyranos: 0.1.8)
 *  @date    2009/03/06
 */
int                oyConversion_RunPixels (
                                       oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access )
{
  oyConversion_s * s = conversion;
  oyFilterPlug_s * plug = 0;
  oyFilterCore_s * filter = 0;
  oyImage_s * image = 0, * image_input = 0;
  int error = 0, result = 0, l_error = 0, i,n, dirty = 0, tmp_ticket = 0;
  oyRectangle_s roi = {oyOBJECT_RECTANGLE_S, 0,0,0};
  double clck;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  /* basic checks */
  if(!conversion->out_ || !conversion->out_->plugs ||
     !conversion->out_->plugs[0])
  {
    WARNc1_S("graph incomplete [%d]", s ? oyObject_GetId( s->oy_ ) : -1)
    return 1;
  }

  /* conversion->out_ has to be linear, so we access only the first plug */
  plug = oyFilterNode_GetPlug( conversion->out_, 0 );

  if(!pixel_access)
  {
    /* create a very simple pixel iterator as job ticket */
    if(plug)
    {
      clck = oyClock();
      pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
      clck = oyClock() - clck;
      DBG_NUM1_S("oyPixelAccess_Create(): %g", clck/1000000.0 );
    }
    tmp_ticket = 1;
  }

  if(!pixel_access)
    error = 1;

  /* should be the same as conversion->out_->filter */
  filter = conversion->out_->core;
  image = oyConversion_GetImage( conversion, OY_OUTPUT );

  if(error <= 0)
    oyRectangle_SetByRectangle( &roi, pixel_access->output_image_roi );
  
  if(error <= 0)
  {
    clck = oyClock();
    result = oyImage_FillArray( image, &roi, 2,
                                &pixel_access->array,
                                pixel_access->output_image_roi, 0 );
    clck = oyClock() - clck;
    DBG_NUM1_S("oyImage_FillArray(): %g", clck/1000000.0 );
    error = ( result != 0 );
  }

  /* run on the graph */
  if(error <= 0)
  {
    clck = oyClock();
    error = conversion->out_->api7_->oyCMMFilterPlug_Run( plug, pixel_access );
    clck = oyClock() - clck;
    DBG_NUM1_S("conversion->out_->api7_->oyCMMFilterPlug_Run(): %g", clck/1000000.0 );
  }

  if(error != 0 && pixel_access)
  {
    dirty = oyOptions_FindString( pixel_access->graph->options, "dirty", "true")
            ? 1 : 0;

    /* refresh the graph representation */
    clck = oyClock();
    oyFilterGraph_SetFromNode( pixel_access->graph, conversion->input, 0, 0 );
    clck = oyClock() - clck;
    DBG_NUM1_S("oyFilterGraph_SetFromNode(): %g", clck/1000000.0 );

    /* resolve missing data */
    clck = oyClock();
    image_input = oyFilterPlug_ResolveImage( plug, plug->remote_socket_,
                                             pixel_access );
    clck = oyClock() - clck;
    DBG_NUM1_S("oyFilterPlug_ResolveImage(): %g", clck/1000000.0 );
    oyImage_Release( &image_input );

    n = oyFilterNodes_Count( pixel_access->graph->nodes );
    for(i = 0; i < n; ++i)
    {
      clck = oyClock();
      l_error = oyArray2d_Release( &pixel_access->array ); OY_ERR
      l_error = oyImage_FillArray( image, &roi, 2,
                                   &pixel_access->array,
                                   pixel_access->output_image_roi, 0 ); OY_ERR
      clck = oyClock() - clck;
      DBG_NUM1_S("oyImage_FillArray(): %g", clck/1000000.0 );

      if(error != 0 &&
         dirty)
      {
        if(pixel_access->start_xy[0] != pixel_access->start_xy_old[0] ||
           pixel_access->start_xy[1] != pixel_access->start_xy_old[1])
        {
          /* set back to previous values, at least for the simplest case */
          pixel_access->start_xy[0] = pixel_access->start_xy_old[0];
          pixel_access->start_xy[1] = pixel_access->start_xy_old[1];
        }

        clck = oyClock();
        oyFilterGraph_PrepareContexts( pixel_access->graph, 0 );
        clck = oyClock() - clck;
        DBG_NUM1_S("oyFilterGraph_PrepareContexts(): %g", clck/1000000.0 );
        clck = oyClock();
        error = conversion->out_->api7_->oyCMMFilterPlug_Run( plug,
                                                              pixel_access);
        clck = oyClock() - clck;
        DBG_NUM1_S("conversion->out_->api7_->oyCMMFilterPlug_Run(): %g", clck/1000000.0 );
      }

      if(error == 0)
        break;
    }
  }

  /* Write the data to the output image.
   *
   * The oyPixelAccess_s job ticket contains a oyArray2d_s object called array
   * holding the in memory data. After the job is done the output images
   * pixel_data pointer is compared with the job tickets array pointer. If 
   * they are the same it is assumed that a observer of the output image will
   * see the same processed data, otherwise oyPixelAccess_s::array must be 
   * copied to the output image.
   *
   * While the design of having whatever data storage in a oyImage_s is very 
   * flexible, the oyPixelAccess_s::array's in memory buffer is not.
   * Users with very large data sets have to process the data in chunks and
   * the oyPixelAccess_s::array allocation can remain constant.
   */
  if(image && pixel_access &&
     ((oyPointer)image->pixel_data != (oyPointer)pixel_access->array ||
      image != pixel_access->output_image))
  {
    /* move the array to the top left place
     * same as : roi.x = roi.y = 0; */
    roi.x -= pixel_access->output_image_roi->x;
    roi.y -= pixel_access->output_image_roi->y;

    result = oyImage_ReadArray( image, &roi, /*pixel_access->output_image_roi,*/
                                       pixel_access->array, 0 );
  }

  if(tmp_ticket)
    oyPixelAccess_Release( &pixel_access );

  oyImage_Release( &image );

  return error;
}

/** Function oyConversion_GetOnePixel
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

  /* conversion->out_ has to be linear, so we access only the first socket
   * (terrible long) */
  plug = (oyFilterPlug_s*) oyStructList_Get_(((oyFilterSocket_s *)conversion->out_->sockets[0])->requesting_plugs_->list_, 0);
  sock = plug->remote_socket_;

  pixel_access = oyPixelAccess_Create ( x, y, plug, oyPIXEL_ACCESS_POINT, 0 );
  /* @todo */
  error = sock->node->api7_->oyCMMFilterPlug_Run( plug, pixel_access );

  return pixel;
}

/** Function oyConversion_GetImage
 *  @memberof oyConversion_s
 *  @brief   get a image copy at the desired position
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     flags               OY_INPUT or OY_OUTPUT
 *  @return                            the image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/19 (Oyranos: 0.1.10)
 *  @date    2009/02/19
 */
oyImage_s        * oyConversion_GetImage (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags )
{
  oyImage_s * image = 0;
  oyFilterPlug_s * plug = 0;
  oyFilterSocket_s * sock = 0;
  int error = 0;
  oyConversion_s * s = conversion;
  oyPixelAccess_s * pixel_access = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  if(!error)
  {
    if(oyToInput_m(flags))
    {
      sock = oyFilterNode_GetSocket( s->input, 0 );
      if(sock)
      {
        image = oyImage_Copy( (oyImage_s*) sock->data, 0 );

        if(!sock->data)
        {
          /* TODO: remove the following hack; the socket->plug cast is ugly */
          s->input->api7_->oyCMMFilterPlug_Run( (oyFilterPlug_s*) sock, 0 );
          image = oyImage_Copy( (oyImage_s*) sock->data, 0 );
        }
      }

    } else
    if(oyToOutput_m(flags))
    {
      plug = oyFilterNode_GetPlug( s->out_, 0 );
      if(plug && plug->remote_socket_)
      {
        image = oyImage_Copy( (oyImage_s*) plug->remote_socket_->data, 0);

        if(!image)
        {
          /* Run the graph to set up processing image data. */
          plug = oyFilterNode_GetPlug( conversion->out_, 0 );
          pixel_access = oyPixelAccess_Create( 0,0, plug,
                                               oyPIXEL_ACCESS_IMAGE, 0 );
          conversion->out_->api7_->oyCMMFilterPlug_Run( plug, pixel_access );

          /* Link the tickets image. It should be real copied in a plug-in. */
          /* error = oyFilterNode_DataSet( conversion->out_, 
                                        (oyStruct_s*)pixel_access->output_image,
                                        0, 0 ); */
          oyPixelAccess_Release( &pixel_access );

          image = oyImage_Copy( (oyImage_s*) plug->remote_socket_->data, 0 );
        }
      }
    }
  }

  return image;
}

/** Function oyConversion_GetGraph
 *  @memberof oyConversion_s
 *  @brief   get the filter graph from a conversion context
 *
 *  @see oyFilterGraph_GetNode() to obtain a node.
 *
 *  @param[in,out] conversion          conversion object
 *  @return                            the filter graph containing all nodes
 *
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/02 (Oyranos: 0.2.1)
 *  @date    2011/01/02
 */
oyFilterGraph_s  * oyConversion_GetGraph (
                                       oyConversion_s    * conversion )
{
  return oyFilterGraph_FromNode( conversion->input, 0 );
}


/** Function oyFilterGraph_ToBlob
 *  @memberof oyBlob_s
 *  @brief   node context to binary blob
 *
 *  Typical a context from a CMM will be returned.
 *
 *  @param         graph               graph object
 *  @param         node_pos            node position in the graph
 *  @param         object              the optional object
 *  @return                            the data blob
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/12 (Oyranos: 0.1.10)
 *  @date    2009/06/12
 */
oyBlob_s * oyFilterGraph_ToBlob      ( oyFilterGraph_s   * graph,
                                       int                 node_pos,
                                       oyObject_s          object )
{
  int flags = 1;
  oyFilterNode_s * node = 0;
  int do_it;
  oyFilterGraph_s * s = graph;
  oyBlob_s * blob = 0;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  node = oyFilterNodes_Get( s->nodes, node_pos );

  if(node)
  {
    if(flags || !node->backend_data)
      do_it = 1;
    else
      do_it = 0;

    if(do_it &&
       node->core->api4_->oyCMMFilterNode_ContextToMem &&
       strlen(node->core->api4_->context_type))
    {
      blob = oyBlob_New( object );
      oyFilterNode_ContextSet_( node, blob );
    }

    oyFilterNode_Release( &node );
  }

  return blob;
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
 *  @return                            the graph description
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/04 (Oyranos: 0.1.8)
 *  @date    2009/03/05
 */
char             * oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc )
{
  oyConversion_s * s = conversion;
  char * text = 0;
  oyFilterGraph_s * adjacency_list = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  adjacency_list = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( adjacency_list, conversion->input, 0, 0 );

  text = oyFilterGraph_ToText( adjacency_list,
                               conversion->input, conversion->out_,
                               head_line, reserved, allocateFunc );

  oyFilterGraph_Release( &adjacency_list );

  return text;
}

/** typedef  oyConversion_Correct
 *  @brief   check for correctly adhering to policies
 *  @memberof oyConversion_s
 *
 *  Without any options the module selected with the registration argument shall
 *  perform graph analysis and correct the graph. 
 *
 *  @par Typical Options:
 *  - "command"-"help" - a string option issuing a help text as message
 *  - "verbose" - reporting changes as message
 *
 *  TODO: display and selection of policy modules
 *
 *  @param   conversion                the to be checked configuration
 *  @param   registration              the to be used policy module
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param   options                   options to the policy module
 *  @return                            0 - indifferent, >= 1 - error
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/24 (Oyranos: 0.1.10)
 *  @date    2009/07/24
 */
int                oyConversion_Correct (
                                       oyConversion_s    * conversion,
                                       const char        * registration,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  int error = 0;
  oyConversion_s * s = conversion;
  const char * pattern = registration;

  if(!conversion)
    return error;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  if(!conversion->input && !conversion->out_)
  {
    WARNc1_S( "%s",_("Found no node in conversion. give up") );
    return 1;
  }

  if(!error)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i;
    oyCMMapi9_s * cmm_api9 = 0;
    char * class, * api_reg;

    class = oyFilterRegistrationToText( pattern, oyFILTER_REG_TYPE, 0 );
    api_reg = oyStringCopy_("//", oyAllocateFunc_ );
    STRING_ADD( api_reg, class );
    oyFree_m_( class );

    apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S, 
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 0,0 );
    apis_n = oyCMMapiFilters_Count( apis );
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

      if(oyFilterRegistrationMatch( cmm_api9->pattern, pattern, 0 ))
      {
        if(cmm_api9->oyConversion_Correct)
          error = cmm_api9->oyConversion_Correct( s, flags, options );
        if(error)
        {
          WARNc2_S( "%s %s",_("error in module:"), cmm_api9->registration );
          return 1;
        }
      }

      if(cmm_api9->release)
        cmm_api9->release( (oyStruct_s**)&cmm_api9 );
    }
    oyCMMapiFilters_Release( &apis );
  }
  
  return 0;
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
 *  @param[in]     ref                 possibly a ICC profile
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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0)
    s->profile_  = oyProfile_Copy( ref, 0 );

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(n)
    s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
  oyCopyColour( chan, &s->channels_[0], 1, ref, n );
  oyCopyColour( 0, &s->XYZ_[0], 1, 0, 0 );

  if(error <= 0 && blob && blob_len)
  {
    s->blob_ = s->oy_->allocateFunc_( blob_len );
    if(!s->blob_) error = 1;

    if(error <= 0)
      error = !memcpy( s->blob_, blob, blob_len );

    if(error <= 0)
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

  if(error <= 0)
    error = oyObject_SetNames( s_obj, nick, name, description );

  s = oyNamedColour_Create( chan, blob, blob_len, profile_ref, s_obj );
  error =!s;

  oyObject_Release( &s_obj );

  if(error <= 0 && XYZ)
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

  oyCheckType__m( oyOBJECT_NAMED_COLOUR_S, return 1 )

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
 *  Function: oyConvertColour_
 *  @memberof oyNamedColour_s
 *  @brief   convert colours
 *
 *  The options are passed to oyConversion_CreateBasicPixels();
 *  The function does the lookups for the profiles and the modules contexts
 *  in the Oyranos cache on the fly. The allocated oyImage_s and
 *  oyConversion_s structures are not cheap as they are not cached.
 *
 *  @version Oyranos: 0.1.11
 *  @since   2007/12/23 (Oyranos: 0.1.8)
 *  @date    2010/09/10
 */
int  oyColourConvert_ ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out,
                        oyOptions_s       * options,
                        int                 count )
{
  oyImage_s * in  = NULL,
            * out = NULL;
  oyConversion_s * conv = NULL;
  int error = 0;

  in    = oyImage_Create( count, 1,
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  out   = oyImage_Create( count, 1,
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  conv   = oyConversion_CreateBasicPixels( in,out, options, 0 );
  error  = oyConversion_RunPixels( conv, 0 );

  oyConversion_Release( &conv );
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
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  int error = !colour || !profile || !buf;
  oyProfile_s * p_in = colour->profile_;

  /* XYZ has priority */
  if(error <= 0 &&
     colour->XYZ_[0] != -1 && colour->XYZ_[1] != -1 && colour->XYZ_[2] != -1)
  {
    p_in = oyProfile_FromStd ( oyEDITING_XYZ, NULL );
    if(!profile)
      return 1;
 
    error = oyColourConvert_( p_in, profile,
                              colour->XYZ_, buf,
                              oyDOUBLE, buf_type, options, 1);

    oyProfile_Release ( &p_in );

  } else if(error <= 0)
    error = oyColourConvert_( p_in, profile,
                              colour->channels_, buf,
                              oyDOUBLE, buf_type, options, 1);

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
 *  @param[in]     options             for filter node creation
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColour_SetColourStd ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  oyNamedColour_s * s = colour;
  int error = !s || !colour_space || !channels;
  oyProfile_s * p_in = 0;
  oyProfile_s * p_out = 0;

  /* abreviate */
  if(error <= 0 && channels_type == oyDOUBLE)
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

  if(error <= 0)
  {
    p_in = oyProfile_FromStd ( colour_space, NULL );
    error = !p_in;
  }

  /* reset and allocate */
  if(error <= 0)
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
  if(error <= 0)
  {
    p_out = s->profile_;
    error = oyColourConvert_( p_in, p_out,
                              channels, s->channels_,
                              channels_type , oyDOUBLE, options, 1 );
    p_out = 0;
  }

  if(error <= 0)                               
  {  
    p_out = oyProfile_FromStd( oyEDITING_XYZ, 0 );
    error = oyColourConvert_( p_in, p_out,
                              channels, s->XYZ_,
                              channels_type , oyDOUBLE, options, 1 );
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
                                       uint32_t          * flags,
                                       oyOptions_s       * options )
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

  ret = oyNamedColour_GetColour ( colour, profile, buf, buf_type, 0, options );
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
    oyNamedColour_GetColourStd( s, oyEDITING_LAB, l, oyDOUBLE, 0, 0 );

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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0)
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

  oyCheckType__m( oyOBJECT_NAMED_COLOURS_S, return 1 )

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

  if(error <= 0 && obj->type_ != oyOBJECT_NAMED_COLOURS_S)
    error = 1;

  if(error <= 0)
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

    if(error <= 0 && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }

    if(error <= 0)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
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

  if(error <= 0 && obj->type_ != oyOBJECT_NAMED_COLOURS_S)
    error = 1;

  if(error <= 0)
    oyStructList_ReleaseAt( obj->list_, position );

  return error; 
}


/** @} *//* objects_single_colour */


/** \addtogroup module_api

 *  @{
 */
const char *   oyModuleGetActual     ( const char        * type )
{
  oyExportStart_(EXPORT_CMMS);
  oyExportEnd_();

  if(oyFilterRegistrationMatch( type, "//" OY_TYPE_STD, oyOBJECT_CMM_API4_S ))
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

/** @} *//* module_api */



/** \addtogroup module_api

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
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
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

  if(error <= 0)
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

  if(!(s && s->type == oyOBJECT_CMM_INFO_S && !s->release_))
  oyCheckType_m( oyOBJECT_CMM_INFO_S, return 1 )

  *obj = 0;

  
  if(!s->oy_ || oyObject_UnRef(s->oy_))
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


/** Function oyCMMapiFilters_New
 *  @memberof oyCMMapiFilters_s
 *  @brief   allocate a new CMMapiFilters list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI oyCMMapiFilters_s * OYEXPORT
                 oyCMMapiFilters_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API_FILTERS_S;
# define STRUCT_TYPE oyCMMapiFilters_s
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
  s->copy = (oyStruct_Copy_f) oyCMMapiFilters_Copy;
  s->release = (oyStruct_Release_f) oyCMMapiFilters_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/** @internal
 *  Function oyCMMapiFilters_Copy_
 *  @memberof oyCMMapiFilters_s
 *  @brief   real copy a CMMapiFilters object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
oyCMMapiFilters_s * oyCMMapiFilters_Copy_(
                                       oyCMMapiFilters_s * obj,
                                       oyObject_s          object )
{
  oyCMMapiFilters_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyCMMapiFilters_New( object );
  error = !s;

  if(error <= 0)
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );

  if(error)
    oyCMMapiFilters_Release( &s );

  return s;
}

/** Function oyCMMapiFilters_Copy
 *  @memberof oyCMMapiFilters_s
 *  @brief   copy or reference a CMMapiFilters list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI oyCMMapiFilters_s * OYEXPORT
                 oyCMMapiFilters_Copy( oyCMMapiFilters_s * obj,
                                       oyObject_s          object)
{
  oyCMMapiFilters_s * s = 0;

  if(!obj || obj->type_ != oyOBJECT_CMM_API_FILTERS_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapiFilters_Copy_( obj, object );

  return s;
}
 
/** Function oyCMMapiFilters_Release
 *  @memberof oyCMMapiFilters_s
 *  @brief   release and possibly deallocate a CMMapiFilters list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI int  OYEXPORT
               oyCMMapiFilters_Release(oyCMMapiFilters_s** obj )
{
  /* ---- start of common object destructor ----- */
  oyCMMapiFilters_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 1 )

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

/** @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
int    oyIsOfTypeCMMapiFilter        ( oyOBJECT_e          type )
{
  return        type == oyOBJECT_CMM_API4_S ||
                type == oyOBJECT_CMM_API6_S ||
                type == oyOBJECT_CMM_API7_S ||
                type == oyOBJECT_CMM_API8_S ||
                type == oyOBJECT_CMM_API9_S ||
                type == oyOBJECT_CMM_API10_S;
}

/** @internal
 *  Function oyCMMapiNumberToChar
 *  @brief   convert a oyOBJECT_e to a char
 *
 *  @param         api_number          object type
 *  @return                            number as char or zero
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/02 (Oyranos: 0.1.10)
 *  @date    2009/06/02
 */
char   oyCMMapiNumberToChar          ( oyOBJECT_e          api_number )
{   
         if(api_number == oyOBJECT_CMM_API4_S)
      return '4';
    else if(api_number == oyOBJECT_CMM_API5_S)
      return '5';
    else if(api_number == oyOBJECT_CMM_API6_S)
      return '6';
    else if(api_number == oyOBJECT_CMM_API7_S)
      return '7';
    else if(api_number == oyOBJECT_CMM_API8_S)
      return '8';
    else if(api_number == oyOBJECT_CMM_API9_S)
      return '9';
  return 0;
}

/** Function oyCMMapiFilters_MoveIn
 *  @memberof oyCMMapiFilters_s
 *  @brief   add a element to a CMMapiFilters list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI int  OYEXPORT
                 oyCMMapiFilters_MoveIn (
                                       oyCMMapiFilters_s * list,
                                       oyCMMapiFilter_s ** obj,
                                       int                 pos )
{
  oyCMMapiFilters_s * s = list;
  int error = !s || s->type_ != oyOBJECT_CMM_API_FILTERS_S;

  if(obj && *obj && oyIsOfTypeCMMapiFilter( (*obj)->type ))
  {
    if(!s)
    {
      s = oyCMMapiFilters_New(0);
      error = !s;
    }                                  

    if(error <= 0 && !s->list_)
    {
      s->list_ = oyStructList_New( 0 );
      error = !s->list_;
    }
      
    if(error <= 0)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
  }   
  
  return error;
}

/** Function oyCMMapiFilters_ReleaseAt
 *  @memberof oyCMMapiFilters_s
 *  @brief   release a element from a CMMapiFilters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI int  OYEXPORT
                 oyCMMapiFilters_ReleaseAt (
                                       oyCMMapiFilters_s * list,
                                       int                 pos )
{ 
  int error = !list;

  if(error <= 0 && list->type_ != oyOBJECT_CMM_API_FILTERS_S)
    error = 1;
  
  if(error <= 0)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function oyCMMapiFilters_Get
 *  @memberof oyCMMapiFilters_s
 *  @brief   get a element of a CMMapiFilters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI oyCMMapiFilter_s * OYEXPORT
                 oyCMMapiFilters_Get ( oyCMMapiFilters_s * list,
                                       int                 pos )
{       
  if(list && list->type_ == oyOBJECT_CMM_API_FILTERS_S)
    return (oyCMMapiFilter_s *) oyStructList_GetRef( list->list_, pos ); 
  else  
    return 0;
}   

/** Function oyCMMapiFilters_Count
 *  @memberof oyCMMapiFilters_s
 *  @brief   count the elements in a CMMapiFilters list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI int  OYEXPORT
                 oyCMMapiFilters_Count(oyCMMapiFilters_s * list )
{       
  if(list && list->type_ == oyOBJECT_CMM_API_FILTERS_S)
    return oyStructList_Count( list->list_ );
  else return 0;
}

/** @} *//* module_api */


/** \addtogroup misc Miscellaneous

 *  @{
 */

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





/** typedef  oyOptions_Handle
 *  @brief   handle a request by a module
 *
 *  @param[in]     registration        the module selector
 *  @param[in]     options             options
 *  @param[in]     command             the command to handle
 *  @param[out]    result              options to the policy module
 *  @return                            0 - indifferent, >= 1 - error,
 *                                     <= -1 - issue,
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.3.0
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2011/02/22
 */
int             oyOptions_Handle     ( const char        * registration,
                                       oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;
  oyOptions_s * s = options;

  if(!options && !command)
    return error;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 )

  if(!error)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i, found = 0;
    oyCMMapi10_s * cmm_api10 = 0;
    char * class, * api_reg;
    char * test = 0;
    uint32_t * rank_list = 0,
               api_n = 0;

    class = oyFilterRegistrationToText( registration, oyFILTER_REG_TYPE, 0 );
    api_reg = oyStringCopy_("//", oyAllocateFunc_ );
    STRING_ADD( api_reg, class );
    oyFree_m_( class );

    STRING_ADD( test, "can_handle." );
    if(command && command[0])
      STRING_ADD( test, command );

    apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API10_S, 
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 &rank_list, &api_n );
    if(rank_list) oyDeAllocateFunc_(rank_list); rank_list = 0;

    apis_n = oyCMMapiFilters_Count( apis );
    if(test)
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api10 = (oyCMMapi10_s*) oyCMMapiFilters_Get( apis, i );

        if(oyFilterRegistrationMatch( cmm_api10->registration, registration, 0))
        {
          if(cmm_api10->oyMOptions_Handle)
          {
            error = cmm_api10->oyMOptions_Handle( s, test, result );
            if(error == 0)
            {
              found = 1;
              error = cmm_api10->oyMOptions_Handle( s, command, result );
            }

          } else
            error = 1;

          if(error > 0)
          {
            WARNc2_S( "%s %s",_("error in module:"), cmm_api10->registration );
          }
        }

        if(cmm_api10->release)
          cmm_api10->release( (oyStruct_s**)&cmm_api10 );
      }
    else
      WARNc2_S( "%s %s",_("Could not allocate memory for:"),
                cmm_api10->registration );

    oyFree_m_( test );
    oyCMMapiFilters_Release( &apis );
    if(!found && error == 0)
      error = -1;
  }

  return error;
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
int      oyGetScreenFromPosition     ( const char        * display_name,
                                       int                 x,
                                       int                 y )
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
char *   oyGetDisplayNameFromPosition2(const char        * device_type,
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

  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  /** we want a fuzzy look at our display, not the narrow "device_name" */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/display_name",
                                 display_name, OY_CREATE_NEW );

  error = oyConfigs_FromDeviceClass ( device_type, device_class, options, &devices,
                                          0 );

  oyOptions_Release( &options );

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    o = oyConfig_Find( device, "device_rectangle" );

    r = (oyRectangle_s*) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );

    if(!device_name &&
       r && oyRectangle_PointIsInside( r, x,y ))
    {
      device_name = oyConfig_FindString( device, "device_name", 0 );
      text = oyStringCopy_( device_name, allocateFunc );
    }
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
char *   oyGetDisplayNameFromPosition( const char        * display_name,
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

  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  /** we want a fuzzy look at our display, not the narrow "device_name" */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/display_name",
                                 display_name, OY_CREATE_NEW );

  error = oyConfigs_FromDeviceClass ( 0, "monitor", options, &devices,
                                          0 );

  oyOptions_Release( &options );

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    o = oyConfig_Find( device, "device_rectangle" );

    r = (oyRectangle_s*) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );

    if(!device_name &&
       r && oyRectangle_PointIsInside( r, x,y ))
    {
      device_name = oyConfig_FindString( device, "device_name", 0 );
      text = oyStringCopy_( device_name, allocateFunc );
    }
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
char *   oyGetMonitorProfile         ( const char        * device_name,
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
char *   oyMonitorProfileNameFromDB  ( oyConfig_s        * device,
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
char *   oyGetMonitorProfileNameFromDB(const char        * display_name,
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
    error = oyDeviceGet( OY_TYPE_STD, "monitor", display_name,
                             options, &device );

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
 *  server side informatin should go away. \n
 *  With a profile name given the function will lookup the monitor in the 
 *  Oyranos device database and stores the given profile there. \n
 *  To sum up, to set a new profile please call the following sequence:
 *  @verbatim
    // store new settings in the Oyranos data base
    oySetMonitorProfile( display_name, profile_name );
    // remove the server entries
    oySetMonitorProfile( display_name, 0 );
    // update the window server from the newly Oyranos data base settings
    oyActivateMonitorProfiles( display_name );
    @endverbatim
 *
 *  @param      display_name  the display string
 *  @param      profile_name  the file to use as monitor profile or 0 to unset
 *  @return                   error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2009/01/28
 */
int      oySetMonitorProfile         ( const char        * display_name,
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
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
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
    error = oyDeviceSetProfile( device, profile_name );
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
int      oyActivateMonitorProfiles   ( const char        * display_name )
{
  int error = !display_name || !display_name[0];
  oyOptions_s * options = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  const char * device_class = "monitor",
             * device_name = 0;
  int i, n;

  if(error > 0)
  {
    WARNc_S( "No device_name argument provided. Give up." );
    return error;
  }

  {
    /* 1. set a general request */
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "list", OY_CREATE_NEW );
    /* we want a fuzzy look at our display, not as narrow as "device_name"*/
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/display_name",
                                   display_name, OY_CREATE_NEW );
    error = oyConfigs_FromDeviceClass ( 0, device_class, options,
                                            &devices, 0 );

    n = oyConfigs_Count( devices );
    for(i = 0; i < n; ++i)
    {
      device = oyConfigs_Get( devices, i );

      device_name = oyConfig_FindString( device, "device_name", 0 );

      oyDeviceSetup( device );
      oyConfig_Release( &device );
    }

    oyConfigs_Release( &devices );
    oyOptions_Release( &options );
  }

  return error;
}


/** @} *//* monitor_api */


/** @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/16 (Oyranos: 0.1.10)
 *  @date    2009/04/16
 */
void     oyAlphaFinish_              ( int                 unused )
{
  oyProfiles_Release( &oy_profile_list_cache_ );
  oyStructList_Release( &oy_meta_module_cache_ );
  oyStructList_Release( &oy_cmm_cache_ );
  oyStructList_Release( &oy_profile_s_file_cache_ );
}


