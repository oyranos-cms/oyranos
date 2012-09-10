/** @file oyranos_cmm_oydi.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/02/23
 */

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

#define CMM_NICK "oydi"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/* OY_IMAGE_DISPLAY_REGISTRATION */

int                oydiCMMInit       ( oyStruct_s        * filter );
int            oydiCMMMessageFuncSet ( oyMessage_f         message_func );
oyMessage_f oydi_msg = 0;


oyWIDGET_EVENT_e   oydiWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );
int  oydiFilterSocket_ImageDisplayInit(oyPixelAccess_s   * ticket,
                                       oyFilterSocket_s  * socket,
                                       oyImage_s         * image );
oyOptions_s* oydiFilter_ImageDisplayValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
const char * oydiGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * oydiApi4UiImageDisplayGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

extern oyCMMapi4_s   oydi_api4_image_display;
extern oyCMMapi7_s   oydi_api7_image_display;



/* OY_IMAGE_DISPLAY_REGISTRATION ---------------------------------------------*/

#if defined(HAVE_X11) && defined(HAVE_XCM)
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <X11/Xcm/Xcm.h>
#endif


oyWIDGET_EVENT_e   oydiWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}

oyOptions_s* oydiFilter_ImageDisplayValidateOptions
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

/** @func    oydiFilterNode_ImageDisplayContextToMem
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
oyPointer  oydiFilterNode_ImageDisplayContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

char *   oydiFilterNode_ImageDisplayID(oyFilterNode_s    * node )
{
  char * ID = malloc(128);

  sprintf( ID, "//" OY_TYPE_STD "/display/filter_id_%d",
           oyObject_GetId( node->oy_ ));

  return ID;
}

int oydiFilterSocket_SetWindowRegion ( oyPixelAccess_s   * ticket,
                                       oyImage_s         * image )
{
  int error = 0;
  oyBlob_s * win_id, * display_id;

  win_id = (oyBlob_s*) oyOptions_GetType( image->tags, -1, "window_id",
                                          oyOBJECT_BLOB_S );
  display_id = (oyBlob_s*) oyOptions_GetType( image->tags, -1, "display_id",
                                          oyOBJECT_BLOB_S );

# if defined(HAVE_X11) && defined (HAVE_XCM)
  if(win_id && display_id)
  {
    int x,y, i,j;
    Atom xColorTarget;
    Window w = (Window) oyBlob_GetPointer(win_id), w_return;
    XWindowAttributes attr;
    const char * display_name = oyOptions_FindString( image->tags,
                                                      "display_name", 0 );
    Display * display = 
#if 0
                         XOpenDisplay( display_name ); /* + XCloseDisplay() */
#else
                         (Display *) oyBlob_GetPointer(display_id);
#endif
    oyRectangle_s * display_rectangle = (oyRectangle_s*) oyOptions_GetType( 
                                       image->tags, -1, "display_rectangle",
                                       oyOBJECT_RECTANGLE_S );
    oyRectangle_s * old_window_rectangle = (oyRectangle_s*) oyOptions_GetType(
                                       image->tags, -1, "old_window_rectangle",
                                       oyOBJECT_RECTANGLE_S ),
                  * window_rectangle = 0;
#ifdef DEBUG
    char * tmp = oyStringCopy_( oyRectangle_Show(display_rectangle), oyAllocateFunc_);

    oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
             OY_DBG_FORMAT_"Display: %s Window id: %d  display_rectangle:%s old_window_rectangle:%s", OY_DBG_ARGS_,
             display_name, w, tmp, oyRectangle_Show( old_window_rectangle ) );
    oyFree_m_( tmp );
#endif

    oyBlob_Release( &win_id );

    if(!old_window_rectangle)
    {
      old_window_rectangle = oyRectangle_NewFrom( 0,0 );

      oyOptions_MoveInStruct( &image->tags,
                            "//" OY_TYPE_STD "/display/old_window_rectangle",
                            (oyStruct_s**) &old_window_rectangle, OY_CREATE_NEW );
      old_window_rectangle = (oyRectangle_s*) oyOptions_GetType(
                                      image->tags, -1, "old_window_rectangle",
                                      oyOBJECT_RECTANGLE_S );
    }

    /* We need window relative coordinates. (Works not everywhere? - FVWM) */
    XGetWindowAttributes( display, w, &attr );
    XTranslateCoordinates( display, w, attr.root, 
                                  -attr.border_width, -attr.border_width,
                                  &x, &y, &w_return);
    oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
               OY_DBG_FORMAT_"Display: %s Window id: %d  display_rectangle:%s @+%d+%d",
               OY_DBG_ARGS_,
               display_name, (int)w, oyRectangle_Show(display_rectangle), x,y );

    window_rectangle = oyRectangle_NewFrom( display_rectangle, 0 );
    window_rectangle->x -= x;
    window_rectangle->y -= y;

    /* Has the window moved on display? */
    if(!oyRectangle_IsEqual( window_rectangle, old_window_rectangle ))
    {
      /* Upload the region to the window. */
      XRectangle rec[2] = { { 0,0,0,0 }, { 0,0,0,0 } },
               * rect = 0;
      int nRect = 0;
      XserverRegion reg = 0;
      XcolorRegion region, *old_regions = 0;
      unsigned long old_regions_n = 0;
      int pos = -1;
      const char * display_string = DisplayString(display);

      rec[0].x = window_rectangle->x;
      rec[0].y = window_rectangle->y;
      rec[0].width = window_rectangle->width;
      rec[0].height = window_rectangle->height;

      reg = XFixesCreateRegion( display, rec, 1);
      rect = XFixesFetchRegion( display, reg, &nRect );
      if(!nRect)
      {
        oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket,
                 OY_DBG_FORMAT_
                 "Display: %s Window id: %d  Could not load Xregion:%d",
                 OY_DBG_ARGS_,
                 display_name, (int)w, (int)reg );
        
      } else if(rect[0].x != rec[0].x ||
                rect[0].y != rec[0].y )
      {
        oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket,
                 OY_DBG_FORMAT_
                 "Display: %s Window id: %d  Xregion:%d has wrong position %d,%d",
                 OY_DBG_ARGS_,
                 display_name, (int)w, (int)reg, rect[0].x, rect[0].y );
      } else
        oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
                 OY_DBG_FORMAT_
                 "Display: %s Window id: %d  Xregion:%d uploaded %dx%d+%d+%d",
                 OY_DBG_ARGS_,
                 display_name, (int)w, (int)reg,
                 rect[0].width, rect[0].height, rect[0].x, rect[0].y );

      region.region = htonl(reg);
      memset( region.md5, 0, 16 );

      /* look for old regions */
      old_regions = XcolorRegionFetch( display, w, &old_regions_n );
      /* remove our own old region */
      for(i = 0; i < old_regions_n; ++i)
      {

        if(!old_regions[i].region || pos >= 0)
          break;

        rect = XFixesFetchRegion( display, ntohl(old_regions[i].region),
                                  &nRect );

        for(j = 0; j < nRect; ++j)
        {
          if(old_window_rectangle->x == rect[j].x &&
             old_window_rectangle->y == rect[j].y &&
             old_window_rectangle->width == rect[j].width &&
             old_window_rectangle->height == rect[j].height )
          {
            pos = i;
            break;
          }
        }
      }
      if(pos >= 0)
      {
        int undeleted_n = old_regions_n;
        XcolorRegionDelete( display, w, pos, 1 );
        old_regions = XcolorRegionFetch( display, w, &old_regions_n );
        if(undeleted_n - old_regions_n != 1)
          oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket,
                   OY_DBG_FORMAT_"removed %d; have still %d", OY_DBG_ARGS_,
                   pos, (int)old_regions_n );
      }

      /* upload the new or changed region to the X server */
      error = XcolorRegionInsert( display, w, 0, &region, 1 );
      xColorTarget = XInternAtom( display, "_ICC_COLOR_TARGET", True );
      XChangeProperty( display, w, xColorTarget, XA_STRING, 8,
                       PropModeReplace,
                       (unsigned char*) display_string, strlen(display_string));

      /* remember the old rectangle */
      oyRectangle_SetByRectangle( old_window_rectangle, window_rectangle );
    }

    /*XCloseDisplay( display );*/
    oyRectangle_Release( &display_rectangle );
    oyRectangle_Release( &window_rectangle );
    oyRectangle_Release( &old_window_rectangle );
  } else
    oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
         OY_DBG_FORMAT_"no window_id/display_id image tags found",OY_DBG_ARGS_);
# endif

  return error;
}

int  oydiFilterSocket_ImageDisplayInit(oyPixelAccess_s   * ticket,
                                       oyFilterSocket_s  * socket,
                                       oyImage_s         * image )
{
  int n = 0, i,m;
  int error = 0;
  oyFilterGraph_s * display_graph = 0;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node,
                 * src_node = 0,
                 * rectangles = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r;
  oyConfigs_s * devices = 0;
  char * tmp = 0,
       * ID = 0;

  if(oy_debug) 
    oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_"Init Start",
                 OY_DBG_ARGS_);

  input_node = node->plugs[0]->remote_socket_->node;

  ID = oydiFilterNode_ImageDisplayID( node );

  /* insert a "rectangles" filter to handle multiple monitors */
  rectangles = oyFilterNode_NewWith( "//" OY_TYPE_STD "/rectangles", 0, 0 );
  /* mark the new node as belonging to this node */
  oyOptions_SetFromText( &rectangles->tags, ID, "true", OY_CREATE_NEW );

  /* insert "rectangles" between "display" and its input_node */
  oyFilterNode_Disconnect( node->plugs[0] );
  error = oyFilterNode_Connect( input_node, "//" OY_TYPE_STD "/data",
                                rectangles, "//" OY_TYPE_STD "/data",0 );
  error = oyFilterNode_Connect( rectangles, "//" OY_TYPE_STD "/data",
                                node, "//" OY_TYPE_STD "/data",0 );



  /* obtain device informations, including geometry and ICC profiles
     from the according Oyranos module */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options,
                                 "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  o = oyOptions_Find( image->tags, "display_name" );
  o = oyOption_Copy( o, 0 );
  oyOptions_MoveIn( options, &o, -1 );
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  oyOptions_Release( &options );
  n = oyConfigs_Count( devices );
  o = oyOptions_Find( node->core->options_, "devices" );
  /* cache the devices scan result; currently is no updating implemented */
  if(!o)
  {
    oyOptions_MoveInStruct( &node->core->options_,
                            "//" OY_TYPE_STD "/display/devices",
                            (oyStruct_s**) &devices, OY_CREATE_NEW );
  }
  else
  {
    oyStruct_s * s = (oyStruct_s*) oyConfigs_Copy( devices, 0 );
    oyOption_StructMoveIn( o, &s );
  }
  oyOption_Release( &o );


  m = oyFilterNode_EdgeCount( rectangles, 1, OY_FILTEREDGE_CONNECTED );

  /* add new rectangles and colour CMMs as needed */
  if(n > m)
  {
    for(i = 0; i < n-m; ++i)
    {
      /* The first rectangle is the one provided by the user graph. */
      if(m != 0 || i != 0)
      {
        /*if(oyFilterRegistrationMatch( input_node->core->registration_,
                                      "//" OY_TYPE_STD "/icc", 0 ))*/
        if(input_node->plugs[0])
        {
          options = oyFilterNode_GetOptions( input_node, 0 );
          src_node = oyFilterNode_NewWith( input_node->core->registration_,
                                           options, 0 );
          error = oyStruct_ObserversCopy( (oyStruct_s*)src_node,
                                          (oyStruct_s*)input_node, 0 );
          oyOptions_Release( &options );
          
          /* mark the new node as belonging to this node */
          oyOptions_SetFromText( &src_node->tags, ID, "true",
                                 OY_CREATE_NEW );

          /* position the new CMM between the original CMMs input and 
             "rectangles" */
          error = oyFilterNode_Connect( src_node, "//" OY_TYPE_STD "/data",
                                      rectangles, "//" OY_TYPE_STD "/data", 0 );
          if(error > 0)
            oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                      "could not add  new CMM: %s\n",
                      OY_DBG_ARGS_,
                      input_node->core->registration_ );

          if(input_node->plugs[0] && 
             input_node->plugs[0]->remote_socket_->node != src_node)
            error = oyFilterNode_Connect( 
                                  input_node->plugs[0]->remote_socket_->node, 0,
                                        src_node, "//" OY_TYPE_STD "/data", 0 );

          /* clone into a new image */
          if(src_node->sockets[0]->data)
            src_node->sockets[0]->data->release( 
                                    (oyStruct_s**)&src_node->sockets[0]->data );
          src_node->sockets[0]->data = (oyStruct_s*)oyImage_CreateForDisplay (
                                                image->width, image->height,
                                                0, image->layout_[0],
                                                0, 0,0,0,0, 0 );
        } else
          error = oyFilterNode_Connect( input_node, "//" OY_TYPE_STD "/data",
                                      rectangles, "//" OY_TYPE_STD "/data", 0 );
      }
    }
  }


  m = oyOptions_CountType( rectangles->core->options_,
                           "//" OY_TYPE_STD "/rectangles/rectangle",
                           oyOBJECT_RECTANGLE_S );
  /* add missed rectangles */
  if(n > m)
  {
    tmp = oyAllocateFunc_(64);

    for(i = m; i < n;  ++i)
    {
      oySprintf_( tmp, "//" OY_TYPE_STD "/rectangles/rectangle/%d", i );

      r = oyRectangle_NewWith( 0., 0., 0., 0., 0);
      oyOptions_MoveInStruct( &rectangles->core->options_, tmp,
                              (oyStruct_s**)&r, OY_CREATE_NEW );
    }

    oyDeAllocateFunc_(tmp); tmp = 0;
  }


  /* describe all our newly created filters and add them to this node */
  display_graph = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( display_graph, rectangles, ID, 0 );
  oyOptions_MoveInStruct( &node->core->options_,
                          "//" OY_TYPE_STD "/display/display_graph",
                          (oyStruct_s**) &display_graph, OY_CREATE_NEW );



  if(oy_debug)
    oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket,
             OY_DBG_FORMAT_"  Init End", OY_DBG_ARGS_);

  free(ID); ID = 0;

  return error;
}

/** @func    oydiFilterPlug_ImageDisplayRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/17
 */
int      oydiFilterPlug_ImageDisplayRun(oyFilterPlug_s   * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int n = 0, i;
  int result = 0, l_result = 0, error = 0;
  oyFilterGraph_s * display_graph = 0;
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterNode_s * node = socket->node,
                 * rectangles = 0;
  oyImage_s * image = 0,
            * image_input = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r, * device_rectangle, * display_rectangle;
  oyRectangle_s roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0};
  oyConfigs_s * devices = 0;
  oyConfig_s * c = 0;
  oyProfile_s * p = 0;
  int display_pos_x,
      display_pos_y;
  int dirty = 0,
      init = 0;
  char * ID = 0;

  image = (oyImage_s*)socket->data;
  image_input = oyFilterPlug_ResolveImage( (oyFilterPlug_s *)node->plugs[0],
                                           socket, ticket );

  if(!image_input)
  {
    oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, 
             OY_DBG_FORMAT_"no input image found", OY_DBG_ARGS_);
    error = 1;
  }

  /* Allocate missing local process data in a specific manner. */
  if(error <= 0 && !image)
  {
    oyPixel_t pixel_layout = oyImage_GetPixelLayout( image_input );
    oyProfile_s * p_in = oyImage_GetProfile( image_input );
    oyDATATYPE_e data_type = oyToDataType_m(pixel_layout);
    int32_t datatype = -1;
    int32_t channels_in = oyToChannels_m(pixel_layout);
    int32_t colours_in = oyProfile_GetChannelsCount( p_in );
    /* keep extra channels */
    int32_t extra_in = channels_in - colours_in;
    int32_t alpha = -1;
    /* fixed RGB? */
    int32_t channels_out = 3 + extra_in;
    oyFilterNode_s * input_node = node->plugs[0]->remote_socket_->node;

    oyProfile_Release( &p_in );

    /* Release the input image copy from oyFilterPlug_ResolveImage(). */
    oyImage_Release( (oyImage_s**)&socket->data );


    pixel_layout &= (~oyChannels_m( oyToChannels_m(pixel_layout) ));
    pixel_layout |= oyChannels_m( channels_out );

    error = oyOptions_FindInt( node->core->options_, "datatype", 0, &datatype );
    if(error == 0)
    {
      oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                 "datatype opt found: %d",
                 OY_DBG_ARGS_, datatype);
      pixel_layout &= (~oyDataType_m(data_type));
      pixel_layout |= oyDataType_m( datatype );
    }
    error = oyOptions_FindInt( node->core->options_,"preserve_alpha",0, &alpha);
    if(error == 0)
    {
      oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
               OY_DBG_FORMAT_"preserve_alpha opt found: %d",
               OY_DBG_ARGS_, alpha);
      pixel_layout &= (~oyChannels_m( oyToChannels_m(pixel_layout) ));
      if(alpha && extra_in)
        pixel_layout |= oyChannels_m(3+alpha);
      else
        pixel_layout |= oyChannels_m(3);
    }

    /* eigther copy the input image with a oyObject_s argument or
     * create it as follows */
    image = oyImage_CreateForDisplay( image_input->width,
                                      image_input->height,
                                      0, pixel_layout,
                                      0, 0,0,0,0, 0 );
    oyFilterNode_SetData( node, (oyStruct_s*)image, 0, 0 );

    /* set as well the ICC node previous in the DAG */
    if(oyFilterRegistrationMatch( input_node->core->registration_,
                                      "//" OY_TYPE_STD "/icc", 0 ))
      oyFilterNode_SetData( input_node, (oyStruct_s*)image, 0, 0 );

    oyImage_Release( &image );
    image = (oyImage_s*)socket->data;
  }
  oyImage_Release( &image_input );


  ID = oydiFilterNode_ImageDisplayID( node );

  if(error <= 0)
  {
    /* display stuff */

    if(!node->core->options_)
      /* allocate options */
      node->core->options_ = oyOptions_New(0);

    /* obtain the local graph */
    display_graph = (oyFilterGraph_s*)oyOptions_GetType( node->core->options_,
                                  -1, "//" OY_TYPE_STD "/display/display_graph",
                                                      oyOBJECT_FILTER_GRAPH_S );

    if(!display_graph)
    {
      init = 1;

      /* init this filter */
      oydiFilterSocket_ImageDisplayInit( ticket, socket, image );

      display_graph = (oyFilterGraph_s*)oyOptions_GetType( node->core->options_,
                                  -1, "//" OY_TYPE_STD "/display/display_graph",
                                                      oyOBJECT_FILTER_GRAPH_S );
      error = !display_graph;
    }

    /* set server side rectangle */
    oydiFilterSocket_SetWindowRegion( ticket, image );

    /* look for our requisites */
    rectangles = oyFilterGraph_GetNode( display_graph, -1, "//" OY_TYPE_STD "/rectangles", ID );

    /* get cached devices */
    devices = (oyConfigs_s*)oyOptions_GetType( node->core->options_, -1, 
                                            "//" OY_TYPE_STD "/display/devices",
                                               oyOBJECT_CONFIGS_S );

    n = oyConfigs_Count( devices );
    if(!n || oyFilterNode_EdgeCount( rectangles, 1, OY_FILTEREDGE_CONNECTED ) < n)
      return 1;

    /* process all display rectangles */
    if(error <= 0)
    for(i = 0; i < n; ++i)
    {
      c = oyConfigs_Get( devices, i );

      /* get device dimension */
      o = oyConfig_Find( c, "device_rectangle" );
      device_rectangle = (oyRectangle_s *) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );
      oyOption_Release( &o );

      if(!device_rectangle)
      {
        oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket,
        OY_DBG_FORMAT_"device %d: Could not obtain \"device_rectangle\" option",
                 OY_DBG_ARGS_, i);
        continue;
      }

      /* get current work rectangle */
      r = (oyRectangle_s *) oyOptions_GetType( rectangles->core->options_, i, 
                                       "//" OY_TYPE_STD "/rectangles/rectangle",
                                               oyOBJECT_RECTANGLE_S );

      /* get display rectangle to project into */
      if(image)
        o = oyOptions_Find( image->tags, "display_rectangle" );
      display_rectangle = (oyRectangle_s *) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );
      oyOption_Release( &o );

      /* trim and adapt the work rectangle */
      oyRectangle_SetByRectangle( &roi_pix, display_rectangle );
      display_pos_x = roi_pix.x;
      display_pos_y = roi_pix.y;
      oyRectangle_Trim( &roi_pix, device_rectangle );
      roi_pix.x -= display_pos_x;
      roi_pix.y -= display_pos_y;
      if(oy_debug)
        oydi_msg( oyMSG_DBG, (oyStruct_s*)image,
            OY_DBG_FORMAT_"image %d: roi_pix:%s", OY_DBG_ARGS_,
            i, oyRectangle_Show(&roi_pix));

      /* all rectangles are relative to image dimensions */
      if(image && image->width != 0)
        oyRectangle_Scale( &roi_pix, 1./image->width );

      /* select actual image from the according  node */
      if(rectangles->plugs && rectangles->plugs[i] &&
         rectangles->plugs[i]->remote_socket_)
        image_input = (oyImage_s*)rectangles->plugs[i]->remote_socket_->data;
      else
      {
        image_input = 0;
        oydi_msg( oyMSG_WARN, (oyStruct_s*)image,
                 OY_DBG_FORMAT_"image %d: is missed roi_pix:%s",
                 OY_DBG_ARGS_, i, oyRectangle_Show( &roi_pix ) );
      }

      oyRectangle_SetByRectangle( r, &roi_pix );

      /* set the device profile of all CMM's image data */
      if(init)
      {
        oyOptions_s * options = 0;
        error = oyOptions_SetFromText( &options,
                               "//"OY_TYPE_STD"/config/x_color_region_target",
                                       "yes", OY_CREATE_NEW );
        error = oyDeviceGetProfile( c, options, &p );
        oyOptions_Release( &options );

        if(p && image_input && !oyProfile_Equal( image_input->profile_, p ))
        {
          oyImage_SetCritical( image_input, 0, p, 0 );
          error = oyOptions_SetFromText( &ticket->graph->options,
                     "//" OY_TYPE_STD "/profile/dirty", "true", OY_CREATE_NEW );
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
      oyFilterNode_Release( &rectangles );
      result = dirty;
      goto clean;
    }


    /* make the graph flow: process the upstream "rectangles" node */
    l_result = rectangles->api7_->oyCMMFilterPlug_Run( node->plugs[0], ticket );
    if(l_result > 0 || result == 0) result = l_result;
  }

  clean:
  oyFilterNode_Release( &rectangles );
  if(ID) free(ID);    

  return result;
}

/** 
 *  @brief   the supported options for "oydi"
 *
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/05 (Oyranos: 0.1.10)
 *  @date    2009/08/05
 */
char oydi_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" "display." CMM_NICK ">\n\
      <display_id.invisible></display_id.invisible>\n\
      <window_id.invisible></window_id.invisible>\n\
      <display_rectangle.invisible></display_rectangle.invisible>\n\
      <datatype.advanced.invisible></datatype.advanced.invisible>\n\
      <preserve_alpha.advanced.invisible></preserve_alpha.advanced.invisible>\n\
     </" "display." CMM_NICK ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

int  oydiUiGet                       ( oyOptions_s       * opts,
                                       char             ** xforms_layout,
                                       oyAlloc_f           allocateFunc )
{
  char * text = (char*)allocateFunc(5);
  text[0] = 0;
  *xforms_layout = text;
  return 0;
}




oyDATATYPE_e oyx1_data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                   oyHALF, oyFLOAT, oyDOUBLE, 0};

oyConnectorImaging_s oyx1_Display_plug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/splitter.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
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
oyConnectorImaging_s *oyx1_Display_plugs[2] = {&oyx1_Display_plug,0};

oyConnectorImaging_s oyx1_Display_socket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
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
oyConnectorImaging_s *oyx1_Display_sockets[2] = {&oyx1_Display_socket,0};


#define OY_IMAGE_DISPLAY_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "display"
/** @instance oydi_api7_image_display
 *  @brief    oydi oyCMMapi7_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi7_s   oydi_api7_image_display = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fields; keep to zero */
  (oyCMMapi_s*) 0, /* oyCMMapi_s * next */
  
  oydiCMMInit, /* oyCMMInit_f */
  oydiCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oydiFilterPlug_ImageDisplayRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyx1_Display_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyx1_Display_sockets,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

const char * oydiApi4UiImageDisplayGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return "display";
    else if(type == oyNAME_NAME)
      return "Display";
    else if(type == oyNAME_DESCRIPTION)
      return "Display Splitter Object";
  } else if(strcmp(select,"help"))
  {
    /* The help text is identical, as the module contains only one filter to
     * provide help for. */
    return oydiGetText(select,type,context);
  }
  return 0;
}
const char * oydi_api4_ui_image_display_texts[] = {"name", "help", 0};

/** @instance oydi_api4_ui_image_display
 *  @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image display.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/09/06
 */
oyCMMui_s oydi_api4_ui_image_display = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,3,0},                            /**< int32_t module_api[3] */

  oydiFilter_ImageDisplayValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oydiWidgetEvent, /* oyWidgetEvent_f */

  "Graph/Display", /* category */
  oydi_extra_options,   /* const char * options */

  oydiUiGet,    /* oyCMMuiGet_f oyCMMuiGet */

  oydiApi4UiImageDisplayGetText,  /* oyCMMGetText_f getText */
  oydi_api4_ui_image_display_texts  /* (const char**)texts */
};

/** @instance oydi_api4_image_display
 *  @brief    oydi oyCMMapi4_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi4_s   oydi_api4_image_display = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fields; keep to zero */
  (oyCMMapi_s*) & oydi_api7_image_display, /* oyCMMapi_s * next */
  
  oydiCMMInit, /* oyCMMInit_f */
  oydiCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oydiFilterNode_ImageDisplayContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  &oydi_api4_ui_image_display          /**< filter UI */
};

/* OY_IMAGE_DISPLAY_REGISTRATION ---------------------------------------------*/



/** Function oydiCMMInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/13 (Oyranos: 0.1.10)
 *  @date    2009/12/17
 */
int                oydiCMMInit       ( oyStruct_s        * filter )
{
  int error = 0;
  return error;
}

/** Function oydiCMMMessageFuncSet
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/13 (Oyranos: 0.1.10)
 *  @date    2009/04/13
 */
int            oydiCMMMessageFuncSet ( oyMessage_f         message_func )
{
  oydi_msg = message_func;
  return 0;
}

/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/04/12
 */
const char * oydiGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Oyranos display filter");
    else
      return _("The client side window data handler of Oyranos.");
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
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("The \"display\" filter supports applications to show image content on single and multi monitor displays. It cares about the server communication in declaring the region as prematched. So a X11 server side colour correction does not disturb the displayed colours and omits the provided rectangle. The \"display\" filter matches the provided image content to each monitor it can find. Of course this has limitations to distorted windows, like wobbly effects or matrix deformed windows.");
    else
      return _("The filter needs some informations attached to the output image tags of the \"output\" image filter. The following list describes the X11/Xorg requirements.\n A \"window_id\" option shall consist of a oyBlob_s object containing the X11 \"Window\" type in its pointer element.\n A \"display_id\" option shall consist of a oyBlob_s object containing the X11 \"Display\" of the application. This is typically exposed as system specific pointer by each individual toolkit.\n A \"display_rectangle\" option of type oyRectangle_s shall represent the application image region in pixel of the absolute display coordinates. \n In the \"datatype\" option, a oyDATATYPE_e encoded as integer is expected, to deliver that data type in a not yet allocated output image. The output data type is by default not changed. A newly not yet allocated output image will be stored as processing data in the socket.\n \"preserve_alpha\" is a integer option to keep a given alpha in a not yet allocated output image. \
");
  }
  return 0;
}

const char *oydi_texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance oydi_cmm_module
 *  @brief    oydi module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/00 (Oyranos: 0.1.10)
 *  @date    2009/01/00
 */
oyCMMInfo_s oydi_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.2",
  oydiGetText,                         /**< oyCMMInfoGetText_f getText */
  (char**)oydi_texts,                  /**<texts; list of arguments to getText*/

  OYRANOS_VERSION,

  (oyCMMapi_s*) & oydi_api4_image_display,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

