/** @file oyranos_monitor_effect.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *
 *  @internal
 *  @brief    monitor effect APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2018/02/19
 */

#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_monitor_effect.h"
#include "oyConversion_s.h"


int      oyGetLinearEffectProfile    ( oyProfiles_s      * effects )
{
  /* 2. get effect profile and decide if it can be embedded into a VGCT tag */
  oyProfile_s * effect = oyProfile_FromStd ( oyPROFILE_EFFECT, 0, NULL );
  int is_linear = oyProfile_FindMeta( effect, "EFFECT_linear", "yes" ) != NULL;
  fprintf(stderr, "EFFECT_linear=yes: %d\n", is_linear);
  if(is_linear)
    oyProfiles_MoveIn( effects, &effect, -1 );
  else
    oyProfile_Release( &effect );

  return is_linear;
}

int      oyProfileAddWhitePointEffect( oyProfile_s       * monitor_profile,
                                       oyOptions_s      ** module_options )
{
  double        src_cie_a = 0.5, src_cie_b = 0.5, dst_cie_a = 0.5, dst_cie_b = 0.5;
  oyProfile_s * wtpt = NULL;
  int error = oyProfile_GetWhitePoint( monitor_profile, &src_cie_a, &src_cie_b );
  int display_white_point = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
  oyOptions_s * result_opts = NULL, * opts = NULL;

  if(!error)
    error = oyGetDisplayWhitePoint( display_white_point, &dst_cie_a, &dst_cie_b );
  fprintf( stderr, "%s display_white_point: %d [%g %g] -> [%g %g]\n",
          oyProfile_GetText( monitor_profile, oyNAME_DESCRIPTION ), display_white_point,
          src_cie_a, src_cie_b, dst_cie_a, dst_cie_b);
  error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/cie_a",
                                   dst_cie_a - src_cie_a, 0, OY_CREATE_NEW );
  error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/cie_b",
                                   dst_cie_b - src_cie_b, 0, OY_CREATE_NEW );
  error = oyOptions_Handle( "//" OY_TYPE_STD "/create_profile.white_point_adjust",
                                         opts,"create_profile.white_point_adjust",
                                         &result_opts );
  wtpt = (oyProfile_s*) oyOptions_GetType( result_opts, -1, "icc_profile",
                                           oyOBJECT_PROFILE_S );
  error = !wtpt;
  oyOptions_MoveInStruct( module_options,
                          OY_STD "/icc_color/display.icc_profile.abstract.white_point.automatic.oy-monitor",
                          (oyStruct_s**) &wtpt, OY_CREATE_NEW );
  oyOptions_Release( &result_opts );
  oyOptions_Release( &opts );

  return error;
}

uint16_t * oyProfileGetWhitePointRamp( int                 width,
                                       oyProfile_s       * p,
                                       oyOptions_s       * options )
{
  oyProfile_s * w      = oyProfile_FromStd ( oyASSUMED_WEB, 0, NULL );
  uint16_t    * ramp   = calloc( sizeof(uint16_t), width*3);
  oyImage_s   * input  = oyImage_Create( width, 1, ramp, OY_TYPE_123_16, w, 0 );
  oyImage_s   * output = oyImage_Create( width, 1, ramp, OY_TYPE_123_16, p, 0 );
  int i,j, error, mul = 65536/width;

  oyConversion_s * cc = oyConversion_CreateBasicPixels( input, output, options, NULL);

  for(i = 0; i < width; ++i)
  {
    for(j = 0; j < 3; ++j)
      ramp[i*3 + j] = i * mul;
  }

  if(getenv("OY_DEBUG_WRITE"))
    oyImage_WritePPM( input, "wtpt-effect-gray.ppm", "gray ramp" );

  error = oyConversion_RunPixels( cc, 0 );
  if(error)
    fprintf( stderr, "found issue while converting ramp: %d\n", error );

  oyProfile_Release( &w );
  oyImage_Release( &input );
  oyImage_Release( &output );

  return ramp;
}

uint16_t *   oyProfile_GetVCGT       ( oyProfile_s       * profile,
                                       int               * width )
{
  oyProfileTag_s * tag = oyProfile_GetTagById( profile, (icTagSignature) icSigVideoCardGammaTable );
  uint8_t * data_ = NULL;
  size_t size = 0;
  int error = oyProfileTag_GetBlock( tag, (void**) &data_, &size, oyAllocateFunc_ );
  uint16_t * ramp         = NULL;
  if(error || size < 18)
  {
    if(error) fprintf( stderr, "no vcgt tag %d\n", error );
    else fprintf( stderr, "vcgt tag size too small %lu\n", size );
    return ramp;
  }

  int parametrisch        = oyValueUInt32(*(icUInt32Number*) &data_[8]);
  icUInt16Number nkurven  = oyValueUInt16(*(icUInt16Number*) &data_[12]);
  icUInt16Number segmente = oyValueUInt16(*(icUInt16Number*) &data_[14]);
  icUInt16Number byte     = oyValueUInt16(*(icUInt16Number*) &data_[16]);
 
  fprintf( stderr, "vcgt parametric: %d curves: %d segments: %d bytes: %d\n", parametrisch, nkurven, segmente, byte );

  if (parametrisch) { //icU16Fixed16Number
      double r_gamma = 1.0/oyValueUInt32(*(icUInt32Number*)&data_[12])*65536.0;
      double start_r = oyValueUInt32(*(icUInt32Number*)&data_[16])/65536.0;
      double ende_r = oyValueUInt32(*(icUInt32Number*)&data_[20])/65536.0;
      double g_gamma = 1.0/oyValueUInt32(*(icUInt32Number*)&data_[24])*65536.0;
      double start_g = oyValueUInt32(*(icUInt32Number*)&data_[28])/65536.0;
      double ende_g = oyValueUInt32(*(icUInt32Number*)&data_[32])/65536.0;
      double b_gamma = 1.0/oyValueUInt32(*(icUInt32Number*)&data_[36])*65536.0;
      double start_b = oyValueUInt32(*(icUInt32Number*)&data_[40])/65536.0;
      double ende_b = oyValueUInt32(*(icUInt32Number*)&data_[44])/65536.0;
  } else {
    int start = 18, i,j;
    if((int)size < start + byte * segmente * nkurven)
    {
      fprintf( stderr, "vcgt tag too small: %lu need %d\n", size, start + byte * segmente * nkurven );
      return ramp;
    }
    if(byte != 2)
    {
      fprintf( stderr, "vcgt bytes not supported: %d need 2\n", byte );
      return ramp;
    }

    ramp = calloc( byte, segmente * nkurven);
    for (j = 0; j < nkurven; j++)
      for (i = segmente * j; i < segmente * (j+1); i++)
        ramp[(i - segmente*j) * nkurven + j] = oyValueUInt16 (*(icUInt16Number*)&data_[start + byte*i]);

    *width = segmente;
  }

  return ramp;
}

int          oyProfile_SetVCGT       ( oyProfile_s       * profile,
                                       uint16_t          * vcgt,
                                       int                 width )
{
  int size = 18+width*3*2, i,j;
  uint8_t * data_ = calloc(sizeof(char), size);
  uint16_t * u;
  int error = !data_;
  oyProfileTag_s * tag;

  if(error) return error;
  *(icUInt32Number*) &data_[0]  = oyValueUInt32( (icTagSignature) icSigVideoCardGammaTable );
  *(icUInt32Number*) &data_[8]  = oyValueUInt32( 0 ); /* parametric */
  *(icUInt16Number*) &data_[12] = oyValueUInt16( 3 ); /* channels */
  *(icUInt16Number*) &data_[14] = oyValueUInt16( width );
  *(icUInt16Number*) &data_[16] = oyValueUInt16( 2 ); /* bytes */

  u = (uint16_t*)&data_[18];
  for(i = 0; i < width; ++i)
    for(j = 0; j < 3; ++j)
      u[j*width+i] = oyValueUInt16(vcgt[i*3+j]);

  tag = oyProfileTag_CreateFromData((icTagSignature) icSigVideoCardGammaTable,
                                (icTagTypeSignature) icSigVideoCardGammaTable,
                                    oyOK, size, data_, NULL );
  if(!tag)
  {
    error = 1;
    fprintf(stderr, "VCGT Tag creation failed\n" );
  }

  if(!error)
  {
    error = oyProfile_TagMoveIn( profile, &tag, 1 );
    if(error)
      fprintf(stderr, "oyProfile_TagMoveIn failed %d\n", error );
  }

  return error;
}

int      oyProfile_CreateEffectVCGT  ( oyProfile_s       * prof )
{
  int error = 0;
  /* 2. get user effect profile and display white point effect */
  /* 2.1. get effect profile and decide if it can be embedded into a VGCT tag */
  oyOptions_s * module_options = NULL;
  oyProfiles_s * effects = oyProfiles_New(NULL);
  int is_linear = oyGetLinearEffectProfile( effects );
  if(is_linear)
  {
    int effect_switch = oyGetBehaviour( oyBEHAVIOUR_EFFECT );
    if(effect_switch)
    error = oyOptions_SetFromString( &module_options, OY_DEFAULT_EFFECT,
                                     effect_switch?"1":"0" , OY_CREATE_NEW );
    if(error)
      fprintf(stderr, "oyOptions_SetFromString(OY_DEFAULT_EFFECT) failed %d\n", error);
  }

  /* 2.2. get the display white point effect */
  error = oyProfileAddWhitePointEffect( prof, &module_options );
  if(error)
    fprintf(stderr, "No white for monitor profile %d\n", error);
  error = oyOptions_MoveInStruct( &module_options,
                                      OY_PROFILES_EFFECT,
                                       (oyStruct_s**) &effects,
                                       OY_CREATE_NEW );

  /* 3. extract a existing VCGT */
  int width = 256;
  uint16_t * vcgt = oyProfile_GetVCGT( prof, &width );
  oyImage_s * img;
  if(getenv("OY_DEBUG_WRITE"))
  {
    img = oyImage_Create( width, 1, vcgt, OY_TYPE_123_16, prof, 0 );
    oyImage_WritePPM( img, "wtpt-vcgt.ppm", "vcgt ramp" );
    oyImage_Release( &img );
  }

  /* 4. create conversion, fill ramp and convert */
  uint16_t * ramp = oyProfileGetWhitePointRamp( width, prof, module_options );
  if(getenv("OY_DEBUG_WRITE"))
  {
    img = oyImage_Create( width, 1, ramp, OY_TYPE_123_16, prof, 0 );
    oyImage_WritePPM( img, "wtpt-effect.ppm", "white point ramp" );
    oyImage_Release( &img );
  }

  /* 5. mix the two ramps */
  uint16_t * mix = calloc( sizeof(uint16_t), width*3);
  int i,j;
  for(i = 0; i < width; ++i)
    for(j = 0; j < 3; ++j)
      mix[i*3+j] = OY_ROUNDp( oyLinInterpolateRampU16c( vcgt, width, j,3, oyLinInterpolateRampU16c( ramp, width, j, 3, (double)i/width )/65535.) );
  if(getenv("OY_DEBUG_WRITE"))
  {
    img  = oyImage_Create( width, 1, mix, OY_TYPE_123_16, prof, 0 );
    oyImage_WritePPM( img, "wtpt-mix.ppm", "white point + vcgt" );
    oyImage_Release( &img );
  }

  /* 6. create a new VCGT tag and exchange the tag */
  if(oyProfile_SetVCGT( prof, mix, width ))
  {
    fprintf(stderr, "Alter VCGT tag failed\n");
    error = 1;
  }

  return error;
}
