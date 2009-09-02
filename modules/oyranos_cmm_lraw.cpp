/** @file oyranos_cmm_lraw.cpp
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    libraw filter for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/06/14
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

#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <libraw/libraw.h>

/* --- internal definitions --- */

#define CMM_NICK "lraw"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}
#define OY_LIBRAW_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "file_read.input_libraw-lite._CPU._NOACCEL"


#ifdef __cplusplus
extern "C" {
namespace oyranos {
#endif /* __cplusplus */

#include <iconv.h>

int lrawCMMWarnFunc( int code, const oyranos::oyStruct_s * context, const char * format, ... );
oyranos::oyMessage_f message = lrawCMMWarnFunc;

extern oyranos::oyCMMapi4_s   lraw_api4_image_input_libraw;
extern oyranos::oyCMMapi7_s   lraw_api7_image_input_libraw;

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

using namespace oyranos;


/* --- implementations --- */

/** Function lrawCMMInit
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int                lrawCMMInit       ( )
{
  int error = 0;
  return error;
}



/** Function lrawIconv
 *  @brief convert between codesets
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
/*int                lrawIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset )
{
  return oyIconv(input, len, output, from_codeset, 0);
}*/





/** Function lrawCMMWarnFunc
 *  @brief message handling
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
extern "C" {
int lrawCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
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
}

/** Function lrawCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int            lrawCMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}


int oyStructList_MoveInName( oyStructList_s * texts, char ** text, int pos )
{
  int error = !texts || ! text;
  oyName_s * name = 0;
  oyStruct_s * oy_struct = 0;
  if(!error)
  {
     name = oyName_new(0);
     name->name = *text;
     *text = 0;
     oy_struct = (oyStruct_s*) name;
     oyStructList_MoveIn( texts, &oy_struct, pos );
  }
  return error;
}

int oyStructList_AddName( oyStructList_s * texts, const char * text, int pos )
{
  int error = !texts;
  oyName_s * name = 0;
  oyStruct_s * oy_struct = 0;
  char * tmp = 0;
  if(!error)
  {
     name = oyName_new(0);
     if(!name) return 1;
     if(text)
     {
       tmp = (char*) oyAllocateFunc_( strlen(text) + 1 );
       if(!tmp) return 1;
       sprintf( tmp, "%s", text ); 
       name->name = tmp;
     }
     oy_struct = (oyStruct_s*) name;
     oyStructList_MoveIn( texts, &oy_struct, pos );
  }
  return error;
}



const char * lrawWidget_GetDummy     ( const char        * func_name,
                                       uint32_t          * result )
{return 0;}
oyWIDGET_EVENT_e lrawWidget_EventDummy
                                     ( const char        * wid,
                                       oyWIDGET_EVENT_e    type )
{return (oyWIDGET_EVENT_e)0;}


oyWIDGET_EVENT_e   lrawWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return (oyWIDGET_EVENT_e)0;}


/** Function lrawFilterNode_LibrawContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyPointer  lrawFilterNode_LibrawContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
const char * lrawGetText             ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("libraw input filter");
    else
      return _("libraw input filter");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Kai-Uwe");
    else if(type == oyNAME_NAME)
      return _("Kai-Uwe Behrmann");
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("newBSD");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2009 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}
const char *lraw_texts[4] = {"name","copyright","manufacturer",0};


/** @instance lraw_cmm_module
 *  @brief    lraw module infos
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMInfo_s lraw_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  (char*)"0.1.8",
  lrawGetText,
  (char**)lraw_texts, /* texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & lraw_api4_image_input_libraw,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, (char*)"oyranos_logo.png"},
};


/* OY_LIBRAW_REGISTRATION ----------------------------------------------------*/


oyOptions_s* lrawFilter_ImageInputRAWValidateOptions
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

int wread ( unsigned char* data, size_t pos, size_t max, size_t *start, size_t *end )
{
  int end_found = 0;

  if( max <= 1 ) return 0;

  while(pos < max && isspace( data[pos] )) ++pos;
  *start = pos;

  while(pos < max && !end_found) {
    if( isspace( data[pos] ) ) {
      end_found = 1;
      break;
    } else
      ++pos;
  }
  *end = pos;

  return end_found;
}

/** Function lrawFilterPlug_ImageInputRAWRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int      lrawFilterPlug_ImageInputRAWRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s * node = 0;
  int error = 0;
  const char * filename = 0;
  oyDATATYPE_e data_type = oyUINT8;
  oyPROFILE_e profile_type = oyEDITING_RGB;
  oyProfile_s * prof = 0;
  oyImage_s * image_in = 0;
  oyPixel_t pixel_type = 0;
  uint8_t * data = 0, * buf = 0;
  size_t  mem_n = 0;   /* needed memory in bytes */

  int info_good = 1;

  int type = 0;        /* PNM type */
  int width = 0;
  int height = 0;
  int spp = 0;         /* samples per pixel */
  int byteps = 1;      /* byte per sample */
  double maxval = 255; 
    
  LibRaw rip;
  libraw_processed_image_t * image_rgb = 0;

  /* passing through the data reading */
  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     requestor_plug->remote_socket_->data)
  {
    socket = requestor_plug->remote_socket_;
    error = oyFilterPlug_ImageRootRun( requestor_plug, ticket );

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
    error = rip.open_file( filename );

  if(error)
  {
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ) );
    return 1;
  }

  error = rip.unpack();
  error = rip.dcraw_process();

  image_rgb = rip.dcraw_make_mem_image();

  if(image_rgb)
  {
    width = image_rgb->width;
    height = image_rgb->height;
    spp = image_rgb->colors;

    if(image_rgb->bits == 16)
    {
      data_type = oyUINT16;
      byteps = 2;
      maxval = 65535;
    }

  } else
  {
    info_good = 0;
  }

    switch(spp)
    {
      case 1:
           profile_type = oyASSUMED_GRAY;
           type = 5;
           break;
      case 2:
           profile_type = oyASSUMED_GRAY;
           type = 5;
           break;
      case 3:
           profile_type = oyASSUMED_RGB;
           type = 6;
           break;
      case 4:
           profile_type = oyASSUMED_RGB;
           type = 6;
           break;
    }

  if( !info_good )
  {
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "failed to get info of %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ));
    oyFree_m_( data )
    return FALSE;
  }

  /* check if the file can hold the expected data (for raw only) */
  mem_n = width*height*byteps*spp;
  error = mem_n != image_rgb->data_size;

  oyAllocHelper_m_( buf, uint8_t, mem_n, 0, return 1);

  /* the following code is almost completely taken from ku.b's ppm CP plug-in */
  /* ... and them copied from the input_ppm Oyranos filter */
  {
    int h, j_h = 0, p, n_samples, n_bytes;
    int byte_swap = 0; /*!oyBigEndian();*/
    unsigned char *d_8 = 0;
    unsigned char *src = image_rgb->data;

    uint16_t *d_16;
    float  *d_f;

    for(h = 0; h < height; ++h)
    {
        n_samples = 1 * width * spp;
        n_bytes = n_samples * byteps;

        d_8  = buf;
        d_16 = (uint16_t*)buf;
        d_f  = (float*)buf;

        /*  TODO 1 bit raw and ascii */
        if (type == 1 || type == 4) {

        /*  TODO ascii  */
        } else if (type == 2 || type == 3) {


        /*  raw and floats */
        } else if (type == 5 || type == 6 ||
                   type == -5 || type == -6 ||
                   type == 7 )
        {
          if(byteps == 1) {
            d_8 = &src[ h * width * spp * byteps ];
          } else if(byteps == 2) {
            d_16 = (uint16_t*)& src[ h * width * spp * byteps ];
          } else if(byteps == 4) {
            d_f = (float*)&src[ h * width * spp * byteps ];
          }
          memcpy (&buf[ h * width * spp * byteps ],
                  &src[ (j_h + h) * width * spp * byteps ],
                  1 * width * spp * byteps);
        }

        /* normalise and byteswap */
        if( byte_swap )
        {
          unsigned char *c_buf = &buf[ h * width * spp * byteps ];
          char  tmp;
          if (byteps == 2) {         /* 16 bit */
            for (p = 0; p < n_bytes; p += 2)
            {
              tmp = c_buf[p];
              c_buf[p] = c_buf[p+1];
              c_buf[p+1] = tmp;
            }
          } else if (byteps == 4) {  /* float */
            for (p = 0; p < n_bytes; p += 4)
            {
              tmp = c_buf[p];
              c_buf[p] = c_buf[p+3];
              c_buf[p+3] = tmp;
              tmp = c_buf[p+1];
              c_buf[p+1] = c_buf[p+2];
              c_buf[p+2] = tmp;
            }
          }
        }

        if (byteps == 1 && maxval < 255) {         /*  8 bit */
          for (p = 0; p < n_samples; ++p)
            d_8[p] = (d_8[p] * 255) / maxval;
        } else if (byteps == 2 && maxval < 65535) {/* 16 bit */
          for (p = 0; p < n_samples; ++p)
            d_16 [p] = (d_16[p] * 65535) / maxval;
        } else if (byteps == 4 && maxval != 1.0) {  /* float */
          for (p = 0; p < n_samples; ++p)
            d_f[p] = d_f[p] * maxval;
        }
    }
  }

  pixel_type = oyChannels_m(spp) | oyDataType_m(data_type); 
  prof = oyProfile_FromStd( profile_type, 0 );

  image_in = oyImage_Create( width, height, buf, pixel_type, prof, 0 );

  if (!image_in)
  {
      message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "libraw can't create a new image\n%dx%d %d",
             OY_DBG_ARGS_,  width, height, pixel_type );
      oyFree_m_ (data)
    return FALSE;
  }

  error = oyOptions_SetFromText( &image_in->tags,
                                 "//" OY_TYPE_STD OY_SLASH CMM_NICK "/filename",
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

const char lraw_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "file_read." CMM_NICK ">\n\
      <filename></filename>\n\
     </" "file_read." CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <output_bps>16</output_bps>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <use_auto_wb.advanced>0</use_auto_wb.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <use_camera_wb.advanced>1</use_camera_wb.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <four_color_rgb.advanced>0</four_color_rgb.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <half_size.advanced>0</half_size.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <highlight.advanced>0</highlight.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <gamma_16bit.advanced>0</gamma_16bit.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <auto_bright.advanced>0.01</auto_bright.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <no_auto_bright.advanced>0</no_auto_bright.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <use_fujy_rotate.advanced>1</use_fujy_rotate.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <filtering_mode.advanced>0</filtering_mode.advanced>\n\
     </" CMM_NICK ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

oyDATATYPE_e lraw_data_types[3] = {oyUINT8, oyUINT16, (oyDATATYPE_e)0};

oyConnectorImaging_s lraw_imageInputRAW_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, (char*)"Img", (char*)"Image", (char*)"Image libraw Socket"},
  (char*)"//" OY_TYPE_STD "/image.data", /* connector_type */
  0, /* is_plug == oyFilterPlug_s */
  lraw_data_types,
  2, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  3, /* max_channels_count; */
  1, /* min_colour_count; */
  3, /* max_colour_count; */
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
oyConnectorImaging_s * lraw_imageInputRAW_connectors[2] = 
             { &lraw_imageInputRAW_connector, 0 };


/** @instance lraw_api4
 *  @brief    lraw oyCMMapi4_s implementation
 *
 *  A filter for reading a RAW image.
 *
 *  @par Options:
 *  - "filename" - the file name to read from
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMapi4_s   lraw_api4_image_input_libraw = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & lraw_api7_image_input_libraw, /* oyCMMapi_s * next */
  
  lrawCMMInit, /* oyCMMInit_f */
  lrawCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_LIBRAW_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,0,10},                  /**< int32_t last_module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lrawFilter_ImageInputRAWValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  lrawWidgetEvent, /* oyWidgetEvent_f */

  lrawFilterNode_LibrawContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  {oyOBJECT_NAME_S, 0,0,0, (char*)"input_libraw-lite", (char*)"Image[input_libraw-lite]", (char*)"Input libraw Image Filter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Files/Read cameraRAW", /* category */
  lraw_extra_options,   /* options */
  0    /* opts_ui_ */
};

/** @instance lraw_api7
 *  @brief    lraw oyCMMapi7_s implementation
 *
 *  A filter reading a RAW image.
 *
 *  @par Options:
 *  - "filename" - the file name to write to
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMapi7_s   lraw_api7_image_input_libraw = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) 0, /* oyCMMapi_s * next */
  
  lrawCMMInit, /* oyCMMInit_f */
  lrawCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_LIBRAW_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,0,10},                  /**< int32_t last_module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lrawFilterPlug_ImageInputRAWRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  0,   /* plugs */
  0,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) lraw_imageInputRAW_connectors,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};


/* OY_LIBRAW_REGISTRATION ----------------------------------------------------*/

