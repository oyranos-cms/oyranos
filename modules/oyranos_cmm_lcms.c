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
#include "oyranos_helper.h"      /* oySprintf_ and other local helpers */
#include "oyranos_alpha_internal.h" /* hashTextAdd_m ... */

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
int      lcmsCMMTransform_GetWrap_   ( oyCMMptr_s        * cmm_ptr,
                                       lcmsTransformWrap_s ** s );
int lcmsCMMDeleteTransformWrap       ( oyPointer         * wrap );

lcmsProfileWrap_s * lcmsCMMProfile_GetWrap_(
                                       oyCMMptr_s        * cmm_ptr );
int lcmsCMMProfileReleaseWrap        ( oyPointer         * p );

int                lcmsCMMCheckPointer(oyCMMptr_s        * cmm_ptr,
                                       const char        * resource );
int        oyPixelToCMMPixelLayout_  ( oyPixel_t           pixel_layout,
                                       icColorSpaceSignature colour_space );
char * lcmsImage_GetText             ( oyImage_s         * image,
                                       int                 verbose,
                                       oyAlloc_f           allocateFunc );


char * lcmsFilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );
oyPointer  lcmsCMMColourConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
oyOptions_s* lcmsFilter_CmmIccValidateOptions
                                     ( oyFilterCore_s    * filter,
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
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 *  @date    2009/05/28
 */
int      lcmsCMMTransform_GetWrap_   ( oyCMMptr_s        * cmm_ptr,
                                       lcmsTransformWrap_s ** s )
{
  char type_[4] = lcmsTRANSFORM;
  int type = *((int*)&type_);

  if(cmm_ptr && !lcmsCMMCheckPointer( cmm_ptr, lcmsTRANSFORM ) &&
     cmm_ptr->ptr)
    *s = (lcmsTransformWrap_s*) cmm_ptr->ptr;

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

/** Function lcmsCMMConversionContextCreate_
 *  @brief   create a CMM transform
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2009/06/25
 */
cmsHTRANSFORM  lcmsCMMConversionContextCreate_ (
                                       cmsHPROFILE       * lps,
                                       int                 profiles_n,
                                       cmsHPROFILE       * simulation,
                                       int                 proof_n,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s       * opts,
                                       lcmsTransformWrap_s ** ltw,
                                       oyCMMptr_s        * oy )
{
  oyPixel_t lcms_pixel_layout_in = 0;
  oyPixel_t lcms_pixel_layout_out = 0;
  int error = !lps;
  cmsHTRANSFORM xform = 0;
  icColorSpaceSignature colour_in = 0;
  icColorSpaceSignature colour_out = 0;
  icProfileClassSignature profile_class_out = 0;
  int proof = 0,
      intent = 0,
      intent_proof = 0,
      bpc = 0,
      cmyk_cmyk_black_preservation = 0,
      gamut_warning = 0,
      high_precission = 0,
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
    profile_class_out = cmsGetDeviceClass( lps[profiles_n-1] );
  }

  lcms_pixel_layout_in  = oyPixelToCMMPixelLayout_(oy_pixel_layout_in,
                                                   colour_in);
  lcms_pixel_layout_out = oyPixelToCMMPixelLayout_(oy_pixel_layout_out,
                                                   colour_out);

#ifndef oyStrlen_
#define oyStrlen_ strlen
#endif
      o_txt = oyOptions_FindString  ( opts, "proof_soft", 0 );
      if(o_txt && oyStrlen_(o_txt)/* && profile_class_out== icSigDisplayClass*/)
        proof = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "proof_hard", 0 );
      if(o_txt && oyStrlen_(o_txt)/* && profile_class_out== icSigOutputClass*/)
        proof = atoi( o_txt ) ? atoi(o_txt) : proof;

      o_txt = oyOptions_FindString  ( opts, "rendering_intent", 0);
      if(o_txt && oyStrlen_(o_txt))
        intent = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_intent_proof", 0);
      if(o_txt && oyStrlen_(o_txt))
        intent_proof = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_bpc", 0 );
      if(o_txt && oyStrlen_(o_txt))
        bpc = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_gamut_warning", 0 );
      if(o_txt && oyStrlen_(o_txt))
        gamut_warning = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_high_precission", 0 );
      if(o_txt && oyStrlen_(o_txt))
        high_precission = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "cmyk_cmyk_black_preservation", 0 );
      if(o_txt && oyStrlen_(o_txt))
        cmyk_cmyk_black_preservation = atoi( o_txt );

      /* this should be moved to the CMM and not be handled here in Oyranos */
      flags = proof ?         flags | cmsFLAGS_SOFTPROOFING :
                              flags & (~cmsFLAGS_SOFTPROOFING);
      flags = bpc ?           flags | cmsFLAGS_WHITEBLACKCOMPENSATION :
                              flags & (~cmsFLAGS_WHITEBLACKCOMPENSATION);
      flags = gamut_warning ? flags | cmsFLAGS_GAMUTCHECK :
                              flags & (~cmsFLAGS_GAMUTCHECK);
      flags = high_precission ? flags | cmsFLAGS_NOTPRECALC :
                              flags & (~cmsFLAGS_NOTPRECALC);
      flags = cmyk_cmyk_black_preservation ? flags | cmsFLAGS_PRESERVEBLACK :
                              flags & (~cmsFLAGS_PRESERVEBLACK);
      if(cmyk_cmyk_black_preservation == 2)
        cmsSetCMYKPreservationStrategy( LCMS_PRESERVE_K_PLANE );

  if(!error)
  {
         if(profiles_n == 1)
        xform = cmsCreateTransform( lps[0], lcms_pixel_layout_in,
                                    0, lcms_pixel_layout_out,
                                    intent, flags );
    else if(profiles_n == 2 && !proof)
        xform = cmsCreateTransform( lps[0], lcms_pixel_layout_in,
                                    lps[1], lcms_pixel_layout_out,
                                    intent, flags );
    else if(profiles_n >= 3)
    {
      int multi_profiles_n = profiles_n;

      if(flags & cmsFLAGS_SOFTPROOFING && proof_n)
        --multi_profiles_n;

      xform =   cmsCreateMultiprofileTransform(
                                    lps, 
                                    multi_profiles_n,
                                    lcms_pixel_layout_in,
                                  flags & cmsFLAGS_SOFTPROOFING && proof_n
                                  ? NOCOLORSPACECHECK(lcms_pixel_layout_out)
                                  : lcms_pixel_layout_out,
                                    intent, flags );
    }

    if(proof_n && flags & cmsFLAGS_SOFTPROOFING)
    {
      int i;

      for(i = 0; i < proof_n; ++i)
      {
        cmsHPROFILE dl = 0;
        if(xform)
        {
          dl = cmsTransform2DeviceLink( xform, cmsFLAGS_GUESSDEVICECLASS );
          cmsDeleteTransform( xform );

        } else if(i == 0 && profiles_n == 2)
          dl = lps[0];

        /* add simulation profile and go to next simulation in chain */
        if(i < proof_n - 1)
          xform = cmsCreateProofingTransform(
                                    dl, lcms_pixel_layout_in,
                                    simulation[ i + 1 ],
                                    lcms_pixel_layout_out,
                                    simulation[ i ],
                                    intent,
                                    intent_proof,
                                    flags);
        else
        /* add last simulation and go to output profile */
          xform = cmsCreateProofingTransform(
                                    dl, lcms_pixel_layout_in,
                                    lps[ profiles_n - 1 ],
                                    lcms_pixel_layout_out,
                                    simulation[ i ], /* should be the last */
                                    intent,
            /* TODO The INTENT_ABSOLUTE_COLORIMETRIC should lead to 
               paper simulation, but does take white point into account.
               Do we want this?
             */
                                    intent_proof,
                                    flags);

        /* release intermediate transform */
        if(i == 0 && profiles_n != 2)
          CMMProfileRelease_M( dl );
      }
    }
  }

  /* reset */
  cmsSetCMYKPreservationStrategy( LCMS_PRESERVE_PURE_K );

  if(!error && ltw && oy)
    *ltw= lcmsTransformWrap_Set_( xform, colour_in, colour_out,
                                  oy_pixel_layout_in, oy_pixel_layout_out, oy );
  return xform;
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

oyOptions_s* lcmsFilter_CmmIccValidateOptions
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

oyWIDGET_EVENT_e   lcmsWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}


oyDATATYPE_e lcms_cmmIcc_data_types[7] = {oyUINT8, oyUINT16, oyDOUBLE, 0};

oyConnectorImaging_s lcms_cmmIccSocket_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Socket"},
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
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
  1, /* id; relative to oyFilterCore_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s* lcms_cmmIccSocket_connectors[2]={&lcms_cmmIccSocket_connector,0};

oyConnectorImaging_s lcms_cmmIccPlug_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Plug"},
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
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
  1, /* id; relative to oyFilterCore_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s* lcms_cmmIccPlug_connectors[2]={&lcms_cmmIccPlug_connector,0};

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
  oyFilterCore_s * filter = 0;
  oyFilterNode_s * input_node = 0;
  oyImage_s * image_input = 0,
            * image_output = 0;
  cmsHPROFILE * lps = 0,
              * simulation = 0;
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
      profiles_proof_n = 0;
  int verbose = oyOptions_FindString( node->tags, "verbose", "true" ) ? 1 : 0;

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
  o = oyOptions_Find( node->core->options_, "profiles_effect" );
  if(o)
  {
    if( o->value_type != oyVAL_STRUCT ||
        !o->value->oy_struct ||
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
        lps[ profiles_n++ ] = lcmsAddProfile( p );
        profs = oyProfiles_MoveIn( profs, &p, -1 );
      }
    }
    oyOption_Release( &o );
  }

  /* simulation profile */
  o = oyOptions_Find( node->core->options_, "profiles_simulation" );
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
      profiles = (oyProfiles_s*) o->value->oy_struct;
      n = oyProfiles_Count( profiles );
      len = sizeof(cmsHPROFILE) * (n + 2 + 1);
      simulation = oyAllocateFunc_( len );
      memset( simulation, 0, len );

      message( oyMSG_DBG,(oyStruct_s*)node,
               "%s:%d %d simulation profile(s) found \"%s\"",
               __FILE__,__LINE__, n,
               profiles?oyStruct_TypeToText((oyStruct_s*)profiles):"????");

      for(i = 0; i < n; ++i)
      {
        p = oyProfiles_Get( profiles, i );

        /* Look in the Oyranos cache for a CMM internal representation */
        /*if(i == 0)*/
        {
          simulation[ profiles_proof_n++ ] = lcmsAddProfile( p );
          if(oy_debug)
            message( oyMSG_DBG,(oyStruct_s*)node,
                     "%s:%d found profile: %s",
               __FILE__,__LINE__, p?oyProfile_GetText( p, oyNAME_NICK ):"????");

          profs = oyProfiles_MoveIn( profs, &p, -1 );

        } /*else if(i == 1)
          message( oyMSG_WARN, (oyStruct_s*)node,
           "%s: %d Currently only one in \"profiles_simulation\" supported: %d",
                   __FILE__,__LINE__, n );*/

        oyProfile_Release( &p );
      }
    }
    oyOption_Release( &o );
  } else if(verbose || oy_debug)
    message( oyMSG_DBG,(oyStruct_s*)node,
             "%s:%d no simulation profile found",
             __FILE__,__LINE__);


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
  xform = lcmsCMMConversionContextCreate_( lps, profiles_n,
                                           simulation, profiles_proof_n,
                                           image_input->layout_[0],
                                           image_output->layout_[0],
                                           node->core->options_, 0, 0);
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
        const char * lib_name = node->core->api4_->id_;

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

char * lcmsImage_GetText             ( oyImage_s         * image,
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
  const char * tmp = 0;
  char * hash_text = 0,
       * temp = 0;
  oyFilterNode_s * s = node;

  oyImage_s * in_image = 0,
                 * out_image = 0;
  int i,n,verbose;
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
    hashTextAdd_m( "  <data_in>\n" );
    if(in_image)
    {
      temp = lcmsImage_GetText( in_image, verbose, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyDeAllocateFunc_(temp); temp = 0;
    }
    hashTextAdd_m( "  </data_in>\n" );

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
    hashTextAdd_m( "  <data_out>\n" );
    if(out_image)
    {
      temp = lcmsImage_GetText( out_image, verbose, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyDeAllocateFunc_(temp); temp = 0;
    }
    hashTextAdd_m( "  </data_out>\n" );
  }
  hashTextAdd_m( tmp );

  hashTextAdd_m(   "</oyFilterNode_s>\n" );

  return oyStringCopy_( hash_text, allocateFunc );
#endif
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
    xform = lcmsCMMConversionContextCreate_( lps, 1, 0,0,
                                           image_input->layout_[0],
                                           image_output->layout_[0],
                                           node->core->options_,
                                           &ltw, cmm_ptr_out );
    CMMProfileRelease_M (lps[0] );
  }

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
  int i,j,k, n;
  int error = 0;
  int channels = 0;
  oyDATATYPE_e data_type = 0;

  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node;
  oyImage_s * image_input = 0, * image = 0;
  oyArray2d_s * array_in = 0, * array_out = 0;
  lcmsTransformWrap_s * ltw  = 0;
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

  if(data_type == oyFLOAT)
  {
    oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    message(oyMSG_WARN,0, "%s: %d can not handle oyFLOAT", __FILE__,__LINE__);
    error = 1;
  }

  if(!ticket->output_image)
  {
    message(oyMSG_WARN,0, "%s: %d no ticket->output_image", __FILE__,__LINE__);
    error = 1;
  }

  if(!error)
  {
    channels = oyToChannels_m( ticket->output_image->layout_[0] );

    error = lcmsCMMTransform_GetWrap_( node->backend_data, &ltw );
  }

  /* now do some position blind manipulations */
  if(ltw)
  {
    n = (int)(array_out->width+0.5) / channels;

    if(!(data_type == oyUINT8 ||
         data_type == oyUINT16 ||
         data_type == oyDOUBLE))
    {
      oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
      error = 1;
    }

    /*  - - - - - conversion - - - - - */
    /*message(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d Start lines: %d",
            __FILE__,__LINE__, array_out->height);*/
    if(!error)
#pragma omp parallel for
      for( k = 0; k < array_out->height; ++k)
        cmsDoTransform( ltw->lcms, array_in->array2d[k],
                                   array_out->array2d[k], n );
    /*message(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d End width: %d",
            __FILE__,__LINE__, n);*/


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
      <cmyk_cmyk_black_preservation.advanced>0</cmyk_cmyk_black_preservation.advanced>\n\
     </" "icc." CMM_NICK ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_INTERNAL ">\n"
};

#define A(long_text) STRING_ADD( tmp, long_text)

/** Function lcmsGetOptionsUI
 *  @brief   return XFORMS for matching options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/29 (Oyranos: 0.1.10)
 *  @date    2009/07/30
 */
int lcmsGetOptionsUI                 ( oyOptions_s        * options,
                                       char              ** ui_text,
                                       oyAlloc_f            allocateFunc )
{
  char * tmp = 0;

  tmp = (char *)oyOptions_FindString( options,
                                      "cmyk_cmyk_black_preservation", 0 );
  if(tmp == 0)
    return 0;

  tmp = oyStringCopy_( "\
  <h3>little CMS ", oyAllocateFunc_ );

  A(       _("Extended Options"));
  A(                         ":</h3>\n\
  <table>\n\
   <tr>\n\
    <td>" );
  A( _("Cmyk to Cmyk transformation"));
  A(              ":</td>\n\
    <td>\n\
     <xf:select1 ref=\"/" OY_TOP_INTERNAL "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc." CMM_NICK "/cmyk_cmyk_black_preservation\">\n\
      <xf:choices label=\"" );
  A(                   _("Black Preservation"));
  A(                                "\">\n\
       <xf:item>\n\
        <xf:label>0 - none</xf:label>\n\
        <xf:value>0</xf:value>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:label>1 - LCMS_PRESERVE_PURE_K</xf:label>\n\
        <xf:value>1</xf:value>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:label>2 - LCMS_PRESERVE_K_PLANE</xf:label>\n\
        <xf:value>2</xf:value>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
    </td>\n\
   </tr>\n\
  </table>\n\
" );

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
  "icc.colour." CMM_NICK "._CPU._ACCEL",

  lcmsVERSION,
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lcmsFilterPlug_CmmIccRun,  /* oyCMMFilterPlug_Run_f */
  lcmsTRANSFORM,             /* data_type, "lcCC" */

  (oyConnector_s**) lcms_cmmIccPlug_connectors,/* plugs */
  1,                         /* plugs_n */
  0,                         /* plugs_last_add */
  (oyConnector_s**) lcms_cmmIccSocket_connectors,   /* sockets */
  1,                         /* sockets_n */
  0,                         /* sockets_last_add */
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
oyCMMapi4_s   lcms_api4_cmm = {

  oyOBJECT_CMM_API4_S,
  0,0,0,
  (oyCMMapi_s*) & lcms_api7_cmm,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,
  lcmsCMMCanHandle,

  OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc.colour." CMM_NICK "._CPU._NOACCEL",

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
  lcms_extra_options,   /* const char * options */
  lcmsGetOptionsUI      /* oyCMMuiGet_f oyCMMuiGet */
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
      return _("littleCMS project; www: http://www.littlecms.com; support/email: support@littlecms.com; sources: http://www.littlecms.com/downloads.htm; Oyranos wrapper: Kai-Uwe Behrmann for the Oyranos project");
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
      return _("The following options are available to create colour contexts:\n \"profiles_simulation\", a option of type oyProfiles_s, can contain device profiles for proofing.\n \"profiles_effect\", a option of type oyProfiles_s, can contain abstract colour profiles.\n The following Oyranos options are supported: \"rendering_high_precission\", \"rendering_gamut_warning\", \"rendering_intent_proof\", \"rendering_bpc\", \"rendering_intent\", \"proof_soft\" and \"proof_hard\".\n The additional lcms option is supported \"cmyk_cmyk_black_preservation\" [0 - none; 1 - LCMS_PRESERVE_PURE_K; 2 - LCMS_PRESERVE_K_PLANE]." );
  }
  return 0;
}
const char *lcms_texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance lcms_cmm_module
 *  @brief    lcms module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMMInfo_s lcms_cmm_module = {

  oyOBJECT_CMM_INFO_S,                 /**< type, struct type */
  0,0,0,                               /**< ,dynamic object functions */
  CMM_NICK,                            /**< cmm, ICC signature */
  "0.6",                               /**< backend_version */
  lcmsInfoGetText,                     /**< getText */
  (char**)lcms_texts,                  /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,                     /**< oy_compatibility */

  (oyCMMapi_s*) & lcms_api4_cmm,       /**< api */

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "lcms_logo2.png"}, /**< icon */
};

