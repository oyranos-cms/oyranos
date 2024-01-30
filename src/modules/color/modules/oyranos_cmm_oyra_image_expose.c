/** @file oyranos_cmm_oyra_image_expose.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    expose module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/04/11
 */

#include "oyCMMapi4_s.h"
#include "oyCMMapi7_s.h"
#include "oyCMMui_s.h"
#include "oyConnectorImaging_s.h"
#include "oyRectangle_s.h"
#include "oyRectangle_s_.h"

#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
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

/* OY_IMAGE_EXPOSE_REGISTRATION */



/* OY_IMAGE_EXPOSE_REGISTRATION ----------------------------------------------*/

void     oySensibleClip ( double * c, icColorSpaceSignature sig, int range_max, double expose )
{
  int max = 0, max_pos = 0,
      mid, mid_pos,
      min = range_max, min_pos = 0,
      i,
      n = oyICCColorSpaceGetChannelCount(sig);

  if(sig == icSigLabData ||
     sig == icSigYCbCrData)
    n = 1;

  for(i = 0; i < n; ++i)
  {
    if(max < c[i]) { max = c[i]; max_pos = i; }
    if(min > c[i]) { min = c[i]; min_pos = i; }
  }

  if( min * expose > range_max)
    for(i = 0; i < n; ++i)
      c[i] = range_max;
  else if(max * expose <= range_max)
    for(i = 0; i < n; ++i)
      c[i] *= expose;
  else if(n > 1)
  {
    double exposed_min = min * expose;
    double mid_part;
    double exposed_mid;

    mid_pos = min_pos != 0 && max_pos != 0 ? 0 : min_pos != 1 && max_pos != 1 ? 1 : 2;
    mid = c[mid_pos];

    mid_part = (double)( mid - min )/(double)( max - min );

    c[min_pos] = exposed_min + 0.5;
    exposed_mid = exposed_min + mid_part * (range_max - exposed_min);
    c[mid_pos] = exposed_mid + 0.5;
    c[max_pos] = range_max;
  }
}

/** @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/04
 *  @since   2013/06/10 (Oyranos: 0.9.5)
 */
int      oyraFilter_ImageExposeRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int result = 0, error = 0;
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s * input_node = 0,
                 * node = 0;
  oyFilterPlug_s * plug = 0;
  oyImage_s * image = 0;

  int dirty = 0;

  socket = oyFilterPlug_GetSocket( requestor_plug );
  node = oyFilterSocket_GetNode( socket );

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  if(!image)
  {
    result = 1;
    goto clean_expose1;
  }

  if(oy_debug)
    oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
              "image [%d](%d)\n",OY_DBG_ARGS_,oyStruct_GetId((oyStruct_s*)image),oyImage_GetWidth(image) );

  {
    oyRectangle_s * ticket_roi = oyPixelAccess_GetArrayROI( ticket );
    double  expose = 1.0;
    oyOptions_s * node_opts = oyFilterNode_GetOptions( node, 0 );

    if(!node_opts)
      dirty = 1;

    if(dirty)
    {
      result = dirty;
      goto clean_expose2;
    }

    plug = oyFilterNode_GetPlug( node, 0 );

    /* select node */
    input_node = oyFilterNode_GetPlugNode( node, 0 );

    /* find filters own expose factor */
    error = oyOptions_FindDouble( node_opts,
                                  "//" OY_TYPE_STD "/expose/expose",
                                  0, &expose );
    if(error) WARNc2_S("%s %d", _("found issues"),error);


    if(oy_debug > 2)
      oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                "%s expose: %f",OY_DBG_ARGS_, oyPixelAccess_Show(ticket), expose);

    if(expose != 1.0)
    {
      oyImage_s * output_image = oyPixelAccess_GetOutputImage( ticket );
      oyArray2d_s * array_out = oyPixelAccess_GetArray( ticket );
      oyProfile_s * p = oyImage_GetProfile( output_image );
      icColorSpaceSignature sig = oyProfile_GetSignature( p, oySIGNATURE_COLOR_SPACE );
      int layout_dst = oyImage_GetPixelLayout( output_image, oyLAYOUT );
      int channels_dst = oyToChannels_m( layout_dst );
      int byte_swap = oyToByteswap_m( layout_dst );
      int ticket_array_pix_width;

      /* avoid division by zero */
      if(!channels_dst) channels_dst = 1;

      ticket_array_pix_width = oyArray2d_GetWidth( array_out ) / channels_dst;

      {
        int w,h,x,y, i, start_x,start_y;
        unsigned int max = 1;
        oyRectangle_s * ticket_roi = oyPixelAccess_GetArrayROI( ticket );
        oyRectangle_s_  roi_= {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
        oyRectangle_s * roi = (oyRectangle_s*)&roi_;
        uint8_t ** array_out_data;
        /* get pixel layout infos for copying */
        oyDATATYPE_e data_type_out = oyToDataType_m( layout_dst );
        int bps_out = oyDataTypeGetSize( data_type_out );

        /* get the source pixels */
        result = oyFilterNode_Run( input_node, plug, ticket );

        /* get the channel buffers */
        array_out_data = oyArray2d_GetData( array_out );
        w = oyArray2d_GetWidth( array_out ) / channels_dst;
        h = oyArray2d_GetHeight( array_out );

        oyRectangle_SetByRectangle( roi, ticket_roi );
        oyRectangle_Scale( roi, ticket_array_pix_width );
        start_x = OY_ROUND(roi_.x);
        start_y = OY_ROUND(roi_.y);

        switch(data_type_out)
        {
          case oyUINT8: max = 255; break;
          case oyUINT16: max = 65535; break;
          case oyUINT32: max = UINT32_MAX; break;
          default: break;
        }

        /* expose the samples */
#if defined(USE_OPENMP)
#pragma omp parallel for private(x,y,i)
#endif
        for(y = start_y; y < h; ++y)
        {
          for(x = start_x; x < w; ++x)
          {
            if( (sig == icSigRgbData ||
                 sig == icSigXYZData ||
                 sig == icSigLabData ||
                 sig == icSigYCbCrData)
                && channels_dst >= 3)
            {
              double rgb[3], v;

              for(i = 0; i < 3; ++i)
              {
                switch(data_type_out)
                {
                case oyUINT8:
                  rgb[i] = array_out_data[y][x*channels_dst*bps_out + i*bps_out];
                  break;
                case oyUINT16:
                  {
                  uint16_t v = *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]);
                  if(byte_swap) v = oyByteSwapUInt16(v);
                  rgb[i] = v;
                  }
                  break;
                case oyUINT32:
                  {
                  uint32_t v = *((uint32_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]);
                  if(byte_swap) v = oyByteSwapUInt32(v);
                  rgb[i] = v;
                  }
                  break;
                case oyHALF:
                  v = *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]);
                  rgb[i] = v;
                  break;
                case oyFLOAT:
                  v = *((float*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]);
                  rgb[i] = v;
                  break;
                case oyDOUBLE:
                  v = *((double*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]);
                  rgb[i] = v;
                  break;
                }
              }

              oySensibleClip ( rgb, sig, max, expose );

              for(i = 0; i < 3; ++i)
              {
                v = rgb[i];
                switch(data_type_out)
                {
                case oyUINT8:
                  array_out_data[y][x*channels_dst*bps_out + i*bps_out] = v;
                  break;
                case oyUINT16:
                  { uint16_t u16 = v;
                  *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = byte_swap ? oyByteSwapUInt16(u16) : u16;
                  }
                  break;
                case oyUINT32:
                  { uint32_t u32 = v;
                  *((uint32_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = byte_swap ? oyByteSwapUInt16(u32) : u32;
                  }
                  break;
                case oyHALF:
                  *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = v;
                  break;
                case oyFLOAT:
                  *((float*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = v;
                  break;
                case oyDOUBLE:
                  *((double*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = v;
                  break;
                }
              }
            }
            else
            for(i = 0; i < channels_dst; ++i)
            {
              int v;
              switch(data_type_out)
              {
              case oyUINT8:
                v = array_out_data[y][x*channels_dst*bps_out + i*bps_out] * expose;
                if(v > 255) v = 255;
                array_out_data[y][x*channels_dst*bps_out + i*bps_out] = v;
                break;
              case oyUINT16:
                v = *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]);
                if(byte_swap) v = oyByteSwapUInt16(v);
                v *= expose;
                if(v > 65535) v = 65535;
                *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) = byte_swap ? oyByteSwapUInt16(v) : v;
                break;
              case oyUINT32:
                *((uint32_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) *= expose;
                break;
              case oyHALF:
                 *((uint16_t*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) *= expose;
                break;
              case oyFLOAT:
                *((float*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) *= expose;
                break;
              case oyDOUBLE:
                *((double*)&array_out_data[y][x*channels_dst*bps_out + i*bps_out]) *= expose;
                break;
              }
            }
          }
        }

      }

      oyArray2d_Release( &array_out );

      oyImage_Release( &output_image );
      oyProfile_Release( &p );
    } else /* expose == 1.0 */
    {
      result = oyFilterNode_Run( input_node, plug, ticket );
    }

    clean_expose2:
    oyOptions_Release( &node_opts );
    oyFilterPlug_Release( &plug );

    oyRectangle_Release( &ticket_roi );
    oyFilterNode_Release( &input_node );
  }

  clean_expose1:
  oyImage_Release( &image );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );

  return result;
}


#define OY_IMAGE_EXPOSE_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "expose"

/** @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a expose image filter
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/14 (Oyranos: 0.9.5)
 *  @date    2013/06/14
 */
oyCMMapi_s * oyraApi7ImageExposeCreate(void)
{
  oyCMMapi7_s * expose7;
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
  oyObject_SetNames( plug->oy_, "oyra7", __func__, OY_IMAGE_EXPOSE_REGISTRATION );
  oyObject_SetNames( socket->oy_, "oyra7", __func__, OY_IMAGE_EXPOSE_REGISTRATION );

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

  expose7 = oyCMMapi7_Create (         oyraCMMInit, oyraCMMReset, oyraCMMMessageFuncSet,
                                       OY_IMAGE_EXPOSE_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       oyraFilter_ImageExposeRun,
                                       (oyConnector_s**)plugs, 1, 0,
                                       (oyConnector_s**)sockets, 1, 0,
                                       0, 0 );
  oyObject_SetNames( expose7->oy_, "oyra7", __func__, OY_IMAGE_EXPOSE_REGISTRATION );
  return (oyCMMapi_s*) expose7;
}

const char * oyraApi4UiImageExposeGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "image_expose";
    else if(type == oyNAME_NAME)
      return _("Image[expose]");
    else if(type == oyNAME_DESCRIPTION)
      return _("Expose Image Filter Object");
  } else if(strcmp(select,"help") == 0)
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter adapts pixel brightness.");
    else if(type == oyNAME_DESCRIPTION)
    {
      static char * help_desc = NULL;
      if(!help_desc)
        oyStringAddPrintf( &help_desc, 0,0, "%s",
        _("The filter expects a \"expose\" double option and will process the data accordingly.")
         );
      return help_desc;
    }
  } else if(strcmp(select,"category") == 0)
  {
    if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return _("Image/Simple Image[expose]");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter is used to reduce pixels.");
  }
  return 0;
}


/** @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing a expose image filter
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/14 (Oyranos: 0.9.5)
 *  @date    2013/06/14
 */
oyCMMapi_s * oyraApi4ImageExposeCreate(void)
{
  static const char * oyra_api4_ui_image_expose_texts[] = {"name", "help", "category", 0};
  oyCMMui_s * ui = oyCMMui_Create( "Image/Simple Image[expose]", /* category */
                                   oyraApi4UiImageExposeGetText,
                                   oyra_api4_ui_image_expose_texts, 0 );
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C};

  oyCMMapi4_s * expose4 = oyCMMapi4_Create( oyraCMMInit, oyraCMMReset, oyraCMMMessageFuncSet,
                                       OY_IMAGE_EXPOSE_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       NULL,
                                       NULL,
                                       ui,
                                       NULL );
  oyObject_SetNames( expose4->oy_, "oyra4", __func__, OY_IMAGE_EXPOSE_REGISTRATION );
  return (oyCMMapi_s*)expose4;
}
/* OY_IMAGE_EXPOSE_REGISTRATION ----------------------------------------------*/
/* ---------------------------------------------------------------------------*/



