/** @file oyranos_cmm_lcms.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    littleCMS CMM backend for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */




#include <lcms.h>

#include "oyranos_cmm.h"         /* the API's this CMM implements */
#include "oyranos_cmms.h"        /* the API's this CMM uses from Oyranos */

void* oyAllocateFunc_           (size_t        size);
void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAlloc_f     allocate_func);
void  oyDeAllocateFunc_         (void *        data);

#include <math.h>



/* --- internal definitions --- */

#define CMM_NICK "lcms"
#define CMMProfileOpen_M    cmsOpenProfileFromMem
#define CMMProfileRelease_M cmsCloseProfile
#define CMMToString_M(text) #text
#define CMMMaxChannels_M 16
#define lcmsPROFILE "lcPR"
#define lcmsTRANSFORM "lcCC"

#define lcmsVERSION {0,1,0}

int lcmsCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );
oyMessage_f message = lcmsCMMWarnFunc;

int lcmsErrorHandlerFunction(int ErrorCode, const char *ErrorText);
int            lcmsCMMMessageFuncSet ( oyMessage_f         message_func );
int                lcmsCMMInit       ( );
int                lcmsCMMCanHandle  ( oyCMMQUERY_e      type,
                                       uint32_t          value );


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
                                       icColorSpaceSignature colour_in,
                                       icColorSpaceSignature colour_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyCMMptr_s        * oy );
lcmsTransformWrap_s * lcmsCMMTransform_GetWrap_(
                                       oyCMMptr_s        * cmm_ptr );
int lcmsCMMDeleteTransformWrap       ( oyPointer         * wrap );

lcmsProfileWrap_s * lcmsCMMProfile_GetWrap_(
                                       oyCMMptr_s        * cmm_ptr );
int lcmsCMMProfileReleaseWrap        ( oyPointer         * p );

int                lcmsCMMCheckPointer(oyCMMptr_s        * cmm_ptr,
                                       const char        * resource );
int        oyPixelToCMMPixelLayout_  ( oyPixel_t           pixel_layout,
                                       icColorSpaceSignature colour_space );


char * lcmsFilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );
cmsHTRANSFORM  lcmsCMMColourConversion_Create_ (
                                       cmsHPROFILE       * lps,
                                       int                 profiles_n,
                                       cmsHPROFILE       * proof,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s       * opts,
                                       lcmsTransformWrap_s ** ltw,
                                       oyCMMptr_s        * oy );
int          lcmsCMMColourConversion_Create (
                                       oyCMMptr_s       ** cmm_profile,
                                       int                 profiles_n,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s *       opts,
                                       oyCMMptr_s        * oy );
int  lcmsCMMColourConversion_FromMem ( oyPointer           mem,
                                       size_t              size,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       icColorSpaceSignature colour_space_in,
                                       icColorSpaceSignature colour_space_out,
                                       int                 intent,
                                       oyCMMptr_s        * oy );
oyPointer  lcmsCMMColourConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
oyPointer  lcmsCMMColourConversion_ToMem (
                                       oyCMMptr_s        * oy,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int              lcmsCMMColourConversion_Run (
                                       oyCMMptr_s        * oy,
                                       oyPointer           in_data,
                                       oyPointer           out_data,
                                       size_t              count,
                                       oyCMMProgress_f     progress );
oyOptions_s* lcmsFilter_CmmIccValidateOptions
                                     ( oyFilter_s        * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
cmsHPROFILE  lcmsAddProfile          ( oyProfile_s       * p );
oyPointer lcmsFilterNode_CmmIccContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int  lcmsCMMdata_Convert             ( oyCMMptr_s        * data_in,
                                       oyCMMptr_s        * data_out,
                                       oyFilterNode_s    * node );
int      lcmsFilterPlug_CmmIccRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
const char * lcmsInfoGetText         ( const char        * select,
                                       oyNAME_e            type );




/* --- implementations --- */

/** Function lcmsCMMInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/11
 *  @since   2007/12/11 (Oyranos: 0.1.8)
 */
int                lcmsCMMInit       ( )
{
  int error = 0;
  cmsErrorAction( LCMS_ERROR_SHOW );
  cmsSetErrorHandler( lcmsErrorHandlerFunction );
  return error;
}

/** Function lcmsCMMCanHandle
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/11
 *  @since   2007/12/11 (Oyranos: 0.1.8)
 */
int                lcmsCMMCanHandle  ( oyCMMQUERY_e      type,
                                       uint32_t          value )
{
  int ret = -1;

  switch(type)
  {
    case oyQUERY_OYRANOS_COMPATIBILITY:
         ret = OYRANOS_VERSION; break;
    case oyQUERY_PIXELLAYOUT_DATATYPE:
         switch(value) {
         case oyUINT8:
         case oyUINT16: ret = 1 ; break;
         case oyUINT32:
         case oyHALF:
         case oyFLOAT:
         case oyDOUBLE: ret = 0; break;
         }
         break;
    case oyQUERY_PIXELLAYOUT_CHANNELCOUNT:
         ret = CMMMaxChannels_M; break;
    case oyQUERY_PIXELLAYOUT_SWAP_COLOURCHANNELS:
         ret = 1; break;
    case oyQUERY_PIXELLAYOUT_COLOURCHANNEL_OFFSET:
         ret = 1; break;
    case oyQUERY_PIXELLAYOUT_PLANAR:
         ret = 1; break;
    case oyQUERY_PIXELLAYOUT_FLAVOUR:
         ret = 1; break;
    case oyQUERY_HDR:
         ret = 0; break;
    case oyQUERY_PROFILE_FORMAT:
         if(value == 1)
           ret = 1;
         else
           ret = 0; break;
    case oyQUERY_PROFILE_TAG_TYPE_READ:
    case oyQUERY_PROFILE_TAG_TYPE_WRITE:
    case oyQUERY_MAX:
         ret = 0; break;
  }

  return ret;
}



/** Function lcmsCMMProfile_GetWrap_
 *  @brief   convert to lcms profile wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/10
 *  @since   2007/12/10 (Oyranos: 0.1.8)
 */
lcmsProfileWrap_s * lcmsCMMProfile_GetWrap_( oyCMMptr_s * cmm_ptr )
{
  lcmsProfileWrap_s * s = 0;

  char type_[4] = lcmsPROFILE;
  int type = *((int*)&type_);

  if(cmm_ptr && !lcmsCMMCheckPointer( cmm_ptr, lcmsPROFILE ) &&
     cmm_ptr->ptr)
    s = (lcmsProfileWrap_s*) cmm_ptr->ptr;

  if(s && s->type != type)
    s = 0;

  return s;
}

/** Function lcmsCMMTransform_GetWrap_
 *  @brief   convert to lcms transform wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/20
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
lcmsTransformWrap_s * lcmsCMMTransform_GetWrap_( oyCMMptr_s * cmm_ptr )
{
  lcmsTransformWrap_s * s = 0;

  char type_[4] = lcmsTRANSFORM;
  int type = *((int*)&type_);

  if(cmm_ptr && !lcmsCMMCheckPointer( cmm_ptr, lcmsTRANSFORM ) &&
     cmm_ptr->ptr)
    s = (lcmsTransformWrap_s*) cmm_ptr->ptr;

  if(s && (s->type != type || !s->lcms))
    s = 0;

  return s;
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
  
  char type_[4] = lcmsPROFILE;
  int type = *((int*)&type_);
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
                                       oyCMMptr_s        * oy )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  if(oy->type != oyOBJECT_CMM_POINTER_S)
    error = 1;

  if(!error)
  {
    char type_[4] = lcmsPROFILE;
    int type = *((int*)&type_);
    size_t size = 0;
    oyPointer block = 0;
    lcmsProfileWrap_s * s = calloc(sizeof(lcmsProfileWrap_s), 1);

    if(data->type_ == oyOBJECT_PROFILE_S)
    {
      oyProfile_s * p = (oyProfile_s*)data;
      size = p->size_;
      block = p->block_;
    }

    s->type = type;
    s->size = size;
    s->block = block;

    s->lcms = CMMProfileOpen_M( block, size );
    oy->ptr = s;
    snprintf( oy->func_name, 32, "%s", CMMToString_M(CMMProfileOpen_M) );
    snprintf( oy->resource, 5, lcmsPROFILE ); 
    error = !oy->ptr;
  }

  if(!error)
  {
    oy->ptrRelease = lcmsCMMProfileReleaseWrap;
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
int                lcmsCMMCheckPointer(oyCMMptr_s        * cmm_ptr,
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
  int cchans = _cmsChannelsOf( colour_space );
  int lcms_colour_space = _cmsLCMScolorSpace( colour_space );
  int extra = chan_n - cchans;

  if(chan_n > CMMMaxChannels_M)
    message( oyMSG_WARN,0, "%s:%d "
             "can not handle more than %d channels; found: %d",
             __FILE__,__LINE__, CMMMaxChannels_M, chan_n);

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
  if(oyToSwapColourChannels_m (pixel_layout))
    cmm_pixel |= DOSWAP_SH(1);
  if(oyToByteswap_m(pixel_layout))
    cmm_pixel |= ENDIAN16_SH(1);
  if(planar)
    cmm_pixel |= PLANAR_SH(1);
  if(flavour)
    cmm_pixel |= FLAVOR_SH(1);

  cmm_pixel |= COLORSPACE_SH( lcms_colour_space );


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

    cmsDeleteTransform (s->lcms);
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
                                       icColorSpaceSignature colour_in,
                                       icColorSpaceSignature colour_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyCMMptr_s        * oy )
{
  int error = !xform;
  lcmsTransformWrap_s * s = 0;
  
  if(!error)
  {
    char type_[4] = lcmsTRANSFORM;
    int type = *((int*)&type_);
    lcmsTransformWrap_s * ltw = calloc(sizeof(lcmsTransformWrap_s), 1);

    ltw->type = type;

    ltw->lcms = xform; xform = 0;

    ltw->sig_in  = colour_in;
    ltw->sig_out = colour_out;
    ltw->oy_pixel_layout_in  = oy_pixel_layout_in;
    ltw->oy_pixel_layout_out = oy_pixel_layout_out;
    s = ltw;
  }

  if(!error)
  {
    oy->ptr = s;

    oy->ptrRelease = lcmsCMMDeleteTransformWrap;

    strcpy( oy->func_name, "lcmsCMMDeleteTransformWrap" );
  }

  return s;
}

/** Function lcmsCMMColourConversion_Create_
 *  @brief   create a CMM transform
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
cmsHTRANSFORM  lcmsCMMColourConversion_Create_ (
                                       cmsHPROFILE       * lps,
                                       int                 profiles_n,
                                       cmsHPROFILE       * proof,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s       * opts,
                                       lcmsTransformWrap_s ** ltw,
                                       oyCMMptr_s        * oy )
{
  oyPixel_t lcms_pixel_layout_in = 0;
  oyPixel_t lcms_pixel_layout_out = 0;
  int error = !lps;
  cmsHTRANSFORM xform;
  icColorSpaceSignature colour_in = 0;
  icColorSpaceSignature colour_out = 0;
  int intent = 0,
      bpc = 0,
      gamut_warning = 0,
      flags = 0;
  const char * o_txt = 0;

  if(!lps || !profiles_n || !oy_pixel_layout_in || !oy_pixel_layout_out)
    return 0;

  
  if(!error)
  {
    colour_in = cmsGetColorSpace( lps[0] );
    if(profiles_n > 1)
      colour_out = cmsGetColorSpace( lps[profiles_n-1] );
    else
      colour_out = cmsGetPCS( lps[profiles_n-1] );
  }

  lcms_pixel_layout_in  = oyPixelToCMMPixelLayout_(oy_pixel_layout_in,
                                                   colour_in);
  lcms_pixel_layout_out = oyPixelToCMMPixelLayout_(oy_pixel_layout_out,
                                                   colour_out);

#ifndef oyStrlen_
#define oyStrlen_ strlen
#endif
      o_txt = oyOptions_FindString  ( opts, "rendering_intent", 0);
      if(o_txt && oyStrlen_(o_txt))
        intent = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_bpc", 0 );
      if(o_txt && oyStrlen_(o_txt))
        bpc = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_gamut_warning", 0 );
      if(o_txt && oyStrlen_(o_txt))
        gamut_warning = atoi( o_txt );

      /* this should be moved to the CMM and not be handled here in Oyranos */
      flags = bpc ?           flags | cmsFLAGS_WHITEBLACKCOMPENSATION :
                              flags & (~cmsFLAGS_WHITEBLACKCOMPENSATION);
      flags = gamut_warning ? flags | cmsFLAGS_GAMUTCHECK :
                              flags & (~cmsFLAGS_GAMUTCHECK);


  if(!error)
  {
         if(profiles_n == 1)
        xform = cmsCreateTransform( lps[0], lcms_pixel_layout_in,
                                    0, lcms_pixel_layout_out,
                                    intent, 0 );
    else if(profiles_n == 2)
        xform = cmsCreateTransform( lps[0], lcms_pixel_layout_in,
                                    lps[1], lcms_pixel_layout_out,
                                    intent, 0 );
    /*else if(profiles_n == 3)
      oy->ptr = */
  }

  if(!error && ltw && oy)
    *ltw= lcmsTransformWrap_Set_( xform, colour_in, colour_out,
                                  oy_pixel_layout_in, oy_pixel_layout_out, oy );
  return xform;
}

/** Function lcmsCMMColourConversion_Create
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/00 (Oyranos: 0.1.8)
 *  @date    2007/12/00
 */
int          lcmsCMMColourConversion_Create (
                                       oyCMMptr_s       ** cmm_profile,
                                       int                 profiles_n,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s *       opts,
                                       oyCMMptr_s        * oy )
{
  cmsHPROFILE * lps = oyAllocateFunc_(sizeof(cmsHPROFILE)*profiles_n+1);
  cmsHTRANSFORM xform = 0;
  int i;
  int error = !(cmm_profile && lps);
  lcmsTransformWrap_s * ltw  = 0;

  if(!cmm_profile || !profiles_n || !oy_pixel_layout_in || !oy_pixel_layout_out)
    return 1;

  
  for(i = 0; i < profiles_n; ++i)
  {
    lcmsProfileWrap_s * s = 0;
    cmsHPROFILE profile = 0;

    if(!error)
      s = lcmsCMMProfile_GetWrap_( cmm_profile[i] );

    if(!error)
      error = !s;

    if(!error)
      profile = s->lcms;

    if(!error)
      lps[i] = profile;

    if(!error)
      error = !lps[i];
  }

  xform = lcmsCMMColourConversion_Create_( lps, profiles_n, 0,
                                           oy_pixel_layout_in,
                                           oy_pixel_layout_out, opts, &ltw, oy);
  error = !xform;


  free(lps);

  if(!error)
    error = !ltw;

  return error;
}

/** Function lcmsCMMColourConversion_FromMem
 *  @brief   oyCMMColourConversion_FromMem_t implementation
 *
 *  Convert a lcms device link to a colour conversion context.
 *  Seems redundant here, as this case is covered by
 *  lcmsCMMColourConversion_Create. => redirect
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2008/11/06
 */
int  lcmsCMMColourConversion_FromMem ( oyPointer           mem,
                                       size_t              size,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       icColorSpaceSignature colour_space_in,
                                       icColorSpaceSignature colour_space_out,
                                       int                 intent,
                                       oyCMMptr_s        * oy )
{
  int error = 0;
  oyCMMptr_s intern = {oyOBJECT_CMM_POINTER_S, 0,0,0,
                       CMM_NICK, {0}, 0, -1, {0}, 0, 0 };
  oyCMMptr_s * dls[] = {0, 0};
  oyProfile_s p = { oyOBJECT_PROFILE_S,0,0,0,0,0,0,0,0,0,0,0 };

  dls[0] = &intern;
  p.block_ = mem;
  p.size_ = size;

  error = lcmsCMMData_Open ( (oyStruct_s*)&p, &intern );
  error = lcmsCMMColourConversion_Create (
                                       dls, 1,
                                       oy_pixel_layout_in, oy_pixel_layout_out,
                                       0, oy );
  return error;
}

/** Function lcmsCMMColourConversion_ToMem_
 *
 *  convert a lcms colour conversion context to a device link
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyPointer  lcmsCMMColourConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !xform;
  oyPointer data = 0;

  if(!error)
  {
    cmsHPROFILE dl = cmsTransform2DeviceLink( xform, 0 );

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

        cmsAddTag(dl, icSigProfileSequenceDescTag, pseq);
        free(pseq);
    }

    _cmsSaveProfileToMem( dl, 0, size );
    data = allocateFunc( *size );
    _cmsSaveProfileToMem( dl, data, size );
  }

  return data;
}

/** Function lcmsCMMColourConversion_ToMem
 *  @brief   oyCMMColourConversion_ToMem_t implementation
 *
 *  convert a lcms colour conversion context to a device link
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/12/21
 */
oyPointer  lcmsCMMColourConversion_ToMem (
                                       oyCMMptr_s        * oy,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !oy;
  lcmsTransformWrap_s * s = lcmsCMMTransform_GetWrap_( oy );
  oyPointer data = 0;

  error = !s;

  if(!error)
  {
    data = lcmsCMMColourConversion_ToMem_( s->lcms, size, allocateFunc );
  }

  return data;
}


/** Function lcmsCMMColourConversion_Run
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/12/21
 */
int              lcmsCMMColourConversion_Run (
                                       oyCMMptr_s        * oy,
                                       oyPointer           in_data,
                                       oyPointer           out_data,
                                       size_t              count,
                                       oyCMMProgress_f     progress )
{
  int error = !oy;

  lcmsTransformWrap_s * s = lcmsCMMTransform_GetWrap_( oy );
  error = !s;

  if(!error && in_data && out_data)
  {
    oyDATATYPE_e dt = oyToDataType_m(s->oy_pixel_layout_in);
    if(dt == oyDOUBLE)
    {
      /*size_t i = 0;
      double * d = in_data;
      int chan = oyToChannels_m(s->oy_pixel_layout_in);
      size_t n = chan * count;
      for(i = 0; i < n; ++i)
        d[i] = d[i] * 100.;*/
    }

    if(count > 0)
      cmsDoTransform( s->lcms, in_data, out_data, count);

    dt = oyToDataType_m(s->oy_pixel_layout_out);
    if(dt == oyDOUBLE)
    {
      /*int i = 0;
      double * d = out_data;
      int chan = oyToChannels_m(s->oy_pixel_layout_out);
      int n = chan * count;
      for(i = 0; i < n; ++i)
        d[i] = d[i] / 100.;*/
    }
  }

  return error;
}


oyOptions_s* lcmsFilter_CmmIccValidateOptions
                                     ( oyFilter_s        * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = oyFilterRegistrationMatch(filter->registration_, "//colour/icc",
                                      oyOBJECT_CMM_API4_S);

  *result = error;

  return 0;
}

oyWIDGET_EVENT_e   lcmsWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}


oyDATATYPE_e lcms_cmmIcc_data_types[7] = {oyUINT8, oyUINT16, oyDOUBLE, 0};

oyConnector_s lcms_cmmIccSocket_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Socket"},
  oyCONNECTOR_MANIPULATOR, /* connector_type */
  0, /* is_plug == oyFilterPlug_s */
  lcms_cmmIcc_data_types, /* data_types */
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
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnector_s* lcms_cmmIccSocket_connectors[2]={&lcms_cmmIccSocket_connector,0};

oyConnector_s lcms_cmmIccPlug_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Socket"},
  oyCONNECTOR_MANIPULATOR, /* connector_type */
  1, /* is_plug == oyFilterPlug_s */
  lcms_cmmIcc_data_types, /* data_types */
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
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnector_s* lcms_cmmIccPlug_connectors[2]={&lcms_cmmIccPlug_connector,0};

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
  oyCMMptr_s * cmm_ptr = 0;
  lcmsProfileWrap_s * s = 0;

  if(!p || p->type_ != oyOBJECT_PROFILE_S)
  {
    message( oyMSG_WARN,0, "%s:%d "
             "no profile provided", __FILE__,__LINE__ );
    return 0;
  }

  cmm_ptr = oyCMMptr_LookUp( (oyStruct_s*)p, lcmsPROFILE );

  cmm_ptr->lib_name = CMM_NICK;

  if(!cmm_ptr->ptr)
    error = lcmsCMMData_Open( (oyStruct_s*)p, cmm_ptr );

  if(!error)
  {
    s = lcmsCMMProfile_GetWrap_( cmm_ptr );
    error = !s;
  }

  if(!error)
    hp = s->lcms;

  oyCMMptr_Release( &cmm_ptr );

  if(!error)
    return hp;
  else
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
  int channels = 0;
  int n,i,len;
  oyDATATYPE_e data_type = 0;
  size_t size_ = 0;
  oyFilterSocket_s * socket = (oyFilterSocket_s *)node->sockets[0];
  oyFilterPlug_s * plug = (oyFilterPlug_s *)node->plugs[0];
  oyFilter_s * filter = 0;
  oyFilterNode_s * input_node = 0;
  oyImage_s * image_input = 0,
            * image_output = 0;
  cmsHPROFILE * lps = 0, simulation = 0;
  cmsHTRANSFORM xform = 0;
  oyOption_s * o = 0;
  oyProfile_s * p = 0,
              * prof = 0;
  oyProfiles_s * profiles = 0,
               * profs = 0;
  oyProfileTag_s * psid = 0,
                 * info = 0,
                 * cprt = 0;
  int profiles_n = 0;

  filter = node->filter;
  input_node = plug->remote_socket_->node;
  image_input = (oyImage_s*)plug->remote_socket_->data;
  image_output = (oyImage_s*)socket->data;

  if(image_input->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message( oyMSG_WARN, (oyStruct_s*)node,
             "%s: %d missed input image %d", __FILE__,__LINE__, image_input->type_ );
  }
  if(image_output->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message( oyMSG_WARN, (oyStruct_s*)node,
             "%s: %d missed output image %d", __FILE__,__LINE__, image_input->type_ );
  }

  data_type = oyToDataType_m( image_input->layout_[0] );

  if(data_type == oyFLOAT)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message( oyMSG_WARN, (oyStruct_s*)node,
             "%s: %d can not handle oyFLOAT", __FILE__,__LINE__ );
  }

  channels = oyToChannels_m( image_input->layout_[0] );

  len = sizeof(cmsHPROFILE) * (15 + 2 + 1);
  lps = oyAllocateFunc_( len );
  memset( lps, 0, len );

  /* input profile */
  lps[ profiles_n++ ] = lcmsAddProfile( image_input->profile_ );
  if(!image_input->profile_)
  {
    message( oyMSG_WARN, (oyStruct_s*)node, "%s: %d "
             "missed image_input->profile_", __FILE__,__LINE__ );
    return 0;
  }
  p = oyProfile_Copy( image_input->profile_, 0 );
  profs = oyProfiles_MoveIn( profs, &p, -1 );

  /* effect profiles */
  o = oyOptions_Find( node->filter->options_, "profiles_effect" );
  if(o)
  {
    if( o->value_type != oyVAL_STRUCT ||
        o->value->oy_struct->type_ != oyOBJECT_PROFILES_S)
    {
      oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION );
      message( oyMSG_WARN, (oyStruct_s*)node,
               "%s: %d incompatible \"profiles_effect\"", __FILE__,__LINE__ );
      
    } else
    {
      profiles = (oyProfiles_s*) o->value;
      n = oyProfiles_Count( profiles );
      for(i = 0; i < n; ++i)
      {
        p = oyProfiles_Get( profiles, i );

        /* Look in the Oyranos cache for a CMM internal representation */
        lps[ profiles_n ] = lcmsAddProfile( p );
        profs = oyProfiles_MoveIn( profs, &p, -1 );
      }
    }
    oyOption_Release( &o );
  }

  /* simulation profile */
  o = oyOptions_Find( node->filter->options_, "profiles_simulation" );
  if(o)
  {
    if( o->value_type != oyVAL_STRUCT ||
        o->value->oy_struct->type_ != oyOBJECT_PROFILES_S)
    {
      oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION );
      message( oyMSG_WARN, (oyStruct_s*)node,
               "%s: %d incompatible \"profiles_simulation\"",__FILE__,__LINE__);
      
    } else
    {
      profiles = (oyProfiles_s*) o->value;
      n = oyProfiles_Count( profiles );
      for(i = 0; i < n; ++i)
      {
        p = oyProfiles_Get( profiles, i );

        /* Look in the Oyranos cache for a CMM internal representation */
        if(i == 0)
        {
          simulation = lcmsAddProfile( p );
          profs = oyProfiles_MoveIn( profs, &p, -1 );
        } else if(i == 1)
          message( oyMSG_WARN, (oyStruct_s*)node,
           "%s: %d Currently only one in \"profiles_simulation\" supported: %d",
                   __FILE__,__LINE__, n );
          
        oyProfile_Release( &p );
      }
    }
    oyOption_Release( &o );
  }

  /* output profile */
  if(!image_output->profile_)
  {
    message( oyMSG_WARN, (oyStruct_s*)node, "%s: %d "
             "missed image_output->profile_", __FILE__,__LINE__ );
    return 0;
  }
  lps[ profiles_n++ ] = lcmsAddProfile( image_output->profile_ );
  p = oyProfile_Copy( image_output->profile_, 0 );
  profs = oyProfiles_MoveIn( profs, &p, -1 );

  *size = 0;

  /* create the context */
  xform = lcmsCMMColourConversion_Create_( lps, profiles_n, simulation,
                                           image_input->layout_[0],
                                           image_output->layout_[0],
                                           node->filter->options_, 0, 0);
  error = !xform;

  if(!error)
  {
    if(oy_debug)
      block = lcmsCMMColourConversion_ToMem_( xform, size, oyAllocateFunc_ );
    else
      block = lcmsCMMColourConversion_ToMem_( xform, size, allocateFunc );
    error = !block && !*size;
    cmsDeleteTransform( xform );
  }


  /* additional tags for debugging */
  if(!error && oy_debug)
  {
    if(!error && size)
    {
      size_ = *size;

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
        char * cc_name = lcmsFilterNode_GetText( node, oyNAME_NICK,
                                                 oyAllocateFunc_ );
        oyName_s * name = oyName_new(0);
        const char * lib_name = node->filter->api4_->id_;

        name = oyName_set_ ( name, cc_name, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        name = oyName_set_ ( name, lib_name, oyNAME_NICK,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        oyDeAllocateFunc_( cc_name );
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


/** Function lcmsFilterNode_GetText
 *  @brief   implement oyCMMFilterNode_GetText_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
char * lcmsFilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc )
{
  return oyStringCopy_( oyFilterNode_GetText( node, type ), allocateFunc );
}

/** Function lcmsCMMdata_Convert
 *  @brief   convert between data formats
 *  @ingroup cmm_handling
 *
 *  The function might be used to provide a backend specific context.
 *  Implements oyCMMdata_Convert_f
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int  lcmsCMMdata_Convert             ( oyCMMptr_s        * data_in,
                                       oyCMMptr_s        * data_out,
                                       oyFilterNode_s    * node )
{
  int error = !data_in || !data_out;
  oyCMMptr_s * cmm_ptr_in = data_in,
             * cmm_ptr_out = data_out;
  lcmsTransformWrap_s * ltw  = 0;
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
       (strcmp( cmm_ptr_out->resource, lcmsTRANSFORM ) != 0) ) )
    error = 1;

  if(!error)
  {
    lps[0] = CMMProfileOpen_M( cmm_ptr_in->ptr, cmm_ptr_in->size );
    xform = lcmsCMMColourConversion_Create_( lps, 1, 0,
                                           image_input->layout_[0],
                                           image_output->layout_[0],
                                           node->filter->options_,
                                           &ltw, cmm_ptr_out );
    CMMProfileRelease_M (lps[0] );
  }

  return error;
}

/** Function lcmsFilterPlug_CmmIccRun
 *  @brief   implement oyCMMFilterPlug_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/10/03
 */
int      lcmsFilterPlug_CmmIccRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int i,j,k, n;
  int error = 0;
  double * in_values = 0, * out_values = 0;
  int channels = 0;
  oyDATATYPE_e data_type = 0;

  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node;
  oyImage_s * image_input = 0;
  oyArray2d_s * array = 0;
  lcmsTransformWrap_s * ltw  = 0;

  plug = (oyFilterPlug_s *)socket->node->plugs[0];
  input_node = plug->remote_socket_->node;
  image_input = (oyImage_s*)plug->remote_socket_->data;

  /* We let the input filter do its processing first. */
  error = input_node->api7_->oyCMMFilterPlug_Run( plug, ticket );
  if(error != 0) return error;

  array = ticket->array;

  data_type = oyToDataType_m( image_input->layout_[0] );

  if(data_type == oyFLOAT)
  {
    oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message(oyMSG_WARN,0, "%s: %d can not handle oyFLOAT", __FILE__,__LINE__);
    error = 1;
  }

  channels = oyToChannels_m( image_input->layout_[0] );

  ltw = lcmsCMMTransform_GetWrap_( node->backend_data );

  /* now do some position blind manipulations */
  if(ltw)
  {
    for( k = 0; k < array->height; ++k)
    {
      n = array->width / channels;

      if(data_type == oyUINT8 ||
         data_type == oyUINT16 ||
         data_type == oyDOUBLE)
      {
        cmsDoTransform( ltw->lcms, array->array2d[k], array->array2d[k], n );

      } else
      {
        oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
        error = 1;
      }

    }

  } else
  {
    oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT );
    error = 1;
  }

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

/** Function lcmsCMMWarnFunc
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int lcmsCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;
  const char * type_name = "";
  int id = -1;

  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
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

  fprintf( stderr, "%s[%d] ", type_name, id );

  fprintf( stderr, text ); fprintf( stderr, "\n" );
  free( text );

  return 0;
}

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
  message( code, 0, ErrorText, 0 );
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
  message = message_func;
  return 0;
}

char lcms_extra_options[] = {
 "\n\
  <" OY_TOP_INTERNAL ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "icc." CMM_NICK ">\n\
      <cmyk_cmky_black_preservation.advanced>0</cmyk_cmky_black_preservation.advanced>\n\
     </" "icc." CMM_NICK ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_INTERNAL ">\n"
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
oyCMMapi6_s   lcms_api6_cmm = {

  oyOBJECT_CMM_API6_S,
  0,0,0,
  0,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,
  lcmsCMMCanHandle,

  OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc." CMM_NICK ".CPU." oyCOLOUR_ICC_DEVICE_LINK "_" lcmsTRANSFORM,

  lcmsVERSION,
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  
  oyCOLOUR_ICC_DEVICE_LINK,  /* data_type_in, "oyDL" */
  lcmsTRANSFORM,             /* data_type_out, "lcCC" */
  lcmsCMMdata_Convert        /* oyCMMdata_Convert_f oyCMMdata_Convert */
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
oyCMMapi7_s   lcms_api7_cmm = {

  oyOBJECT_CMM_API7_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api6_cmm,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,
  lcmsCMMCanHandle,

  OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc." CMM_NICK ".CPU",

  lcmsVERSION,
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lcmsFilterPlug_CmmIccRun,  /* oyCMMFilterPlug_Run_f */
  lcmsTRANSFORM,             /* data_type, "lcCC" */

  lcms_cmmIccPlug_connectors,/* plugs */
  1,                         /* plugs_n */
  0,                         /* plugs_last_add */
  lcms_cmmIccSocket_connectors,   /* sockets */
  1,                         /* sockets_n */
  0,                         /* sockets_last_add */
};

/** @instance lcms_api4
 *  @brief    littleCMS oyCMMapi4_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyCMMapi4_s   lcms_api4_cmm = {

  oyOBJECT_CMM_API4_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api7_cmm,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,
  lcmsCMMCanHandle,

  OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc." CMM_NICK,

  lcmsVERSION,
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lcmsFilter_CmmIccValidateOptions,
  lcmsWidgetEvent,

  lcmsFilterNode_CmmIccContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  lcmsFilterNode_GetText, /* oyCMMFilterNode_GetText_f */
  oyCOLOUR_ICC_DEVICE_LINK, /* context data_type */

  {oyOBJECT_NAME_S, 0,0,0, "colour", "Colour", "ICC compatible CMM"},
  "Colour/CMM/littleCMS", /* category */
  lcms_extra_options,   /* options */
  0,   /* opts_ui_ */
};



/** @instance lcms_api1
 *  @brief    lcms oyCMMapi1_s implementations
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
oyCMMapi1_s  lcms_api1 = {

  oyOBJECT_CMM_API1_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api4_cmm,
  
  lcmsCMMInit,
  lcmsCMMMessageFuncSet,
  lcmsCMMCanHandle,

  lcmsCMMData_Open,
  lcmsCMMColourConversion_Create,
  lcmsCMMColourConversion_FromMem,
  lcmsCMMColourConversion_ToMem,
  lcmsCMMColourConversion_Run,
};


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * lcmsInfoGetText         ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Little CMS");
    else
      return _("LittleCMS is a CMM, a color management engine; it implements fast transforms between ICC profiles. \"Little\" stands for its small overhead. With a typical footprint of about 100K including C runtime, you can color-enable your application without the pain of ActiveX, OCX, redistributables or binaries of any kind. We are using little cms in several commercial projects, however, we are offering lcms library free for anybody under an extremely liberal open source license.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Marti");
    else if(type == oyNAME_NAME)
      return _("Marti Maria");
    else
      return _("littleCMS project; www: http://www.littlecms.com; support/email: support@littlecms.com; sources: http://www.littlecms.com/downloads.htm");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("MIT");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 1998-2008 Marti Maria Saguer; MIT");
    else
      return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
  }
  return 0;
}

/** @instance lcms_cmm_module
 *  @brief    lcms module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMMInfo_s lcms_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.6",
  lcmsInfoGetText,
  OYRANOS_VERSION,

  (oyCMMapi_s*) & lcms_api1,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "lcms_logo2.png"},
};

