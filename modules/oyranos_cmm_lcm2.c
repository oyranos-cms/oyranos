/** @file oyranos_cmm_lcm2.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    littleCMS CMM module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */




#include <lcms2.h>
#include <stdarg.h>

#include "oyranos_cmm.h"         /* the API's this CMM implements */
#include "oyranos_helper.h"      /* oySprintf_ and other local helpers */
#include "oyranos_alpha_internal.h" /* hashTextAdd_m ... */
#include "oyranos_i18n.h"
#include "oyranos_string.h"

/*
oyCMMInfo_s   lcm2_cmm_module;
oyCMMapi4_s     lcm2_api4_cmm;
oyCMMui_s         lcm2_api4_ui;
oyCMMapi7_s     lcm2_api7_cmm;
oyConnectorImaging_s* lcm2_cmmIccSocket_connectors[2];
oyConnectorImaging_s    lcm2_cmmIccSocket_connector;
oyConnectorImaging_s* lcm2_cmmIccPlug_connectors[2];
oyConnectorImaging_s    lcm2_cmmIccPlug_connector;
oyCMMapi6_s     lcm2_api6_cmm;
oyCMMapi10_s    lcm2_api10_cmm;
*/

void* oyAllocateFunc_           (size_t        size);
void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAlloc_f     allocate_func);
void  oyDeAllocateFunc_         (void *        data);

#include <math.h>
#include <string.h>                    /* memcpy */


/* --- internal definitions --- */

#define CMM_NICK "lcm2"
#define CMMProfileOpen_M    cmsOpenProfileFromMemTHR
#define CMMProfileRelease_M cmsCloseProfile
#define CMMToString_M(text) #text
#define CMMMaxChannels_M 16
#define lcm2PROFILE "lcP2"
#define lcm2TRANSFORM "lcC2"
/** The proofing LUTs grid size may improove the sharpness of out of colour 
 *  marking, but at the prise of lost speed and increased memory consumption.
 *  53 is the grid size used internally in lcm2' gamut marking code. */
#define lcm2PROOF_LUT_GRID_RASTER 53

#define CMM_VERSION {0,1,0}

int lcm2CMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );
oyMessage_f message = lcm2CMMWarnFunc;

void lcm2ErrorHandlerFunction         ( cmsContext          ContextID,
                                       cmsUInt32Number     ErrorCode,
                                       const char        * ErrorText );
int            lcm2CMMMessageFuncSet ( oyMessage_f         message_func );
int                lcm2CMMInit       ( );


/** @struct lcm2ProfileWrap_s
 *  @brief lcm2 wrapper for profile data struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/10
 *  @since   2007/12/10 (Oyranos: 0.1.8)
 */
typedef struct lcm2ProfileWrap_s_ {
  uint32_t     type;                   /**< shall be lcm2PROFILE */
  size_t       size;
  oyPointer    block;                  /**< Oyranos raw profile pointer. Dont free! */
  oyPointer    lcm2;                   /**< cmsHPROFILE struct */
  icColorSpaceSignature sig;           /**< ICC profile signature */
} lcm2ProfileWrap_s;

/** @struct  lcm2TransformWrap_s
 *  @brief   lcm2 wrapper for transform data struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/20
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
typedef struct lcm2TransformWrap_s_ {
  int          type;                   /**< shall be lcm2TRANSFORM */
  oyPointer    lcm2;                   /**< cmsHPROFILE struct */
  icColorSpaceSignature sig_in;        /**< ICC profile signature */
  icColorSpaceSignature sig_out;       /**< ICC profile signature */
  oyPixel_t    oy_pixel_layout_in;
  oyPixel_t    oy_pixel_layout_out;
} lcm2TransformWrap_s;


lcm2TransformWrap_s * lcm2TransformWrap_Set_ (
                                       cmsHTRANSFORM       xform,
                                       icColorSpaceSignature colour_in,
                                       icColorSpaceSignature colour_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyCMMptr_s        * oy );
int      lcm2CMMTransform_GetWrap_   ( oyCMMptr_s        * cmm_ptr,
                                       lcm2TransformWrap_s ** s );
int lcm2CMMDeleteTransformWrap       ( oyPointer         * wrap );

lcm2ProfileWrap_s * lcm2CMMProfile_GetWrap_(
                                       oyCMMptr_s        * cmm_ptr );
int lcm2CMMProfileReleaseWrap        ( oyPointer         * p );

int                lcm2CMMCheckPointer(oyCMMptr_s        * cmm_ptr,
                                       const char        * resource );
int        oyPixelToCMMPixelLayout_  ( oyPixel_t           pixel_layout,
                                       icColorSpaceSignature colour_space );
char * lcm2Image_GetText             ( oyImage_s         * image,
                                       int                 verbose,
                                       oyAlloc_f           allocateFunc );


char * lcm2FilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );
char * lcm2FlagsToText               ( int                 flags );
cmsHPROFILE  lcm2GamutCheckAbstract  ( oyProfile_s       * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof );
oyPointer  lcm2CMMColourConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
oyOptions_s* lcm2Filter_CmmIccValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
cmsHPROFILE  lcm2AddProfile          ( oyProfile_s       * p );
cmsHPROFILE  lcm2AddProofProfile     ( oyProfile_s       * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof );
oyPointer lcm2FilterNode_CmmIccContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int  lcm2CMMdata_Convert             ( oyCMMptr_s        * data_in,
                                       oyCMMptr_s        * data_out,
                                       oyFilterNode_s    * node );
int      lcm2FilterPlug_CmmIccRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
const char * lcm2InfoGetText         ( const char        * select,
                                       oyNAME_e            type );




/* --- implementations --- */

/** Function lcm2CMMInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/11 (Oyranos: 0.1.8)
 *  @date    2009/12/17
 */
int                lcm2CMMInit       ( oyStruct_s        * filter )
{
  int error = 0;
  /*cmsErrorAction( LCMS_ERROR_SHOW );*/
  cmsSetLogErrorHandler( lcm2ErrorHandlerFunction );
  return error;
}



/** Function lcm2CMMProfile_GetWrap_
 *  @brief   convert to lcm2 profile wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/10
 *  @since   2007/12/10 (Oyranos: 0.1.8)
 */
lcm2ProfileWrap_s * lcm2CMMProfile_GetWrap_( oyCMMptr_s * cmm_ptr )
{
  lcm2ProfileWrap_s * s = 0;

  char type_[4] = lcm2PROFILE;
  int type = *((int*)&type_);

  if(cmm_ptr && !lcm2CMMCheckPointer( cmm_ptr, lcm2PROFILE ) &&
     cmm_ptr->ptr)
    s = (lcm2ProfileWrap_s*) cmm_ptr->ptr;

  if(s && s->type != type)
    s = 0;

  return s;
}

/** Function lcm2CMMTransform_GetWrap_
 *  @brief   convert to lcm2 transform wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 *  @date    2009/05/28
 */
int      lcm2CMMTransform_GetWrap_   ( oyCMMptr_s        * cmm_ptr,
                                       lcm2TransformWrap_s ** s )
{
  char type_[4] = lcm2TRANSFORM;
  int type = *((int*)&type_);

  if(cmm_ptr && !lcm2CMMCheckPointer( cmm_ptr, lcm2TRANSFORM ) &&
     cmm_ptr->ptr)
    *s = (lcm2TransformWrap_s*) cmm_ptr->ptr;

  if(*s && ((*s)->type != type || !(*s)->lcm2))
  {
    *s = 0;
    return 1;
  }

  return 0;
}

/** Function lcm2CMMProfileReleaseWrap
 *  @brief   release a lcm2 profile wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/20
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
int lcm2CMMProfileReleaseWrap(oyPointer *p)
{
  int error = !p;
  lcm2ProfileWrap_s * s = 0;
  
  char type_[4] = lcm2PROFILE;
  int type = *((int*)&type_);
  char s_type[4];

  if(!error && *p)
    s = (lcm2ProfileWrap_s*) *p;

  if(!error)
    error = !s;

  if(!error)
    memcpy(s_type, &s->type, 4);

  if(!error && s->type != type)
    error = 1;

  if(!error)
  {
    CMMProfileRelease_M (s->lcm2);

    s->lcm2 = 0;
    s->type = 0;
    s->size = 0;
    s->block = 0;
    free(s);
  }

  if(!error)
    *p = 0;

  return error;
}


/** Function lcm2CMMDataOpen
 *  @brief   oyCMMProfileOpen_t implementation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/12 (Oyranos: 0.1.8)
 *  @date    2007/12/27
 */
int          lcm2CMMData_Open        ( oyStruct_s        * data,
                                       oyCMMptr_s        * oy )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  if(oy->type != oyOBJECT_CMM_POINTER_S)
    error = 1;

  if(!error)
  {
    char type_[4] = lcm2PROFILE;
    int type = *((int*)&type_);
    size_t size = 0;
    oyPointer block = 0;
    lcm2ProfileWrap_s * s = calloc(sizeof(lcm2ProfileWrap_s), 1);

    if(data->type_ == oyOBJECT_PROFILE_S)
    {
      oyProfile_s * p = (oyProfile_s*)data;
      size = p->size_;
      block = p->block_;
    }

    s->type = type;
    s->size = size;
    s->block = block;

    s->lcm2 = CMMProfileOpen_M( data, block, size );
    oy->ptr = s;
    snprintf( oy->func_name, 32, "%s", CMMToString_M(CMMProfileOpen_M) );
    snprintf( oy->resource, 5, lcm2PROFILE ); 
    error = !oy->ptr;
  }

  if(!error)
  {
    oy->ptrRelease = lcm2CMMProfileReleaseWrap;
  }

  if(!error)
    s = oy;

  if(!error)
    error = !s;

  return error;
}

/** Function lcm2CMMCheckPointer
 *  @brief   
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/12
 *  @since   2007/11/12 (Oyranos: 0.1.8)
 */
int                lcm2CMMCheckPointer(oyCMMptr_s        * cmm_ptr,
                                       const char        * resource )
{
  int error = !cmm_ptr;

  if(cmm_ptr && cmm_ptr->type == oyOBJECT_CMM_POINTER_S &&
     cmm_ptr->ptr && strlen(cmm_ptr->resource))
  {
    int * res_id = (int*)cmm_ptr->resource;

    if(!oyCMMlibMatchesCMM(cmm_ptr->lib_name, CMM_NICK) ||
       *res_id != *((int*)(resource)) )
      error = 1;
  } else {
    error = 1;
  }

  return error;
}



/** Function oyPixelToCMMPixelLayout_
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int        oyPixelToCMMPixelLayout_  ( oyPixel_t           pixel_layout,
                                       icColorSpaceSignature colour_space )
{
  int cmm_pixel = 0;
  int chan_n = oyToChannels_m (pixel_layout);
  int c_off = oyToColourOffset_m (pixel_layout);
  oyDATATYPE_e data_type = oyToDataType_m (pixel_layout);
  int planar = oyToPlanar_m (pixel_layout);
  int flavour = oyToFlavor_m (pixel_layout);
  int cchans = cmsChannelsOf( colour_space );
  int lcm2_colour_space = _cmsLCMScolorSpace( colour_space );
  int extra = chan_n - cchans;

  if(chan_n > CMMMaxChannels_M)
    message( oyMSG_WARN,0, OY_DBG_FORMAT_" "
             "can not handle more than %d channels; found: %d",
             OY_DBG_ARGS_, CMMMaxChannels_M, chan_n);

  cmm_pixel |= CHANNELS_SH(cchans);
  if(extra)
    cmm_pixel |= EXTRA_SH(extra);
  if(c_off == 1)
    cmm_pixel |= SWAPFIRST_SH(1);
  if(data_type == oyUINT8)
    cmm_pixel |= BYTES_SH(1);
  else if(data_type == oyUINT16 || data_type == oyHALF)
    cmm_pixel |= BYTES_SH(2);
  else if(data_type == oyFLOAT)
    cmm_pixel |= BYTES_SH(4);
  if(data_type == oyDOUBLE || data_type == oyFLOAT || data_type == oyHALF)
    cmm_pixel |= FLOAT_SH(1);
  if(oyToSwapColourChannels_m (pixel_layout))
    cmm_pixel |= DOSWAP_SH(1);
  if(oyToByteswap_m(pixel_layout))
    cmm_pixel |= ENDIAN16_SH(1);
  if(planar)
    cmm_pixel |= PLANAR_SH(1);
  if(flavour)
    cmm_pixel |= FLAVOR_SH(1);

  cmm_pixel |= COLORSPACE_SH( lcm2_colour_space );


  return cmm_pixel;
}

/** Function lcm2CMMDeleteTransformWrap
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/00 (Oyranos: 0.1.8)
 *  @date    2007/12/00
 */
int lcm2CMMDeleteTransformWrap(oyPointer * wrap)
{
  
  if(wrap && *wrap)
  {
    lcm2TransformWrap_s * s = (lcm2TransformWrap_s*) *wrap;

    cmsDeleteTransform (s->lcm2);
    s->lcm2 = 0;

    free(s);

    *wrap = 0;

    return 0;
  }

  return 1;
}

/** Function lcm2TransformWrap_Set_
 *  @brief   fill a lcm2TransformWrap_s struct
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/12/21
 */
lcm2TransformWrap_s * lcm2TransformWrap_Set_ (
                                       cmsHTRANSFORM       xform,
                                       icColorSpaceSignature colour_in,
                                       icColorSpaceSignature colour_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyCMMptr_s        * oy )
{
  int error = !xform;
  lcm2TransformWrap_s * s = 0;
  
  if(!error)
  {
    char type_[4] = lcm2TRANSFORM;
    int type = *((int*)&type_);
    lcm2TransformWrap_s * ltw = calloc(sizeof(lcm2TransformWrap_s), 1);

    ltw->type = type;

    ltw->lcm2 = xform; xform = 0;

    ltw->sig_in  = colour_in;
    ltw->sig_out = colour_out;
    ltw->oy_pixel_layout_in  = oy_pixel_layout_in;
    ltw->oy_pixel_layout_out = oy_pixel_layout_out;
    s = ltw;
  }

  if(!error)
  {
    oy->ptr = s;

    oy->ptrRelease = lcm2CMMDeleteTransformWrap;

    strcpy( oy->func_name, "lcm2CMMDeleteTransformWrap" );
  }

  return s;
}

/** Function lcm2CMMConversionContextCreate_
 *  @brief   create a CMM transform
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2009/11/04
 */
cmsHTRANSFORM  lcm2CMMConversionContextCreate_ (
                                       cmsHPROFILE       * lps,
                                       int                 profiles_n,
                                       oyProfiles_s      * simulation,
                                       int                 proof_n,
                                       int                 proof,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s       * opts,
                                       lcm2TransformWrap_s ** ltw,
                                       oyCMMptr_s        * oy )
{
  oyPixel_t lcm2_pixel_layout_in = 0;
  oyPixel_t lcm2_pixel_layout_out = 0;
  int error = !lps;
  cmsHTRANSFORM xform = 0;
  cmsHPROFILE * merge = 0;
  icColorSpaceSignature colour_in = 0;
  icColorSpaceSignature colour_out = 0;
  icProfileClassSignature profile_class_in = 0;
  int intent = 0,
      intent_proof = 0,
      bpc = 0,
      cmyk_cmyk_black_preservation = 0,
      gamut_warning = 0,
      precalculation = 0,
      flags = cmsFLAGS_NOCACHE;
  const char * o_txt = 0;

  if(!lps || !profiles_n || !oy_pixel_layout_in || !oy_pixel_layout_out)
    return 0;

  
  if(!error && lps[0] && lps[profiles_n-1])
  {
    colour_in = cmsGetColorSpace( lps[0] );
    if(profiles_n > 1)
      colour_out = cmsGetColorSpace( lps[profiles_n-1] );
    else
      colour_out = cmsGetPCS( lps[profiles_n-1] );
    profile_class_in = cmsGetDeviceClass( lps[0] );
  }

  lcm2_pixel_layout_in  = oyPixelToCMMPixelLayout_(oy_pixel_layout_in,
                                                   colour_in);
  lcm2_pixel_layout_out = oyPixelToCMMPixelLayout_(oy_pixel_layout_out,
                                                   colour_out);

#ifndef oyStrlen_
#define oyStrlen_ strlen
#endif
      o_txt = oyOptions_FindString  ( opts, "rendering_intent", 0);
      if(o_txt && oyStrlen_(o_txt))
        intent = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_intent_proof", 0);
      if(o_txt && oyStrlen_(o_txt))
        intent_proof = atoi( o_txt );

      intent_proof = intent_proof == 0 ? INTENT_RELATIVE_COLORIMETRIC :
                                         INTENT_ABSOLUTE_COLORIMETRIC;

      o_txt = oyOptions_FindString  ( opts, "rendering_bpc", 0 );
      if(o_txt && oyStrlen_(o_txt))
        bpc = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_gamut_warning", 0 );
      if(o_txt && oyStrlen_(o_txt))
        gamut_warning = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "precalculation", 0 );
      if(o_txt && oyStrlen_(o_txt))
        precalculation = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "cmyk_cmyk_black_preservation", 0 );
      if(o_txt && oyStrlen_(o_txt))
        cmyk_cmyk_black_preservation = atoi( o_txt );

      /* this should be moved to the CMM and not be handled here in Oyranos */
      flags = proof ?         flags | cmsFLAGS_SOFTPROOFING :
                              flags & (~cmsFLAGS_SOFTPROOFING);
      flags = bpc ?           flags | cmsFLAGS_BLACKPOINTCOMPENSATION :
                              flags & (~cmsFLAGS_BLACKPOINTCOMPENSATION);
      flags = gamut_warning ? flags | cmsFLAGS_GAMUTCHECK :
                              flags & (~cmsFLAGS_GAMUTCHECK);
      switch(precalculation)
      {
      case 0: flags |= cmsFLAGS_NOOPTIMIZE; break;
      case 1: flags |= 0; break;
      case 2: flags |= cmsFLAGS_HIGHRESPRECALC; break;
      case 3: flags |= cmsFLAGS_LOWRESPRECALC; break;
      }
      intent = cmyk_cmyk_black_preservation ? intent + 10 : intent;
      if(cmyk_cmyk_black_preservation == 2)
        intent += 13;

  if(oy_debug)
    message( oyMSG_WARN,0, OY_DBG_FORMAT_"\n"
    "  intent: %d proof: %d  bpc: %d  gamut_warning: %d  precalculation: %d\n"
             "  profiles_n: %d,  flags: %d(%s)",
             OY_DBG_ARGS_,
       intent,    proof_n,   bpc,     gamut_warning,     precalculation,
                profiles_n,      flags,    lcm2FlagsToText(flags) );

  if(!error)
  {
         if(profiles_n == 1 || profile_class_in == icSigLinkClass)
    {
        /* we have to erase the colour space */
#if 1
        int csp = T_COLORSPACE(lcm2_pixel_layout_in);
        lcm2_pixel_layout_in &= (~COLORSPACE_SH( csp ));
        csp = T_COLORSPACE(lcm2_pixel_layout_out);
        lcm2_pixel_layout_out &= (~COLORSPACE_SH( csp ));
#endif
        xform = cmsCreateTransform( lps[0], lcm2_pixel_layout_in,
                                    0, lcm2_pixel_layout_out,
                                    intent, flags );
    }
    else if(profiles_n == 2 && (!proof_n || (!proof && !gamut_warning)))
        xform = cmsCreateTransform( lps[0], lcm2_pixel_layout_in,
                                    lps[1], lcm2_pixel_layout_out,
                                    intent, flags );
    else
    {
      int multi_profiles_n = profiles_n;
      int i;

      if(proof_n && (proof || gamut_warning))
      {
        int len = sizeof(cmsHPROFILE) * (profiles_n + proof_n);

        oyAllocHelper_m_( merge, cmsHPROFILE, profiles_n + proof_n,0, goto end);
        memset( merge, 0, len );
        memcpy( merge, lps, sizeof(cmsHPROFILE) * (profiles_n - 1) );

        for(i = 0; i < proof_n; ++i)
          merge[profiles_n-1 + i] = lcm2AddProofProfile( 
                                             oyProfiles_Get(simulation,i),flags,
                                             intent, intent_proof);

        merge[profiles_n + proof_n -1] = lps[profiles_n - 1];

        /* merge effect and simulation profiles */
        multi_profiles_n += proof_n;
        lps = merge;
      }

      if(flags & cmsFLAGS_GAMUTCHECK)
        flags |= cmsFLAGS_GRIDPOINTS(lcm2PROOF_LUT_GRID_RASTER);

    if(oy_debug)
    {
    uint32_t f = lcm2_pixel_layout_in;
    printf ("%s:%d %s() float:%d optimised:%d colourspace:%d extra:%d channels:%d lcms_bytes %d \n", __FILE__,__LINE__,__func__, T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
    f = lcm2_pixel_layout_out;
    printf ("%s:%d %s() float:%d optimised:%d colourspace:%d extra:%d channels:%d lcms_bytes %d \n", __FILE__,__LINE__,__func__, T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
      printf("multi_profiles_n: %d intent: %d flags: %d \"%s\" l1 %d, l2 %d\n",
              multi_profiles_n, intent, flags, lcm2FlagsToText(flags),
              lcm2_pixel_layout_in, lcm2_pixel_layout_out);
    }
      xform =   cmsCreateMultiprofileTransform(
                                    lps, 
                                    multi_profiles_n,
                                    lcm2_pixel_layout_in,
                                    lcm2_pixel_layout_out,
                                    intent, flags );


      if(merge) oyDeAllocateFunc_( merge ); merge = 0;
    }
  }

  if(!xform)
  {
    uint32_t f = lcm2_pixel_layout_in;
    printf ("%s:%d %s() float:%d optimised:%d colourspace:%d extra:%d channels:%d lcms_bytes %d \n", __FILE__,__LINE__,__func__, T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
    f = lcm2_pixel_layout_out;
    printf ("%s:%d %s() float:%d optimised:%d colourspace:%d extra:%d channels:%d lcms_bytes %d \n", __FILE__,__LINE__,__func__, T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
    error = 1;
  }

  if(!error && ltw && oy)
    *ltw= lcm2TransformWrap_Set_( xform, colour_in, colour_out,
                                  oy_pixel_layout_in, oy_pixel_layout_out, oy );

  end:
  return xform;
}

/** Function lcm2CMMColourConversion_ToMem_
 *
 *  convert a lcm2 colour conversion context to a device link
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyPointer  lcm2CMMColourConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !xform;
  oyPointer data = 0;
  cmsUInt32Number size_ = 0;

  if(!error)
  {
    cmsHPROFILE dl= cmsTransform2DeviceLink( xform,4.2,cmsFLAGS_KEEP_SEQUENCE );

    *size = 0;

#if 0
    {
        int nargs = 1, i;
        size_t size = sizeof(int) + nargs * sizeof(cmsPSEQDESC);
        LPcmsSEQ pseq = (LPcmsSEQ) oyAllocateFunc_(size);
        
        ZeroMemory(pseq, size);
        pseq ->n = nargs;

        for (i=0; i < nargs; i++) {

            strcpy(pseq ->seq[i].Manufacturer, CMM_NICK);
            strcpy(pseq ->seq[i].Model, "CMM ");
        }

        cmsAddTag(dl, icSigProfileSequenceDescTag, pseq);
        free(pseq);
    }
#endif

    cmsSaveProfileToMem( dl, 0, &size_ );
    if(size_)
    {
      data = allocateFunc( size_ );
      cmsSaveProfileToMem( dl, data, &size_ );
    }
    *size = size_;
  }

  return data;
}

oyOptions_s* lcm2Filter_CmmIccValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = oyFilterRegistrationMatch(filter->registration_, "//imaging/icc",
                                      oyOBJECT_CMM_API4_S);

  *result = error;

  return 0;
}

oyWIDGET_EVENT_e   lcm2WidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}


oyDATATYPE_e lcm2_cmmIcc_data_types[7] = {oyUINT8, oyUINT16, oyFLOAT, oyDOUBLE, 0};

oyConnectorImaging_s lcm2_cmmIccSocket_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Socket"},
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  lcm2_cmmIcc_data_types, /* data_types */
  3, /* data_types_n; elements in data_types array */
  1, /* max_colour_offset */
  1, /* min_channels_count; */
  16, /* max_channels_count; */
  1, /* min_colour_count; */
  16, /* max_colour_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* channel_types_n */
  1, /* id; relative to oyFilterCore_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s* lcm2_cmmIccSocket_connectors[2]={&lcm2_cmmIccSocket_connector,0};

oyConnectorImaging_s lcm2_cmmIccPlug_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Plug"},
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  lcm2_cmmIcc_data_types, /* data_types */
  3, /* data_types_n; elements in data_types array */
  1, /* max_colour_offset */
  1, /* min_channels_count; */
  16, /* max_channels_count; */
  1, /* min_colour_count; */
  16, /* max_colour_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* channel_types_n */
  1, /* id; relative to oyFilterCore_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s* lcm2_cmmIccPlug_connectors[2]={&lcm2_cmmIccPlug_connector,0};

/** Function lcm2AddProofProfile
 *  @brief   add a abstract proofing profile to the lcm2 profile stack 
 *
 *  Look in the Oyranos cache for a CMM internal representation or generate a
 *  new abstract profile containing the proofing profiles changes. This can be
 *  a proofing colour space simulation or out of gamut marking.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/05 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
cmsHPROFILE  lcm2AddProofProfile     ( oyProfile_s       * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof )
{
  int error = 0;
  cmsHPROFILE * hp = 0;
  oyCMMptr_s * cmm_ptr = 0;
  lcm2ProfileWrap_s * s = 0;
  char * hash_text = 0,
       num[12];

  if(!proof || proof->type_ != oyOBJECT_PROFILE_S)
  {
    message( oyMSG_WARN, (oyStruct_s*)proof, OY_DBG_FORMAT_" "
             "no profile provided", OY_DBG_ARGS_ );
    return 0;
  }

  /* build hash text */
  STRING_ADD( hash_text, "abstract proofing profile " );
  STRING_ADD( hash_text, oyObject_GetName( proof->oy_, oyNAME_NICK ) );
  STRING_ADD( hash_text, " intent:" );
  sprintf( num, "%d", intent );
  STRING_ADD( hash_text, num );
  STRING_ADD( hash_text, " intent_proof:" );
  sprintf( num, "%d", intent_proof );
  STRING_ADD( hash_text, num );
  STRING_ADD( hash_text, " flags|gmtCheck|softPrf:" );
  sprintf( num, "%d|%d|%d", (int)flags, (flags & cmsFLAGS_GAMUTCHECK)?1:0,
                                        (flags & cmsFLAGS_SOFTPROOFING)?1:0 );
  STRING_ADD( hash_text, num );

  /* cache look up */
  cmm_ptr = oyCMMptrLookUpFromText( hash_text, lcm2PROFILE );

  cmm_ptr->lib_name = CMM_NICK;

  /* for empty profile create a new abstract one */
  if(!cmm_ptr->ptr)
  {
    oyCMMptr_s * oy = cmm_ptr;

    char type_[4] = lcm2PROFILE;
    uint32_t type = *((uint32_t*)&type_);
    cmsUInt32Number size = 0;
    oyPointer block = 0;
    lcm2ProfileWrap_s * s = calloc(sizeof(lcm2ProfileWrap_s), 1);

    if(oy_debug == 1)
      fprintf( stderr, OY_DBG_FORMAT_" created: \"%s\"",
               OY_DBG_ARGS_, hash_text );
    else
    message( oyMSG_DBG, (oyStruct_s*)proof,
             OY_DBG_FORMAT_" created abstract proofing profile: \"%s\"",
             OY_DBG_ARGS_, hash_text );
 
    /* create */
    hp = lcm2GamutCheckAbstract( proof, flags, intent, intent_proof );
    if(hp)
    {
      /* save to memory */
      cmsSaveProfileToMem( hp, 0, &size );
      block = oyAllocateFunc_( size );
      cmsSaveProfileToMem( hp, block, &size );
      cmsCloseProfile( hp ); hp = 0;
    }

    s->type = type;
    s->size = size;
    s->block = block;

    /* reopen */
    s->lcm2 = CMMProfileOpen_M( proof, block, size );
    oy->ptr = s;
    snprintf( oy->func_name, 32, "%s", CMMToString_M(CMMProfileOpen_M) );
    snprintf( oy->resource, 5, lcm2PROFILE ); 
    error = !oy->ptr;

    if(!error)
    {
      oy->ptrRelease = lcm2CMMProfileReleaseWrap;
    }
  }

  if(!error)
  {
    s = lcm2CMMProfile_GetWrap_( cmm_ptr );
    error = !s;
  }

  if(!error)
    hp = s->lcm2;

  oyCMMptr_Release( &cmm_ptr );
  if(hash_text)
    oyFree_m_(hash_text);

  if(!error)
    return hp;
  else
    return 0;
}


/** Function lcm2AddProfile
 *  @brief   add a profile from Oyranos to the lcm2 profile stack 
 *
 *  Look in the Oyranos cache for a CMM internal representation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
cmsHPROFILE  lcm2AddProfile          ( oyProfile_s       * p )
{
  int error = 0;
  cmsHPROFILE * hp = 0;
  oyCMMptr_s * cmm_ptr = 0;
  lcm2ProfileWrap_s * s = 0;

  if(!p || p->type_ != oyOBJECT_PROFILE_S)
  {
    message( oyMSG_WARN, (oyStruct_s*)p, OY_DBG_FORMAT_" "
             "no profile provided", OY_DBG_ARGS_ );
    return 0;
  }

  cmm_ptr = oyCMMptrLookUpFromObject( (oyStruct_s*)p, lcm2PROFILE );

  if(!cmm_ptr)
  {
    message( oyMSG_WARN, (oyStruct_s*)p,
             OY_DBG_FORMAT_" oyCMMptrLookUpFromObject() failed", OY_DBG_ARGS_ );
    return 0;
  }

  cmm_ptr->lib_name = CMM_NICK;

  if(!cmm_ptr->ptr)
    error = lcm2CMMData_Open( (oyStruct_s*)p, cmm_ptr );

  if(!error)
  {
    s = lcm2CMMProfile_GetWrap_( cmm_ptr );
    error = !s;
  }

  if(!error)
    hp = s->lcm2;

  oyCMMptr_Release( &cmm_ptr );

  if(!error)
    return hp;
  else
    return 0;
}

int gamutCheckSampler16(const cmsUInt16Number In[],
                              cmsUInt16Number Out[],
                        void                * Cargo)
{
  cmsCIELab Lab1, Lab2;
  double d;
  oyPointer * ptr = (oyPointer*)Cargo;

  cmsLabEncoded2Float(&Lab1, In);
  cmsDoTransform( ptr[0], In, Out, 1 );
  cmsLabEncoded2Float(&Lab2, Out);
  d = cmsDeltaE( &Lab1, &Lab2 );
  /*if(abs(d) > 10 && ptr[1] != NULL)
  {
    Lab2.L = 50.0;
    Lab2.a = Lab2.b = 0.0;
  }*/
  cmsFloat2LabEncoded(Out, &Lab2); 

  return TRUE;
}

int  gamutCheckSamplerFloat          ( const cmsFloat32Number In[],
                                             cmsFloat32Number Out[],
                                       void                 * Cargo )
{
  cmsCIELab Lab1, Lab2;
  double d;
  cmsFloat32Number i[3], o[3];
  oyPointer * ptr = (oyPointer*)Cargo;

  i[0] = Lab1.L = In[0] * 100.0;
  i[1] = Lab1.a = In[1] * 257.0 - 128.0;
  i[2] = Lab1.b = In[2] * 257.0 - 128.0;

  cmsDoTransform( ptr[0], i, o, 1 );

  Lab2.L = o[0]; Lab2.a = o[1]; Lab2.b = o[2];

  d = cmsDeltaE( &Lab1, &Lab2 );
  if((abs(d) > 10) && ptr[1] != NULL)
  {
    Lab2.L = 50.0;
    Lab2.a = Lab2.b = 0.0;
  }

  Out[0] = Lab2.L/100.0; 
  Out[1] = (Lab2.a + 128.0) / 257.0;
  Out[2] = (Lab2.b + 128.0) / 257.0;

  return TRUE;
}

/** Function lcm2GamutCheckAbstract
 *  @brief   convert a proofing profile into a abstract one
 *
 *  Abstract profiles can easily be merged into a multi profile transform.
 *
 *  @param         proof               the proofing profile; owned by the
 *                                     function
 *  @param         flags               the gamut check and softproof flags
 *  @param         intent              rendering intent
 *  @param         intent_proof        proof rendering intent
 *
 *  @version Oyranos: 0.1.11
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 *  @date    2010/08/14
 */
cmsHPROFILE  lcm2GamutCheckAbstract  ( oyProfile_s       * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof )
{
      cmsUInt32Number size = 0;
      char * data = 0;
      cmsHPROFILE gmt = 0,
                  hLab = 0,
                  hproof = 0;
      cmsHTRANSFORM tr = 0, tr16 = 0;
      cmsStage * gmt_lut = 0,
               * gmt_lut16 = 0;
      cmsPipeline * gmt_pl = cmsPipelineAlloc( 0,3,3 ),
                  * gmt_pl16 = cmsPipelineAlloc( 0,3,3 );
      cmsToneCurve * t[3] = {0,0,0},
                   * g[3] = {0,0,0};

      oyPointer ptr[2] = {0,0},
                ptr16[2] = {0,0};
      int r = 0, i,
          error = 0;
      cmsMLU * mlu[2] = {0,0};
      cmsCurveSegment seg[2];
      oyOption_s * id = oyOption_New( OY_TOP_SHARED OY_SLASH 
                              OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
                                      "gmt_pl.TYPE_Lab_FLT." CMM_NICK, 0);
      oyOption_s * id16 = oyOption_New( OY_TOP_SHARED OY_SLASH 
                               OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
                                        "gmt_pl.TYPE_Lab_16." CMM_NICK, 0);

      if(!(flags & cmsFLAGS_GAMUTCHECK || flags & cmsFLAGS_SOFTPROOFING))
        return gmt;

      hLab  = cmsCreateLab4Profile(cmsD50_xyY());
      hproof = lcm2AddProfile( proof );
      if(!hLab || !hproof)
      { message( oyMSG_ERROR, (oyStruct_s*)proof, OY_DBG_FORMAT_
                 "hLab or hproof failed", OY_DBG_ARGS_);
                 goto clean; }

#if 0
#pragma omp parallel for
#endif
  for(i = 0; i < 2; ++i)
  {
    if(i)
    {
      tr = cmsCreateProofingTransformTHR ( id, hLab, TYPE_Lab_FLT,
                                               hLab, TYPE_Lab_FLT,
                                               hproof,
                                               intent,
            /* TODO The INTENT_ABSOLUTE_COLORIMETRIC should lead to 
               paper simulation, but does take white point into account.
               Do we want this?
             */
                                               intent_proof,
                                               flags | cmsFLAGS_KEEP_SEQUENCE);
      if(!tr) { message( oyMSG_ERROR, (oyStruct_s*)proof, OY_DBG_FORMAT_
                          "cmsCreateProofingTransform() failed", OY_DBG_ARGS_);
                error = 1; }
      ptr[0] = tr;
      ptr[1] = flags & cmsFLAGS_GAMUTCHECK ? (oyPointer)1 : 0;
      if(!error)
      {
        gmt_lut = cmsStageAllocCLutFloat( 0,lcm2PROOF_LUT_GRID_RASTER, 3,3, 0);
        r = cmsStageSampleCLutFloat( gmt_lut, gamutCheckSamplerFloat, ptr, 0 );
        if(!r) { message( oyMSG_ERROR, (oyStruct_s*)proof, OY_DBG_FORMAT_
                          "cmsStageSampleCLutFloat() failed", OY_DBG_ARGS_);
                 error = 1; }
      }

    } else
    {
      tr16 = cmsCreateProofingTransformTHR( id16, hLab, TYPE_Lab_16,
                                               hLab, TYPE_Lab_16,
                                               hproof,
                                               intent,
            /* TODO The INTENT_ABSOLUTE_COLORIMETRIC should lead to 
               paper simulation, but does take white point into account.
               Do we want this?
             */
                                               intent_proof,
                                               flags | cmsFLAGS_KEEP_SEQUENCE);
      if(!tr16) { message( oyMSG_ERROR, (oyStruct_s*)proof, OY_DBG_FORMAT_
                          "cmsCreateProofingTransform() failed", OY_DBG_ARGS_);
                  error = 1; }
      ptr16[0] = tr16;
      ptr16[1] = flags & cmsFLAGS_GAMUTCHECK ? (oyPointer)1 : 0;

      if(!error)
      {
        gmt_lut16 = cmsStageAllocCLut16bit( 0, lcm2PROOF_LUT_GRID_RASTER,3,3,0);
        r = cmsStageSampleCLut16bit( gmt_lut16, gamutCheckSampler16, ptr16, 0);
        if(!r)   { message( oyMSG_ERROR, (oyStruct_s*)proof, OY_DBG_FORMAT_
                          "cmsStageSampleCLut16bit() failed", OY_DBG_ARGS_);
                   error = 1; }
      }
    }
  }

      if(error) goto clean;

      gmt = cmsCreateProfilePlaceholder( 0 ); if(!gmt) goto clean;
      cmsSetProfileVersion( gmt, 4.2 );
      cmsSetDeviceClass( gmt, icSigAbstractClass );
      cmsSetColorSpace( gmt, icSigLabData );
      cmsSetPCS( gmt, icSigLabData );
#define E if(!r) { message( oyMSG_ERROR, (oyStruct_s*)proof, \
                   OY_DBG_FORMAT_ "could not write tag", OY_DBG_ARGS_); \
                   if(gmt) cmsCloseProfile( gmt ); gmt = 0; \
                   goto clean; }
      mlu[0] = cmsMLUalloc(0,1);
      mlu[1] = cmsMLUalloc(0,1);
      r = cmsMLUsetASCII(mlu[0], "EN", "us", "proofing"); E
      r = cmsWriteTag( gmt, icSigProfileDescriptionTag, mlu[0] ); E
      r = cmsMLUsetASCII(mlu[1], "EN", "us", "no copyright; use freely"); E
      r = cmsWriteTag( gmt, icSigCopyrightTag, mlu[1]); E
      r = cmsWriteTag( gmt, icSigMediaWhitePointTag, cmsD50_XYZ() ); E

      /* Initialize segmented curve
         Segment 0: from minus infinite */
      size =  sizeof(seg);
      memset( seg, 0, size );
      seg[0].x0 = -1.0;
      seg[0].x1 = 1.0;
      seg[0].Type = 6;

      seg[0].Params[0] = 1;
      seg[0].Params[1] = 1;
      seg[0].Params[2] = 0;
      seg[0].Params[3] = 0;
      seg[0].Params[4] = 0;

      t[0] = t[1] = t[2] = cmsBuildSegmentedToneCurve(0, 1, seg);
      /* float */
      /* cmsPipeline owns the cmsStage memory */
      cmsPipelineInsertStage( gmt_pl, cmsAT_BEGIN,
                              cmsStageAllocToneCurves( 0, 3, t ) );
      cmsPipelineInsertStage( gmt_pl, cmsAT_END, gmt_lut );
      cmsPipelineInsertStage( gmt_pl, cmsAT_END,
                              cmsStageAllocToneCurves( 0, 3, t ) );
      r = cmsWriteTag( gmt, cmsSigDToB0Tag, gmt_pl ); E

      /* 16-bit int */
      g[0] = g[1] = g[2] = cmsBuildGamma(0, 1.0);
      cmsPipelineInsertStage( gmt_pl16, cmsAT_BEGIN,
                              cmsStageAllocToneCurves( 0, 3, g ) );
      cmsPipelineInsertStage( gmt_pl16, cmsAT_END, gmt_lut16 );
      cmsPipelineInsertStage( gmt_pl16, cmsAT_END,
                              cmsStageAllocToneCurves( 0, 3, g ) );
      r = cmsWriteTag( gmt, cmsSigAToB0Tag, gmt_pl16 ); E
#undef E

  if(oy_debug)
  {
      cmsSaveProfileToMem( gmt, 0, &size );
      data = oyAllocateFunc_( size );
      cmsSaveProfileToMem( gmt, data, &size );
      oyWriteMemToFile_( "dbg_abstract_proof.icc", data, size );
      if(data) oyDeAllocateFunc_( data ); data = 0;
  }

  cmsUInt16Number OldAlarm[cmsMAXCHANNELS];
  cmsGetAlarmCodes(OldAlarm);

  clean:

      if(hLab) cmsCloseProfile( hLab ); hLab = 0;
      if(tr) cmsDeleteTransform( tr ); tr = 0;
      if(tr16) cmsDeleteTransform( tr16 ); tr16 = 0;
      if(t[0])  cmsFreeToneCurve( t[0] );
      if(g[0])  cmsFreeToneCurve( g[0] );
      if(gmt_pl) cmsPipelineFree( gmt_pl );
      if(gmt_pl16) cmsPipelineFree( gmt_pl16 );
      if(mlu[0]) cmsMLUfree( mlu[0] ); mlu[0] = 0;
      if(mlu[1]) cmsMLUfree( mlu[1] ); mlu[1] = 0;

  oyProfile_Release( &proof );
  oyOption_Release( &id );
  oyOption_Release( &id16 );

  return gmt;
}

/** Function lcm2FilterNode_CmmIccContextToMem
 *  @brief   implement oyCMMFilterNode_CreateContext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
oyPointer lcm2FilterNode_CmmIccContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  /*int error = !node || !size;*/
  oyPointer block = 0;
  int error = 0;
  int channels = 0;
  int n,i,len;
  oyDATATYPE_e data_type = 0;
  size_t size_ = 0;
  oyFilterSocket_s * socket = (oyFilterSocket_s *)node->sockets[0];
  oyFilterPlug_s * plug = (oyFilterPlug_s *)node->plugs[0];
  oyFilterCore_s * filter = 0;
  oyFilterNode_s * input_node = 0;
  oyImage_s * image_input = 0,
            * image_output = 0;
  cmsHPROFILE * lps = 0;
  cmsHTRANSFORM xform = 0;
  oyOption_s * o = 0;
  oyProfile_s * p = 0,
              * prof = 0;
  oyProfiles_s * profiles = 0,
               * profs = 0;
  oyProfileTag_s * psid = 0,
                 * info = 0,
                 * cprt = 0;
  int profiles_n = 0,
      profiles_proof_n = 0,
      proof = 0;
  int verbose = oyOptions_FindString( node->tags, "verbose", "true" ) ? 1 : 0;
  const char * o_txt = 0;

  filter = node->core;
  input_node = plug->remote_socket_->node;
  image_input = (oyImage_s*)plug->remote_socket_->data;
  image_output = (oyImage_s*)socket->data;

  if(!image_input)
    return 0;

  if(image_input->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" missed input image %d", OY_DBG_ARGS_,
             image_input->type_ );
  }
  if(image_output->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" missed output image %d", OY_DBG_ARGS_, image_input->type_ );
  }

  data_type = oyToDataType_m( image_input->layout_[0] );

  if(data_type == oyHALF)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" can not handle oyHALF", OY_DBG_ARGS_ );
  }

  channels = oyToChannels_m( image_input->layout_[0] );

  len = sizeof(cmsHPROFILE) * (15 + 2 + 1);
  lps = oyAllocateFunc_( len );
  memset( lps, 0, len );

  /* input profile */
  lps[ profiles_n++ ] = lcm2AddProfile( image_input->profile_ );
  if(!image_input->profile_)
  {
    message( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_" "
             "missed image_input->profile_", OY_DBG_ARGS_ );
    return 0;
  }
  p = oyProfile_Copy( image_input->profile_, 0 );
  profs = oyProfiles_MoveIn( profs, &p, -1 );

  /* effect profiles */
  o = oyOptions_Find( node->core->options_, "profiles_effect" );
  if(o)
  {
    if( o->value_type != oyVAL_STRUCT ||
        !o->value->oy_struct ||
        o->value->oy_struct->type_ != oyOBJECT_PROFILES_S)
    {
      oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION );
      message( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_
               " incompatible \"profiles_effect\"", OY_DBG_ARGS_ );
      
    } else
    {
      profiles = (oyProfiles_s*) o->value;
      n = oyProfiles_Count( profiles );
      for(i = 0; i < n; ++i)
      {
        p = oyProfiles_Get( profiles, i );

        /* Look in the Oyranos cache for a CMM internal representation */
        lps[ profiles_n++ ] = lcm2AddProfile( p );
        profs = oyProfiles_MoveIn( profs, &p, -1 );
      }
    }
    oyOption_Release( &o );
  }

  /* simulation profile */
  o = oyOptions_Find( node->core->options_, "profiles_simulation" );
  o_txt = oyOptions_FindString  ( node->core->options_, "proof_soft", 0 );
  if(o_txt && oyStrlen_(o_txt)/* && profile_class_out== icSigDisplayClass*/)
    proof = atoi( o_txt );

  o_txt = oyOptions_FindString  ( node->core->options_, "proof_hard", 0 );
  if(o_txt && oyStrlen_(o_txt)/* && profile_class_out== icSigOutputClass*/)
    proof = atoi( o_txt ) ? atoi(o_txt) : proof;

  if(oy_debug && proof)
      message( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_
               " proof requested",OY_DBG_ARGS_);

  if(o)
  {
    if( o->value_type != oyVAL_STRUCT ||
        o->value->oy_struct->type_ != oyOBJECT_PROFILES_S)
    {
      oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION );
      message( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_
               " incompatible \"profiles_simulation\"",OY_DBG_ARGS_);
      
    } else
    {
      profiles = (oyProfiles_s*) o->value->oy_struct;
      n = oyProfiles_Count( profiles );

      message( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
               " %d simulation profile(s) found \"%s\"",
               OY_DBG_ARGS_, n,
               profiles?oyStruct_TypeToText((oyStruct_s*)profiles):"????");

      for(i = 0; i < n; ++i)
      {
        p = oyProfiles_Get( profiles, i );

        message( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
                 " found profile: %s",
                 OY_DBG_ARGS_, p?oyProfile_GetFileName( p,-1 ):"????");

        profs = oyProfiles_MoveIn( profs, &p, -1 );
        ++profiles_proof_n;

        oyProfile_Release( &p );
      }
    }
    oyOption_Release( &o );
  } else if(verbose || oy_debug)
    message( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
             " no simulation profile found", OY_DBG_ARGS_);


  /* output profile */
  if(!image_output->profile_)
  {
    message( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_" "
             "missed image_output->profile_", OY_DBG_ARGS_ );
    return 0;
  }
  lps[ profiles_n++ ] = lcm2AddProfile( image_output->profile_ );
  p = oyProfile_Copy( image_output->profile_, 0 );
  profs = oyProfiles_MoveIn( profs, &p, -1 );

  *size = 0;

  /* create the context */
  xform = lcm2CMMConversionContextCreate_( lps, profiles_n,
                                           profiles, profiles_proof_n, proof,
                                           image_input->layout_[0],
                                           image_output->layout_[0],
                                           node->core->options_, 0, 0);
  error = !xform;

  if(!error)
  {
    if(oy_debug)
      block = lcm2CMMColourConversion_ToMem_( xform, size, oyAllocateFunc_ );
    else
      block = lcm2CMMColourConversion_ToMem_( xform, size, allocateFunc );
    error = !block || !*size;
    cmsDeleteTransform( xform ); xform = 0;
  }

  /* additional tags for debugging */
  if(!error && (oy_debug || verbose))
  {
    if(!error && size)
    {
      size_ = *size;

      if(!size_)
        return NULL;

      prof = oyProfile_FromMem( size_, block, 0, 0 );
      psid = oyProfile_GetTagById( prof, icSigProfileSequenceIdentifierTag );

      /* icSigProfileSequenceIdentifierType */
      if(!psid)
      {
        psid = oyProfileTag_Create( profs->list_,
                     icSigProfileSequenceIdentifierType, 0, OY_MODULE_NICK, 0 );

        if(psid)
          error = oyProfile_TagMoveIn ( prof, &psid, -1 );
      }

      /* Info tag */
      if(!error)
      {
        oyStructList_s * list = 0;
        char h[5] = {"Info"};
        uint32_t * hi = (uint32_t*)&h;
        char * cc_name = lcm2FilterNode_GetText( node, oyNAME_NICK,
                                                 oyAllocateFunc_ );
        oyName_s * name = oyName_new(0);
        const char * lib_name = node->core->api4_->id_;

        name = oyName_set_ ( name, cc_name, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        name = oyName_set_ ( name, lib_name, oyNAME_NICK,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        oyDeAllocateFunc_( cc_name );
        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list,  (oyStruct_s**) &name, 0, 0 );

        if(!error)
        {
          info = oyProfileTag_Create( list, icSigTextType, 0,OY_MODULE_NICK, 0);
          error = !info;
        }

        if(!error)
          info->use = (icTagSignature)oyValueUInt32(*hi);

        oyStructList_Release( &list );

        if(info)
          error = oyProfile_TagMoveIn ( prof, &info, -1 );
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
        error = oyStructList_MoveIn( list, (oyStruct_s**) &name, 0, 0 );

        if(!error)
        {
          cprt = oyProfileTag_Create( list, icSigTextType, 0,OY_MODULE_NICK, 0);
          error = !cprt;
        }

        if(!error)
          cprt->use = icSigCopyrightTag;

        oyStructList_Release( &list );

        if(cprt)
          error = oyProfile_TagMoveIn ( prof, &cprt, -1 );
      }

      if(block)
        oyDeAllocateFunc_( block ); block = 0; size_ = 0;

      block = oyProfile_GetMem( prof, &size_, 0, allocateFunc );

      *size = size_;
      oyProfile_Release( &prof );
    }
  }

  return block;
}

char * lcm2Image_GetText             ( oyImage_s         * image,
                                       int                 verbose,
                                       oyAlloc_f           allocateFunc )
{
  oyPixel_t pixel_layout = image->layout_[oyLAYOUT]; 
  int n     = oyToChannels_m( pixel_layout );
  oyProfile_s * profile = image->profile_;
  int cchan_n = oyProfile_GetChannelsCount( profile );
  int coff_x = oyToColourOffset_m( pixel_layout );
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  int swap  = oyToSwapColourChannels_m( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  int so = oySizeofDatatype( t );
  char * text = oyAllocateFunc_(512);
  char * hash_text = 0;
  oyImage_s * s = image;

  /* describe the image */
  oySprintf_( text,   "  <oyImage_s\n");
  hashTextAdd_m( text );
  if(oy_debug || verbose)
    oySprintf_( text, "    profile=\"%s\"\n", oyProfile_GetText(profile,
                                                                oyNAME_NAME));
  else
    oySprintf_( text, "    %s\n", oyProfile_GetText(profile, oyNAME_NICK));
  hashTextAdd_m( text );
  oySprintf_( text,   "    <channels all=\"%d\" colour=\"%d\" />\n", n,cchan_n);
  hashTextAdd_m( text );
  oySprintf_( text,
                      "    <offsets first_colour_sample=\"%d\" next_pixel=\"%d\" />\n"
              /*"  next line = %d\n"*/,
              coff_x, s->layout_[oyPOFF_X]/*, mask[oyPOFF_Y]*/ );
  hashTextAdd_m( text );

  if(swap || oyToByteswap_m( pixel_layout ))
  {
    hashTextAdd_m(    "    <swap" );
    if(swap)
      hashTextAdd_m(  " colourswap=\"yes\"" );
    if( oyToByteswap_m( pixel_layout ) )
      hashTextAdd_m(  " byteswap=\"yes\"" );
    hashTextAdd_m(    " />\n" );
  }

  if( oyToFlavor_m( pixel_layout ) )
  {
    oySprintf_( text, "    <flawor value=\"yes\" />\n" );
    hashTextAdd_m( text );
  }
  oySprintf_( text,   "    <sample_type value=\"%s[%dByte]\" />\n",
                    oyDatatypeToText(t), so );
  hashTextAdd_m( text );
  oySprintf_( text,   "  </oyImage_s>");
  hashTextAdd_m( text );

  if(allocateFunc == oyAllocateFunc_)
    oyDeAllocateFunc_(text);
  else
  {
    oyDeAllocateFunc_(text);
    text = hash_text;
    hash_text = oyStringCopy_( text, allocateFunc );
    oyDeAllocateFunc_( text );
  }
  text = 0;

  return hash_text;
}

/** Function lcm2FilterNode_GetText
 *  @brief   implement oyCMMFilterNode_GetText_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2009/06/02
 */
char * lcm2FilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc )
{
#ifdef NO_OPT
  return oyStringCopy_( oyFilterNode_GetText( node, type ), allocateFunc );
#else
  const char * tmp = 0,
             * model = 0;
  char * hash_text = 0,
       * temp = 0;
  oyFilterNode_s * s = node;

  oyImage_s * in_image = 0,
            * out_image = 0;
  int verbose;
  oyOptions_s * opts = node->core->options_;

  if(!node)
    return 0;

  verbose = oyOptions_FindString( node->tags, "verbose", "true" ) ? 1 : 0;

  /* 1. create hash text */
  hashTextAdd_m( "<oyFilterNode_s>\n  " );

  /* the filter text */
  hashTextAdd_m( oyFilterCore_GetText( node->core, oyNAME_NAME ) );

  /* pick all plug (input) data */
  in_image = (oyImage_s*) node->plugs[0]->remote_socket_->data;

  /* pick all sockets (output) data */
  out_image = (oyImage_s*) node->sockets[0]->data;

  /* make a description */
  {
    /* input data */
    hashTextAdd_m(   " <data_in>\n" );
    if(in_image)
    {
      temp = lcm2Image_GetText( in_image, verbose, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyDeAllocateFunc_(temp); temp = 0;
    }
    hashTextAdd_m( "\n </data_in>\n" );

    /* options -> xforms */
    hashTextAdd_m(   " <oyOptions_s>\n" );
    model = oyOptions_GetText( opts, oyNAME_NAME );
    hashTextAdd_m( model );
    hashTextAdd_m( "\n </oyOptions_s>\n" );

    /* output data */
    hashTextAdd_m(   " <data_out>\n" );
    if(out_image)
    {
      temp = lcm2Image_GetText( out_image, verbose, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyDeAllocateFunc_(temp); temp = 0;
    }
    hashTextAdd_m( "\n </data_out>\n" );
  }
  hashTextAdd_m( tmp );

  hashTextAdd_m(   "</oyFilterNode_s>\n" );

  return oyStringCopy_( hash_text, allocateFunc );
#endif
}

/** Function lcm2FlagsToText
 *  @brief   debugging helper
 *
 *  @version Oyranos: 0.1.13
 *  @since   2010/11/28 (Oyranos: 0.1.13)
 *  @date    2010/11/28
 */
char * lcm2FlagsToText               ( int                 flags )
{
  char * t = 0;
  char num[24];

  sprintf(num, "%d", flags);
  STRING_ADD( t, "flags[" );
  STRING_ADD( t, num );
  STRING_ADD( t, "]: " );
#define STRING_ADD_FLAG( flag_name ) \
  if(flags & flag_name) \
    STRING_ADD( t, " " #flag_name );
  STRING_ADD_FLAG( cmsFLAGS_NOCACHE );
  STRING_ADD_FLAG( cmsFLAGS_NOOPTIMIZE );
  STRING_ADD_FLAG( cmsFLAGS_NULLTRANSFORM );
  STRING_ADD_FLAG( cmsFLAGS_GAMUTCHECK );
  STRING_ADD_FLAG( cmsFLAGS_SOFTPROOFING );
  STRING_ADD_FLAG( cmsFLAGS_BLACKPOINTCOMPENSATION );
  STRING_ADD_FLAG( cmsFLAGS_NOWHITEONWHITEFIXUP );
  STRING_ADD_FLAG( cmsFLAGS_HIGHRESPRECALC );
  STRING_ADD_FLAG( cmsFLAGS_LOWRESPRECALC );
  STRING_ADD_FLAG( cmsFLAGS_8BITS_DEVICELINK );
  STRING_ADD_FLAG( cmsFLAGS_GUESSDEVICECLASS );
  STRING_ADD_FLAG( cmsFLAGS_KEEP_SEQUENCE );
  STRING_ADD_FLAG( cmsFLAGS_FORCE_CLUT );
  STRING_ADD_FLAG( cmsFLAGS_CLUT_POST_LINEARIZATION );
  STRING_ADD_FLAG( cmsFLAGS_CLUT_PRE_LINEARIZATION );
  return t;
}

/** Function lcm2CMMdata_Convert
 *  @brief   convert between data formats
 *  @ingroup cmm_handling
 *
 *  The function might be used to provide a module specific context.
 *  Implements oyCMMdata_Convert_f
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int  lcm2CMMdata_Convert             ( oyCMMptr_s        * data_in,
                                       oyCMMptr_s        * data_out,
                                       oyFilterNode_s    * node )
{
  int error = !data_in || !data_out;
  oyCMMptr_s * cmm_ptr_in = data_in,
             * cmm_ptr_out = data_out;
  lcm2TransformWrap_s * ltw  = 0;
  cmsHTRANSFORM xform = 0;
  cmsHPROFILE lps[2] = {0,0};
  oyFilterSocket_s * socket = (oyFilterSocket_s *)node->sockets[0];
  oyFilterPlug_s * plug = (oyFilterPlug_s *)node->plugs[0];
  oyImage_s * image_input = 0,
            * image_output = 0;

  image_input = (oyImage_s*)plug->remote_socket_->data;
  image_output = (oyImage_s*)socket->data;

  if(!error)
    error = data_in->type != oyOBJECT_CMM_POINTER_S || 
            data_out->type != oyOBJECT_CMM_POINTER_S;

  if(!error)
  {
    cmm_ptr_in = (oyCMMptr_s*) data_in;
    cmm_ptr_out = (oyCMMptr_s*) data_out;
  }

  if(!error &&
     ( (strcmp( cmm_ptr_in->resource, oyCOLOUR_ICC_DEVICE_LINK ) != 0) ||
       (strcmp( cmm_ptr_out->resource, lcm2TRANSFORM ) != 0) ) )
    error = 1;

  if(!error)
  {
    lps[0] = CMMProfileOpen_M( node, cmm_ptr_in->ptr, cmm_ptr_in->size );
    xform = lcm2CMMConversionContextCreate_( lps, 1, 0,0,0,
                                           image_input->layout_[0],
                                           image_output->layout_[0],
                                           node->core->options_,
                                           &ltw, cmm_ptr_out );
    if(!xform)
    {
      uint32_t f = image_input->layout_[0];
      message( oyMSG_WARN,(oyStruct_s*) node, OY_DBG_FORMAT_
      "float:%d optimised:%d colourspace:%d extra:%d channels:%d lcms_bytes%d",
      OY_DBG_ARGS_,
      T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
      error = 1;
    }
    CMMProfileRelease_M (lps[0] );
  }

  return error;
}

/** Function lcm2FilterPlug_CmmIccRun
 *  @brief   implement oyCMMFilterPlug_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2009/05/01
 */
int      lcm2FilterPlug_CmmIccRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int k, n;
  int error = 0;
  int channels = 0;
  oyDATATYPE_e data_type = 0;

  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node;
  oyImage_s * image_input = 0;
  oyArray2d_s * array_in = 0, * array_out = 0;
  lcm2TransformWrap_s * ltw  = 0;
  oyPixelAccess_s * new_ticket = ticket;

  plug = (oyFilterPlug_s *)node->plugs[0];
  input_node = plug->remote_socket_->node;

  image_input = oyFilterPlug_ResolveImage( plug, socket, ticket );

  if(oyImage_PixelLayoutGet( image_input ) != 
     oyImage_PixelLayoutGet( ticket->output_image ))
  {
    /* adapt the region of interesst to the new image dimensions */
    /* create a new ticket to avoid pixel layout conflicts */
    new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
    oyArray2d_Release( &new_ticket->array );
    oyImage_Release( &new_ticket->output_image );
    new_ticket->output_image = oyImage_Copy( image_input, 0 );
    error = oyImage_FillArray( image_input, new_ticket->output_image_roi, 2,
                               &new_ticket->array, 0, 0 );
  }

  /* We let the input filter do its processing first. */
  error = input_node->api7_->oyCMMFilterPlug_Run( plug, new_ticket );
  if(error != 0) return error;

  array_in = new_ticket->array;
  array_out = ticket->array;

  data_type = oyToDataType_m( oyImage_PixelLayoutGet( image_input ) );

  if(data_type == oyHALF)
  {
    oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message(oyMSG_WARN,0, OY_DBG_FORMAT_" can not handle oyHALF",OY_DBG_ARGS_);
    error = 1;
  }

  if(!ticket->output_image)
  {
    message( oyMSG_WARN,0, OY_DBG_FORMAT_ " no ticket->output_image",
             OY_DBG_ARGS_);
    error = 1;
  }

  if(!error)
  {
    channels = oyToChannels_m( ticket->output_image->layout_[0] );

    error = lcm2CMMTransform_GetWrap_( node->backend_data, &ltw );
  }

  /* now do some position blind manipulations */
  if(ltw)
  {
    n = (int)(array_out->width+0.5) / channels;

    if(!(data_type == oyUINT8 ||
         data_type == oyUINT16 ||
         data_type == oyFLOAT ||
         data_type == oyDOUBLE))
    {
      oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
      error = 1;
    }

    /*  - - - - - conversion - - - - - */
    /*message(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d Start lines: %d",
            __FILE__,__LINE__, array_out->height);*/
    if(!error)
    {
      if(array_out->height > 20)
      {
#pragma omp parallel for
        for( k = 0; k < array_out->height; ++k)
          cmsDoTransform( ltw->lcm2, array_in->array2d[k],
                                     array_out->array2d[k], n );
      } else
        for( k = 0; k < array_out->height; ++k)
          cmsDoTransform( ltw->lcm2, array_in->array2d[k],
                                     array_out->array2d[k], n );
    /*message(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d End width: %d",
            __FILE__,__LINE__, n);*/
    }

  } else
  {
    if(error)
      oyFilterSocket_Callback( requestor_plug,
                               oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT );
    else
      oyFilterSocket_Callback( requestor_plug,
                               oyCONNECTOR_EVENT_OK );

    error = oyOptions_SetFromText( &ticket->graph->options,
                     "//" OY_TYPE_STD "/profile/dirty", "true", OY_CREATE_NEW );
    error = 1;
  }

  if(oyImage_PixelLayoutGet( image_input ) != 
     oyImage_PixelLayoutGet( ticket->output_image ))
    oyPixelAccess_Release( &new_ticket );

  oyImage_Release( &image_input );

  return error;
}




/*
oyPointer          oyCMMallocateFunc   ( size_t            size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               oyCMMdeallocateFunc ( oyPointer         mem )
{
  if(mem)
    free(mem);
}*/

/** Function lcm2CMMWarnFunc
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int lcm2CMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;
  const char * type_name = "";
  int id = -1;
  const char * id_text = 0;

  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
    if(context->type_ == oyOBJECT_OPTION_S)
    {
      id_text = oyOption_GetText( (oyOption_s*)context, oyNAME_NAME );
      if(!id_text)
        id_text = ((oyOption_s*)context)->registration;
    }
  }

  va_start( list, format);
  vsprintf( text, format, list);
  va_end  ( list );

  switch(code)
  {
    case oyMSG_WARN:
         fprintf( stderr, "WARNING"); fprintf( stderr, ": " );
         break;
    case oyMSG_ERROR:
         fprintf( stderr, "!!! ERROR"); fprintf( stderr, ": " );
         break;
  }

  fprintf( stderr, "%s[%d]%s%s ", type_name, id,
           id_text ? "=" : "", id_text ? id_text : "" );

  fprintf( stderr, "%s", text ); fprintf( stderr, "\n" );
  free( text );

  return 0;
}

/** Function lcm2ErrorHandlerFunction
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
void lcm2ErrorHandlerFunction        ( cmsContext          ContextID,
                                       cmsUInt32Number     ErrorCode,
                                       const char        * ErrorText )
{
  int code = 0;
  code = oyMSG_ERROR;
  message( code, ContextID, CMM_NICK ": %s", ErrorText );
}

/** Function lcm2CMMMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int            lcm2CMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

char lcm2_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "icc" ">\n\
      <cmyk_cmyk_black_preservation.advanced>0</cmyk_cmyk_black_preservation.advanced>\n\
      <precalculation.advanced>2</precalculation.advanced>\n\
     </" "icc" ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

#define A(long_text) STRING_ADD( tmp, long_text)

/** Function lcm2GetOptionsUI
 *  @brief   return XFORMS for matching options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/29 (Oyranos: 0.1.10)
 *  @date    2009/07/30
 */
int lcm2GetOptionsUI                 ( oyOptions_s        * options,
                                       char              ** ui_text,
                                       oyAlloc_f            allocateFunc )
{
  char * tmp = 0;

  tmp = (char *)oyOptions_FindString( options,
                                      "cmyk_cmyk_black_preservation", 0 );
  if(tmp == 0)
    return 0;

  tmp = oyStringCopy_( "\
  <h3>little CMS 2 ", oyAllocateFunc_ );

  A(       _("Extended Options"));
  A(                         ":</h3>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc/cmyk_cmyk_black_preservation\">\n\
      <xf:label>" );
  A(          _("Black Preservation"));
  A(                              "</xf:label>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0</xf:value>\n\
        <xf:label>none</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1</xf:value>\n\
        <xf:label>LCMS_PRESERVE_PURE_K</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>2</xf:value>\n\
        <xf:label>LCMS_PRESERVE_K_PLANE</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc/precalculation\">\n\
      <xf:label>" );
  A(          _("Optimization"));
  A(                              "</xf:label>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0</xf:value>\n\
        <xf:label>LCMS2_NOOPTIMIZE</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1</xf:value>\n\
        <xf:label>normal</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>2</xf:value>\n\
        <xf:label>LCMS2_HIGHRESPRECALC</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>3</xf:value>\n\
        <xf:label>LCMS2_LOWRESPRECALC</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n");

  if(allocateFunc && tmp)
  {
    char * t = oyStringCopy_( tmp, allocateFunc );
    oyFree_m_( tmp );
    tmp = t; t = 0;
  } else
    return 1;

  *ui_text = tmp;

  return 0;
}

/** Function lcm2CreateICCMatrixProfile
 *  @brief   ICC from EDID
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/24 (Oyranos: 0.1.10)
 *  @date    2009/12/10
 */
oyProfile_s *      lcm2CreateICCMatrixProfile (
                                       float             gamma,
                                       float rx, float ry,
                                       float gx, float gy,
                                       float bx, float by,
                                       float wx, float wy)
{
  cmsCIExyYTRIPLE p;
  cmsToneCurve * g[3] = {0,0,0};
  /* 0.31271, 0.32902 D65 */
  cmsCIExyY wtpt_xyY;
  cmsHPROFILE lp = 0;
  cmsUInt32Number size = 0;
  char * data = 0;

  int error = 0;
  oyProfile_s * prof = 0;

  p.Red.x = rx; 
  p.Red.y = ry;
  p.Green.x = gx;
  p.Green.y = gy;
  p.Blue.x = bx;
  p.Blue.y = by;
  wtpt_xyY.x = wx;
  wtpt_xyY.y = wy;
  wtpt_xyY.Y = 1.0;
  g[0] = g[1] = g[2] = cmsBuildGamma(0, (double)gamma);
  lp = cmsCreateRGBProfile( &wtpt_xyY, &p, g);

  cmsSaveProfileToMem( lp, 0, &size );
  data = oyAllocateFunc_( size );
  cmsSaveProfileToMem( lp, data, &size );
  cmsCloseProfile( lp );
  cmsFreeToneCurve( g[0] );

  prof = oyProfile_FromMem( size, data, 0,0 );


  error = oyProfile_AddTagText( prof, icSigCopyrightTag,
                                      "no copyright; use freely" );

  oyDeAllocateFunc_( data ); size = 0;
  return prof;
}

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
int          lcm2MOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  oyOption_s * o = 0;
  oyProfile_s * prof = 0;
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"create_profile", 0))
    {
      double val = 0.0;
      o = oyOptions_Find( options, "colour_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma" );
      error = oyOptions_FindDouble( options,
        "colour_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                            8, &val );
      if(!o)
      {
        message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ " "
                 "no option \"colour_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" found",
                 OY_DBG_ARGS_ );
        error = 1;
      } else if( error != 0 )
      {
        message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_" "
                 "option \"colour_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" %s",
                 OY_DBG_ARGS_,
                 (error < 0) ? "contains less than 9 required values" :
                               "access returned with error" );
      }

      oyOption_Release( &o );

      return error;
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"create_profile", 0))
  {
    o = oyOptions_Find( options, "colour_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma" );
    if(o)
    {
      prof = lcm2CreateICCMatrixProfile (
                    oyOption_GetValueDouble(o,8),
                    oyOption_GetValueDouble(o,0), oyOption_GetValueDouble(o,1),
                    oyOption_GetValueDouble(o,2), oyOption_GetValueDouble(o,3),
                    oyOption_GetValueDouble(o,4), oyOption_GetValueDouble(o,5),
                    oyOption_GetValueDouble(o,6), oyOption_GetValueDouble(o,7));
      oyOption_Release( &o );

      o = oyOption_New( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_profile.create_profile.colour_matrix._" CMM_NICK,
                        0 );
      error = oyOption_StructMoveIn( o, (oyStruct_s**) &prof );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    }
  }

  return 0;
}

/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
const char * lcm2InfoGetTextProfileC ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if LittleCMS can handle a certain command.");
  } else if(strcmp(select, "create_profile")==0)
  {
         if(type == oyNAME_NICK)
      return "create_profile";
    else if(type == oyNAME_NAME)
      return _("Create a ICC matrix profile.");
    else
      return _("The littleCMS \"create_profile.colour_matrix\" command lets you create ICC profiles from some given colourimetric coordinates. The filter expects a oyOption_s object with name \"colour_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" containing 9 floats in the order of CIE*x for red, CIE*y for red, CIE*x for green, CIE*y for green, CIE*x for blue, CIE*y for blue, CIE*x for white, CIE*y for white and a gamma value.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Create a ICC matrix profile.");
    else
      return _("The littleCMS \"create_profile.colour_matrix\" command lets you create ICC profiles from some given colourimetric coordinates. See the \"create_profile\" info item.");
  }
  return 0;
}
const char *lcm2_texts_profile_create[4] = {"can_handle","create_profile","help",0};

/** @instance lcm2_api10_cmm
 *  @brief    littleCMS oyCMMapi10_s implementation
 *
 *  a filter for simple profile creation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
oyCMMapi10_s    lcm2_api10_cmm = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  0,

  lcm2CMMInit,
  lcm2CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "create_profile.colour_matrix.icc._" CMM_NICK "._CPU",

  CMM_VERSION,
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
 
  lcm2InfoGetTextProfileC,             /**< getText */
  (char**)lcm2_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  lcm2MOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};


/** @instance lcm2_api6
 *  @brief    littleCMS oyCMMapi6_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyCMMapi6_s   lcm2_api6_cmm = {

  oyOBJECT_CMM_API6_S,
  0,0,0,
  (oyCMMapi_s*) & lcm2_api10_cmm,

  lcm2CMMInit,
  lcm2CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc._" CMM_NICK "._CPU." oyCOLOUR_ICC_DEVICE_LINK "_" lcm2TRANSFORM,

  CMM_VERSION,
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  
  oyCOLOUR_ICC_DEVICE_LINK,  /* data_type_in, "oyDL" */
  lcm2TRANSFORM,             /* data_type_out, lcm2TRANSFORM */
  lcm2CMMdata_Convert        /* oyCMMdata_Convert_f oyCMMdata_Convert */
};


/** @instance lcm2_api7
 *  @brief    littleCMS oyCMMapi7_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s   lcm2_api7_cmm = {

  oyOBJECT_CMM_API7_S,
  0,0,0,
  (oyCMMapi_s*) & lcm2_api6_cmm,

  lcm2CMMInit,
  lcm2CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc.colour._" CMM_NICK "._CPU._ACCEL",

  CMM_VERSION,
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lcm2FilterPlug_CmmIccRun,  /* oyCMMFilterPlug_Run_f */
  lcm2TRANSFORM,             /* data_type, lcm2TRANSFORM */

  (oyConnector_s**) lcm2_cmmIccPlug_connectors,/* plugs */
  1,                         /* plugs_n */
  0,                         /* plugs_last_add */
  (oyConnector_s**) lcm2_cmmIccSocket_connectors,   /* sockets */
  1,                         /* sockets_n */
  0,                         /* sockets_last_add */
};

/**
 *  This function implements oyCMMGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/22 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
const char * lcm2Api4UiGetText (
                                       const char        * select,
                                       oyNAME_e            type )
{
  static char * category = 0;
  if(strcmp(select,"name") ||
     strcmp(select,"help"))
  {
    return lcm2InfoGetText( select, type );
  }
  else if(strcmp(select,"category"))
  {
    if(!category)
    {
      STRING_ADD( category, _("Colour") );
      STRING_ADD( category, _("/") );
      /* CMM: abbreviation for Colour Matching Module */
      STRING_ADD( category, _("CMM") );
      STRING_ADD( category, _("/") );
      STRING_ADD( category, _("littleCMS") );
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
const char * lcm2_api4_ui_texts[] = {"name", "category", "help", 0};
/** @instance lcm2_api4_ui
 *  @brief    lcm2 oyCMMapi4_s::ui implementation
 *
 *  The UI for lcm2.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s lcm2_api4_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,1,10},                            /**< int32_t module_api[3] */

  lcm2Filter_CmmIccValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  lcm2WidgetEvent, /* oyWidgetEvent_f */

  "Colour/CMM/littleCMS2", /* category */
  lcm2_extra_options,   /* const char * options */
  lcm2GetOptionsUI,     /* oyCMMuiGet_f oyCMMuiGet */

  lcm2Api4UiGetText, /* oyCMMGetText_f   getText */
  lcm2_api4_ui_texts /* const char    ** texts */
};

/** @instance lcm2_api4_cmm
 *  @brief    littleCMS oyCMMapi4_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyCMMapi4_s   lcm2_api4_cmm = {

  oyOBJECT_CMM_API4_S,
  0,0,0,
  (oyCMMapi_s*) & lcm2_api7_cmm,

  lcm2CMMInit,
  lcm2CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc.colour._" CMM_NICK "._CPU._NOACCEL",

  CMM_VERSION,
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lcm2FilterNode_CmmIccContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  lcm2FilterNode_GetText, /* oyCMMFilterNode_GetText_f */
  oyCOLOUR_ICC_DEVICE_LINK, /* context data_type */

  &lcm2_api4_ui                        /**< oyCMMui_s *ui */
};



/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * lcm2InfoGetText         ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Little CMS 2");
    else
      return _("LittleCMS 2 is a CMM, a color management engine; it implements fast transforms between ICC profiles. \"Little\" stands for its small overhead. With a typical footprint of about 100K including C runtime, you can color-enable your application without the pain of ActiveX, OCX, redistributables or binaries of any kind. We are using little cms in several commercial projects, however, we are offering lcms library free for anybody under an extremely liberal open source license.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Marti");
    else if(type == oyNAME_NAME)
      return _("Marti Maria");
    else
      return _("littleCMS 2 project; www: http://www.littlecms.com; support/email: support@littlecms.com; sources: http://www.littlecms.com/downloads.htm; Oyranos wrapper: Kai-Uwe Behrmann for the Oyranos project");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("MIT");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 1998-2008 Marti Maria Saguer; MIT");
    else
      return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("The lcms \"colour.icc\" filter is a one dimensional colour conversion filter. It can both create a colour conversion context, some precalculated for processing speed up, and the colour conversion with the help of that context. The adaption part of this filter transforms the Oyranos colour context, which is ICC device link based, to the internal lcms format.");
    else
      return _("The following options are available to create colour contexts:\n \"profiles_simulation\", a option of type oyProfiles_s, can contain device profiles for proofing.\n \"profiles_effect\", a option of type oyProfiles_s, can contain abstract colour profiles.\n The following Oyranos options are supported: \"rendering_gamut_warning\", \"rendering_intent_proof\", \"rendering_bpc\", \"rendering_intent\", \"proof_soft\" and \"proof_hard\".\n The additional lcms option is supported \"cmyk_cmyk_black_preservation\" [0 - none; 1 - LCMS_PRESERVE_PURE_K; 2 - LCMS_PRESERVE_K_PLANE] and \"precalculation\": [0 - cmsFLAGS_NOOPTIMIZE; 1 - normal; 2 - cmsFLAGS_HIGHRESPRECALC, 3 - cmsFLAGS_LOWRESPRECALC] ." );
  }
  return 0;
}
const char *lcm2_texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance lcm2_cmm_module
 *  @brief    lcm2 module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMMInfo_s lcm2_cmm_module = {

  oyOBJECT_CMM_INFO_S,                 /**< type, struct type */
  0,0,0,                               /**< ,dynamic object functions */
  CMM_NICK,                            /**< cmm, ICC signature */
  "0.6",                               /**< backend_version */
  lcm2InfoGetText,                     /**< getText */
  (char**)lcm2_texts,                  /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,                     /**< oy_compatibility */

  (oyCMMapi_s*) & lcm2_api4_cmm,       /**< api */

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "lcms_logo2.png"}, /**< icon */
};

