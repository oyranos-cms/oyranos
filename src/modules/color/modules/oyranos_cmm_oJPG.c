/** @file oyranos_cmm_oJPG.c
 *
 *  JPEG file i/o module for Oyranos 
 *
 *  @par Copyright:
 *            2014-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    JPEG filter for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <oy@oyranos.org>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2014/03/21
 */

#include "oyCMM_s.h"
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

#include "jpegmarkers.h"

/* --- internal definitions --- */

/** The CMM_NICK consists of four bytes, which appear as well in the library name. This is important for Oyranos to identify the required filter struct name. */
#define CMM_NICK "oJPG"
#define OY_oJPG_FILTER_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "file_loader"

/** The message function pointer to use for messaging. */
oyMessage_f ojpg_msg = oyMessageFunc;

/* Helpers */
#if defined(__GNUC__)
# define  OY_DBG_FORMAT_ "%s:%d %s() "
# define  OY_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__,__func__
#else
# define  OY_DBG_FORMAT_ "%s:%d "
# define  OY_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
#endif
#define _DBG_FORMAT_ OY_DBG_FORMAT_
#define _DBG_ARGS_ OY_DBG_ARGS_

/* i18n */
#include "oyranos_i18n.h"

#define AD oyAllocateFunc_, oyDeAllocateFunc_

int  ojpgInit                        ( oyStruct_s        * module_info );
int      ojpgFilter_CmmRun           ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
const char * ojpgApi4UiGetText2      ( const char        * select,
                                       oyNAME_e            type,
                                       const char        * format );
int                ojpgGetOFORMS     ( oyCMMapiFilter_s  * module,
                                       oyOptions_s       * oy_opts,
                                       char             ** ui_text,
                                       oyAlloc_f           allocateFunc );
const char * ojpgApi4UiGetText       ( const char        * select,
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
int                ojpgCMMInit       ( oyStruct_s * s )
{
  int error = 0;
  return error;
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

/** @instance oJPG_cmm_module
 *  @brief    ojpg module infos
 *
 *  This structure is dlopened by Oyranos. Its name has to consist of the
 *  following elements:
 *  - the four byte CMM_NICK plus
 *  - "_cmm_module"
 *  This string must be included in the the filters filename.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2014/03/21
 */
oyCMM_s oJPG_cmm_module = {

  oyOBJECT_CMM_INFO_S, /**< ::type; the object type */
  0,0,0,               /**< static objects omit these fields */
  CMM_NICK,            /**< ::cmm; the four char filter id */
  (char*)"0.9.6",      /**< ::backend_version */
  ojpgGetText,         /**< ::getText; UI texts */
  (char**)oyCMM_texts, /**< ::texts; list of arguments to getText */
  OYRANOS_VERSION,     /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
  NULL,

  /** ::icon; module icon */
  &ojpg_icon,
  ojpgInit
};


/* OY_oJPG_FILTER_REGISTRATION ----------------------------------------------*/

#define OY_oJPG_FILTER_REGISTRATION_BASE OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH

/** @instance ojpg_api7
 *  @brief    ojpg oyCMMapi7_s implementation
 *
 *  a filter providing a CMM filter
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/03/21
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 */
oyCMMapi_s * ojpgApi7CmmCreate       ( const char        * format,
                                       const char        * ext )
{
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {0,9,6};
  static oyDATATYPE_e data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                       oyHALF, oyFLOAT, oyDOUBLE, (oyDATATYPE_e)0};
  oyConnectorImaging_s * plug = oyConnectorImaging_New(0),
                       * socket = oyConnectorImaging_New(0);
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

  plugs[0] = plug;
  sockets[0] = socket;
  char * registration = NULL;

  oyStringAddPrintf( &registration, AD,
                     OY_oJPG_FILTER_REGISTRATION_BASE"file_read.input_%s._%s._CPU._ACCEL", format, CMM_NICK );

  if(oy_debug >= 2) ojpg_msg(oyMSG_DBG, NULL, _DBG_FORMAT_ "registration:%s ojpg %s", _DBG_ARGS_,
                             registration,
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

  oyCMMapi7_s * cmm7 = oyCMMapi7_Create( ojpgCMMInit, ojpgCMMMessageFuncSet,
                                       registration,
                                       cmm_version, module_api,
                                       NULL,
                                       ojpgFilter_CmmRun,
                                       (oyConnector_s**)plugs, 0, 0,
                                       (oyConnector_s**)sockets, 1, 0,
                                       properties, 0 );
  //oyFree_m_( registration );
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

/** @instance ojpg_api4
 *  @brief    ojpg oyCMMapi4_s implementation
 *
 *  a filter providing a CMM device link creator
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/03/21 (Oyranos: 0.9.6)
 *  @date    2014/03/21
 */
oyCMMapi_s * ojpgApi4CmmCreate       ( const char        * format )
{
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {0,9,6};
  oyPointer_s * backend_context = oyPointer_New(0);
  char * registration = NULL;
  const char * category = ojpgApi4UiGetText2("category", oyNAME_NAME, format);
  oyCMMuiGet_f getOFORMS = ojpgGetOFORMS;
  oyCMMui_s * ui = oyCMMui_Create( category, ojpgApi4UiGetText,
                                   ojpg_api4_ui_texts, 0 );
  oyOptions_s * oy_opts = NULL;
  const char * oforms_options = ojpg_read_extra_options;

  oyCMMui_SetUiOptions( ui, oyStringCopy( oforms_options, oyAllocateFunc_ ), getOFORMS ); 

  oyPointer_Set( backend_context, NULL, "ojpg_file_format", oyStringCopy(format, oyAllocateFunc_),
                 "char*", deAllocData );

  oyStringAddPrintf( &registration, AD,
                     OY_oJPG_FILTER_REGISTRATION_BASE"file_read.input_%s._" CMM_NICK "._CPU._ACCEL", format );

  oyCMMapi4_s * cmm4 = oyCMMapi4_Create( ojpgCMMInit, ojpgCMMMessageFuncSet,
                                       registration,
                                       cmm_version, module_api,
                                       "",
                                       NULL,
                                       ojpgFilterNode_GetText,
                                       ui,
                                       NULL );

  oyCMMapi4_SetBackendContext( cmm4, backend_context );
  oyOptions_Release( &oy_opts );

  return (oyCMMapi_s*)cmm4;
}


char * ojpgFilterNode_GetText        ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
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
int                ojpgGetOFORMS     ( oyCMMapiFilter_s  * module,
                                       oyOptions_s       * oy_opts,
                                       char             ** ui_text,
                                       oyAlloc_f           allocateFunc )
{
  int error = 0;
  char * tmp = NULL;

  *ui_text = tmp;
  
  return error;
}


oyOptions_s* ojpgFilter_CmmLoaderValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

#if 0
  if(!error)
    error = !oyOptions_FindString( validate, "my_options", 0 );
#endif

  *result = error;

  return 0;
}


oyProfile_s * profileFromMatrix( double pandg[9], const char * name, int32_t icc_profile_flags  )
{
  oyProfile_s * p = oyProfile_FromName(name, icc_profile_flags, NULL);

  if(!p)
  {
            oyOption_s * primaries = oyOption_FromRegistration( "//" 
                    OY_TYPE_STD 
                    "/color_matrix."
                    "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                    0);
            oyOptions_s * opts = oyOptions_New(0),
                        * result = 0;

            int pos = 0;


            oyOptions_SetFromInt( &opts, "///icc_profile_flags", icc_profile_flags,
                                  0, OY_CREATE_NEW ); 

            /* red */
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            /* green */
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            /* blue */
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            /* white */
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
            /* gamma */
            oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;

            oyOptions_MoveIn( opts, &primaries, -1 );
            oyOptions_Handle( "//"OY_TYPE_STD"/create_profile.icc",
                                opts,"create_profile.icc_profile.color_matrix",
                                &result );
            p = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                        oyOBJECT_PROFILE_S );
            oyProfile_AddTagText( p, icSigProfileDescriptionTag, name);
            oyProfile_AddTagText( p, icSigCopyrightTag, "ICC License 2011");
            oyOptions_Release( &result );
            oyOptions_Release( &opts );

            oyProfile_Install( p, oySCOPE_USER, NULL );
  }

  return p;
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
             profile_name = strdup("YCC profile - supports extended sRGB range PRELIMINARY 1-4-2002.icc");
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
           profile_name = strdup("YCC profile - supports extended sRGB range PRELIMINARY 1-4-2002.icc");
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

/** Function ojpgFilter_CmmRun
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
int      ojpgFilter_CmmRun           ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = 0;
  oyStruct_s * socket_data = 0;
  oyFilterNode_s * node = 0;
  oyOptions_s * tags = 0;
  int error = 0;
  const char * filename = 0;
  FILE * fp = 0;
  oyDATATYPE_e data_type = oyUINT8;
  oyPROFILE_e profile_type = oyASSUMED_RGB;
  oyProfile_s * prof = 0;
  oyImage_s * image_in = 0,
            * output_image = 0;
  oyPixel_t pixel_type = 0;
  size_t  fsize = 0;
  uint8_t * buf = 0;
  size_t  mem_n = 0;   /* needed memory in bytes */
  int width,height,nchannels;
  int32_t icc_profile_flags = 0;
  const char * format = "jpeg";

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

    return error;

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

  /* file tests */
  if(filename)
    fp = fopen( filename, "rm" );

  if(!fp)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m( filename ) );
    return 1;
  }

  /* file size fun */
  fseek(fp,0L,SEEK_END);
  fsize = ftell(fp);
  rewind(fp);
  if(oy_debug)
    ojpg_msg( oyMSG_DBG, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "file size %u",
             OY_DBG_ARGS_, fsize );


  pixel_type = oyChannels_m(3) | oyDataType_m(data_type); 


  /* get ICC Profile */
  {
    struct jpeg_decompress_struct cinfo; 
    struct jpeg_error_mgr jerr;
    unsigned int len = 0;
    unsigned char * icc = NULL;
    int m;

    // Setup decompression structure
    cinfo.err = jpeg_std_error(&jerr); 
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
      ojpg_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_ "jpeg embedded profile found: %d", OY_DBG_ARGS_, len);
    } else if (select_icc_profile(&cinfo, filename, &icc, &len))
    { if(oy_debug)
      ojpg_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_ "jpeg default profile selected: %d", OY_DBG_ARGS_, len);
    } else
      if(oy_debug)
      ojpg_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_ "jpeg no profile found", OY_DBG_ARGS_);

    if(icc && len)
    {
      prof = oyProfile_FromMem( len, icc, 0, 0 );
      free(icc); icc = NULL;
      len = 0;
    }

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
        ojpg_msg(oyMSG_WARN, (oyStruct_s *) node, _DBG_FORMAT_ "Could not allocate enough memory.", _DBG_ARGS_);
        return 1;
      }
    }
    if(oy_debug)
    ojpg_msg( oyMSG_DBG, (oyStruct_s *) node, _DBG_FORMAT_ "allocate image data: 0x%x size: %d ", _DBG_ARGS_, (int)(intptr_t)
              buf, mem_n );


    while (cinfo.output_scanline < height) {
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
    ojpg_msg( oyMSG_DBG, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "%dx%d %s|%s[%d]",
             OY_DBG_ARGS_,  width, height,
             format, oyDataTypeToText(data_type), nchannels );

  /* create a Oyranos image */
  image_in = oyImage_Create( width, height, buf, pixel_type, prof, 0 );

  if (!image_in)
  {
    ojpg_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "can't create a new image\n%dx%d %s[%d]",
             OY_DBG_ARGS_,  width, height, format, nchannels );
    return FALSE;
  }

  /* remember the meta data like file name */
  tags = oyImage_GetTags( image_in );
  error = oyOptions_SetFromText( &tags,
                                 "//" OY_TYPE_STD "/file_read.input_ojpg"
                                                                    "/filename",
                                 filename, OY_CREATE_NEW );

  oyOptions_Release( &tags );

  /* close the image and FILE pointer */
  fclose(fp); fp = NULL;

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

  /* release Oyranos stuff */
  oyImage_Release( &image_in );
  oyImage_Release( &output_image );
  oyFilterNode_Release( &node );
  oyFilterSocket_Release( &socket );

  /* return an error to cause the graph to retry */
  return 1;
}


const char * ojpgApi4UiGetText2      ( const char        * select,
                                       oyNAME_e            type,
                                       const char        * format )
{
  char * category = 0;

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
    if(!category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Files"),_("Read"),0};
      int len =  strlen(i18n[0]) + strlen(i18n[1]) + strlen(format);

      category = (char*)malloc( len + 64 );
      if(category)
      {
        char * t;
        /* Create a translation for ojpg_api4_ui_cmm_loader::category. */
        sprintf( category,"%s/%s %s", i18n[0], i18n[1], format );
        t = strstr(category, format);
        if(t) t[0] = toupper(t[0]);
      } else
        ojpg_msg(oyMSG_WARN, (oyStruct_s *) 0, _DBG_FORMAT_ "\n " "Could not allocate enough memory.", _DBG_ARGS_);
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
/**
 *  This function implements oyCMMGetText_f.
 *
 */
const char * ojpgApi4UiGetText       ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  oyCMMapiFilter_s * api = oyCMMui_GetParent( (oyCMMui_s *) context );
  oyPointer_s * backend_context = oyCMMapiFilter_GetBackendContext( api );
  const char * format = (const char*) oyPointer_GetPointer( backend_context );
  oyPointer_Release( &backend_context );
  api->release( (oyStruct_s**) &api );

  return ojpgApi4UiGetText2(select, type, format);
}
const char * ojpg_api4_ui_texts[] = {"name", "category", "help", NULL};

/* OY_oJPG_FILTER_REGISTRATION ----------------------------------------------*/

extern oyCMM_s oJPG_cmm_module;
int  ojpgInit                        ( oyStruct_s        * module_info )
{
  oyCMMapi_s * a = 0,
             * a_tmp = 0,
             * m = 0;
  int i,n = 1;

  if((oyStruct_s*)&oJPG_cmm_module != module_info)
    ojpg_msg( oyMSG_WARN, module_info, _DBG_FORMAT_ "wrong module info passed in", _DBG_ARGS_ );

  /* search the last filter */
  a = oJPG_cmm_module.api;
  while(a && ((a_tmp = oyCMMapi_GetNext( a )) != 0))
    a = a_tmp;

  /* append new items */
  {
    const char * format = "jpeg";

    m = ojpgApi4CmmCreate( format );
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

    m = ojpgApi7CmmCreate( format, "jpeg,jpg" );
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
