/** @file oyranos_conversion.c

   [Template file inheritance graph]
   +-- oyranos_conversion.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#include "oyConnectorImaging_s_.h"
#include "oyConversion_s.h"
#include "oyFilterGraph_s.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterSocket_s_.h"
#include "oyImage_s_.h"
#include "oyPixelAccess_s_.h"

#include "oyranos_conversion.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_io.h"
#include "oyranos_module_internal.h"
#include "oyranos_object_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_types.h"

char * oyGetPSViewer()
{
  /* Linux */
  char * ps_viewer = oyFindApplication("xdg-open");

  /* OS X */
  if(!ps_viewer)
    ps_viewer = oyFindApplication("open");

  /* Ghostview */
  if(!ps_viewer)
  {
    ps_viewer = oyFindApplication("gv");
    if(ps_viewer)
    {
      oyFree_m_(ps_viewer);
      ps_viewer = strdup("gv -spartan -antialias");
    }
  }

  return ps_viewer;
}
void oyShowGraph__( oyFilterGraph_s * s )
{
  char * ptr = 0;
  int error = 0;
  char * ps_viewer = 0,
       * command = 0;

  oyFilterGraph_s * adjacency_list = s;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return )
  /*return;*/

  ptr = oyFilterGraph_ToText( adjacency_list, 0, 0,
                              "Oyranos Test Graph", 0, malloc );
  oyWriteMemToFile_( "test.dot", ptr, strlen(ptr) );

  ps_viewer = oyGetPSViewer(),

  oyStringAddPrintf_( &command, oyAllocateFunc_, oyDeAllocateFunc_,
                      "dot -Tps test.dot -o test.ps; %s test.ps &",
                      ps_viewer );

  error = system(command);
  if(error)
    WARNc2_S("error during calling \"%s\": %d", error, command);

  free(ptr); ptr = 0;
  oyFree_m_(ps_viewer);
  oyFree_m_(command);
}
void oyShowGraph_( oyFilterNode_s * s, const char * selector )
{
  oyFilterGraph_s * adjacency_list = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return )
  /*return;*/

  adjacency_list = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( adjacency_list, s, selector, 0 );

  oyShowGraph__(adjacency_list);

  oyFilterGraph_Release( &adjacency_list );
}
void               oyShowConversion_ ( oyConversion_s    * conversion,
                                       uint32_t            flags )
{
  char * ptr = 0, * t = 0, * t2 = 0, * command = 0;
  int error = 0;
  oyConversion_s * s = conversion;
  char * ps_viewer = NULL;
  oyCheckType__m( oyOBJECT_CONVERSION_S, return )
  /*return;*/

  ps_viewer = oyGetPSViewer();

  ptr = oyConversion_ToText( s, "Conversion Graph", 0, oyAllocateFunc_ );

  oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                      "test-%d.dot",
                      oyStruct_GetId( (oyStruct_s*) conversion ) );
  oyStringAddPrintf_( &t2, oyAllocateFunc_, oyDeAllocateFunc_,
                      "test-%d.ps",
                      oyStruct_GetId( (oyStruct_s*) conversion ) );

  oyWriteMemToFile_( t, ptr, strlen(ptr) );
  if(!(flags & 0x01))
  {
    STRING_ADD( command, "dot -Tps ");
    STRING_ADD( command, t );
    STRING_ADD( command, " -o ");
    STRING_ADD( command, t2 );
    STRING_ADD( command, "; ");
    STRING_ADD( command, ps_viewer);
    STRING_ADD( command, " ");
    STRING_ADD( command, t2 );
    STRING_ADD( command, " &");
  } else
  {
    STRING_ADD( command, "dot -Tps ");
    STRING_ADD( command, t );
    STRING_ADD( command, " -o ");
    STRING_ADD( command, t2 );
    STRING_ADD( command, " &");
  }
  error = system(command);
  if(error)
    WARNc2_S("error during calling \"%s\": %d", command, error);

  oyFree_m_(ptr);
  oyFree_m_(t);
  oyFree_m_(t2);
  oyFree_m_(ps_viewer);
  oyFree_m_(command);
}


/** Function oyFilterSocket_MatchImagingPlug
 *  @brief   verify connectors matching each other
 *  @ingroup module_api
 *  @memberof oyConnectorImaging_s
 *
 *  @param         socket              a filter socket
 *  @param         plug                a filter plug
 *  @return                            1 on success, otherwise 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/20 (Oyranos: 0.1.10)
 *  @date    2009/04/28
 */
int          oyFilterSocket_MatchImagingPlug (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug )
{
  int match = 0;
  oyConnectorImaging_s * a = 0,  * b = 0;
  oyImage_s * image = 0;
  int colors_n = 0, n, i, j;
  oyDATATYPE_e data_type = 0;

  if(socket && socket->type_ == oyOBJECT_FILTER_SOCKET_S)
    a = (oyConnectorImaging_s*)oyFilterSocket_GetPattern(socket);

  if(plug && plug->type_ == oyOBJECT_FILTER_PLUG_S)
    b = (oyConnectorImaging_s*) oyFilterPlug_GetPattern(plug);

  if(a && b)
  {
    oyPixel_t layout;
    match = 1;
    image = (oyImage_s*) oyFilterSocket_GetData(socket);
    layout = oyImage_GetPixelLayout(image, oyLAYOUT);

    if(!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_IS_PLUG))
      match = 0;

    /** For a zero set pixel layout we skip most tests and assume it will be
        checked later. */
    if(image && layout && match)
    {
      /* channel counts */
      int coff = oyImage_GetPixelLayout(image, oyCOFF);      
      oyProfile_s * image_profile = oyImage_GetProfile( image );
      colors_n = oyProfile_GetChannelsCount( image_profile );
      oyProfile_Release( &image_profile );
      if(oyChannels_m(layout) < (size_t)oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT) ||
         oyChannels_m(layout) > (size_t)oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT) ||
         colors_n < oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT) ||
         colors_n > oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT))
        match = 0;

      /* data types */
      if(match)
      {
        const oyDATATYPE_e * data_types = 0;
        int data_types_n = 0;
        oyConnectorImaging_GetDataTypes(b, &data_types, &data_types_n);
        data_type = oyToDataType_m( layout );
        n = data_types_n;
        match = 0;
        for(i = 0; i < n; ++i)
          if(data_types[i] == data_type)
            match = 1;
      }

      /* planar and interwoven capabilities */
      if(oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET) < coff ||
         (!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_PLANAR) && oyToPlanar_m(layout)) ||
         (!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN) && !oyToPlanar_m(layout)))
        match = 0;

      /* swap and byteswapping capabilities */
      if((!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_SWAP) && oyToSwapColorChannels_m(layout)) ||
         (!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_SWAP_BYTES) && oyToByteswap_m(coff)))
        match = 0;

      /* revert or chockolat and vanilla */
      if((!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_REVERT) && oyToFlavor_m(coff)))
        match = 0;

      /* channel types */
      if(match)
      {
        const oyCHANNELTYPE_e * channel_types = 0;
        int channel_types_n = 0;
        oyConnectorImaging_GetChannelTypes( b, &channel_types,
                                            &channel_types_n );
        n = oyChannels_m(layout);
        for(i = 0; i < channel_types_n; ++i)
        {
          match = 0;
          for(j = 0; j < n; ++j)
          {
            oyCHANNELTYPE_e channel_type = oyImage_GetChannelType( image, j );

            if(channel_types[i] == channel_type &&
               !(!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA) &&
                 channel_type == oyCHANNELTYPE_COLOR_LIGHTNESS) &&
               !(!oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA) &&
                 channel_type == oyCHANNELTYPE_COLOR_LIGHTNESS_PREMULTIPLIED))
              match = 1;
          }
          if(!match)
            break;
        }
      }

      /* subpixels */
      if(oyImage_GetSubPositioning(image) && !oyConnectorImaging_GetCapability(b,oyCONNECTOR_IMAGING_CAP_CAN_SUBPIXEL))
        match = 0;
    }
  }

  oyImage_Release( &image );
  oyConnectorImaging_Release( &a );
  oyConnectorImaging_Release( &b );

  return match;
}

/** Function  oyFilterPlug_ResolveImage
 *  @memberof oyFilterPlug_s
 *  @brief    Resolve processing data during a filter run
 *
 *  The function is a convenience function to use inside a filters
 *  oyCMMFilterPlug_Run_f call. The function makes only sense for non root
 *  filters.
 *
 *  @param[in,out] plug                the filters own plug
 *  @param[in,out] socket              the filters own socket
 *  @param[in,out] ticket              the actual ticket
 *  @return                            the input image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/01 (Oyranos: 0.1.10)
 *  @date    2009/05/01
 */
OYAPI oyImage_s * OYEXPORT
             oyFilterPlug_ResolveImage(oyFilterPlug_s    * plug,
                                       oyFilterSocket_s  * socket,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterPlug_s_ ** plug_ = (oyFilterPlug_s_**)&plug;
  oyFilterSocket_s_ ** socket_ = (oyFilterSocket_s_**)&socket;
  oyPixelAccess_s_ ** ticket_ = (oyPixelAccess_s_**)&ticket;

  int error = !plug || !(*plug_)->remote_socket_ ||
              !ticket ||
              !socket || !(*socket_)->node;
  oyImage_s_ * image_input_ = 0;
  oyImage_s  * image = 0;
  oyFilterNode_s_ * input_node_ = 0;
  oyFilterNode_s_ * node = (*socket_)->node;
  oyPixel_t pixel_layout = 0;
  oyOptions_s * options = 0,
              * requests = 0,
              * ticket_orig;
  int32_t n = 0;

  if(error)
    return 0;

  image_input_ = (oyImage_s_*)oyImage_Copy( (oyImage_s*)(*plug_)->remote_socket_->data, 0 );
  input_node_ = (oyFilterNode_s_*)(*plug_)->remote_socket_->node;

  if(!image_input_)
  {
    /* get options */
    options = oyFilterNode_GetOptions( (oyFilterNode_s*)node, 0 );

    /* store original queue */
    ticket_orig = (*ticket_)->request_queue;
    (*ticket_)->request_queue = 0;

    /* select only resolve requests */
    error = oyOptions_Filter( &requests, &n, 0,
                              oyBOOLEAN_INTERSECTION,
                              "////resolve", options );
    oyOptions_Release( &options );

    /* combine old queue and requests from the current node */
    oyOptions_CopyFrom( &(*ticket_)->request_queue, requests, oyBOOLEAN_UNION, 0,0);
    oyOptions_CopyFrom( &(*ticket_)->request_queue, ticket_orig, oyBOOLEAN_UNION,
                        0, 0 );

    /* filter again, (really needed?) */
    oyOptions_Filter( &(*ticket_)->request_queue, &n, 0,
                      oyBOOLEAN_INTERSECTION, "////resolve", requests );
    oyOptions_Release( &requests );

    /* try to obtain the processing data from a generator filter */
    input_node_->api7_->oyCMMFilterPlug_Run( (oyFilterPlug_s*)(node->plugs[0]), ticket );
    image_input_ = (oyImage_s_*)oyImage_Copy( (oyImage_s*)(*plug_)->remote_socket_->data, 0 );

    /* clean up the queue */
    oyOptions_Release( &(*ticket_)->request_queue );

    /* restore old queue */
    (*ticket_)->request_queue = ticket_orig; ticket_orig = 0;

    if(!image_input_)
      return 0;
  }

  if(!(*socket_)->data)
  {
    /* Copy a root image or link to a non root image. */
    if(!(*plug_)->remote_socket_->node->api7_->plugs_n)
    {
      options = oyFilterNode_GetOptions( (oyFilterNode_s*)node, 0 );
      error = oyOptions_Filter( &requests, &n, 0,
                                oyBOOLEAN_INTERSECTION,
                                "////resolve", options );
      oyOptions_Release( &options );
      oyOptions_CopyFrom( &requests, (*ticket_)->request_queue,oyBOOLEAN_UNION,0,0);

      error = oyOptions_FindInt( requests, "pixel_layout", 0,
                                 (int32_t*)&pixel_layout );
      oyOptions_Release( &requests );

      if(error == 0)
      {
        /* possibly complete the pixel layout information */
        int n = oyToChannels_m( pixel_layout );
        int cchan_n = oyProfile_GetChannelsCount( image_input_->profile_ );
        oyPixel_t layout = oyDataType_m( oyToDataType_m(pixel_layout) ) |
                           oyChannels_m( OY_MAX(n, cchan_n) );
        /* create a new image */
        image = oyImage_Create( image_input_->width, image_input_->height,
                                0, layout,
                                image_input_->profile_, node->oy_ );

      } else
      {
        image = oyImage_Copy( (oyImage_s*) image_input_, node->oy_ );
      }


    } else
    {
      image = oyImage_Copy( (oyImage_s*) image_input_, 0 );
    }

    error = oyFilterNode_SetData( (oyFilterNode_s*)node, (oyStruct_s*)image, 0, 0 );
    oyImage_Release( &image );
  }

  if(!(*ticket_)->output_image)
  {
    (*ticket_)->output_image = oyImage_Copy( (oyImage_s*) (*socket_)->data, 0 );
  }

  oyOptions_Release( &requests );

  return (oyImage_s*)image_input_;
}

