/** @file oyranos_monitor_effect.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *
 *  @internal
 *  @brief    monitor effect APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2018/02/19
 *
 *  The display effects depend on a color server being active or not.
 *  With oyDisplayColorServerIsActive(), the normal case, the effects
 *  are created for servers and late binding application transforms.
 *  Those both work on the same way on a local scope. The options for 
 *  marking that is "display_mode"="1".
 *  Other non display_mode transforms shall not see the display effects,
 *  e.g. white_point and display effect.
 *
 *  If oyDisplayColorServerIsActive() is inactive, only the monitor
 *  profile shall see the display effects. It gets them embedded into
 *  it's VCGT table during oyDeviceSetup2(). These display effects are
 *  then applied globally by the graphics card gamma table API.
 *
 *  All these transforms rely on the XCM _ICC_COLOR_DESKTOP atom being
 *  present with at color server start. So oyDisplayColorServerIsActive()
 *  can detect the color server correctly.
 */

#include "oyranos_color.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_devices.h"
#include "oyranos_icc.h"
#include "oyranos_monitor_effect.h"
#include "oyranos_object_internal.h"
#include "oyranos_texts.h"
#include "oyConversion_s.h"
#include "oyProfile_s_.h"
#include "oyranos_color_internal.h"


/* The function allows only a effect profile with meta:"EFFECT_linear", "yes". */
int      oyAddLinearDisplayEffect    ( oyOptions_s      ** module_options )
{
  /* 2. get display effect profile and decide if it can be embedded into a VGCT tag */
  char * fn = oyGetPersistentString( OY_DEFAULT_DISPLAY_EFFECT_PROFILE, 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
  oyProfile_s * effect = (fn && fn[0]) ? oyProfile_FromName ( fn, 0, NULL ) : NULL;
  int is_linear = effect ? oyProfile_FindMeta( effect, "EFFECT_linear", "yes" ) != NULL : 0;
  int error = (is_linear == 0 && effect) ? 1 : 0;
  if(!effect) error = -1;
  oyMessageFunc_p( error > 0 ? oyMSG_WARN : oyMSG_DBG,(oyStruct_s*)*module_options, OY_DBG_FORMAT_
                   "EFFECT_linear=yes: %d %s", OY_DBG_ARGS_, is_linear, oyNoEmptyString_m_(fn) );
  if(is_linear)
    oyOptions_MoveInStruct( module_options,
                          OY_STD "/icc_color/display.icc_profile.abstract.effect.automatic.oy-monitor",
                          (oyStruct_s**) &effect, OY_CREATE_NEW );
  else
    oyProfile_Release( &effect );

  if(fn) oyFree_m_( fn );

  return error;
}

double oyGetTemperature                ( double              default_ )
{
  double cie_a = 0.0, cie_b = 0.0, XYZ[3], Lab[3];
  oyGetDisplayWhitePoint( 1 /* automatic */, XYZ );
  oyXYZ2Lab(XYZ,Lab); cie_a = Lab[1]/256.0+0.5; cie_b = Lab[2]/256.0+0.5;
  double dist = 1.0;
  double temperature = oyEstimateTemperature( cie_a, cie_b, &dist );
  if( (temperature && dist < 0.0001) ||
      default_ < 0 )
    return temperature;
  else
    return default_;
}

#define DBG_S_ if(oy_debug >= 1)DBG_S
/** @brief  create_profile.white_point_adjust.bradford
 *
 *  The profile will be generated in many different shades, which will explode
 *  conversion cache.
 */
int      oyProfileAddWhitePointEffect( oyProfile_s       * monitor_profile,
                                       oyOptions_s      ** module_options )
{
  oyProfile_s * wtpt = NULL;
  double        src_XYZ[3] = {0.0, 0.0, 0.0}, dst_XYZ[3] = {0.0, 0.0, 0.0},
                scale = 1.0;
  int error = oyProfile_GetWhitePoint( monitor_profile, src_XYZ );
  int32_t display_white_point = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
  oyOptions_s * result_opts = NULL, * opts = NULL;
  const char * desc = NULL;

  if(*module_options)
  {
    const char * value = oyOptions_FindString( *module_options, "display_white_point", 0 );
    if(value)
    {
      int c = atoi( value );
      if(c >= 0)
        display_white_point = c;
    }
  }

  if(!display_white_point)
    return 0;

  if(!error)
    error = oyGetDisplayWhitePoint( display_white_point, dst_XYZ );
  if(isnan(dst_XYZ[0]))
    error = 1;
  if(error)
  {
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                   "automatic display_white_point: not readable", OY_DBG_ARGS_);
    return error;
  }

  desc = oyProfile_GetText( monitor_profile, oyNAME_DESCRIPTION );

  error = oyOptions_SetFromString( &opts, "//" OY_TYPE_STD "/src_name", desc, OY_CREATE_NEW );
  if(error)
    return error;
  DBG_S_( oyPrintTime() );
  {
    int current = -1, choices = 0;
    const char ** choices_string_list = NULL;
    uint32_t flags = 0;
#ifdef HAVE_LOCALE_H
    char * old_loc = strdup(setlocale(LC_ALL,NULL));
    setlocale(LC_ALL,"C");
#endif
    error = oyOptionChoicesGet2( oyWIDGET_DISPLAY_WHITE_POINT, flags,
                                 oyNAME_NAME, &choices,
                                 &choices_string_list, &current );
    if(error > 0)
      oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                   "oyOptionChoicesGet2 failed %d", OY_DBG_ARGS_,
                   error);
#ifdef HAVE_LOCALE_H
    setlocale(LC_ALL,old_loc);
    if(old_loc) { free(old_loc); } old_loc = NULL;
#endif
    if(current > 0 && current < choices && choices_string_list)
    {
      if(current == 1) /* automatic */
      {
        double temperature = oyGetTemperature(-1);
        char k[12];
        sprintf(k, "%dK", (int)temperature);
        oyOptions_SetFromString( &opts, "//" OY_TYPE_STD "/illu_name", k, OY_CREATE_NEW );
        if(temperature <= 0.1)
          oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                   "automatic display_white_point: [%g %g %g] %s", OY_DBG_ARGS_,
                   dst_XYZ[0], dst_XYZ[1], dst_XYZ[2], k);
      }
      else
        oyOptions_SetFromString( &opts, "//" OY_TYPE_STD "/illu_name", choices_string_list[current], OY_CREATE_NEW );
      if(oy_debug)
        oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                   "illu_name: %s", OY_DBG_ARGS_,
                   oyOptions_FindString( opts, "illu_name", 0) );
    }
    oyOptionChoicesFree( oyWIDGET_DISPLAY_WHITE_POINT, &choices_string_list, choices );
  }

  if(oy_debug)
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_
                   "src_name: %s -> illu_name: %s",
                   OY_DBG_ARGS_, oyOptions_FindString(opts, "src_name", 0), oyOptions_FindString(opts, "illu_name", 0) );
  oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[0], 0, OY_CREATE_NEW );
  oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[1], 1, OY_CREATE_NEW );
  oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[2], 2, OY_CREATE_NEW );
  oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[0], 0, OY_CREATE_NEW );
  oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[1], 1, OY_CREATE_NEW );
  oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[2], 2, OY_CREATE_NEW );
  /* cache the display white point abstract profile */
  error = oyOptions_Handle( "//" OY_TYPE_STD "/create_profile.white_point_adjust.bradford",
                            opts,             "create_profile.white_point_adjust.bradford.file_name",
                            &result_opts );
  DBG_S_( oyPrintTime() );
  if(error > 0)
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                   "oyOptions_Handle(white_point_adjust.bradford.file_name) failed %d", OY_DBG_ARGS_,
                   error);

  /* write cache profile for slightly better speed and useful for debugging */
  if(error == 0)
  {
    const char * file_name = oyOptions_FindString( result_opts, "file_name", 0 );
    char * cache_path = oyGetInstallPath( oyPATH_CACHE, oySCOPE_USER, oyAllocateFunc_ ), *t;
    if(strstr( cache_path, "device_link") != NULL)
    {
      t = strstr( cache_path, "device_link");
      t[0] = '\000';
      oyStringAddPrintf( &cache_path, 0,0, "white_point_adjust/%s.icc", file_name );
    }

    if(oyIsFile_(cache_path))
    {
      if(oy_debug)
        oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                     "found file_name: %s -> %s\n", OY_DBG_ARGS_, file_name, cache_path );
      wtpt = oyProfile_FromFile( cache_path, 0,0 );
      DBG_S_( oyPrintTime() );
    }
    oyFree_m_(cache_path);
  }

  if(!error && wtpt)
  {
    oyOptions_MoveInStruct( module_options,
                          OY_STD "/icc_color/display.icc_profile.abstract.white_point.automatic.oy-monitor",
                          (oyStruct_s**) &wtpt, OY_CREATE_NEW );
    oyOptions_Release( &result_opts );
    oyOptions_Release( &opts );

    return error;
  }

  if(!error && !wtpt)
  {
    /* detect scaling factor
     * - convert monitor device RGB to XYZ,
     * - apply white point adaption matrix,
     * - convert back to monitor device RGB and
     * - again to XYZ to detect clipping and
     * - use that information for scaling inside the white point effect profile */
    oyProfile_s * xyz_profile = oyProfile_FromStd( oyASSUMED_XYZ, 0, 0 );
    float rgb[9] = {1,0,0, 0,1,0, 0,0,1},
          xyz[9] = {0,0,0, 0,0,0, 0,0,0};
    oyDATATYPE_e buf_type = oyFLOAT;
    oyConversion_s * cc_moni2xyz = oyConversion_CreateBasicPixelsFromBuffers(
                              monitor_profile, rgb, oyDataType_m(buf_type),
                              xyz_profile, xyz, oyDataType_m(buf_type),
                              0, 3 );
    oyConversion_s * cc_xyz2moni = oyConversion_CreateBasicPixelsFromBuffers(
                              xyz_profile, xyz, oyDataType_m(buf_type),
                              monitor_profile, rgb, oyDataType_m(buf_type),
                              0, 3 );
    oyMAT3 wtpt_adapt;
    oyCIEXYZ srcWtpt = {src_XYZ[0], src_XYZ[1], src_XYZ[2]},
             dstIllu = {dst_XYZ[0], dst_XYZ[1], dst_XYZ[2]};
    error = !oyAdaptationMatrix( &wtpt_adapt, NULL, &srcWtpt, &dstIllu );
    int i,j;
    for(j = 0; j < 100; ++j)
    {
      oyConversion_RunPixels( cc_moni2xyz, 0 );
      if(oy_debug)
        oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)cc_moni2xyz, OY_DBG_FORMAT_
                   "rgb->xyz:\nR[%g %g %g] G[%g %g %g] B[%g %g %g]",
                   OY_DBG_ARGS_, xyz[0], xyz[1], xyz[2], xyz[3], xyz[4], xyz[5], xyz[6], xyz[7], xyz[8]);
      oyVEC3 rXYZ, srcXYZ[3] = { {{xyz[0], xyz[1], xyz[2]}}, {{xyz[3], xyz[4], xyz[5]}}, {{xyz[6], xyz[7], xyz[8]}} };
      oyMAT3 wtpt_adapt_scaled,
             scale_mat = {{ {{scale,0,0}}, {{0,scale,0}}, {{0,0,scale}} }};
      oyMAT3per( &wtpt_adapt_scaled, &wtpt_adapt, &scale_mat );
      oyMAT3eval( &rXYZ, &wtpt_adapt_scaled, &srcXYZ[0] ); for(i = 0; i < 3; ++i) xyz[0+i] = rXYZ.n[i];
      oyMAT3eval( &rXYZ, &wtpt_adapt_scaled, &srcXYZ[1] ); for(i = 0; i < 3; ++i) xyz[3+i] = rXYZ.n[i];
      oyMAT3eval( &rXYZ, &wtpt_adapt_scaled, &srcXYZ[2] ); for(i = 0; i < 3; ++i) xyz[6+i] = rXYZ.n[i];
      if(oy_debug)
        oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)cc_moni2xyz, OY_DBG_FORMAT_
                   "srcWtpt->Illu:\nR[%g %g %g] G[%g %g %g] B[%g %g %g]", OY_DBG_ARGS_,
          xyz[0], xyz[1], xyz[2], xyz[3], xyz[4], xyz[5], xyz[6], xyz[7], xyz[8]);
      oyConversion_RunPixels( cc_xyz2moni, 0 );
      if(oy_debug)
        oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)cc_moni2xyz, OY_DBG_FORMAT_
                   "xyz->rgb:\nR[%g %g %g] G[%g %g %g] B[%g %g %g] %g",
                   OY_DBG_ARGS_, rgb[0], rgb[1], rgb[2], rgb[3], rgb[4], rgb[5], rgb[6], rgb[7], rgb[8], scale);
      if(rgb[0+0] < 0.99 && rgb[3+1] < 0.99 && rgb[6+2] < 0.99)
      {
        if(scale < 1.00) scale += 0.01;
        break;
      }
      scale -= 0.01;
    }
  }

  oyMessageFunc_p( /*error ?*/ oyMSG_WARN/*:oyMSG_DBG*/,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                   "%s display_white_point: %d [%g %g %g] -> [%g %g %g] * %g %d", OY_DBG_ARGS_,
          desc, display_white_point,
          src_XYZ[0], src_XYZ[1], src_XYZ[2], dst_XYZ[0], dst_XYZ[1], dst_XYZ[2], scale, error);
  if(error > 0)
    return error;

  /* write cache profile for slightly better speed and useful for debugging */
  if(error == 0)
  {
    const char * file_name = oyOptions_FindString( result_opts, "file_name", 0 );
    char * cache_path = oyGetInstallPath( oyPATH_CACHE, oySCOPE_USER, oyAllocateFunc_ ), *t;
    if(strstr( cache_path, "device_link") != NULL)
    {
      t = strstr( cache_path, "device_link");
      t[0] = '\000';
      oyStringAddPrintf( &cache_path, 0,0, "white_point_adjust/%s.icc", file_name );
    }

    {
      error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/scale", scale, 0, OY_CREATE_NEW );
      if(oy_debug)
        oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)monitor_profile, OY_DBG_FORMAT_
                     "creating file_name: %s -> %s\n", OY_DBG_ARGS_, file_name, cache_path );
      error = oyOptions_Handle( "//" OY_TYPE_STD "/create_profile.white_point_adjust.bradford",
                            opts,             "create_profile.white_point_adjust.bradford",
                            &result_opts );
      wtpt = (oyProfile_s*) oyOptions_GetType( result_opts, -1, "icc_profile",
                                           oyOBJECT_PROFILE_S );
      error = !wtpt;
      if(!error)
        oyProfile_ToFile_( (oyProfile_s_*) wtpt, cache_path );
      DBG_S_( oyPrintTime() );
    }
  }

  error = !wtpt;
  if(error == 0)
    oyOptions_MoveInStruct( module_options,
                          OY_STD "/icc_color/display.icc_profile.abstract.white_point.automatic.oy-monitor",
                          (oyStruct_s**) &wtpt, OY_CREATE_NEW );
  oyOptions_Release( &result_opts );
  oyOptions_Release( &opts );

  return error;
}

int        oyAddMonitorEffects       ( oyProfile_s       * monitor_profile,
                                       oyOptions_s      ** module_options )
{
  int error = 0;
  oyOptions_s * f_options = *module_options;
  int f_options_n, i,
      display_white_point = 0;
  oyOption_s * o = oyOptions_Find( f_options, "display_white_point", oyNAME_PATTERN );
  if(o)
  {
    const char * value = oyOption_GetValueString(o,0);
    if(value)
    {
      int c = atoi( value );
      if(c >= 0)
        display_white_point = c;
    }
    oyOption_Release( &o );
  }

  if( oy_debug )
      oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*)f_options,
                OY_DBG_FORMAT_"display_white_point: %d", OY_DBG_ARGS_, display_white_point);
  /* erase old display profile */
  f_options_n = oyOptions_Count( f_options );
  for(i = 0; i < f_options_n; ++i)
  {
    oyOption_s * o = oyOptions_Get(f_options, i);
    if(o && oyFilterRegistrationMatch( oyOption_GetRegistration(o),
                       OY_STD "/icc_color/display.icc_profile.abstract.white_point.automatic", 0 ))
    {
      if(oy_debug)
        oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*)f_options,
                  OY_DBG_FORMAT_"release: %s", OY_DBG_ARGS_, oyOption_GetRegistration(o));
      oyOption_Release( &o );
      oyOptions_ReleaseAt( f_options, i );
      break;
    }
    oyOption_Release( &o );
  }

  if(display_white_point) /* not "none" */
  {
    error = oyProfileAddWhitePointEffect( monitor_profile, module_options );
    if( error || oy_debug )
        oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)f_options,
                  OY_DBG_FORMAT_"display_white_point: %d %s", OY_DBG_ARGS_, display_white_point, oyProfile_GetText( monitor_profile, oyNAME_DESCRIPTION ));
  }

  return error;
}

int        oyAddDisplayEffects       ( oyOptions_s      ** module_options )
{
  int error = 0;
  oyOptions_s * f_options = *module_options;
  int f_options_n, i;

  /* erase old display profile */
  f_options_n = oyOptions_Count( f_options );
  for(i = 0; i < f_options_n; ++i)
  {
    oyOption_s * o = oyOptions_Get(f_options, i);
    if(o && oyFilterRegistrationMatch( oyOption_GetRegistration(o),
                       OY_STD "/icc_color/display.icc_profile.abstract.effect.automatic", 0 ))
    {
      if(oy_debug)
        oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*)f_options,
                  OY_DBG_FORMAT_"release: %s", OY_DBG_ARGS_, oyOption_GetRegistration(o));
      oyOption_Release( &o );
      oyOptions_ReleaseAt( f_options, i );
      break;
    }
    oyOption_Release( &o );
  }
  error = oyAddLinearDisplayEffect( module_options );
  if( error > 0 || oy_debug )
    oyMessageFunc_p( error > 0 ? oyMSG_WARN : oyMSG_DBG, (oyStruct_s*)f_options,
                  OY_DBG_FORMAT_"display_white_point error: %d", OY_DBG_ARGS_, error );

  return error;
}

uint16_t * oyProfileGetWhitePointRamp( int                 width,
                                       oyProfile_s       * p,
                                       oyOptions_s       * options )
{
  uint16_t    * ramp   = calloc( sizeof(uint16_t), width*3);
  oyImage_s   * input  = oyImage_Create( width, 1, ramp, OY_TYPE_123_16, p, 0 );
  oyImage_s   * output = oyImage_Create( width, 1, ramp, OY_TYPE_123_16, p, 0 );
  int i,j, error, mul = 65536/width;

  oyConversion_s * cc = oyConversion_CreateBasicPixels( input, output, options, NULL);

  for(i = 0; i < width; ++i)
  {
    for(j = 0; j < 3; ++j)
      ramp[i*3 + j] = i * mul;
  }

  if(getenv("OY_DEBUG_WRITE"))
    oyImage_WritePPM( input, "wtpt-effect-raw.ppm", "unaltered gray ramp" );


  oyConversion_Correct( cc, "//" OY_TYPE_STD "/icc_color", 0, NULL);
  error = oyConversion_RunPixels( cc, 0 );
  if(error)
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)p, OY_DBG_FORMAT_
                     "found issue while converting ramp: %d", OY_DBG_ARGS_, error );

  if(getenv("OY_DEBUG_WRITE"))
  {
    oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
    oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
    char * comment = oyjlStringCopy("gray ramp", oyAllocateFunc_);
    const char * ndesc = oyFilterNode_GetText( icc, oyNAME_NAME );
    oyjlStringAdd( &comment, oyAllocateFunc_, oyDeAllocateFunc_, "\n%s", ndesc );
    oyImage_WritePPM( input, "wtpt-effect-gray.ppm", comment );
    oyFree_m_(comment);
    oyFilterGraph_Release( &cc_graph );
    oyFilterNode_Release( &icc );
  }

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
  oyProfileTag_Release( &tag );
  if(error || size < 18)
  {
    const char * name = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
    if(error) oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)profile, OY_DBG_FORMAT_
                               "no vcgt tag %d %s", OY_DBG_ARGS_, error, oyNoEmptyString_m_(name) );
    else oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)profile, OY_DBG_FORMAT_ "vcgt tag size too small %lu %s\n",
                          OY_DBG_ARGS_, size, oyNoEmptyString_m_(name) );
    goto clean_oyProfile_GetVCGT;
  }

  int parametrisch        = oyValueUInt32(*(icUInt32Number*) &data_[8]);
  icUInt16Number nkurven  = oyValueUInt16(*(icUInt16Number*) &data_[12]);
  icUInt16Number segmente = oyValueUInt16(*(icUInt16Number*) &data_[14]);
  icUInt16Number byte     = oyValueUInt16(*(icUInt16Number*) &data_[16]);
 
  if(oy_debug)
    oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)profile, OY_DBG_FORMAT_
                     "vcgt parametric: %d curves: %d segments: %d bytes: %d\n",
                     OY_DBG_ARGS_, parametrisch, nkurven, segmente, byte );

  if (parametrisch)
  { //icU16Fixed16Number
    const char * name = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)profile, OY_DBG_FORMAT_
                     "parametric vcgt is not supported: %s",
                     OY_DBG_ARGS_, oyNoEmptyString_m_(name) );
#if 0
      double r_gamma = 1.0/oyValueUInt32(*(icUInt32Number*)&data_[12])*65536.0;
      double start_r = oyValueUInt32(*(icUInt32Number*)&data_[16])/65536.0;
      double ende_r = oyValueUInt32(*(icUInt32Number*)&data_[20])/65536.0;
      double g_gamma = 1.0/oyValueUInt32(*(icUInt32Number*)&data_[24])*65536.0;
      double start_g = oyValueUInt32(*(icUInt32Number*)&data_[28])/65536.0;
      double ende_g = oyValueUInt32(*(icUInt32Number*)&data_[32])/65536.0;
      double b_gamma = 1.0/oyValueUInt32(*(icUInt32Number*)&data_[36])*65536.0;
      double start_b = oyValueUInt32(*(icUInt32Number*)&data_[40])/65536.0;
      double ende_b = oyValueUInt32(*(icUInt32Number*)&data_[44])/65536.0;
#endif
  } else {
    int start = 18, i,j;
    if((int)size < start + byte * segmente * nkurven)
    {
      const char * name = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
      oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)profile, OY_DBG_FORMAT_
                       "vcgt tag too small: %lu need %d %s",
                        OY_DBG_ARGS_,
                        size, start + byte * segmente * nkurven, oyNoEmptyString_m_(name) );
      goto clean_oyProfile_GetVCGT;
    }
    if(byte != 2)
    {
      const char * name = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
      oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)profile, OY_DBG_FORMAT_
                        "vcgt bytes not supported: %d need 2 %s",
                        OY_DBG_ARGS_,
                        byte, oyNoEmptyString_m_(name) );
      goto clean_oyProfile_GetVCGT;
    }

    ramp = calloc( byte, (unsigned long)segmente * (unsigned long)nkurven);
    for (j = 0; j < nkurven; j++)
      for (i = segmente * j; i < segmente * (j+1); i++)
        ramp[(i - segmente*j) * nkurven + j] = oyValueUInt16 (*(icUInt16Number*)&data_[start + byte*i]);

    *width = segmente;
  }

clean_oyProfile_GetVCGT:
  if(data_ && size)
    oyDeAllocateFunc_( data_ );

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
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)profile, OY_DBG_FORMAT_
                        "VCGT Tag creation failed",
                        OY_DBG_ARGS_ );
  }
  free(data_); data_ = NULL; u = NULL;

  if(!error)
  {
    error = oyProfile_TagMoveIn( profile, &tag, 1 );
    if(error)
      oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)profile, OY_DBG_FORMAT_
                        "oyProfile_TagMoveIn failed %d",
                        OY_DBG_ARGS_, error );
  }

  return error;
}

int      oyProfile_CreateEffectVCGT  ( oyProfile_s       * prof )
{
  int error = 0;
  /* 2. get user effect profile and display white point effect */
  /* 2.1. get effect profile and decide if it can be embedded into a VGCT tag */
  oyOptions_s * module_options = NULL;
  error = oyAddLinearDisplayEffect( &module_options );
  if(error)
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)prof, OY_DBG_FORMAT_
                        "No display effect for monitor profile %d",
                        OY_DBG_ARGS_,
                        error );

  /* 2.2. get the display white point effect */
  error = oyProfileAddWhitePointEffect( prof, &module_options );
  if(error)
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)prof, OY_DBG_FORMAT_
                        "No white for monitor profile %d",
                        OY_DBG_ARGS_,
                        error );

  /* 3. extract a existing VCGT */
  int width = 256;
  uint16_t * vcgt = oyProfile_GetVCGT( prof, &width );
  oyImage_s * img;
  if(vcgt && getenv("OY_DEBUG_WRITE"))
  {
    img = oyImage_Create( width, 1, vcgt, OY_TYPE_123_16, prof, 0 );
    oyImage_WritePPM( img, "wtpt-vcgt.ppm", "vcgt ramp" );
    oyImage_Release( &img );
  }

  /* 4. create conversion, fill ramp and convert */
  uint16_t * ramp = oyProfileGetWhitePointRamp( width, prof, module_options );
  if(ramp && getenv("OY_DEBUG_WRITE"))
  {
    img = oyImage_Create( width, 1, ramp, OY_TYPE_123_16, prof, 0 );
    oyImage_WritePPM( img, "wtpt-effect.ppm", "white point ramp" );
    oyImage_Release( &img );
  }

  /* 5. mix the two ramps */
  uint16_t * mix = NULL;
  if(vcgt)
    mix = calloc( sizeof(uint16_t), width*3);
  int i,j;
  if(mix)
  for(i = 0; i < width; ++i)
    for(j = 0; j < 3; ++j)
    {
      uint16_t v = oyLinInterpolateRampU16c( ramp, width, j, 3, (double)i/(double)width );
      double vd = v / 65535.0;
      mix[i*3+j] = OY_ROUNDp( oyLinInterpolateRampU16c( vcgt, width, j,3, vd ) );
    }
  if(mix && getenv("OY_DEBUG_WRITE"))
  {
    img  = oyImage_Create( width, 1, mix, OY_TYPE_123_16, prof, 0 );
    oyImage_WritePPM( img, "wtpt-mix.ppm", "white point + vcgt" );
    oyImage_Release( &img );
  }

  /* 6. create a new VCGT tag and exchange the tag */
  if((mix || ramp) && oyProfile_SetVCGT( prof, mix?mix:ramp, width ))
  {
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)prof, OY_DBG_FORMAT_
                        "Alter VCGT tag failed",
                        OY_DBG_ARGS_ );
    error = 1;
  }

  if(mix) oyDeAllocateFunc_(mix);
  if(ramp) oyDeAllocateFunc_(ramp);
  if(vcgt) oyDeAllocateFunc_(vcgt);

  return error;
}

int oyDisplayColorServerIsActive( )
{
  int color_server_active = 0; /* A XCM color server expects local white point adjustment and effect. */
  oyOptions_s * result_opts = oyOptions_New(NULL);

  oyOptions_Handle( "//" OY_TYPE_STD "/color_server_active",
                          result_opts,"color_server_active.source_data",
                          &result_opts );
  color_server_active = oyOptions_FindString( result_opts, "color_server_active", "1" ) != NULL;
  oyOptions_Release( &result_opts );

  if(oy_debug && color_server_active)
    oyMessageFunc_p( oyMSG_DBG, NULL, OY_DBG_FORMAT_
                        "color_server_active=%d",
                        OY_DBG_ARGS_, color_server_active );

  return color_server_active;
}

void     oyDeviceSetupVCGT           ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       const char        * profile_name )
{
  int error;
  /* set the gamma table */
  error = oyOptions_SetFromString( &options,
                                   "//"OY_TYPE_STD"/config/gamma_only", "yes", OY_CREATE_NEW );
  error = oyOptions_SetFromString( &options,
                                   "//"OY_TYPE_STD"/config/profile_name", profile_name, OY_CREATE_NEW|OY_MATCH_KEY );
  if(error)
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)device, OY_DBG_FORMAT_
                        "%s -> %s",
                        OY_DBG_ARGS_,
                        profile_name, oyOptions_GetText( options, oyNAME_JSON) );

  error = oyOptions_SetFromString( &options,
                                   "//"OY_TYPE_STD"/config/command", "setup", OY_CREATE_NEW|OY_MATCH_KEY );
  error = oyOptions_SetFromString( &options,
                                   "//"OY_TYPE_STD"/config/device_name", oyConfig_FindString( device, "device_name", 0), OY_CREATE_NEW|OY_MATCH_KEY );
  if(oy_debug || error)
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)device, OY_DBG_FORMAT_
                        "oyDeviceBackendCall(%s)%s",
                        OY_DBG_ARGS_,
                        profile_name, oyOptions_GetText( options, oyNAME_JSON) );

  /* send the query to a module */
  error = oyDeviceBackendCall( device, options );
}

/** Function oyDeviceSetup2
 *  @brief   activate the device using the stored configuration
 *
 *  @param[in]     device              the device
 *  @param[in]     options             additional options,
 *                                     - "skip_ask_for_profile == "yes" - skips oyDeviceAskProfile2() call; useful for systems, where no empty profile is possible like CS
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/23
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 */
int      oyDeviceSetup2              ( oyConfig_s        * device,
                                       oyOptions_s       * options )
{
  int error = oyDeviceSetup( device, options );

  /* modify the VCGT tag */
  /* 1. detect if a XCM color server is active */
  int active = oyDisplayColorServerIsActive( );
  /* 1.1. stop if XCM is active*/
  if(active)
    return error;
  else
  {
    char * tmpname = oyGetTempFileName_( NULL, "vcgt.icc", 0, oyAllocateFunc_ );
    oyProfile_s * prof = NULL;
    const char * profile_name;

    oyDeviceAskProfile2( device, options, &prof );
    profile_name = oyProfile_GetFileName( prof, -1 );

    if(oyProfile_CreateEffectVCGT( prof ))
      oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)device, OY_DBG_FORMAT_
                       "Create Effect VCGT failed: %s", OY_DBG_ARGS_, profile_name );
    if(oy_debug)
      oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)device, OY_DBG_FORMAT_
                       "%s + modified VCGT -> %s", OY_DBG_ARGS_, profile_name, tmpname );
    /* dump altered profile tag to clean memory */
    oyProfile_GetSize( prof,0 );
    /* write the profile */
    oyProfile_ToFile_( (oyProfile_s_*) prof, tmpname );

    /* set the gamma table */
    oyDeviceSetupVCGT( device, options, tmpname );

    if(oy_debug == 0)
      remove( tmpname );
    oyFree_m_( tmpname );
  }

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
 *  @param         options             
 *                                     - options passed to the backend
 *                                     - "///icc_profile_flags" with a int 
 *                                       containing ::OY_ICC_VERSION_2 or
 *                                       ::OY_ICC_VERSION_4
 *  @param         profile             the device's ICC profile
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/23
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 */
OYAPI int OYEXPORT oyDeviceGetProfile( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyProfile_s      ** profile )
{
  int error = !device,
      l_error = 0;
  oyConfig_s * s = device;
  oyProfile_s * p;
  oyOption_s * o = NULL;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )


  l_error = oyDeviceAskProfile2( device, options, profile ); OY_ERR

  /** This function does a device setup in case no profile is delivered
   *  by the according module. */
  if(error != 0 && !*profile)
  {
    oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)device, OY_DBG_FORMAT_
                     "calling oyDeviceSetup2()\n", OY_DBG_ARGS_ );
    error = oyDeviceSetup2( device, options );
  }

  /* The backend shows with the existence of the "icc_profile" response that it
   * can handle device profiles through the driver. */
  if(error <= 0 && !*profile)
    o = oyConfig_Find( device, "icc_profile" );

  p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
  if(oyProfile_GetSignature( p, oySIGNATURE_MAGIC ) == icMagicNumber)
    *profile = p;
  else if(!error)
    error = -1;
  p = NULL;

  if(error <= 0 && !*profile) 
  { l_error = oyDeviceAskProfile2( device, options, profile ); OY_ERR }

  /** As a last means oyASSUMED_WEB is delivered. */
  if(!*profile)
  {
    int32_t icc_profile_flags = 0;

    oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags );
    *profile = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
    if(error == 0)
      error = -1;
  }

  return error;
}


/**
 *  @brief get the ICC*XYZ coordinates for display white point target
 *
 *  The function asks for a white point target for displaying.
 *
 *  @see ::OY_DEFAULT_DISPLAY_WHITE_POINT.
 *
 *  @param[in]     mode                -1 for oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT )
 *  @param[out]    ICC_XYZ             ICC*XYZ trio in 0.0 - 2.0 range
 *  @return                            error
 *                                     - 0: success
 *                                     - -1: no white point available
 *                                     - >1: error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/01
 *  @since   2017/06/01 (Oyranos: 0.9.7)
 */
int      oyGetDisplayWhitePoint      ( int                 mode,
                                       double            * XYZ )
{
  int error = -1;
  char * value = NULL;

  double DE_xyz[5][3] = {
  { 0.34567,0.35850,0.29583}, /* D50 */
  { 0.33242,0.34743,0.32015}, /* D55 */
  { 0.31271,0.32902,0.35827}, /* D65 */
  { 0.29902,0.31485,0.38613}, /* D75 */
  { 0.2848, 0.2932, 0.42200}  /* D93 */ };

  if(mode == -1)
    mode = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );

  switch(mode)
  {
  case 0: return error;
  case 1:
    {
      value = oyGetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_X, 0,
                                     oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(!value)
        oyMessageFunc_p( oyMSG_ERROR,NULL, OY_DBG_FORMAT_
                         "Can not obtain white point! Try CLI command: oyranos-monitor-white-point -a 5000", OY_DBG_ARGS_ );
      if(oyjlStringToDouble( value, &XYZ[0] ) > 0)
        return error;
      oyFree_m_( value );
      value = oyGetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_Y, 0,
                                     oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(oyjlStringToDouble( value, &XYZ[1] ) > 0)
        return error;
      oyFree_m_( value );
      value = oyGetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_Z, 0,
                                     oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(oyjlStringToDouble( value, &XYZ[2] ) > 0)
        return error;
      oyFree_m_( value );
      error = 0;
    }
    break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    {
      int p = mode-2;
      double xyz[3] = { DE_xyz[p][0], DE_xyz[p][1], DE_xyz[p][2] };

      XYZ[0] = xyz[0]/xyz[1];
      XYZ[1] = 1.0;
      XYZ[2] = xyz[2]/xyz[1];
      error = 0;
    }
    break;
  }

  if(mode > 6)
  {
    int pos = mode - 7;
    oyOptions_s * options = NULL;
    oyConfigs_s * devices = oyGetMonitors( &options ); /* cached in oy_monitors_cache_; do not release here */
    oyConfig_s * monitor = oyConfigs_Get( devices, pos );
    oyProfile_s* profile = NULL;

    oyDeviceGetProfile( monitor, options, &profile );
    error = oyProfile_GetWhitePoint( profile, XYZ );

    oyConfig_Release( &monitor );
    oyOptions_Release( &options );
    oyProfile_Release( &profile );
  }

  return error;
}

