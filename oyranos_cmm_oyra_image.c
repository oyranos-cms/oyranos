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

/* OY_IMAGE_DISPLAY_REGISTRATION */
/* OY_IMAGE_REGIONS_REGISTRATION */
/* OY_IMAGE_ROOT_REGISTRATION */
/* OY_IMAGE_OUTPUT_REGISTRATION */


oyDATATYPE_e oyra_image_data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                         oyHALF, oyFLOAT, oyDOUBLE, 0};

int  oyraFilterSocket_ImageDisplayInit(oyFilterSocket_s  * socket,
                                       oyImage_s         * image );



/* OY_IMAGE_DISPLAY_REGISTRATION ---------------------------------------------*/


/** @func    oyraFilter_ImageDisplayCanHandle
 *  @brief   inform about image handling capabilities
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int    oyraFilter_ImageDisplayCanHandle(oyCMMQUERY_e     type,
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

oyOptions_s* oyraFilter_ImageDisplayValidateOptions
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

/** @func    oyraFilterNode_ImageDisplayContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyPointer  oyraFilterNode_ImageDisplayContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

char *   oyraFilterNode_ImageDisplayID(oyFilterNode_s    * node )
{
  char * ID = malloc(128);

  sprintf( ID, "//image/display/filter_id_%d", oyObject_GetId( node->oy_ ));

  return ID;
}

int  oyraFilterSocket_ImageDisplayInit(oyFilterSocket_s  * socket,
                                       oyImage_s         * image )
{
  int n = 0, i,m;
  int error = 0;
  oyFilterGraph_s * display_graph = 0;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node,
                 * cmm_node = 0,
                 * regions = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRegion_s * r;
  oyConfigs_s * devices = 0;
  char * tmp = 0,
       * ID = 0;


  if(oy_debug) WARNc_S("Init Start");

  input_node = node->plugs[0]->remote_socket_->node;

  ID = oyraFilterNode_ImageDisplayID( node );

  /* insert a "regions" filter to handle multiple monitors */
  regions = oyFilterNode_NewWith( "//image/regions", 0,0, 0 );
  /* mark the new node as belonging to this node */
  oyOptions_SetFromText( &regions->tags, ID, "true", OY_CREATE_NEW );

  /* insert "regions" between "display" and its input_node */
  oyFilterNode_Disconnect( node->plugs[0] );
  error = oyFilterNode_Connect( input_node, "Img", regions, "Img",0 );
  error = oyFilterNode_Connect( regions, "Img", node, "Img",0 );



  /* obtain device informations, including geometry and ICC profiles
     from the according Oyranos backend */
  error = oyOptions_SetFromText( &options, "//colour/config/list",
                                 "true", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options,
                                 "//colour/config/display_geometry",
                                 "true", OY_CREATE_NEW );
  o = oyOptions_Find( image->tags, "display_name" );
  o = oyOption_Copy( o, 0 );
  oyOptions_MoveIn( options, &o, -1 );
  error = oyDevicesGet( "colour", "monitor", options, &devices );
  n = oyConfigs_Count( devices );
  o = oyOptions_Find( node->core->options_, "devices" );
  /* cache the devices scan result; currently is no updating implemented */
  if(!o)
  {
    oyOptions_MoveInStruct( &node->core->options_,
                            "//image/display/devices",
                            (oyStruct_s**) &devices, OY_CREATE_NEW );
  }
  else
  {
    oyConfigs_Release( (oyConfigs_s**) & o->value->oy_struct );
    o->value->oy_struct = (oyStruct_s*) oyConfigs_Copy( devices, 0 );
  }
  oyOption_Release( &o );


  m = oyFilterNode_EdgeCount( regions, 1, OY_FILTEREDGE_CONNECTED );

  /* add new regions and colour CMMs as needed */
  if(n > m)
  {
    for(i = 0; i < n-m; ++i)
    {
      /* The first region is the one provided by the user graph. */
      if(m != 0 || i != 0)
      {
        if(oyFilterRegistrationMatch( input_node->core->registration_,
                                      "//colour/icc", 0 ))
          cmm_node = oyFilterNode_NewWith( input_node->core->registration_,
                                           0,0, 0 );
        else
          WARNc2_S("\n  Filter %s expects a colour conversion filter as"
                   " input\n  But obtained: %s", node->relatives_,
                   input_node->relatives_ );

        /* mark the new node as belonging to this node */
        oyOptions_SetFromText( &cmm_node->tags, ID, "true",
                               OY_CREATE_NEW );

        /* position the new CMM between the original CMMs input and 
           "regions" */
        error = oyFilterNode_Connect( cmm_node, "Img",
                                      regions, "Img", 0 );
        if(error > 0)
          WARNc1_S( "could not add  new CMM: %s\n",
                    input_node->core->registration_ );

        error = oyFilterNode_Connect( 
                                  input_node->plugs[0]->remote_socket_->node, 0,
                                      cmm_node, "Img", 0 );

        /* clone into a new image */
        if(cmm_node->sockets[0]->data)
          cmm_node->sockets[0]->data->release( 
                                    (oyStruct_s**)&cmm_node->sockets[0]->data );
        cmm_node->sockets[0]->data = (oyStruct_s*)oyImage_CreateForDisplay (
                                                image->width, image->height,
                                                0, image->layout_[0],
                                                0, 0,0,0,0, 0 );
      }
    }
  }


  m = oyOptions_CountType( regions->core->options_,
                           "//image/regions/region", oyOBJECT_REGION_S );
  /* add missed regions */
  if(n > m)
  {
    tmp = oyAllocateFunc_(64);

    for(i = m; i < n;  ++i)
    {
      oySprintf_( tmp, "//image/regions/region/%d", i );

      r = oyRegion_NewWith( 0., 0., 0., 0., 0);
      oyOptions_MoveInStruct( &regions->core->options_, tmp,
                              (oyStruct_s**)&r, OY_CREATE_NEW );
    }

    oyDeAllocateFunc_(tmp); tmp = 0;
  }


  /* describe all our newly created filters and add them to this node */
  display_graph = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( display_graph, regions, ID, 0 );
  oyOptions_MoveInStruct( &node->core->options_,
                          "//image/display/display_graph",
                          (oyStruct_s**) &display_graph, OY_CREATE_NEW );



  if(oy_debug) WARNc_S("Init End");

  free(ID); ID = 0;

  return error;
}

/** @func    oyraFilterPlug_ImageDisplayRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int      oyraFilterPlug_ImageDisplayRun(oyFilterPlug_s   * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int x = 0, y = 0, n = 0, i;
  int result = 0, error = 0;
  oyFilterGraph_s * display_graph = 0;
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterNode_s * node = socket->node,
                 * regions = 0;
  oyImage_s * image = (oyImage_s*)socket->data,
            * input_image = 0;
  oyOption_s * o = 0;
  oyRegion_s * r, * rd, * ri;
  oyConfigs_s * devices = 0;
  oyConfig_s * c = 0;
  oyProfile_s * p = 0;
  int display_pos_x,
      display_pos_y;
  int dirty = 0,
      init = 0;
  char * ID = 0;

  x = ticket->start_xy[0];
  y = ticket->start_xy[1];

  result = oyPixelAccess_CalculateNextStartPixel( ticket, requestor_plug);

  if(result != 0)
    return result;

  ID = oyraFilterNode_ImageDisplayID( node );

  {
    /* display stuff */

    if(!node->core->options_)
      /* allocate options */
      node->core->options_ = oyOptions_New(0);

    /* obtain the local graph */
    display_graph = (oyFilterGraph_s*)oyOptions_GetType( node->core->options_,
                                            -1, "//image/display/display_graph",
                                            oyOBJECT_FILTER_GRAPH_S );

    if(!display_graph)
    {
      init = 1;

      /* init this filter */
      oyraFilterSocket_ImageDisplayInit( socket, image );

      display_graph = (oyFilterGraph_s*)oyOptions_GetType( node->core->options_,
                                            -1, "//image/display/display_graph",
                                            oyOBJECT_FILTER_GRAPH_S );
      error = !display_graph;
    }


    /* look for our requisites */
    regions = oyFilterGraph_GetNode( display_graph, -1, "//image/regions", ID );

    /* get cached devices */
    devices = (oyConfigs_s*)oyOptions_GetType( node->core->options_, -1, 
                                "//image/display/devices", oyOBJECT_CONFIGS_S );

    n = oyConfigs_Count( devices );
    if(!n || oyFilterNode_EdgeCount( regions, 1, OY_FILTEREDGE_CONNECTED ) < n)
      return 1;

    /* process all display regions */
    if(error <= 0)
    for(i = 0; i < n; ++i)
    {
      c = oyConfigs_Get( devices, i );

      /* get device dimension */
      o = oyConfig_Find( c, "display_geometry" );
      if(o && o->value_type == oyVAL_STRUCT)
        rd = (oyRegion_s *) o->value->oy_struct;
      oyOption_Release( &o );

      /* get current work region */
      r = (oyRegion_s *) oyOptions_GetType( regions->core->options_, i, 
                                  "//image/regions/region", oyOBJECT_REGION_S );

      /* get display region to project into */
      o = oyOptions_Find( image->tags, "display_region" );
      if(o && o->value_type == oyVAL_STRUCT && o->value &&
         o->value->oy_struct->type_ == oyOBJECT_REGION_S)
        ri = (oyRegion_s *) o->value->oy_struct;
      oyOption_Release( &o );

      /* trim and adapt the work region */
      oyRegion_SetByRegion( r, ri );
      display_pos_x = r->x;
      display_pos_y = r->y;
      oyRegion_Trim( r, rd );
      r->x -= display_pos_x;
      r->y -= display_pos_y;
      if(oy_debug) WARNc2_S("image %d: %s", i, oyRegion_Show( r ));

      /* all regions are relative to image dimensions */
      if(image->width != 0)
        oyRegion_Scale( r, 1./image->width );

      /* select actual image from the according CMM node */
      if(regions->plugs && regions->plugs[i] &&
         regions->plugs[i]->remote_socket_)
        input_image = (oyImage_s*)regions->plugs[i]->remote_socket_->data;
      else
      {
        input_image = 0;
        WARNc2_S("image %d: is missed", i, oyRegion_Show( r ));
      }

      /* set the device profile of all CMM's image data */
      if(init)
      {
        error = oyDeviceGetProfile( c, &p );

        if(p && input_image && !oyProfile_Equal( input_image->profile_, p ))
        {
          oyImage_SetCritical( input_image, 0, p, 0 );
          error = oyOptions_SetFromText( &ticket->graph->options,
                               "//image/profile/dirty", "true", OY_CREATE_NEW );
          ++dirty;
        }

        oyProfile_Release( &p );
      }

      oyConfig_Release( &c );
    }

    oyConfigs_Release( &devices );

    /* stop here and request an update */
    if(dirty > 0)
    {
      oyFilterNode_Release( &regions );
      result = dirty;
      goto clean;
    }


    /* make the graph flow: process the upstream "regions" node */
    regions->api7_->oyCMMFilterPlug_Run( node->plugs[0], ticket );

  }

  clean:
  oyFilterNode_Release( &regions );
  if(ID) free(ID);    

  return result;
}


oyConnector_s oyra_imageDisplay_plug = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Display Plug"},
  oyCONNECTOR_SPLITTER, /* connector_type */
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
oyConnector_s *oyra_imageDisplay_plugs[2] = {&oyra_imageDisplay_plug,0};

oyConnector_s oyra_imageDisplay_socket = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Display Plug"},
  oyCONNECTOR_IMAGE, /* connector_type */
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
oyConnector_s *oyra_imageDisplay_sockets[2] = {&oyra_imageDisplay_socket,0};


#define OY_IMAGE_DISPLAY_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "image/display"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi7_s   oyra_api7_image_display = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_write_ppm, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageDisplayCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageDisplayRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  oyra_imageDisplay_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  oyra_imageDisplay_sockets,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi4_s   oyra_api4_image_display = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_display, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageDisplayCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  {oyOBJECT_NAME_S, 0,0,0, "display", "Display", "Display Splitter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Graph/Display", /* category */
  0,   /* options */
  0    /* opts_ui_ */
};

/* OY_IMAGE_DISPLAY_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_REGIONS_REGISTRATION ---------------------------------------------*/


/** @func    oyraFilter_ImageRegionsCanHandle
 *  @brief   inform about image handling capabilities
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/02/23
 */
int    oyraFilter_ImageRegionsCanHandle(oyCMMQUERY_e     type,
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

oyOptions_s* oyraFilter_ImageRegionsValidateOptions
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

/** @func    oyraFilterNode_ImageRegionsContextToMem
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
oyPointer  oyraFilterNode_ImageRegionsContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

/** @func    oyraFilterPlug_ImageRegionsRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int      oyraFilterPlug_ImageRegionsRun(oyFilterPlug_s   * requestor_plug,
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
  oyRegion_s * r;
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
                             "//image/regions/region", oyOBJECT_REGION_S );

    /* regions stuff */
    for(i = 0; i < n; ++i)
    {
      /* select current region */
      r = (oyRegion_s*)oyOptions_GetType( node->core->options_, i,
                             "//image/regions/region", oyOBJECT_REGION_S );

      /* Map each matching plug to a new ticket with a corrected region. */
      new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
      oyArray2d_Release( &new_ticket->array );

      if(r)
        oyRegion_SetByRegion( new_ticket->output_image_roi, r );

      /* select node */
      input_node = node->plugs[i]->remote_socket_->node;
      /* adapt the region of interesst to the new image dimensions */
      oyRegion_Trim( new_ticket->output_image_roi, ticket->output_image_roi );

      if(oyRegion_CountPoints(  new_ticket->output_image_roi ) > 0)
      {
        /* prepare the array for the following filter */
        if(!new_ticket->array)
          oyImage_FillArray( new_ticket->output_image,
                             new_ticket->output_image_roi, 0,
                            &new_ticket->array, 0 );

        /* start new call */
        l_result = input_node->api7_->oyCMMFilterPlug_Run( node->plugs[i],
                                                           new_ticket );
        if(l_result != 0 && (result <= 0 || l_result > 0))
          result = l_result;

        error = oyImage_ReadArray( image, new_ticket->output_image_roi,
                                   new_ticket->array );
      }
      oyPixelAccess_Release( &new_ticket );

      oyOption_Release( &o );
    }
  }

  return result;
}


oyConnector_s oyra_imageRegions_plug = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Regions Plug"},
  oyCONNECTOR_SPLITTER, /* connector_type */
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
oyConnector_s *oyra_imageRegions_plugs[2] = {&oyra_imageRegions_plug,0};

oyConnector_s oyra_imageRegions_socket = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Regions Plug"},
  oyCONNECTOR_IMAGE, /* connector_type */
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
oyConnector_s *oyra_imageRegions_sockets[2] = {&oyra_imageRegions_socket,0};


#define OY_IMAGE_REGIONS_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "image/regions"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter routing the graph to several regions
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi7_s   oyra_api7_image_regions = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_display, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRegionsCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_REGIONS_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageRegionsRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  oyra_imageRegions_plugs,   /* plugs */
  1,   /* plugs_n */
  UINT16_MAX,   /* plugs_last_add */
  oyra_imageRegions_sockets,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter routing the graph to several regions
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi4_s   oyra_api4_image_regions = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_regions, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_ImageRegionsCanHandle, /* oyCMMCanHandle_f */

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

  {oyOBJECT_NAME_S, 0,0,0, "regions", "Regions", "Regions Splitter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Graph/Regions", /* category */
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

    /* adapt the region of interesst to the new image dimensions */
    oyRegion_s * new_roi = oyRegion_NewFrom( ticket->output_image_roi, 0 );
    double correct = ticket->output_image->width / (double) image->width;
    new_roi->width *= correct;
    new_roi->height *= correct;
    error = oyImage_FillArray( image, new_roi, 1, &ticket->array, 0 );
  }

  return result;
}


oyConnector_s oyra_imageRoot_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Socket"},
  oyCONNECTOR_IMAGE, /* connector_type */
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
oyConnector_s * oyra_imageRoot_connectors[2] = {&oyra_imageRoot_connector,0};


#define OY_IMAGE_ROOT_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "image/root"
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
  oyra_imageRoot_connectors,   /* sockets */
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

oyConnector_s oyra_imageOutput_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Plug"},
  oyCONNECTOR_IMAGE, /* connector_type */
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
oyConnector_s* oyra_imageOutput_connectors[2] = {&oyra_imageOutput_connector,0};


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


#define OY_IMAGE_OUTPUT_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "image/output"
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
  (oyCMMapi_s*) & oyra_api4_image_regions, /* oyCMMapi_s * next */
  
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

  oyra_imageOutput_connectors,   /* plugs */
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



