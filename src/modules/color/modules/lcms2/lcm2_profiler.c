/** @file lcm2_profiler.c
 *
 *  @par Copyright:
 *            2009-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    littleCMS CMM profile generator for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/MIT>
 *  @since    2009/10/24
 */

#include "lcm2_profiler.h"

#include <assert.h>
#include <lcms2.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <wchar.h>

#ifndef OY_UNUSED
#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
#define OY_UNUSED                      __attribute__ ((unused))
#elif defined(_MSC_VER)
#define OY_UNUSED                      __declspec(unused)
#else
#define OY_UNUSED
#endif
#endif

#ifndef OY_FALLTHROUGH
#if   defined(__clang__)
#define OY_FALLTHROUGH
#elif __GNUC__ >= 7 
#define OY_FALLTHROUGH                 __attribute__ ((fallthrough));
#else
#define OY_FALLTHROUGH
#endif
#endif


#if LCMS_VERSION < 2050
/* 'dscm' */
#define cmsSigProfileDescriptionMLTag 0x6473636d
#endif

#define lcm2Free_m(v) if(v) { free(v); v = NULL; }

extern lcm2Message_f lcm2msg_p;

static const int max_channels = 16;

/* core functions */

typedef struct {
  cmsHTRANSFORM in2MySpace;
  cmsHTRANSFORM mySpace2Out;
  lcm2Sampler_f sampler;
  void * sampler_variables;
  int channelsIn;
  int channelsProcess;
  int channelsOut;
} lcm2Cargo_s;


int  lcm2samplerDouble               ( double              in[],
                                       double              out[],
                                       void              * Cargo )
{
  int i;
  lcm2Cargo_s * d = (lcm2Cargo_s*) Cargo;

  // color convert from input space to prcess color space
  if(d->in2MySpace)
    cmsDoTransform( d->in2MySpace, in, in, 1 );

  // custom data processing
  d->sampler(in,out,d->sampler_variables);

  // converting from process space to output space
  if(d->mySpace2Out)
    cmsDoTransform( d->mySpace2Out, out, out, 1 );

  // clipping
  for(i = 0; i < d->channelsOut; ++i)
  {
    if(out[i] > 1.0)
      out[i] = 1.0;
    if(out[i] < 0.0)
      out[i] = 0.0;
  }

  return TRUE;
}

int lcm2sampler16      (const cmsUInt16Number In[],
                              cmsUInt16Number Out[],
                        void                * Cargo)
{
  int i, v, result = TRUE;
  double in[max_channels], out[max_channels],
         scaler = 65536.0;
  lcm2Cargo_s * d = (lcm2Cargo_s*) Cargo;

  for(i = 0; i < d->channelsIn; ++i)
    in[i] = In[i] / scaler;

  result = lcm2samplerDouble( in, out, Cargo );

  for(i = 0; i < d->channelsOut; ++i)
  {
    v = out[i] * scaler;
    // integer clipping
    if(v > 65535)
      Out[i] = 65535;
    else
      Out[i] = v;
  }

  return result;
}


int  lcm2samplerFloat                ( const cmsFloat32Number In[],
                                             cmsFloat32Number Out[],
                                       void              * Cargo )
{
  int i, result = TRUE;
  double in[max_channels], out[max_channels];
  lcm2Cargo_s * d = (lcm2Cargo_s*) Cargo;

  for(i = 0; i < d->channelsIn; ++i)
    in[i] = In[i];

  result = lcm2samplerDouble( in, out, Cargo );

  for(i = 0; i < d->channelsOut; ++i)
    Out[i] = out[i];

  return result;
}

/** \addtogroup profiler ICC profiler API
 *  @brief   Easy to use API to generate matrix and LUT ICC profiles.
 *
 *  @{ */

/** Function  lcm2OpenProfileFile
 *  @brief    Open a profile from file
 *
 *  @code
  // create ICC profile with linear gamma, RGB.709 primaries + D65 from wildcard
  if(in_space_profile) h_in_space  = lcm2OpenProfileFile( "*srgblinear", NULL );
    @endcode
 *
 *  @param[in]    my_space_profile                        operating color space.
 *                                                        Use a file name or
 *                                                        possible wildcards:
 *                                                        - *srgblinear
 *                                                        - *srgb
 *                                                        - *lab
 *                                                        - *rec601.625.linear
 *                                                        - *rec601.525.linear
 *  @param[in]    my_space_profile_path                   path name for 
 *                                                        for my_space_profile; optional
 *  @return                                               lcms profile handle
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/04
 *  @since   2016/03/04 (Oyranos: 0.9.6)
 */
cmsHPROFILE  lcm2OpenProfileFile     ( const char        * my_space_profile,
                                       const char        * my_space_profile_path )
{
  cmsHPROFILE h_my_space = 0;

  if(my_space_profile_path == NULL) my_space_profile_path = "";
  if(my_space_profile && my_space_profile[0])
  {
    char * full_name = (char*) malloc(strlen(my_space_profile_path) + strlen(my_space_profile) + 1);
    if(!full_name) return NULL;
    sprintf( full_name, "%s%s", my_space_profile_path, my_space_profile );

    if(strcmp(my_space_profile,"*lab") == 0)
      h_my_space = cmsCreateLab4Profile(cmsD50_xyY());
    else
    if(strcmp(my_space_profile,"*xyz") == 0)
      h_my_space = cmsCreateXYZProfile( );
    else
    if(strcmp(my_space_profile,"*srgb") == 0)
      h_my_space = cmsCreate_sRGBProfile( );
    else
    if(strcmp(my_space_profile,"*srgblinear") == 0)
      h_my_space = lcm2CreateICCMatrixProfile2( 1.0, 0.64,  0.33,
                                                     0.30,  0.60,
                                                     0.15,  0.06,
                                                     0.3127,0.329 );
    else /* ITU-R BT.601-7 625-line, 50 field/s systems */
    if(strcmp(my_space_profile,"*rec601.625.linear") == 0)
      h_my_space = lcm2CreateICCMatrixProfile2( 1.0, 0.64,  0.33,
                                                     0.29,  0.60,
                                                     0.15,  0.06,
                                                     0.3127,0.329 );
    else /* ITU-R BT.601-7 525-line, 60/1.001, field/s systems */
    if(strcmp(my_space_profile,"*rec601.525.linear") == 0)
      h_my_space = lcm2CreateICCMatrixProfile2( 1.0, 0.63,  0.34,
                                                     0.31,  0.595,
                                                     0.155, 0.07,
                                                     0.3127,0.329 );

    if(!h_my_space)
      h_my_space = cmsOpenProfileFromFile( full_name, "rb" );

    if(!h_my_space) { lcm2msg_p( 300, NULL, "no profile from %s", full_name); }
    /*else printf("will use %s\n", full_name);*/

    lcm2Free_m(full_name);
  }

  return h_my_space;
}

/** Function  lcm2WriteProfileToFile
 *  @brief    Write a profile to a file
 *
 *  Suggested is a scheme of "space version vendor.icc".
 *
 *  @code
    // "My-Space_v1.0_myna.icc"
    char * file_name  = lcm2WriteProfileToFile( my_space_profile,
                                                "My-Space", "v1.0", "myna" );
    @endcode
 *
 *  @param[in]     my_space_profile                        the profile
 *  @param[in]     my_space_profile_name                   the color space name
 *  @param[in]     my_space_profile_version                the version of the profile; optional
 *  @param[in]     vendor_four_bytes                       the vendor, just four bytes; optional
 *  @return                                                constructed file name;
 *                                                         can be released with free()
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/06
 *  @since   2016/02/16 (Oyranos: 0.9.6)
 */
char *       lcm2WriteProfileToFile  ( cmsHPROFILE         my_space_profile,
                                       const char        * my_space_profile_name,
                                       const char        * my_space_profile_version,
                                       const char        * vendor_four_bytes )
{
  int i;

  i = 0;
  char * fn = (char*) malloc(strlen(my_space_profile_name) + 
                             (my_space_profile_version ? strlen(my_space_profile_version):0) +
                             (vendor_four_bytes ? strlen(vendor_four_bytes):0) + 8);
  if(!fn) return fn;
  sprintf( fn, "%s%s%s%s%s%s", my_space_profile_name,
           my_space_profile_version ? " " : "", my_space_profile_version?my_space_profile_version:"",
           vendor_four_bytes ? " " : "", vendor_four_bytes?vendor_four_bytes:"",
           strstr(my_space_profile_name, ".icc") ? "" : ".icc" );
  while(fn[i]) { if(fn[i] == ' ') fn[i] = '_'; ++i; }
  cmsSaveProfileToFile( my_space_profile, fn );

  return fn;
}

/** Function lcm2WriteProfileToMem
 *
 *  Save a cmsHPROFILE to a in memory data blob
 *
 *  @version Oyranos: 0.9.7
 *  @since   2008/12/28 (Oyranos: 0.9.7)
 *  @date    2017/06/07
 */
void *       lcm2WriteProfileToMem   ( cmsHPROFILE       * profile,
                                       size_t            * size,
                                       void *            (*allocateFunc)(size_t size) )
{
  int error = !profile;
  void * data = 0;
  cmsUInt32Number size_ = 0;

  if(!error)
  {
    *size = 0;

    if(!cmsSaveProfileToMem( profile, NULL, &size_ ))
      lcm2msg_p( 300, NULL, "cmsSaveProfileToMem failed" );

    if(size_)
    {
      if(allocateFunc)
        data = allocateFunc( size_ );
      else
        data = malloc( size_ );

      cmsSaveProfileToMem( profile, data, &size_ );

    } else
      lcm2msg_p( 300, NULL, "can not convert lcms2 profile to memory" );

    *size = size_;

  } else
      lcm2msg_p( 301, NULL, "no profle" );

  return data;
}

/* --- CIE*Lab space familiy --- */

/** \addtogroup samplers Samplers
 *  @{ */

static double CIE_C_scaler = M_SQRT2; /* fit all Lab into LCh */
/** Function  lcm2SamplerLab2LCh
 *  @brief    CIE*Lab -> CIE*LCh in PCS*Lab range
 *
 *  The CIE*C channel is scaled to contain all CIE*Lab colors.
 *  The ICC PCS*Lab space with range of 0->1 for all channels is utilised to
 *  be useful as a sampler argument to lcm2CreateProfileLutByFunc().
 *
 *  @param[in]     i                   input Lab triple
 *  @param[out]    o                   output LCh triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/13
 *  @since   2016/13/13 (Oyranos: 0.9.6)
 */
void         lcm2SamplerLab2LCh      ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
  double a = (i[1] - 0.5) * CIE_C_scaler,
         b = (i[2] - 0.5) * CIE_C_scaler;

  /* CIE*L */
  o[0] = i[0];
  /* CIE*C = sqrt(CIE*a² + CIE*b²) */
  o[1] = hypot(a,b);
  /* CIE*h = atan2(CIE*b, CIE*a) */
  o[2] = atan2(b,a)/M_PI/2.0 + 0.5;
}

/** Function  lcm2SamplerLCh2Lab
 *  @brief    CIE*LCh -> CIE*Lab in PCS*Lab range
 *
 *  The CIE*C channel is scaled to contain all CIE*Lab colors.
 *  The ICC PCS*Lab space with range of 0->1 for all channels is utilised to
 *  be useful as a sampler argument to lcm2CreateProfileLutByFunc().
 *
 *  @param[in]     i                   input LCh triple
 *  @param[out]    o                   output Lab triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/12/05
 *  @since   2016/13/13 (Oyranos: 0.9.6)
 */
void         lcm2SamplerLCh2Lab      ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
  /* CIE*L */
  o[0] = i[0];
  /* CIE*a = C * cos(h) */
  o[1] = 1.0 - (i[1] * cos(M_PI*2.0*i[2]) / CIE_C_scaler + 0.5);
  /* CIE*b = C * sin(h) */
  o[2] = 1.0 - (i[1] * sin(M_PI*2.0*i[2]) / CIE_C_scaler + 0.5);
}



/* sRGB */
cmsViewingConditions lcm2_vc_srgb_ =
{
    { 95.05, 100.0, 108.88 }, /* D65 white point */
    20, /* viewing background luminance Yb */
    4,  /* ambient in cd/m² (== 64 lux) */
    2,  /* Dim sourround */
    1   /* adapted (0-1) */
};

/** Function  lcm2SamplerJCh2Lab
 *  @brief    CIE*LCh -> CIE*Lab in PCS*Lab range
 *
 *  The CIE*C channel is scaled to contain all CIE*Lab colors.
 *  The ICC PCS*Lab space with range of 0->1 for all channels is utilised to
 *  be useful as a sampler argument to lcm2CreateProfileLutByFunc().
 *
 *  @param[in]     i                   input LCh triple
 *  @param[out]    o                   output Lab triple
 *  @param[in]     v                   (cmsViewingConditions*); optional, default sRGB
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/28
 *  @since   2018/02/28 (Oyranos: 0.9.7)
 */
void         lcm2SamplerJCh2Lab      ( const double        i[],
                                       double              o[],
                                       void              * v )
{
  cmsViewingConditions * vc = &lcm2_vc_srgb_;
  cmsHANDLE vh;
  cmsCIEXYZ XYZ;
  cmsJCh JCh = { i[0], i[1], i[2] };

  vh = cmsCIECAM02Init( NULL, v?v:vc );
  cmsCIECAM02Reverse( vh, &JCh, &XYZ );
  cmsCIECAM02Done( vh );

  lcm2CIEXYZ2iccLab( &XYZ, o );
}

/** Function  lcm2SamplerLab2JCh
 *  @brief    CIE*Lab -> CIE*JCh
 *
 *  The CIECAM02 appearance space.
 *
 *  @param[in]     i                   input Lab triple
 *  @param[out]    o                   output JCh triple
 *  @param[in]     v                   (cmsViewingConditions*); optional, default sRGB
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/28
 *  @since   2018/02/28 (Oyranos: 0.9.7)
 */
void         lcm2SamplerLab2JCh      ( const double        i[],
                                       double              o[],
                                       void              * v )
{
  cmsViewingConditions * vc = &lcm2_vc_srgb_;
  cmsHANDLE vh;
  cmsCIEXYZ XYZ;
  cmsJCh JCh;

  lcm2iccLab2CIEXYZ( i, &XYZ );

  vh = cmsCIECAM02Init( NULL, v?v:vc );
  cmsCIECAM02Forward( vh, &XYZ, &JCh );
  cmsCIECAM02Done( vh );

  o[0] = JCh.J;
  o[1] = JCh.C;
  o[2] = JCh.h;
}


/* --- YCbCr space familiy --- */
typedef enum {
  ITU_R_BT_601,
  ITU_R_BT_601_JPEG,
  ITU_REC_709,
  ITU_R_BT_2020
} ITU_Std_e;
const char * ITU_Std_dscr [] = { "ITU-R BT.601", "ITU-R BT.601 / JPEG", "ITU REC-709", "ITU-R BT.2020", NULL };

static void selectKbKr( ITU_Std_e ITU_Std, double * Kb, double * Kr )
{
  switch(ITU_Std)
  {
    case ITU_R_BT_601:
    case ITU_R_BT_601_JPEG:
    // ITU-R BT.601 - JPEG
    *Kb = 0.114;
    *Kr = 0.299;
    break;
    case ITU_REC_709:
    // ITU REC-709
    *Kb = 0.0722;
    *Kr = 0.2126;
    break;
    case ITU_R_BT_2020:
    // ITU-R BT.2020
    *Kb = 0.0593;
    *Kr = 0.2627;
    break;
  }
}

void selectBlackScale( ITU_Std_e ITU_Std, double * black, double * scale )
{
  switch(ITU_Std)
  {
    case ITU_R_BT_601_JPEG:
    *black = 0;
    *scale = 255;
    break;
    case ITU_R_BT_601:
    case ITU_REC_709:
    case ITU_R_BT_2020:
    *black = 16;
    *scale = 219;
    break;
  }
}

void linear2ycbcr( double *L_ )
{
  double L = *L_;
  double alpha = 1.09929682680944,
         beta  = 0.018053968510807;
  // linear -> gamma
  if(L < beta)
    L *= 4.5;
  else
    L = pow(L,0.45) - (alpha - 1);
  *L_ = L;
}

void ycbcr2linear( double *V_ )
{
  double L = *V_;
  double alpha = 1.09929682680944,
         beta  = 0.081243; /* 0.018053968510807 * 4.5 */
  // linear -> gamma
  if(L < beta)
    L /= 4.5;
  else
    L = pow( (L + (alpha-1)) / alpha, 1.0/0.45 );
  *V_ = L;
}

static void rgb2ycbcr( double R, double G, double B,
                double *Y_, double *Pb_, double *Pr_,
                double Kb, double Kr )
{
  double Y,Pb,Pr;
  // common RGB -> YCbCr formula
  Y = Kr * R + (1.0-Kr-Kb) * G + Kb * B;
  Pb = 1.0/2.0 * (B-Y)/(1.0-Kb);
  Pr = 1.0/2.0 * (R-Y)/(1.0-Kr);
  *Y_ = Y; *Pb_ = Pb; *Pr_ = Pr;
}

static void ycbcr2rgb( double Y, double Pb, double Pr,
                double *R_, double *G_, double *B_,
                double Kb, double Kr )
{
  double R,G,B;
  // common YCbCr -> RGB formula

  // Pb = 1.0/2.0 * (B-Y)/(1.0-Kb);
  //  2*Pb = (B-Y)/(1-Kb)
  //  2*Pb*(1-Kb) = B-Y
  //  2*Pb*(1-Kb)+Y = B
  B = 2*Pb*(1-Kb) + Y;

  // Pr = 1.0/2.0 * (R-Y)/(1.0-Kr);
  //  2*Pr*(1-Kr)+Y = R
  R = 2*Pr*(1-Kr) + Y;

  // Y = Kr * R + (1.0-Kr-Kb) * G + Kb * B;
  //  Y-(Kr*R)-(Kb*B) = (1-Kb-Kr) * G
  //  (Y-(Kr*R)-(Kb*B))/(1-Kb-Kr) = G
  G = (Y - Kr*R - Kb*B)/(1.0-Kb-Kr);

  *R_ = R; *G_ = G; *B_ = B;
}

static void scaleRGB( ITU_Std_e ITU_Std, double scale, double * R, double * G, double * B )
{
  switch(ITU_Std)
  {
    case ITU_R_BT_601: // ITU-R BT.601
    case ITU_REC_709: // ITU REC-709
    case ITU_R_BT_2020: // ITU-R BT.2020
    case ITU_R_BT_601_JPEG: // ITU-R BT.601 - JPEG
    *R *= scale;
    *G *= scale;
    *B *= scale;
    break;
  }
}

static void scaleLinearToYCbCr( ITU_Std_e ITU_Std, double max, double * Y, double * Cb, double * Cr )
{
  max /= 255.0;
  switch(ITU_Std)
  {
    case ITU_R_BT_601: // ITU-R BT.601
    case ITU_REC_709: // ITU REC-709
    case ITU_R_BT_2020: // ITU-R BT.2020
    *Y *= (235.*max-16.*max);
    *Y += 16.*max;
    *Cb *= (240.*max-16.*max);
    *Cb += 128.*max;
    *Cr *= (240.*max-16.*max);
    *Cr += 128.*max;
    break;
    case ITU_R_BT_601_JPEG: // ITU-R BT.601 - JPEG
    *Y *= 255.*max;
    *Cb *= 255.*max;
    *Cb += 128.*max;
    *Cr *= 255.*max;
    *Cr += 128.*max;
    break;
  }
}

static void scaleYCbCrToLinear( ITU_Std_e ITU_Std, double max, double * Y, double * Cb, double * Cr )
{
  max /= 255.0;
  switch(ITU_Std)
  {
    case ITU_R_BT_601: // ITU-R BT.601
    case ITU_REC_709: // ITU REC-709
    case ITU_R_BT_2020: // ITU-R BT.2020
    *Y -= 16.*max;
    *Y /= (235.*max-16.*max);
    *Cb -= 128.*max;
    *Cb /= (240.*max-16.*max);
    *Cr -= 128.*max;
    *Cr /= (240.*max-16.*max);
    break;
    case ITU_R_BT_601_JPEG: // ITU-R BT.601 - JPEG
    *Y /= 255.*max;
    *Cb -= 128.*max;
    *Cb /= 255.*max;
    *Cr -= 128.*max;
    *Cr /= 255.*max;
    break;
  }
}

/** Function  lcm2SamplerRGB2JpegYCbCr
 *  @brief    RGB -> YCbCr in Jpeg range
 *
 *  ITU R BT 601 / REC.601 coefficients with Jpeg range of 0-1 is generated.
 *
 *  @param[in]     i                   input RGB triple
 *  @param[out]    o                   output REC.601 YCbCr in JPEG range triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/13
 *  @since   2016/03/07 (Oyranos: 0.9.6)
 */
void         lcm2SamplerRGB2JpegYCbCr (
                                       const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
    /* final space PCS.Lab -> YCbCr */

    /** Jpeg assumes no gamma correction.
     *  Thus this sampler converts from RGB.
     */
  ITU_Std_e std = ITU_R_BT_601_JPEG;
  double Kr,Kb,
         Y = i[0], Pb = i[1], Pr = i[2],
         R = i[0], G = i[1], B = i[2];

  selectKbKr( std, &Kb, &Kr );

  scaleRGB( std, 1.0, &R, &G, &B );
  rgb2ycbcr( R, G, B, &Y, &Pb, &Pr, Kb,Kr );
  scaleLinearToYCbCr( std, 1.0, &Y, &Pb, &Pr );

  o[0] = Y; o[1] = Pb; o[2] = Pr;
}

/** Function  lcm2SamplerRGB2JpegYCbCr
 *  @brief    YCbCr in Jpeg range -> RGB
 *
 *  ITU R BT 601 / REC.601 coefficients in Jpeg range of 0-1 is assumed.
 *
 *  @param[in]     i                   input REC.601 YCbCr in JPEG range triple
 *  @param[out]    o                   output RGB triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/13
 *  @since   2016/03/13 (Oyranos: 0.9.6)
 */
void         lcm2SamplerJpegYCbCr2RGB( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
    /* final space YCbCr -> PCS.Lab
     * Jpeg assumes no gamma correction
     * Thus this sampler converts to RGB
     * YCbCr -> scale range -> linear YCbCr -> (linear RGB (REC.709) -> Lab)
     */
  ITU_Std_e std = ITU_R_BT_601_JPEG;
  double Kr,Kb,
         Y = i[0], Pb = i[1], Pr = i[2],
         R,G,B;

  selectKbKr( std, &Kb, &Kr );

  scaleYCbCrToLinear( std, 1.0, &Y, &Pb, &Pr );
  ycbcr2rgb( Y, Pb, Pr, &R, &G, &B, Kb,Kr );
  scaleRGB( std, 1.0, &R, &G, &B );

  o[0] = R; o[1] = G; o[2] = B;
}

/** Function  lcm2SamplerIdendity
 *  @brief    Lab -> Lab
 *
 *  PCS Lab range of 0-1 for all channels is assumed.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/26
 *  @since   2018/02/26 (Oyranos: 0.9.7)
 */
void         lcm2SamplerIdendity     ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
  o[0] = i[0]; // L / CIE*L / Y  / R
  o[1] = i[1]; // M / CIE*a / Cb / G
  o[2] = i[2]; // S / CIE*b / Cr / B
}

/** Function  lcm2SamplerGrayer
 *  @brief    Lab -> Gray -> Lab
 *
 *  PCS Lab range of 0-1 for all channels is assumed.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/13
 *  @since   2016/03/13 (Oyranos: 0.9.6)
 */
void         lcm2SamplerGrayer       ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
  o[0] = i[0]*1.0; // L / CIE*L / Y  / R
  o[1] = 0.5; // M / CIE*a / Cb / G
  o[2] = 0.5; // S / CIE*b / Cr / B
}

/** Function  lcm2SamplerBlacknWhite
 *  @brief    Lab -> Black&White -> Lab
 *
 *  PCS Lab range of 0-1 for all channels is assumed.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/13
 *  @since   2016/03/13 (Oyranos: 0.9.6)
 */
void         lcm2SamplerBlacknWhite  ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
  if(i[0] <= 0.5)
    o[0] = 0.0; // L / CIE*L / Y  / R
  else
    o[0] = 1.0; // L / CIE*L / Y  / R
  o[1] = 0.5; // M / CIE*a / Cb / G
  o[2] = 0.5; // S / CIE*b / Cr / B
}

/** Function  lcm2SamplerSepia
 *  @brief    Lab -> LCh -> Yellow -> LCh -> Lab
 *
 *  PCS Lab range of 0-1 for all channels is assumed.
 *  Creates a single reddish hue.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/14
 *  @since   2016/03/14 (Oyranos: 0.9.6)
 */
void         lcm2SamplerSepia        ( const double        i[],
                                       double              o[],
                                       void              * none )
{
  double in[3],out[3];
  lcm2SamplerLab2LCh( i,in,none );
 
  out[0] = in[0];
  out[1] = 0.04+0.04*in[0];
  out[2] = 0.18;

  lcm2SamplerLCh2Lab( out,o,none );
}

/** Function  lcm2SamplerReddish
 *  @brief    Lab -> reddish tint -> Lab
 *
 *  PCS Lab range of 0-1 for all channels is assumed.
 *  Same like Sepia, but gives all colors a reddish tint.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/15
 *  @since   2016/03/15 (Oyranos: 0.9.6)
 */
void         lcm2SamplerReddish      ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
  o[0] = i[0];
  o[1] = i[1] + 0.012+0.012*i[0];
  o[2] = i[2] + 0.025+0.025*i[0];
}

/** Function  lcm2SamplerWhitePointLab
 *  @brief    Lab -> White Point Adaption -> Lab
 *
 *  PCS Lab range of 0-1 for all channels is assumed.
 *  Same like reddish, but adapts all colors to a given white point difference.
 *  It uses simple linear adaption inside CIE*Lab.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param[out]    data                pointer to array of two doubles with 
 *                                     desired ICC*ab differences
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/05/17
 *  @since   2017/05/17 (Oyranos: 0.9.7)
 */
void         lcm2SamplerWhitePointLab( const double        i[],
                                       double              o[],
                                       void              * data )
{
  double * icc_ab = (double*) data;

  o[0] = i[0];
  o[1] = i[1] + icc_ab[0] * i[0];
  o[2] = i[2] + icc_ab[1] * i[0];
}

/** Function  lcm2iccLab2CIEXYZ
 *  @brief    ICC*Lab -> CIE*XYZ
 *
 *  Converts from PCS Lab encoding to lcms XYZ type.
 *
 *  @param[in]     icc_Lab             input Lab triple in PCS range
 *  @param[out]    XYZ                 output XYZ struct
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/28
 *  @since   2018/02/28 (Oyranos: 0.9.7)
 */
void         lcm2iccLab2CIEXYZ       ( const double      * icc_Lab,
                                       cmsCIEXYZ         * XYZ )
{
  cmsCIELab Lab;

  Lab.L = icc_Lab[0] * 100.0;
  Lab.a = icc_Lab[1] * 257.0 - 128.0;
  Lab.b = icc_Lab[2] * 257.0 - 128.0;

  cmsLab2XYZ( cmsD50_XYZ(), XYZ, &Lab);
}

/** Function  lcm2CIEXYZ2iccLab
 *  @brief    CIE*XYZ -> ICC*Lab
 *
 *  Converts from lcms XYZ type to PCS Lab encoding.
 *
 *  @param[in]     XYZ                 input XYZ struct
 *  @param[out]    icc_Lab             output Lab triple in PCS range
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/28
 *  @since   2018/02/28 (Oyranos: 0.9.7)
 */
void         lcm2CIEXYZ2iccLab       ( const cmsCIEXYZ   * XYZ,
                                       double            * icc_Lab )
{
  cmsCIELab Lab;

  cmsXYZ2Lab( cmsD50_XYZ(), &Lab, XYZ );

  icc_Lab[0] =  Lab.L          / 100.0;
  icc_Lab[1] = (Lab.a + 128.0) / 257.0;
  icc_Lab[2] = (Lab.b + 128.0) / 257.0;
}

/** Function  lcm2iccXYZ2iccLab
 *  @brief    ICC*XYZ -> ICC*Lab
 *
 *  Converts from PCS XYZ to PCS Lab encoding.
 *
 *  @param[in]     XYZ                 input XYZ triple
 *  @param[out]    icc_Lab             output Lab triple in PCS range
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/28
 *  @since   2018/02/28 (Oyranos: 0.9.7)
 */
void         lcm2iccXYZ2iccLab       ( const double      * XYZ,
                                       double            * icc_Lab )
{
  cmsCIEXYZ XYZ_ = { XYZ[0], XYZ[1], XYZ[2] };
  lcm2CIEXYZ2iccLab( &XYZ_, icc_Lab );
}

/** Function  lcm2SamplerWhitePointBradford
 *  @brief    Lab -> Bradford White Point Adaption -> Lab
 *
 *  PCS Lab range of 0-1 for all channels is assumed.
 *  Same like reddish, but adapts all colors to a given white point difference.
 *  It uses Bradford CAT.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param[out]    data                pointer to array of two doubles with 
 *                                     source ICC*XYZ white point, followed by
 *                                     destination ICC*XYZ whitepoint
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/28
 *  @since   2018/02/28 (Oyranos: 0.9.7)
 */
void lcm2SamplerWhitePointBradford   ( const double        i[],
                                       double              o[],
                                       void              * data )
{
  double * icc_XYZ = (double*) data;
  double scale = 100.0;
  cmsCIEXYZ srcXYZwtpt, iXYZ, oXYZ, dstXYZillu;

  srcXYZwtpt.X = icc_XYZ[0] * scale;
  srcXYZwtpt.Y = icc_XYZ[1] * scale;
  srcXYZwtpt.Z = icc_XYZ[2] * scale;
  dstXYZillu.X = icc_XYZ[3+0] * scale;
  dstXYZillu.Y = icc_XYZ[3+1] * scale;
  dstXYZillu.Z = icc_XYZ[3+2] * scale;

  lcm2iccLab2CIEXYZ( i, &iXYZ );
  cmsAdaptToIlluminant( &oXYZ, &srcXYZwtpt, &dstXYZillu, &iXYZ );
  lcm2CIEXYZ2iccLab( &oXYZ, o );
}


/** Function  lcm2SamplerProof
 *  @brief    Lab -> proofing profile -> Lab
 *
 *  Convert a proofing profile into a abstract one.
 *  Abstract profiles can easily be merged into a multi profile transform.
 *  PCS Lab range of 0-1 for all channels is assumed.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param[out]    data                pointer to array of two void* with 
 *                                     - desired cmsHTRANSFORM
 *                                       for uint32_t arrays in PT_Lab
 *                                     - cmsFLAGS_GAMUTCHECK flag
 *
 *  @version Oyranos: 0.9.7
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 *  @date    2017/06/03
 */
void         lcm2SamplerProof        ( const double        i[],
                                             double        o[],
                                       void              * data )
{
  cmsCIELab Lab1, Lab2;
  double d;
  cmsFloat32Number i_[3], o_[3];
  void ** ptr = (void**)data;

  i_[0] = Lab1.L = i[0] * 100.0;
  i_[1] = Lab1.a = i[1] * 257.0 - 128.0;
  i_[2] = Lab1.b = i[2] * 257.0 - 128.0;

  cmsDoTransform( ptr[0], i_, o_, 1 );

  Lab2.L = o_[0]; Lab2.a = o_[1]; Lab2.b = o_[2];

  d = cmsDeltaE( &Lab1, &Lab2 );
  if((fabs(d) > 10) && ptr[1] != NULL)
  {
    Lab2.L = 50.0;
    Lab2.a = Lab2.b = 0.0;
  }

  o[0] = Lab2.L/100.0; 
  o[1] = (Lab2.a + 128.0) / 257.0;
  o[2] = (Lab2.b + 128.0) / 257.0;
}

/** Function  lcm2SamplerProofD
 *  @brief    Lab -> proofing profile -> Lab
 *
 *  Convert a proofing profile into a abstract one.
 *  Abstract profiles can easily be merged into a multi profile transform.
 *  PCS Lab range of 0-1 for all channels is assumed.
 *
 *  @param[in]     i                   input PCS.Lab triple
 *  @param[out]    o                   output PCS.Lab triple
 *  @param[out]    data                pointer to array of two void* with 
 *                                     - desired cmsHTRANSFORM and
 *                                       for uint64_t arrays in PT_Lab
 *                                     - cmsFLAGS_GAMUTCHECK flag
 *
 *  @version Oyranos: 0.9.7
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 *  @date    2017/11/06
 */
void         lcm2SamplerProofD       ( const double        i[],
                                             double        o[],
                                       void              * data )
{
  cmsCIELab Lab1, Lab2;
  double d;
  cmsFloat64Number i_[3], o_[3];
  void ** ptr = (void**)data;

  i_[0] = Lab1.L = i[0] * 100.0;
  i_[1] = Lab1.a = i[1] * 257.0 - 128.0;
  i_[2] = Lab1.b = i[2] * 257.0 - 128.0;

  cmsDoTransform( ptr[0], i_, o_, 1 );

  Lab2.L = o_[0]; Lab2.a = o_[1]; Lab2.b = o_[2];

  d = cmsDeltaE( &Lab1, &Lab2 );
  if((fabs(d) > 10) && ptr[1] != NULL)
  {
    Lab2.L = 50.0;
    Lab2.a = Lab2.b = 0.0;
  }

  o[0] = Lab2.L/100.0; 
  o[1] = (Lab2.a + 128.0) / 257.0;
  o[2] = (Lab2.b + 128.0) / 257.0;
}

/** @} */ /* samplers */

/** Function  lcm2CreateProfileLutByFunc
 *  @brief    Generate a ICC profile LUT
 *
 *  This function takes a series of parameters and functions to create a
 *  ICC profile from. The sampler function operates in a input space and
 *  and creates colors in a output space. These values are filled into the
 *  profile LUT. It is possible to create effect profiles of class abstract
 *  or LUT profiles in any other color space including device links.
 *
 *  For some already available sampler funtions see @ref samplers.
 *
 *  @param[in,out] profile             profile to add LUT table
 *  @param[in]     samplerMySpace      the function to fill the LUT with color
 *  @param[in]     samplerArg          data pointer to samplerMySpace
 *  @param[in]     my_space_profile    operating color space
 *                                     for samplerMySpace(); for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     in_space_profile    input color space
 *                                     for samplerMySpace(); for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     out_space_profile   output color space
 *                                     for samplerMySpace(); for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     grid_size           dimensions of the created LUT; e.g. 33
 *  @param[in]     tag_sig             tag signature for the generated LUT;
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/05/17
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 */
int          lcm2CreateProfileLutByFunc (
                                       cmsHPROFILE         profile,
                                       lcm2Sampler_f       samplerMySpace,
                                       void              * samplerArg,
                                       const char        * in_space_profile,
                                       const char        * my_space_profile,
                                       const char        * out_space_profile,
                                       int                 grid_size,
                                       cmsTagSignature     tag_sig
                                     )
{
  cmsToneCurve * t[max_channels];
  int i;
  int error = 0;

  if(!profile) return 1;

  t[0] = cmsBuildGamma(0, 1.0);
  if(!t[0]) return 1;
  for(i = 1; i < max_channels; ++i) t[i] = t[0];

  error = lcm2CreateProfileLutByFuncAndCurves (
                                       profile,
                                       samplerMySpace,
                                       samplerArg,
                                       t, t,
                                       in_space_profile,
                                       my_space_profile,
                                       out_space_profile,
                                       grid_size, tag_sig
                                     );

  cmsFreeToneCurve( t[0] );

  return error;
}

/** Function  lcm2CreateProfileLutByFuncAndCurves
 *  @brief    Generate a ICC profile LUT
 *
 *  This function takes a series of parameters and functions to create a
 *  ICC profile from. The sampler function operates in a input space and
 *  and creates colors in a output space. These values are filled into the
 *  profile LUT. It is possible to create effect profiles of class abstract
 *  or LUT profiles in any other color space including device links.
 *
 *  For some already available sampler funtions see @ref samplers.
 *
 *  @param[in,out] profile             profile to add LUT table
 *  @param[in]     samplerMySpace      the function to fill the LUT with color
 *  @param[in]     samplerArg          data pointer to samplerMySpace
 *  @param[in]     in_curves           input curves
 *  @param[in]     out_curves          output curves
 *  @param[in]     my_space_profile    operating color space
 *                                     for samplerMySpace(); for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     in_space_profile    input color space
 *                                     for samplerMySpace(); for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     out_space_profile   output color space
 *                                     for samplerMySpace(); for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     grid_size           dimensions of the created LUT; e.g. 33
 *  @param[in]     tag_sig             tag signature for the generated LUT;
 *
 *  @version Oyranos: 0.9.6
 *  @date    2017/05/17
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 */
int          lcm2CreateProfileLutByFuncAndCurves (
                                       cmsHPROFILE         profile,
                                       lcm2Sampler_f       samplerMySpace,
                                       void              * samplerArg,
                                       cmsToneCurve      * in_curves[],
                                       cmsToneCurve      * out_curves[],
                                       const char        * in_space_profile,
                                       const char        * my_space_profile,
                                       const char        * out_space_profile,
                                       int                 grid_size,
                                       cmsTagSignature     tag_sig
                                     )
{
  cmsHPROFILE h_in_space = 0,
              h_my_space = 0,
              h_out_space = 0;
  cmsHTRANSFORM tr_In2MySpace = 0, tr_MySpace2Out = 0;
  cmsStage * gmt_lut = 0, * gmt_lut16 = 0;
  cmsPipeline * gmt_pl = cmsPipelineAlloc( 0,3,3 ),
              * gmt_pl16 = cmsPipelineAlloc( 0,3,3 );
 
  lcm2Cargo_s cargo;
  int i;
  int error = 0;
  int in_layout, my_layout, out_layout;
  in_layout = my_layout = out_layout = (FLOAT_SH(1)|CHANNELS_SH(3)|BYTES_SH(0));

  if(!profile) return 1;

  if(in_space_profile) h_in_space  = lcm2OpenProfileFile( in_space_profile, NULL );
  if(my_space_profile) h_my_space  = lcm2OpenProfileFile( my_space_profile, NULL );
  if(out_space_profile)h_out_space = lcm2OpenProfileFile( out_space_profile, NULL );

  if(h_in_space && h_my_space && strcmp(in_space_profile,my_space_profile) != 0)
  {

    tr_In2MySpace = cmsCreateTransformTHR ( 0, h_in_space, in_layout,
                                            h_my_space, my_layout,
                                            INTENT_RELATIVE_COLORIMETRIC,
                                            cmsFLAGS_NOOPTIMIZE);
    if(!tr_In2MySpace) { lcm2msg_p( 300, NULL, "no transform"); error = 1; goto lcm2CreateProfileLutByFuncAndCurvesClean; }
  }

  if(h_my_space && h_out_space && strcmp(my_space_profile,out_space_profile) != 0)
  {
    tr_MySpace2Out = cmsCreateTransformTHR( 0, h_my_space, my_layout,
                                            h_out_space, out_layout,
                                            INTENT_RELATIVE_COLORIMETRIC,
                                            cmsFLAGS_NOOPTIMIZE);

    if(!tr_MySpace2Out) { lcm2msg_p( 300, NULL, "no transform"); error = 1; goto lcm2CreateProfileLutByFuncAndCurvesClean; }
  }
      
  memset(&cargo, 0, sizeof(lcm2Cargo_s));
  cargo.in2MySpace = tr_In2MySpace;
  cargo.mySpace2Out = tr_MySpace2Out;
  cargo.sampler = samplerMySpace;
  cargo.sampler_variables = samplerArg,
  cargo.channelsIn = h_in_space ? cmsChannelsOf( cmsGetColorSpace( h_in_space ) ) : 3;
  cargo.channelsProcess = h_my_space ? cmsChannelsOf( cmsGetColorSpace( h_my_space ) ) : 3;
  cargo.channelsOut = h_out_space ? cmsChannelsOf( cmsGetColorSpace( h_out_space ) ) : 3;

#pragma omp parallel for
  for(i = 0; i < 2; ++i)
  {
    if(i)
    {
      gmt_lut16 = cmsStageAllocCLut16bit( 0, grid_size, 3,3,0 );
      cmsStageSampleCLut16bit( gmt_lut16, lcm2sampler16, &cargo, 0 );

    } else
    {
      gmt_lut = cmsStageAllocCLutFloat( 0, grid_size, 3,3,0 );
      cmsStageSampleCLutFloat( gmt_lut, lcm2samplerFloat, &cargo, 0 );
    }
  }


  /* 16-bit int */
  cmsPipelineInsertStage( gmt_pl16, cmsAT_BEGIN,
                          cmsStageAllocToneCurves( 0, cargo.channelsIn, in_curves ) );
  cmsPipelineInsertStage( gmt_pl16, cmsAT_END, gmt_lut16 );
  cmsPipelineInsertStage( gmt_pl16, cmsAT_END,
                          cmsStageAllocToneCurves( 0, cargo.channelsOut, out_curves ) );
  cmsWriteTag( profile, (tag_sig!=0)?tag_sig:cmsSigAToB0Tag, gmt_pl16 );

  /* float */
  /* cmsPipeline owns the cmsStage memory */
  cmsPipelineInsertStage( gmt_pl, cmsAT_BEGIN,
                          cmsStageAllocToneCurves( 0, cargo.channelsIn, in_curves ) );
  cmsPipelineInsertStage( gmt_pl, cmsAT_END, gmt_lut );
  cmsPipelineInsertStage( gmt_pl, cmsAT_END,
                          cmsStageAllocToneCurves( 0, cargo.channelsOut, out_curves ) );
  //cmsWriteTag( gmt, cmsSigDToB0Tag, gmt_pl );


lcm2CreateProfileLutByFuncAndCurvesClean:
  if(h_in_space) {cmsCloseProfile( h_in_space );} h_in_space = 0;
  if(h_my_space) {cmsCloseProfile( h_my_space );} h_my_space = 0;
  if(h_out_space) {cmsCloseProfile( h_out_space );} h_out_space = 0;
  if(tr_In2MySpace) {cmsDeleteTransform( tr_In2MySpace );} tr_In2MySpace = 0;
  if(tr_MySpace2Out) {cmsDeleteTransform( tr_MySpace2Out );} tr_MySpace2Out = 0;
  if(gmt_pl16) cmsPipelineFree( gmt_pl16 );
  if(gmt_pl) cmsPipelineFree( gmt_pl );

  return error;
}

/** Function  lcm2CreateProfileLutByMatrixAndCurves
 *  @brief    Generate a ICC Lut profile
 *
 *  This function takes a series of parameters to create a
 *  ICC profile from. It is possible to create effect profiles of class abstract
 *  or LUT profiles in any other color space including device links.
 *  The LUT will contain B curves, a matrix and M curves. The allowed number
 *  of color channels is 3.
 *
 *  @param[in,out] profile             profile to add LUT table;
 *                                     requires a version 4.3 profile
 *  @param[in]     in_curves           input curves
 *  @param[in]     matrix              the 3x3 matrix
 *  @param[in]     out_curves          output curves
 *  @param[in]     in_space_profile    input color space
 *                                     for wildcards see lcm2OpenProfileFile()
 *  @param[in]     out_space_profile   output color space
 *                                     for wildcards see lcm2OpenProfileFile()
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/03/03
 *  @since   2019/03/01 (Oyranos: 0.9.7)
 */
int          lcm2CreateProfileLutByMatrixAndCurves (
                                       cmsHPROFILE         profile,
                                       cmsToneCurve      * in_curves[],
                                       const double      * matrix,
                                       cmsToneCurve      * out_curves[],
                                       const char        * in_space_profile,
                                       const char        * out_space_profile,
                                       cmsTagSignature     tag_sig
                                     )
{
  cmsHPROFILE h_in_space = 0,
              h_out_space = 0;
  int channelsIn, channelsOut;
  cmsPipeline * pl = cmsPipelineAlloc( 0,3,3 );
 
  int error = 0;

  if(!profile) return 1;

  if(in_space_profile) h_in_space  = lcm2OpenProfileFile( in_space_profile, NULL );
  if(out_space_profile)h_out_space = lcm2OpenProfileFile( out_space_profile, NULL );

  channelsIn = h_in_space ? cmsChannelsOf( cmsGetColorSpace( h_in_space ) ) : 3;
  channelsOut = h_out_space ? cmsChannelsOf( cmsGetColorSpace( h_out_space ) ) : 3;
  if(channelsIn == 0 || channelsOut == 0 || !matrix)
  {
    error = 1;
    goto lcm2CreateProfileLutByMatrixAndCurvesClean;
  }

  cmsPipelineInsertStage( pl, cmsAT_BEGIN,
                          cmsStageAllocToneCurves( 0, channelsIn, in_curves ) );
  cmsPipelineInsertStage( pl, cmsAT_END,
                          cmsStageAllocMatrix(0, channelsIn, channelsOut,
                          (const cmsFloat64Number*) matrix, NULL) );
  cmsPipelineInsertStage( pl, cmsAT_END,
                          cmsStageAllocToneCurves( 0, channelsOut, out_curves ) );
  cmsWriteTag( profile, (tag_sig!=0)?tag_sig:cmsSigAToB0Tag, pl );

lcm2CreateProfileLutByMatrixAndCurvesClean:
  if(h_in_space) {cmsCloseProfile( h_in_space );} h_in_space = 0;
  if(h_out_space) {cmsCloseProfile( h_out_space );} h_out_space = 0;
  if(pl) cmsPipelineFree( pl );

  return error;
}

/** Function  lcm2CreateAbstractProfileM
 *  @brief    Create a effect profile of type abstract in ICC*XYZ PCS
 *
 *  Possible computation emlements are m_curve + matrix + b_curve or
 *  matrix only or b_curve only for PCS*XYZ.
 *  In case m_curve and b_curve are passed in, then
 *  PCS*Lab is assumed and m_curve is mapped to PSC*L and
 *  b_curve is mapped to PCS*ab.
 *
 *  Here a code example:
 *  @code
double matrix[3][3] = {
      { 0.0000000,  0.9642957,  0.0000000},
      { 0.0000000,  1.0000000,  0.0000000},
      { 0.0000000,  0.8251046,  0.0000000}
};
const char * name_i18n[] = {
      "de", "DE", "Graustufen (MyProject)",
      "en", "US", "Grayer (MyProject)"
};
lcm2CreateAbstractProfileM ( NULL,
                             &matrix[0][0],
                             NULL,
                             4.3,
                             "Grayer (MyProject)",
                             name_i18n,
                             "Grayer myna",
                             "My Project 2019",
                             "My Name",
                             ICC_2011_LICENSE,
                             "CIE*XYZ",
                             "http://www.cie.co.at",
			     NULL,
			     NULL
                           );
    @endcode
 *
 *  @param[in]    m_curve              optional input curve for all CIE*XYZ;
 *                                     channels in range 0.0 ... PCS*XYZ_MAX
 *  @param[in]    matrix               the 3x3 matrix; optional if one of b_curve or both curves are specified
 *  @param[in]    b_curve              output curve for all PCS*XYZ; optional if matrix is specified;
 *                                     channels in range 0.0 ... 1.0;
 *                                     use LCM2_ADAPT_TO_PCS_XYZ
 *  @param[in]    icc_profile_version  4.3
 *  @param[in]    my_abstract_description                 internal profile name
 *  @param[in]    my_abstract_descriptions                internal profile name translated
 *  @param[in]    my_abstract_file_name                   profile file name. If present a ICC profile will be written to that name. optional
 *  @param[in]    provider             e.g. "My Project 2019"
 *  @param[in]    vendor               e.g. "My Name"
 *  @param[in]    my_license           e.g. "This profile is made available by %s, with permission of %s, and may be copied, distributed, embedded, made, used, and sold without restriction. Altered versions of this profile shall have the original identification and copyright information removed and shall not be misrepresented as the original profile."
 *                                     - first %%s is replaced by the provider string arg and
 *                                     - second %%s is replaced by the vendor string arg
 *  @param[in]    device_model         e.g. "My Set"
 *  @param[in]    device_manufacturer  e.g. "www.mydomain.net"
 *  @param[in]    my_meta_data         e.g. {"DOMAIN_,GROUP_","DOMAIN_key1","value1","GROUP_key2","value2"}
 *  @param[out]   h_profile            the resulting profile
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/03/09
 *  @since   2019/03/03 (Oyranos: 0.9.7)
 */
int          lcm2CreateAbstractProfileM (
                                       cmsToneCurve      * m_curve,
                                       const double      * matrix,
                                       cmsToneCurve      * b_curve,
                                       double              icc_profile_version,
                                       const char        * my_abstract_description,
                                       const char       ** my_abstract_descriptions,
                                       const char        * my_abstract_file_name,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       const char       ** my_meta_data,
                                       cmsHPROFILE       * h_profile
                                     )
{
  cmsHPROFILE profile = 0;
  int error = !matrix && !m_curve && !b_curve;
  cmsToneCurve * m_curves[3] = {NULL,NULL,NULL},
               * b_curves[3] = {NULL,NULL,NULL},
               * allocated_m_curve = NULL, * allocated_b_curve = NULL;
  int i;
  const char * csp = "*xyz"; // color space CIE*XYZ with PCS*XYZ encoding range

  if(m_curve == NULL)
    allocated_m_curve = cmsBuildGamma(0, 1.0);
  if(b_curve == NULL)
    allocated_b_curve = cmsBuildGamma(0, 1.0);
  if(error) goto lcm2CreateAbstractProfileMClean;

  if(m_curve && matrix == NULL && b_curve)
  {
    csp = "*lab";          // CIE*Lab with PCS*Lab encoding range
    b_curves[0] = m_curve; // CIE*L   with PCS*Lab encoding range
    b_curves[1] = b_curve; // CIE*a   with PCS*Lab encoding range
    b_curves[2] = b_curve; // CIE*b   with PCS*Lab encoding range
  }
  else
  {
    for(i = 0; i < 3; ++i)
    {
      if(m_curve)
        m_curves[i] = m_curve;
      else
        m_curves[i] = allocated_m_curve;

      if(b_curve)
        b_curves[i] = b_curve;
      else
        b_curves[i] = allocated_b_curve;
    }
  }

  profile = lcm2CreateProfileFragment (
                             csp, csp,
                             icc_profile_version,
                             my_abstract_description,
                             provider, vendor, my_license, 
                             device_model, device_manufacturer, NULL);
  if(!profile) goto lcm2CreateAbstractProfileMClean;

  if(my_meta_data)
    lcm2AddMetaTexts ( profile, my_meta_data[0], &my_meta_data[1], cmsSigMetaTag );

  if((m_curve && matrix == NULL && b_curve) || // L*ab
     (m_curve == NULL && matrix == NULL))      // XYZ
  {
    cmsPipeline * pl = cmsPipelineAlloc( 0,3,3 );
 
    cmsPipelineInsertStage( pl, cmsAT_END,
                            cmsStageAllocToneCurves( 0, 3, b_curves ) );
    cmsWriteTag( profile, cmsSigAToB0Tag, pl );

    if(pl) cmsPipelineFree( pl );
  }
  else
    error = lcm2CreateProfileLutByMatrixAndCurves( profile,
                                      m_curves, matrix, b_curves,
                                      "*xyz", "*xyz", 
                                      cmsSigAToB0Tag );
  if(error) goto lcm2CreateAbstractProfileMClean;


  lcm2AddMluDescription ( profile, my_abstract_descriptions,
                          cmsSigProfileDescriptionMLTag
                        );

  if(my_abstract_file_name)
  {
    char * fn = lcm2WriteProfileToFile( profile, my_abstract_file_name, 0,0 );
    lcm2msg_p( 302, NULL, "wrote to: %s", fn?fn:"----");
    lcm2Free_m(fn);
  }

  if(h_profile)
    *h_profile = profile;
  else
    cmsCloseProfile( profile );
lcm2CreateAbstractProfileMClean:
  if(allocated_m_curve) cmsFreeToneCurve( allocated_m_curve );
  if(allocated_b_curve) cmsFreeToneCurve( allocated_b_curve );

  return error;
}


/** Function  lcm2CreateAbstractProfile
 *  @brief    Create a effect profile of type abstract in ICC*Lab PCS
 *
 *  Here a code example:
 *  @code
void samplerGrayer      (const double i[],
                               double o[])
{
  o[0] = i[0]*1.0; // L / CIE*L / Y  / R
  o[1] =      0.5; // M / CIE*a / Cb / G
  o[2] =      0.5; // S / CIE*b / Cr / B
}
const char * name_i18n[] = {
      "de", "DE", "Graustufen (MyProject)",
      "en", "US", "Grayer (MyProject)"
};
lcm2CreateAbstractProfile  (
                             samplerGrayer,
                             NULL,
                             "*lab", // CIE*Lab
                             5,
                             2.3,
                             "Grayer (MyProject)",
                             name_i18n,
                             "Grayer myna",
                             "My Project 2016",
                             "My Name",
                             ICC_2011_LICENSE,
                             "CIE*L",
                             "http://www.cie.co.at",
			     NULL,
			     NULL
                           );
    @endcode
 *
 *  @param[in]    samplerMySpace       the function to fill the LUT with color
 *  @param[in]    samplerArg           data pointer to samplerMySpace
 *  @param[in]    my_space_profile                        operating color space
 *                                                        for samplerMySpace();
 *                                                        "*lab" will set CIE*Lab
 *  @param[in]    grid_size            dimensions of the created LUT; e.g. 33
 *  @param[in]    icc_profile_version  2.3 or 4.3
 *  @param[in]    my_abstract_description                 internal profile name
 *  @param[in]    my_abstract_descriptions                internal profile name translated
 *  @param[in]    my_abstract_file_name                   profile file name. If present a ICC profile will be written to that name. optional
 *  @param[in]    provider             e.g. "My Project 2016"
 *  @param[in]    vendor               e.g. "My Name"
 *  @param[in]    my_license           e.g. "This profile is made available by %s, with permission of %s, and may be copied, distributed, embedded, made, used, and sold without restriction. Altered versions of this profile shall have the original identification and copyright information removed and shall not be misrepresented as the original profile."
 *                                     - first %%s is replaced by the provider string arg and
 *                                     - second %%s is replaced by the vendor string arg
 *  @param[in]    device_model         e.g. "My Set"
 *  @param[in]    device_manufacturer  e.g. "www.mydomain.net"
 *  @param[in]    my_meta_data         e.g. {"DOMAIN_,GROUP_","DOMAIN_key1","value1","GROUP_key2","value2"}
 *  @param[out]   h_profile            the resulting profile
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/05/17
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 */
int          lcm2CreateAbstractProfile(
                                       lcm2Sampler_f       samplerMySpace,
                                       void              * samplerArg,
                                       const char        * my_space_profile,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       const char        * my_abstract_description,
                                       const char       ** my_abstract_descriptions,
                                       const char        * my_abstract_file_name,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       const char       ** my_meta_data,
                                       cmsHPROFILE       * h_profile
                                     )
{
  cmsHPROFILE profile = 0;
  int error = 0;

  profile = lcm2CreateProfileFragment (
                             "*lab", // CIE*Lab
                             "*lab", // CIE*Lab
                             icc_profile_version,
                             my_abstract_description,
                             provider, vendor, my_license, 
                             device_model, device_manufacturer, NULL);
  if(!profile) goto lcm2CreateAbstractProfileClean;

  if(my_meta_data)
    lcm2AddMetaTexts ( profile, my_meta_data[0], &my_meta_data[1], cmsSigMetaTag );


  error = lcm2CreateProfileLutByFunc( profile, samplerMySpace, samplerArg,
                                      "*lab", my_space_profile, "*lab",
                                      grid_size, cmsSigAToB0Tag );
  if(error) goto lcm2CreateAbstractProfileClean;


  lcm2AddMluDescription ( profile, my_abstract_descriptions,
                          cmsSigProfileDescriptionMLTag
                        );

  if(my_abstract_file_name)
  {
    char * fn = lcm2WriteProfileToFile( profile, my_abstract_file_name, 0,0 );
    lcm2msg_p( 302, NULL, "wrote to: %s", fn?fn:"----");
    lcm2Free_m(fn);
  }

  if(h_profile)
    *h_profile = profile;
  else
    cmsCloseProfile( profile );
lcm2CreateAbstractProfileClean:

  return error;
}

/** Function  lcm2CreateAbstractTemperatureProfile
 *  @brief    Create a effect profile of type abstract in ICC*Lab PCS from Kelvin
 *
 *  @param[in]    kelvin               the desired temperature in Kelvin; ICC reference (D50) is 5000 Kelvin
 *  @param[in]    source_white_profile a profile, e.g. the actual monitor profile; optional, default is D50
 *  @param[in]    grid_size            dimensions of the created LUT; e.g. 33
 *  @param[in]    icc_profile_version  2.3 or 4.3
 *  @param[out]   my_abstract_file_name                   profile file name
 *  @param[out]   h_profile            the resulting profile; If omitted the function will write the profile to my_abstract_file_name.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/05/17
 *  @since   2017/05/17 (Oyranos: 0.9.7)
 */
int          lcm2CreateAbstractTemperatureProfile (
                                       float               kelvin,
                                       cmsHPROFILE         source_white_profile,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       char             ** my_abstract_file_name,
                                       cmsHPROFILE       * h_profile
                                     )
{
  cmsHPROFILE profile = NULL;
  cmsToneCurve * i_curve[3] = {NULL,NULL,NULL}, * o_curve[3] = {NULL,NULL,NULL};
  /* type[6]  Y = (a * X + b) ^ Gamma + c  order: {g, a, b, c} */
  double curve_params[4] = {1,1,0,0}, curve_params_low[4] = {1,0.95,0,0};
  int i;
  cmsCIEXYZ * source_white = NULL;

  const char * kelvin_meta[] = {
    "EFFECT_class", "reddish,white_point,atom",
    "EFFECT_type", "CIEab",
    "COLORIMETRY_white_point", "yes,reddish,kelvin",
    "CMF_binary", "create-abstract",
    "CMF_version", "0.9.7",
    "CMF_product", "Oyranos",
    0,0
  };
  char * kelvin_name = malloc(1024);
  int error = !kelvin_name;
  double icc_ab[2];
  char * desc = NULL;

  if(error) return 1;

  if(source_white_profile)
  {
    if(cmsIsTag(source_white_profile, cmsSigProfileDescriptionTag))
    {
      cmsUInt32Number n = cmsGetProfileInfoASCII(source_white_profile, cmsInfoDescription, cmsNoLanguage, cmsNoCountry, NULL, 0);
      if(n)
      {
        desc = calloc( n+1, sizeof(char) );
	if(!desc) goto lcm2CreateAbstractTemperatureProfileClean;
        cmsUInt32Number nr = cmsGetProfileInfoASCII(source_white_profile, cmsInfoDescription, cmsNoLanguage, cmsNoCountry, desc, n);
	if(n != nr)
          lcm2msg_p( 301, NULL, "found propblem reading desc tag: %d %d", n,nr);
      }
    }

    source_white = cmsReadTag( source_white_profile, cmsSigMediaWhitePointTag ); // MediaWhitePointTag
  }

  i_curve[0] = o_curve[0] = cmsBuildGamma(0, 1.0);
  if(!i_curve[0]) error = 1;
  for(i = 1; i < 3; ++i) { i_curve[i] = i_curve[0]; }

  if(!error)
  {
    cmsCIExyY xyWhitePoint;
    cmsFloat64Number TempK = kelvin;
    /* 4000 - 25000 K */
    cmsWhitePointFromTemp( &xyWhitePoint, TempK );
    cmsCIEXYZ WhitePoint;
    const cmsCIEXYZ * reference_white = cmsD50_XYZ();
    float max_brightness;

    cmsxyY2XYZ( &WhitePoint, &xyWhitePoint );
    cmsCIELab LabWhitePoint;
    cmsCIELab SrcLabWhitePoint;

    if(source_white)
      reference_white = source_white;

    cmsXYZ2Lab( reference_white, &LabWhitePoint, &WhitePoint );
    icc_ab[0] = LabWhitePoint.a/128.0;
    icc_ab[1] = LabWhitePoint.b/128.0;

#ifndef OY_HYP
    #define OY_SQRT(a,b)   ((a)*(a) + (b)*(b))
    #define OY_HYP(a,b)    pow(OY_SQRT(a,b),1.0/2.0)
#endif
    /* reduce brightness remaining inside a cone with a roof angle of 30° */
    max_brightness = 1.0 - OY_HYP(icc_ab[0],icc_ab[1]/1.5);

    cmsXYZ2Lab( cmsD50_XYZ(), &SrcLabWhitePoint, reference_white );
    cmsXYZ2Lab( cmsD50_XYZ(), &LabWhitePoint, &WhitePoint );
    lcm2msg_p( 302, NULL, "SrcW: %g %g %g LabW: %g %g %g  diff: %g %g  max brightness: %g",
            SrcLabWhitePoint.L, SrcLabWhitePoint.a, SrcLabWhitePoint.b,
            LabWhitePoint.L, LabWhitePoint.a, LabWhitePoint.b,
            icc_ab[0], icc_ab[1], max_brightness );

    /* avoid color clipping around the white point */
    curve_params_low[1] = max_brightness;
    o_curve[0] = cmsBuildParametricToneCurve(0, 6, curve_params_low);
    o_curve[1] = o_curve[2] = cmsBuildParametricToneCurve(0, 6, curve_params);
    if(!o_curve[0] || !o_curve[1]) error = 1;
  }

  if(error) goto lcm2CreateAbstractTemperatureProfileClean;

  if(icc_ab[1] > 0)
  {
    sprintf( kelvin_name, "Reddish %d K (www.oyranos.org)", (int)kelvin );
  } else if(icc_ab[1] == 0) {
    sprintf( kelvin_name, "%d K (www.oyranos.org)", (int)kelvin );
    kelvin_meta[1] = "neutral,white_point,atom";
    kelvin_meta[3] = "yes,D50,kelvin";
  } else {
    sprintf( kelvin_name, "Bluish %d K (www.oyranos.org)", (int)kelvin );
    kelvin_meta[1] = "bluish,white_point,atom";
    kelvin_meta[3] = "yes,bluish,kelvin";
  }

  if(source_white_profile)
  {
    if(desc && strlen(desc) < 900)
      sprintf( &kelvin_name[strlen(kelvin_name)], " - %s", desc);
    if(icc_ab[1] > 0)
    {
      kelvin_meta[1] = "reddish,white_point,atom,device";
      kelvin_meta[3] = "yes,reddish,kelvin";
    } else if(icc_ab[1] == 0) {
      kelvin_meta[1] = "neutral,white_point,atom,device";
      kelvin_meta[3] = "yes,D50,kelvin";
    } else {
      kelvin_meta[1] = "bluish,white_point,atom,device";
      kelvin_meta[3] = "yes,bluish,kelvin";
    }
  }

  if(!error)
    /* profile fragment creation */
    profile = lcm2CreateProfileFragment (
                             "*lab", // CIE*Lab
                             "*lab", // CIE*Lab
                             icc_profile_version,
                             kelvin_name,
                             "Oyranos project 2017",
                             "Kai-Uwe Behrmann",
                             ICC_2011_LICENSE,
                             "CIE*Lab",
                             "http://www.cie.co.at",
                             NULL);
  if(!profile) error = 1;

  if(!error)
    error = lcm2CreateProfileLutByFuncAndCurves( profile,
                                      lcm2SamplerWhitePointLab, icc_ab,
                                      o_curve, i_curve,
                                      "*lab", "*lab", "*lab",
                                      grid_size, cmsSigAToB0Tag );

  if(!error)
    lcm2AddMetaTexts ( profile, "EFFECT_,COLORIMETRY_,CMF_", kelvin_meta, cmsSigMetaTag );

lcm2CreateAbstractTemperatureProfileClean:
  if(i_curve[0]) cmsFreeToneCurve( i_curve[0] );
  if(o_curve[0]) cmsFreeToneCurve( o_curve[0] );
  if(o_curve[1]) cmsFreeToneCurve( o_curve[1] );

  *my_abstract_file_name = kelvin_name;
  if(h_profile)
    *h_profile = profile;
  else if(profile && *my_abstract_file_name)
  {
    char * fn = lcm2WriteProfileToFile( profile, *my_abstract_file_name, 0,0 );

    lcm2msg_p( 302, NULL, "wrote to: %s", fn?fn:"----");

    lcm2Free_m(fn);
    cmsCloseProfile( profile );
  }

  return error;
}

/** Function  lcm2CreateAbstractWhitePointProfileLab
 *  @brief    Create a effect profile of type abstract in ICC*Lab PCS for white point adjustment
 *
 *  These profiles can be applied to 1D / per single channel only adjustments.
 *  It will be marked with EFFECT_linear=yes in the meta tag.
 *
 *  @param[in]    cie_a                CIE*a correction value in -0.5 - 0.5 range
 *  @param[in]    cie_b                CIE*b correction value in -0.5 - 0.5 range
 *  @param[in]    grid_size            dimensions of the created LUT; e.g. 33
 *  @param[in]    icc_profile_version  2.3 or 4.3
 *  @param[out]   my_abstract_file_name                   profile file name
 *  @param[out]   h_profile            the resulting profile; If omitted the function will write the profile to my_abstract_file_name.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/28
 *  @since   2017/06/02 (Oyranos: 0.9.7)
 */
int          lcm2CreateAbstractWhitePointProfileLab (
                                       double              cie_a,
                                       double              cie_b,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       char             ** my_abstract_file_name,
                                       cmsHPROFILE       * h_profile
                                     )
{
  cmsHPROFILE profile = NULL;
  cmsToneCurve * i_curve[3] = {NULL,NULL,NULL}, * o_curve[3] = {NULL,NULL,NULL};
  /* type[6]  Y = (a * X + b) ^ Gamma + c  order: {g, a, b, c} */
  double curve_params[4] = {1,1,0,0}, curve_params_low[4] = {1,0.95,0,0};
  int i;

  const char * kelvin_meta[] = {
    "EFFECT_class", "reddish,white_point,linear,atom",
    "EFFECT_linear", "yes", /* can be used for 1D curves like VCGT */
    "EFFECT_type", "CIEab",
    "COLORIMETRY_white_point", "yes,reddish,kelvin",
    "CMF_binary", "create-abstract",
    "CMF_version", "0.9.7",
    "CMF_product", "Oyranos",
    0,0
  };
  char * kelvin_name = malloc(1024);
  int error = !kelvin_name;
  double icc_ab[2] = {cie_a, cie_b};

  if(error) return 1;

  i_curve[0] = cmsBuildGamma(0, 1.0);
  if(!i_curve[0]) error = 1;
  for(i = 1; i < 3; ++i)
  { i_curve[i] = i_curve[0]; }

  if(!error)
  {
#ifndef OY_HYP
    #define OY_SQRT(a,b)   ((a)*(a) + (b)*(b))
    #define OY_HYP(a,b)    pow(OY_SQRT(a,b),1.0/2.0)
#endif
    /* reduce brightness remaining inside a cone with a roof angle of 30° */
    double max_brightness = 1.0 - OY_HYP(icc_ab[0],icc_ab[1]/1.5);

    /* avoid color clipping around the white point */
    curve_params_low[1] = max_brightness;
    o_curve[0] = cmsBuildParametricToneCurve(0, 6, curve_params_low);
    o_curve[1] = o_curve[2] = cmsBuildParametricToneCurve(0, 6, curve_params);
    if(!o_curve[0] || !o_curve[1]) error = 1;
  }

  if(error) goto lcm2CreateAbstractWhitePointProfileClean;

  if(icc_ab[1] > 0)
  {
    sprintf( kelvin_name, "Reddish CIE*a %g CIE*b %g", cie_a, cie_b );
  } else if(-0.001 < icc_ab[1] && icc_ab[0] < 0.001) {
    sprintf( kelvin_name, "CIE*a %g CIE*b %g", cie_a, cie_b );
    kelvin_meta[1] = "neutral,white_point,atom";
    kelvin_meta[3] = "yes,D50,kelvin";
  } else {
    sprintf( kelvin_name, "Bluish CIE*a %g CIE*b %g", cie_a, cie_b );
    kelvin_meta[1] = "bluish,white_point,atom";
    kelvin_meta[3] = "yes,bluish,kelvin";
  }

  profile = lcm2CreateProfileFragment (
                             "*lab", // CIE*Lab
                             "*lab", // CIE*Lab
                             icc_profile_version,
                             kelvin_name,
                             "Oyranos project 2018",
                             "Kai-Uwe Behrmann",
                             ICC_2011_LICENSE,
                             "CIE*Lab",
                             "http://www.cie.co.at",
                             NULL);
  if(!profile) goto lcm2CreateAbstractWhitePointProfileClean;

  error = lcm2CreateProfileLutByFuncAndCurves( profile,
                                      lcm2SamplerWhitePointLab, icc_ab,
                                      o_curve, i_curve,
                                      "*lab", "*lab", "*lab",
                                      grid_size, cmsSigAToB0Tag );

  if(!error)
    lcm2AddMetaTexts ( profile, "EFFECT_,COLORIMETRY_,CMF_", kelvin_meta, cmsSigMetaTag );

lcm2CreateAbstractWhitePointProfileClean:
  if(i_curve[0]) cmsFreeToneCurve( i_curve[0] );
  if(o_curve[0]) cmsFreeToneCurve( o_curve[0] );
  if(o_curve[1]) cmsFreeToneCurve( o_curve[1] );

  *my_abstract_file_name = kelvin_name;
  if(h_profile)
    *h_profile = profile;
  else if(profile && *my_abstract_file_name)
  {
    char * fn = lcm2WriteProfileToFile( profile, *my_abstract_file_name, 0,0 );

    lcm2msg_p( 302, NULL, "wrote to: %s", fn?fn:"----");

    lcm2Free_m(fn);
    cmsCloseProfile( profile );
  }

  return error;
}

/** Function  lcm2CreateAbstractWhitePointProfileBradford
 *  @brief    Create a effect profile of type abstract in ICC*XYZ PCS for white point adjustment
 *
 *  These profiles can be applied to 1D / per single channel only adjustments.
 *  It will be marked with EFFECT_linear=yes in the meta tag.
 *
 *  @param[in]     src_iccXYZ          source media white point;
 *                                     The triple is stored in meta:COLORIMETRY_white_point_xyz_src.
 *  @param[in]     src_name            source media white point name or profile; optional
 *  @param[in]     illu_iccXYZ         ICC*XYZ illuminant in 0.0 - 2.0 range;
 *                                     The triple stored in meta:COLORIMETRY_white_point_xyz_dst.
 *  @param[in]     illu_name           target illuminant name; optional
 *  @param[in]     scale               scale factor to multiply the chromatice outgoing white point;
 *                                     optional, It is usually be set to the scale factor. Can be set to
 *                                     *scale == 0 for a generic scale factor, which might be good for a sRGB gamut.
 *  @param[in,out] icc_profile_version 4.3 is supported
 *  @param[in]     flags               - 0x01 : return only fast my_abstract_file_name, without expensive profile computation
 *  @param[out]    my_abstract_file_name                    profile file name
 *  @param[out]    h_profile           the resulting profile; If omitted the function will write the profile to my_abstract_file_name.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/03/17
 *  @since   2017/06/02 (Oyranos: 0.9.7)
 */
int          lcm2CreateAbstractWhitePointProfileBradford (
                                       double            * src_iccXYZ,
                                       const char        * src_name,
                                       double            * illu_iccXYZ,
                                       const char        * illu_name,
                                       double            * scale,
                                       double              icc_profile_version OY_UNUSED,
                                       int                 flags,
                                       char             ** my_abstract_file_name,
                                       cmsHPROFILE       * h_profile
                                     )
{
  cmsHPROFILE profile = NULL;
  cmsToneCurve * m_curves[3] = {NULL,NULL,NULL}, * b_curves[3] = {NULL,NULL,NULL};
  int i;

  char white_point_xyz_src[64] = {0},
       white_point_xyz_dst[64] = {0},
       brightness_scale[16] = {0};
  const char * kelvin_meta[] = {
    "EFFECT_class", "reddish,type,white_point,linear,atom",
    "EFFECT_linear", "yes", /* can be used for 1D curves like VCGT */
    "COLORIMETRY_white_point", "yes,reddish,kelvin",
    "COLORIMETRY_white_point_icc_xyz_src", white_point_xyz_src,
    "COLORIMETRY_white_point_icc_xyz_dst", white_point_xyz_dst,
    "COLORIMETRY_white_point_scale", brightness_scale,
    "EFFECT_type", "bradford",
    "CMF_binary", "create-abstract",
    "CMF_version", "0.9.7",
    "CMF_product", "Oyranos",
    0,0,
    0,0,
    0,0
  };
  int len = (src_name ? strlen(src_name) : 0) + (illu_name ? strlen(illu_name) : 0) + 1024;
  char * kelvin_name = malloc(len);
  int error = !kelvin_name;

  cmsCIEXYZ SourceWhitePt = { src_iccXYZ[0],  src_iccXYZ[1],  src_iccXYZ[2]},
            Illuminant    = {illu_iccXYZ[0], illu_iccXYZ[1], illu_iccXYZ[2]};
  double icc_ab[2] = {0,0};
  double matrix[3][3] = {{1.0, 0.0, 0.0},
                         {0.0, 1.0, 0.0},
                         {0.0, 0.0, 1.0}};
  double max_brightness, b_scale;
#ifdef HAVE_LOCALE_H
  char * old_loc;
#endif

  if(error) return 1;

#ifdef HAVE_LOCALE_H
  old_loc = strdup(setlocale(LC_ALL,NULL));
#endif

  if(scale)
    b_scale = *scale;
  else
    b_scale = 0.0;

  if(!(flags & 0x01)) /* skip computation */
  {
    m_curves[0] = cmsBuildGamma(0, 1.0);
    if(!m_curves[0]) error = 1;
    for(i = 1; i < 3; ++i)
    { m_curves[i] = m_curves[0]; }

    b_curves[0] = cmsBuildGamma(0, 1.0);
    if(!b_curves[0]) error = 1;
    for(i = 1; i < 3; ++i)
    { b_curves[i] = b_curves[0]; }
  }


  if(!error)
  {
#ifndef OY_HYP
    #define OY_SQRT(a,b)   ((a)*(a) + (b)*(b))
    #define OY_HYP(a,b)    pow(OY_SQRT(a,b),1.0/2.0)
#endif
#ifndef OY_MAX
    #define OY_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif
    /* reduce brightness remaining inside a cone with a roof angle of 30° */
    double src_Lab[3], dst_Lab[3];

    lcm2iccXYZ2iccLab(  src_iccXYZ, src_Lab );
    lcm2iccXYZ2iccLab( illu_iccXYZ, dst_Lab );
    icc_ab[0] = dst_Lab[1] - src_Lab[1];
    icc_ab[1] = dst_Lab[2] - src_Lab[2];
    max_brightness = OY_HYP(icc_ab[0],icc_ab[1]/1.2);
    /* avoid color clipping around the white point */
    if(b_scale == 0.0)
      b_scale = OY_MAX( 1.0 - max_brightness, 0.2 );

#ifdef HAVE_LOCALE_H
    setlocale(LC_ALL,"C");
#endif
    sprintf( white_point_xyz_src, "%f,%f,%f", src_iccXYZ[0], src_iccXYZ[1], src_iccXYZ[2] );
    sprintf( white_point_xyz_dst, "%f,%f,%f", illu_iccXYZ[0], illu_iccXYZ[1], illu_iccXYZ[2] );
    sprintf( brightness_scale, "%f", b_scale );
#ifdef HAVE_LOCALE_H
    setlocale(LC_ALL,old_loc);
#endif
    /* avoid color clipping around the white point, with PCS*XYZ scaling */
    if(scale && *scale != 0.0)
      b_scale = OY_MAX( 1.0 - max_brightness * LCM2_ADAPT_TO_PCS_XYZ, 0.2 );
    if(scale)
      *scale = b_scale;
  }

  if(error) goto lcm2CreateAbstractWhitePointProfileBClean;

#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL,"C");
#endif
  if(icc_ab[1] > 0)
  {
    sprintf( kelvin_name, "Bradford Reddish CIE*a %g CIE*b %g", icc_ab[0], icc_ab[1] );
  } else if(-0.001 < icc_ab[1] && icc_ab[0] < 0.001) {
    sprintf( kelvin_name, "Bradford CIE*a %g CIE*b %g", icc_ab[0], icc_ab[1] );
    kelvin_meta[1] = "neutral,type,white_point,atom";
    kelvin_meta[3] = "yes,D50,kelvin";
  } else {
    sprintf( kelvin_name, "Bradford Bluish CIE*a %g CIE*b %g", icc_ab[0], icc_ab[1] );
    kelvin_meta[1] = "bluish,type,white_point,atom";
    kelvin_meta[3] = "yes,bluish,kelvin";
  }
#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL,old_loc);
#endif
  if(src_name)
    sprintf( &kelvin_name[strlen(kelvin_name)], " src:%s", src_name );
  if(illu_name)
    sprintf( &kelvin_name[strlen(kelvin_name)], " illu:%s", illu_name );
  sprintf( &kelvin_name[strlen(kelvin_name)], " v2 lcm2" );

  *my_abstract_file_name = kelvin_name;
  if(flags & 0x01) /* skip computation */
  {
#ifdef HAVE_LOCALE_H
    if(old_loc) free(old_loc);
#endif
    return error;
  }

  profile = lcm2CreateProfileFragment (
                             "*xyz", // CIE*XYZ
                             "*xyz", // CIE*XYZ
                             4.3, //icc_profile_version,
                             kelvin_name,
                             "Oyranos project 2019",
                             "Kai-Uwe Behrmann",
                             ICC_2011_LICENSE,
                             "Bradford",
                             "http://www.cie.co.at",
                             NULL);
  kelvin_name = NULL;
  if(!profile) goto lcm2CreateAbstractWhitePointProfileBClean;

  lcm2MAT3 Bradford, BB, Brightness = {{ {{b_scale,0,0}}, {{0,b_scale,0}}, {{0,0,b_scale}} }};
  if( !lcm2AdaptationMatrix(&Bradford, NULL, &SourceWhitePt, &Illuminant) ) goto lcm2CreateAbstractWhitePointProfileBClean;

  /** Scale the adaption matrix to avoid clipping. Scale factor is stored in meta:COLORIMETRY_white_point_scale. */
  lcm2MAT3per(&BB, &Bradford, &Brightness);
  matrix[0][0] = BB.v[0].n[0]; matrix[0][1] = BB.v[0].n[1]; matrix[0][2] = BB.v[0].n[2];
  matrix[1][0] = BB.v[1].n[0]; matrix[1][1] = BB.v[1].n[1]; matrix[1][2] = BB.v[1].n[2];
  matrix[2][0] = BB.v[2].n[0]; matrix[2][1] = BB.v[2].n[1]; matrix[2][2] = BB.v[2].n[2];

  error = lcm2CreateProfileLutByMatrixAndCurves( profile,
                                      m_curves, &matrix[0][0], b_curves,
                                      "*xyz", "*xyz",
                                      cmsSigAToB0Tag );

  if(!error)
  {
    int pos = 2*10;
    if(src_name)
    {
      kelvin_meta[pos++] = "COLORIMETRY_white_point_name_src";
      kelvin_meta[pos++] = src_name;
    }
    if(illu_name)
    {
      kelvin_meta[pos++] = "COLORIMETRY_white_point_name_dst";
      kelvin_meta[pos++] = illu_name;
    }
    lcm2AddMetaTexts ( profile, "EFFECT_,COLORIMETRY_,CMF_", kelvin_meta, cmsSigMetaTag );
  }

lcm2CreateAbstractWhitePointProfileBClean:
  if(m_curves[0]) cmsFreeToneCurve( m_curves[0] );
  if(b_curves[0]) cmsFreeToneCurve( b_curves[0] );

  if(h_profile)
    *h_profile = profile;
  else if(profile && *my_abstract_file_name)
  {
    char * fn = lcm2WriteProfileToFile( profile, *my_abstract_file_name, 0,0 );

    lcm2msg_p( 302, NULL, "wrote to: %s", fn?fn:"----");

    lcm2Free_m(fn);
    cmsCloseProfile( profile );
  }
  lcm2Free_m(kelvin_name);
#ifdef HAVE_LOCALE_H
  if(old_loc) free(old_loc);
#endif

  return error;
}

/** Function  lcm2CreateCalibrationProfileM
 *  @brief    Create a calibration profile of type device link in rgb or cmyk PCS
 *
 *  Possible computation emlements are m_curves.
 *
 *  @param[in]    m_curve              optional input curve for all CIE*XYZ;
 *                                     channels in range 0.0 ... PCS*XYZ_MAX
 *  @param[in]    csp                  color space; currently only "*srgb"
 *  @param[in]    icc_profile_version  4.3
 *  @param[in]    my_calibration_description              internal profile name
 *  @param[in]    my_calibration_descriptions             internal profile name translated
 *  @param[in]    my_calibration_file_name                profile file name. If present a ICC profile will be written to that name. optional
 *  @param[in]    provider             e.g. "My Project 2019"
 *  @param[in]    vendor               e.g. "My Name"
 *  @param[in]    my_license           e.g. "This profile is made available by %s, with permission of %s, and may be copied, distributed, embedded, made, used, and sold without restriction. Altered versions of this profile shall have the original identification and copyright information removed and shall not be misrepresented as the original profile."
 *                                     - first %%s is replaced by the provider string arg and
 *                                     - second %%s is replaced by the vendor string arg
 *  @param[in]    device_model         e.g. "My Set"
 *  @param[in]    device_manufacturer  e.g. "www.mydomain.net"
 *  @param[in]    my_meta_data         e.g. {"DOMAIN_,GROUP_","DOMAIN_key1","value1","GROUP_key2","value2"}
 *  @param[out]   h_profile            the resulting profile
 *
 *  @version Oyranos: 0.9.7
 *  @date    2024/02/18
 *  @since   2019/03/03 (Oyranos: 0.9.7)
 */
int          lcm2CreateCalibrationProfileM (
                                       cmsToneCurve     ** m_curves,
                                       const char        * csp,
                                       double              icc_profile_version,
                                       const char        * my_calibration_description,
                                       const char       ** my_calibration_descriptions,
                                       const char        * my_calibration_file_name,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       const char       ** my_meta_data,
                                       cmsHPROFILE       * h_profile
                                     )
{
  cmsHPROFILE profile = 0;
  int error = !m_curves;

  if(error)
  {
    lcm2msg_p( 301, NULL, "%s(%s:%d) no m_curves arg: %s", __func__, strrchr(__FILE__,'/')+1,__LINE__, csp?csp:"----");
    goto lcm2CreateCalibrationProfileMClean;
  }

  profile = lcm2CreateProfileFragment (
                             csp, csp,
                             icc_profile_version,
                             my_calibration_description,
                             provider, vendor, my_license, 
                             device_model, device_manufacturer, NULL);
  if(!profile) goto lcm2CreateCalibrationProfileMClean;

  if(my_meta_data)
    lcm2AddMetaTexts ( profile, my_meta_data[0], &my_meta_data[1], cmsSigMetaTag );

  {
    cmsPipeline * pl = cmsPipelineAlloc( 0,3,3 );
 
    cmsPipelineInsertStage( pl, cmsAT_BEGIN,
                            cmsStageAllocToneCurves( 0, 3, m_curves ) );
    cmsWriteTag( profile, cmsSigAToB0Tag, pl );

    if(pl) cmsPipelineFree( pl );
  }
  if(error) goto lcm2CreateCalibrationProfileMClean;


  lcm2AddMluDescription ( profile, my_calibration_descriptions,
                          cmsSigProfileDescriptionMLTag
                        );

  if(my_calibration_file_name)
  {
    char * fn = lcm2WriteProfileToFile( profile, my_calibration_file_name, 0,0 );
    lcm2msg_p( 302, NULL, "wrote to: %s", fn?fn:"----");
    lcm2Free_m(fn);
  }

  if(h_profile)
    *h_profile = profile;
  else
    cmsCloseProfile( profile );

lcm2CreateCalibrationProfileMClean:

  return error;
}


/** Function  lcm2CreateProfileFragment
 *  @brief    Create a color profile starter
 *
 *  In case both the in_space_profile and out_space_profile arguments are set
 *  to "*lab", the profile will be set to class abstract. In case the 
 *  in_space_profile is not "*lab" and the later one is different, a color
 *  profile of class input will be generated. With in_space_profile not "*lab"
 *  and out_space_profile "*lab" a color profile of class output will be
 *  generated. Note such profiles have initially no backward LUT and can not
 *  be used for inverse color transforms, which might be a problem for general
 *  purpose ICC profiles. But you can add more tables if needed by passing in a
 *  previously created profile.
 *
 *  All profiles generated by this function are meant to be filled with
 *  colorimetric data by e.g. lcm2CreateProfileLutByFunc() or 
 *  lcm2CreateICCMatrixProfile2().
 *
 *  Here a code example:
 *  @code
    cmsHPROFILE profile = lcm2CreateProfileFragment (
                             "*srgb", // sRGB
                             "*lab", // CIE*Lab
                             2.3,
                             "MySpace (MyProject)",
                             "My Project 2016",
                             "My Name",
                             ICC_2011_LICENSE,
                             "My Box", "www.mydomain.net", NULL
                           );
    @endcode
 *
 *  @param[in]     in_space_profile    input color space; for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     out_space_profile   output color space; for wildcards see
 *                                     lcm2OpenProfileFile()
 *  @param[in]     icc_profile_version 2.3 or 4.3
 *  @param[in]     my_abstract_description                 internal profile name
 *  @param[in]     provider            e.g. "My Project 2016"
 *  @param[in]     vendor              e.g. "My Name"
 *  @param[in]     my_license          e.g. "This profile is made available by %s, with permission of %s, and may be copied, distributed, embedded, made, used, and sold without restriction. Altered versions of this profile shall have the original identification and copyright information removed and shall not be misrepresented as the original profile."
 *                                     first %s is provider string arg and
 *                                     second %s is filled by vendor string arg
 *  @param[in]    device_model         e.g. "My Set"
 *  @param[in]    device_manufacturer  e.g. "www.mydomain.net"; hint:
 *                                     lcms <= 2.08 writes a malformed desc tag
 *  @param[in,out] h_profile           use existing profile; optional
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/06
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 */
cmsHPROFILE  lcm2CreateProfileFragment(
                                       const char        * in_space_profile,
                                       const char        * out_space_profile,
                                       double              icc_profile_version,
                                       const char        * my_abstract_description,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       cmsHPROFILE         h_profile
                                     )
{
  cmsHPROFILE h_in_space = 0,
              h_out_space = 0;
  cmsColorSpaceSignature csp_in, csp_out;
  cmsProfileClassSignature profile_class = cmsSigAbstractClass;
  cmsMLU * mlu[4] = {0,0,0,0};
  int i;
  char * license = NULL;

  if(!h_profile)
  { h_profile = cmsCreateProfilePlaceholder( 0 ); } if(!h_profile) goto lcm2CreateProfileFragmentClean;

  if(in_space_profile)
  {
    h_in_space  = lcm2OpenProfileFile( in_space_profile, NULL );
    if(h_in_space)
      csp_in = cmsGetColorSpace( h_in_space );
    else if(strcmp("rgb", in_space_profile) == 0)
      csp_in = cmsSigRgbData;
    else if(strcmp("cmyk", in_space_profile) == 0)
      csp_in = cmsSigCmykData;
  }
  if(out_space_profile)
  {
    h_out_space = lcm2OpenProfileFile( out_space_profile, NULL );
    if(h_out_space)
      csp_out = cmsGetColorSpace( h_out_space );
    else if(strcmp("rgb", out_space_profile) == 0)
      csp_out = cmsSigRgbData;
    else if(strcmp("cmyk", out_space_profile) == 0)
      csp_out = cmsSigCmykData;
  }


  cmsSetProfileVersion( h_profile, icc_profile_version );

#define CSP_IS_PCS(csp) (csp == cmsSigLabData || csp == cmsSigXYZData)
  if( CSP_IS_PCS(csp_in) && CSP_IS_PCS(csp_out) )
    profile_class = cmsSigAbstractClass;
  else if( CSP_IS_PCS(csp_out) )
    profile_class = cmsSigInputClass;
  else if( CSP_IS_PCS(csp_in) )
    profile_class = cmsSigOutputClass;
  else 
    profile_class = cmsSigLinkClass;

  cmsSetDeviceClass( h_profile, profile_class );
  cmsSetColorSpace( h_profile, csp_in );
  cmsSetPCS( h_profile, csp_out );
  for(i = 0; i < 4; ++i)
    mlu[i] = cmsMLUalloc(0,1);
  if(!(mlu[0] && mlu[1] && mlu[2] && mlu[3]))
    return h_profile;
  cmsMLUsetASCII(mlu[0], "EN", "us", my_abstract_description);
  cmsWriteTag( h_profile, cmsSigProfileDescriptionTag, mlu[0] );

  if(device_model)
  {
    cmsMLUsetASCII(mlu[1], "EN", "us", device_model);
    cmsWriteTag( h_profile, cmsSigDeviceModelDescTag, mlu[1]);
  }

  if(device_manufacturer)
  {
    cmsMLUsetASCII(mlu[2], "EN", "us", device_manufacturer);
    cmsWriteTag( h_profile, cmsSigDeviceMfgDescTag, mlu[2]);
  }

  license = (char *) malloc( strlen(my_license) + strlen(provider) + strlen(vendor) + 1 );
  if(!license) goto lcm2CreateProfileFragmentClean;;
  sprintf( license, my_license, provider, vendor );
  cmsMLUsetASCII(mlu[3], "EN", "us", license);
  cmsWriteTag( h_profile, cmsSigCopyrightTag, mlu[3]);

  cmsWriteTag( h_profile, cmsSigMediaWhitePointTag, cmsD50_XYZ() );

lcm2CreateProfileFragmentClean:
  if(h_in_space) { cmsCloseProfile( h_in_space ); } h_in_space = 0;
  if(h_out_space) { cmsCloseProfile( h_out_space ); } h_out_space = 0;
  for(i = 0; i < 4; ++i)
    cmsMLUfree( mlu[i] );
  lcm2Free_m(license);

  return h_profile;
}

int isBigEndian ()
{ union { unsigned short u16; unsigned char c; } test = { .u16 = 1 }; return !test.c; }

/* UTF-8 to WCHAR_T conversion */
typedef uint32_t	UTF32;	/* at least 32 bits */
typedef uint16_t	UTF16;	/* at least 16 bits */
typedef uint8_t		UTF8;	/* typically 8 bits */
typedef unsigned char	Boolean; /* 0 or 1 */

/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_BMP (UTF32)0x0000FFFF
#define UNI_MAX_UTF16 (UTF32)0x0010FFFF
#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF

typedef enum {
  conversionOK, 		/* conversion successful */
  sourceExhausted,	/* partial character in source, but hit end */
  targetExhausted,	/* insuff. room in target for conversion */
  sourceIllegal		/* source sequence is illegal/malformed */
} lcm2UtfConversionResult;
typedef enum {
  strictConversion = 0,
  lenientConversion
} lcm2UtfConversionFlags;

static const int halfShift  = 10; /* used for shifting by 10 bits */

static const UTF32 halfBase = 0x0010000UL;
static const UTF32 halfMask = 0x3FFUL;

#define UNI_SUR_HIGH_START  (UTF32)0xD800
#define UNI_SUR_HIGH_END    (UTF32)0xDBFF
#define UNI_SUR_LOW_START   (UTF32)0xDC00
#define UNI_SUR_LOW_END     (UTF32)0xDFFF
#define false	   0
#define true	    1

/*
* Index into the table below with the first byte of a UTF-8 sequence to
* get the number of trailing bytes that are supposed to follow it.
* Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
* left as-is for anyone who may want to do such conversion, which was
* allowed in earlier algorithms.
*/
static const char trailingBytesForUTF8[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
* Magic values subtracted from a buffer value during UTF8 conversion.
* This table contains as many values as there might be trailing bytes
* in a UTF-8 sequence.
*/
static const UTF32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
0x03C82080UL, 0xFA082080UL, 0x82082080UL };

/*
* Utility routine to tell whether a sequence of bytes is legal UTF-8.
* This must be called with the length pre-determined by the first byte.
* If not calling this from ConvertUTF8to*, then the length can be set by:
*  length = trailingBytesForUTF8[*source]+1;
* and the sequence is illegal right away if there aren't that many bytes
* available.
* If presented with a length > 4, this returns false.  The Unicode
* definition of UTF-8 goes up to 4-byte sequences.
*/

static Boolean isLegalUTF8(const UTF8 *source, int length)
{
  UTF8 a;
  const UTF8 *srcptr = source+length;
  switch (length) {
    default: return false;
      /* Everything else falls through when "true"... */
    case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false; OY_FALLTHROUGH
    case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false; OY_FALLTHROUGH
    case 2: if ((a = (*--srcptr)) > 0xBF) return false;

      switch (*source) {
            /* no fall-through in this inner switch */
        case 0xE0: if (a < 0xA0) return false; break;
        case 0xED: if (a > 0x9F) return false; break;
        case 0xF0: if (a < 0x90) return false; break;
        case 0xF4: if (a > 0x8F) return false; break;
        default:   if (a < 0x80) return false; break;
      } OY_FALLTHROUGH

    case 1: if (*source >= 0x80 && *source < 0xC2) return false;
  }
  if (*source > 0xF4) return false;
  return true;
}

lcm2UtfConversionResult lcm2ConvertUTF8toUTF16 (const UTF8** sourceStart, const UTF8* sourceEnd, 
                                       UTF16** targetStart, UTF16* targetEnd, lcm2UtfConversionFlags flags)
{
  lcm2UtfConversionResult result = conversionOK;
  const UTF8* source = *sourceStart;
  UTF16* target = *targetStart;
  while (source < sourceEnd) {
    UTF32 ch = 0;
    unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
    if (source + extraBytesToRead >= sourceEnd) {
      result = sourceExhausted; break;
    }
    /* Do this check whether lenient or strict */
    if (! isLegalUTF8(source, extraBytesToRead+1)) {
      result = sourceIllegal;
      break;
    }
    /*
    * The cases all fall through. See "Note A" below.
    */
    switch (extraBytesToRead) {
      case 5: ch += *source++; ch <<= 6; OY_FALLTHROUGH/* remember, illegal UTF-8 */
      case 4: ch += *source++; ch <<= 6; OY_FALLTHROUGH /* remember, illegal UTF-8 */
      case 3: ch += *source++; ch <<= 6; OY_FALLTHROUGH
      case 2: ch += *source++; ch <<= 6; OY_FALLTHROUGH
      case 1: ch += *source++; ch <<= 6; OY_FALLTHROUGH
      case 0: ch += *source++;
    }
    ch -= offsetsFromUTF8[extraBytesToRead];

    if (target >= targetEnd) {
      source -= (extraBytesToRead+1); /* Back up source pointer! */
      result = targetExhausted; break;
    }
    if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
      /* UTF-16 surrogate values are illegal in UTF-32 */
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
        if (flags == strictConversion) {
          source -= (extraBytesToRead+1); /* return to the illegal value itself */
          result = sourceIllegal;
          break;
        } else {
          *target++ = UNI_REPLACEMENT_CHAR;
        }
      } else {
        *target++ = (UTF16)ch; /* normal case */
      }
    } else if (ch > UNI_MAX_UTF16) {
      if (flags == strictConversion) {
        result = sourceIllegal;
        source -= (extraBytesToRead+1); /* return to the start */
        break; /* Bail out; shouldn't continue */
      } else {
        *target++ = UNI_REPLACEMENT_CHAR;
      }
    } else {
      /* target is a character in range 0xFFFF - 0x10FFFF. */
      if (target + 1 >= targetEnd) {
        source -= (extraBytesToRead+1); /* Back up source pointer! */
        result = targetExhausted; break;
      }
      ch -= halfBase;
      *target++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
      *target++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
    }
  }
  *sourceStart = source;
  *targetStart = target;
  return result;
}

wchar_t *    lcm2Utf8ToWchar         ( const char        * text )
{
  wchar_t * wchar_out, * tmp_out;
  char * in, * tmp_in;
  size_t in_len  = strlen(text),
         out_len = in_len*sizeof(wchar_t)+sizeof(wchar_t);
  lcm2UtfConversionResult error;

  if(!in_len) return 0;
  else ++in_len;

  tmp_out = wchar_out = calloc( in_len+1, sizeof(wchar_t) );
  in = tmp_in = strdup( text );
  error = lcm2ConvertUTF8toUTF16( (const UTF8**)&in, (const UTF8*)in+in_len, (UTF16**)&tmp_out, (UTF16*)(tmp_out+out_len), lenientConversion );

  if(error == conversionOK)
  {
    /* store UTF16BE in wchar_t for lcms2 */
    uint16_t * icc_utf16 = (uint16_t*) wchar_out;
    int i;
    for(i = in_len; i >= 0; --i) wchar_out[i] = icc_utf16[i];
  }
  else
  {
    lcm2msg_p( 300, NULL, "error[%d] %lu %lu %s", error, in_len, out_len, text );
    lcm2Free_m(wchar_out);
  }

  lcm2Free_m( tmp_in );

  return wchar_out;
}

/** Function  lcm2AddMluDescription
 *  @brief    Add translated texts to a profile
 *
 *  Iterates over the provided string list converts from "UTF-8" input
 *  to "WCHAR_T" for lcms and 
 *  does byteswapping on little endian machines.
 *
 *  Here a code example:
 *  @code
    const char * texts[] = {
      "de", "DE", "Mein Text",
      "en", "US", "My Text"
    };
    lcm2AddMluDescription ( profile, texts,
                            cmsSigProfileDescriptionMLTag
                          );
    @endcode
 *
 *  @param[in,out] profile             color profile
 *  @param[in]     texts               language + country + text list
 *  @param[in]     tag_sig             signature
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/13
 *  @since   2016/03/13 (Oyranos: 0.9.6)
 */
void         lcm2AddMluDescription   ( cmsHPROFILE         profile,
                                       const char        * texts[],
                                       cmsTagSignature     tag_sig )
{
  int n = 0, i;
  cmsMLU * mlu = NULL;

  if(texts)
    while( texts[n] ) ++n;

  if(!n) return;

  mlu = cmsMLUalloc( 0, n/3 + 1 );
  if(!mlu) return;

  for( i = 0; i < n; i += 3 )
  {
    char lang[4] = {0,0,0,0}, country[4] = {0,0,0,0};
    const char * text = texts[i+2];
    wchar_t * wchar_out;

    wchar_out = lcm2Utf8ToWchar( text );

    if(!wchar_out) continue;

    /* the language code is stored as readable 4 byte string */
    lang[0] = texts[i+0][0]; lang[1] = texts[i+0][1];
    country[0] = texts[i+1][0]; country[1] = texts[i+1][1];

    cmsMLUsetWide( mlu, lang, country, wchar_out );

    lcm2Free_m( wchar_out );
  }

  cmsWriteTag( profile, tag_sig, mlu );

  cmsMLUfree( mlu );
}

/** Function  lcm2AddMetaTexts
 *  @brief    Add meta data to a profile
 *
 *  Iterates over the provided string list converts from "UTF-8" input
 *  to "WCHAR_T" for lcms and 
 *  does byteswapping on little endian machines.
 *
 *  Here a code example:
 *  @code
    const char * texts[] = {
      "GROUP_key1", "value1",
      "DOMAIN_key2", "value2"
    };
    lcm2AddMetaTexts ( profile, "GROUP_,DOMAIN_", texts,
                       cmsSigMetaTag
                     );
    @endcode
 *
 *  A prefix allows for grouping of keys like "EDID_" or "EXIF_". 
 *  The prefix part might be cut off in some cases to access an other level
 *  of keys. Think of "EDID_model" for monitors and "EXIF_model" for cameras,
 *  which both represent the key "model" concept.
 *
 *  @param[in,out] profile             color profile
 *  @param[in]     prefixes            The used uppercase prefix list.
 *  @param[in]     key_value           key + value list
 *  @param[in]     tag_sig             signature
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/02/11
 *  @since   2017/02/11 (Oyranos: 0.9.7)
 */
void         lcm2AddMetaTexts        ( cmsHPROFILE         profile,
                                       const char        * prefixes,
                                       const char        * key_value[],
                                       cmsTagSignature     tag_sig )
{
  int n = 0, i;
  cmsHANDLE dict = NULL;
  cmsContext contextID = cmsCreateContext( NULL,NULL );
  wchar_t * wchar_key = NULL, * wchar_val = NULL;

  if(key_value)
    while( key_value[n] ) ++n;

  if(n)
    dict = cmsDictAlloc( contextID );
  else
    lcm2msg_p( 300, NULL, "nothing to write %s", __func__ );
  if(contextID)
    cmsDeleteContext( contextID );

  if(!dict)
    return;

  if(prefixes)
  {
    wchar_key = lcm2Utf8ToWchar( "prefix" );
    wchar_val = lcm2Utf8ToWchar( prefixes );
  }
  if(wchar_key && wchar_val)
    cmsDictAddEntry( dict, wchar_key, wchar_val, NULL,NULL );

  lcm2Free_m( wchar_key );
  lcm2Free_m( wchar_val );

  for( i = 0; i < n; i += 2 )
  {
    const char * key = key_value[i+0],
               * val = key_value[i+1];

    wchar_key = lcm2Utf8ToWchar(key),
    wchar_val = lcm2Utf8ToWchar(val);

    if(!wchar_key || !wchar_val)
    {
      lcm2Free_m( wchar_key );
      lcm2Free_m( wchar_val );
      continue;
    }

    cmsDictAddEntry( dict, wchar_key, wchar_val, NULL,NULL );

    lcm2Free_m( wchar_key );
    lcm2Free_m( wchar_val );
  }

  cmsWriteTag( profile, tag_sig, dict );

  cmsDictFree( dict );
}

/** Function lcm2CreateICCMatrixProfile2
 *  @brief   Create a profile from primaries, white point and one gamma value
 *
 *  Used for ICC from EDID, Camera RAW etc. Marti calls these matrix/shaper.
 * @code
      // create linear space with REC.709/sRGB primaries and D65 white point
      cmsHPROFILE h_my_space = lcm2CreateICCMatrixProfile2( 1.0, 0.64,0.33, 0.30,0.60, 0.15,0.06, 0.3127,0.329 );
   @endcode
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/04
 *  @since   2009/10/24 (Oyranos: 0.1.10)
 */
cmsHPROFILE  lcm2CreateICCMatrixProfile2 (
                                       float             gamma,
                                       float rx, float ry,
                                       float gx, float gy,
                                       float bx, float by,
                                       float wx, float wy )
{
  cmsCIExyYTRIPLE p;
  cmsToneCurve * g[3] = {0,0,0};
  /* 0.31271, 0.32902 D65 */
  cmsCIExyY wtpt_xyY;
  cmsHPROFILE lp = 0;


  p.Red.x = rx;
  p.Red.y = ry;
  p.Red.Y = 1.0;
  p.Green.x = gx;
  p.Green.y = gy;
  p.Green.Y = 1.0;
  p.Blue.x = bx;
  p.Blue.y = by;
  p.Blue.Y = 1.0;
  wtpt_xyY.x = wx;
  wtpt_xyY.y = wy;
  wtpt_xyY.Y = 1.0;
  g[0] = g[1] = g[2] = cmsBuildGamma(0, (double)gamma);
  if(!g[0]) return NULL;
  lp = cmsCreateRGBProfile( &wtpt_xyY, &p, g);

  cmsFreeToneCurve( g[0] );

  return lp;
}

/** Function lcm2MessageFunc
 *  @brief   default message function to console
 *
 *  The default message function is used as a message printer to the console 
 *  from library start.
 *
 *  @param         code                a message code understood be your message
 *                                     handler or openiccMSG_e
 *  @param         context_object      a openicc object is expected
 *  @param         format              the text format string for following args
 *  @param         ...                 the variable args fitting to format
 *  @return                            0 - success; 1 - error
 *
 *  @version OpenICC: 0.1.0
 *  @date    2009/07/20
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int  lcm2MessageFunc                 ( int/*openiccMSG_e*/ code OY_UNUSED,
                                       const void        * context_object OY_UNUSED,
                                       const char        * format,
                                       ... )
{
  char * text = 0;
  int error = 0;
  va_list list;
  size_t sz = 0;
  int len = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  {
    text = calloc( sizeof(char), len+2 );
    if(!text)
    {
      fprintf(stderr, "Could not allocate 256 byte of memory.\n");
      return 1;
    }
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  if(text)
    fprintf( stderr, "%s\n", text );

  lcm2Free_m( text );

  return error;
}

lcm2Message_f      lcm2msg_p = lcm2MessageFunc;
/**  @brief    set a custom message function
 *
 *   Use to connect to user message system.
 */
int            lcm2MessageFuncSet    ( lcm2Message_f       message_func )
{
  if(message_func)
    lcm2msg_p = message_func;
  else
    lcm2msg_p = lcm2MessageFunc;

  return 1;
}

/**  @brief    run time API version
 */
int            lcm2Version           ( )
{
  return LCM2PROFILER_API;
}

// Compute chromatic adaptation matrix using Chad as cone matrix

static cmsBool ComputeChromaticAdaptation(lcm2MAT3* Conversion,
                                const cmsCIEXYZ* SourceWhitePoint,
                                const cmsCIEXYZ* DestWhitePoint,
                                const lcm2MAT3* Chad)

{

    lcm2MAT3 Chad_Inv;
    lcm2VEC3 ConeSourceXYZ, ConeSourceRGB;
    lcm2VEC3 ConeDestXYZ, ConeDestRGB;
    lcm2MAT3 Cone, Tmp;


    Tmp = *Chad;
    if (!lcm2MAT3inverse(&Tmp, &Chad_Inv)) return FALSE;

    lcm2VEC3init(&ConeSourceXYZ, SourceWhitePoint -> X,
                             SourceWhitePoint -> Y,
                             SourceWhitePoint -> Z);

    lcm2VEC3init(&ConeDestXYZ,   DestWhitePoint -> X,
                             DestWhitePoint -> Y,
                             DestWhitePoint -> Z);

    lcm2MAT3eval(&ConeSourceRGB, Chad, &ConeSourceXYZ);
    lcm2MAT3eval(&ConeDestRGB,   Chad, &ConeDestXYZ);

    // Build matrix
    lcm2VEC3init(&Cone.v[0], ConeDestRGB.n[0]/ConeSourceRGB.n[0],    0.0,  0.0);
    lcm2VEC3init(&Cone.v[1], 0.0,   ConeDestRGB.n[1]/ConeSourceRGB.n[1],   0.0);
    lcm2VEC3init(&Cone.v[2], 0.0,   0.0,   ConeDestRGB.n[2]/ConeSourceRGB.n[2]);


    // Normalize
    lcm2MAT3per(&Tmp, &Cone, Chad);
    lcm2MAT3per(Conversion, &Chad_Inv, &Tmp);

    return TRUE;
}
/** Returns the final chrmatic adaptation from illuminant FromIll to Illuminant ToIll.
 *  The cone matrix can be specified in ConeMatrix. If NULL, Bradford is assumed
 */
cmsBool  lcm2AdaptationMatrix(lcm2MAT3* r, const lcm2MAT3* ConeMatrix, const cmsCIEXYZ* FromIll, const cmsCIEXYZ* ToIll)
{
    lcm2MAT3 LamRigg   = {{ // Bradford matrix
        {{  0.8951,  0.2664, -0.1614 }},
        {{ -0.7502,  1.7135,  0.0367 }},
        {{  0.0389, -0.0685,  1.0296 }}
    }};

    if (ConeMatrix == NULL)
        ConeMatrix = &LamRigg;

    return ComputeChromaticAdaptation(r, FromIll, ToIll, ConeMatrix);
}

/* The lcm2VEC3, lcm2MAT3, lcm2MAT3inverse, lcm2VEC3init and lcm2MAT3per definitions 
 * origin from lcms2' cmsmtrx.c written by Marti Maria www.littlecms.com 
 * and is MIT licensed there
 * Vectors
 */
#define MATRIX_DET_TOLERANCE    0.0001
/** Inverse of a matrix b = a^(-1) */
int lcm2MAT3inverse(const lcm2MAT3* a, lcm2MAT3* b)
{
   double det, c0, c1, c2;

   c0 =  a -> v[1].n[1]*a -> v[2].n[2] - a -> v[1].n[2]*a -> v[2].n[1];
   c1 = -a -> v[1].n[0]*a -> v[2].n[2] + a -> v[1].n[2]*a -> v[2].n[0];
   c2 =  a -> v[1].n[0]*a -> v[2].n[1] - a -> v[1].n[1]*a -> v[2].n[0];

   det = a -> v[0].n[0]*c0 + a -> v[0].n[1]*c1 + a -> v[0].n[2]*c2;

   if (fabs(det) < MATRIX_DET_TOLERANCE) return 0;  // singular matrix; can't invert

   b -> v[0].n[0] = c0/det;
   b -> v[0].n[1] = (a -> v[0].n[2]*a -> v[2].n[1] - a -> v[0].n[1]*a -> v[2].n[2])/det;
   b -> v[0].n[2] = (a -> v[0].n[1]*a -> v[1].n[2] - a -> v[0].n[2]*a -> v[1].n[1])/det;
   b -> v[1].n[0] = c1/det;
   b -> v[1].n[1] = (a -> v[0].n[0]*a -> v[2].n[2] - a -> v[0].n[2]*a -> v[2].n[0])/det;
   b -> v[1].n[2] = (a -> v[0].n[2]*a -> v[1].n[0] - a -> v[0].n[0]*a -> v[1].n[2])/det;
   b -> v[2].n[0] = c2/det;
   b -> v[2].n[1] = (a -> v[0].n[1]*a -> v[2].n[0] - a -> v[0].n[0]*a -> v[2].n[1])/det;
   b -> v[2].n[2] = (a -> v[0].n[0]*a -> v[1].n[1] - a -> v[0].n[1]*a -> v[1].n[0])/det;

   return 1;
}
/* Axis of the matrix/array. No specific meaning at all. */
#define VX      0
#define VY      1
#define VZ      2
/** Initiate a vector */
void lcm2VEC3init(lcm2VEC3* r, double x, double y, double z)
{
    r -> n[VX] = x;
    r -> n[VY] = y;
    r -> n[VZ] = z;
}
/** Multiply two matrices */
void lcm2MAT3per(lcm2MAT3* r, const lcm2MAT3* a, const lcm2MAT3* b)
{
#define ROWCOL(i, j) \
    a->v[i].n[0]*b->v[0].n[j] + a->v[i].n[1]*b->v[1].n[j] + a->v[i].n[2]*b->v[2].n[j]

    lcm2VEC3init(&r-> v[0], ROWCOL(0,0), ROWCOL(0,1), ROWCOL(0,2));
    lcm2VEC3init(&r-> v[1], ROWCOL(1,0), ROWCOL(1,1), ROWCOL(1,2));
    lcm2VEC3init(&r-> v[2], ROWCOL(2,0), ROWCOL(2,1), ROWCOL(2,2));

#undef ROWCOL //(i, j)
}

/** Evaluate a vector across a matrix */
void CMSEXPORT lcm2MAT3eval(lcm2VEC3* r, const lcm2MAT3* a, const lcm2VEC3* v)
{
    r->n[VX] = a->v[0].n[VX]*v->n[VX] + a->v[0].n[VY]*v->n[VY] + a->v[0].n[VZ]*v->n[VZ];
    r->n[VY] = a->v[1].n[VX]*v->n[VX] + a->v[1].n[VY]*v->n[VY] + a->v[1].n[VZ]*v->n[VZ];
    r->n[VZ] = a->v[2].n[VX]*v->n[VX] + a->v[2].n[VY]*v->n[VY] + a->v[2].n[VZ]*v->n[VZ];
}


/* end of lcms code */

/** convert a matrix to CIE * xy triple */
int lcm2MAT3toCIExyYTriple ( const lcm2MAT3* a, lcm2CIExyYTriple * triple )
{
  int i,j,
      fail=0;
  double sum;
    for(i = 0; i < 3; ++i)
    {
      for(j = 0; j < 3; ++j)
      {
        if(i < 3 && a->v[i].n[j] == 0)
          fail = 1;
      }
      sum = a->v[i].n[0]+a->v[i].n[1]+a->v[i].n[2];
      if(sum != 0)
      {
        triple->v[i].xy[0] = a->v[i].n[0]/sum;
        triple->v[i].xy[1] = a->v[i].n[1]/sum;
      } else
      {
        triple->v[i].xy[0] = 1;
        triple->v[i].xy[1] = 1;
      }
    }
  return fail;
}

const char * lcm2MAT3show ( const lcm2MAT3* a )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    for(j = 0; j < 3; ++j)
      sprintf( &t[strlen(t)], " %g", a->v[i].n[j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * lcm2Mat34show ( const float a[3][4] )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    for(j = 0; j < 4; ++j)
      sprintf( &t[strlen(t)], " %g", a[i][j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * lcm2Mat4show ( const float a[4] )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i;
  t[0] = 0;
  for(i = 0; i < 4; ++i)
    sprintf( &t[strlen(t)], " %g", a[i]);
  sprintf( &t[strlen(t)], "\n" );
  return t;
}
const char * lcm2Mat43show ( const float a[4][3] )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 4; ++i)
  {
    for(j = 0; j < 3; ++j)
      sprintf( &t[strlen(t)], " %g", a[i][j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * lcm2CIExyYTriple_Show( lcm2CIExyYTriple * triple )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    sprintf( &t[strlen(t)], " x:%g y:%g", triple->v[i].xy[0],
                                          triple->v[i].xy[1]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}


/** @} */ /* profiler */

/** \addtogroup profiler
 *
 *  Oyranos ICC Profiler API provides a platformindependent C interface to generate 
 *  ICC profiles. It's main purpose is to generate ICC Profiles in a programatic way.
 *  The only dependency is littleCMS 2
 *  <a href="http://www.littlecms.com">www.littlecms.com</a>.
 * It reduces the need of many of the lcms2
 *  boilerplate for format independent sampling, multi localised strings from UTF8
 *  and more. The sampler collection contains effects and color space converters.
 *  The code consists of one source file and a header. So it can easily
 *  be placed inside your project.
 *
 *
 *  @section api API Documentation
 *  The Oyranos ICC Profiler API is contained in the lcm2_profiler.h header file.
 *
 *  The high level API takes few arguments and generates a profile in 
 *  one go. 
 *  Effect profiles can be created in one call
 *  by lcm2CreateAbstractProfile(). It needs a @ref samplers function, which
 *  fills the Look Up Table (LUT). Three APIs exist to generate white point
 *  effects, lcm2CreateAbstractTemperatureProfile() and
 *  lcm2CreateAbstractWhitePointProfileLab() or
 *  lcm2CreateAbstractWhitePointProfileBradford(). These above high level APIs allow to
 *  write the profile to disc in one go.
 *
 *  The lower level APIs can be used to customise the profile generation.
 *  Basic matrix/shaper profiles can be created with
 *  lcm2CreateICCMatrixProfile2() and filled with custom texts in
 *  lcm2CreateProfileFragment(). LUT elements can be added with,
 *  lcm2CreateProfileLutByMatrixAndCurves() and 
 *  lcm2CreateProfileLutByFuncAndCurves(). The later expects a ::lcm2Sampler_f
 *  function to fill in the LUT.
 *
 *  The following low level code sample comes from @ref lcm2_profiler.c.
 *  The code sets up a basic profile description and color spaces:
 *  @dontinclude lcm2_profiler.c
 *  @code
 *    // prepare some variables
 *    double icc_profile_version = 2.3;
 *    double icc_ab[2] = {0.0, 0.0};
 *    cmsHPROFILE profile;
 *    const char * kelvin_name = "5000 K"
 *    int error;
 *    int grid_size = 17;
 *    cmsToneCurve * i_curve[3] = {NULL,NULL,NULL}, * o_curve[3] = {NULL,NULL,NULL};
      i_curve[0] = o_curve[0] = cmsBuildGamma(0, 1.0);
      for(i = 1; i < 3; ++i) { i_curve[i] = o_curve[i] = i_curve[0]; }
 *  @endcode
 *  @skip fragment
    @until cmsSigAToB0Tag
 *
 *  Profile i/o happens with lcm2OpenProfileFile(), which takes file names and
 *  a few wildcards as arguments. lcm2WriteProfileToFile() helps writing of
 *  canonical profile names. lcm2WriteProfileToMem() writes a profile to a
 *  custom memory allocator.
 *  
 *  Most of the functions come with examples.
 *
 */

