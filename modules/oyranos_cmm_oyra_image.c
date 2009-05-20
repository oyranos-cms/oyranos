/** @file oyranos_cmm_oyra_image.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2008-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    backends for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/01/02
 */

#include "config.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
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
#include <stdint.h>  /* UINT32_MAX */

/* OY_IMAGE_REGIONS_REGISTRATION */
/* OY_IMAGE_ROOT_REGISTRATION */
/* OY_IMAGE_OUTPUT_REGISTRATION */


oyDATATYPE_e oyra_image_data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                         oyHALF, oyFLOAT, oyDOUBLE, 0};

/* OY_IMAGE_REGIONS_REGISTRATION ---------------------------------------------*/


/** @func    oyraFilter_ImageRectanglesCanHandle
 *  @brief   inform about image handling capabilities
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/02/23
 */
int    oyraFilter_ImageRectanglesCanHandle(oyCMMQUERY_e     type,
                                       uint32_t          value )
{
  int ret = -1;

  switch(type)
  {
    case oyQUERY_OYRANOS_COMPATIBILITY:
         ret = OYRANOS_VERSION; break;
    case oyQUERY_PIXELLAYOUT_CHANNELCOUNT:
         ret = 65535;
         break;
    case oyQUERY_PIXELLAYOUT_DATATYPE:
         switch(value) {
         case oyUINT8:
         case oyUINT16:
         case oyUINT32:
         case oyHALF:
         case oyFLOAT:
         case oyDOUBLE:
              ret = 1; break;
         default:
              ret = 0; break;
         }
         break;
    case oyQUERY_PIXELLAYOUT_SWAP_COLOURCHANNELS:
         ret = 1;
         break;
    case oyQUERY_PIXELLAYOUT_PLANAR:
         ret = 1;
         break;
    case oyQUERY_HDR:
         ret = 1;
         break;
    default: break;
  }

  return ret;
}

oyOptions_s* oyraFilter_ImageRectanglesValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}

/** @func    oyraFilterNode_ImageRectanglesContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/02/23
 */
oyPointer  oyraFilterNode_ImageRectanglesContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

/** @func    oyraFilterPlug_ImageRectanglesRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int      oyraFilterPlug_ImageRectanglesRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int x = 0, y = 0, n = 0, i;
  int result = 0, l_result = 0, error = 0;
  int is_allocated = 0;
  oyPointer * ptr = 0;
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node;
  oyImage_s * image = (oyImage_s*)socket->data;
  oyOption_s * o = 0;
  oyRectangle_s * r, * sr;
  oyPixelAccess_s * new_ticket = 0;
  int dirty = 0;

  x = ticket->start_xy[0];
  y = ticket->start_xy[1];

  result = oyPixelAccess_CalculateNextStartPixel( ticket, requestor_plug);

  if(result != 0)
    return result;

  if(x < image->width &&
     y < image->height &&
     ticket->pixels_n)
  {
    n = ticket->pixels_n;
    if(n == 1)
      ptr = image->getPoint( image, x, y, 0, &is_allocated );

    result = !ptr;

  } else {

    if(!node->core->options_)
      dirty = 1;

    if(dirty)
      return dirty;

    n = oyOptions_CountType( node->core->options_,
                             "//" OY_TYPE_STD "/rectangles/rectangle",
                             oyOBJECT_RECTANGLE_S );

    /* rectangles stuff */
    for(i = 0; i < n; ++i)
    {
      /* select current rectangle */
      r = (oyRectangle_s*)oyOptions_GetType( node->core->options_, i,
                                       "//" OY_TYPE_STD "/rectangles/rectangle",
                                             oyOBJECT_RECTANGLE_S );

      /* Map each matching plug to a new ticket with a corrected rectangle. */
      new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
      oyArray2d_Release( &new_ticket->array );

      if(r)
        oyRectangle_SetByRectangle( new_ticket->output_image_roi, r );

      /* select node */
      input_node = node->plugs[i]->remote_socket_->node;
      /* adapt the rectangle of interesst to the new image dimensions */
      oyRectangle_Trim( new_ticket->output_image_roi, ticket->output_image_roi );

      if(oyRectangle_CountPoints(  new_ticket->output_image_roi ) > 0)
      {
        /* fill the array rectangle for the following filter */
        if(!new_ticket->array)
          oyImage_FillArray( new_ticket->output_image,
                             new_ticket->output_image_roi, 1,
                            &new_ticket->array, 0, 0 );

        /* start new call into branch */
        l_result = input_node->api7_->oyCMMFilterPlug_Run( node->plugs[i],
                                                           new_ticket );
        if(l_result != 0 && (result <= 0 || l_result > 0))
          result = l_result;

        /* The image is used as intermediate cache between the forward and
           backward array. @todo use direct array copy oyArray2d_DataCopy() */
        error = oyImage_ReadArray( image, new_ticket->output_image_roi,
                                   new_ticket->array, 0 );
        sr = oyRectangle_SamplesFromImage( image, new_ticket->output_image_roi, 0);
        error = oyImage_FillArray( new_ticket->output_image,
                                   new_ticket->output_image_roi, 1,
                                  &ticket->array, sr, 0 );
        oyRectangle_Release( &sr );
      }
      oyPixelAccess_Release( &new_ticket );

      oyOption_Release( &o );
    }
  }

  return result;
}


oyConnectorImaging_s oyra_imageRectangles_plug = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Rectangles Plug"},
  "//" OY_TYPE_STD "/splitter", /* connector_type */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageRectangles_plugs[2] = {&oyra_imageRectangles_plug,0};

oyConnectorImaging_s oyra_imageRectangles_socket = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Rectangles Plug"},
  "//" OY_TYPE_STD "/image", /* connector_type */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  2, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageRectangles_sockets[2] = {&oyra_imageRectangles_socket,0};


#define OY_IMAGE_REGIONS_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "rectangles"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter routing the graph to several rectangles
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi7_s   oyra_api7_image_rectangles = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_write_ppm, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRectanglesCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_REGIONS_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageRectanglesRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageRectangles_plugs,   /* plugs */
  1,   /* plugs_n */
  UINT16_MAX,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageRectangles_sockets,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter routing the graph to several rectangles
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi4_s   oyra_api4_image_rectangles = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_rectangles, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRectanglesCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_REGIONS_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  {oyOBJECT_NAME_S, 0,0,0, "rectangles", "Rectangles", "Rectangles Splitter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Graph/Rectangles", /* category */
  0,   /* options */
  0    /* opts_ui_ */
};

/* OY_IMAGE_REGIONS_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_ROOT_REGISTRATION ------------------------------------------------*/


/** @func    oyraFilter_ImageRootCanHandle
 *  @brief   inform about image handling capabilities
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2008/07/10
 */
int    oyraFilter_ImageRootCanHandle ( oyCMMQUERY_e      type,
                                       uint32_t          value )
{
  int ret = -1;

  switch(type)
  {
    case oyQUERY_OYRANOS_COMPATIBILITY:
         ret = OYRANOS_VERSION; break;
    case oyQUERY_PIXELLAYOUT_CHANNELCOUNT:
         ret = 65535;
         break;
    case oyQUERY_PIXELLAYOUT_DATATYPE:
         switch(value) {
         case oyUINT8:
         case oyUINT16:
         case oyUINT32:
         case oyFLOAT:
         case oyDOUBLE:
              ret = 1; break;
         case oyHALF:
         default:
              ret = 0; break;
         }
         break;
    case oyQUERY_PIXELLAYOUT_SWAP_COLOURCHANNELS:
         ret = 1;
         break;
    case oyQUERY_PIXELLAYOUT_PLANAR:
         ret = 1;
         break;
    case oyQUERY_HDR:
         ret = 1;
         break;
    default: break;
  }

  return ret;
}

oyOptions_s* oyraFilter_ImageRootValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

#if 0
  if(!error)
    if(filter->image_ && filter->image_->layout_)
    {
      oyDATATYPE_e data_type = oyToDataType_m( filter->image_->layout_[0] );
      if(!(data_type == oyUINT8 ||
           data_type == oyUINT16 ||
           data_type == oyUINT32 ||
           data_type == oyHALF ||
           data_type == oyFLOAT ||
           data_type == oyDOUBLE
                                   ))
        error = 1;
    }
#endif

  *result = error;

  return 0;
}

/** @func    oyraFilterNode_ImageRootContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyPointer  oyraFilterNode_ImageRootContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

/** @func    oyraFilterPlug_ImageRootRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2008/10/05
 */
int      oyraFilterPlug_ImageRootRun ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int x = 0, y = 0, n = 0;
  int result = 0, error = 0;
  int is_allocated = 0;
  oyPointer * ptr = 0;
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyImage_s * image = (oyImage_s*)socket->data;

  /* Set a unknown output image dimension to something appropriate. */
  if(!ticket->output_image->width && !ticket->output_image->height)
  {
    ticket->output_image->width = image->width;
    ticket->output_image->height = image->height;
    oyImage_SetCritical( ticket->output_image, image->layout_[0], 0, 0 );
  }

  x = ticket->start_xy[0];
  y = ticket->start_xy[1];

  result = oyPixelAccess_CalculateNextStartPixel( ticket, requestor_plug);

  if(result != 0)
    return result;

  if(x < image->width &&
     y < image->height &&
     ticket->pixels_n)
  {
    n = ticket->pixels_n;
    if(n == 1)
      ptr = image->getPoint( image, x, y, 0, &is_allocated );

    result = !ptr;

  } else {

    /* adapt the rectangle of interesst to the new image dimensions */
    oyRectangle_s * new_roi = oyRectangle_NewFrom( ticket->output_image_roi, 0);
    double correct = ticket->output_image->width / (double) image->width;
    new_roi->width *= correct;
    new_roi->height *= correct;
    error = oyImage_FillArray( image, new_roi, 1, &ticket->array, 0, 0 );
    oyRectangle_Release( &new_roi );
  }

  return result;
}


oyConnectorImaging_s oyra_imageRoot_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Socket"},
  "//" OY_TYPE_STD "/image", /* connector_type */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
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
oyConnectorImaging_s * oyra_imageRoot_connectors[2] = {&oyra_imageRoot_connector,0};


#define OY_IMAGE_ROOT_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "root"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a source image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s   oyra_api7_image_root = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_output, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRootCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_ROOT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageRootRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  0,   /* plugs */
  0,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageRoot_connectors,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing a source image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/08 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyCMMapi4_s   oyra_api4_image_root = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_root, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRootCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_ROOT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  {oyOBJECT_NAME_S, 0,0,0, "image", "Root Image", "Root Image Filter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Image/Simple Image[in]", /* category */
  0,   /* options */
  0    /* opts_ui_ */
};


/* OY_IMAGE_ROOT_REGISTRATION ------------------------------------------------*/

/* OY_IMAGE_OUTPUT_REGISTRATION ----------------------------------------------*/

oyConnectorImaging_s oyra_imageOutput_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Plug"},
  "//" OY_TYPE_STD "/image", /* connector_type */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
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
oyConnectorImaging_s* oyra_imageOutput_connectors[2] = {&oyra_imageOutput_connector,0};


/** @func    oyraFilter_ImageOutputRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/19 (Oyranos: 0.1.8)
 *  @date    2008/10/03
 */
int      oyraFilterPlug_ImageOutputRun(oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterNode_s * node = 0;
  int result = 0;

  node = socket->node;

  /* to reuse the requestor_plug is a exception for the starting request */
  result = node->api7_->oyCMMFilterPlug_Run( requestor_plug, ticket );

  return result;
}


#define OY_IMAGE_OUTPUT_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "output"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a target image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s   oyra_api7_image_output = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_rectangles, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRootCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_OUTPUT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageOutputRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageOutput_connectors,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  0,   /* sockets */
  0,   /* sockets_n */
  0    /* sockets_last_add */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing a target image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/19 (Oyranos: 0.1.8)
 *  @date    2008/07/19
 */
oyCMMapi4_s   oyra_api4_image_output = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_output, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRootCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_OUTPUT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  {oyOBJECT_NAME_S, 0,0,0, "image_out", "Image[out]", "Output Image Filter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Image/Simple Image[out]", /* category */
  0,   /* options */
  0   /* opts_ui_ */
};

/* OY_IMAGE_OUTPUT_REGISTRATION ----------------------------------------------*/
/* ---------------------------------------------------------------------------*/



