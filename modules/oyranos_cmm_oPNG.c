/** @file oyranos_cmm_oyra_image_png.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2008-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    PNG module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/06
 */

#include "config.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"
#include <iconv.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CMM_NICK "oPNG"
#define CMM_VERSION {0,1,0}

oyMessage_f message = oyFilterMessageFunc;

extern oyCMMapi4_s   oPNG_api4_image_write_png;
extern oyCMMapi7_s   oPNG_api7_image_write_png;
extern oyCMMapi4_s   oPNG_api4_image_input_png;
extern oyCMMapi7_s   oPNG_api7_image_input_png;

/* OY_INPUT_PNG_REGISTRATION */
/* OY_WRITE_PNG_REGISTRATION */


oyWIDGET_EVENT_e   oPNGWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}

int                oPNGCMMInit       ( oyStruct_s        * filter )
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
  message = message_func;
  return 0;
}


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/06 (Oyranos: 0.1.11)
 *  @date    2010/09/06
 */
const char * oPNGGetText             ( const char        * select,
                                       oyNAME_e            type )
{
  if(strcmp(select, "name")==0)
    if(type == oyNAME_NICK)
      return _(CMM_NICK);

  return oyCMMgetText( select, type );
}


/** @instance oPNG_cmm_module
 *  @brief    oPNG module infos
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
oyCMMInfo_s oPNG_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.1.11",
  oPNGGetText,        /**< getText */
  (char**)oyCMM_texts, /**< texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oPNG_api4_image_write_png,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};


/* OY_WRITE_PNG_REGISTRATION ---------------------------------------------*/


oyOptions_s* oPNGFilter_ImageOutputPNGValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
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

/** @func    oPNGFilterPlug_ImageOutputPNGWrite
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/07 (Oyranos: 0.1.8)
 *  @date    2009/02/18
 */
int      oPNGFilterPlug_ImageOutputPNGWrite (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * input_node = 0,
                 * node = 0;
  int result = 0;
  const char * filename = 0;
  FILE * fp = 0;

  node = socket->node;
  plug = (oyFilterPlug_s *)node->plugs[0];
  input_node = plug->remote_socket_->node;

  /* to reuse the requestor_plug is a exception for the starting request */
  result = input_node->api7_->oyCMMFilterPlug_Run( plug, ticket );

  if(result <= 0)
    filename = oyOptions_FindString( node->core->options_, "filename", 0 );

  if(filename)
    fp = fopen( filename, "wb" );

  if(fp)
  {
      size_t pt = 0;
      char text[128];
      int  len = 0;
      int  i,j,k,l, n;
      char bytes[48];
      oyImage_s *image_output = (oyImage_s*)socket->data;

      int cchan_n = oyProfile_GetChannelsCount( image_output->profile_ );
      int channels = oyToChannels_m( image_output->layout_[0] );
      oyDATATYPE_e data_type = oyToDataType_m( image_output->layout_[0] );
      int alpha = channels - cchan_n;
      int byteps = oySizeofDatatype( data_type );
      const char * colourspacename = oyProfile_GetText( image_output->profile_,
                                                        oyNAME_DESCRIPTION );
      char * vs = oyVersionString(1,malloc);
      uint8_t * out_values = 0;
      const uint8_t * u8;
      double * dbls;
      float flt;

            fputc( 'P', fp );
      if(alpha) 
            fputc( '7', fp );
      else
      {
        if(byteps == 1 ||
           byteps == 2)
        {
          if(channels == 1)
            fputc( '5', fp );
          else
            fputc( '6', fp );
        } else
        if (byteps == 4 || byteps == 8)
        {
          if(channels == 1)
            fputc( 'f', fp ); /* PFM gray */
          else
            fputc( 'F', fp ); /* PFM rgb */
        }
      }

      fputc( '\n', fp );

      snprintf( text, 128, "# CREATOR: Oyranos-%s " CMM_NICK "\"%s\"\n",
                oyNoEmptyString_m_(vs), node->relatives_ );
      if(vs) free(vs); vs = 0;
      len = strlen( text );
      do { fputc ( text[pt++] , fp); } while (--len); pt = 0;

      {
        time_t  cutime;         /* Time since epoch */
        struct tm       *gmt;
        char time_str[24];

        cutime = time(NULL); /* time right NOW */
        gmt = gmtime(&cutime);
        strftime(time_str, 24, "%Y/%m/%d %H:%M:%S", gmt);
        snprintf( text, 128, "# DATE/TIME: %s\n", time_str );
        len = strlen( text );
        do { fputc ( text[pt++] , fp); } while (--len); pt = 0;
      }

      snprintf( text, 128, "# COLORSPACE: %s\n", colourspacename ?
                colourspacename : "--" );
      len = strlen( text );
      do { fputc ( text[pt++] , fp); } while (--len); pt = 0;

      if(byteps == 1)
        snprintf( bytes, 48, "255" );
      else
      if(byteps == 2)
        snprintf( bytes, 48, "65535" );
      else
      if (byteps == 4 || byteps == 8) 
      {
        if(oyBigEndian())
          snprintf( bytes, 48, "1.0" );
        else
          snprintf( bytes, 48, "-1.0" );
      }
      else
        message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " byteps: %d",
             OY_DBG_ARGS_, byteps );


      if(alpha)
      {
        const char *tupl = "RGB_ALPHA";

        if(channels == 2)
          tupl = "GRAYSCALE_ALPHA";
        snprintf( text, 128, "WIDTH %d\nHEIGHT %d\nDEPTH %d\nMAXVAL "
                  "%s\nTUPLTYPE %s\nENDHDR\n",
                  image_output->width, image_output->height,
                  channels, bytes, tupl );
        len = strlen( text );
        do { fputc ( text[pt++] , fp); } while (--len); pt = 0;

      }
      else
      {
        snprintf( text, 128, "%d %d\n", image_output->width,
                                       image_output->height);
        len = strlen( text );
        do { fputc ( text[pt++] , fp); } while (--len); pt = 0;

        snprintf( text, 128, "%s\n", bytes );
        len = strlen( text );
        do { fputc ( text[pt++] , fp); } while (--len); pt = 0;
      }

      n = image_output->width * channels;
      if(byteps == 8)
        u8 = (uint8_t*) &flt;

      for( k = 0; k < image_output->height; ++k)
      {
        int height = 0,
            is_allocated = 0;
        out_values = image_output->getLine( image_output, k, &height, -1, 
                                            &is_allocated );
        len = n * byteps;

        for( l = 0; l < height; ++l )
        {
          if(byteps == 8)
          {
            dbls = (double*)out_values;
            for(i = 0; i < n; ++i)
            {
              flt = dbls[l * len + i];
              for(j = 0; j < 4; ++j)
                fputc ( u8[j], fp);
            }
          } else 
          for(i = 0; i < len; ++i)
          {
            if(!oyBigEndian() && (byteps == 2))
            { if(i%2)
                fputc ( out_values[l * len + i - 1] , fp);
              else
                fputc ( out_values[l * len + i + 1] , fp);
            } else
              fputc ( out_values[l * len + i] , fp);
          }
        }

        if(is_allocated)
          image_output->oy_->deallocateFunc_(out_values);
      }

      fflush( fp );
      fclose (fp);

    /*message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "write file %s",
             OY_DBG_ARGS_, filename );*/
  }

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

int  oPNGPNGwriteUiGet               ( oyOptions_s       * opts,
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

oyConnectorImaging_s oPNG_imageOutputPNG_connector_out = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image PNG Socket"},
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oPNG_image_png_data_types,
  4, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  4, /* max_channels_count; */
  1, /* min_colour_count; */
  4, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap colour channels (BGR)*/
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
oyConnectorImaging_s * oPNG_imageOutputPNG_connectors_socket[2] = 
             { &oPNG_imageOutputPNG_connector_out, 0 };

oyConnectorImaging_s oPNG_imageOutputPNG_connector_in = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image PNG Plug"},
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oPNG_image_png_data_types,
  4, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  4, /* max_channels_count; */
  1, /* min_colour_count; */
  4, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap colour channels (BGR)*/
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
oyConnectorImaging_s * oPNG_imageOutputPNG_connectors_plug[2] = 
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
                                       oyNAME_e            type )
{
  static char * category = 0;
  if(strcmp(select,"name"))
  {
         if(type == oyNAME_NICK)
      return "write_png";
    else if(type == oyNAME_NAME)
      return _("Image[write_png]");
    else
      return _("Write PNG Image Filter Object");
  }
  else if(strcmp(select,"category"))
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
  else if(strcmp(select,"help"))
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

/** @instance oPNG_api4_image_write_png_ui
 *  @brief    oPNG oyCMMapi4_s::ui implementation
 *
 *  The UI for filter write png.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
oyCMMui_s oPNG_api4_image_write_png_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,1,10},                            /**< int32_t module_api[3] */

  oPNGFilter_ImageOutputPNGValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oPNGWidgetEvent, /* oyWidgetEvent_f */

  "Files/Write PNG", /* category */
  png_write_extra_options, /* const char * options */
  oPNGPNGwriteUiGet, /* oyCMMuiGet_f oyCMMuiGet */

  oPNGApi4ImageWriteUiGetText, /* oyCMMGetText_f   getText */
  oPNG_api4_image_write_png_ui_texts /* const char    ** texts */
};

/** @instance oPNG_api4
 *  @brief    oPNG oyCMMapi4_s implementation
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
oyCMMapi4_s   oPNG_api4_image_write_png = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oPNG_api7_image_write_png, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/write_png.file_write._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  &oPNG_api4_image_write_png_ui        /**< oyCMMui_s *ui */
};

/** @instance oPNG_api7
 *  @brief    oPNG oyCMMapi7_s implementation
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
oyCMMapi7_s   oPNG_api7_image_write_png = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oPNG_api4_image_input_png, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/write_png.file_write._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oPNGFilterPlug_ImageOutputPNGWrite, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oPNG_imageOutputPNG_connectors_plug,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oPNG_imageOutputPNG_connectors_socket,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};


/* OY_INPUT_PNG_REGISTRATION ---------------------------------------------*/



oyOptions_s* oPNGFilter_ImageInputPNGValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = !oyOptions_FindString( validate, "filename", 0 );

  *result = error;

  return 0;
}

/** @func    oPNGFilterPlug_ImageInputPNGRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/18 (Oyranos: 0.1.10)
 *  @date    2009/02/18
 */
int      oPNGFilterPlug_ImageInputPNGRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s * node = 0;
  int error = 0;
  const char * filename = 0;
  FILE * fp = 0;
  oyDATATYPE_e data_type = oyUINT8;
  oyPROFILE_e profile_type = oyEDITING_RGB;
  oyProfile_s * prof = 0;
  oyImage_s * image_in = 0;
  oyPixel_t pixel_type = 0;
  int     fsize = 0;
  size_t  fpos = 0;
  uint8_t * data = 0, * buf = 0;
  size_t  mem_n = 0;   /* needed memory in bytes */
    
  int info_good = 1;

  int type = 0;        /* PNM type */
  int width = 0;
  int height = 0;
  int spp = 0;         /* samples per pixel */
  int byteps = 1;      /* byte per sample */
  double maxval = 0; 
    
  size_t start, end;

  /* passing through the data reading */
  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     requestor_plug->remote_socket_->data)
  {
    error = oPNGFilterPlug_ImageRootRun( requestor_plug, ticket );

    return error;

  } else if(requestor_plug->type_ == oyOBJECT_FILTER_SOCKET_S)
  {
    /* To open the a image here seems not so straight forward.
     * Still the plug-in should be prepared to initialise the image data before
     * normal processing occurs.
     */
    socket = (oyFilterSocket_s*) requestor_plug;
    requestor_plug = 0;
    node = socket->node;

  } else {
    /* second option to open the file */
    socket = requestor_plug->remote_socket_;
    node = socket->node;
  }


  if(error <= 0)
    filename = oyOptions_FindString( node->core->options_, "filename", 0 );

  if(filename)
    fp = fopen( filename, "rm" );

  if(!fp)
  {
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ) );
    return 1;
  }

  fseek(fp,0L,SEEK_END);
  fsize = ftell(fp);
  rewind(fp);

  prof = oyProfile_FromStd( profile_type, 0 );

  image_in = oyImage_Create( width, height, buf, pixel_type, prof, 0 );

  if (!image_in)
  {
      message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "PNM can't create a new image\n%dx%d %d",
             OY_DBG_ARGS_,  width, height, pixel_type );
      oyFree_m_ (data)
    return FALSE;
  }

  error = oyOptions_SetFromText( &image_in->tags,
                                 "//" OY_TYPE_STD "/input_png.file_read"
                                                                    "/filename",
                                 filename, OY_CREATE_NEW );

  if(error <= 0)
  {
    socket->data = (oyStruct_s*)oyImage_Copy( image_in, 0 );
  }

  if(ticket &&
     ticket->output_image &&
     ticket->output_image->width == 0 &&
     ticket->output_image->height == 0)
  {
    ticket->output_image->width = image_in->width;
    ticket->output_image->height = image_in->height;
    oyImage_SetCritical( ticket->output_image, image_in->layout_[0], 0,0 );
  }

  oyImage_Release( &image_in );
  oyFree_m_ (data)

  /* return an error to cause the graph to retry */
  return 1;
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

int  oPNGPNGreadUiGet                ( oyOptions_s       * opts,
                                       char             ** xforms_layout,
                                       oyAlloc_f           allocateFunc )
{
  char * text = (char*)allocateFunc(5);
  text[0] = 0;
  *xforms_layout = text;
  return 0;
}


oyConnectorImaging_s oPNG_imageInputPNG_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image PNG Socket"},
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oPNG_image_png_data_types,
  4, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  4, /* max_channels_count; */
  1, /* min_colour_count; */
  4, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap colour channels (BGR)*/
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
oyConnectorImaging_s * oPNG_imageInputPNG_connectors[2] = 
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
                                       oyNAME_e            type )
{
  static char * category = 0;
  if(strcmp(select,"name"))
  {
         if(type == oyNAME_NICK)
      return "input_png";
    else if(type == oyNAME_NAME)
      return _("Image[input_png]");
    else
      return _("Input PNG Image Filter Object");
  }
  else if(strcmp(select,"category"))
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
  else if(strcmp(select,"help"))
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Option \"filename\", a valid filename of a existing PNG image");
    else
      return _("The Option \"filename\" should contain a valid filename to read the png data from. If the file does not exist, a error will occure.");
  }
  return 0;
}
const char * oPNG_api4_image_input_png_ui_texts[] = {"name", "category", "help", 0};

/** @instance oPNG_api4_ui_image_input_png
 *  @brief    oPNG oyCMMapi4_s::ui implementation
 *
 *  The UI for filter input png.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s oPNG_api4_ui_image_input_png = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,1,10},                            /**< int32_t module_api[3] */

  oPNGFilter_ImageInputPNGValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oPNGWidgetEvent, /* oyWidgetEvent_f */

  "Files/Read PNG", /* category */
  png_read_extra_options, /* const char * options */
  oPNGPNGreadUiGet, /* oyCMMuiGet_f oyCMMuiGet */

  oPNGApi4ImageInputUiGetText, /* oyCMMGetText_f   getText */
  oPNG_api4_image_input_png_ui_texts /* const char    ** texts */
};

/** @instance oPNG_api4
 *  @brief    oPNG oyCMMapi4_s implementation
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
oyCMMapi4_s   oPNG_api4_image_input_png = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oPNG_api7_image_input_png, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/input_png.file_read._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  &oPNG_api4_ui_image_input_png        /**< oyCMMui_s *ui */
};

char * oPNG_api7_image_input_png_properties[] =
{
  "file=read",    /* file load|write */
  "image=pixel",  /* image type, pixel/vector/font */
  "layers=1",     /* layer count, one for plain images */
  "icc=0",        /* image type ICC profile support */
  "ext=png", /* supported extensions */
  0
};

/** @instance oPNG_api7
 *  @brief    oPNG oyCMMapi7_s implementation
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
oyCMMapi7_s   oPNG_api7_image_input_png = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) NULL, /* oyCMMapi_s * next */
  
  oPNGCMMInit, /* oyCMMInit_f */
  oPNGCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD "/input_png.file_read._CPU._" CMM_NICK,

  CMM_VERSION, /* int32_t version[3] */
  {0,1,10},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

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

