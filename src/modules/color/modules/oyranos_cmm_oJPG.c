/** @file oyranos_cmm_oJPG.c
 *
 *  JPEG file i/o module for Oyranos 
 *
 *  @par Copyright:
 *            2014-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    JPEG filter for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <oy@oyranos.org>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2014/03/21
 */

#include "oyCMM_s.h"
#include "oyCMMinfo_s.h"
#include "oyCMMapi4_s.h"
#include "oyCMMapi7_s.h"
#include "oyCMMapiFilter_s.h"
#include "oyCMMui_s.h"
#include "oyConnectorImaging_s.h"
#include "oyProfiles_s.h"

#include "oyranos_cmm.h"
#include "oyranos_config.h"
#include "oyranos_definitions.h"
#include "oyranos_helper.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>   /* isspace() */
#include <setjmp.h>  /* libjpeg specific error handling */

#include "jpegmarkers.h"

/* --- internal definitions --- */

/** The CMM_NICK consists of four bytes, which appear as well in the library name. This is important for Oyranos to identify the required filter struct name. */
#define CMM_NICK "oJPG"

/** The message function pointer to use for messaging. */
oyMessage_f ojpg_msg = oyMessageFunc;

/* Helpers */
#ifndef OY_DBG_FORMAT_
# if defined(__GNUC__)
#  define  OY_DBG_FORMAT_ "%s:%d %s() "
#  define  OY_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__,__func__
# else
#  define  OY_DBG_FORMAT_ "%s:%d "
#  define  OY_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
# endif
#endif
#define _DBG_FORMAT_ OY_DBG_FORMAT_
#define _DBG_ARGS_ OY_DBG_ARGS_

/* i18n */
#include "oyranos_i18n.h"

#define AD oyAllocateFunc_, oyDeAllocateFunc_

int  ojpgInit                        ( oyStruct_s        * module_info );
int  ojpgReset                       ( oyStruct_s        * module_info );
oyImage_s *  oyImage_FromJPEG        ( const char        * filename,
                                       int32_t             icc_profile_flags );
int          oyImage_WriteJPEG       ( oyImage_s         * image,
                                       const char        * filename,
                                       oyOptions_s       * options );
int      ojpgFilter_CmmRunRead       ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
int      ojpgFilter_CmmRunWrite      ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
const char * ojpgApi4UiGetText2Read  ( const char        * select,
                                       oyNAME_e            type,
                                       const char        * format );
const char * ojpgApi4UiGetText2Write ( const char        * select,
                                       oyNAME_e            type,
                                       const char        * format );
int                ojpgGetOFORMS     ( oyCMMapiFilter_s  * module,
                                       oyOptions_s       * oy_opts,
                                       int                 flags,
                                       char             ** ui_text,
                                       oyAlloc_f           allocateFunc );
const char * ojpgApi4UiGetTextRead   ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * ojpgApi4UiGetTextWrite  ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
extern const char * ojpg_api4_ui_texts[];
char * ojpgFilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );

/* --- implementations --- */

/** Function ojpgCMMInit
 *  @brief API requirement
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2014/03/21
 */
int                ojpgCMMInit       ( oyStruct_s * s OY_UNUSED )
{
  int error = 0;
  return error;
}

/** Function ojpgCMMReset
 *  @brief API requirement
 */
int                ojpgCMMReset      ( oyStruct_s * s OY_UNUSED )
{
  return 0;
}



/** Function ojpgCMMMessageFuncSet
 *  @brief API requirement
 *
 *  A Oyranos user might want its own message function and omit the default
 *  one.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2014/03/21
 */
int            ojpgCMMMessageFuncSet ( oyMessage_f         message_func )
{
  ojpg_msg = message_func;
  return 0;
}


/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  Implement at least "name", "manufacturer" and "copyright". If you like with
 *  internationalisation.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2014/03/21
 */
const char * ojpgGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select, "name")==0)
    if(type == oyNAME_NICK)
      return _(CMM_NICK);

  return oyCMMgetText( select, type, context );
}
oyIcon_s ojpg_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @brief    ojpg module infos
 *
 *  This structure is dlopened by Oyranos. Its name has to consist of the
 *  following elements:
 *  - the four byte CMM_NICK plus
 *  - "_cmm_module"
 *  This string must be included in the the filters filename.
 *
 *  @version Oyranos: 0.9.7
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2018/08/25
 */
oyCMM_s oJPG_cmm_module = {

  oyOBJECT_CMM_INFO_S, /**< ::type; the object type */
  0,0,0,               /**< static objects omit these fields */
  CMM_NICK,            /**< ::cmm; the four char filter id */
  (char*)"0.9.7",      /**< ::backend_version */
  ojpgGetText,         /**< ::getText; UI texts */
  (char**)oyCMM_texts, /**< ::texts; list of arguments to getText */
  OYRANOS_VERSION,     /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
  NULL,

  /** ::icon; module icon */
  &ojpg_icon,
  ojpgInit,
  ojpgReset
};


#define OY_oJPG_FILTER_REGISTRATION_BASE OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH

/* OY_oJPG_FILTER_REGISTRATION_READ -----------------------------------------*/


static char * oJPG_registration_7read = NULL;
/** @brief    ojpg oyCMMapi7_s implementation
 *
 *  a filter providing a CMM filter
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/03/21
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 */
oyCMMapi_s * ojpgApi7CmmCreateRead   ( const char        * format,
                                       const char        * ext )
{
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {0,9,7};
  static oyDATATYPE_e data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                       oyHALF, oyFLOAT, oyDOUBLE, (oyDATATYPE_e)0};
  oyConnectorImaging_s * socket = oyConnectorImaging_New(0);
  static oyConnectorImaging_s * plugs[2] = {0,0},
                              * sockets[2] = {0,0};
  
  const char * properties[] =
  {
    "file=read",    /* file read|write */
    "image=pixel",  /* image type, pixel/vector/font */
    "layers=1",     /* layer count, one for plain images */
    "icc=1",        /* image type ICC profile support */
    "ext=jpg,jpeg", /* supported extensions */
    0
  };

  sockets[0] = socket;

  oyStringAddPrintf( &oJPG_registration_7read, AD,
                     OY_oJPG_FILTER_REGISTRATION_BASE"file_read.input_%s._%s._CPU._ACCEL", format, CMM_NICK );

  if(oy_debug >= 2) ojpg_msg(oyMSG_DBG, NULL, _DBG_FORMAT_ "registration:%s ojpg %s", _DBG_ARGS_,
                             oJPG_registration_7read,
                             ext );


  oyConnectorImaging_SetDataTypes( socket, data_types, 6 );
  oyConnectorImaging_SetReg( socket, "//" OY_TYPE_STD "/image.data" );
  oyConnectorImaging_SetMatch( socket, oyFilterSocket_MatchImagingPlug );
  oyConnectorImaging_SetTexts( socket, oyCMMgetImageConnectorSocketText,
                               oy_image_connector_texts );
  oyConnectorImaging_SetIsPlug( socket, 0 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET, -1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT, 16 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT, 16 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_ID, 1 );

  oyCMMapi7_s * cmm7 = oyCMMapi7_Create( ojpgCMMInit, ojpgCMMReset, ojpgCMMMessageFuncSet,
                                       oJPG_registration_7read,
                                       cmm_version, module_api,
                                       NULL,
                                       ojpgFilter_CmmRunRead,
                                       (oyConnector_s**)plugs, 0, 0,
                                       (oyConnector_s**)sockets, 1, 0,
                                       properties, 0 );
  oyFree_m_(oJPG_registration_7read);
  return (oyCMMapi_s*) cmm7;
}

int deAllocData ( void ** data ) { oyDeAllocateFunc_(*data); *data = NULL; return 0; }
const char ojpg_read_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "file_read" ">\n\
      <filename></filename>\n\
     </" "file_read" ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

static char * oJPG_registration_4read = NULL;
/** @brief    ojpg oyCMMapi4_s implementation
 *
 *  a filter providing a CMM device link creator
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2014/03/21
 */
oyCMMapi_s * ojpgApi4CmmCreateRead   ( const char        * format )
{
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {0,9,7};
  oyPointer_s * backend_context = oyPointer_New(0);
  const char * category = ojpgApi4UiGetText2Read("category", oyNAME_NAME, format);
  oyCMMuiGet_f getOFORMS = ojpgGetOFORMS;
  oyCMMui_s * ui = oyCMMui_Create( category, ojpgApi4UiGetTextRead,
                                   ojpg_api4_ui_texts, 0 );
  oyOptions_s * oy_opts = NULL;
  const char * oforms_options = ojpg_read_extra_options;

  oyCMMui_SetUiOptions( ui, oforms_options, getOFORMS ); 

  oyPointer_Set( backend_context, NULL, "ojpg_file_format", oyStringCopy(format, oyAllocateFunc_),
                 "char*", deAllocData );

  oyStringAddPrintf( &oJPG_registration_4read, AD,
                     OY_oJPG_FILTER_REGISTRATION_BASE"file_read.input_%s._" CMM_NICK "._CPU._ACCEL", format );

  oyCMMapi4_s * cmm4 = oyCMMapi4_Create( ojpgCMMInit, ojpgCMMReset, ojpgCMMMessageFuncSet,
                                       oJPG_registration_4read,
                                       cmm_version, module_api,
                                       "",
                                       NULL,
                                       ojpgFilterNode_GetText,
                                       ui,
                                       NULL );
  oyFree_m_(oJPG_registration_4read);


  oyCMMapi4_SetBackendContext( cmm4, backend_context );
  oyPointer_Release( &backend_context );
  oyOptions_Release( &oy_opts );

  return (oyCMMapi_s*)cmm4;
}


char * ojpgFilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type OY_UNUSED,
                                       oyAlloc_f           allocateFunc )
{
  char * t = NULL;
  const char * tmp = NULL;
  oyOptions_s * node_options = oyFilterNode_GetOptions( node, 0 );

  tmp = oyOptions_GetText(node_options, oyNAME_NICK);
  if(tmp)
    t = oyStringCopy( tmp, allocateFunc );

  oyOptions_Release( &node_options );

  return t;
}

#define A(long_text) oyStringAdd_( &tmp, long_text, AD )

/* TODO */
int                ojpgGetOFORMS     ( oyCMMapiFilter_s  * module OY_UNUSED,
                                       oyOptions_s       * oy_opts OY_UNUSED,
                                       int                 flags OY_UNUSED,
                                       char             ** ui_text,
                                       oyAlloc_f           allocateFunc OY_UNUSED )
{
  int error = 0;
  char * tmp = NULL;

  *ui_text = tmp;
  
  return error;
}


int select_icc_profile(j_decompress_ptr cinfo,
                      const char * filename,
                      JOCTET **icc_data_ptr,
                      unsigned int *icc_data_len)
{
  unsigned int len;
  int lIsITUFax = jpeg_get_marker_size( cinfo, JPEG_APP0+1, (JOCTET*)"G3FAX", 5, &len ) == 0;

  {
    char * profile_name = 0;
    char * prof_mem = 0;
    size_t size = 0;
    switch(cinfo->out_color_space)
    {
    case JCS_GRAYSCALE:
         profile_name = oyGetDefaultProfileName (oyASSUMED_GRAY, malloc);
         break;
    case JCS_RGB:
         if(lIsITUFax)
         {
           profile_name = strdup("ITULab.icc");
           if( !oyCheckProfile (profile_name, 0) )
             prof_mem = (char*)oyGetProfileBlock( profile_name, &size, malloc );
           else if(!oyCheckProfile ("ITUFAX.ICM", 0) )
             prof_mem = (char*)oyGetProfileBlock( "ITUFAX.ICM", &size, malloc );

           cinfo->out_color_space = JCS_YCbCr;  // do'nt convert colors
         } else {
           /* guesswork */
           const char * fn = strrchr( filename, OY_SLASH_C );
           if(fn)
             fn += 1;
           else
             fn = filename;

           if(fn[0] == '_') /* Canon RAW AdobeRGB */
             profile_name = strdup("compatibleWithAdobeRGB1998.icc");
           else
           {
             profile_name = strdup("YCbCr-Jpeg_v1_oyra.icc");
             if( !oyCheckProfile (profile_name, 0) )
             {
               prof_mem = (char*)oyGetProfileBlock( profile_name, &size, malloc );
               cinfo->out_color_space = JCS_YCbCr;  // do'nt convert colors
             } else
               profile_name = oyGetDefaultProfileName (oyASSUMED_RGB, malloc);
           }
         }
         break;
    case JCS_CMYK:
         profile_name = oyGetDefaultProfileName (oyASSUMED_CMYK, malloc);
         break;
    case JCS_YCbCr:
         if(lIsITUFax)
           profile_name = strdup("ITULab.icc");
         if( !oyCheckProfile (profile_name, 0) )
           prof_mem = (char*)oyGetProfileBlock( profile_name, &size, malloc );
         else if(!oyCheckProfile ("ITUFAX.ICM", 0) )
           prof_mem = (char*)oyGetProfileBlock( "ITUFAX.ICM", &size, malloc );
         else
           profile_name = strdup("YCbCr-Jpeg_v1_oyra.icc");
         break;
    case JCS_UNKNOWN:
    case JCS_YCCK:
         break;
    }

    if( !oyCheckProfile (profile_name, 0) )
      prof_mem = (char*)oyGetProfileBlock( profile_name, &size, malloc );

    *icc_data_ptr = (JOCTET*)prof_mem;
    *icc_data_len = size;

    if(profile_name) free( profile_name );

    if(size && prof_mem)
      return 1;
  }

  return 0;
}

/* Taken from the libjpeg's example.c */
typedef struct oJPG_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
} * oJPG_error_ptr;


static void
oJPG_error_exit (j_common_ptr cinfo)
{
  oJPG_error_ptr myerr = (oJPG_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp (myerr->setjmp_buffer, 1);
}

oyImage_s *  oyImage_FromJPEG        ( const char        * filename,
                                       int32_t             icc_profile_flags )
{
  oyOptions_s * tags = 0;
  FILE * fp = 0;
  oyDATATYPE_e data_type = oyUINT8;
  oyPROFILE_e profile_type = oyASSUMED_RGB;
  oyProfile_s * prof = 0;
  oyImage_s * image = NULL;
  oyPixel_t pixel_type = 0;
  size_t  fsize = 0;
  uint8_t * buf = 0;
  size_t  mem_n = 0;   /* needed memory in bytes */
  int width,height,nchannels;
  const char * format = "jpeg";

  /* file tests */
  if(filename)
    fp = fopen( filename, "rm" );

  if(!fp)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)NULL,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m( filename ) );
    return NULL;
  }

  /* file size fun */
  fseek(fp,0L,SEEK_END);
  fsize = ftell(fp);
  rewind(fp);
  if(oy_debug)
    ojpg_msg( oyMSG_DBG, (oyStruct_s*)NULL,
             OY_DBG_FORMAT_ "file size %u",
             OY_DBG_ARGS_, fsize );



  /* get ICC Profile */
  {
    struct jpeg_decompress_struct cinfo; 
    struct oJPG_error_mgr jerr;
    unsigned int len = 0;
    unsigned char * icc = NULL;
    int m;

    cinfo.err = jpeg_std_error (&jerr.pub);
    jerr.pub.error_exit = oJPG_error_exit;

    // Provide custom error handling to avoid libjpeg calling exit()
    if( setjmp( jerr.setjmp_buffer ))
    {
      jpeg_destroy_decompress (&cinfo);
      ojpg_msg( oyMSG_WARN, (oyStruct_s*)NULL,
             OY_DBG_FORMAT_ "Exit from libjpeg for %s",
             OY_DBG_ARGS_, oyNoEmptyString_m( filename ) );
      goto ojpgReadClean;
    }

    // Setup decompression structure
    jpeg_create_decompress (&cinfo);

    jpeg_stdio_src (&cinfo, fp);

    for (m = 0; m < 16; m++)
      jpeg_save_markers(&cinfo, JPEG_APP0 + m, 0xFFFF);

    (void) jpeg_read_header (&cinfo, TRUE);

    if( jpeg_get_marker_size( &cinfo, JPEG_APP0+2, (JOCTET*)"ICC_PROFILE", 12, &len ) == 0 )
    {
      icc = (unsigned char*) malloc(len);
      jpeg_get_marker_data( &cinfo, JPEG_APP0+2, (JOCTET*)"ICC_PROFILE", 12, len, (JOCTET*)icc );
    }

    if (icc && len)
    { if(oy_debug)
      ojpg_msg( oyMSG_DBG, (oyStruct_s*)NULL, OY_DBG_FORMAT_ "jpeg embedded profile found: %d", OY_DBG_ARGS_, len);
    } else if (select_icc_profile(&cinfo, filename, &icc, &len))
    { if(oy_debug)
      ojpg_msg( oyMSG_DBG, (oyStruct_s*)NULL, OY_DBG_FORMAT_ "jpeg default profile selected: %d", OY_DBG_ARGS_, len);
    } else
      if(oy_debug)
      ojpg_msg( oyMSG_DBG, (oyStruct_s*)NULL, OY_DBG_FORMAT_ "jpeg no profile found", OY_DBG_ARGS_);

    if(icc && len)
    {
      prof = oyProfile_FromMem( len, icc, 0, 0 );
      len = 0;
    }
    if(icc) oyFree_m_(icc);

    jpeg_start_decompress (&cinfo);

    nchannels = cinfo.out_color_components;
    width = cinfo.output_width;
    height = cinfo.output_height;

    /* allocate a buffer to hold the whole image */
    mem_n = width*height*oyDataTypeGetSize(data_type)*nchannels;
    if(mem_n)
    {
      buf = (uint8_t*) oyAllocateFunc_(mem_n * sizeof(uint8_t));
      if(!buf)
      {
        ojpg_msg(oyMSG_WARN, (oyStruct_s *) NULL, _DBG_FORMAT_ "Could not allocate enough memory.", _DBG_ARGS_);
        goto ojpgReadClean;
      }
    } else
    {
      ojpg_msg( oyMSG_WARN, (oyStruct_s *) NULL, _DBG_FORMAT_ "nothing to allocate: %dx%dx%d", _DBG_ARGS_,
                width, height, nchannels );
      goto ojpgReadClean;
    }
    if(oy_debug)
    ojpg_msg( oyMSG_DBG, (oyStruct_s *) NULL, _DBG_FORMAT_ "allocate image data: 0x%x size: %d ", _DBG_ARGS_, (int)(intptr_t)
              buf, mem_n );


    while (cinfo.output_scanline < (unsigned)height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    JSAMPROW b = &buf[(cinfo.output_width * nchannels)*cinfo.output_scanline];
    jpeg_read_scanlines(&cinfo, &b, 1);
    }

    icColorSpaceSignature csp = (icColorSpaceSignature) oyProfile_GetSignature(prof,oySIGNATURE_COLOR_SPACE);
    if(csp == icSigCmykData)
    {
      int n = width * height * 4;
      if(data_type == oyUINT8)
      {
        uint8_t * d = (uint8_t*)buf;
        int i;
#pragma omp parallel for private(i)
        for(i = 0; i < n; ++i)
          d[i] = 255 - d[i];
      }
    }
 
    jpeg_finish_decompress (&cinfo);
    jpeg_destroy_decompress (&cinfo);
  }

  /* fallback profile */
  if(!prof)
    prof = oyProfile_FromStd( profile_type, icc_profile_flags, 0 );

  if(oy_debug)
    ojpg_msg( oyMSG_DBG, (oyStruct_s*)NULL,
             OY_DBG_FORMAT_ "%dx%d %s|%s[%d]",
             OY_DBG_ARGS_,  width, height,
             format, oyDataTypeToText(data_type), nchannels );

  /* create a Oyranos image */
  pixel_type = oyChannels_m(nchannels) | oyDataType_m(data_type); 
  image = oyImage_Create( width, height, NULL, pixel_type, prof, 0 );
  oyProfile_Release( &prof );
  oyArray2d_s * a = oyArray2d_Create( buf, width*nchannels, height, data_type, NULL );
  oyImage_ReadArray(image, NULL, a, NULL);
  oyArray2d_Release( &a );
  free(buf); buf = NULL;

  if (!image)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)NULL,
             OY_DBG_FORMAT_ "can't create a new image\n%dx%d %s[%d]",
             OY_DBG_ARGS_,  width, height, format, nchannels );
    goto ojpgReadClean;
  }

  /* remember the meta data like file name */
  tags = oyImage_GetTags( image );
  oyOptions_SetFromString( &tags, "//" OY_TYPE_STD "/file_read.input_ojpg"
                                                                    "/filename",
                                 filename, OY_CREATE_NEW );

ojpgReadClean:
  oyOptions_Release( &tags );
  if(fp) fclose(fp);
  return image;
}

/** Function ojpgFilter_CmmRunRead
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  The primary filter entry for data processing.
 *
 *  @param         requestor_plug      the plug of the requesting node after 
 *                                     my filter in the graph
 *  @param         ticket              the job ticket
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2014/03/21
 */
int      ojpgFilter_CmmRunRead       ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = 0;
  oyStruct_s * socket_data = 0;
  oyFilterNode_s * node = 0;
  int error = 0;
  const char * filename = 0;
  oyImage_s * image_in = 0,
            * output_image = 0;
  int32_t icc_profile_flags = 0;

  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S)
  {
    socket = oyFilterPlug_GetSocket( requestor_plug );
    socket_data = oyFilterSocket_GetData( socket );
  }

  /* passing through the data reading */
  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     socket_data)
  {
    error = oyFilterPlug_ImageRootRun( requestor_plug, ticket );

    goto ojpgFilter_CmmRunClean;

  } else if(requestor_plug->type_ == oyOBJECT_FILTER_SOCKET_S)
  {
    /* To open the a image here seems not so straight forward.
     * Still the plug-in should be prepared to initialise the image data before
     * normal processing occurs.
     */
    socket = oyFilterSocket_Copy( (oyFilterSocket_s*)requestor_plug, 0 );
    requestor_plug = 0;
  }

  node = oyFilterSocket_GetNode( socket );

  /* parse options */
  if(error <= 0)
  {
    oyOptions_s * opts = oyFilterNode_GetOptions( node ,0 );
    filename = oyOptions_FindString( opts, "filename", 0 );
    oyOptions_FindInt( opts, "icc_profile_flags", 0, &icc_profile_flags );
    oyOptions_Release( &opts );
  }
  
  image_in = oyImage_FromJPEG( filename, icc_profile_flags );

  if(!image_in)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m( filename ) );
    goto ojpgFilter_CmmRunClean;
  }

  /* add image to filter socket */
  if(error <= 0)
  {
    oyFilterSocket_SetData( socket, (oyStruct_s*)image_in );
  }

  /* update the job ticket */
  if(ticket)
    output_image = oyPixelAccess_GetOutputImage( ticket );

  if(ticket &&
     output_image &&
     oyImage_GetWidth( output_image ) == 0 &&
     oyImage_GetHeight( output_image ) == 0)
  {
    oyImage_SetCritical( output_image, oyImage_GetPixelLayout( image_in,
                                                               oyLAYOUT ),
                         0,0,
                         oyImage_GetWidth( image_in ),
                         oyImage_GetHeight( image_in ) );
  }

ojpgFilter_CmmRunClean:
  /* release Oyranos stuff */
  oyImage_Release( &image_in );
  oyImage_Release( &output_image );
  oyFilterNode_Release( &node );
  oyFilterSocket_Release( &socket );

  /* return an error to cause the graph to retry */
  return 1;
}


static char * oJPG_category = NULL;
const char * ojpgApi4UiGetText2Read  ( const char        * select,
                                       oyNAME_e            type,
                                       const char        * format )
{

  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "read";
    else if(type == oyNAME_NAME)
      return _("read");
    else if(type == oyNAME_DESCRIPTION)
      return _("Load Image File Object");
  } else if(strcmp(select,"help") == 0)
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Option \"filename\", a valid filename of a existing image");
    else if(type == oyNAME_DESCRIPTION)
      return _("The Option \"filename\" should contain a valid filename to read the image data from. If the file does not exist, a error will occure.");
  }
  else if(strcmp(select,"category") == 0)
  {
    if(!oJPG_category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Files"),_("Read"),0};
      int len =  strlen(i18n[0]) + strlen(i18n[1]) + strlen(format);

      oJPG_category = (char*)malloc( len + 64 );
      if(oJPG_category)
      {
        char * t;
        /* Create a translation for ojpg_api4_ui_cmm_loader::category. */
        sprintf( oJPG_category,"%s/%s %s", i18n[0], i18n[1], format );
        t = strstr(oJPG_category, format);
        if(t) t[0] = toupper(t[0]);
      } else
        ojpg_msg(oyMSG_WARN, (oyStruct_s *) 0, _DBG_FORMAT_ "\n " "Could not allocate enough memory.", _DBG_ARGS_);
    }

         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return oJPG_category;
    else
      return oJPG_category;
  }
  return 0;
}
/**
 *  This function implements oyCMMGetText_f.
 *
 */
const char * ojpgApi4UiGetTextRead   ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  oyCMMapiFilter_s * api = oyCMMui_GetParent( (oyCMMui_s *) context );
  oyPointer_s * backend_context = oyCMMapiFilter_GetBackendContext( api );
  const char * format = (const char*) oyPointer_GetPointer( backend_context );
  oyPointer_Release( &backend_context );
  api->release( (oyStruct_s**) &api );

  return ojpgApi4UiGetText2Read(select, type, format);
}
const char * ojpg_api4_ui_texts[] = {"name", "category", "help", NULL};

/* OY_oJPG_FILTER_REGISTRATION_READ -----------------------------------------*/

/* OY_oJPG_FILTER_REGISTRATION_WRITE ----------------------------------------*/

static char * oJPG_registration_7write = NULL;
/** @brief    ojpg oyCMMapi7_s implementation
 *
 *  a filter providing a CMM filter
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/08/25
 *  @since   2018/08/25 (Oyranos: 0.9.7)
 */
oyCMMapi_s * ojpgApi7CmmCreateWrite  ( const char        * format,
                                       const char        * ext )
{
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {0,9,7};
  static oyDATATYPE_e data_types[6] = {oyUINT8, oyUINT16, oyUINT32,
                                       oyFLOAT, oyDOUBLE, (oyDATATYPE_e)0};
  oyConnectorImaging_s * socket = oyConnectorImaging_New(0);
  static oyConnectorImaging_s * plugs[2] = {0,0},
                              * sockets[2] = {0,0};
  
  const char * properties[] =
  {
    "file=write",   /* file read|write */
    "image=pixel",  /* image type, pixel/vector/font */
    "layers=1",     /* layer count, one for plain images */
    "icc=1",        /* image type ICC profile support */
    "ext=jpg,jpeg", /* supported extensions */
    0
  };

  sockets[0] = socket;

  oyStringAddPrintf( &oJPG_registration_7write, AD,
                     OY_oJPG_FILTER_REGISTRATION_BASE"file_write.write_%s._%s._CPU._ACCEL", format, CMM_NICK );

  if(oy_debug >= 2) ojpg_msg(oyMSG_DBG, NULL, _DBG_FORMAT_ "registration:%s ojpg %s", _DBG_ARGS_,
                             oJPG_registration_7write,
                             ext );


  oyConnectorImaging_SetDataTypes( socket, data_types, 5 );
  oyConnectorImaging_SetReg( socket, "//" OY_TYPE_STD "/image.data" );
  oyConnectorImaging_SetMatch( socket, oyFilterSocket_MatchImagingPlug );
  oyConnectorImaging_SetTexts( socket, oyCMMgetImageConnectorSocketText,
                               oy_image_connector_texts );
  oyConnectorImaging_SetIsPlug( socket, 0 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET, -1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT, 4 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT, 4 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_ID, 1 );

  oyCMMapi7_s * cmm7 = oyCMMapi7_Create( ojpgCMMInit, ojpgCMMReset, ojpgCMMMessageFuncSet,
                                       oJPG_registration_7write,
                                       cmm_version, module_api,
                                       NULL,
                                       ojpgFilter_CmmRunWrite,
                                       (oyConnector_s**)plugs, 0, 0,
                                       (oyConnector_s**)sockets, 1, 0,
                                       properties, 0 );
  oyFree_m_(oJPG_registration_7write);
  return (oyCMMapi_s*) cmm7;
}

const char ojpg_write_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "file_read" ">\n\
      <filename></filename>\n\
      <quality>97</quality>\n\
     </" "file_read" ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

static char * oJPG_registration_4write = NULL;
/** @brief    ojpg oyCMMapi4_s implementation
 *
 *  a filter providing a CMM device link creator
 *
 *  @version Oyranos: 0.9.7
 *  @since   2018/08/25 (Oyranos: 0.9.7)
 *  @date    2018/08/25
 */
oyCMMapi_s * ojpgApi4CmmCreateWrite  ( const char        * format )
{
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {0,9,7};
  oyPointer_s * backend_context = oyPointer_New(0);
  const char * category = ojpgApi4UiGetText2Write("category", oyNAME_NAME, format);
  oyCMMuiGet_f getOFORMS = ojpgGetOFORMS;
  oyCMMui_s * ui = oyCMMui_Create( category, ojpgApi4UiGetTextWrite,
                                   ojpg_api4_ui_texts, 0 );
  oyOptions_s * oy_opts = NULL;
  const char * oforms_options = ojpg_write_extra_options;

  oyCMMui_SetUiOptions( ui, oforms_options, getOFORMS ); 

  oyPointer_Set( backend_context, NULL, "ojpg_file_format", oyStringCopy(format, oyAllocateFunc_),
                 "char*", deAllocData );

  oyStringAddPrintf( &oJPG_registration_4write, AD,
                     OY_oJPG_FILTER_REGISTRATION_BASE"file_write.write_%s._" CMM_NICK "._CPU._ACCEL", format );

  oyCMMapi4_s * cmm4 = oyCMMapi4_Create( ojpgCMMInit, ojpgCMMReset, ojpgCMMMessageFuncSet,
                                       oJPG_registration_4write,
                                       cmm_version, module_api,
                                       "",
                                       NULL,
                                       ojpgFilterNode_GetText,
                                       ui,
                                       NULL );
  oyFree_m_(oJPG_registration_4write);

  oyCMMapi4_SetBackendContext( cmm4, backend_context );
  oyPointer_Release( &backend_context );
  oyOptions_Release( &oy_opts );

  return (oyCMMapi_s*)cmm4;
}

/* Start - added from Marti Marias little cms library  */

/*
 * Since an ICC profile can be larger than the maximum size of a JPEG marker
 * (64K), we need provisions to split it into multiple markers.  The format
 * defined by the ICC specifies one or more APP2 markers containing the
 * following data:
 *	Identifying string	ASCII "ICC_PROFILE\0"  (12 bytes)
 *	Marker sequence number	1 for first APP2, 2 for next, etc (1 byte)
 *	Number of markers	Total number of APP2's used (1 byte)
 *      Profile data		(remainder of APP2 data)
 * Decoders should use the marker sequence numbers to reassemble the profile,
 * rather than assuming that the APP2 markers appear in the correct sequence.
 */

#define ICC_MARKER  (JPEG_APP0 + 2)	/* JPEG marker code for ICC */
#define ICC_OVERHEAD_LEN  14		/* size of non-profile data in APP2 */
#define MAX_BYTES_IN_MARKER  65533	/* maximum data len of a JPEG marker */
#define MAX_DATA_BYTES_IN_MARKER  (MAX_BYTES_IN_MARKER - ICC_OVERHEAD_LEN)


/*
 * This routine writes the given ICC profile data into a JPEG file.
 * It *must* be called AFTER calling jpeg_start_compress() and BEFORE
 * the first call to jpeg_write_scanlines().
 * (This ordering ensures that the APP2 marker(s) will appear after the
 * SOI and JFIF or Adobe markers, but before all else.)
 */

void
write_icc_profile (j_compress_ptr cinfo,
		   const JOCTET *icc_data_ptr,
		   unsigned int icc_data_len)
{
  unsigned int num_markers;	/* total number of markers we'll write */
  int cur_marker = 1;		/* per spec, counting starts at 1 */
  unsigned int length;		/* number of bytes to write in this marker */

  /* Calculate the number of markers we'll need, rounding up of course */
  num_markers = icc_data_len / MAX_DATA_BYTES_IN_MARKER;
  if (num_markers * MAX_DATA_BYTES_IN_MARKER != icc_data_len)
    num_markers++;

  while (icc_data_len > 0) {
    /* length of profile to put in this marker */
    length = icc_data_len;
    if (length > MAX_DATA_BYTES_IN_MARKER)
      length = MAX_DATA_BYTES_IN_MARKER;
    icc_data_len -= length;

    /* Write the JPEG marker header (APP2 code and marker length) */
    jpeg_write_m_header(cinfo, ICC_MARKER,
			(unsigned int) (length + ICC_OVERHEAD_LEN));

    /* Write the marker identifying string "ICC_PROFILE" (null-terminated).
     * We code it in this less-than-transparent way so that the code works
     * even if the local character set is not ASCII.
     */
    jpeg_write_m_byte(cinfo, 0x49);
    jpeg_write_m_byte(cinfo, 0x43);
    jpeg_write_m_byte(cinfo, 0x43);
    jpeg_write_m_byte(cinfo, 0x5F);
    jpeg_write_m_byte(cinfo, 0x50);
    jpeg_write_m_byte(cinfo, 0x52);
    jpeg_write_m_byte(cinfo, 0x4F);
    jpeg_write_m_byte(cinfo, 0x46);
    jpeg_write_m_byte(cinfo, 0x49);
    jpeg_write_m_byte(cinfo, 0x4C);
    jpeg_write_m_byte(cinfo, 0x45);
    jpeg_write_m_byte(cinfo, 0x0);

    /* Add the sequencing info */
    jpeg_write_m_byte(cinfo, cur_marker);
    jpeg_write_m_byte(cinfo, (int) num_markers);

    /* Add the profile data */
    while (length--) {
      jpeg_write_m_byte(cinfo, *icc_data_ptr);
      icc_data_ptr++;
    }
    cur_marker++;
  }
}
/* End - added from Marti Marias little cms library  */

int          oyImage_WriteJPEG       ( oyImage_s         * image,
                                       const char        * filename,
                                       oyOptions_s       * options )
{
  int error = 0;
  int32_t quality = -1;
  int y;
  uint8_t * cmyk = NULL;

  int image_height = oyImage_GetHeight( image );
  int image_width = oyImage_GetWidth( image );
  int pixel_layout = oyImage_GetPixelLayout( image, oyLAYOUT );
  oyProfile_s * prof = oyImage_GetProfile( image );
  //const char * colorspacename = oyProfile_GetText( prof, oyNAME_DESCRIPTION );
  icColorSpaceSignature sig = oyProfile_GetSignature( prof,
                                                      oySIGNATURE_COLOR_SPACE);
  int channels_n = oyToChannels_m( pixel_layout );
  //int cchan_n = oyProfile_GetChannelsCount( prof );
  oyDATATYPE_e data_type = oyToDataType_m( pixel_layout );
  int stride = image_width * channels_n;
  char * comment = (char*) oyOptions_FindString( options, "comment", 0 );;

  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;		/* target file */
  int jcs; /* jpeg color space */

  if(!image)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)image,
             OY_DBG_FORMAT_ " not image obtained for %s",
             OY_DBG_ARGS_, oyNoEmptyString_m( filename ) );
    return 1;
  }

  if(!prof)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)image,
             OY_DBG_FORMAT_ " not profile found for: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m( filename ) );
    return -1;
  }

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
  if ((outfile = fopen(filename, "wb")) == NULL)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)image,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m( filename ) );
    return 1;
  }

  oyOptions_FindInt( options, "quality", 0, &quality );
  if(quality <= 0)
    quality = 97;

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = image_width; 	/* image width and height, in pixels */
  cinfo.image_height = image_height;
  cinfo.input_components = channels_n;		/* # of color components per pixel */
  switch(sig)
  {
    case icSigGrayData:
         jcs = JCS_GRAYSCALE;
         break;
    case icSigRgbData:
         jcs = JCS_RGB;
         break;
    case icSigCmykData:
         jcs = JCS_CMYK;
         break;
    case icSigYCbCrData:
         jcs = JCS_YCbCr;
         break;
    default: jcs = JCS_RGB;
  }
  cinfo.in_color_space = jcs; 	/* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* 4b: Write ICC profile */
  {
    void * pmem = 0;
    size_t psize = 0;
    pmem = oyProfile_GetMem( prof, &psize, 0,0 );
    write_icc_profile( &cinfo, (JOCTET*)pmem, psize );
    /*jpeg_write_marker_APP( &cinfo, JPEG_APP0+2, (JOCTET*)"ICC_PROFILE", 12,
                           (JOCTET*)pmem, psize ); -- appears not to work */
    if(oy_debug)
      ojpg_msg( oyMSG_DBG, (oyStruct_s*)image, OY_DBG_FORMAT_ "embedd profile: %lu", OY_DBG_ARGS_, psize);
    oyFree_m_( pmem );
  }

  if(comment)
    jpeg_write_marker(&cinfo, JPEG_COM, (const JOCTET *)comment, strlen(comment));

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  if( jcs == JCS_CMYK && 
      data_type == oyUINT8 )
    cmyk = (uint8_t*) malloc( stride );

  for(y = 0; y < image_height; ++y)
  {
    int is_allocated = 0;
    int height = 0;
    void * p = oyImage_GetLineF(image)( image, y, &height, -1, &is_allocated );
    JSAMPROW row_pointer[2] = {p,0};	/* Points to large array of R,G,B-order data */

    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    if( jcs == JCS_CMYK && 
        data_type == oyUINT8 )
    {
      int i;
      memcpy(cmyk, p, stride);
#pragma omp parallel for private(i)
      for(i = 0; i < stride; ++i)
        cmyk[i] = 255 - cmyk[i];

      row_pointer[0] = cmyk;
      (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);

    } else
      (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  if(cmyk) {free(cmyk);} cmyk = NULL;

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);
  outfile = NULL;

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */

  oyProfile_Release( &prof );

  return error;
}


/** Function ojpgFilter_CmmRunWrite
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  The primary filter entry for data processing.
 *
 *  @param         requestor_plug      the plug of the requesting node after 
 *                                     my filter in the graph
 *  @param         ticket              the job ticket
 *
 *  @version Oyranos: 0.9.7
 *  @since   2018/08/25 (Oyranos: 0.9.7)
 *  @date    2018/08/25
 */
int      ojpgFilter_CmmRunWrite      ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket;
  oyFilterNode_s * node = 0;
  oyOptions_s * node_opts = 0;
  int result = 0;
  const char * filename = 0;
  FILE * fp = 0;

  socket = oyFilterPlug_GetSocket( requestor_plug );
  node = oyFilterSocket_GetNode( socket );
  node_opts = oyFilterNode_GetOptions( node, 0 );

  /* to reuse the requestor_plug is a exception for the starting request */
  if(node)
    result = oyFilterNode_Run( node, requestor_plug, ticket );
  else
    result = 1;

  if(result <= 0)
    filename = oyOptions_FindString( node_opts, "filename", 0 );

  if(filename)
    fp = fopen( filename, "wb" );

  if(fp)
  {
    oyImage_s *image_output = (oyImage_s*)oyFilterSocket_GetData( socket );

    fclose (fp); fp = 0;

    result = oyImage_WriteJPEG( image_output, filename,
                                node_opts );

    oyImage_Release( &image_output );
  }

  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );
  oyOptions_Release( &node_opts );

  return result;
}


const char * ojpgApi4UiGetText2Write ( const char        * select,
                                       oyNAME_e            type,
                                       const char        * format )
{

  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "read";
    else if(type == oyNAME_NAME)
      return _("read");
    else if(type == oyNAME_DESCRIPTION)
      return _("Load Image File Object");
  } else if(strcmp(select,"help") == 0)
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Option \"filename\", a valid filename of a existing image");
    else if(type == oyNAME_DESCRIPTION)
      return _("The Option \"filename\" should contain a valid filename to read the image data from. If the file does not exist, a error will occure.");
  }
  else if(strcmp(select,"category") == 0)
  {
    if(!oJPG_category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Files"),_("Read"),0};
      int len =  strlen(i18n[0]) + strlen(i18n[1]) + strlen(format);

      oJPG_category = (char*)malloc( len + 64 );
      if(oJPG_category)
      {
        char * t;
        /* Create a translation for ojpg_api4_ui_cmm_loader::category. */
        sprintf( oJPG_category,"%s/%s %s", i18n[0], i18n[1], format );
        t = strstr(oJPG_category, format);
        if(t) t[0] = toupper(t[0]);
      } else
        ojpg_msg(oyMSG_WARN, (oyStruct_s *) 0, _DBG_FORMAT_ "\n " "Could not allocate enough memory.", _DBG_ARGS_);
    }

         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return oJPG_category;
    else
      return oJPG_category;
  }
  return 0;
}
/**
 *  This function implements oyCMMGetText_f.
 *
 */
const char * ojpgApi4UiGetTextWrite  ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  oyCMMapiFilter_s * api = oyCMMui_GetParent( (oyCMMui_s *) context );
  oyPointer_s * backend_context = oyCMMapiFilter_GetBackendContext( api );
  const char * format = (const char*) oyPointer_GetPointer( backend_context );
  oyPointer_Release( &backend_context );
  api->release( (oyStruct_s**) &api );

  return ojpgApi4UiGetText2Write(select, type, format);
}
/* OY_oJPG_FILTER_REGISTRATION_WRITE ----------------------------------------*/

extern oyCMM_s oJPG_cmm_module;
static int ojpg_initialised = 0;
int  ojpgInit                        ( oyStruct_s        * module_info )
{
  oyCMMapi_s * a = 0,
             * a_tmp = 0,
             * m = 0;
  int i,n = 2;

  if(ojpg_initialised)
    return 0;
  ++ojpg_initialised;

  if((oyStruct_s*)&oJPG_cmm_module != module_info)
    ojpg_msg( oyMSG_WARN, module_info, _DBG_FORMAT_ "wrong module info passed in", _DBG_ARGS_ );

  /* search the last filter */
  a = oJPG_cmm_module.api;
  while(a && ((a_tmp = oyCMMapi_GetNext( a )) != 0))
    a = a_tmp;

  /* append new items */
  for( i = 0; i < n; ++i)
  {
    const char * format = "jpeg";

    if(i == 0)
      m = ojpgApi4CmmCreateRead( format );
    else
      m = ojpgApi4CmmCreateWrite( format );
    if(!a)
    {
      oJPG_cmm_module.api = m;
      a = m;
    }
    else
      if(a && m)
      {
        oyCMMapi_SetNext( a, m ); a = m;
      }
  }
  for( i = 0; i < n; ++i)
  {
    const char * format = "jpeg";

    if(i == 0)
      m = ojpgApi7CmmCreateRead( format, "jpeg,jpg" );
    else
      m = ojpgApi7CmmCreateWrite( format, "jpeg,jpg" );
    if(!oJPG_cmm_module.api)
    {
      oJPG_cmm_module.api = m;
      a = m;
    }
    else
      if(a && m)
      {
        oyCMMapi_SetNext( a, m ); a = m;
      }
  }

  return 0;
}

int  ojpgReset                       ( oyStruct_s        * module_info )
{
  if(!ojpg_initialised)
    return 0;

  ojpg_initialised = 0;
  if(oy_debug)
    ojpg_msg( oyMSG_DBG, module_info, _DBG_FORMAT_, _DBG_ARGS_ );

  if((oyStruct_s*)&oJPG_cmm_module != module_info)
    ojpg_msg( oyMSG_WARN, module_info, _DBG_FORMAT_ "wrong module info passed in", _DBG_ARGS_ );

  if(oJPG_category)
    free(oJPG_category);
  oJPG_category = NULL;

  return 0;
}
