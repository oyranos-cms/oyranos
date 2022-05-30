/** @file oyranos_cmm_oPNG.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    PNG module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/09/06
 */

#include "oyranos_config_internal.h"

#include "oyCMM_s.h"
#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi7_s.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMui_s_.h"
#include "oyConnectorImaging_s_.h"
#include "oyFilterNode_s_.h"         /* for oyFilterNode_TextToInfo_ */

#include "oyranos_cmm.h"
#include "oyranos_generic.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_icc.h"
#include "oyranos_image_internal.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include LIBPNG_INCLUDE
#define LIBPNG_VERSION_NUM PNG_LIBPNG_VER_MAJOR * 10000 + PNG_LIBPNG_VER_MINOR *100 + PNG_LIBPNG_VER_RELEASE

#define CMM_NICK "oPNG"
#define CMM_VERSION {0,1,0}

oyMessage_f oPNG_msg = oyMessageFunc;

extern oyCMMapi4_s_   oPNG_api4_image_write_png;
extern oyCMMapi7_s_   oPNG_api7_image_write_png;
extern oyCMMapi4_s_   oPNG_api4_image_input_png;
extern oyCMMapi7_s_   oPNG_api7_image_input_png;

/* OY_INPUT_PNG_REGISTRATION */
/* OY_WRITE_PNG_REGISTRATION */


oyWIDGET_EVENT_e   oPNGWidgetEvent   ( oyOptions_s       * options OY_UNUSED,
                                       oyWIDGET_EVENT_e    type OY_UNUSED,
                                       oyStruct_s        * event OY_UNUSED )
{return 0;}

int                oPNGCMMInit       ( oyStruct_s        * filter OY_UNUSED )
{ int error = 0; return error; }
int                oPNGCMMReset      ( oyStruct_s        * filter OY_UNUSED )
{ int error = 0; return error; }

/** Function oPNGCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2010/09/06
 */
int            oPNGCMMMessageFuncSet ( oyMessage_f         message_func )
{
  oPNG_msg = message_func;
  return 0;
}


/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/06 (Oyranos: 0.1.11)
 *  @date    2010/09/06
 */
const char * oPNGGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select, "name")==0)
    if(type == oyNAME_NICK)
      return CMM_NICK;

  return oyCMMgetText( select, type, context );
}

oyIcon_s oPNG_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @brief    oPNG module infos
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
oyCMM_s oPNG_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.1.11",
  oPNGGetText,        /**< getText */
  (char**)oyCMM_texts, /**< texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oPNG_api4_image_write_png,

  &oPNG_icon,

  NULL,                                /**< init() */
  NULL                                 /**< reset() */
};

static int oPNG_error = 0;

void oPNGerror( png_structp png OY_UNUSED, const char * text )
{
  oPNG_error = 1;
  oPNG_msg( oyMSG_ERROR, (oyStruct_s*)NULL/*node*/,
             OY_DBG_FORMAT_ "%s",
             OY_DBG_ARGS_, text );
}
void oPNGwarn( png_structp png OY_UNUSED, const char * text )
{
  oPNG_msg( oyMSG_WARN, (oyStruct_s*)NULL/*node*/,
             OY_DBG_FORMAT_ "%s",
             OY_DBG_ARGS_, text );
}


/* OY_WRITE_PNG_REGISTRATION ---------------------------------------------*/


oyOptions_s* oPNGFilter_ImageOutputPNGValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

#if 0
  oyDATATYPE_e data_type = 0;
  int planar, channels;
  oyImage_s * image = 0;

  if(!error)
    filter = node->filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_S;

  if(!error)
  {
    if(filter->image_ && filter->image_->layout_)
    {
      data_type = oyToDataType_m( filter->image_->layout_[0] );
      if(!(data_type == oyUINT8 ||
           data_type == oyUINT16 ||
           data_type == oyFLOAT ||
           data_type == oyDOUBLE
                                   ))
        error = 1;

      planar = oyToPlanar_m( filter->image_->layout_[0] );
      if(!error && planar)
        error = 1;

      channels = oyToChannels_m( filter->image_->layout_[0] );
      if(!error && channels > 4)
        error = 1;
    }
  }
#endif

  if(!error)
    error = !oyOptions_FindString( validate, "filename", 0 );

  *result = error;

  return 0;
}

int png_jumpbuf_set ( png_structp png_ptr )
{
  if( setjmp( png_jmpbuf( png_ptr) ) )
    return 1;
  return 0;
}

/* the more heavily commented parts are from libpng/example.c */
int  oyImage_WritePNG                ( oyImage_s         * image,
                                       const char        * file_name,
                                       oyOptions_s       * options )
{
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  int y;

  int width = oyImage_GetWidth( image );
  int height = oyImage_GetHeight( image );
  int pixel_layout = oyImage_GetPixelLayout( image, oyLAYOUT );
  oyProfile_s * prof = oyImage_GetProfile( image );
  const char * colorspacename = oyProfile_GetText( prof,
                                                    oyNAME_DESCRIPTION );
#if LIBPNG_VERSION_NUM >= 10501
  png_bytep
#else
  png_charp
#endif
         pmem = 0;
  size_t psize = 0;
  icColorSpaceSignature sig = oyProfile_GetSignature( prof,
                                                      oySIGNATURE_COLOR_SPACE);
  int cchan_n = oyProfile_GetChannelsCount( prof );
  int channels_n = oyToChannels_m( pixel_layout );
  oyDATATYPE_e data_type = oyToDataType_m( pixel_layout );
  int alpha = channels_n - cchan_n;
  int color = PNG_COLOR_TYPE_GRAY;
  int byteps = oyDataTypeGetSize( data_type );
  png_text text_ptr[2];
  time_t ttime;
  png_time png_time_data;

  if(cchan_n > 3)
  {
    oPNG_msg( oyMSG_WARN, image,
             OY_DBG_FORMAT_ "Not supported. Color space needs more than 3 color planes: c%d. But oPNG supoprts only 1 or 3 color planes plus eventually one alpha channel.",
             OY_DBG_ARGS_, cchan_n );
    return (1);
  }

   /* Open the file */
   fp = fopen(file_name, "wb");
   if (fp == NULL)
      return (1);

  /* set ICC profile */
  pmem = oyProfile_GetMem( prof, &psize, 0,0 );
  if(!pmem)
  {
    oPNG_msg( oyMSG_WARN, image,
             OY_DBG_FORMAT_ "not profile available for %s",
             OY_DBG_ARGS_, file_name );
    if(fp) fclose(fp);
    return (1);
  }

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      (png_voidp) file_name, oPNGerror, oPNGwarn);

   if (png_ptr == NULL)
   {
      fclose(fp);
      return (1);
   }

   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      fclose(fp);
      png_destroy_write_struct(&png_ptr,  NULL);
      return (1);
   }

   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error handling functions in the png_create_write_struct() call.
    */
   if(png_jumpbuf_set(png_ptr))
   {
      /* If we get here, we had a problem writing the file */
      fclose(fp);
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return (1);
   }

  if(sig != icSigGrayData)
    color = PNG_COLOR_MASK_COLOR;

  if((channels_n == 2 && alpha == 0) ||
      channels_n == 4 )
    color |= PNG_COLOR_MASK_ALPHA;

   /* One of the following I/O initialization functions is REQUIRED */

   /* Set up the output control if you are using standard C streams */
   png_init_io(png_ptr, fp);

   /* This is the hard way */

   /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */
   png_set_IHDR(png_ptr, info_ptr, width, height, byteps*8, color,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_set_iCCP( png_ptr, info_ptr, (char*)colorspacename, 0,
                pmem, psize);
  if(pmem) {oyDeAllocateFunc_( pmem );} pmem = 0;

  /* set time stamp */
  ttime= time(NULL); /* time right NOW */
  png_convert_from_time_t( &png_time_data, ttime );
  png_set_tIME( png_ptr, info_ptr, &png_time_data );

  /* Optionally write comments into the image */
  if(oyOptions_FindString( options, "comment", 0 ))
  {
    const char * t = oyOptions_FindString( options, "comment", 0 );;
    text_ptr[0].key = "Description";
    text_ptr[0].text = (char*)t;
    text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
#ifdef PNG_iTXt_SUPPORTED
    text_ptr[0].lang = NULL;
    text_ptr[0].lang_key = NULL;
#endif
    t = oyVersionString(1);
    text_ptr[1].key = "Software";
    text_ptr[1].text = (char*)t;
    text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
#ifdef PNG_iTXt_SUPPORTED
    text_ptr[1].lang = NULL;
    text_ptr[1].lang_key = NULL;
#endif
    png_set_text(png_ptr, info_ptr, text_ptr, 2);
  }

   /* Other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs */

   /* Note that if sRGB is present the gAMA and cHRM chunks must be ignored
    * on read and, if your application chooses to write them, they must
    * be written in accordance with the sRGB profile
    */

   /* Write the file header information.  REQUIRED */
   png_write_info(png_ptr, info_ptr);

   /* If you want, you can write the info in two steps, in case you need to
    * write your private chunk ahead of PLTE:
    *
    *   png_write_info_before_PLTE(write_ptr, write_info_ptr);
    *   write_my_chunk();
    *   png_write_info(png_ptr, info_ptr);
    *
    * However, given the level of known- and unknown-chunk support in 1.2.0
    * and up, this should no longer be necessary.
    */

   /* Once we write out the header, the compression type on the text
    * chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
    * PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
    * at the end.
    */

   /* Set up the transformations you want.  Note that these are
    * all optional.  Only call them if you want them.
    */

   /* Pack pixels into bytes */
   png_set_packing(png_ptr);

   /* Swap bytes of 16-bit files to most significant byte first */
   if(byteps > 1 && !oyBigEndian())
     png_set_swap( png_ptr );

   /* One of the following output methods is REQUIRED */

      /* If you are only writing one row at a time, this works */
  for(y = 0; y < height; ++y)
  {
    int is_allocated = 0;
    int height = 0;
    void * p = oyImage_GetLineF(image)( image, y, &height, -1, &is_allocated );
    png_bytep pointers[2] = {0,0};

    pointers[0] = p;
    png_write_rows(png_ptr, pointers, 1);
  }

   /* You can write optional chunks like tEXt, zTXt, and tIME at the end
    * as well.  Shouldn't be necessary in 1.2.0 and up as all the public
    * chunks are supported and you can use png_set_unknown_chunks() to
    * register unknown chunks into the info structure to be written out.
    */

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);

   /* Whenever you use png_free() it is a good idea to set the pointer to
    * NULL in case your application inadvertently tries to png_free() it
    * again.  When png_free() sees a NULL it returns without action, thus
    * avoiding the double-free security problem.
    */

   /* Clean up after the write, and free any memory allocated */
   png_destroy_write_struct(&png_ptr, &info_ptr);

   /* Close the file */
   fclose(fp);

   /* That's it */
   return 0;
}

/** @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/07 (Oyranos: 0.1.8)
 *  @date    2009/02/18
 */
int      oPNGFilterPlug_ImageOutputPNGWrite (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = oyFilterPlug_GetSocket( requestor_plug );
  oyFilterNode_s * node = 0;
  oyOptions_s * opts = 0;
  int result = 0;
  const char * filename = 0;
  FILE * fp = 0;

  if(socket)
    node = oyFilterSocket_GetNode( socket );

  /* to reuse the requestor_plug is a exception for the starting request */
  if(node)
  {
    result = oyFilterNode_Run( node, requestor_plug, ticket );
    opts = oyFilterNode_GetOptions( node, 0 );
  } else
    result = 1;

  if(result <= 0)
  {
    filename = oyOptions_FindString( opts, "filename", 0 );
    if(!filename)
    {
      result = 1;
      oPNG_msg( oyMSG_WARN, node,
             OY_DBG_FORMAT_ "filename missed",
             OY_DBG_ARGS_ );
    }
  }

  if(filename)
    fp = fopen( filename, "wb" );

  if(fp)
  {
    oyImage_s *image = (oyImage_s*)oyFilterSocket_GetData( socket );

    fclose (fp); fp = 0;

    result = oyImage_WritePNG( image, filename, opts );
    oyImage_Release( &image );
  }
  else
    oPNG_msg( oyMSG_WARN, node,
             OY_DBG_FORMAT_ "could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ) );

  oyOptions_Release( &opts );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );
  return result;
}

const char png_write_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "file_write" ">\n\
      <filename></filename>\n\
     </" "file_write" ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

int  oPNGPNGwriteUiGet               ( oyCMMapiFilter_s   * module OY_UNUSED,
                                       oyOptions_s       * opts OY_UNUSED,
                                       int                 flags OY_UNUSED,
                                       char             ** xforms_layout,
                                       oyAlloc_f           allocateFunc )
{
  char * text = (char*)allocateFunc(5);
  text[0] = 0;
  *xforms_layout = text;
  return 0;
}


oyDATATYPE_e oPNG_image_png_data_types[5] = {oyUINT8, oyUINT16,
                                             oyFLOAT, oyDOUBLE, 0};



oyConnectorImaging_s_ oPNG_imageOutputPNG_connector_in = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oPNG_image_png_data_types,
  4, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  4, /* max_channels_count; */
  1, /* min_color_count; */
  4, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap color channels (BGR)*/
  0, /* can_swap_bytes; non host byte order */
  0, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  2, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s_ * oPNG_imageOutputPNG_connectors_plug[2] = 
             { &oPNG_imageOutputPNG_connector_in, 0 };

/**
 *  This function implements oyCMMGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/22 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
const char * oPNGApi4ImageWriteUiGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0)
  {
         if(type == oyNAME_NICK)
      return "write_png";
    else if(type == oyNAME_NAME)
      return _("Image[write_png]");
    else
      return _("Write PNG Image Filter Object");
  }
  else if(strcmp(select,"category") == 0)
  {
    if(!category)
    {
      STRING_ADD( category, _("Files") );
      STRING_ADD( category, _("/") );
      STRING_ADD( category, _("Write PNG") );
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return category;
    else
      return category;
  }
  else if(strcmp(select,"help") == 0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Option \"filename\", a valid filename");
    else
      return _("The Option \"filename\" should contain a valid filename to write the png data into. A existing file will be overwritten without notice.");
  }
  return 0;
}
const char * oPNG_api4_image_write_png_ui_texts[] = {"name", "category", "help", 0};

/** @brief    oPNG oyCMMapi4_s::ui implementation
 *
 *  The UI for filter write png.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
oyCMMui_s_ oPNG_api4_image_write_png_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oPNGFilter_ImageOutputPNGValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oPNGWidgetEvent, /* oyWidgetEvent_f */

  "Files/Write PNG", /* category */
  png_write_extra_options, /* const char * options */
  oPNGPNGwriteUiGet, /* oyCMMuiGet_f oyCMMuiGet */

  oPNGApi4ImageWriteUiGetText, /* oyCMMGetText_f   getText */
  oPNG_api4_image_write_png_ui_texts, /* const char    ** texts */
  (oyCMMapiFilter_s*)&oPNG_api4_image_write_png /* oyCMMapiFilter_s*parent */
};

/** @brief    oPNG oyCMMapi4_s implementation
 *
 *  A filter writing a PNG image.
 *
 *  @par Options:
 *  - "filename" - the file name to write to
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/07 (Oyranos: 0.1.8)
 *  @date    2008/10/07
 */
oyCMMapi4_s_ oPNG_api4_image_write_png = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oPNG_api7_image_write_png, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMReset, /* oyCMMReset_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/file_write.write_png._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  (oyCMMFilterNode_ContextToMem_f)oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  &oPNG_api4_image_write_png_ui        /**< oyCMMui_s_ *ui */
};

char * oPNG_api7_image_output_png_properties[] =
{
  "file=write",   /* file read|write */
  "image=pixel",  /* image type, pixel/vector/font */
  "layers=1",     /* layer count, one for plain images */
  "icc=1",        /* image type ICC profile support */
  "ext=png", /* supported extensions */
  0
};

/** @brief    oPNG oyCMMapi7_s implementation
 *
 *  A filter writing a PNG image.
 *
 *  @par Options:
 *  - "filename" - the file name to write to
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/07 (Oyranos: 0.1.8)
 *  @date    2008/10/07
 */
oyCMMapi7_s_ oPNG_api7_image_write_png = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oPNG_api4_image_input_png, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMReset, /* oyCMMReset_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/file_write.write_png._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oPNGFilterPlug_ImageOutputPNGWrite, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oPNG_imageOutputPNG_connectors_plug,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  0,   /* sockets */
  0,   /* sockets_n */
  0,   /* sockets_last_add */

  oPNG_api7_image_output_png_properties /* char * properties */
};


/* OY_INPUT_PNG_REGISTRATION ---------------------------------------------*/



oyOptions_s* oPNGFilter_ImageInputPNGValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = !oyOptions_FindString( validate, "filename", 0 );

  *result = error;

  return 0;
}

/** @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/12 (Oyranos: 0.1.11)
 *  @date    2010/09/12
 */
oyImage_s *  oyImage_FromPNG         ( const char        * filename,
                                       int32_t             icc_profile_flags,
                                       oyStruct_s        * object )
{
  int error = 0;

  /* file variables */
  FILE * fp = 0;
  int     fsize = 0, size = 0;
  size_t  fpos = 0;
  uint8_t * data = 0;

  int info_good = 1;

  /* general image variables */
  oyDATATYPE_e data_type = oyUINT8;
  oyPROFILE_e profile_type = oyASSUMED_WEB;
  oyProfile_s * prof = 0;
  oyImage_s * image_in = 0;
  oyPixel_t pixel_layout = 0;
  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int spp = 0;         /* samples per pixel */
  int bitps = 1;      /* byte per sample */
  /*double maxval = 0;*/
    
  /* PNG image variables */
  int is_png = 0;
  png_structp png_ptr = 0;
  png_infop info_ptr = 0;
  int color_type = 0,
      num_passes,
      channels_n = 0;


  if(filename)
    fp = fopen( filename,
#ifdef _WIN32
                "rb"
#else
                "rmb"
#endif
              );

  if(!fp)
  {
    oPNG_msg( oyMSG_WARN, object,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ) );
    return NULL;
  }

  fseek(fp,0L,SEEK_END);
  fsize = ftell(fp);
  rewind(fp);

  /* read the PNG header */
  size = 8;

  oyAllocHelper_m_( data, uint8_t, size, oyAllocateFunc_, fclose(fp); return NULL);

  fpos = fread( data, sizeof(uint8_t), size, fp );
  if( fpos < (size_t)size ) {
    oPNG_msg( oyMSG_WARN, object,
             OY_DBG_FORMAT_ " could not read: %s %d %d",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ), size, (int)fpos );
    oyFree_m_( data )
    fclose (fp);
    return NULL;
  }

  /* check the PNG header */
  is_png = !png_sig_cmp(data, 0, size);
  if (!is_png)
  {
    info_good = 0;
    goto png_read_clean;
  }

  png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,
                                    (png_voidp)filename,
                                    oPNGerror, oPNGwarn );
  if(!png_ptr)
  {
    info_good = 0;
    goto png_read_clean;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr)
  {
    png_destroy_read_struct( &png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    info_good = 0;
    goto png_read_clean;
  }

  if(png_jumpbuf_set(png_ptr))
  {
    /* Free all of the memory associated with the png_ptr and info_ptr */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    /* If we get here, we had a problem reading the file */
    info_good = 0;
    goto png_read_clean;
  }

  rewind( fp );
  png_init_io( png_ptr, fp );
  png_read_info( png_ptr, info_ptr );

  width = png_get_image_width( png_ptr, info_ptr );
  height = png_get_image_height( png_ptr, info_ptr );
  bitps = png_get_bit_depth( png_ptr, info_ptr );
  color_type = png_get_color_type( png_ptr, info_ptr );
  channels_n = png_get_channels( png_ptr, info_ptr );

  switch( color_type )
  {
  case PNG_COLOR_TYPE_GRAY:
       profile_type = oyASSUMED_GRAY;
       spp = 1; break;
  case PNG_COLOR_TYPE_GRAY_ALPHA:
       profile_type = oyASSUMED_GRAY;
       spp = 2; break;
  case PNG_COLOR_TYPE_PALETTE:
       png_set_palette_to_rgb( png_ptr );
       /* expect alpha */
       spp = 4; break;
  case PNG_COLOR_TYPE_RGB:
       spp = 3; break;
  case PNG_COLOR_TYPE_RGB_ALPHA:
       spp = 4; break;
  default: goto png_read_clean;
  }
  if(spp < channels_n)
    spp = channels_n;
  pixel_layout |= oyChannels_m(spp);


  switch(bitps)
  {
  case 1:
  case 2:
  case 4:
       png_set_expand( png_ptr ); OY_FALLTHROUGH;
  case 8:
       data_type = oyUINT8; break;
  case 16:
       if(!oyBigEndian())
         png_set_swap( png_ptr );
       data_type = oyUINT16; break;
  }
  pixel_layout |= oyDataType_m(data_type);

  oPNG_msg( oyMSG_DBG, object,
             OY_DBG_FORMAT_ " color_type: %d width: %d spp:%d channels: %d",
             OY_DBG_ARGS_, color_type, width, spp,oyToChannels_m(pixel_layout));

  num_passes = png_set_interlace_handling( png_ptr );
  /* update after all the above changes to the png structures */
  png_read_update_info( png_ptr, info_ptr );

  {
#if defined(PNG_iCCP_SUPPORTED)
    png_charp name = 0;
#if LIBPNG_VERSION_NUM >= 10501
  png_bytep
#else
  png_charp
#endif
                profile = 0;
    png_uint_32 proflen = 0;
    int compression = 0;

    if( png_get_iCCP( png_ptr, info_ptr, &name, &compression,
                      &profile, &proflen ) )
    {
      prof = oyProfile_FromMem( proflen, profile, 0,0 );
      oPNG_msg( oyMSG_DBG, object,
             OY_DBG_FORMAT_ " ICC profile (size: %d): \"%s\"",
             OY_DBG_ARGS_, proflen, oyNoEmptyString_m_( name ) );
      if(getenv("oPNG_ICC"))
        printf(
             OY_DBG_FORMAT_ " ICC profile (size: %d): \"%s\"\n",
             OY_DBG_ARGS_, (int)proflen, oyNoEmptyString_m_( name ) );
    } else
#endif
    prof = oyProfile_FromStd( profile_type, icc_profile_flags, 0 );
  }

  /* create the image */
  image_in = oyImage_Create( width, height, NULL, pixel_layout, prof, 0 );
  oyProfile_Release( &prof );
  if(image_in)
  {
    oyArray2d_s * a = oyArray2d_Create( NULL,
                                        width * oyToChannels_m(pixel_layout),
                                        height,
                                        oyToDataType_m(pixel_layout),
                                        0 );
    png_byte ** array2d = (png_byte**) oyArray2d_GetData( a );
    int i;
    unsigned y;

    /* both variants of libpng access appear equal */
    if(1)
      png_read_image( png_ptr, array2d );
    else
    for( i = 0; i < num_passes; ++i )
      for( y = 0; y < height; ++y )
        png_read_row( png_ptr, array2d[y], NULL );

    if(oPNG_error)
    {
      oPNG_error = 0;
      return NULL;
    }
    oyImage_SetData ( image_in, (oyStruct_s**) &a, 0,0,0,0,0,0 );
  }

  png_read_end( png_ptr, info_ptr );
  png_destroy_read_struct( &png_ptr, &info_ptr, (png_infopp)NULL );

  if (!image_in)
  {
      oPNG_msg( oyMSG_WARN, object,
             OY_DBG_FORMAT_ "PNG can't create a new image\n%dx%d %d",
             OY_DBG_ARGS_,  width, height, pixel_layout );
      oyFree_m_ (data)
    return NULL;
  }

  {
    oyOptions_s * tags = oyImage_GetTags( image_in );
    error = oyOptions_SetFromString( &tags,
                                 "//" OY_TYPE_STD "/file_read.input_png"
                                                                    "/filename",
                                 filename, OY_CREATE_NEW );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    oyOptions_Release( &tags );
  }

  png_read_clean:
  oyFree_m_ (data)

  if(!info_good)
  {
    oyImage_Release( &image_in );
    oPNG_msg( oyMSG_WARN, object,
             OY_DBG_FORMAT_ " could not read: %s %d %d",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ), fsize, (int)fpos );
  }
  fpos = 0;
  fclose (fp);
  fp = NULL;


  /* return an error to cause the graph to retry */
  return image_in;
}

/** @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/18 (Oyranos: 0.1.10)
 *  @date    2009/02/18
 */
int      oPNGFilterPlug_ImageInputPNGRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  /* module variables */
  oyFilterSocket_s * socket = 0;
  oyStruct_s * data = 0;
  oyFilterNode_s * node = 0;
  int error = 0;
  oyImage_s * image_in = 0,
            * output_image = 0;

  /* file variables */
  const char * filename = 0;

  int info_good = 1;
  int32_t icc_profile_flags = 0;

  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S)
  {
    socket = oyFilterPlug_GetSocket( requestor_plug );
    data = oyFilterSocket_GetData( socket );
  }

  /* passing through the data reading */
  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     data)
  {
    error = oyFilterPlug_ImageRootRun( requestor_plug, ticket );

    goto png_input_clean;

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

  if(error <= 0)
  {
    oyOptions_s * opts = oyFilterNode_GetOptions( node, 0 );
    filename = oyOptions_FindString( opts, "filename", 0 );
    oyOptions_FindInt( opts, "icc_profile_flags", 0, &icc_profile_flags );
    oyOptions_Release( &opts );
  }

  image_in = oyImage_FromPNG( filename, icc_profile_flags, (oyStruct_s*)node );

  if(!image_in)
  {
    oPNG_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " failed: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ) );
    goto png_input_clean;
  }

  if(error <= 0)
  {
    oyFilterSocket_SetData( socket, (oyStruct_s*)image_in );
  }

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

  png_input_clean:
  oyImage_Release( &image_in );
  oyImage_Release( &output_image );
  oyFilterNode_Release( &node );
  oyFilterSocket_Release( &socket );

  /* return an error to cause the graph to retry */
  return info_good;
}

const char png_read_extra_options[] = {
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

int  oPNGPNGreadUiGet                ( oyCMMapiFilter_s   * module OY_UNUSED,
                                       oyOptions_s       * opts OY_UNUSED,
                                       int                 flags OY_UNUSED,
                                       char             ** xforms_layout,
                                       oyAlloc_f           allocateFunc )
{
  char * text = (char*)allocateFunc(5);
  text[0] = 0;
  *xforms_layout = text;
  return 0;
}

const char * oPNG_imageInputPNG_connectorGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select, "name")==0)
  {
    if(type == oyNAME_NICK)
      return _("Img");
    else if(type == oyNAME_NAME)
      return _("Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Image PNG Socket");
  }
  return NULL;
}


oyConnectorImaging_s_ oPNG_imageInputPNG_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oPNG_image_png_data_types,
  4, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  4, /* max_channels_count; */
  1, /* min_color_count; */
  4, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap color channels (BGR)*/
  0, /* can_swap_bytes; non host byte order */
  0, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s_ * oPNG_imageInputPNG_connectors[2] = 
             { &oPNG_imageInputPNG_connector, 0 };


/**
 *  This function implements oyCMMGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/22 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
const char * oPNGApi4ImageInputUiGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0)
  {
         if(type == oyNAME_NICK)
      return "input_png";
    else if(type == oyNAME_NAME)
      return _("Image[input_png]");
    else
      return _("Input PNG Image Filter Object");
  }
  else if(strcmp(select,"category") == 0)
  {
    if(!category)
    {
      STRING_ADD( category, _("Files") );
      STRING_ADD( category, _("/") );
      STRING_ADD( category, _("Read PNG") );
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return category;
    else
      return category;
  }
  else if(strcmp(select,"help") == 0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Option \"filename\", a valid filename of a existing PNG image");
    else
      return _("The Option \"filename\" should contain a valid filename to read the png data from. If the file does not exist, a error will occure.\nThe iCCP chunk is searched for or a oyASSUMED_WEB/oyASSUMED_GRAY ICC profile will be attached to the resulting image. A embedded renering intent will be ignored.");
  }
  return 0;
}
const char * oPNG_api4_image_input_png_ui_texts[] = {"name", "category", "help", 0};

/** @brief    oPNG oyCMMapi4_s::ui implementation
 *
 *  The UI for filter input png.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_ oPNG_api4_ui_image_input_png = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oPNGFilter_ImageInputPNGValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oPNGWidgetEvent, /* oyWidgetEvent_f */

  "Files/Read PNG", /* category */
  png_read_extra_options, /* const char * options */
  oPNGPNGreadUiGet, /* oyCMMuiGet_f oyCMMuiGet */

  oPNGApi4ImageInputUiGetText, /* oyCMMGetText_f   getText */
  oPNG_api4_image_input_png_ui_texts, /* const char    ** texts */
  (oyCMMapiFilter_s*)&oPNG_api4_image_input_png /* oyCMMapiFilter_s*parent */
};

/** @brief    oPNG oyCMMapi4_s implementation
 *
 *  A filter for reading a PNG image.
 *
 *  @par Options:
 *  - "filename" - the file name to read from
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/18 (Oyranos: 0.1.10)
 *  @date    2009/02/18
 */
oyCMMapi4_s_ oPNG_api4_image_input_png = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oPNG_api7_image_input_png, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMReset, /* oyCMMReset_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/file_read.input_png._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  (oyCMMFilterNode_ContextToMem_f)oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  &oPNG_api4_ui_image_input_png        /**< oyCMMui_s_ *ui */
};

char * oPNG_api7_image_input_png_properties[] =
{
  "file=read",    /* file read|write */
  "image=pixel",  /* image type, pixel/vector/font */
  "layers=1",     /* layer count, one for plain images */
  "icc=0",        /* image type ICC profile support */
  "ext=png", /* supported extensions */
  0
};

/** @brief    oPNG oyCMMapi7_s implementation
 *
 *  A filter reading a PNG image.
 *
 *  @par Options:
 *  - "filename" - the file name to read from
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/18 (Oyranos: 0.1.10)
 *  @date    2009/02/18
 */
oyCMMapi7_s_ oPNG_api7_image_input_png = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) NULL, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMReset, /* oyCMMReset_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/file_read.input_png._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION, /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oPNGFilterPlug_ImageInputPNGRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  0,   /* plugs */
  0,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oPNG_imageInputPNG_connectors,   /* sockets */
  1,   /* sockets_n */
  0,    /* sockets_last_add */

  oPNG_api7_image_input_png_properties /* char * properties */
};

/* ---------------------------------------------------------------------------*/

