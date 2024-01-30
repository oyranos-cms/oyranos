/** @file oyranos_cmm_oyra_image.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2013-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    modules for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2013/06/10
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

/* OY_IMAGE_SCALE_REGISTRATION */



/* OY_IMAGE_SCALE_REGISTRATION ----------------------------------------------*/


/** @func    oyraFilter_ImageScaleRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/04
 *  @since   2013/06/10 (Oyranos: 0.9.5)
 */
int      oyraFilter_ImageScaleRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int result = 0, error = 0;
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s * input_node = 0,
                 * node = 0;
  oyFilterPlug_s * plug = 0;
  oyImage_s * image = 0;
  int image_width;

  int dirty = 0;

  socket = oyFilterPlug_GetSocket( requestor_plug );
  node = oyFilterSocket_GetNode( socket );

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  if(!image)
  {
    result = 1;
    goto clean_scale1;
  }

  image_width = oyImage_GetWidth(image);
  if(oy_debug)
    oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
              "image [%d](%d)\n",OY_DBG_ARGS_,oyStruct_GetId((oyStruct_s*)image),oyImage_GetWidth(image) );

  {
    oyRectangle_s_  ticket_roi_pix_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
    oyRectangle_s * ticket_roi_pix = (oyRectangle_s*)&ticket_roi_pix_;
    double  scale = 1.0;
    oyOptions_s * node_opts = oyFilterNode_GetOptions( node, 0 );

    if(!node_opts)
      dirty = 1;

    if(dirty)
    {
      result = dirty;
      goto clean_scale2;
    }

    plug = oyFilterNode_GetPlug( node, 0 );

    /* select node */
    input_node = oyFilterNode_GetPlugNode( node, 0 );

    /* find filters own scale factor */
    error = oyOptions_FindDouble( node_opts,
                                  "//" OY_TYPE_STD "/scale/scale",
                                  0, &scale );
    if(error) WARNc2_S("%s %d", _("found issues"),error);


    oyPixelAccess_RoiToPixels( ticket, NULL, &ticket_roi_pix );

    if(oy_debug > 2)
      oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                "%s",OY_DBG_ARGS_, oyPixelAccess_Show(ticket));

    if(scale != 1.0 && scale > 0)
    {
      oyImage_s * output_image = oyPixelAccess_GetOutputImage( ticket );
      int output_image_width = oyImage_GetWidth( output_image );
      oyRectangle_s_  new_ticket_array_roi_pix_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
      oyRectangle_s * new_ticket_array_roi = NULL,
                    * new_ticket_array_roi_pix = (oyRectangle_s*)&new_ticket_array_roi_pix_,
                    * image_pix = oyRectangle_NewWith( 0,0,+
                                     oyImage_GetWidth(image),
                                     oyImage_GetHeight(image), 0);
      oyPixelAccess_s * new_ticket = 0;
      /* start_xy is defined relative to the tickets output image width */
      double start_x_src_pixel = oyPixelAccess_GetStart( ticket, 0 )
                                 * output_image_width,
             start_y_src_pixel = oyPixelAccess_GetStart( ticket, 1 )
                                 * output_image_width,
             start_x_dst_pixel,start_y_dst_pixel;
      int layout_src = oyImage_GetPixelLayout( image, oyLAYOUT ),
          layout_dst = oyImage_GetPixelLayout( output_image, oyLAYOUT );
      int channels_src = oyToChannels_m( layout_src );
      int channels_dst = oyToChannels_m( layout_dst );

      /* avoid division by zero */
      if(!channels_src) channels_src = 1;
      if(!channels_dst) channels_dst = 1;

      new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
      oyPixelAccess_SetArray( new_ticket, 0, 0 );
      oyPixelAccess_SetOutputImage( new_ticket, image );

      if(oy_debug)
      {
        oyArray2d_s * a_dest = oyPixelAccess_GetArray( ticket );
        int a_width_dest = oyArray2d_GetWidth( a_dest ) / channels_dst;
        oyra_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
             "output_image [%d](%d*%d)-array[%d](w%d) image [%d](%d*%d)\n",
             OY_DBG_ARGS_,
             oyStruct_GetId((oyStruct_s*)output_image),
             oyImage_GetWidth(output_image),channels_dst,
             oyStruct_GetId((oyStruct_s*)a_dest), a_width_dest,
             oyStruct_GetId((oyStruct_s*)image),
             oyImage_GetWidth(image), channels_src );
        oyArray2d_Release( &a_dest );
      }

      oyPixelAccess_RoiToPixels( ticket, NULL, &new_ticket_array_roi_pix );

        /* scale */
      oyRectangle_Scale( new_ticket_array_roi_pix, 1.0/scale );
      oyRectangle_Round( new_ticket_array_roi_pix );
        /* convert to new_ticket relative dimensions */
      oyPixelAccess_PixelsToRoi( new_ticket, new_ticket_array_roi_pix,
                                 &new_ticket_array_roi );

      /* adapt the access start and write relative to new tickets image width */
      start_x_dst_pixel = OY_ROUND(start_x_src_pixel / scale);
      start_y_dst_pixel = OY_ROUND(start_y_src_pixel / scale);
      oyPixelAccess_ChangeRectangle( new_ticket,
                                     start_x_dst_pixel / image_width,
                                     start_y_dst_pixel / image_width,
                                     new_ticket_array_roi );

      if(oy_debug)
        oyMessageFunc_p( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
              "ticket: %s image[%d](%d) -> [%d](%d) scale %f\n",OY_DBG_ARGS_,
                   oyPixelAccess_Show( ticket ),
                   oyStruct_GetId((oyStruct_s*)image),oyImage_GetWidth(image),
                   oyStruct_GetId((oyStruct_s*)output_image),oyImage_GetWidth(output_image), scale );

      if(oy_debug)
      {
        char * troi;
        troi = strdup( oyRectangle_Show(ticket_roi_pix) );
        oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                  "ticket_roi_pix: %s  %s %f  new_ticket_array_roi_pix: %s",OY_DBG_ARGS_,
                  troi, "scale factor:", scale,
                  oyRectangle_Show(new_ticket_array_roi_pix) );
        if(troi) free(troi);

        oyra_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                  "image_pix: %s start_x:%g start_y:%g",OY_DBG_ARGS_,
                  oyRectangle_Show(image_pix),
                  oyPixelAccess_GetStart( new_ticket, 0 )*image_width,
                  oyPixelAccess_GetStart( new_ticket, 1 )*image_width );
      }


      if(oyRectangle_CountPoints(  new_ticket_array_roi ) > 0)
      {
        int nw,nh,w,h,x,y,xs,ys;
        oyArray2d_s * array_in,
                    * array_out;
        uint8_t ** array_in_data,
                ** array_out_data;
        /* get pixel layout infos for copying */
        oyDATATYPE_e data_type_in = oyToDataType_m( layout_src ),
                     data_type_out = oyToDataType_m( layout_dst );
        int bps_in = oyDataTypeGetSize( data_type_in ),
            bps_out = oyDataTypeGetSize( data_type_out );
        int issue = 0;

        /* get the source pixels */
          if(oy_debug > 2)
            oyra_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                     "%s %s",OY_DBG_ARGS_,
                     "Run new_ticket",
                     oyPixelAccess_Show( new_ticket ) );
        result = oyFilterNode_Run( input_node, plug, new_ticket );

        /* prepare the current ticket */
        oyPixelAccess_SetArrayFocus( ticket, 0 );

        /* get the channel buffers */
        array_in = oyPixelAccess_GetArray( new_ticket );
        array_out = oyPixelAccess_GetArray( ticket );
        array_in_data  = oyArray2d_GetData( array_in );
        array_out_data = oyArray2d_GetData( array_out );
        w  = oyArray2d_GetWidth ( array_out ) / channels_dst;
        h  = oyArray2d_GetHeight( array_out );
        nw = oyArray2d_GetWidth ( array_in ) / channels_src;
        nh = oyArray2d_GetHeight( array_in );

        if(nw < (int)OY_ROUND(w/scale)) issue |= 1;
        if(nh < (int)OY_ROUND(h/scale)) issue |= 2;
        if(issue || oy_debug > 2)
        {
          oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                "ticket: %s",OY_DBG_ARGS_, oyPixelAccess_Show(ticket));
          oyra_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                "new_ti: %s",OY_DBG_ARGS_, oyPixelAccess_Show(new_ticket));
        }
        if(issue || oy_debug)
        {
          char *a,*b,*c;
          a = strdup(oyRectangle_Show( ticket_roi_pix ));
          b = strdup(oyRectangle_Show( image_pix ));
          c = strdup(oyRectangle_Show( new_ticket_array_roi_pix ));
          oyra_msg( issue?oyMSG_ERROR:oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                     "node [%d] scale: %.02f old roi %s/%s(image) -> new roi %s array_in[%d](%dx%d)%dc w/scale=%g h/scale=%g-> array_out[%d](%dx%d)%dc"
                     "%s%s%s",OY_DBG_ARGS_,
                     oyStruct_GetId( (oyStruct_s*)node ), scale,
                     a,b,c,
                     oyStruct_GetId( (oyStruct_s*)array_in ), nw,nh,channels_src, w/scale, h/scale,
                     oyStruct_GetId( (oyStruct_s*)array_out ), w,h, channels_dst,
                     issue?" found issue(s): too":"",
                     issue & 1 ? " wide":"",
                     issue & 2 ? " heigh":"" );
          if(a) {free(a);} if(b) {free(b);} if(c) {free(c);}
        }

        /* do the scaling while copying the channels */
#if defined(USE_OPENMP)
#pragma omp parallel for private(x,xs,ys)
#endif
        for(y = 0; y < h; ++y)
        {
          ys = y/scale;
          if(OY_ROUNDp(ys) >= nh)
          {
            if(oy_debug || (OY_ROUNDp(ys) >= (nh + 1)))
              oyra_msg( oy_debug?oyMSG_DBG:oyMSG_ERROR, (oyStruct_s*)ticket,
                      OY_DBG_FORMAT_"scale:%g y:%d h:%d ys:%d/%g nh:%d\n",
                      OY_DBG_ARGS_, scale, y,h,ys,y/scale,nh);
          } else
          for(x = 0; x < w; ++x)
          {
            xs = x/scale;
            if(OY_ROUNDp(xs) < nw)
            {
#if 0
              /* optimisations which have not much benefit */
              int chars = channels_src*bps_in, b;
              uint32_t ** array_out_4 = (uint32_t**)array_out_data;
              uint32_t ** array_in_4  = (uint32_t**)array_in_data;
              if(bps_in == 4)
              for( b = 0; b < channels_src; ++b )
                array_out_4[y] [x  *channels_dst+b] =
                array_in_4 [ys][xs *channels_src+b];
              else
              for( b = 0; b < chars; ++b )
                array_out_data[y] [x  *channels_dst*bps_out+b] =
                array_in_data [ys][xs *channels_src*bps_in +b];
#else
              memmove( &array_out_data[y] [x  *channels_dst*bps_out],
                       &array_in_data [ys][xs *channels_src*bps_in], channels_src*bps_in );
#endif
            }
          }
        }

        oyPixelAccess_Release( &new_ticket );
        oyArray2d_Release( &array_in );
        oyArray2d_Release( &array_out );
      }

      oyRectangle_Release( &new_ticket_array_roi );
      //oyRectangle_Release( &new_ticket_array_roi_pix );
      oyRectangle_Release( &image_pix );

    } else /* scale == 1.0 */
    {
      result = oyFilterNode_Run( input_node, plug, ticket );
    }

    clean_scale2:
    oyOptions_Release( &node_opts );
    oyFilterPlug_Release( &plug );

    //oyRectangle_Release( &ticket_roi_pix );
    oyFilterNode_Release( &input_node );
  }

  clean_scale1:
  oyImage_Release( &image );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );

  return result;
}


#define OY_IMAGE_SCALE_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "scale"

/** @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a scale image filter
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/14 (Oyranos: 0.9.5)
 *  @date    2013/06/14
 */
oyCMMapi_s * oyraApi7ImageScaleCreate(void)
{
  oyCMMapi7_s * scale7;
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
  oyObject_SetNames( plug->oy_, "oyra7", __func__, OY_IMAGE_SCALE_REGISTRATION );
  oyObject_SetNames( socket->oy_, "oyra7", __func__, OY_IMAGE_SCALE_REGISTRATION );

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

  scale7 = oyCMMapi7_Create (          oyraCMMInit, oyraCMMReset, oyraCMMMessageFuncSet,
                                       OY_IMAGE_SCALE_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       oyraFilter_ImageScaleRun,
                                       (oyConnector_s**)plugs, 1, 0,
                                       (oyConnector_s**)sockets, 1, 0,
                                       0, 0 );
  oyObject_SetNames( scale7->oy_, "oyra7", __func__, OY_IMAGE_SCALE_REGISTRATION );
  return (oyCMMapi_s*) scale7;
}

const char * oyraApi4UiImageScaleGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "image_scale";
    else if(type == oyNAME_NAME)
      return _("Image[scale]");
    else if(type == oyNAME_DESCRIPTION)
      return _("Scale Image Filter Object");
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
        oyStringAddPrintf( &help_desc, 0,0, "%s\n"
"    %s \n"
" \n"
"                start_xy          %s \n"
"                   |                / \n"
"             +-----|---------------/--------------+ \n"
"             |     |              /               | \n"
"             |     |             /                +--- %s \n"
"             |  ---+------------/----------+      | \n"
"             |     |           /           +---------- %s \n"
"             |     |   +------+--------+   |      | \n"
"             |     |   |               |   |      | \n"
"             |     |   |               |   |      | \n"
"             |     |   +---------------+   |      | \n"
"             |     |                       |      | \n"
"             |     +-----------------------+      | \n"
"             |                                    | \n"
"             +------------------------------------+ \n"
        "",
        _("The filter will expect a \"scale\" double option and will create, fill and process a according data version with a new job ticket. The new job tickets image, array and output_array_roi will be divided by the supplied \"scale\" factor. It's plug will request the divided image sizes from the source socket."),
        _("Relation of positional parameters:"),
        /* output image region of interesst */
        _("output_array_roi"),
        _("source image"),
        _("output image") );
      return help_desc;
    }
  } else if(strcmp(select,"category") == 0)
  {
    if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return _("Image/Simple Image[scale]");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter is used to reduce pixels.");
  }
  return 0;
}


/** @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing a scale image filter
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/14 (Oyranos: 0.9.5)
 *  @date    2013/06/14
 */
oyCMMapi_s * oyraApi4ImageScaleCreate(void)
{
  static const char * oyra_api4_ui_image_scale_texts[] = {"name", "help", "category", 0};
  oyCMMui_s * ui = oyCMMui_Create( "Image/Simple Image[scale]", /* category */
                                   oyraApi4UiImageScaleGetText,
                                   oyra_api4_ui_image_scale_texts, 0 );
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C};

  oyCMMapi4_s * scale4 = oyCMMapi4_Create( oyraCMMInit, oyraCMMReset, oyraCMMMessageFuncSet,
                                       OY_IMAGE_SCALE_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       NULL,
                                       NULL,
                                       ui,
                                       NULL );
  oyObject_SetNames( scale4->oy_, "oyra4", __func__, OY_IMAGE_SCALE_REGISTRATION );
  return (oyCMMapi_s*)scale4;
}
/* OY_IMAGE_SCALE_REGISTRATION ----------------------------------------------*/
/* ---------------------------------------------------------------------------*/



