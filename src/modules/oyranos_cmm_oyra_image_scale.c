/** @file oyranos_cmm_oyra_image.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2013 (C) Kai-Uwe Behrmann
 *
 *  @brief    modules for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2013/06/10
 */

#include "oyCMMapi4_s.h"
#include "oyCMMapi7_s.h"
#include "oyCMMui_s.h"
#include "oyConnectorImaging_s.h"
#include "oyRectangle_s.h"

#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_string.h"

#include <iconv.h>
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
 *  @version Oyranos: 0.9.5
 *  @date    2013/06/10
 *  @since   2013/06/10 (Oyranos: 0.9.5)
 */
int      oyraFilter_ImageScaleRun    ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int result = 0, error = 0;
  oyFilterSocket_s * socket;
  oyFilterNode_s * input_node = 0,
                 * node;
  oyFilterPlug_s * plug;
  oyImage_s * image;

  int dirty = 0;

  socket = oyFilterPlug_GetSocket( requestor_plug );
  node = oyFilterSocket_GetNode( socket );

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  if(!image)
    return 1;

  {
    oyRectangle_s * ticket_roi = oyPixelAccess_GetOutputROI( ticket );
    oyArray2d_s * ticket_array = oyPixelAccess_GetArray( ticket );
    double  scale = 1.0;
    oyOptions_s * node_opts = oyFilterNode_GetOptions( node, 0 );

    if(!node_opts)
      dirty = 1;

    if(dirty)
      return dirty;

    plug = oyFilterNode_GetPlug( node, 0 );

    /* select node */
    input_node = oyFilterNode_GetPlugNode( node, 0 );

    /* find scale factor */
    error = oyOptions_FindDouble( node_opts,
                                  "//" OY_TYPE_STD "/scale/scale",
                                  0, &scale );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
    WARNc2_S("%s %f", "scale factor:", scale);

    if(scale != 1.0)
    {
      oyRectangle_s * new_ticket_roi;
      oyPixelAccess_s * new_ticket = 0;
      oyRectangle_s * r = oyRectangle_New(0);
      oyRectangle_s * array_pix = oyRectangle_New(0);

      oyRectangle_SetByRectangle( r, ticket_roi );
      /*oyRectangle_Scale( r, scale );*/

      new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
      oyPixelAccess_SetArray( new_ticket, 0 );

      new_ticket_roi = oyPixelAccess_GetOutputROI( new_ticket );
      oyRectangle_SetByRectangle( new_ticket_roi, r );

      /* use the correct source pixels */
      oyPixelAccess_ChangeRectangle( new_ticket,
                                     oyPixelAccess_GetStart( new_ticket, 0 )
                                     + oyRectangle_GetGeo1( new_ticket_roi, 0 ),
                                     oyPixelAccess_GetStart( new_ticket, 1 )
                                     + oyRectangle_GetGeo1( new_ticket_roi, 1 ),
                                     new_ticket_roi );

      if(oyRectangle_CountPoints(  new_ticket_roi ) > 0)
      {
        oyImage_s * ticket_image = oyPixelAccess_GetOutputImage( ticket ),
                  * new_ticket_image = oyImage_Copy( ticket_image, ticket_image->oy_ );
        int nw,nh,w,h,x,y;
        oyArray2d_s * new_ticket_array,
                    * array_in,
                    * array_out;
        uint8_t ** array_in_data,
                ** array_out_data;
        oyDATATYPE_e data_type_in = oyToDataType_m( oyImage_GetPixelLayout( ticket_image, oyLAYOUT ) );
        int bps_in = oyDataTypeGetSize( data_type_in );
        int w_in, stride_in;
        int channels = oyToChannels_m( oyImage_GetPixelLayout( ticket_image, oyLAYOUT ) );

        error = oyPixelAccess_SetOutputImage ( new_ticket, new_ticket_image );
        if(error) WARNc2_S("%s %d", _("found issues"),error);
        new_ticket_array = oyPixelAccess_GetArray( new_ticket );

        /* fill the array rectangle for the following filter */
        if(!new_ticket_array)
        {
          DBGs_PROG3_S( new_ticket, "%s[%d] %s",
                      "Fill new_ticket->array from new_ticket->output_image",
                      oyStruct_GetId( (oyStruct_s*)new_ticket_image ),
                      oyRectangle_Show( new_ticket_roi ) );
          oyImage_FillArray( new_ticket_image,
                             new_ticket_roi, 0,
                             &new_ticket_array, new_ticket_roi,
                             0 );
          oyPixelAccess_SetArray( new_ticket, new_ticket_array );
        }

        /* start new call into branch */
        DBGs_PROG3_S( new_ticket, "%s[%d] %s",
                     "Run new_ticket through filter in node",
                     oyStruct_GetId( (oyStruct_s*)node ),
                     oyRectangle_Show( new_ticket_roi ) );
        result = oyFilterNode_Run( input_node, plug, new_ticket );

        array_in = oyPixelAccess_GetArray( new_ticket );
        array_out = oyPixelAccess_GetArray( ticket );
        array_in_data  = oyArray2d_GetData( array_in );
        array_out_data = oyArray2d_GetData( array_out );
        w_in =  (int)(oyArray2d_GetWidth(array_in)+0.5);
        stride_in = w_in * bps_in;
        w = oyArray2d_GetWidth( array_out )/channels;
        h = oyArray2d_GetHeight( array_out );
        nw = oyArray2d_GetWidth( array_in )/channels;
        nh = oyArray2d_GetHeight( array_in );
        for(y = oyPixelAccess_GetStart( new_ticket, 1 )
                                    + oyRectangle_GetGeo1( new_ticket_roi, 1 );
              y < h; ++y)
        {
          int is_allocated = 0;
          void * p;
          if(y/scale >= nh) break;
          for(x = oyPixelAccess_GetStart( new_ticket, 0 )
                                    + oyRectangle_GetGeo1( new_ticket_roi, 0 );
                x < w; ++x)
          {
            if(x/scale >= nw) continue;
            memcpy( &array_out_data[y][x*channels*bps_in],
                    &array_in_data [(int)(y/scale)][(int)(x/scale)*channels*bps_in], channels*bps_in );
            /*p = oyImage_GetPointF(new_ticket_image)( new_ticket_image, x,y, -1, &is_allocated );
            oyImage_GetSetPointF(ticket_image)( ticket_image, x,y, -1, p );*/
          }
        }

        oyImage_Release( &ticket_image );
        oyImage_Release( &new_ticket_image );
        oyArray2d_Release( &new_ticket_array );
        oyPixelAccess_Release( &new_ticket );
        oyRectangle_Release( &array_pix );
        oyRectangle_Release( &r );
      }

    } else /* scale == 1.0 */
    {
      result = oyFilterNode_Run( input_node, plug, ticket );
    }
    oyFilterPlug_Release( &plug );

    oyRectangle_Release( &ticket_roi );
    oyArray2d_Release( &ticket_array );
    oyFilterNode_Release( &input_node );
  }

  return result;
}


#define OY_IMAGE_SCALE_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "scale"

/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a scale image filter
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/14 (Oyranos: 0.9.5)
 *  @date    2013/06/14
 */
oyCMMapi_s * oyraApi7ImageScaleCreate(void)
{
  int32_t cmm_version[3] = {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},
          module_api[3]  = {0,9,5};
  static oyDATATYPE_e data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                       oyHALF, oyFLOAT, oyDOUBLE, 0};
  oyConnectorImaging_s * plug = oyConnectorImaging_New(0),
                       * socket = oyConnectorImaging_New(0);
  static oyConnectorImaging_s * plugs[2] = {0,0},
                              * sockets[2] = {0,0};
  plugs[0] = plug;
  sockets[0] = socket;

  oyConnectorImaging_SetDataTypes( plug, data_types, 6 );
  oyConnectorImaging_SetReg( plug, "//" OY_TYPE_STD "/manipulator.data" );
  oyConnectorImaging_SetMatch( plug, oyFilterSocket_MatchImagingPlug );
  oyConnectorImaging_SetTexts( plug, oyCMMgetImageConnectorPlugText,
                               oy_image_connector_texts );
  oyConnectorImaging_SetIsPlug( plug, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_OFFSET, -1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT, 255 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MIN_COLOUR_COUNT, 1 );
  oyConnectorImaging_SetCapability( plug, oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_COUNT, 255 );
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
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_OFFSET, -1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT, 255 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MIN_COLOUR_COUNT, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_MAX_COLOUR_COUNT, 255 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA, 1 );
  oyConnectorImaging_SetCapability( socket, oyCONNECTOR_IMAGING_CAP_ID, 1 );

  oyCMMapi7_s * scale7 = oyCMMapi7_Create( oyraCMMInit, oyraCMMMessageFuncSet,
                                       OY_IMAGE_SCALE_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       oyraFilter_ImageScaleRun,
                                       (oyConnector_s**)plugs, 1, 0,
                                       (oyConnector_s**)sockets, 1, 0,
                                       0, 0 );
  return (oyCMMapi_s*) scale7;
}

const char * oyraApi4UiImageScaleGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return "image_scale";
    else if(type == oyNAME_NAME)
      return _("Image[scale]");
    else if(type == oyNAME_DESCRIPTION)
      return _("Scale Image Filter Object");
  } else if(strcmp(select,"help"))
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter is used to reduce pixels.");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter will expect a \"scale\" option and will create, fill and process a according data version with a new job ticket.");
  } else if(strcmp(select,"category"))
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


/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
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
          module_api[3]  = {0,9,5};

  oyCMMapi4_s * scale4 = oyCMMapi4_Create( oyraCMMInit, oyraCMMMessageFuncSet,
                                       OY_IMAGE_SCALE_REGISTRATION,
                                       cmm_version, module_api,
                                       NULL,
                                       NULL,
                                       NULL,
                                       ui,
                                       NULL );
  return (oyCMMapi_s*)scale4;
}
/* OY_IMAGE_SCALE_REGISTRATION ----------------------------------------------*/
/* ---------------------------------------------------------------------------*/



