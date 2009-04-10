/** @file oyranos_cmm_oyX1.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 backend for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/12/12
 */

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

#define CMM_NICK "oyX1"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/* OY_IMAGE_DISPLAY_REGISTRATION */
/* OYX1_MONITOR_REGISTRATION */

int                oyX1CMMInit       ( );
int            oyX1CMMMessageFuncSet ( oyMessage_f         message_func );
int            oyX1CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value );
oyWIDGET_EVENT_e   oyX1WidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );
int  oyX1FilterSocket_ImageDisplayInit(oyFilterSocket_s  * socket,
                                       oyImage_s         * image );
oyOptions_s* oyX1Filter_ImageDisplayValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );

extern oyCMMapi4_s   oyX1_api4_image_display;
extern oyCMMapi7_s   oyX1_api7_image_display;



/* OY_IMAGE_DISPLAY_REGISTRATION ---------------------------------------------*/

#ifdef HAVE_X11
#include <X11/extensions/Xfixes.h>
#endif
#ifdef HAVE_Xcolor
#include <Xcolor.h>
#endif

/** @func    oyX1Filter_ImageDisplayCanHandle
 *  @brief   inform about image handling capabilities
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int    oyX1Filter_ImageDisplayCanHandle(oyCMMQUERY_e     type,
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

oyWIDGET_EVENT_e   oyX1WidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}

oyOptions_s* oyX1Filter_ImageDisplayValidateOptions
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

/** @func    oyX1FilterNode_ImageDisplayContextToMem
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
oyPointer  oyX1FilterNode_ImageDisplayContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

char *   oyX1FilterNode_ImageDisplayID(oyFilterNode_s    * node )
{
  char * ID = malloc(128);

  sprintf( ID, "//image/display/filter_id_%d", oyObject_GetId( node->oy_ ));

  return ID;
}

int  oyX1FilterSocket_ImageDisplayInit(oyFilterSocket_s  * socket,
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
  oyBlob_s * win_id = 0;

  if(oy_debug) WARNc_S("Init Start");

  input_node = node->plugs[0]->remote_socket_->node;

  ID = oyX1FilterNode_ImageDisplayID( node );

  win_id = (oyBlob_s*) oyOptions_GetType( image->tags, -1, "window_id",
                                            oyOBJECT_BLOB_S );

# if defined(HAVE_X11) && defined (HAVE_Xcolor)
  if(win_id)
  {
    Atom netColorTarget;
    Window w = (Window) win_id->ptr;
    const char * display_name = oyOptions_FindString( image->tags,
                                                      "display_name", 0 );
    Display * display = XOpenDisplay( display_name );
    oyBlob_Release( &win_id );
    WARNc2_S("\n  Display: %s Window id: %d", display_name, w );

    {
      oyRegion_s * display_region = (oyRegion_s*) oyOptions_GetType( 
                                          image->tags, -1,
                                          "display_region", oyOBJECT_REGION_S );
      /* Upload the region to the window. */
      XRectangle rec[2] = { { 0,0,0,0 }, { 0,0,0,0 } };
      XserverRegion reg = 0;
      XcolorRegion region;

      rec[0].x = display_region->x;
      rec[0].y = display_region->y;
      rec[0].width = display_region->width;
      rec[0].height = display_region->height;

      reg = XFixesCreateRegion( display, rec, 1);

      region.region = reg;
      memset( region.md5, 0, 16 );

      error = XcolorRegionInsert( display, w, 0, &region, 1 );
      netColorTarget = XInternAtom( display, "_NET_COLOR_TARGET", True );
      XChangeProperty( display, w, netColorTarget, XA_STRING, 8,
                       PropModeReplace,
                       (unsigned char *) ":0.0", strlen(":0.0") );

      oyRegion_Release( &display_region );
    }
    XClearWindow( display, w );
  }
# endif

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
                                 "//colour/config/device_region",
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

/** @func    oyX1FilterPlug_ImageDisplayRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/17
 */
int      oyX1FilterPlug_ImageDisplayRun(oyFilterPlug_s   * requestor_plug,
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

  ID = oyX1FilterNode_ImageDisplayID( node );

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
      oyX1FilterSocket_ImageDisplayInit( socket, image );

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
      o = oyConfig_Find( c, "device_region" );
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

oyDATATYPE_e oyx1_data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                         oyHALF, oyFLOAT, oyDOUBLE, 0};

oyConnector_s oyx1_Display_plug = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Display Plug"},
  oyCONNECTOR_SPLITTER, /* connector_type */
  1, /* is_plug == oyFilterPlug_s */
  oyx1_data_types, /* data_types */
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
oyConnector_s *oyx1_Display_plugs[2] = {&oyx1_Display_plug,0};

oyConnector_s oyx1_Display_socket = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "Image Display Plug"},
  oyCONNECTOR_IMAGE, /* connector_type */
  0, /* is_plug == oyFilterPlug_s */
  oyx1_data_types, /* data_types */
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
oyConnector_s *oyx1_Display_sockets[2] = {&oyx1_Display_socket,0};


#define OY_IMAGE_DISPLAY_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "image/display"
/** @instance oyX1_api7_image_display
 *  @brief    oyX1 oyCMMapi7_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi7_s   oyX1_api7_image_display = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) 0, /* oyCMMapi_s * next */
  
  oyX1CMMInit, /* oyCMMInit_f */
  oyX1CMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyX1CMMCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyX1FilterPlug_ImageDisplayRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  oyx1_Display_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  oyx1_Display_sockets,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

/** @instance oyX1_api4_image_display
 *  @brief    oyX1 oyCMMapi4_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi4_s   oyX1_api4_image_display = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyX1_api7_image_display, /* oyCMMapi_s * next */
  
  oyX1CMMInit, /* oyCMMInit_f */
  oyX1CMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyX1CMMCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyX1Filter_ImageDisplayValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyX1WidgetEvent, /* oyWidgetEvent_f */

  oyX1FilterNode_ImageDisplayContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  {oyOBJECT_NAME_S, 0,0,0, "display", "Display", "Display Splitter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Graph/Display", /* category */
  0,   /* options */
  0    /* opts_ui_ */
};

/* OY_IMAGE_DISPLAY_REGISTRATION ---------------------------------------------*/

/* OYX1_MONITOR_REGISTRATION -------------------------------------------------*/

#define OYX1_MONITOR_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.monitor." CMM_NICK

oyMessage_f message = 0;

extern oyCMMapi8_s oyX1_api8;
oyRankPad oyX1_rank_map[];

/* --- implementations --- */

int                oyX1CMMInit       ( )
{
  int error = 0;
  return error;
}


/*
oyPointer          oyCMMallocateFunc   ( size_t            size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               oyCMMdeallocateFunc ( oyPointer         mem )
{
  if(mem)
    free(mem);
}*/

/** @func  oyX1CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX1CMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

/** @func  oyX1CMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX1CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                     OYX1_MONITOR_REGISTRATION OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void     oyX1ConfigsFromPatternUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN shall make shure our message will be visible. */
    message( oyMSG_WARN, options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"list\" will provide a list of available\n"
      " devices. The actual device name can be found in option\n"
      " \"device_name\". The call is as lightwight as possible.\n"
      " The option \"display_name\" is optional to pass the X11 display name\n"
      " and obtain a unfiltered result. It the way to get all monitors\n"
      " connected to a display.\n"
      " The option \"oyNAME_NAME\" returns a string containting geometry and\n"
      " if available, the profile name or size.\n"
      " The bidirectional option \"device_region\" will cause to\n"
      " additionally add display geometry information as a oyRegion_s\n"
      " object.\n"
      " The bidirectional option \"icc_profile\" will add a oyProfile_s.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n"
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n"
      " string.\n"
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The bidirectional optional \"edid\" (specific) key word will\n"
      " additionally add the EDID information inside a oyBlob_s struct.\n"
      " The option \"device_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION above."
      " Informations are stored in the returned oyConfig_s::data member."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"properties\" will provide the devices \n"
      " properties. Requires one device identifier returned with the \n"
      " \"list\" option. The properties may cover following entries:\n"
      " - \"manufacturer\"\n"
      " - \"model\"\n"
      " - \"serial\"\n"
      " - \"host\"\n"
      " - \"system_port\"\n"
      " - \"display_geometry\" (specific) x,y,widthxheight ,e.g."
      " \"0,0,1024x786\"\n"
      " \n"
      " One option \"device_name\" will select the according X display.\n"
      " If not the backend will try to get this information from \n"
      " your \"DISPLAY\" environment variable or uses what the system\n"
      " provides. The \"device_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one. Informations are\n"
      " stored in the returned oyConfig_s::backend_core member."
       );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"setup\" will setup the device from a profile.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"      " file name."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"unset\" will invalidate a profile of a device.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"get\" will provide a oyProfile_s of the\n"
      " device in a \"icc_profile\" option.\n"
      " The option \"device_name\" must be present, see \"list\" above.\n"
      );

  return;
}

int          oyX1DeviceFromName_     ( const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device,
                                       oyAlloc_f           allocateFunc )
{
  const char * value3 = 0;
  oyOption_s * o = 0;
  int error = !device;

    value3 = oyOptions_FindString( options, "edid", 0 );

    if(!error)
    {
      char * manufacturer=0, *model=0, *serial=0, *host=0, *display_geometry=0,
           * system_port=0;
      oyBlob_s * edid = 0;

      if(!device_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                "The \"device_name\" argument is\n"
                " missed to select a appropriate device for the"
                " \"properties\" call.", OY_DBG_ARGS_ );
        error = 1;
      }

      if(error <= 0)
        error = oyGetMonitorInfo_lib( device_name,
                                      &manufacturer, &model, &serial,
                                      &display_geometry, &system_port,
                                      &host, value3 ? &edid : 0, allocateFunc,
                                      (oyStruct_s*)options );

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 OY_DBG_FORMAT_ "\n  Could not complete \"properties\" call.\n"
                 "  oyGetMonitorInfo_lib returned with %s; device_name:"
                 " \"%s\"", OY_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 oyNoEmptyString_m_( device_name ) );

      if(error <= 0)
      {
        if(!*device)
          *device = oyConfig_New( OYX1_MONITOR_REGISTRATION, 0 );
        error = !*device;
        if(!error && device_name)
        error = oyOptions_SetFromText( &(*device)->backend_core,
                                       OYX1_MONITOR_REGISTRATION OY_SLASH "device_name",
                                       device_name, OY_CREATE_NEW );

        OPTIONS_ADD( (*device)->backend_core, manufacturer )
        OPTIONS_ADD( (*device)->backend_core, model )
        OPTIONS_ADD( (*device)->backend_core, serial )
        OPTIONS_ADD( (*device)->backend_core, display_geometry )
        OPTIONS_ADD( (*device)->backend_core, system_port )
        OPTIONS_ADD( (*device)->backend_core, host )
        if(!error && edid)
        {
          o = oyOption_New( OYX1_MONITOR_REGISTRATION OY_SLASH "edid", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, edid->ptr, edid->size );
          if(!error)
            oyOptions_MoveIn( (*device)->data, &o, -1 );
          oyBlob_Release( &edid );
        }
      }
    }

  return error;
}


/** Function oyX1Configs_FromPattern
 *  @brief   oyX1 oyCMMapi8_s Xorg monitors
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/28
 */
int            oyX1Configs_FromPattern (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
  oyConfigs_s * devices = 0;
  oyConfig_s * device = 0;
  oyOption_s * o = 0;
  oyRegion_s * rect = 0;
  const oyRegion_s * r = 0;
  oyProfile_s * p = 0;
  char ** texts = 0;
  char * text = 0,
       * device_name_temp = 0;
  int texts_n = 0, i,
      error = !s;
  const char * odevice_name = 0,
             * oprofile_name = 0,
             * odisplay_name = 0,
             * device_name = 0;
  int rank = oyFilterRegistrationMatch( oyX1_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  const char * tmp = 0;


  /** 1. In case no option is provided or something fails, show a message. */
  if(!options || !oyOptions_Count( options ))
  {
    oyX1ConfigsFromPatternUsage( (oyStruct_s*)options );
    return 0;
  }

  if(rank && error <= 0)
  {
    devices = oyConfigs_New(0);


    /** 2. obtain a proper device_name */
    odisplay_name = oyOptions_FindString( options, "display_name", 0 );
    odevice_name = oyOptions_FindString( options, "device_name", 0 );
    /*message(oyMSG_WARN, (oyStruct_s*)options, "list: %s", value2);*/

    if(odisplay_name && odisplay_name[0])
      device_name = odisplay_name;
    else if(odevice_name && odevice_name[0])
      device_name = odevice_name;
    else
    {
      tmp = getenv("DISPLAY");
#if !defined(__APPLE__)
      if(!tmp)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "DISPLAY variable not set: giving up\n. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
        error = 1;
        return error;
      }

      device_name_temp = oyStringCopy_( tmp, oyAllocateFunc_ );
      if(device_name_temp &&
         (text = strchr(device_name_temp,':')) != 0)
        if( (text = strchr(device_name_temp, '.')) != 0 )
          text[0] = '\000';

      device_name = device_name_temp;
#endif
      text = 0;
    }

    /** 3.  handle the actual call */
    /** 3.1 "list" call */
    if(oyOptions_FindString( options, "list", 0 ))
    {
      texts_n = oyGetAllScreenNames( device_name, &texts, allocateFunc );

      /** 3.1.1 iterate over all requested devices */
      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odevice_name && strcmp(odevice_name, texts[i]) != 0)
          continue;

        device = oyConfig_New( OYX1_MONITOR_REGISTRATION, 0 );
        error = !device;

        /** 3.1.2 tell the "device_name" */
        if(error <= 0)
        error = oyOptions_SetFromText( &device->backend_core,
                                       OYX1_MONITOR_REGISTRATION OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );

        /** 3.1.3 tell the "device_region" in a oyRegion_s */
        if(oyOptions_FindString( options, "device_region", 0 ) ||
           oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          rect = oyX1Region_FromDevice( texts[i] );
          if(!rect)
          {
            WARNc1_S("Could not obtain region information for %s", texts[i]);
          } else
          {
            o = oyOption_New( OYX1_MONITOR_REGISTRATION OY_SLASH "device_region", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &rect );
            oyOptions_MoveIn( device->data, &o, -1 );
          }
        }

        /** 3.1.4 tell the "icc_profile" in a oyProfile_s */
        if( oyOptions_FindString( options, "icc_profile", 0 ) ||
            oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 0;
          char * data = oyX1GetMonitorProfile( texts[i], &size, allocateFunc );

          
          /** Warn and return issue on not found profile. */
          if(!size || !data)
          {
            message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n  "
            "Could not obtain _ICC_PROFILE(_xxx) information for ",OY_DBG_ARGS_,
                     "%s", texts[i]);
            error = -1;
          } else
          {
            p = oyProfile_FromMem( size, data, 0, 0 );
            o = oyOption_New( OYX1_MONITOR_REGISTRATION OY_SLASH "icc_profile",
                              0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( device->data, &o, -1 );
            free( data );
          }
        }

        /** 3.1.5 contruct a oyNAME_NAME string */
        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          o = oyOptions_Find( device->data, "device_region" );
          r = (oyRegion_s*) o->value->oy_struct;

          text = 0; tmp = 0;
      
          tmp = oyRegion_Show( (oyRegion_s*)r );
          STRING_ADD( text, tmp );
          oyOption_Release( &o );

          o = oyOptions_Find( device->data, "icc_profile" );

          if( o && o->value && o->value->oy_struct && 
              o->value->oy_struct->type_ == oyOBJECT_PROFILE_S)
          {
            p = oyProfile_Copy( (oyProfile_s*) o->value->oy_struct, 0 );
            tmp = oyProfile_GetFileName( p, 0 );

            STRING_ADD( text, "  " );
            if(tmp)
            {
              if(oyStrrchr_( tmp, OY_SLASH_C ))
                STRING_ADD( text, oyStrrchr_( tmp, OY_SLASH_C ) + 1 );
              else
                STRING_ADD( text, tmp );
            }

            oyProfile_Release( &p );
          }

          if(error <= 0)
          error = oyOptions_SetFromText( &device->data,
                                         OYX1_MONITOR_REGISTRATION OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          oyFree_m_( text );
        }


        /** 3.1.6 add the rank scheme to combine properties */
        if(error <= 0)
          device->rank_map = oyRankMapCopy( oyX1_rank_map,
                                            device->oy_->allocateFunc_ );

        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        *s = devices;

      oyStringListRelease_( &texts, texts_n, free );

      goto cleanup;

    } else


    /** 3.2 "properties" call; provide extensive infos for the DB entry */
    if(oyOptions_FindString( options, "properties", 0 ))
    {
      texts_n = oyGetAllScreenNames( device_name, &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(odevice_name && strcmp(odevice_name, texts[i]) != 0)
          continue;

        device = oyConfig_New( OYX1_MONITOR_REGISTRATION, 0 );
        error = !device;

        if(error <= 0)
        error = oyOptions_SetFromText( &device->backend_core,
                                       OYX1_MONITOR_REGISTRATION OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );

        /** 3.2.1 add properties */
        error = oyX1DeviceFromName_( texts[i], options, &device,
                                     allocateFunc );


        /** 3.2.2 add the rank map to wight properties for ranking in the DB */
        if(error <= 0 && device)
          device->rank_map = oyRankMapCopy( oyX1_rank_map,
                                            device->oy_->allocateFunc_);
        oyConfigs_MoveIn( devices, &device, -1 );
      }

      if(error <= 0)
        *s = devices;

      oyStringListRelease_( &texts, texts_n, free );

      goto cleanup;

    } else

    /** 3.3 "setup" call; bring a profile to the device */
    if(error <= 0 &&
       oyOptions_FindString( options, "setup", 0 ))
    {
      oprofile_name = oyOptions_FindString( options, "profile_name", 0 );
      error = !odevice_name || !oprofile_name;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
              "The device_name/profile_name option is missed. Options:\n%s",
                OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX1MonitorProfileSetup( odevice_name, oprofile_name );

      goto cleanup;

    } else

    /** 3.4 "unset" call; clear a profile from a device */
    if(error <= 0 &&
       oyOptions_FindString( options, "unset", 0 ))
    {
      error = !odevice_name;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "The device_name option is missed. Options:\n%s",
                OY_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = oyX1MonitorProfileUnset( odevice_name );

      goto cleanup;
    }
  }


  message(oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", OY_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  oyX1ConfigsFromPatternUsage( (oyStruct_s*)options );


  cleanup:
  if(device_name_temp)
    oyFree_m_( device_name_temp );


  return error;
}

/** Function oyX1Config_Check
 *  @brief   oyX1 oyCMMapi8_s Xorg monitor check
 *
 *  @param[in]     config              the monitor device configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/26
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
int            oyX1Config_Check      ( oyConfig_s        * config )
{
  int error = !config,
      rank = 1;

  if(!config)
  {
    message(oyMSG_DBG, (oyStruct_s*)config, OY_DBG_FORMAT_ "\n "
                "No config argument provided.\n", OY_DBG_ARGS_ );
    return 0;
  }

  if(error <= 0)
  {
    /* evaluate a driver specific part of the options */
  }

  return rank;
}

/** @instance oyX1_rank_map
 *  @brief    oyRankPad map for mapping device to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/27
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 */
oyRankPad oyX1_rank_map[] = {
  {"device_name", 2, -1, 0},           /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for device properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {"display_geometry", 3, -1, 0},      /**< important to match */
  {0,0,0,0}                            /**< end of list */
};

/** @instance oyX1_api8
 *  @brief    oyX1 oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/19
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 */
oyCMMapi8_s oyX1_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  (oyCMMapi_s*) & oyX1_api4_image_display, /**< next */

  oyX1CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  oyX1CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  oyX1CMMCanHandle,          /**< oyCMMCanHandle_f oyCMMCanHandle */

  OYX1_MONITOR_REGISTRATION, /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  oyX1Configs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  oyX1Config_Check,          /**< oyConfig_Check_f oyConfig_Check */
  oyX1_rank_map              /**< oyRankPad ** rank_map */
};

/* OYX1_MONITOR_REGISTRATION -------------------------------------------------*/


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * oyX1GetText             ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos X11");
    else
      return _("The window support backend of Oyranos.");
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
      return _("Copyright (c) 2005-2008 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}

/** @instance oyX1_cmm_module
 *  @brief    oyX1 module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMMInfo_s oyX1_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.2",
  oyX1GetText, /* oyCMMInfoGetText_f */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyX1_api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

