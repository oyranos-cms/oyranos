
/** @file @internal
 *  @brief littleCMS backend for Oyranos
 *
 */

/** @date      12. 11. 2007 */


#include <lcms.h>

#include "oyranos_cmm.h"
#include "oyranos_icc.h"
#include "oyranos_definitions.h"
/*#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h" */ /* TODO */
#include <math.h>



/* --- internal definitions --- */

#define CMM_NICK "lcms"
#define CMMProfileOpen_M    cmsOpenProfileFromMem
#define CMMProfileRelease_M cmsCloseProfile
#define CMMToString_M(text) #text
#define CMMMaxChannels_M 16
#define lcmsPROFILE "lcPR"
#define lcmsTRANSFORM "lcCC"

int lcmsCMMWarnFunc( int code, const char * format, ... );
oyMessageFunc_t message = lcmsCMMWarnFunc;

int lcmsErrorHandlerFunction(int ErrorCode, const char *ErrorText);

int                lcmsCMMCheckPointer(oyCMMptr_s        * cmm_ptr,
                                       const char        * resource );

/** @brief lcms wrapper for profile data struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  10 december 2007 (API 0.1.8)
 */
typedef struct {
  int          type;                   /**< shall be lcPR */
  size_t       size;
  oyPointer    block;                  /**< Oyranos raw profile pointer. Dont free! */
  oyPointer    lcms;                   /**< cmsHPROFILE struct */
  icColorSpaceSignature sig;           /**< ICC profile signature */
} lcmsProfileWrap_s;

/** @brief lcms wrapper for transform data struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  20 december 2007 (API 0.1.8)
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

int                lcmsCMMInit       ( )
{
  int error = 0;
  cmsErrorAction( LCMS_ERROR_SHOW );
  cmsSetErrorHandler( lcmsErrorHandlerFunction );
  return error;
}

int                lcmsCMMCanHandle  ( oyCMMQUERY_e      type,
                                       int               value )
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
         ret = 1; break;
  }

  return ret;
}



/** @brief convert to lcms profile wrapper struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  10 december 2007 (API 0.1.8)
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

/** @brief convert to lcms transform wrapper struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  20 december 2007 (API 0.1.8)
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


/* oyCMMProfileOpen_t */
int                lcmsCMMProfile_Open ( oyPointer         block,
                                         size_t            size,
                                         oyCMMptr_s      * oy )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  if(oy->type != oyOBJECT_TYPE_CMM_POINTER_S || 
     strcmp(oy->cmm, CMM_NICK) != 0)
    error = 1;

  if(!error)
  {
    char type_[4] = lcmsPROFILE;
    int type = *((int*)&type_);
    lcmsProfileWrap_s * s = calloc(sizeof(lcmsProfileWrap_s), 1);

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

/* oyCMMProfileOpen_t */
oyChar     *       lcmsCMMProfile_GetText(oyCMMptr_s        * cmm_ptr,
                                       oyNAME_e            type,
                                       const char          language[4],
                                       const char          country[4],
                                       oyAllocFunc_t       allocateFunc )
{
  const char * name = 0;
  lcmsProfileWrap_s * s = lcmsCMMProfile_GetWrap_(cmm_ptr);

  if(!lcmsCMMCheckPointer( cmm_ptr, oyCMM_PROFILE ) && s)
  {
    if(language && country)
      cmsSetLanguage(language, country);

    switch(type)
    {
      case oyNAME_ID:
      case oyNAME_PROD_DESC:
           name = cmsTakeProductDesc( s->lcms );
           break;
      case oyNAME_PROD_NAME:
      case oyNAME_NAME:
           name = cmsTakeProductName( s->lcms );
           break;
      case oyNAME_INFO:
      case oyNAME_DESCRIPTION:
           name = cmsTakeProductInfo( s->lcms );
           break;
      case oyNAME_COPYRIGHT:
           name = cmsTakeCopyright( s->lcms );
           break;
      case oyNAME_MODELL:
           name = cmsTakeModel( s->lcms );
           break;
      case oyNAME_MANUFACTURER:
           name = cmsTakeManufacturer( s->lcms );
           break;
    }
  }

  if(name)
  {
    char * tmp = allocateFunc( sizeof(char) * strlen( name ) + 1 );

    sprintf( tmp, "%s", name );
    name = tmp;
  }

  return (char*)name;
}

/** @brief the API 1 to implement and set by a CMM
 *
 *  @since Oyranos: version 0.1.8
 *  @date  10 december 2007 (API 0.1.8)
 */
icSignature      lcmsCMMProfile_GetSignature (oyCMMptr_s * cmm_ptr,
                                       int                 pcs )
{
  icColorSpaceSignature sig = 0;
  int error = !cmm_ptr;
  cmsHPROFILE profile = 0;
  icHeader *h = 0;
  lcmsProfileWrap_s * s = 0;
  
  if(!error)
    s = lcmsCMMProfile_GetWrap_( cmm_ptr );

  if(!error)
    error = !s;

  if(!error)
    profile = s->lcms;

  if(s && s->sig)
    return s->sig;

  if(!error)
    h = (icHeader*) s->block;

  if(!error)
  {
    if(pcs)
      sig = cmsGetPCS( s->lcms );
    else
      sig = cmsGetColorSpace( s->lcms );
  }

  if(!error)
  {
    if(pcs)
      sig = oyValueCSpaceSig( h->pcs );
    else
      sig = oyValueCSpaceSig( h->colorSpace );
  }

  return sig;
}


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
    message(oyMSG_WARN,"%s: %d can not handle more than %d channels; found: %d",
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

/** @func lcmsTransformWrap_Set_
 *  @brief fill a lcmsTransformWrap_s struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 december 2007 (API 0.1.8)
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

int          lcmsCMMColourConversion_Create (
                                       oyCMMptr_s       ** cmm_profile,
                                       int                 profiles_n,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       int                 intent,
                                       int                 proofing_intent,
                                       uint32_t            flags,
                                       oyCMMptr_s        * oy )
{
  oyCMMptr_s * s = 0;
  oyPixel_t lcms_pixel_layout_in = 0;
  oyPixel_t lcms_pixel_layout_out = 0;
  cmsHPROFILE * lps = malloc(sizeof(cmsHPROFILE)*profiles_n+1);
  cmsHTRANSFORM xform = 0;
  int i;
  int error = !(cmm_profile && lps);
  icColorSpaceSignature colour_in = 0;
  icColorSpaceSignature colour_out = 0;
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
    error = !s;

  return error;
}

/** @func lcmsCMMColourConversion_FromMem
 *  @brief oyCMMColourConversion_FromMem_t implementation
 *
 *  Convert a lcms device link to a colour conversion context.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 december 2007 (API 0.1.8)
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
  oyPixel_t lcms_pixel_layout_in = 0;
  oyPixel_t lcms_pixel_layout_out = 0;
  cmsHTRANSFORM xform = 0;

  cmsHPROFILE dl = cmsOpenProfileFromMem(mem, size);

  lcms_pixel_layout_in  = oyPixelToCMMPixelLayout_( oy_pixel_layout_in,
                                                    colour_space_in );
  lcms_pixel_layout_out = oyPixelToCMMPixelLayout_( oy_pixel_layout_out,
                                                    colour_space_out );

  xform = cmsCreateTransform( dl, lcms_pixel_layout_in,
                              0,  lcms_pixel_layout_out,
                              intent, 0 );

  return !xform;
}

/** @func lcmsCMMColourConversion_ToMem
 *  @brief oyCMMColourConversion_ToMem_t implementation
 *
 *  convert a lcms colour conversion context to a device link
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 december 2007 (API 0.1.8)
 */
oyPointer  lcmsCMMColourConversion_ToMem (
                                       oyCMMptr_s        * oy,
                                       size_t            * size,
                                       oyAllocFunc_t       allocateFunc )
{
  int error = !oy;
  lcmsTransformWrap_s * s = lcmsCMMTransform_GetWrap_( oy );
  oyPointer data = 0;

  error = !s;

  if(!error)
  {
    cmsHPROFILE dl = cmsTransform2DeviceLink( s->lcms, 0 );

    *size = 0;

    _cmsSaveProfileToMem( dl, 0, size );
    data = allocateFunc( *size );
    _cmsSaveProfileToMem( dl, data, size );
  }

  return data;
}


int              lcmsCMMColourConversion_Run(
                                       oyCMMptr_s        * oy,
                                       oyPointer           in_data,
                                       oyPointer           out_data,
                                       size_t              count,
                                       oyCMMProgress_t     progress )
{
  int error = !oy;

  lcmsTransformWrap_s * s = lcmsCMMTransform_GetWrap_( oy );
  error = !s;

  if(!error && in_data && out_data)
  {
    oyDATATYPE_e dt = oyToDataType_m(s->oy_pixel_layout_in);
    if(dt == oyDOUBLE)
    {
      size_t i = 0;
      double * d = in_data;
      int chan = oyToChannels_m(s->oy_pixel_layout_in);
      size_t n = chan * count;
      /*for(i = 0; i < n; ++i)
        d[i] = d[i] * 100.;*/
    }

    if(count > 0)
      cmsDoTransform( s->lcms, in_data, out_data, count);

    dt = oyToDataType_m(s->oy_pixel_layout_out);
    if(dt == oyDOUBLE)
    {
      int i = 0;
      double * d = out_data;
      int chan = oyToChannels_m(s->oy_pixel_layout_out);
      int n = chan * count;
      /*for(i = 0; i < n; ++i)
        d[i] = d[i] / 100.;*/
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

int lcmsCMMWarnFunc( int code, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;

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
  fprintf( stderr, text ); fprintf( stderr, "\n" );
  free( text );

  return 0;
}

int lcmsErrorHandlerFunction(int ErrorCode, const char *ErrorText)
{
  int code = 0;
  switch(ErrorCode) {
  case LCMS_ERRC_WARNING: code = oyMSG_WARN; break;
  case LCMS_ERRC_RECOVERABLE: code = oyMSG_WARN; break;
  case LCMS_ERRC_ABORTED: code = oyMSG_ERROR; break;
  default: code = ErrorCode;
  }
  message( code, ErrorText, 0 );
  return 0;
}

int            lcmsCMMMessageFuncSet ( oyMessageFunc_t     message_func )
{
  message = message_func;
  return 0;
}



oyCMMapi1_s  lcms_api1 = {

  oyOBJECT_TYPE_CMM_API1_S,
  0,
  
  lcmsCMMInit,
  lcmsCMMMessageFuncSet,

  lcmsCMMCanHandle,
  lcmsCMMProfile_Open,
  lcmsCMMProfile_GetText,
  lcmsCMMProfile_GetSignature,
  lcmsCMMColourConversion_Create,
  lcmsCMMColourConversion_FromMem,
  lcmsCMMColourConversion_ToMem,
  lcmsCMMColourConversion_Run,
};


oyCMMInfo_s lcms_cmm_module = {

  oyOBJECT_TYPE_CMM_INFO_S,
  lcmsSignature,
  "0.6",
  {oyOBJECT_TYPE_NAME_S, "lcms", "Little CMS", "LittleCMS is a CMM, a color management engine; it implements fast transforms between ICC profiles. \"Little\" stands for its small overhead. With a typical footprint of about 100K including C runtime, you can color-enable your application without the pain of ActiveX, OCX, redistributables or binaries of any kind. We are using little cms in several commercial projects, however, we are offering lcms library free for anybody under an extremely liberal open source license."},
  {oyOBJECT_TYPE_NAME_S, "Marti", "Marti Maria", "littleCMS project; www: http://www.littlecms.com; support/email: support@littlecms.com; sources: http://www.littlecms.com/downloads.htm"},
  {oyOBJECT_TYPE_NAME_S, "MIT", "Copyright (c) 1998-2007 Marti Maria Saguer", "MIT license: http://www.opensource.org/licenses/mit-license.php"},
  108,

  (oyCMMapi_s*) & lcms_api1,
  0,

  {oyOBJECT_TYPE_ICON_S, 0,0,0, "lcms_logo2.png"}

};

