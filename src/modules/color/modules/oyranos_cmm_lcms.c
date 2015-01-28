/** @file oyranos_cmm_lcms.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2007-2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    littleCMS CMM module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */




#include <lcms.h>
#include <dlfcn.h> /* dlopen() */

#include "oyCMM_s.h"
#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyCMMui_s_.h"
#include "oyConnectorImaging_s_.h"
#include "oyProfiles_s.h"

#include "oyranos_cmm.h"         /* the API's this CMM implements */
#include "oyranos_generic.h"         /* oy_connector_imaging_static_object */
#include "oyranos_helper.h"      /* oySprintf_ and other local helpers */
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_image.h"
#include "oyranos_object_internal.h"
#include "oyranos_string.h"

#ifdef _OPENMP
#define USE_OPENMP 1
#include <omp.h>
#endif

/*
oyCMMinfo_s   lcms_cmm_module;
oyCMMapi4_s     lcms_api4_cmm;
oyCMMui_s         lcms_api4_ui;
oyCMMapi7_s     lcms_api7_cmm;
oyConnectorImaging_s* lcms_cmmIccSocket_connectors[2];
oyConnectorImaging_s    lcms_cmmIccSocket_connector;
oyConnectorImaging_s* lcms_cmmIccPlug_connectors[2];
oyConnectorImaging_s    lcms_cmmIccPlug_connector;
oyCMMapi6_s     lcms_api6_cmm;
oyCMMapi10_s    lcms_api10_cmm;
oyCMMapi10_s    lcms_api10_cmm2;
*/

void* oyAllocateFunc_           (size_t        size);
void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAlloc_f     allocate_func);
void  oyDeAllocateFunc_         (void *        data);

#include <math.h>



/* --- internal definitions --- */

#define CMM_NICK "lcms"
#define CMM_FUNC lcms
#define CMMProfileOpen_M    lcmsOpenProfileFromMem
#define CMMProfileRelease_M lcmsCloseProfile
#define CMMToString_M(text) #text
#define CMMMaxChannels_M 16
#define lcmsPROFILE "lcPR"
#define lcmsTRANSFORM "lcCC"
/** The proofing LUTs grid size may improove the sharpness of out of color 
 *  marking, but at the prise of lost speed and increased memory consumption.
 *  53 is the grid size used internally in lcms' gamut marking code. */
#define lcmsPROOF_LUT_GRID_RASTER 53

#define CMM_VERSION {0,1,1}

oyMessage_f lcms_msg = oyMessageFunc;

int lcmsErrorHandlerFunction(int ErrorCode, const char *ErrorText);
int            lcmsCMMMessageFuncSet ( oyMessage_f         message_func );
int                lcmsCMMInit       ( );




/** @struct lcmsProfileWrap_s
 *  @brief lcms wrapper for profile data struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/10
 *  @since   2007/12/10 (Oyranos: 0.1.8)
 */
typedef struct {
  uint32_t     type;                   /**< shall be lcPR */
  size_t       size;
  oyPointer    block;                  /**< Oyranos raw profile pointer. Dont free! */
  oyPointer    lcms;                   /**< cmsHPROFILE struct */
  icColorSpaceSignature sig;           /**< ICC profile signature */
} lcmsProfileWrap_s;

/** @struct  lcmsTransformWrap_s
 *  @brief   lcms wrapper for transform data struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/20
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
typedef struct {
  int          type;                   /**< shall be lcCC */
  oyPointer    lcms;                   /**< cmsHPROFILE struct */
  icColorSpaceSignature sig_in;        /**< ICC profile signature */
  icColorSpaceSignature sig_out;       /**< ICC profile signature */
  oyPixel_t    oy_pixel_layout_in;
  oyPixel_t    oy_pixel_layout_out;
} lcmsTransformWrap_s;


lcmsTransformWrap_s * lcmsTransformWrap_Set_ (
                                       cmsHTRANSFORM       xform,
                                       icColorSpaceSignature color_in,
                                       icColorSpaceSignature color_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyPointer_s       * oy );
int      lcmsCMMTransform_GetWrap_   ( oyPointer_s       * cmm_ptr,
                                       lcmsTransformWrap_s ** s );
int lcmsCMMDeleteTransformWrap       ( oyPointer         * wrap );

lcmsProfileWrap_s * lcmsCMMProfile_GetWrap_(
                                       oyPointer_s       * cmm_ptr );
int lcmsCMMProfileReleaseWrap        ( oyPointer         * p );

int                lcmsCMMCheckPointer(oyPointer_s       * cmm_ptr,
                                       const char        * resource );
int        oyPixelToCMMPixelLayout_  ( oyPixel_t           pixel_layout,
                                       icColorSpaceSignature color_space );
char * lcmsImage_GetText             ( oyImage_s         * image,
                                       int                 verbose,
                                       oyAlloc_f           allocateFunc );


char * lcmsFilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );
extern char lcms_extra_options[];
cmsHPROFILE  lcmsGamutCheckAbstract  ( oyProfile_s       * proof,
                                       DWORD               flags,
                                       int                 intent,
                                       int                 intent_proof );
oyPointer  lcmsCMMColorConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
oyOptions_s* lcmsFilter_CmmIccValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
cmsHPROFILE  lcmsAddProfile          ( oyProfile_s       * p );
cmsHPROFILE  lcmsAddProofProfile     ( oyProfile_s       * proof,
                                       DWORD               flags,
                                       int                 intent,
                                       int                 intent_proof );
oyPointer lcmsFilterNode_CmmIccContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int  lcmsModuleData_Convert          ( oyPointer_s       * data_in,
                                       oyPointer_s       * data_out,
                                       oyFilterNode_s    * node );
int      lcmsFilterPlug_CmmIccRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
const char * lcmsInfoGetText         ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );




/* --- implementations --- */

/* explicitely load liblcms functions, to avoid conflicts */
static int lcms_initialised = 0;
static void * lcms_handle = NULL;

static int (*lcmsErrorAction)(int nAction) = NULL;
static void (*lcmsSetErrorHandler)(cmsErrorHandlerFunction Fn) = NULL;
static icColorSpaceSignature (*lcmsGetColorSpace)(cmsHPROFILE hProfile) = NULL;
static icColorSpaceSignature (*lcmsGetPCS)(cmsHPROFILE hProfile) = NULL;
static icProfileClassSignature (*lcmsGetDeviceClass)(cmsHPROFILE hProfile) = NULL;
static int (*l_cmsChannelsOf)(icColorSpaceSignature ColorSpace) = NULL;
static int (*lcmsSetCMYKPreservationStrategy)(int n) = NULL;
static cmsHTRANSFORM (*lcmsCreateTransform)(cmsHPROFILE Input,
                                               DWORD InputFormat,
                                               cmsHPROFILE Output,
                                               DWORD OutputFormat,
                                               int Intent,
                                               DWORD dwFlags) = NULL;
static cmsHTRANSFORM (*lcmsCreateProofingTransform)(cmsHPROFILE Input,
                                               DWORD InputFormat,
                                               cmsHPROFILE Output,
                                               DWORD OutputFormat,
                                               cmsHPROFILE Proofing,
                                               int Intent,
                                               int ProofingIntent,
                                               DWORD dwFlags) = NULL;
static cmsHTRANSFORM (*lcmsCreateMultiprofileTransform)(cmsHPROFILE hProfiles[],
                                                                int nProfiles,
                                                                DWORD InputFormat,
                                                                DWORD OutputFormat,
                                                                int Intent,
                                                                DWORD dwFlags) = NULL;
static void (*lcmsDeleteTransform)(cmsHTRANSFORM hTransform) = NULL;
static void (*lcmsDoTransform)(cmsHTRANSFORM Transform,
                                                 LPVOID InputBuffer,
                                                 LPVOID OutputBuffer,
                                                 unsigned int Size) = NULL;
static cmsHPROFILE (*lcmsTransform2DeviceLink)(cmsHTRANSFORM hTransform, DWORD dwFlags) = NULL;
static LCMSBOOL (*lcmsAddTag)(cmsHPROFILE hProfile, icTagSignature sig, const void* data) = NULL;
static LCMSBOOL (*l_cmsSaveProfileToMem)(cmsHPROFILE hProfile, void *MemPtr, 
                                                                size_t* BytesNeeded) = NULL;
static cmsHPROFILE (*lcmsOpenProfileFromMem)(LPVOID MemPtr, DWORD dwSize) = NULL;
static LCMSBOOL (*lcmsCloseProfile)(cmsHPROFILE hProfile) = NULL;
static LPLUT (*lcmsAllocLUT)(void) = NULL;
static LPLUT (*lcmsAlloc3DGrid)(LPLUT Lut, int clutPoints, int inputChan, int outputChan) = NULL;
static int (*lcmsSample3DGrid)(LPLUT Lut, _cmsSAMPLER Sampler, LPVOID Cargo, DWORD dwFlags) = NULL;
static void (*lcmsFreeLUT)(LPLUT Lut) = NULL;
static cmsHPROFILE (*l_cmsCreateProfilePlaceholder)(void) = NULL;
static void (*lcmsSetDeviceClass)(cmsHPROFILE hProfile, icProfileClassSignature sig) = NULL;
static void (*lcmsSetColorSpace)(cmsHPROFILE hProfile, icColorSpaceSignature sig) = NULL;
static void (*lcmsSetPCS)(cmsHPROFILE hProfile, icColorSpaceSignature pcs) = NULL;
static LPGAMMATABLE (*lcmsBuildGamma)(int nEntries, double Gamma) = NULL;
static void (*lcmsFreeGamma)(LPGAMMATABLE Gamma) = NULL;
static cmsHPROFILE (*lcmsCreateRGBProfile)(LPcmsCIExyY WhitePoint,
                                        LPcmsCIExyYTRIPLE Primaries,
                                        LPGAMMATABLE TransferFunction[3]) = NULL;
static cmsHPROFILE (*lcmsCreateLabProfile)(LPcmsCIExyY WhitePoint) = NULL;
static LPcmsCIEXYZ (*lcmsD50_XYZ)(void) = NULL;
static LPcmsCIExyY (*lcmsD50_xyY)(void) = NULL; 
static void (*lcmsLabEncoded2Float)(LPcmsCIELab Lab, const WORD wLab[3]) = NULL;
static void (*lcmsFloat2LabEncoded)(WORD wLab[3], const cmsCIELab* Lab) = NULL;
static double (*lcmsDeltaE)(LPcmsCIELab Lab1, LPcmsCIELab Lab2) = NULL;


#define LOAD_FUNC( func ) l##func = dlsym(lcms_handle, #func ); \
               if(!l##func) lcms_msg( oyMSG_ERROR,0, OY_DBG_FORMAT_" " \
                                      "init failed: %s", \
                                      OY_DBG_ARGS_, dlerror() );

/** Function lcmsCMMInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/01/18
 *  @since   2007/12/11 (Oyranos: 0.1.8)
 */
int                lcmsCMMInit       ( oyStruct_s        * filter )
{
  int error = 0;

  if(!lcms_initialised)
  {
    lcms_initialised = 1;

    char * fn = oyLibNameCreate_( "lcms", 1 );
    lcms_handle = dlopen(fn, RTLD_LAZY);
    oyFree_m_( fn );

    if(!lcms_handle)
    {
      lcms_msg( oyMSG_ERROR,0, OY_DBG_FORMAT_" "
               "init failed: %s",
                OY_DBG_ARGS_, dlerror() );
      error = 1;
    } else
    {
      LOAD_FUNC( cmsErrorAction );
      LOAD_FUNC( cmsSetErrorHandler );
      LOAD_FUNC( cmsGetColorSpace );
      LOAD_FUNC( cmsGetPCS );
      LOAD_FUNC( cmsGetDeviceClass );
      LOAD_FUNC( _cmsChannelsOf );
      LOAD_FUNC( cmsSetCMYKPreservationStrategy );
      LOAD_FUNC( cmsCreateTransform );
      LOAD_FUNC( cmsCreateProofingTransform );
      LOAD_FUNC( cmsCreateMultiprofileTransform );
      LOAD_FUNC( cmsDeleteTransform );
      LOAD_FUNC( cmsDoTransform );
      LOAD_FUNC( cmsTransform2DeviceLink );
      LOAD_FUNC( cmsAddTag );
      LOAD_FUNC( _cmsSaveProfileToMem );
      LOAD_FUNC( cmsOpenProfileFromMem );
      LOAD_FUNC( cmsCloseProfile );
      LOAD_FUNC( cmsAllocLUT );
      LOAD_FUNC( cmsAlloc3DGrid );
      LOAD_FUNC( cmsSample3DGrid );
      LOAD_FUNC( cmsFreeLUT );
      LOAD_FUNC( _cmsCreateProfilePlaceholder );
      LOAD_FUNC( cmsSetDeviceClass );
      LOAD_FUNC( cmsSetColorSpace );
      LOAD_FUNC( cmsSetPCS );
      LOAD_FUNC( cmsBuildGamma );
      LOAD_FUNC( cmsFreeGamma );
      LOAD_FUNC( cmsCreateRGBProfile );
      LOAD_FUNC( cmsCreateLabProfile );
      LOAD_FUNC( cmsD50_XYZ );
      LOAD_FUNC( cmsD50_xyY );
      LOAD_FUNC( cmsLabEncoded2Float );
      LOAD_FUNC( cmsFloat2LabEncoded );
      LOAD_FUNC( cmsDeltaE );

      lcmsErrorAction( LCMS_ERROR_SHOW );
      lcmsSetErrorHandler( lcmsErrorHandlerFunction );
    }
  }
  return error;
}



/** Function lcmsCMMProfile_GetWrap_
 *  @brief   convert to lcms profile wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/10
 *  @since   2007/12/10 (Oyranos: 0.1.8)
 */
lcmsProfileWrap_s * lcmsCMMProfile_GetWrap_( oyPointer_s * cmm_ptr )
{
  lcmsProfileWrap_s * s = 0;

  char * type_ = lcmsPROFILE;
  int type = *((int32_t*)type_);

  if(cmm_ptr && !lcmsCMMCheckPointer( cmm_ptr, lcmsPROFILE ) &&
     oyPointer_GetPointer(cmm_ptr))
    s = (lcmsProfileWrap_s*) oyPointer_GetPointer(cmm_ptr);

  if(s && s->type != type)
    s = 0;

  return s;
}

/** Function lcmsCMMTransform_GetWrap_
 *  @brief   convert to lcms transform wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 *  @date    2009/05/28
 */
int      lcmsCMMTransform_GetWrap_   ( oyPointer_s       * cmm_ptr,
                                       lcmsTransformWrap_s ** s )
{
  char * type_ = lcmsTRANSFORM;
  int type = *((int32_t*)type_);

  if(cmm_ptr && !lcmsCMMCheckPointer( cmm_ptr, lcmsTRANSFORM ) &&
     oyPointer_GetPointer(cmm_ptr))
    *s = (lcmsTransformWrap_s*) oyPointer_GetPointer(cmm_ptr);

  if(*s && ((*s)->type != type || !(*s)->lcms))
  {
    *s = 0;
    return 1;
  }

  return 0;
}

/** Function lcmsCMMProfileReleaseWrap
 *  @brief   release a lcms profile wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/20
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
int lcmsCMMProfileReleaseWrap(oyPointer *p)
{
  int error = !p;
  lcmsProfileWrap_s * s = 0;
  
  char * type_ = lcmsPROFILE;
  int type = *((int32_t*)type_);
  char s_type[4];

  if(!error && *p)
    s = (lcmsProfileWrap_s*) *p;

  if(!error)
    error = !s;

  if(!error)
    memcpy(s_type, &s->type, 4);

  if(!error && s->type != type)
    error = 1;

  if(!error)
  {
    CMMProfileRelease_M (s->lcms);

    s->lcms = 0;
    s->type = 0;
    s->size = 0;
    s->block = 0;
    free(s);
  }

  if(!error)
    *p = 0;

  return error;
}


/** Function lcmsCMMDataOpen
 *  @brief   oyCMMProfileOpen_t implementation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/12 (Oyranos: 0.1.8)
 *  @date    2007/12/27
 */
int          lcmsCMMData_Open        ( oyStruct_s        * data,
                                       oyPointer_s       * oy )
{
  oyPointer_s * s = 0;
  int error = 0;

  if(!error)
  {
    char * type_ = lcmsPROFILE;
    int type = *((int32_t*)type_);
    size_t size = 0;
    oyPointer block = 0;
    lcmsProfileWrap_s * s = calloc(sizeof(lcmsProfileWrap_s), 1);

    if(data->type_ == oyOBJECT_PROFILE_S)
    {
      oyProfile_s * p = (oyProfile_s*)data;
      block = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
    }

    s->type = type;
    s->size = size;
    s->block = block;

    s->lcms = CMMProfileOpen_M( block, size );
    error = oyPointer_Set( oy, 0,
                          lcmsPROFILE, s, CMMToString_M(CMMProfileOpen_M),
                          lcmsCMMProfileReleaseWrap );
  }

  if(!error)
    s = oy;

  if(!error)
    error = !s;

  return error;
}

/** Function lcmsCMMCheckPointer
 *  @brief   
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/12
 *  @since   2007/11/12 (Oyranos: 0.1.8)
 */
int                lcmsCMMCheckPointer(oyPointer_s       * cmm_ptr,
                                       const char        * resource )
{
  int error = !cmm_ptr;

  if(cmm_ptr &&
     oyPointer_GetPointer(cmm_ptr) && oyPointer_GetResourceName(cmm_ptr))
  {
    int * res_id = (int*)oyPointer_GetResourceName(cmm_ptr);

    if(!oyCMMlibMatchesCMM(oyPointer_GetLibName(cmm_ptr), CMM_NICK) ||
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
                                       icColorSpaceSignature color_space )
{
  int cmm_pixel = 0;
  int chan_n = oyToChannels_m (pixel_layout);
  int c_off = oyToColorOffset_m (pixel_layout);
  oyDATATYPE_e data_type = oyToDataType_m (pixel_layout);
  int planar = oyToPlanar_m (pixel_layout);
  int flavour = oyToFlavor_m (pixel_layout);
  int cchans = l_cmsChannelsOf( color_space );
  int extra = chan_n - cchans;

  if(chan_n > CMMMaxChannels_M)
    lcms_msg( oyMSG_WARN,0, OY_DBG_FORMAT_" "
             "can not handle more than %d channels; found: %d",
             OY_DBG_ARGS_, CMMMaxChannels_M, chan_n);

  cmm_pixel = COLORSPACE_SH(PT_ANY);
  cmm_pixel |= CHANNELS_SH(cchans);
  if(extra)
    cmm_pixel |= EXTRA_SH(extra);
  if(c_off == 1)
    cmm_pixel |= SWAPFIRST_SH(1);
  if(data_type == oyUINT8)
    cmm_pixel |= BYTES_SH(1);
  else if(data_type == oyUINT16)
    cmm_pixel |= BYTES_SH(2);
  if(oyToSwapColorChannels_m (pixel_layout))
    cmm_pixel |= DOSWAP_SH(1);
  if(oyToByteswap_m(pixel_layout))
    cmm_pixel |= ENDIAN16_SH(1);
  if(planar)
    cmm_pixel |= PLANAR_SH(1);
  if(flavour)
    cmm_pixel |= FLAVOR_SH(1);

  return cmm_pixel;
}

/** Function lcmsCMMDeleteTransformWrap
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/00 (Oyranos: 0.1.8)
 *  @date    2007/12/00
 */
int lcmsCMMDeleteTransformWrap(oyPointer * wrap)
{
  
  if(wrap && *wrap)
  {
    lcmsTransformWrap_s * s = (lcmsTransformWrap_s*) *wrap;

    lcmsDeleteTransform (s->lcms);
    s->lcms = 0;

    free(s);

    *wrap = 0;

    return 0;
  }

  return 1;
}

/** Function lcmsTransformWrap_Set_
 *  @brief   fill a lcmsTransformWrap_s struct
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/12/21
 */
lcmsTransformWrap_s * lcmsTransformWrap_Set_ (
                                       cmsHTRANSFORM       xform,
                                       icColorSpaceSignature color_in,
                                       icColorSpaceSignature color_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyPointer_s       * oy )
{
  int error = !xform;
  lcmsTransformWrap_s * s = 0;
  
  if(!error)
  {
    char * type_ = lcmsTRANSFORM;
    int type = *((int32_t*)type_);
    lcmsTransformWrap_s * ltw = calloc(sizeof(lcmsTransformWrap_s), 1);

    ltw->type = type;

    ltw->lcms = xform; xform = 0;

    ltw->sig_in  = color_in;
    ltw->sig_out = color_out;
    ltw->oy_pixel_layout_in  = oy_pixel_layout_in;
    ltw->oy_pixel_layout_out = oy_pixel_layout_out;
    s = ltw;
  }

  if(!error)
    oyPointer_Set( oy, 0, 0, s,
                  "lcmsCMMDeleteTransformWrap", lcmsCMMDeleteTransformWrap );

  return s;
}

int            lcmsIntentFromOptions ( oyOptions_s       * opts,
                                       int                 proof )
{
  int intent = 0,
      intent_proof = 0;
  const char * o_txt = 0;

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

  if(proof)
    return intent_proof;
  else
    return intent;
}

uint32_t       lcmsFlagsFromOptions  ( oyOptions_s       * opts )
{
  int bpc = 0,
      cmyk_cmyk_black_preservation = 0,
      gamut_warning = 0,
      precalculation = 0,
      flags = 0;
  const char * o_txt = 0;

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
      flags = bpc ?           flags | cmsFLAGS_WHITEBLACKCOMPENSATION :
                              flags & (~cmsFLAGS_WHITEBLACKCOMPENSATION);
      flags = gamut_warning ? flags | cmsFLAGS_GAMUTCHECK :
                              flags & (~cmsFLAGS_GAMUTCHECK);
      switch(precalculation)
      {
      case 0: flags |= cmsFLAGS_NOTPRECALC; break;
      case 1: flags |= 0; break;
      case 2: flags |= cmsFLAGS_HIGHRESPRECALC; break;
      case 3: flags |= cmsFLAGS_LOWRESPRECALC; break;
      }
      flags = cmyk_cmyk_black_preservation ? flags | cmsFLAGS_PRESERVEBLACK :
                              flags & (~cmsFLAGS_PRESERVEBLACK);

  if(oy_debug)
    lcms_msg( oyMSG_WARN,0, OY_DBG_FORMAT_"\n"
             "  bpc: %d  gamut_warning: %d  precalculation: %d\n",
             OY_DBG_ARGS_,
                bpc,     gamut_warning,     precalculation );

  return flags;
}

/** Function lcmsCMMConversionContextCreate_
 *  @brief   create a CMM transform
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2009/11/04
 */
cmsHTRANSFORM  lcmsCMMConversionContextCreate_ (
                                       cmsHPROFILE       * lps,
                                       int                 profiles_n,
                                       oyProfiles_s      * simulation,
                                       int                 proof_n,
                                       int                 proof,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s       * opts,
                                       lcmsTransformWrap_s ** ltw,
                                       oyPointer_s       * oy )
{
  oyPixel_t lcms_pixel_layout_in = 0;
  oyPixel_t lcms_pixel_layout_out = 0;
  int error = !lps;
  cmsHTRANSFORM xform = 0;
  cmsHPROFILE * merge = 0;
  icColorSpaceSignature color_in = 0;
  icColorSpaceSignature color_out = 0;
  icProfileClassSignature profile_class_in = 0;
  int intent = lcmsIntentFromOptions( opts,0 ),
      intent_proof = lcmsIntentFromOptions( opts,1 ),
      cmyk_cmyk_black_preservation = 0,
      flags = lcmsFlagsFromOptions( opts ),
      gamut_warning = flags & cmsFLAGS_GAMUTCHECK;
  const char * o_txt = 0;

  if(!lps || !profiles_n || !oy_pixel_layout_in || !oy_pixel_layout_out)
    return 0;

      flags = proof ?         flags | cmsFLAGS_SOFTPROOFING :
                              flags & (~cmsFLAGS_SOFTPROOFING);
 
  if(!error)
  {
    color_in = lcmsGetColorSpace( lps[0] );
    if(profiles_n > 1)
      color_out = lcmsGetColorSpace( lps[profiles_n-1] );
    else
      color_out = lcmsGetPCS( lps[profiles_n-1] );
    profile_class_in = lcmsGetDeviceClass( lps[0] );
  }

  lcms_pixel_layout_in  = oyPixelToCMMPixelLayout_(oy_pixel_layout_in,
                                                   color_in);
  lcms_pixel_layout_out = oyPixelToCMMPixelLayout_(oy_pixel_layout_out,
                                                   color_out);

      o_txt = oyOptions_FindString  ( opts, "cmyk_cmyk_black_preservation", 0 );
      if(o_txt && oyStrlen_(o_txt))
        cmyk_cmyk_black_preservation = atoi( o_txt );

      if(cmyk_cmyk_black_preservation == 2)
        lcmsSetCMYKPreservationStrategy( LCMS_PRESERVE_K_PLANE );


  if(!error)
  {
     /* we have to erase the color space */
         if(profiles_n == 1 || profile_class_in == icSigLinkClass)
    {
        xform = lcmsCreateTransform( lps[0], lcms_pixel_layout_in,
                                    0, lcms_pixel_layout_out,
                                    intent, flags );
    }
    else if(profiles_n == 2 && (!proof_n || (!proof && !gamut_warning)))
        xform = lcmsCreateTransform( lps[0], lcms_pixel_layout_in,
                                    lps[1], lcms_pixel_layout_out,
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
          merge[profiles_n-1 + i] = lcmsAddProofProfile( 
                                             oyProfiles_Get(simulation,i),flags,
                                             intent, intent_proof);

        merge[profiles_n + proof_n -1] = lps[profiles_n - 1];

        /* merge effect and simulation profiles */
        multi_profiles_n += proof_n;
        lps = merge;
      }

      if(flags & cmsFLAGS_GAMUTCHECK)
        flags |= cmsFLAGS_GRIDPOINTS(lcmsPROOF_LUT_GRID_RASTER);

      xform =   lcmsCreateMultiprofileTransform(
                                    lps, 
                                    multi_profiles_n,
                                    lcms_pixel_layout_in,
                                    lcms_pixel_layout_out,
                                    intent, flags );


      if(merge) oyDeAllocateFunc_( merge ); merge = 0;
    }
  }

  /* reset */
  lcmsSetCMYKPreservationStrategy( LCMS_PRESERVE_PURE_K );

  if(!error && ltw && oy)
    *ltw= lcmsTransformWrap_Set_( xform, color_in, color_out,
                                  oy_pixel_layout_in, oy_pixel_layout_out, oy );

  end:
  return xform;
}

/** Function lcmsCMMColorConversion_ToMem_
 *
 *  convert a lcms color conversion context to a device link
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyPointer  lcmsCMMColorConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !xform;
  oyPointer data = 0;

  if(!error)
  {
    cmsHPROFILE dl = lcmsTransform2DeviceLink( xform, 0 );

    *size = 0;

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

        lcmsAddTag(dl, icSigProfileSequenceDescTag, pseq);
        free(pseq);
    }

    l_cmsSaveProfileToMem( dl, 0, size );
    data = allocateFunc( *size );
    l_cmsSaveProfileToMem( dl, data, size );
  }

  return data;
}

oyOptions_s* lcmsFilter_CmmIccValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = oyFilterRegistrationMatch(oyFilterCore_GetRegistration(filter),
                                      "//"OY_TYPE_STD"/icc_color",
                                      oyOBJECT_CMM_API4_S);

  *result = error;

  return 0;
}

oyWIDGET_EVENT_e   lcmsWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}


oyDATATYPE_e lcms_cmmIcc_data_types[7] = {oyUINT8, oyUINT16, oyDOUBLE, 0};

oyConnectorImaging_s_ lcms_cmmIccSocket_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  lcms_cmmIcc_data_types, /* data_types */
  3, /* data_types_n; elements in data_types array */
  1, /* max_color_offset */
  1, /* min_channels_count; */
  16, /* max_channels_count; */
  1, /* min_color_count; */
  16, /* max_color_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_* lcms_cmmIccSocket_connectors[2]={&lcms_cmmIccSocket_connector,0};

oyConnectorImaging_s_ lcms_cmmIccPlug_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  lcms_cmmIcc_data_types, /* data_types */
  3, /* data_types_n; elements in data_types array */
  1, /* max_color_offset */
  1, /* min_channels_count; */
  16, /* max_channels_count; */
  1, /* min_color_count; */
  16, /* max_color_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_* lcms_cmmIccPlug_connectors[2]={&lcms_cmmIccPlug_connector,0};

/** Function lcmsAddProofProfile
 *  @brief   add a abstract proofing profile to the lcms profile stack 
 *
 *  Look in the Oyranos cache for a CMM internal representation or generate a
 *  new abstract profile containing the proofing profiles changes. This can be
 *  a proofing color space simulation or out of gamut marking.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/05 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
cmsHPROFILE  lcmsAddProofProfile     ( oyProfile_s       * proof,
                                       DWORD               flags,
                                       int                 intent,
                                       int                 intent_proof )
{
  int error = 0;
  cmsHPROFILE * hp = 0;
  oyPointer_s * cmm_ptr = 0;
  lcmsProfileWrap_s * s = 0;
  char * hash_text = 0,
       num[12];

  if(!proof || proof->type_ != oyOBJECT_PROFILE_S)
  {
    lcms_msg( oyMSG_WARN, (oyStruct_s*)proof, OY_DBG_FORMAT_" "
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
  cmm_ptr = oyPointer_LookUpFromText( hash_text, lcmsPROFILE );

  oyPointer_Set( cmm_ptr, CMM_NICK, 0,0,0,0 );

  /* for empty profile create a new abstract one */
  if(!oyPointer_GetPointer(cmm_ptr))
  {
    oyPointer_s * oy = cmm_ptr;

    char * type_ = lcmsPROFILE;
    uint32_t type = *((uint32_t*)type_);
    size_t size = 0;
    oyPointer block = 0;
    lcmsProfileWrap_s * s = calloc(sizeof(lcmsProfileWrap_s), 1);

    if(oy_debug == 1)
      fprintf( stderr, OY_DBG_FORMAT_" created: \"%s\"",
               OY_DBG_ARGS_, hash_text );
    else
    lcms_msg( oyMSG_DBG, (oyStruct_s*)proof,
             OY_DBG_FORMAT_" created abstract proofing profile: \"%s\"",
             OY_DBG_ARGS_, hash_text );
 
    /* create */
    hp = lcmsGamutCheckAbstract( proof, flags, intent, intent_proof );
    if(hp)
    {
      /* save to memory */
      l_cmsSaveProfileToMem( hp, 0, &size );
      block = oyAllocateFunc_( size );
      l_cmsSaveProfileToMem( hp, block, &size );
      lcmsCloseProfile( hp ); hp = 0;
    }

    s->type = type;
    s->size = size;
    s->block = block;

    /* reopen */
    s->lcms = CMMProfileOpen_M( block, size );
    error = oyPointer_Set( oy, 0,lcmsPROFILE, s, CMMToString_M(CMMProfileOpen_M),
                          lcmsCMMProfileReleaseWrap );
  }

  if(!error)
  {
    s = lcmsCMMProfile_GetWrap_( cmm_ptr );
    error = !s;
  }

  if(!error)
    hp = s->lcms;

  oyPointer_Release( &cmm_ptr );
  if(hash_text)
    oyFree_m_(hash_text);

  if(!error)
    return hp;
  else
    return 0;
}


/** Function lcmsAddProfile
 *  @brief   add a profile from Oyranos to the lcms profile stack 
 *
 *  Look in the Oyranos cache for a CMM internal representation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
cmsHPROFILE  lcmsAddProfile          ( oyProfile_s       * p )
{
  int error = 0;
  cmsHPROFILE * hp = 0;
  oyPointer_s * cmm_ptr = 0;
  lcmsProfileWrap_s * s = 0;

  if(!p || p->type_ != oyOBJECT_PROFILE_S)
  {
    lcms_msg( oyMSG_WARN, (oyStruct_s*)p, OY_DBG_FORMAT_" "
             "no profile provided", OY_DBG_ARGS_ );
    return 0;
  }

  cmm_ptr = oyPointer_LookUpFromObject( (oyStruct_s*)p, lcmsPROFILE );

  if(!cmm_ptr)
  {
    lcms_msg( oyMSG_WARN, (oyStruct_s*)p,
             OY_DBG_FORMAT_" oyPointer_LookUpFromObject() failed", OY_DBG_ARGS_ );
    return 0;
  }

  oyPointer_Set( cmm_ptr, CMM_NICK, 0,0,0,0 );

  if(!oyPointer_GetPointer( cmm_ptr ))
    error = lcmsCMMData_Open( (oyStruct_s*)p, cmm_ptr );

  if(!error)
  {
    s = lcmsCMMProfile_GetWrap_( cmm_ptr );
    error = !s;
  }

  if(!error)
    hp = s->lcms;

  oyPointer_Release( &cmm_ptr );

  if(!error)
    return hp;
  else
    return 0;
}

int
gamutCheckSampler(register WORD In[],
                      register WORD Out[],
                      register LPVOID Cargo)
{
  cmsCIELab Lab1, Lab2;
  double d;
  oyPointer * ptr = (oyPointer*)Cargo;

  lcmsLabEncoded2Float(&Lab1, In);
  lcmsDoTransform( ptr[0], &Lab1, &Lab2, 1 );
  d = lcmsDeltaE( &Lab1, &Lab2 );
  if(abs(d) > 10 && ptr[1])
  {
    Lab2.L = 50.0;
    Lab2.a = Lab2.b = 0.0;
  }
  lcmsFloat2LabEncoded(Out, &Lab2); 

  return TRUE;
}

/** Function lcmsGamutCheckAbstract
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
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
cmsHPROFILE  lcmsGamutCheckAbstract  ( oyProfile_s       * proof,
                                       DWORD               flags,
                                       int                 intent,
                                       int                 intent_proof )
{
      size_t size = 0;
      char * data = 0;
      cmsHPROFILE gmt = 0,
                  hLab = 0,
                  hproof = 0;
      cmsHTRANSFORM tr1 = 0;
      LPLUT gmt_lut = 0;
      oyPointer ptr[2] = {0,0};

      if(!(flags & cmsFLAGS_GAMUTCHECK || flags & cmsFLAGS_SOFTPROOFING))
        return gmt;

      hLab  = lcmsCreateLabProfile(lcmsD50_xyY());
      hproof = lcmsAddProfile( proof );

      tr1 = lcmsCreateProofingTransform  (hLab, TYPE_Lab_DBL,
                                               hLab, TYPE_Lab_DBL,
                                               hproof,
                                               intent,
            /* TODO The INTENT_ABSOLUTE_COLORIMETRIC should lead to 
               paper simulation, but does take white point into account.
               Do we want this?
             */
                                               intent_proof,
                                               flags | cmsFLAGS_HIGHRESPRECALC);
      ptr[0] = tr1;
      ptr[1] = flags & cmsFLAGS_GAMUTCHECK ? (oyPointer)1 : 0;


      gmt_lut = lcmsAllocLUT();
      lcmsAlloc3DGrid( gmt_lut, lcmsPROOF_LUT_GRID_RASTER, 3, 3);
      lcmsSample3DGrid( gmt_lut, gamutCheckSampler, &ptr, 0 );

      gmt = l_cmsCreateProfilePlaceholder();
      lcmsSetDeviceClass( gmt, icSigAbstractClass );
      lcmsSetColorSpace( gmt, icSigLabData );
      lcmsSetPCS( gmt, icSigLabData );
      lcmsAddTag( gmt, icSigProfileDescriptionTag, (char*)"proofing");
      lcmsAddTag( gmt, icSigCopyrightTag, (char*)"no copyright; use freely" );
      lcmsAddTag( gmt, icSigMediaWhitePointTag, lcmsD50_XYZ() );
      lcmsAddTag( gmt, icSigAToB0Tag, gmt_lut );

  if(oy_debug)
  {
      l_cmsSaveProfileToMem( gmt, 0, &size );
      data = oyAllocateFunc_( size );
      l_cmsSaveProfileToMem( gmt, data, &size );
      oyWriteMemToFile_( "dbg_dl_proof.icc", data, size );
      if(data) oyDeAllocateFunc_( data ); data = 0;
  }

      if(hLab) lcmsCloseProfile( hLab ); hLab = 0;
      if(tr1) lcmsDeleteTransform( tr1 ); tr1 = 0;
      if(gmt_lut) lcmsFreeLUT( gmt_lut ); gmt_lut = 0;

  oyProfile_Release( &proof );

  return gmt;
}

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/21 (Oyranos: 0.3.0)
 *  @date    2011/02/21
 */
int          lcmsMOptions_Handle2    ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;
  oyProfile_s * prof = 0,
              * p = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"create_profile", 0))
    {
      p = (oyProfile_s*) oyOptions_GetType( options,-1, "proofing_profile",
                                            oyOBJECT_PROFILE_S );
      if(!p)
      {
        error = -1;
      }

      oyProfile_Release( &p );

      return error;
    }
    else
      return -1;
  }
  else if(oyFilterRegistrationMatch(command,"create_profile", 0))
  {
    p = (oyProfile_s*) oyOptions_GetType( options,-1, "proofing_profile",
                                          oyOBJECT_PROFILE_S );
    if(p)
    {
      int intent = lcmsIntentFromOptions( options,0 ),
      intent_proof = lcmsIntentFromOptions( options,1 ),
      flags = lcmsFlagsFromOptions( options );
      oyOption_s * o;
      size_t size = 0;
      char * block = 0;

      cmsHPROFILE hp = lcmsAddProofProfile( p, flags | cmsFLAGS_GAMUTCHECK,
                                            intent, intent_proof );
      oyProfile_Release( &p );
      if(hp)
      {
        l_cmsSaveProfileToMem( hp, 0, &size );
        block = oyAllocateFunc_( size );
        l_cmsSaveProfileToMem( hp, block, &size );
        lcmsCloseProfile( hp ); hp = 0;
      }

      prof = oyProfile_FromMem( size, block, 0, 0 );
      if(block && size)
        free(block); block = 0; size = 0;

      o = oyOption_FromRegistration( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_profile.create_profile.proofing_effect._" CMM_NICK,
                        0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &prof );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    } else
        lcms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ " "
                 "no option \"proofing_effect\" of type oyProfile_s found",
                 OY_DBG_ARGS_ );
  }

  return 0;
}


/** Function lcmsFilterNode_CmmIccContextToMem
 *  @brief   implement oyCMMFilterNode_CreateContext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
oyPointer lcmsFilterNode_CmmIccContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  /*int error = !node || !size;*/
  oyPointer block = 0;
  int error = 0;
  int n,i,len;
  oyDATATYPE_e data_type = 0;
  size_t size_ = 0;
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, 0 ),
                   * remote_socket = oyFilterPlug_GetSocket( plug );
  oyImage_s * image_input = 0,
            * image_output = 0;
  cmsHPROFILE * lps = 0;
  cmsHTRANSFORM xform = 0;
  oyOption_s * o = 0;
  oyOptions_s * node_tags = oyFilterNode_GetTags( node ),
              * node_options = oyFilterNode_GetOptions( node, 0 );
  oyProfile_s * p = 0,
              * prof = 0,
              * image_input_profile,
              * image_output_profile;
  oyProfiles_s * profiles = 0,
               * profs = 0;
  oyProfileTag_s * psid = 0,
                 * info = 0,
                 * cprt = 0;
  int profiles_n = 0,
      profiles_proof_n = 0,
      proof = 0;
  int verbose = oyOptions_FindString( node_tags, "verbose", "true" ) ? 1 : 0;
  const char * o_txt = 0;

  image_input = (oyImage_s*)oyFilterSocket_GetData( remote_socket );
  image_output = (oyImage_s*)oyFilterSocket_GetData( socket );
  image_input_profile = oyImage_GetProfile( image_input );
  image_output_profile = oyImage_GetProfile( image_output );

  if(!image_input)
    return 0;

  if(image_input->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    lcms_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" missed input image %d", OY_DBG_ARGS_,
             image_input->type_ );
  }
  if(!image_output || image_output->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    lcms_msg( oyMSG_WARN, (oyStruct_s*)node,
              OY_DBG_FORMAT_" missed output image %d", OY_DBG_ARGS_,
              image_output?image_output->type_:0 );
  }

  data_type = oyToDataType_m( oyImage_GetPixelLayout( image_input, oyLAYOUT ) );

  if(data_type == oyFLOAT)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    lcms_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" can not handle oyFLOAT", OY_DBG_ARGS_ );
  }

  len = sizeof(cmsHPROFILE) * (15 + 2 + 1);
  lps = oyAllocateFunc_( len );
  memset( lps, 0, len );

  /* input profile */
  lps[ profiles_n++ ] = lcmsAddProfile( image_input_profile );
  if(!image_input_profile)
  {
    lcms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_" "
             "missed image_input->profile_", OY_DBG_ARGS_ );
    return 0;
  }
  p = oyProfile_Copy( image_input_profile, 0 );
  profs = oyProfiles_New( 0 );
  oyProfiles_MoveIn( profs, &p, -1 );

  /* effect profiles */
  o = oyOptions_Find( node_options, "profiles_effect" );
  if(o)
  {
    profiles = (oyProfiles_s*) oyOption_GetStruct( o, oyOBJECT_PROFILES_S );
    if( !profiles )
    {
      oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION );
      lcms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_
               " incompatible \"profiles_effect\"", OY_DBG_ARGS_ );
      
    } else
    {
      n = oyProfiles_Count( profiles );
      for(i = 0; i < n; ++i)
      {
        p = oyProfiles_Get( profiles, i );

        /* Look in the Oyranos cache for a CMM internal representation */
        lps[ profiles_n++ ] = lcmsAddProfile( p );
        oyProfiles_MoveIn( profs, &p, -1 );
      }
    }
    oyOption_Release( &o );
  }

  /* simulation profile */
  o = oyOptions_Find( node_options, "profiles_simulation" );
  o_txt = oyOptions_FindString  ( node_options, "proof_soft", 0 );
  if(o_txt && oyStrlen_(o_txt)/* && profile_class_out== icSigDisplayClass*/)
    proof = atoi( o_txt );

  o_txt = oyOptions_FindString  ( node_options, "proof_hard", 0 );
  if(o_txt && oyStrlen_(o_txt)/* && profile_class_out== icSigOutputClass*/)
    proof = atoi( o_txt ) ? atoi(o_txt) : proof;

  if(oy_debug && proof)
      lcms_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_
               " proof requested",OY_DBG_ARGS_);

  if(o)
  {
    profiles = (oyProfiles_s*) oyOption_GetStruct( o, oyOBJECT_PROFILES_S );
    if( !profiles )
    {
      oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION );
      lcms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_
               " incompatible \"profiles_simulation\"",OY_DBG_ARGS_);
      
    } else
    {
      n = oyProfiles_Count( profiles );

      lcms_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
               " %d simulation profile(s) found \"%s\"",
               OY_DBG_ARGS_, n,
               profiles?oyStruct_TypeToText((oyStruct_s*)profiles):"????");

      for(i = 0; i < n; ++i)
      {
        p = oyProfiles_Get( profiles, i );

        lcms_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
                 " found profile: %s",
                 OY_DBG_ARGS_, p?oyProfile_GetFileName( p,-1 ):"????");

        oyProfiles_MoveIn( profs, &p, -1 );
        ++profiles_proof_n;

        oyProfile_Release( &p );
      }
    }
    oyOption_Release( &o );
  } else if(verbose || oy_debug)
    lcms_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
             " no simulation profile found", OY_DBG_ARGS_);


  /* output profile */
  if(!image_output_profile)
  {
    lcms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_" "
             "missed image_output->profile_", OY_DBG_ARGS_ );
    return 0;
  }
  lps[ profiles_n++ ] = lcmsAddProfile( image_output_profile );
  p = oyProfile_Copy( image_output_profile, 0 );
  oyProfiles_MoveIn( profs, &p, -1 );

  *size = 0;

  /* create the context */
  xform = lcmsCMMConversionContextCreate_( lps, profiles_n,
                                           profiles, profiles_proof_n, proof,
                                oyImage_GetPixelLayout( image_input, oyLAYOUT ),
                                oyImage_GetPixelLayout( image_output, oyLAYOUT ),
                                           node_options, 0, 0);
  error = !xform;


  if(!error)
  {
    if(oy_debug)
      block = lcmsCMMColorConversion_ToMem_( xform, size, oyAllocateFunc_ );
    else
      block = lcmsCMMColorConversion_ToMem_( xform, size, allocateFunc );
    error = !block && !*size;
    lcmsDeleteTransform( xform ); xform = 0;
  }

  /* additional tags for debugging */
  if(!error && (oy_debug || verbose))
  {
    if(!error && size)
    {
      size_ = *size;

      prof = oyProfile_FromMem( size_, block, 0, 0 );
      psid = oyProfile_GetTagById( prof, icSigProfileSequenceIdentifierTag );

      /* icSigProfileSequenceIdentifierType */
      if(!psid)
      {
        oyStructList_s * list = oyStructList_New(0);
        int i, n = oyProfiles_Count( profs );
        for( i = 0; i < n ; ++i )
        {
           oyProfile_s * p = oyProfiles_Get( profs, i );
           oyStructList_MoveIn( list, (oyStruct_s**) &p, -1, 0 );
        }
        psid = oyProfileTag_Create( list, icSigProfileSequenceIdentifierTag,
                     icSigProfileSequenceIdentifierType, OY_MODULE_NICK, 0 );
        if(psid)
          error = oyProfile_TagMoveIn ( prof, &psid, -1 );

        oyStructList_Release( &list );
      }

      /* Info tag */
      if(!error)
      {
        oyStructList_s * list = 0;
        char h[5] = {"Info"};
        uint32_t * hi = (uint32_t*)&h;
        char * cc_name = lcmsFilterNode_GetText( node, oyNAME_NICK,
                                                 oyAllocateFunc_ );
        oyName_s * name = oyName_new(0);
        const char * lib_name = oyFilterNode_GetModuleName( node );

        name = oyName_set_ ( name, cc_name, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        name = oyName_set_ ( name, lib_name, oyNAME_NICK,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        oyDeAllocateFunc_( cc_name );
        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list,  (oyStruct_s**) &name, -1, 0 );

        if(!error)
        {
          info = oyProfileTag_Create( list, (icTagSignature)oyValueUInt32(*hi),
                                      icSigTextType, OY_MODULE_NICK, 0);
          error = !info;
        }

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
        /* FIXME replace oyName_s with oyOption_s */
        oyName_s * name = oyName_new(0);

        name = oyName_set_ ( name, c_text, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list, (oyStruct_s**) &name, -1, 0 );

        if(!error)
        {
          cprt = oyProfileTag_Create( list, icSigCopyrightTag,
                                      icSigTextType, OY_MODULE_NICK, 0);
          error = !cprt;
        }

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

  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterSocket_Release( & remote_socket );
  oyOptions_Release( &node_tags );
  oyImage_Release( &image_input );
  oyImage_Release( &image_output );
  oyProfile_Release( &image_input_profile );
  oyProfile_Release( &image_output_profile );
  oyOptions_Release( &node_options );

  return block;
}

char * lcmsImage_GetText             ( oyImage_s         * image,
                                       int                 verbose,
                                       oyAlloc_f           allocateFunc )
{
  oyPixel_t pixel_layout = oyImage_GetPixelLayout(image,oyLAYOUT); 
  int n     = oyToChannels_m( pixel_layout );
  oyProfile_s * profile = oyImage_GetProfile( image );
  int cchan_n = oyProfile_GetChannelsCount( profile );
  int coff_x = oyToColorOffset_m( pixel_layout );
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  int swap  = oyToSwapColorChannels_m( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  int so = oyDataTypeGetSize( t );
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
  oySprintf_( text,   "    <channels all=\"%d\" color=\"%d\" />\n", n,cchan_n);
  hashTextAdd_m( text );
  oySprintf_( text,
                      "    <offsets first_color_sample=\"%d\" next_pixel=\"%d\" />\n"
              /*"  next line = %d\n"*/,
              coff_x, oyImage_GetPixelLayout( s,oyPOFF_X )/*, mask[oyPOFF_Y]*/ );
  hashTextAdd_m( text );

  if(swap || oyToByteswap_m( pixel_layout ))
  {
    hashTextAdd_m(    "    <swap" );
    if(swap)
      hashTextAdd_m(  " colorswap=\"yes\"" );
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
                    oyDataTypeToText(t), so );
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

/** Function lcmsFilterNode_GetText
 *  @brief   implement oyCMMFilterNode_GetText_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2009/06/02
 */
char * lcmsFilterNode_GetText        ( oyFilterNode_s    * node,
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
  oyOptions_s * opts = oyFilterNode_GetOptions( node, 0 );
  oyOptions_s * node_tags = oyFilterNode_GetTags( node ),
              * opts_tmp, * opts_tmp2, * options;
  oyFilterCore_s * node_core = oyFilterNode_GetCore( node );
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, 0 ),
                   * remote_socket = oyFilterPlug_GetSocket( plug );

  /* pick all sockets (output) data */
  out_image = (oyImage_s*)oyFilterSocket_GetData( remote_socket );
  /* pick all plug (input) data */
  in_image = (oyImage_s*)oyFilterSocket_GetData( socket );

  if(!node)
    return 0;

  verbose = oyOptions_FindString( node_tags, "verbose", "true" ) ? 1 : 0;

  /* 1. create hash text */
  hashTextAdd_m( "<oyFilterNode_s>\n  " );

  /* the filter text */
  hashTextAdd_m( oyFilterCore_GetText( node_core, oyNAME_NAME ) );

  /* make a description */
  {
    /* input data */
    hashTextAdd_m(   " <data_in>\n" );
    if(in_image)
    {
      temp = lcmsImage_GetText( in_image, verbose, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyDeAllocateFunc_(temp); temp = 0;
    }
    hashTextAdd_m( "\n </data_in>\n" );

    /* pick inbuild defaults */
    opts_tmp2 = oyOptions_FromText( lcms_extra_options, 0, NULL );
    opts_tmp = oyOptions_ForFilter( "//" OY_TYPE_STD "/icc_color",
                                oyOPTIONSOURCE_FILTER | OY_SELECT_COMMON , 0 );
    options = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION,NULL);
    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );
    opts_tmp = options;
    /* add existing custom options */
    options = oyOptions_FromBoolean( opts_tmp, opts, oyBOOLEAN_UNION,NULL);
    oyOptions_Release( &opts_tmp );

    /* options -> xforms */
    hashTextAdd_m(   " <oyOptions_s>\n" );
    model = oyOptions_GetText( opts, oyNAME_NAME );
    hashTextAdd_m( model );
    hashTextAdd_m( "\n </oyOptions_s>\n" );

    /* output data */
    hashTextAdd_m(   " <data_out>\n" );
    if(out_image)
    {
      temp = lcmsImage_GetText( out_image, verbose, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyDeAllocateFunc_(temp); temp = 0;
    }
    hashTextAdd_m( "\n </data_out>\n" );
  }
  hashTextAdd_m( tmp );

  hashTextAdd_m(   "</oyFilterNode_s>\n" );

  oyOptions_Release( &opts );
  oyOptions_Release( &node_tags );
  oyFilterCore_Release( &node_core );
  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterSocket_Release( &remote_socket );

  return oyStringCopy_( hash_text, allocateFunc );
#endif
}

/** Function lcmsModuleData_Convert
 *  @brief   convert between data formats
 *  @ingroup cmm_handling
 *
 *  The function might be used to provide a module specific context.
 *  Implements oyModuleData_Convert_f
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int  lcmsModuleData_Convert          ( oyPointer_s       * data_in,
                                       oyPointer_s       * data_out,
                                       oyFilterNode_s    * node )
{
  int error = !data_in || !data_out;
  oyPointer_s * cmm_ptr_in = data_in,
             * cmm_ptr_out = data_out;
  lcmsTransformWrap_s * ltw  = 0;
  cmsHTRANSFORM xform = 0;
  cmsHPROFILE lps[2] = {0,0};
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, 0 ),
                   * remote_socket = oyFilterPlug_GetSocket( plug );
  oyOptions_s * node_options = oyFilterNode_GetOptions( node, 0 );
  oyImage_s * image_input = (oyImage_s*)oyFilterSocket_GetData( remote_socket ),
            * image_output = (oyImage_s*)oyFilterSocket_GetData( socket );

  if(!error)
  {
    cmm_ptr_in = (oyPointer_s*) data_in;
    cmm_ptr_out = (oyPointer_s*) data_out;
  }

  if(!error &&
     ( (strcmp( oyPointer_GetResourceName(cmm_ptr_in), oyCOLOR_ICC_DEVICE_LINK ) != 0) ||
       (strcmp( oyPointer_GetResourceName(cmm_ptr_out), lcmsTRANSFORM ) != 0) ) )
    error = 1;

  if(!error)
  {
    lps[0] = CMMProfileOpen_M( oyPointer_GetPointer(cmm_ptr_in),
                               oyPointer_GetSize( cmm_ptr_in ) );
    xform = lcmsCMMConversionContextCreate_( lps, 1, 0,0,0,
                                oyImage_GetPixelLayout( image_input, oyLAYOUT ),
                                oyImage_GetPixelLayout( image_output, oyLAYOUT ),
                                           node_options,
                                           &ltw, cmm_ptr_out );
    if(!xform)
    {
      uint32_t f = oyImage_GetPixelLayout( image_input, oyLAYOUT );
      lcms_msg( oyMSG_WARN,(oyStruct_s*) node, OY_DBG_FORMAT_
      "colorspace:%d extra:%d channels:%d lcms_bytes%d",
      OY_DBG_ARGS_,
      T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
      error = 1;
    }
    CMMProfileRelease_M (lps[0] );
  }
  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterSocket_Release( & remote_socket );
  oyImage_Release( &image_input );
  oyImage_Release( &image_output );
  oyOptions_Release( &node_options );

  return error;
}

/** Function lcmsFilterPlug_CmmIccRun
 *  @brief   implement oyCMMFilterPlug_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2009/05/01
 */
int      lcmsFilterPlug_CmmIccRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int j, k, n;
  int error = 0;
  int channels = 0;
  oyDATATYPE_e data_type_in = 0,
               data_type_out = 0;
  int bps_in;
#if defined(DEBUG)
  oyPixel_t pixel_layout_in;
#endif

  oyFilterSocket_s * socket = oyFilterPlug_GetSocket( requestor_plug );
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * input_node,
                 * node = oyFilterSocket_GetNode( socket );
  oyImage_s * image_input = 0, * image_output = 0;
  oyArray2d_s * array_in = 0, * array_out = 0;
  lcmsTransformWrap_s * ltw  = 0;
  oyPixelAccess_s * new_ticket = ticket;

  plug = oyFilterNode_GetPlug( node, 0 );
  input_node = oyFilterNode_GetPlugNode( node, 0 );

  image_input = oyFilterPlug_ResolveImage( plug, socket, ticket );
#if defined(DEBUG)
  pixel_layout_in = oyImage_GetPixelLayout( image_input, oyLAYOUT );
#endif

  image_output = oyPixelAccess_GetOutputImage( ticket );

  if(oyImage_GetPixelLayout( image_input, oyLAYOUT ) != 
     oyImage_GetPixelLayout( image_output, oyLAYOUT ))
  {
    /* adapt the region of interesst to the new image dimensions */
    /* create a new ticket to avoid pixel layout conflicts */
    oyRectangle_s * new_roi = oyPixelAccess_GetOutputROI( new_ticket );
    oyArray2d_s * a = 0;
    new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
    oyPixelAccess_SetArray( new_ticket, 0 );
    oyPixelAccess_SetOutputImage( new_ticket, image_input );
    error = oyImage_FillArray( image_input, new_roi, 1,
                               &a, 0, 0 );
    oyPixelAccess_SetArray( new_ticket, a );
    oyArray2d_Release( &a );
    oyRectangle_Release( & new_roi );
  }

  /* We let the input filter do its processing first. */
  error = oyFilterNode_Run( input_node, plug, new_ticket );
  if(error != 0) return error;

  array_in = oyPixelAccess_GetArray( new_ticket );
  array_out = oyPixelAccess_GetArray( ticket );

  data_type_in = oyToDataType_m( oyImage_GetPixelLayout( image_input, oyLAYOUT ) );
  bps_in = oyDataTypeGetSize( data_type_in );

  if(data_type_in == oyFLOAT)
  {
    oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    lcms_msg(oyMSG_WARN,0, OY_DBG_FORMAT_" can not handle oyFLOAT",OY_DBG_ARGS_);
    error = 1;
  }

  if(!image_output)
  {
    lcms_msg( oyMSG_WARN,0, OY_DBG_FORMAT_ " no ticket->output_image",
             OY_DBG_ARGS_);
    error = 1;
  }

  if(!error)
  {
    oyPointer_s * backend_data = oyFilterNode_GetContext( node );
    data_type_out = oyToDataType_m( oyImage_GetPixelLayout( image_output, oyLAYOUT ) );
    channels = oyToChannels_m( oyImage_GetPixelLayout( image_output, oyLAYOUT ) );

    error = lcmsCMMTransform_GetWrap_( backend_data, &ltw );
    oyPointer_Release( &backend_data );
  }

  DBG_NUM2_S( "channels in/out: %d->%d",
              oyToChannels_m( pixel_layout_in ), channels );

  if(ltw && !array_out)
  {
    lcms_msg( oyMSG_ERROR,0, OY_DBG_FORMAT_ " no ticket->array",
             OY_DBG_ARGS_);
    error = 1;
  }

  /* now do some position blind manipulations */
  if(ltw && error <= 0)
  {
    uint8_t * array_in_tmp = 0,
            * array_out_tmp = 0;
    float * array_in_tmp_flt = 0,
          * array_out_tmp_flt = 0;
    double * array_in_tmp_dbl = 0,
           * array_out_tmp_dbl = 0;
    uint8_t ** array_in_data = oyArray2d_GetData( array_in ),
            ** array_out_data = oyArray2d_GetData( array_out );
    int threads_n = 
#if defined(_OPENMP) && defined(USE_OPENMP)
                    omp_get_max_threads();
#else
                    1;
#endif
    int w_in =  (int)(oyArray2d_GetWidth(array_in)+0.5),
        w_out = (int)(oyArray2d_GetWidth(array_out)+0.5);
    int stride_in = w_in * bps_in;
    double xyz_factor_in = 1.0,
           xyz_factor_out = 1.0;

    n = w_out / channels;

    lcms_msg( oyMSG_DBG,(oyStruct_s*)requestor_plug, OY_DBG_FORMAT_
             " threads_n: %d",
             OY_DBG_ARGS_, threads_n );

    if(!(data_type_in == oyUINT8 ||
         data_type_in == oyUINT16 ||
         data_type_in == oyDOUBLE))
    {
      oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
      error = 1;
    }
    /* format for PT_ANY from lcms */
    if((data_type_in == oyFLOAT ||
        data_type_in == oyDOUBLE))
    {
      if(ltw->sig_in  == icSigXYZData)
        xyz_factor_in = 1.0 + 32767.0/32768.0;

      array_in_tmp = oyAllocateFunc_( stride_in * threads_n );
      if(data_type_in == oyFLOAT)
        array_in_tmp_flt = (float*) array_in_tmp;
      else if(data_type_in == oyDOUBLE)
        array_in_tmp_dbl = (double*) array_in_tmp;
    }
    if((data_type_out == oyFLOAT ||
        data_type_out == oyDOUBLE))
    {
      if(ltw->sig_out  == icSigXYZData)
        xyz_factor_out = 1.0 + 32767.0/32768.0;
      array_out_tmp = array_out_data[0];
    }
    

    /*  - - - - - conversion - - - - - */
    /*lcms_msg(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d Start lines: %d",
            __FILE__,__LINE__, array_out->height);*/
    if(!error)
    {
      const int use_xyz_scale = 1;
      int index = 0;
      int array_out_height = oyArray2d_GetHeight(array_out);
      if(array_out_height > threads_n * 10)
      {
#if defined(USE_OPENMP)
#pragma omp parallel for private(index,j,array_in_tmp_flt,array_in_tmp_dbl,array_out_tmp_flt,array_out_tmp_dbl)
#endif
        for( k = 0; k < array_out_height; ++k)
        {
          if(array_in_tmp && use_xyz_scale)
          {
#if defined(_OPENMP) && defined(USE_OPENMP)
            index = omp_get_thread_num();
#endif
            memcpy( &array_in_tmp[stride_in*index], array_in_data[k],
                    w_in * bps_in );
            if(data_type_in == oyFLOAT)
            {
              array_in_tmp_flt = (float*) &array_in_tmp[stride_in*index];
              for(j = 0; j < w_in; ++j)
              {
                array_in_tmp_flt[j] *= 100.0 / xyz_factor_in;
              }
            } else
            if(data_type_in == oyDOUBLE)
            {
              array_in_tmp_dbl = (double*) &array_in_tmp[stride_in*index];
              for(j = 0; j < w_in; ++j)
              {
                array_in_tmp_dbl[j] *= 100.0 / xyz_factor_in;
              }
            }
            lcmsDoTransform( ltw->lcms, &array_in_tmp[stride_in*index],
                                       array_out_data[k], n );
          } else
            lcmsDoTransform( ltw->lcms, array_in_data[k],
                                       array_out_data[k], n );
          if(array_out_tmp && use_xyz_scale)
          {
            if(data_type_out == oyFLOAT)
            {
              array_out_tmp_flt = (float*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_flt[j] *= xyz_factor_out / 100.0;
            } else
            if(data_type_out == oyDOUBLE)
            {
              array_out_tmp_dbl = (double*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_dbl[j] *= xyz_factor_out / 100.0;
            }
          }
        }
      } else
        for( k = 0; k < array_out_height; ++k)
        {
          if(array_in_tmp && use_xyz_scale)
          {
            memcpy( array_in_tmp, array_in_data[k], w_in * bps_in );
            if(data_type_in == oyFLOAT)
            for(j = 0; j < w_in; ++j)
            {
              array_in_tmp_flt[j] *= 100.0 / xyz_factor_in;
            }
            if(data_type_in == oyDOUBLE)
            for(j = 0; j < w_in; ++j)
            {
              array_in_tmp_dbl[j] *= 100.0 / xyz_factor_in;
            }
            lcmsDoTransform( ltw->lcms, array_in_tmp,
                                       array_out_data[k], n );
          } else
            lcmsDoTransform( ltw->lcms, array_in_data[k],
                                       array_out_data[k], n );
          if(array_out_tmp && use_xyz_scale)
          {
            if(data_type_out == oyFLOAT)
            {
              array_out_tmp_flt = (float*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_flt[j] *= xyz_factor_out / 100.0;
            } else
            if(data_type_out == oyDOUBLE)
            {
              array_out_tmp_dbl = (double*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_dbl[j] *= xyz_factor_out / 100.0;
            }
          }
        }
    /*message(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d End width: %d",
            __FILE__,__LINE__, n);*/
    }

    if(array_in_tmp)
      oyDeAllocateFunc_( array_in_tmp );

  } else
  {
    oyFilterGraph_s * ticket_graph = oyPixelAccess_GetGraph( ticket );
    oyOptions_s * ticket_graph_opts = 
                                       oyFilterGraph_GetOptions( ticket_graph );
    if(error)
      oyFilterSocket_Callback( requestor_plug,
                               oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT );
    else
      oyFilterSocket_Callback( requestor_plug,
                               oyCONNECTOR_EVENT_OK );

    error = oyOptions_SetFromText( &ticket_graph_opts,
                     "//" OY_TYPE_STD "/profile/dirty", "true", OY_CREATE_NEW );
    oyFilterGraph_Release( &ticket_graph );
    oyOptions_Release( &ticket_graph_opts );
    error = 1;
  }

  if(oyImage_GetPixelLayout( image_input, oyLAYOUT ) != 
     oyImage_GetPixelLayout( image_output, oyLAYOUT ))
    oyPixelAccess_Release( &new_ticket );

  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &input_node );
  oyFilterNode_Release( &node );
  oyImage_Release( &image_input );
  oyImage_Release( &image_output );
  oyArray2d_Release( &array_in );
  oyArray2d_Release( &array_out );

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

/** Function lcmsErrorHandlerFunction
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int lcmsErrorHandlerFunction(int ErrorCode, const char *ErrorText)
{
  int code = 0;
  switch(ErrorCode) {
  case LCMS_ERRC_WARNING: code = oyMSG_WARN; break;
  case LCMS_ERRC_RECOVERABLE: code = oyMSG_WARN; break;
  case LCMS_ERRC_ABORTED: code = oyMSG_ERROR; break;
  default: code = ErrorCode;
  }
  lcms_msg( code, 0, ErrorText, 0 );
  return 0;
}

/** Function lcmsCMMMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int            lcmsCMMMessageFuncSet ( oyMessage_f         message_func )
{
  lcms_msg = message_func;
  return 0;
}

char lcms_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "icc_color" ">\n\
      <cmyk_cmyk_black_preservation.advanced>0</cmyk_cmyk_black_preservation.advanced>\n\
      <precalculation.advanced>0</precalculation.advanced>\n\
     </" "icc_color" ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

#define A(long_text) STRING_ADD( tmp, long_text)

/** Function lcmsGetOptionsUI
 *  @brief   return XFORMS for matching options
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/01/08
 *  @since   2009/07/29 (Oyranos: 0.1.10)
 */
int lcmsGetOptionsUI                 ( oyCMMapiFilter_s   * module,
                                       oyOptions_s        * options,
                                       char              ** ui_text,
                                       oyAlloc_f            allocateFunc )
{
  char * tmp = 0;

  tmp = (char *)oyOptions_FindString( options,
                                      "cmyk_cmyk_black_preservation", 0 );
  if(tmp == 0)
    return 0;

  tmp = oyStringCopy_( "\
  <xf:group type=\"h3\">\
    <xf:label>little CMS ", oyAllocateFunc_ );

  A(       _("Extended Options"));
  A(                         ":</xf:label>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc_color/cmyk_cmyk_black_preservation\">\n\
      <xf:label>" );
  A(          _("Black Preservation"));
  A(                              "</xf:label>\n\
      <xf:hint>" );
  A(          _("Decide how to preserve the black channel for Cmyk to Cmyk transforms"));
  A(                              "</xf:hint>\n\
      <xf:help>" );
  A(          _("Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane."));
  A(                              "</xf:help>\n\
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
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc_color/precalculation\">\n\
      <xf:label>" );
  A(          _("Optimization"));
  A(                              "</xf:label>\n\
      <xf:hint>" );
  A(          _("Color Transforms can be differently stored internally"));
  A(                              "</xf:hint>\n\
      <xf:help>" );
  A(          _("Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags."));
  A(                              "</xf:help>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0</xf:value>\n\
        <xf:label>LCMS_NOOPTIMIZE</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1</xf:value>\n\
        <xf:label>normal</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>2</xf:value>\n\
        <xf:label>LCMS_HIGHRESPRECALC</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>3</xf:value>\n\
        <xf:label>LCMS_LOWRESPRECALC</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
   </xf:group>\n");

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

/** Function lcmsCreateICCMatrixProfile
 *  @brief   ICC from EDID
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/24 (Oyranos: 0.1.10)
 *  @date    2009/12/10
 */
oyProfile_s *      lcmsCreateICCMatrixProfile (
                                       float             gamma,
                                       float rx, float ry,
                                       float gx, float gy,
                                       float bx, float by,
                                       float wx, float wy)
{
  cmsCIExyYTRIPLE p;
  LPGAMMATABLE g[3] = {0,0,0};
  /* 0.31271, 0.32902 D65 */
  cmsCIExyY wtpt_xyY;
  cmsHPROFILE lp = 0;
  size_t size = 0;
  char * data = 0;

  int error = 0;
  oyProfile_s * prof = 0;

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
  g[0] = g[1] = g[2] = lcmsBuildGamma(1, (double)gamma);
  lcms_msg( oyMSG_DBG,0, OY_DBG_FORMAT_
             " red: %g %g %g green: %g %g %g blue: %g %g %g white: %g %g gamma: %g",
             OY_DBG_ARGS_, rx,ry,p.Red.Y, gx,gy,p.Green.Y,bx,by,p.Blue.Y,wx,wy,gamma );
  lp = lcmsCreateRGBProfile( &wtpt_xyY, &p, g);

  l_cmsSaveProfileToMem( lp, 0, &size );
  data = oyAllocateFunc_( size );
  l_cmsSaveProfileToMem( lp, data, &size );
  lcmsCloseProfile( lp );
  lcmsFreeGamma( g[0] );

  prof = oyProfile_FromMem( size, data, 0,0 );


  error = oyProfile_AddTagText( prof, icSigCopyrightTag,
                                      "no copyright; use freely" );
  if(error) WARNc2_S("%s %d", _("found issues"),error);

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
int          lcmsMOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  oyOption_s * o = 0;
  oyProfile_s * prof = 0;
  int error = 0;
  double val = 0.0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"create_profile", 0))
    {
      o = oyOptions_Find( options, "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma" );
      error = oyOptions_FindDouble( options,
        "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                            8, &val );
      if(!o)
      {
        error = -1;
      } else if( error != 0 )
      {
        lcms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_" "
                 "option \"color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" %s",
                 OY_DBG_ARGS_,
                 (error < 0) ? "contains less than 9 required values" :
                               "access returned with error" );
      }

      oyOption_Release( &o );

      return error;
    }
    else
      return -1;
  }
  else if(oyFilterRegistrationMatch(command,"create_profile", 0))
  {
    o = oyOptions_Find( options, "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma" );
    if(o)
    {
      error = oyOptions_FindDouble( options,
        "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                            8, &val );
      if( error != 0 )
      {
        lcms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_" "
                 "option \"color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" %s",
                 OY_DBG_ARGS_,
                 (error < 0) ? "contains less than 9 required values" :
                               "access returned with error" );
      }
      prof = lcmsCreateICCMatrixProfile (
                    oyOption_GetValueDouble(o,8),
                    oyOption_GetValueDouble(o,0), oyOption_GetValueDouble(o,1),
                    oyOption_GetValueDouble(o,2), oyOption_GetValueDouble(o,3),
                    oyOption_GetValueDouble(o,4), oyOption_GetValueDouble(o,5),
                    oyOption_GetValueDouble(o,6), oyOption_GetValueDouble(o,7));
      oyOption_Release( &o );

      o = oyOption_FromRegistration( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_profile.create_profile.color_matrix._" CMM_NICK,
                        0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &prof );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    } else
        lcms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ " "
                 "no option \"color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" found",
                 OY_DBG_ARGS_ );
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/21 (Oyranos: 0.3.0)
 *  @date    2011/02/21
 */
const char * lcmsInfoGetTextProfileC2( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
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
      return "proofing_effect";
    else if(type == oyNAME_NAME)
      return _("Create a ICC abstract proofing profile.");
    else
      return _("The littleCMS \"create_profile.proofing_effect\" command lets you create ICC abstract profiles from a given ICC profile for proofing. The filter expects a oyOption_s object with name \"proofing_profile\" containing a oyProfile_s as value. The options \"rendering_intent\", \"rendering_intent_proof\", \"rendering_bpc\", \"rendering_gamut_warning\", \"precalculation\" and \"cmyk_cmyk_black_preservation\" are honoured. The result will appear in \"icc_profile\" with the additional attributes \"create_profile.proofing_effect\" as a oyProfile_s object.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Create a ICC proofing profile.");
    else
      return _("The littleCMS \"create_profile.proofing_effect\" command lets you create ICC abstract profiles from some given ICC profile. See the \"proofing_effect\" info item.");
  }
  return 0;
}
const char *lcms_texts_profile_create[4] = {"can_handle","create_profile","help",0};

/** @instance lcms_api10_cmm2
 *  @brief    littleCMS oyCMMapi10_s implementation
 *
 *  a filter for proofing effect profile creation
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/21 (Oyranos: 0.3.0)
 *  @date    2011/02/21
 */
oyCMMapi10_s_  lcms_api10_cmm2 = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  0,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "create_profile.proofing_effect.icc._" CMM_NICK "._CPU",

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  lcmsInfoGetTextProfileC2,            /**< getText */
  (char**)lcms_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  lcmsMOptions_Handle2                 /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
const char * lcmsInfoGetTextProfileC ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
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
      return _("The littleCMS \"create_profile.color_matrix\" command lets you create ICC profiles from some given colorimetric coordinates. The filter expects a oyOption_s object with name \"color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" containing 9 floats in the order of CIE*x for red, CIE*y for red, CIE*x for green, CIE*y for green, CIE*x for blue, CIE*y for blue, CIE*x for white, CIE*y for white and a gamma value. The result will appear in \"icc_profile\" with the additional attributes \"create_profile.color_matrix\".");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Create a ICC matrix profile.");
    else
      return _("The littleCMS \"create_profile.color_matrix\" command lets you create ICC profiles from some given colorimetric coordinates. See the \"create_profile\" info item.");
  }
  return 0;
}

/** @instance lcms_api10_cmm
 *  @brief    littleCMS oyCMMapi10_s implementation
 *
 *  a filter for simple profile creation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
oyCMMapi10_s_  lcms_api10_cmm = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api10_cmm2,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "create_profile.color_matrix.icc._" CMM_NICK "._CPU",

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  lcmsInfoGetTextProfileC,             /**< getText */
  (char**)lcms_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  lcmsMOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};


/** @instance lcms_api6
 *  @brief    littleCMS oyCMMapi6_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyCMMapi6_s_ lcms_api6_cmm = {

  oyOBJECT_CMM_API6_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api10_cmm,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc_color._" CMM_NICK "._CPU." oyCOLOR_ICC_DEVICE_LINK "_" lcmsTRANSFORM,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
  
  oyCOLOR_ICC_DEVICE_LINK,  /* data_type_in, "oyDL" */
  lcmsTRANSFORM,             /* data_type_out, "lcCC" */
  lcmsModuleData_Convert     /* oyModuleData_Convert_f oyModuleData_Convert */
};


/** @instance lcms_api7
 *  @brief    littleCMS oyCMMapi7_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s_ lcms_api7_cmm = {

  oyOBJECT_CMM_API7_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api6_cmm,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc_color._" CMM_NICK "._CPU._ACCEL",

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  lcmsFilterPlug_CmmIccRun,  /* oyCMMFilterPlug_Run_f */
  lcmsTRANSFORM,             /* data_type, "lcCC" */

  (oyConnector_s**) lcms_cmmIccPlug_connectors,/* plugs */
  1,                         /* plugs_n */
  0,                         /* plugs_last_add */
  (oyConnector_s**) lcms_cmmIccSocket_connectors,   /* sockets */
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
const char * lcmsApi4UiGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  static char * category = 0;
  if(strcmp(select,"name") ||
     strcmp(select,"help"))
  {
    return lcmsInfoGetText( select, type, context );
  }
  else if(strcmp(select,"category"))
  {
    if(!category)
    {
      STRING_ADD( category, _("Color") );
      STRING_ADD( category, _("/") );
      /* CMM: abbreviation for Color Matching Module */
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
const char * lcms_api4_ui_texts[] = {"name", "category", "help", 0};
/** @instance lcms_api4_ui
 *  @brief    lcms oyCMMapi4_s::ui implementation
 *
 *  The UI for lcms.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_ lcms_api4_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                            /**< int32_t module_api[3] */

  lcmsFilter_CmmIccValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  lcmsWidgetEvent, /* oyWidgetEvent_f */

  "Color/CMM/littleCMS", /* category */
  lcms_extra_options,   /* const char * options */
  lcmsGetOptionsUI,     /* oyCMMuiGet_f oyCMMuiGet */

  lcmsApi4UiGetText, /* oyCMMGetText_f   getText */
  lcms_api4_ui_texts /* const char    ** texts */
};

/** @instance lcms_api4_cmm
 *  @brief    littleCMS oyCMMapi4_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyCMMapi4_s_ lcms_api4_cmm = {

  oyOBJECT_CMM_API4_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api7_cmm,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc_color._" CMM_NICK "._CPU._NOACCEL",

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  lcmsFilterNode_CmmIccContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  lcmsFilterNode_GetText, /* oyCMMFilterNode_GetText_f */
  oyCOLOR_ICC_DEVICE_LINK, /* context data_type */

  &lcms_api4_ui                        /**< oyCMMui_s *ui */
};



/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * lcmsInfoGetText         ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Little CMS");
    else
      return _("LittleCMS is a CMM, a color management engine; it implements fast transforms between ICC profiles. \"Little\" stands for its small overhead. With a typical footprint of about 100K including C runtime, you can color-enable your application without the pain of ActiveX, OCX, redistributables or binaries of any kind. We are using little cms in several commercial projects, however, we are offering lcms library free for anybody under an extremely liberal open source license.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Marti";
    else if(type == oyNAME_NAME)
      return "Marti Maria";
    else
      return _("littleCMS project; www: http://www.littlecms.com; support/email: support@littlecms.com; sources: http://www.littlecms.com/downloads.htm; Oyranos wrapper: Kai-Uwe Behrmann for the Oyranos project");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "MIT";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 1998-2008 Marti Maria Saguer; MIT");
    else
      return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The lcms \"color.icc\" filter is a one dimensional color conversion filter. It can both create a color conversion context, some precalculated for processing speed up, and the color conversion with the help of that context. The adaption part of this filter transforms the Oyranos color context, which is ICC device link based, to the internal lcms format.");
    else
      return _("The following options are available to create color contexts:\n \"profiles_simulation\", a option of type oyProfiles_s, can contain device profiles for proofing.\n \"profiles_effect\", a option of type oyProfiles_s, can contain abstract color profiles.\n The following Oyranos options are supported: \"rendering_gamut_warning\", \"rendering_intent_proof\", \"rendering_bpc\", \"rendering_intent\", \"proof_soft\" and \"proof_hard\".\n The additional lcms options are supported \"cmyk_cmyk_black_preservation\" [0 - none; 1 - LCMS_PRESERVE_PURE_K; 2 - LCMS_PRESERVE_K_PLANE] and \"precalculation\".");
  }
  return 0;
}
const char *lcms_texts[5] = {"name","copyright","manufacturer","help",0};
oyIcon_s lcms_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "lcms_logo2.png"};

/** @instance lcms_cmm_module
 *  @brief    lcms module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMM_s lcms_cmm_module = {

  oyOBJECT_CMM_INFO_S,                 /**< type, struct type */
  0,0,0,                               /**< ,dynamic object functions */
  CMM_NICK,                            /**< cmm, ICC signature */
  "0.6",                               /**< backend_version */
  lcmsInfoGetText,                     /**< getText */
  (char**)lcms_texts,                  /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,                     /**< oy_compatibility */

  (oyCMMapi_s*) & lcms_api4_cmm,       /**< api */

  &lcms_icon /**< icon */
};

