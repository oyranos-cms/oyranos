/** @file oyranos_cmm_oyra_image_channel.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    Channel selection module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/04/04
 */

#include "oyCMMapi4_s.h"
#include "oyCMMapi7_s.h"
#include "oyCMMui_s.h"
#include "oyConnectorImaging_s.h"
#include "oyRectangle_s.h"
#include "oyRectangle_s_.h"

#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
#include "oyranos_db.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_string.h"


#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_POSIX
#include <stdint.h>  /* UINT32_MAX */
#endif
#include <locale.h>

/* OY_IMAGE_CHANNEL_REGISTRATION */



/* OY_IMAGE_CHANNEL_REGISTRATION ----------------------------------------------*/


/** @brief   implement oyCMMFilter_GetNext_f()
 *
 *  The "channel" option is build of channel fields. It contains the
 *  output section in one text string each in squared brackets: "[a|b|c]".
 *  Each channel is separated by pipe sign '|' and can contain the channel
 *  symbol or a fill value. -1 indicates the module shall select a appropriate
 *  fill value. The counting of channels starts from a and ends with z,
 *  covering the range of ASCII a-z. A special case is a "" no op signature.
 *  Use it for pass through.
 *
 *  With the above syntax it is possible to add or remove channels or simply
 *  switch channels of.
 *
 *  switch the second and thierd channels of: ["a", -1, -1]
 *
 *  swap first with thierd channel: ["c", "b". "a"]
 *
 *  duplicate the second channel and skip the first and possible the c and 
 *  more source channels: ["b", "b"]
 *
 *  Note: changing the channel count might require a new ICC profile for the
 *  output image. Please setup the graph accordingly.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/04
 *  @since   2016/04/04 (Oyranos: 0.9.6)
 */
int      oyraFilter_ImageChannelRun  ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int result = 0, error = 0;
  oyFilterSocket_s * socket;
  oyFilterNode_s * input_node = 0,
                 * node;
  oyFilterPlug_s * plug = NULL;
  oyImage_s * image;

  int dirty = 0;

  socket = oyFilterPlug_GetSocket( requestor_plug );
  node = oyFilterSocket_GetNode( socket );

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  if(!image)
  {
    result = 1;
    goto oyraFilter_ImageChannelRun_clean;
  }

  if(oy_debug)
    oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
              "image [%d](%d)\n",OY_DBG_ARGS_,oyStruct_GetId((oyStruct_s*)image),oyImage_GetWidth(image) );

  {
    const char * channels_json;
    oyOptions_s * node_opts = oyFilterNode_GetOptions( node, 0 );
    oyjl_val json = 0;

    if(!node_opts)
      dirty = 1;

    if(dirty)
    {
      result = dirty;
      goto oyraFilter_ImageChannelRun_clean2;
    }

    plug = oyFilterNode_GetPlug( node, 0 );

    /* select node */
    input_node = oyFilterNode_GetPlugNode( node, 0 );

    /* find filters own channel factor */
    channels_json = oyOptions_FindString( node_opts,
                                  "//" OY_TYPE_STD "/channel/channel",
                                  0 );
    oyOptions_Release( &node_opts );
    error = !channels_json;
    if(error) {WARNc_S("found not \"channel\" option for filter");}
    else if(oy_debug) 
      oyra_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                "channels_json: \"%s\"",OY_DBG_ARGS_, channels_json);

    if(!error && strlen(channels_json))
    {
      char * save_locale = 0;
      /* sensible parsing */
      save_locale = oyStringCopy_( setlocale( LC_NUMERIC, 0 ),
                                         oyAllocateFunc_ );
      setlocale( LC_NUMERIC, "C" );
      json = oyJsonParse( channels_json, NULL );
      if(!json)
      {
        WARNc1_S( "channel option: %s\n", _("found issues parsing JSON") );
        error = 1;
      }

      setlocale(LC_NUMERIC, save_locale);
      if(save_locale)
        oyFree_m_( save_locale );
    }

    if(oy_debug > 2)
      oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                "%s",OY_DBG_ARGS_, oyPixelAccess_Show(ticket));

    if(channels_json && strlen(channels_json) > 2)
    {
      oyImage_s * output_image = oyPixelAccess_GetOutputImage( ticket );
      oyArray2d_s * a_dest = oyPixelAccess_GetArray( ticket );
      int layout_src = oyImage_GetPixelLayout( image, oyLAYOUT );
      int layout_dst = oyImage_GetPixelLayout( output_image, oyLAYOUT );
      int channels_src = oyToChannels_m( layout_src );
      int channels_dst = oyToChannels_m( layout_dst );
      int ticket_array_pix_width;
      int count = oyjlValueCount( json ), i;
      const int max_channels = 'z'-'a'+1;
      double  channel[max_channels+1];
      int channel_pos[max_channels+1];

      /* avoid division by zero */
      if(!channels_src) channels_src = 1;
      if(!channels_dst) channels_dst = 1;

      ticket_array_pix_width = oyArray2d_GetWidth( a_dest ) / channels_dst;

      memset( channel, 0, sizeof(double) * (max_channels+1) );
      memset( channel_pos, 0, sizeof(int) * (max_channels+1) );

      if(count > channels_dst)
      {
        WARNc3_S( "\"channel=%s\" option channel count %d exceeds destination image %d", channels_json, count, channels_dst );
        error = 1;
      }

      /* parse the "channel" option as JSON string */
      if(!error)
      for(i = 0; i < count && !error; ++i)
      {
        oyjl_val v = oyjlValuePosGet( json, i );
        if( OYJL_IS_NUMBER(v) ||
            OYJL_IS_DOUBLE(v) )
        {
          channel[i] = OYJL_GET_DOUBLE( v );
          if(channel[i] == -1)
            channel[i] = 0.5;
          channel_pos[i] = -1;
        } else if( OYJL_IS_STRING( v ) )
        {
          const char * p = OYJL_GET_STRING( v );
          channel_pos[i] = p[0] - 'a';
          if(channel_pos[i] >= channels_src)
          {
            WARNc2_S( "channel position %d not available in source image %d", channel_pos[i], channels_src );
            error = 1;
          }
        }
      }
      oyjlTreeFree( json );

      if(!error)
      {
        int w,h,x,y, start_x,start_y, max_value = -1;
        oyRectangle_s * ticket_roi = oyPixelAccess_GetArrayROI( ticket );
        oyRectangle_s_  roi_= {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
        oyRectangle_s * roi = (oyRectangle_s*)&roi_;
        oyArray2d_s * array_out;
        uint8_t ** array_out_data;
        /* get pixel layout infos for copying */
        oyDATATYPE_e data_type_out = oyToDataType_m( layout_dst );
        int bps_out = oyDataTypeGetSize( data_type_out );

        /* get the source pixels */
        result = oyFilterNode_Run( input_node, plug, ticket );

        /* get the channel buffers */
        array_out = oyPixelAccess_GetArray( ticket );
        array_out_data = oyArray2d_GetData( array_out );
        w = oyArray2d_GetWidth( array_out ) / channels_dst;
        h = oyArray2d_GetHeight( array_out );
        switch(data_type_out)
        {
        case oyUINT8:
             max_value = 255;
             break;
        case oyUINT16:
             max_value = 65535;
             break;
        case oyUINT32:
             max_value = UINT32_MAX;
             break;
        case oyHALF:
        case oyFLOAT:
        case oyDOUBLE:
             max_value = 1.0;
             break;
        }

        oyRectangle_SetByRectangle( roi, ticket_roi );
        oyRectangle_Scale( roi, ticket_array_pix_width );
        start_x = OY_ROUND(roi_.x);
        start_y = OY_ROUND(roi_.y);

        /* copy the channels */
#if defined(USE_OPENMP)
#pragma omp parallel for private(x,y,i)
#endif
        for(y = start_y; y < h; ++y)
        {
          for(x = start_x; x < w; ++x)
          {
            union u8421 { uint32_t u4; uint16_t u2; uint8_t u1; float f; double d; };
            union u8421 cache[max_channels];
            float flt;
            uint32_t u4;
            
            /* fill the intermediate pixel cache;
             * It is not known which channels are needed and in which order.
             * Thus all channels are stored outside the main buffer.
             */
            for(i = 0; i < count; ++i)
            {
              int pos = (channel_pos[i] == -1) ? i : channel_pos[i];
              switch(data_type_out)
              {
              case oyUINT8:
                cache[i].u1 = (channel_pos[i] == -1) ? OY_ROUND(channel[i] * max_value) : array_out_data[y][x*channels_dst*bps_out + pos*bps_out];
                break;
              case oyUINT16:
                cache[i].u2 = (channel_pos[i] == -1) ? OY_ROUND(channel[i] * max_value) : *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + pos*bps_out]);
                break;
              case oyUINT32:
                cache[i].u4 = (channel_pos[i] == -1) ? (uint32_t) OY_ROUND(channel[i] * max_value) : *((uint32_t*)&array_out_data[y][x*channels_dst*bps_out + pos*bps_out]);
                break;
              case oyHALF:
                flt = channel[i] * max_value;
                memcpy( &u4, &flt, 4 );
                cache[i].u2 = (channel_pos[i] == -1) ? OY_FLOAT2HALF(u4) : *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + pos*bps_out]);
                break;
              case oyFLOAT:
                cache[i].f = (channel_pos[i] == -1) ? channel[i] * max_value : *((float*)&array_out_data[y][x*channels_dst*bps_out + pos*bps_out]);
                break;
              case oyDOUBLE:
                cache[i].d = (channel_pos[i] == -1) ? channel[i] * max_value : *((double*)&array_out_data[y][x*channels_dst*bps_out + pos*bps_out]);
                break;
              }
            }

            /* read back all scattered channels */
            for(i = 0; i < count; ++i)
            {
              int pos = i;
              switch(data_type_out)
              {
              case oyUINT8:
                array_out_data[y][x*channels_dst*bps_out + i*bps_out] = cache[pos].u1;
                break;
              case oyUINT16:
                *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = cache[pos].u2;
                break;
              case oyUINT32:
                *((uint32_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = cache[pos].u4;
                break;
              case oyHALF:
                 *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = cache[pos].u2;
                break;
              case oyFLOAT:
                *((float*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = cache[pos].f;
                break;
              case oyDOUBLE:
                *((double*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = cache[pos].d;
                break;
              }
            }
          }
        }

        oyArray2d_Release( &array_out );
      }
      oyImage_Release( &output_image );
    } else /* nothing to do */
      result = oyFilterNode_Run( input_node, plug, ticket );

    oyraFilter_ImageChannelRun_clean2:
    oyFilterPlug_Release( &plug );
    oyFilterNode_Release( &input_node );
  }
  oyraFilter_ImageChannelRun_clean:
  oyImage_Release( &image );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );

  return result;
}


#define OY_IMAGE_CHANNEL_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "channel"

/** @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a channel image filter
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/04
 *  @since   2016/04/04 (Oyranos: 0.9.6)
 */
oyCMMapi_s * oyraApi7ImageChannelCreate(void)
{
  oyCMMapi7_s * channel7;
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C};
  static oyDATATYPE_e data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                       oyHALF, oyFLOAT, oyDOUBLE, 0};
  oyConnectorImaging_s * plug = oyConnectorImaging_New(0),
                       * socket = oyConnectorImaging_New(0);
  static oyConnectorImaging_s * plugs[2] = {0,0},
                              * sockets[2] = {0,0};
  plugs[0] = plug;
  sockets[0] = socket;
  oyObject_SetNames( plug->oy_, "oyra7", __func__, OY_IMAGE_CHANNEL_REGISTRATION );
  oyObject_SetNames( socket->oy_, "oyra7", __func__, OY_IMAGE_CHANNEL_REGISTRATION );

  oyConnectorImaging_SetDataTypes( plug, data_types, 6 );
  oyConnectorImaging_SetReg( plug, "//" OY_TYPE_STD "/manipulator.data" );
  oyConnectorImaging_SetMatch( plug, oyFilterSocket_MatchImagingPlug );
  oyConnectorImaging_SetTexts( plug, oyCMMgetImageConnectorPlugText,
                               oy_image_connector_texts );
  oyConnectorImaging_SetIsPlug( plug, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET, -1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT, 255 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT, 255 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_ID, 1 );
                               

  oyConnectorImaging_SetDataTypes( socket, data_types, 6 );
  oyConnectorImaging_SetReg( socket, "//" OY_TYPE_STD "/manipulator.data" );
  oyConnectorImaging_SetMatch( socket, oyFilterSocket_MatchImagingPlug );
  oyConnectorImaging_SetTexts( socket, oyCMMgetImageConnectorSocketText,
                               oy_image_connector_texts );
  oyConnectorImaging_SetIsPlug( socket, 0 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET, -1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT, 255 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT, 255 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_ID, 1 );

  channel7 = oyCMMapi7_Create (        oyraCMMInit, oyraCMMReset, oyraCMMMessageFuncSet,
                                       OY_IMAGE_CHANNEL_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       oyraFilter_ImageChannelRun,
                                       (oyConnector_s**)plugs, 1, 0,
                                       (oyConnector_s**)sockets, 1, 0,
                                       0, 0 );
  oyObject_SetNames( channel7->oy_, "oyra7", __func__, OY_IMAGE_CHANNEL_REGISTRATION );
  return (oyCMMapi_s*) channel7;
}

const char * oyraApi4UiImageChannelGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "image_channel";
    else if(type == oyNAME_NAME)
      return _("Image[channel]");
    else if(type == oyNAME_DESCRIPTION)
      return _("Channel Image Filter Object");
  } else if(strcmp(select,"help") == 0)
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter is used to reduce pixels.");
    else if(type == oyNAME_DESCRIPTION)
    {
      static char * help_desc = NULL;
      if(!help_desc)
        oyStringAddPrintf( &help_desc, 0,0, "%s\n",
        _("The filter will expect a \"channel\" double option and will create, fill and process a according data version with a new job ticket. The new job tickets image, array and output_array_roi will be divided by the supplied \"channel\" factor. It's plug will request the divided image sizes from the source socket.") );
      return help_desc;
    }
  } else if(strcmp(select,"category") == 0)
  {
    if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return _("Image/Simple Image[channel]");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter is used to reduce pixels.");
  }
  return 0;
}


/** @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing a channel image filter
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/04
 *  @since   2016/04/04 (Oyranos: 0.9.6)
 */
oyCMMapi_s * oyraApi4ImageChannelCreate(void)
{
  static const char * oyra_api4_ui_image_channel_texts[] = {"name", "help", "category", 0};
  oyCMMui_s * ui = oyCMMui_Create( "Image/Simple Image[channel]", /* category */
                                   oyraApi4UiImageChannelGetText,
                                   oyra_api4_ui_image_channel_texts, 0 );
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C};

  oyCMMapi4_s * channel4 = oyCMMapi4_Create( oyraCMMInit, oyraCMMReset, oyraCMMMessageFuncSet,
                                       OY_IMAGE_CHANNEL_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       NULL,
                                       NULL,
                                       ui,
                                       NULL );
  oyObject_SetNames( channel4->oy_, "oyra4", __func__, OY_IMAGE_CHANNEL_REGISTRATION );
  return (oyCMMapi_s*)channel4;
}
/* OY_IMAGE_CHANNEL_REGISTRATION ----------------------------------------------*/
/* ---------------------------------------------------------------------------*/



