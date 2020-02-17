/** @file oyranos_cmm_oydi.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2009/02/23
 */

#include "oyCMM_s.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMui_s_.h"
#include "oyConnectorImaging_s_.h"
#include "oyFilterNode_s_.h"         /* for oyFilterNode_TextToInfo_ */
#include "oyRectangle_s_.h"
#include "oyProfiles_s.h"

#include "oyranos_config_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_generic.h"         /* oy_connector_imaging_static_object */
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_effect.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(HAVE_XCM)
#include <X11/Xcm/XcmVersion.h>
#endif

/* --- internal definitions --- */

#define CMM_NICK "oydi"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/* OY_IMAGE_DISPLAY_REGISTRATION */

int                oydiCMMInit       ( oyStruct_s        * filter );
int                oydiCMMReset      ( oyStruct_s        * filter );
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

extern oyCMMapi4_s_   oydi_api4_image_display;
extern oyCMMapi7_s_   oydi_api7_image_display;



/* OY_IMAGE_DISPLAY_REGISTRATION ---------------------------------------------*/

#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
#include <X11/Xcm/Xcm.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#endif


oyWIDGET_EVENT_e   oydiWidgetEvent   ( oyOptions_s       * options OY_UNUSED,
                                       oyWIDGET_EVENT_e    type OY_UNUSED,
                                       oyStruct_s        * event OY_UNUSED )
{return 0;}

oyOptions_s* oydiFilter_ImageDisplayValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate OY_UNUSED,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}

/** @brief   implement oyCMMFilter_ContextToMem_f()
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
  return oyFilterNode_TextToInfo_( (oyFilterNode_s_*)node, size, allocateFunc );
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
  oyOptions_s * tags = oyImage_GetTags( image );
  const char * display_name = oyOptions_FindString( tags, "display_name", 0 );

  win_id = (oyBlob_s*) oyOptions_GetType( tags, -1, "window_id",
                                          oyOBJECT_BLOB_S );
  display_id = (oyBlob_s*) oyOptions_GetType( tags, -1, "display_id",
                                          oyOBJECT_BLOB_S );

  if(display_name && strcmp(display_name,"oy-test") == 0)
  {
  } else
# if defined(XCM_HAVE_X11) && defined (HAVE_XCM)
  if(win_id && display_id)
  {
    int x,y, i,j;
    Atom xColorTarget;
    Window w = (Window) oyBlob_GetPointer(win_id), w_return;
    XWindowAttributes attr;
    Display * display = 
#if 0
                         XOpenDisplay( display_name ); /* + XCloseDisplay() */
#else
                         (Display *) oyBlob_GetPointer(display_id);
#endif
    oyRectangle_s * display_rectangle = (oyRectangle_s*) oyOptions_GetType( 
                                       tags, -1, "display_rectangle",
                                       oyOBJECT_RECTANGLE_S );
    oyRectangle_s * old_window_rectangle = (oyRectangle_s*) oyOptions_GetType(
                                       tags, -1, "old_window_rectangle",
                                       oyOBJECT_RECTANGLE_S ),
                  * window_rectangle = 0;
#ifdef DEBUG
    char * tmp = oyStringCopy_( oyRectangle_Show(display_rectangle), oyAllocateFunc_);
    if(oy_debug > 2)
    oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
             OY_DBG_FORMAT_"Display: %s Window id: %d  display_rectangle:%s old_window_rectangle:%s", OY_DBG_ARGS_,
             display_name, w, tmp, oyRectangle_Show( old_window_rectangle ) );
    oyFree_m_( tmp );
#endif

    if(!old_window_rectangle)
    {
      old_window_rectangle = oyRectangle_NewFrom( 0,0 );

      oyOptions_MoveInStruct( &tags,
                            "//" OY_TYPE_STD "/display/old_window_rectangle",
                            (oyStruct_s**) &old_window_rectangle, OY_CREATE_NEW );
      old_window_rectangle = (oyRectangle_s*) oyOptions_GetType(
                                      tags, -1, "old_window_rectangle",
                                      oyOBJECT_RECTANGLE_S );
    }

    /* We need window relative coordinates. (Works not everywhere? - FVWM) */
    XGetWindowAttributes( display, w, &attr );
    XTranslateCoordinates( display, w, attr.root, 
                                  -attr.border_width, -attr.border_width,
                                  &x, &y, &w_return);
    if(oy_debug > 2)
    oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
               OY_DBG_FORMAT_"Display: %s Window id: %d  display_rectangle:%s @+%d+%d",
               OY_DBG_ARGS_,
               display_name, (int)w, oyRectangle_Show(display_rectangle), x,y );

    window_rectangle = oyRectangle_NewFrom( display_rectangle, 0 );
    *oyRectangle_SetGeo1( window_rectangle, 0 ) -= x;
    *oyRectangle_SetGeo1( window_rectangle, 1 ) -= y;

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

      rec[0].x = oyRectangle_GetGeo1( window_rectangle, 0 );
      rec[0].y = oyRectangle_GetGeo1( window_rectangle, 1 );
      rec[0].width = oyRectangle_GetGeo1( window_rectangle, 2 );
      rec[0].height = oyRectangle_GetGeo1( window_rectangle, 3 );

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
      } else if(oy_debug > 2)
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
      for(i = 0; (unsigned)i < old_regions_n; ++i)
      {

        if(!old_regions[i].region || pos >= 0)
          break;

        rect = XFixesFetchRegion( display, ntohl(old_regions[i].region),
                                  &nRect );

        for(j = 0; j < nRect; ++j)
        {
          if(oyRectangle_GetGeo1(old_window_rectangle,0) == rect[j].x &&
             oyRectangle_GetGeo1(old_window_rectangle,1) == rect[j].y &&
             oyRectangle_GetGeo1(old_window_rectangle,2) == rect[j].width &&
             oyRectangle_GetGeo1(old_window_rectangle,3) == rect[j].height )
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
  } else if(oy_debug > 2)
  {
    oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
              OY_DBG_FORMAT_"no window_id/display_id image tags found image:%d",
              OY_DBG_ARGS_,
              oyStruct_GetId((oyStruct_s*)image) );
  } else
# endif
    oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
              OY_DBG_FORMAT_"display_name: %s",
              OY_DBG_ARGS_,
              oyNoEmptyString_m_(display_name) );

  oyBlob_Release( &win_id );
  oyBlob_Release( &display_id );
  oyOptions_Release( &tags );

  return error;
}

int  oydiFilterSocket_ImageDisplayInit(oyPixelAccess_s   * ticket,
                                       oyFilterSocket_s  * socket,
                                       oyImage_s         * image )
{
  int n = 0, i,m;
  int error = 0;
  oyFilterNode_s * input_node = NULL,
                 * node = oyFilterSocket_GetNode(socket),
                 * src_node = NULL,
                 * rectangles = NULL;
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyOptions_s * options = NULL,
              * image_tags = oyImage_GetTags( image ),
              * rectangles_tags = NULL,
              * rectangles_options = NULL,
              * node_options = oyFilterNode_GetOptions( node, 0 );
  oyOption_s * o = NULL;
  oyRectangle_s * r = NULL;
  oyConfigs_s * devices = NULL;
  char * tmp = NULL,
       * ID = NULL;
  int icc_profile_flags = 0;
  const char * display_name = oyOptions_FindString( image_tags, "display_name", 0 );

  input_node = oyFilterPlug_GetRemoteNode( plug );
  oyFilterPlug_Release( &plug );

  if(oy_debug > 2) 
    oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_"Init Start behind %s",
              OY_DBG_ARGS_, oyFilterNode_GetRegistration( input_node ));

  ID = oydiFilterNode_ImageDisplayID( node );

  icc_profile_flags = oyICCProfileSelectionFlagsFromRegistration(
                                  oyFilterNode_GetRegistration( input_node ) );

  /* insert a "rectangles" filter to handle multiple monitors */
  rectangles = oyFilterNode_NewWith( "//" OY_TYPE_STD "/rectangles", 0, 0 );
  /* mark the new node as belonging to this node */
  rectangles_tags = oyFilterNode_GetTags( rectangles );
  oyOptions_SetFromString( &rectangles_tags, ID, "true", OY_CREATE_NEW );
  oyOptions_Release( &rectangles_tags );

  /* insert "rectangles" between "display" and its input_node */
  oyFilterNode_Disconnect( node, 0 );
  error = oyFilterNode_Connect( input_node, "//" OY_TYPE_STD "/data",
                                rectangles, "//" OY_TYPE_STD "/data",0 );
  error = oyFilterNode_Connect( rectangles, "//" OY_TYPE_STD "/data",
                                node, "//" OY_TYPE_STD "/data",0 );

  /* obtain device informations, including geometry and ICC profiles
     from the according Oyranos module */
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromString( &options,
                                 "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  o = oyOptions_Find( image_tags, "display_name", oyNAME_PATTERN );
  oyOptions_MoveIn( options, &o, -1 );

  /* create 3 test devices in a row */
  if(display_name && strcmp(display_name,"oy-test") == 0)
  {
    const char * json_dev = "\
{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"openicc\": {\n\
        \"device\": {\n\
          \"monitor\": [{\n\
              \"device_name\": \":0.0\",\n\
              \"prefix\": \"EDID_\",\n\
              \"EDID_manufacturer\": \"Oyranos CMS\",\n\
              \"manufacturer\": \"Oyranos CMS\",\n\
              \"EDID_mnft\": \"OYR\",\n\
              \"EDID_model\": \"Test Monitor\",\n\
              \"model\": \"Test Monitor\",\n\
              \"display_geometry\": \"1024x768+0+0\",\n\
              \"system_port\": \"eDP1\",\n\
              \"host\": \"myhost\",\n\
              \"EDID_date\": \"2013-T50\",\n\
              \"EDID_mnft_id\": \"1234\",\n\
              \"EDID_model_id\": \"5678\",\n\
              \"EDID_red_x\": \"0.65332\",\n\
              \"EDID_red_y\": \"0.333984\",\n\
              \"EDID_green_x\": \"0.299805\",\n\
              \"EDID_green_y\": \"0.620117\",\n\
              \"EDID_blue_x\": \"0.146484\",\n\
              \"EDID_blue_y\": \"0.0498047\",\n\
              \"EDID_white_x\": \"0.3125\",\n\
              \"EDID_white_y\": \"0.329102\",\n\
              \"EDID_gamma\": \"2.2\",\n\
              \"color_matrix.from_edid.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\": \"0.65332,0.333984,0.299805,0.620117,0.146484,0.0498047,0.3125,0.329102,2.2\"\n\
            }\n\
          ]\n\
        }\n\
      }\n\
    }\n\
  }\n\
}\
";
    oyConfig_s * device = NULL;
    oyOptions_s * options = NULL;
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                                                    "//" OY_TYPE_STD "/icc_color",
                                                                    options, 0 );

    devices = oyConfigs_New(0);
    for( i = 0; i < 3; ++i )
    {
#define MONITOR_REGISTRATION_BASE OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "device" OY_SLASH "config.icc_profile.monitor."
#define MONITOR_REGISTRATION MONITOR_REGISTRATION_BASE "oyMo"
      oyRectangle_s * rect = oyRectangle_NewWith( i*1024,0,1024,768, 0 );
      oyProfile_s * p = oyProfile_FromName( i==0?"compatibleWithAdobeRGB1998.icc":i==1?"lab":"xyz", icc_profile_flags, NULL );
      oyDeviceFromJSON( json_dev, 0, &device );

      o = oyOption_FromRegistration( MONITOR_REGISTRATION OY_SLASH
                                                  "device_rectangle", 0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &rect );
      oyOptions_MoveIn( *oyConfig_GetOptions(device,"data"), &o, -1 );
      
      o = oyOption_FromRegistration( MONITOR_REGISTRATION OY_SLASH
                                                  "icc_profile", 0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
      oyOptions_MoveIn( *oyConfig_GetOptions(device,"data"), &o, -1 );

      oyConfigs_MoveIn( devices, &device, -1 );
    }

  } else
    error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  oyOptions_Release( &options );
  n = oyConfigs_Count( devices );
  o = oyOptions_Find( node_options, "devices", oyNAME_PATTERN );
  /* cache the devices scan result; currently is no updating implemented */
  if(!o)
  {
    oyOptions_MoveInStruct( &node_options,
                            "//" OY_TYPE_STD "/display/devices",
                            (oyStruct_s**) &devices, OY_CREATE_NEW );
  }
  else
  {
    oyStruct_s * s = (oyStruct_s*) oyConfigs_Copy( devices, 0 );
    oyOption_MoveInStruct( o, &s );
  }
  oyOption_Release( &o );


  m = oyFilterNode_EdgeCount( rectangles, 1, OY_FILTEREDGE_CONNECTED );

  /* add new rectangles and color CMMs as needed */
  if(n > m)
  {
    for(i = 0; i < n-m; ++i)
    {
      /* The first rectangle is the one provided by the user graph. */
      if(m != 0 || i != 0)
      {
        oyFilterPlug_s * input_node_plug = oyFilterNode_GetPlug( input_node, 0);
        /*if(oyFilterRegistrationMatch( input_node->core->registration_,
                                      "//" OY_TYPE_STD "/icc_color", 0 ))*/
        if(input_node_plug)
        {
          oyOptions_s * src_node_tags;

          options = oyFilterNode_GetOptions( input_node, 0 );
          src_node = oyFilterNode_NewWith( oyFilterNode_GetRegistration(input_node),
                                           options, 0 );
          error = oyStruct_ObserversCopy( (oyStruct_s*)src_node,
                                          (oyStruct_s*)input_node, 0 );
          oyOptions_Release( &options );
          
          /* mark the new node as belonging to this node */
          src_node_tags = oyFilterNode_GetTags( src_node );
          oyOptions_SetFromString( &src_node_tags, ID, "true",
                                 OY_CREATE_NEW );
          oyOptions_Release( &src_node_tags );

          /* position the new CMM between the original CMMs input and 
             "rectangles" */
          error = oyFilterNode_Connect( src_node, "//" OY_TYPE_STD "/data",
                                      rectangles, "//" OY_TYPE_STD "/data", 0 );
          if(error > 0)
            oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                      "could not add  new CMM: %s\n",
                      OY_DBG_ARGS_,
                      oyFilterNode_GetRegistration(input_node) );

          /* reconnect to the ancestor */
          if(input_node_plug)
          {
            oyFilterNode_s * input_node_remote_node =
                                  oyFilterPlug_GetRemoteNode( input_node_plug );
            if(input_node_remote_node != src_node)
              error = oyFilterNode_Connect( input_node_remote_node, 0,
                                        src_node, "//" OY_TYPE_STD "/data", 0 );
            oyFilterNode_Release( &input_node_remote_node );
          }

          /* clone into a new image */
          {
            int width = oyImage_GetWidth(image),
                height = oyImage_GetHeight(image);
            oyImage_s * display_image = oyImage_CreateForDisplay (
                                                width, height, 0,
                                        oyImage_GetPixelLayout(image, oyLAYOUT),
                                                0, 0,0,0,0,
                                                icc_profile_flags, 0 );
            oyFilterNode_SetData( src_node, (oyStruct_s*)display_image, 0,0 );
            oyImage_Release( &display_image );
          }
          /* oyFilterNode_Release( &src_node ); // It would destroy the node */
        } else
          error = oyFilterNode_Connect( input_node, "//" OY_TYPE_STD "/data",
                                      rectangles, "//" OY_TYPE_STD "/data", 0 );
        oyFilterPlug_Release( &input_node_plug );
      }
    }
  }

  rectangles_options = oyFilterNode_GetOptions( rectangles, 0 );
  m = oyOptions_CountType( rectangles_options,
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
      oyOptions_MoveInStruct( &rectangles_options, tmp,
                              (oyStruct_s**)&r, OY_CREATE_NEW );
    }

    oyDeAllocateFunc_(tmp); tmp = 0;
  }


  /* describe all our newly created filters and add them to this node */
  oyOptions_MoveInStruct( &node_options,
                          "//" OY_TYPE_STD "/display/rectangles_options",
                          (oyStruct_s**) &rectangles_options, OY_CREATE_NEW );


  if(oy_debug > 2)
    oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket,
             OY_DBG_FORMAT_"  Init End", OY_DBG_ARGS_);

  if(ID) { free(ID); ID = NULL; }

  oyFilterNode_Release( &node );
  oyFilterNode_Release( &input_node );
  oyOptions_Release( &node_options );
  oyOptions_Release( &rectangles_options );
  oyOptions_Release( &image_tags );

  return error;
}


/** @brief   wrap XcmColorServerCapabilities
 *
 *  @version Oyranos: 0.9.2
 *  @since   2012/12/20 (Oyranos: 0.9.2)
 *  @date    2012/12/20
 */
int      oydiColorServerActive( oyBlob_s * display_id )
{
  static int active = 0;
#if defined(XCM_HAVE_X11)
  static double z = 0;
  Display * display = (Display *) oyBlob_GetPointer(display_id);
  if(z + 1.0 < oySeconds())
  {
    active = XcmColorServerCapabilities( display );
    z = oySeconds();
    if(oy_debug)
      DBG2_S("color server active: %d %g\n", active, z);
  }
#endif
  return active;
}

/** @brief   implement oyCMMFilter_GetNext_f()
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
  oyFilterSocket_s * socket = oyFilterPlug_GetSocket( requestor_plug );
  oyFilterNode_s * node = oyFilterSocket_GetNode(socket),
                 * rectangles = 0;
  oyOptions_s * node_options = oyFilterNode_GetOptions( node, 0 ),
              * rectangles_options = NULL,
              * image_tags = NULL;
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyImage_s * image = 0,
            * image_input = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = NULL, * device_rectangle = NULL, * display_rectangle;
  oyRectangle_s_ roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0,0,0,0,0};
  oyConfigs_s * devices = 0;
  oyConfig_s * c = 0;
  oyProfile_s * p = 0;
  int display_pos_x,
      display_pos_y;
  int dirty = 0,
      init = 0,
      test = 0;
  char * ID = 0;
  int icc_profile_flags = 0;
  int width = 0, height = 0;
  int display_white_point = 0;

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  image_input = oyFilterPlug_ResolveImage( plug, socket, ticket );

  if(!image_input)
  {
    oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, 
             OY_DBG_FORMAT_"no input image found", OY_DBG_ARGS_);
    error = 1;
  }

  /* Allocate missing local process data in a specific manner. */
  if(error <= 0 && !image)
  {
    oyPixel_t pixel_layout = oyImage_GetPixelLayout( image_input, oyLAYOUT );
    oyProfile_s * p_in = oyImage_GetProfile( image_input );
    oyDATATYPE_e data_type = oyToDataType_m(pixel_layout);
    int32_t datatype = -1;
    int32_t channels_in = oyToChannels_m(pixel_layout);
    int32_t colors_in = oyProfile_GetChannelsCount( p_in );
    /* keep extra channels */
    int32_t extra_in = channels_in - colors_in;
    int32_t alpha = -1;
    /* fixed RGB? */
    int32_t channels_out = 3 + extra_in;
    oyFilterNode_s * input_node = oyFilterPlug_GetRemoteNode( plug );

    width = oyImage_GetWidth( image_input );
    height = oyImage_GetHeight( image_input );

    oyProfile_Release( &p_in );

    /* Release the input image copy from oyFilterPlug_ResolveImage(). */
    oyFilterSocket_SetData( socket, NULL );

    pixel_layout &= (~oyChannels_m( oyToChannels_m(pixel_layout) ));
    pixel_layout |= oyChannels_m( channels_out );

    oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
              "ping", OY_DBG_ARGS_);
    error = oyOptions_FindInt( node_options, "datatype", 0, &datatype );
    if(error == 0)
    {
      if(oy_debug > 2)
      oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                 "datatype opt found: %d",
                 OY_DBG_ARGS_, datatype);
      pixel_layout &= (~oyDataType_m(data_type));
      pixel_layout |= oyDataType_m( datatype );
    }
    error = oyOptions_FindInt( node_options,"preserve_alpha",0, &alpha);
    if(error == 0)
    {
      if(oy_debug > 2)
      oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
               OY_DBG_FORMAT_"preserve_alpha opt found: %d",
               OY_DBG_ARGS_, alpha);
      pixel_layout &= (~oyChannels_m( oyToChannels_m(pixel_layout) ));
      if(alpha && extra_in)
        pixel_layout |= oyChannels_m(3+alpha);
      else
        pixel_layout |= oyChannels_m(3);
    }

    icc_profile_flags = oyICCProfileSelectionFlagsFromRegistration(
                                  oyFilterNode_GetRegistration( input_node ) );

    /* eigther copy the input image with a oyObject_s argument or
     * create it as follows */
    image = oyImage_CreateForDisplay( width,
                                      height,
                                      0, pixel_layout,
                                      0, 0,0,0,0,
                                      icc_profile_flags, 0 );
    oyFilterNode_SetData( node, (oyStruct_s*)image, 0, 0 );

    /* set as well the ICC node previous in the DAG */
    if(oyFilterRegistrationMatch( oyFilterNode_GetRegistration(input_node),
                                      "//" OY_TYPE_STD "/icc_color", 0 ))
      oyFilterNode_SetData( input_node, (oyStruct_s*)image, 0, 0 );

    oyImage_Release( &image );
    image = (oyImage_s*)oyFilterSocket_GetData(socket);

    oyFilterNode_Release( &input_node );
  }
  oyImage_Release( &image_input );


  ID = oydiFilterNode_ImageDisplayID( node );

  if(error <= 0)
  {
    /* display stuff */

    /* obtain the rectangles */
    rectangles_options = (oyOptions_s*)oyOptions_GetType( node_options,
                                  -1, "//" OY_TYPE_STD "/display/rectangles_options",
                                                      oyOBJECT_OPTIONS_S );

    if(!rectangles_options)
    {
      oyOptions_s * tags = oyImage_GetTags( image );
      const char * display_name = oyOptions_FindString( tags,
                                                        "display_name", 0 );
      if(display_name && strcmp(display_name,"oy-test") == 0)
        test = 1;


      if(oy_debug)
        oydi_msg( oyMSG_DBG, (oyStruct_s*)image,
                    OY_DBG_FORMAT_"no rectangles_options (display_name=%s)", OY_DBG_ARGS_,
                    oyNoEmptyString_m_(display_name) );

# if defined(XCM_HAVE_X11) && defined (HAVE_XCM)
      if(test == 0)
      {
        oyBlob_s * display_id = (oyBlob_s*) oyOptions_GetType( tags, -1,
                                              "display_id",
                                              oyOBJECT_BLOB_S );
        if(!display_id)
        {
          oyFilterNode_s * input_node = oyFilterNode_GetPlugNode( node, 0 );
          oydi_msg( oyMSG_DBG, (oyStruct_s*)image,
                    OY_DBG_FORMAT_"no display_id", OY_DBG_ARGS_ );
          /* make the graph flow: process the upstream node */
          l_result = oyFilterNode_Run( input_node, plug, ticket );
          if(l_result > 0 || result == 0) result = l_result;

          oyFilterPlug_Release( &plug );
          oyFilterNode_Release( &input_node );
          oyImage_Release( &image );
          oyOptions_Release( &node_options );
          oyOptions_Release( &tags );
          goto clean2;
        }
        oyBlob_Release( &display_id );
      }
#endif
      oyOptions_Release( &tags );

      init = 1;

      /* init this filter */
      oydiFilterSocket_ImageDisplayInit( ticket, socket, image );

      rectangles_options = (oyOptions_s*)oyOptions_GetType( node_options,
                                  -1, "//" OY_TYPE_STD "/display/rectangles_options",
                                                      oyOBJECT_OPTIONS_S );
      error = !rectangles_options;
    }

    /* set server side rectangle */
    oydiFilterSocket_SetWindowRegion( ticket, image );


    /* get cached devices */
    devices = (oyConfigs_s*)oyOptions_GetType( node_options, -1, 
                                            "//" OY_TYPE_STD "/display/devices",
                                               oyOBJECT_CONFIGS_S );

    n = oyConfigs_Count( devices );
    int count = oyOptions_CountType( rectangles_options, "rectangles_options", oyOBJECT_RECTANGLE_S );
    if(!n || count < n)
    {
      dirty = 1;
      goto clean2;
    }

    /* process all display rectangles */
    if(error <= 0)
    for(i = 0; i < n; ++i)
    {
      oyOptions_s * f_options = NULL;

      c = oyConfigs_Get( devices, i );

      /* get device dimension */
      o = oyConfig_Find( c, "device_rectangle" );
      device_rectangle = (oyRectangle_s *) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );
      oyOption_Release( &o );

      if(!device_rectangle)
      {
        oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket,
        OY_DBG_FORMAT_"device %d: Could not obtain \"device_rectangle\" option",
                 OY_DBG_ARGS_, i);
        oyConfig_Release( &c );
        continue;
      }

      /* get current work rectangle */
      r = (oyRectangle_s *) oyOptions_GetType( rectangles_options, i, 
                                       "//" OY_TYPE_STD "/rectangles/rectangle",
                                               oyOBJECT_RECTANGLE_S );

      /* get display rectangle to project into */
      image_tags = oyImage_GetTags( image );
      o = oyOptions_Find( image_tags, "display_rectangle", oyNAME_PATTERN );
      display_rectangle = (oyRectangle_s *) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );
      oyOption_Release( &o );
      oyOptions_Release( &image_tags );

      /* trim and adapt the work rectangle */
      oyRectangle_SetByRectangle( (oyRectangle_s*)&roi_pix, display_rectangle );
      oyRectangle_Release( &display_rectangle );
      display_pos_x = roi_pix.x;
      display_pos_y = roi_pix.y;
      oyRectangle_Trim( (oyRectangle_s*)&roi_pix, device_rectangle );
      roi_pix.x -= display_pos_x;
      roi_pix.y -= display_pos_y;
      if(oy_debug)
        oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
            OY_DBG_FORMAT_"image %d: roi_pix:%s", OY_DBG_ARGS_,
            i, oyRectangle_Show((oyRectangle_s*)&roi_pix));

      /* the rectangles are in pixel dimension */
      width = oyImage_GetWidth(image);
      if(image && width != 0)
        oyRectangle_SetByRectangle( r, (oyRectangle_s*)&roi_pix );

      /* select actual image from the according input node */
      {
        oyFilterPlug_s * rectangles_plug = oyFilterNode_GetPlug( rectangles, i );
        oyFilterSocket_s * rectangles_plug_socket = oyFilterPlug_GetSocket( rectangles_plug );
        oyFilterNode_s   * node = oyFilterSocket_GetNode( rectangles_plug_socket );
        int                flags = 0;
        image_input = (oyImage_s*)oyFilterSocket_GetData( rectangles_plug_socket );
        oyFilterSocket_Release( &rectangles_plug_socket );
        oyFilterPlug_Release( &rectangles_plug );

        /* obtain filter options */
        f_options = oyFilterNode_GetOptions( node, flags );

        oyFilterNode_Release( &node );
      }
      if(!image_input)
      {
        oydi_msg( oyMSG_WARN, (oyStruct_s*)image,
                 OY_DBG_FORMAT_"image %d: is missed roi_pix:%s",
                 OY_DBG_ARGS_, i, oyRectangle_Show( (oyRectangle_s*)&roi_pix ) );
      }

      /* set the device profile of all CMM's image data */
      if(init)
      {
        oyProfile_s * image_input_profile = oyImage_GetProfile( image_input );
        oyOptions_s * options = 0;
        int color_server_active = 0;
# if defined(XCM_HAVE_X11) && defined (HAVE_XCM)
        oyOptions_s * tags = oyImage_GetTags( image );
        oyBlob_s * display_id = (oyBlob_s*) oyOptions_GetType( tags, -1, "display_id",
                                          oyOBJECT_BLOB_S );
        if(!display_id && test == 0)
          oydi_msg( oyMSG_ERROR, (oyStruct_s*)image,
            OY_DBG_FORMAT_"no display_id", OY_DBG_ARGS_ );
        if(test == 0)
          color_server_active = oydiColorServerActive( display_id );
        oyOptions_Release( &tags );
        oyBlob_Release( &display_id );
        if(color_server_active & XCM_COLOR_SERVER_REGIONS)
#endif
        error = oyOptions_SetFromString( &options,
                               "//" OY_TYPE_STD "/config/x_color_region_target",
                                       "yes", OY_CREATE_NEW );
        error = oyDeviceGetProfile( c, options, &p );
        oyOptions_Release( &options );

        if(p && image_input && !oyProfile_Equal( image_input_profile, p ))
        {
          oyImage_SetCritical( image_input, 0, p, 0, -1,-1 );
          error = oyPixelAccess_SetFromString ( ticket,
                     "//" OY_TYPE_STD "/profile/dirty", "true", OY_CREATE_NEW );
          ++dirty;
        } else if(oy_debug)
          oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket,
             OY_DBG_FORMAT_"%s[%d](%s) profile already set: %s", OY_DBG_ARGS_,
             _("Image"), oyStruct_GetId((oyStruct_s*)image_input),
             oyStruct_GetInfo((oyStruct_s*)image_input,0,0),
             oyProfile_GetText(p,oyNAME_NAME) );

        oyProfile_Release( &image_input_profile );
        oyRectangle_Release( &r );

        /* automatic disable the local white point handling */
        if(!color_server_active && o)
        {
          /* display_white_point */
          oyOption_SetFromString( o, "0", 0 );
          oyOption_SetFlags( o, oyOption_GetFlags(o) | oyOPTIONATTRIBUTE_AUTOMATIC );
        }

        /* Ensure the effect profile is not set twice: once in the global VCGT
         * and second here in the local transform. */
        {
          oyOption_s * o = oyOptions_Find( f_options, "effect_switch", oyNAME_PATTERN );
          int touched = oyOption_GetFlags(o) & oyOPTIONATTRIBUTE_EDIT;
          int effect_switch = oyOptions_FindString( f_options, "effect_switch", "1" ) != NULL;
          if(oy_debug)
          oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                    "color_server_active = %d touched = %d flags = %d effect_switch = %d", OY_DBG_ARGS_,
                    color_server_active, touched, oyOption_GetFlags(o), effect_switch );
          oyOption_Release( &o );

          if(!touched && effect_switch)
          {
            int reject = 0;
            oyProfiles_s * effps = NULL;
            oyProfile_s * effp = NULL;

            o = oyOptions_Find( f_options, "profiles_effect", oyNAME_PATTERN );
            effps = (oyProfiles_s*) oyOption_GetStruct( o, oyOBJECT_PROFILES_S );
            oyOption_Release( &o );

            effp = oyProfiles_Get( effps, 0 );
            if(color_server_active == 0)
            {
              o = oyOptions_Find( f_options, "effect_switch", oyNAME_PATTERN );
              oyOption_SetFromString( o, "0", 0 );
              oyOption_SetFlags( o, oyOption_GetFlags(o) | oyOPTIONATTRIBUTE_AUTOMATIC );
              oyOption_Release( &o );
              reject = 1;
            }
            oyProfiles_Release( &effps );
            if(oy_debug)
              oydi_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                    "Effect profile acceptance: color_server_active = %d EFFECT_linear=%s%s", OY_DBG_ARGS_,
                    color_server_active, oyProfile_FindMeta( effp, "EFFECT_linear", "yes" ),
                    reject?" automatic rejected":" passed" );
            oyProfile_Release( &effp );
          }
        }
      }

      o = oyOptions_Find( f_options, "display_white_point", oyNAME_PATTERN );
      if(o)
      {
        const char * value = oyOption_GetValueString(o,0);
        if(value)
        {
          int c = atoi( value );
          if(c >= 0)
            display_white_point = c;
        }
        oyOption_Release( &o );
      }

      {
        int32_t old_display_white_point = 0;

        oyOptions_FindInt( node_options,"old_display_white_point", 0, &old_display_white_point );
        if(oy_debug)
          oydi_msg( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)ticket, 
                    OY_DBG_FORMAT_"display_white_point: %d -> %d", OY_DBG_ARGS_,
                    old_display_white_point, display_white_point);

        if(old_display_white_point != display_white_point)
        {
          oyOptions_SetFromInt( &node_options,
                                "//" OY_TYPE_STD "/display/old_display_white_point",
                                display_white_point, 0, OY_CREATE_NEW );

          error = oyAddMonitorEffects( p, &f_options );

          if(error || oy_debug)
              oydi_msg( oyMSG_WARN, (oyStruct_s*)ticket, 
                    OY_DBG_FORMAT_"display_white_point: %d %s", OY_DBG_ARGS_, display_white_point, oyProfile_GetText( p, oyNAME_DESCRIPTION ));

          error = oyPixelAccess_SetFromString ( ticket,
                     "//" OY_TYPE_STD "/profile/dirty", "true", OY_CREATE_NEW );
          ++dirty;
        }
      }

      oyProfile_Release( &p );
      oyConfig_Release( &c );
      oyImage_Release( &image_input );
      oyOptions_Release( &f_options );
    }

    clean2:
    oyConfigs_Release( &devices );
    oyOptions_Release( &rectangles_options );

    /* stop here and request an update */
    if(dirty > 0)
    {
      oyFilterNode_Release( &rectangles );
      result = dirty;
      goto clean;
    }


    /* make the graph flow: process the upstream "rectangles" node */
    if(!dirty)
    {
      plug = oyFilterNode_GetPlug( node, 0 );
      l_result = oyFilterNode_Run( rectangles, plug, ticket );
      if(l_result > 0 || result == 0) result = l_result;
      oyFilterPlug_Release( &plug );
    }
  }

  clean:
  oyFilterNode_Release( &rectangles );
  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );
  oyImage_Release( &image );
  oyOptions_Release( &node_options );
  oyRectangle_Release( &device_rectangle );
  oyRectangle_Release( &r );
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

int  oydiUiGet                       ( oyCMMapiFilter_s   * module OY_UNUSED,
                                       oyOptions_s       * opts OY_UNUSED,
                                       int                 flags OY_UNUSED,
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

oyConnectorImaging_s_ oyx1_Display_plug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/splitter.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyx1_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyx1_Display_plugs[2] = {&oyx1_Display_plug,0};

oyConnectorImaging_s_ oyx1_Display_socket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyx1_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyx1_Display_sockets[2] = {&oyx1_Display_socket,0};


#define OY_IMAGE_DISPLAY_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "display"
/** @brief    oydi oyCMMapi7_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi7_s_   oydi_api7_image_display = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fields; keep to zero */
  (oyCMMapi_s*) 0, /* oyCMMapi_s * next */
  
  oydiCMMInit, /* oyCMMInit_f */
  oydiCMMReset, /* oyCMMReset_f */
  oydiCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oydiFilterPlug_ImageDisplayRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyx1_Display_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyx1_Display_sockets,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */
  NULL /* properties */
};

const char * oydiApi4UiImageDisplayGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "display";
    else if(type == oyNAME_NAME)
      return "Display";
    else if(type == oyNAME_DESCRIPTION)
      return "Display Splitter Object";
  } else if(strcmp(select,"help") == 0)
  {
    /* The help text is identical, as the module contains only one filter to
     * provide help for. */
    return oydiGetText(select,type,context);
  }
  return 0;
}
const char * oydi_api4_ui_image_display_texts[] = {"name", "help", 0};

/** @brief    oydi oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image display.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/09/06
 */
oyCMMui_s_ oydi_api4_ui_image_display = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oydiFilter_ImageDisplayValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oydiWidgetEvent, /* oyWidgetEvent_f */

  "Graph/Display", /* category */
  oydi_extra_options,   /* const char * options */

  oydiUiGet,    /* oyCMMuiGet_f oyCMMuiGet */

  oydiApi4UiImageDisplayGetText,  /* oyCMMGetText_f getText */
  oydi_api4_ui_image_display_texts,  /* (const char**)texts */
  (oyCMMapiFilter_s*)&oydi_api4_image_display /* oyCMMapiFilter_s*parent */
};

/** @brief    oydi oyCMMapi4_s implementation
 *
 *  a filter for expanding the graph to several display devices
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyCMMapi4_s_   oydi_api4_image_display = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fields; keep to zero */
  (oyCMMapi_s*) & oydi_api7_image_display, /* oyCMMapi_s * next */
  
  oydiCMMInit, /* oyCMMInit_f */
  oydiCMMReset, /* oyCMMReset_f */
  oydiCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_DISPLAY_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

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
int                oydiCMMInit       ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}

/** Function oydiCMMReset
 *  @brief   API requirement
 */
int                oydiCMMReset      ( oyStruct_s        * filter OY_UNUSED )
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
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/04/12
 */
const char * oydiGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
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
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "newBSD";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2009 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The \"display\" filter supports applications to show image content on single and multi monitor displays. It cares about the server communication in declaring the region as prematched. So a X11 server side color correction does not disturb the displayed colors and omits the provided rectangle. The \"display\" filter matches the provided image content to each monitor it can find. Of course this has limitations to distorted windows, like wobbly effects or matrix deformed windows.");
    else
      return _("The filter needs some informations attached to the output image tags of the \"output\" image filter. The following list describes the X11/Xorg requirements.\n A \"window_id\" option shall consist of a oyBlob_s object containing the X11 \"Window\" type in its pointer element.\n A \"display_id\" option shall consist of a oyBlob_s object containing the X11 \"Display\" of the application. This is typically exposed as system specific pointer by each individual toolkit.\n A \"display_rectangle\" option of type oyRectangle_s shall represent the application image region in pixel of the absolute display coordinates. \n In the \"datatype\" option, a oyDATATYPE_e encoded as integer is expected, to deliver that data type in a not yet allocated output image. The output data type is by default not changed. A newly not yet allocated output image will be stored as processing data in the socket.\n \"preserve_alpha\" is a integer option to keep a given alpha in a not yet allocated output image. \
");
  }
  return 0;
}

const char *oydi_texts[5] = {"name","copyright","manufacturer","help",0};
oyIcon_s oydi_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @brief    oydi module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/00 (Oyranos: 0.1.10)
 *  @date    2009/01/00
 */
oyCMM_s oydi_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.2",
  oydiGetText,                         /**< oyCMMinfoGetText_f getText */
  (char**)oydi_texts,                  /**<texts; list of arguments to getText*/

  OYRANOS_VERSION,

  (oyCMMapi_s*) & oydi_api4_image_display,

  &oydi_icon,

  NULL,                                /**< init() */
  NULL                                 /**< reset() */
};

