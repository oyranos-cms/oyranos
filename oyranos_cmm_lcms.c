/** @file oyranos_cmm_lcms.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2007-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    littleCMS CMM backend for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */




#include <lcms.h>

#include "oyranos_cmm.h"         /* the API this CMM implements */

#include <math.h>



/* --- internal definitions --- */

#define CMM_NICK "lcms"
#define CMMProfileOpen_M    cmsOpenProfileFromMem
#define CMMProfileRelease_M cmsCloseProfile
#define CMMToString_M(text) #text
#define CMMMaxChannels_M 16
#define lcmsPROFILE "lcPR"
#define lcmsTRANSFORM "lcCC"

int lcmsCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );
oyMessage_f message = lcmsCMMWarnFunc;

int lcmsErrorHandlerFunction(int ErrorCode, const char *ErrorText);

int                lcmsCMMCheckPointer(oyCMMptr_s        * cmm_ptr,
                                       const char        * resource );

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





/* --- implementations --- */

/** @func    lcmsCMMInit
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

/** @func    lcmsCMMCanHandle
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



/** @func    lcmsCMMProfile_GetWrap_
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

  if(cmm_ptr && !lcmsCMMCheckPointer( cmm_ptr, oyCMM_PROFILE ) &&
     cmm_ptr->ptr)
    s = (lcmsProfileWrap_s*) cmm_ptr->ptr;

  if(s && s->type != type)
    s = 0;

  return s;
}

/** @func    lcmsCMMTransform_GetWrap_
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

  if(cmm_ptr && !lcmsCMMCheckPointer( cmm_ptr, oyCMM_COLOUR_CONVERSION ) &&
     cmm_ptr->ptr)
    s = (lcmsTransformWrap_s*) cmm_ptr->ptr;

  if(s && (s->type != type || !s->lcms))
    s = 0;

  return s;
}

/** @func    lcmsCMMProfileReleaseWrap
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


/** @func    lcmsCMMProfileReleaseWrap
 *  @brief   oyCMMProfileOpen_t implementation
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/12
 *  @since   2007/11/12 (Oyranos: 0.1.8)
 */
int          lcmsCMMProfile_Open     ( oyStruct_s        * data,
                                       oyCMMptr_s        * oy )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  if(oy->type != oyOBJECT_CMM_POINTER_S || 
     strcmp(oy->cmm, CMM_NICK) != 0)
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
    sprintf( oy->func_name, "%s", CMMToString_M(CMMProfileOpen_M) );
    sprintf( oy->resource, oyCMM_PROFILE ); 
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

/** @func    lcmsCMMCheckPointer
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

  if(cmm_ptr && cmm_ptr->ptr && strlen(cmm_ptr->resource))
  {
    int * cmm_id = (int*)cmm_ptr->cmm;
    int * res_id = (int*)cmm_ptr->resource;
    int * oy_id = (int*)CMM_NICK;

    if(*cmm_id != *oy_id || *res_id != *((int*)(resource)) )
      error = 1;
  } else {
    error = 1;
  }

  return error;
}



/** @func    oyPixelToCMMPixelLayout_
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
    message(oyMSG_WARN,0,
                "%s: %d can not handle more than %d channels; found: %d",
                __FILE__,__LINE__,CMMMaxChannels_M, chan_n);

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

/** @func    lcmsCMMDeleteTransformWrap
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

/** @func    lcmsTransformWrap_Set_
 *  @brief   fill a lcmsTransformWrap_s struct
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/12/21
 */
lcmsTransformWrap_s* lcmsTransformWrap_Set_ (
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

/** @func    lcmsCMMColourConversion_Create
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
  oyPixel_t lcms_pixel_layout_in = 0;
  oyPixel_t lcms_pixel_layout_out = 0;
  cmsHPROFILE * lps = malloc(sizeof(cmsHPROFILE)*profiles_n+1);
  cmsHTRANSFORM xform = 0;
  int i;
  int error = !(cmm_profile && lps);
  icColorSpaceSignature colour_in = 0;
  icColorSpaceSignature colour_out = 0;
  lcmsTransformWrap_s * ltw  = 0;
  int intent = 0,
      bpc = 0,
      gamut_warning = 0,
      flags = 0;
  const char * o_txt = 0;

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

  error = !xform;

  if(!error)
    ltw = lcmsTransformWrap_Set_( xform, colour_in, colour_out,
                                  oy_pixel_layout_in, oy_pixel_layout_out, oy );

  free(lps);

  if(!error)
    error = !ltw;

  return error;
}

/** @func    lcmsCMMColourConversion_FromMem
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
                       CMM_NICK, {0}, 0, {0}, 0, 0 };
  oyCMMptr_s * dls[] = {0, 0};
  oyProfile_s p = { oyOBJECT_PROFILE_S,0,0,0,0,0,0,0,0,0,0,0 };

  dls[0] = &intern;
  p.block_ = mem;
  p.size_ = size;

  error = lcmsCMMProfile_Open ( (oyStruct_s*)&p, &intern );
  error = lcmsCMMColourConversion_Create (
                                       dls, 1,
                                       oy_pixel_layout_in, oy_pixel_layout_out,
                                       0, oy );
  return error;
}

/** @func    lcmsCMMColourConversion_ToMem
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
    cmsHPROFILE dl = cmsTransform2DeviceLink( s->lcms, 0 );

    *size = 0;

    {
        int nargs = 1, i;
        size_t size = sizeof(int) + nargs * sizeof(cmsPSEQDESC);
        LPcmsSEQ pseq = (LPcmsSEQ) malloc(size);
        
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


/** @func    lcmsCMMColourConversion_Run
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
    error = filter->filter_type_ != oyFILTER_TYPE_COLOUR;

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
uint32_t lcms_cache_data_types[] = { oyOBJECT_PROFILE_S, oyOBJECT_IMAGE_S, 0 };

/** Function lcmsCMMFilterNode_CreateContext
 *  @brief   implement oyCMMFilterNode_CreateContext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
int      lcmsCMMFilterNode_CreateContext (
                                       oyFilterNode_s    * node,
                                       oyCMMptr_s       ** cmm_profile_array,
                                       int                 profiles_n,
                                       oyCMMptr_s        * oy )
{
  int error = !node;

  

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
                                       oyPixelAccess_s   * ticket,
                                       oyArray2d_s      ** pixel )
{
  int i,j,k, n;
  int error = 0;
  double * in_values = 0, * out_values = 0;
  int channels = 0;
  oyDATATYPE_e data_type = 0;

  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterPlug_s * plug = 0;
  oyFilter_s * filter = 0,
             * input_filter = 0;
  oyImage_s * input_image = 0;
  oyArray2d_s * array = 0;
  

  filter = socket->node->filter;
  plug = (oyFilterPlug_s *)socket->node->plugs[0];
  input_filter = plug->remote_socket_->node->filter;
  input_image = (oyImage_s*)plug->remote_socket_->data;

  /* We let the input filter do its processing first. */
  error = input_filter->api4_->oyCMMFilterPlug_Run( plug, ticket, pixel);
  if(error != 0) return error;

  array = *pixel;

  data_type = oyToDataType_m( input_image->layout_[0] );

  if(data_type == oyFLOAT)
  {
    oyFilterSocket_Callback( socket, oyCONNECTOR_EVENT_INCOMPATIBLE_IMAGE );
    message(oyMSG_WARN,0, "%s: %d can not handle oyFLOAT", __FILE__,__LINE__);
  }

  channels = oyToChannels_m( input_image->layout_[0] );

  /* now do some position blind manipulations */
  for( k = 0; k < array->height; ++k)
  {
    if(data_type == oyDOUBLE)
    {
      in_values = (double*) array->array2d[k];
      out_values = (double*) array->array2d[k];
      n = array->width / channels;
      for(i = 0; i < n; ++i)
        for(j = 0; j < channels; ++j)
          out_values[i*channels + j] = in_values[i*channels + j] - 0.5;
    }
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

/** @func    lcmsCMMWarnFunc
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
    type_name = oyStruct_TypeToText( context );
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

/** @func    lcmsErrorHandlerFunction
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

/** @func    lcmsCMMMessageFuncSet
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
     <" CMM_NICK ">\n\
      <rendering_bpc>0</rendering_bpc>\n\
      <cmyk_cmky_black_preservation.advanced>0</cmyk_cmky_black_preservation.advanced>\n\
     </" CMM_NICK ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_INTERNAL ">\n"
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
  0,

  lcmsCMMInit,
  lcmsCMMMessageFuncSet,
  lcmsCMMCanHandle,

  OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "colour/lcms",

  {0,0,1},

  lcms_cache_data_types,

  lcmsFilter_CmmIccValidateOptions,
  lcmsWidgetEvent,

  lcmsCMMProfile_Open, /* oyCMMDataOpen_f */
  lcmsCMMFilterNode_CreateContext, /* oyCMMFilterNode_CreateContext_f */
  0, /* lcmsFilterNode_CmmIccContextToMem */ /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextFromMem_f */
  lcmsFilterPlug_CmmIccRun,

  {oyOBJECT_NAME_S, 0,0,0, "colour", "Colour", "ICC compatible CMM"},
  "Colour/CMM/littleCMS", /* category */
  lcms_extra_options,   /* options */
  0,   /* opts_ui_ */

  lcms_cmmIccPlug_connectors,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  lcms_cmmIccSocket_connectors,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */

  0    /* api5_; keep empty */
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

  lcmsCMMProfile_Open,
  lcmsCMMColourConversion_Create,
  lcmsCMMColourConversion_FromMem,
  lcmsCMMColourConversion_ToMem,
  lcmsCMMColourConversion_Run,
};


/** @instance lcms_cmm_module
 *  @brief    lcms module infos
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
oyCMMInfo_s lcms_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.6",
  {oyOBJECT_NAME_S, 0,0,0,"lcms", "Little CMS", "LittleCMS is a CMM, a color management engine; it implements fast transforms between ICC profiles. \"Little\" stands for its small overhead. With a typical footprint of about 100K including C runtime, you can color-enable your application without the pain of ActiveX, OCX, redistributables or binaries of any kind. We are using little cms in several commercial projects, however, we are offering lcms library free for anybody under an extremely liberal open source license."},
  {oyOBJECT_NAME_S, 0,0,0,"Marti", "Marti Maria", "littleCMS project; www: http://www.littlecms.com; support/email: support@littlecms.com; sources: http://www.littlecms.com/downloads.htm"},
  {oyOBJECT_NAME_S, 0,0,0,"MIT", "Copyright (c) 1998-2008 Marti Maria Saguer", "MIT license: http://www.opensource.org/licenses/mit-license.php"},
  108,

  (oyCMMapi_s*) & lcms_api1,
  0,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "lcms_logo2.png"}

};

