/** @file oyranos_monitor_x11_extend.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/01/31
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>  /* intptr_t */
#include <locale.h>

#include "oyranos_config_internal.h"

# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# include <X11/Xcm/XcmEdidParse.h>
# include <X11/Xcm/XcmEvents.h>
# include <X11/Xcm/Xcm.h>
# if defined(HAVE_XINERAMA)
#  include <X11/extensions/Xinerama.h>
# endif
# ifdef HAVE_XXF86VM
#  include <X11/extensions/xf86vmode.h>
# endif
#include <X11/extensions/Xfixes.h>

#include "oyranos.h"
#include "oyranos_cmm.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_internal_x11.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyCMMapi10_s_.h"

/* ---  Helpers  --- */

/* --- internal API definition --- */

extern oyMessage_f oyX1_msg;



int  oyMoveColorServerProfiles       ( const char        * display_name,
                                       int                 screen,
                                       int                 setup )
{
  return 1;
}



#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
int XcolorRegionFind(XcolorRegion * old_regions, unsigned long old_regions_n, Display * dpy, Window win, XRectangle * rectangle)
{   
  XRectangle * rect = 0;
  int nRect = 0;
  int pos = -1;
  unsigned long i;
  int j;
    
  /* get old regions */ 
  old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
  /* search region */
  for(i = 0; i < old_regions_n; ++i) 
  {     
                 
    if(!old_regions[i].region || pos >= 0)
      break;                    

    rect = XFixesFetchRegion( dpy, ntohl(old_regions[i].region),
                              &nRect );

    for(j = 0; j < nRect; ++j)
    {
      if(oy_debug) 
        printf( "reg[%lu]: %dx%d+%d+%d %dx%d+%d+%d\n",
                   i,
                   rectangle->width, rectangle->height,
                   rectangle->x, rectangle->y,
                   rect[j].width, rect[j].height, rect[j].x, rect[j].y
                  );
      if(rectangle->x == rect[j].x &&
         rectangle->y == rect[j].y &&
         rectangle->width == rect[j].width &&
         rectangle->height == rect[j].height )
      {
        pos = i;
        break;
      }
    }
  }

  return pos;
}
#endif

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/03/10 (Oyranos: 0.9.6)
 *  @date    2016/03/10
 */
int          oyX1UpdateOptions_Handle( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"send_native_update_event", 0))
    {
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"send_native_update_event", 0))
  {
    // ping X11 observers about option change
    // ... by setting a known property again to its old value
    Display * display = XOpenDisplay(NULL);
    Atom atom = XInternAtom(display, XCM_COLOUR_DESKTOP_ADVANCED, False); // "_ICC_COLOR_DISPLAY_ADVANCED"
    Window root = RootWindow( display, 0 );
  
    XFlush( display );
  
    Atom actual;
    int format;
    int advanced = -1;
    unsigned long left;
    unsigned long size;
    unsigned char *data;
    int result = XGetWindowProperty( display, root, atom, 0, ~0, 0, XA_STRING, &actual,
                                     &format, &size, &left, &data );
    if(data && size && atoi((const char*)data) > 0)
      advanced = atoi((const char*)data);
    _msg( oyMSG_DBG, (oyStruct_s*)options,
          OY_DBG_FORMAT_ "desktop uses advanced settings: %d\n", OY_DBG_ARGS_,
          advanced );
    XChangeProperty( display, root,
                       atom, XA_STRING, 8, PropModeReplace,
                       data, size );
    if(result == Success && data)
      XFree( data ); data = 0;
    if(display)
      XCloseDisplay(display);
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/03/10 (Oyranos: 0.9.6)
 *  @date    2016/03/10
 */
const char * oyX1InfoGetTextMyHandlerU(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "send_native_update_event")==0)
  {
         if(type == oyNAME_NICK)
      return "send_native_update_event";
    else if(type == oyNAME_NAME)
      return _("Set a X Color Management update toggle.");
    else
      return _("Ping the XCM_COLOUR_DESKTOP_ADVANCED X11 atom.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"send_native_update_event\" handler lets you ping "
               "X Color Management advanced X11 atom. "
               "The implementation uses Xlib.");
  }
  return 0;
}
const char *oyX1_texts_send_native_update_event[4] = {"can_handle","send_native_update_event","help",0};

/** @instance oyX1_api10_send_native_update_event_handler
 *  @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Management desktop advanced toogle
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/03/10 (Oyranos: 0.9.6)
 *  @date    2016/03/10
 */
oyCMMapi10_s_    oyX1_api10_send_native_update_event_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) NULL,

  CMMInit,
  CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "send_native_update_event._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandlerU,             /**< getText */
  (char**)oyX1_texts_send_native_update_event, /**<texts; list of arguments to getText*/
 
  oyX1UpdateOptions_Handle               /**< oyMOptions_Handle_f oyMOptions_Handle */
};


/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
int          oyX1MOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  oyOption_s * o = 0;
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"set_xcm_region", 0))
    {
      o = oyOptions_Find( options, "window_rectangle", oyNAME_PATTERN );
      if(!o)
      {
        _msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option window_rectangle found");
        error = 1;
      }
      oyOption_Release( &o );
      o = oyOptions_Find( options, "window_id", oyNAME_PATTERN );
      if(!o)
      {
        _msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option window_id found");
        error = 1;
      }
      oyOption_Release( &o );
      o = oyOptions_Find( options, "display_id", oyNAME_PATTERN );
      if(!o)
      {
        _msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option display_id found");
        error = 1;
      }
      oyOption_Release( &o );

      return error;
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"set_xcm_region", 0))
  {
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
    oyProfile_s * p = NULL;
    oyRectangle_s * win_rect = NULL;
    oyRectangle_s * old_rect = NULL;

    Display * dpy = NULL;
    Window win = 0;
    char * blob = 0;
    size_t size = 0;
    XcolorProfile * profile = 0;
    XserverRegion reg = 0;
    XcolorRegion region;
    int error;
    XRectangle rec[2] = { { 0,0,0,0 }, { 0,0,0,0 } };
    double rect[4];

    oyBlob_s * win_id, * display_id;

    win_id = (oyBlob_s*) oyOptions_GetType( options, -1, "window_id",
                                          oyOBJECT_BLOB_S );
    display_id = (oyBlob_s*) oyOptions_GetType( options, -1, "display_id",
                                          oyOBJECT_BLOB_S );
    win = (Window) oyBlob_GetPointer(win_id);
    dpy = (Display *) oyBlob_GetPointer(display_id);

    oyBlob_Release( &win_id );
    oyBlob_Release( &display_id );

    /* now handle the options */
    win_rect = (oyRectangle_s*) oyOptions_GetType( options, -1, "window_rectangle",
                                          oyOBJECT_RECTANGLE_S );
    old_rect = (oyRectangle_s*) oyOptions_GetType( options, -1,
                                 "old_window_rectangle", oyOBJECT_RECTANGLE_S );
    o = oyOptions_Find( options, "icc_profile", oyNAME_PATTERN );
    p = (oyProfile_s*) oyOptions_GetType( options, -1, "icc_profile",
                                          oyOBJECT_PROFILE_S );
    if(!win || !dpy)
      _msg( oyMSG_WARN, (oyStruct_s*)options,
                "options display_id or window_id not found");
    if(!win_rect)
      _msg( oyMSG_WARN, (oyStruct_s*)options,
                "option window_rectangle not found");


    if(old_rect)
    {
      XcolorRegion *old_regions = 0;
      unsigned long old_regions_n = 0;
      int pos = -1;

      oyRectangle_GetGeo( old_rect, &rect[0], &rect[1], &rect[2], &rect[3] );

      rec[0].x = rect[0];
      rec[0].y = rect[1];
      rec[0].width = rect[2];
      rec[0].height = rect[3];

      /* get old regions */
      old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
      /* remove specified region */
      pos = XcolorRegionFind( old_regions, old_regions_n, dpy, win, rec );
      XFree( old_regions );
      if(pos >= 0)
      {
        int undeleted_n = old_regions_n;
        XcolorRegionDelete( dpy, win, pos, 1 );
        old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
        if(undeleted_n - old_regions_n != 1)
          _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "removed %d; have still %d",
             OY_DBG_ARGS_, pos, (int)old_regions_n );
      } else
        _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                  "region not found in %lu\n",OY_DBG_ARGS_, old_regions_n );

      XFlush( dpy );

    }

    oyRectangle_GetGeo( win_rect, &rect[0], &rect[1], &rect[2], &rect[3] );

    rec[0].x = rect[0];
    rec[0].y = rect[1];
    rec[0].width = rect[2];
    rec[0].height = rect[3];

    if(p)
    {
        blob = (char*)oyProfile_GetMem( p, &size, 0,0 );

        if(blob && size)
        {
          int result;
        /* Create a XcolorProfile object that will be uploaded to the display.*/
          profile = (XcolorProfile*)malloc(sizeof(XcolorProfile) + size);

          oyProfile_GetMD5(p, 0, (uint32_t*)profile->md5);

          profile->length = htonl(size);
          memcpy(profile + 1, blob, size);

          result = XcolorProfileUpload( dpy, profile );
          if(result)
            _msg( oyMSG_WARN, (oyStruct_s*)options,
                "XcolorProfileUpload: %d\n", result);

          XFlush( dpy );
        }
    }

    if( rect[0] || rect[1] || rect[2] || rect[3] )
    {
      reg = XFixesCreateRegion( dpy, rec, 1);

      region.region = htonl(reg);
      if(blob && size)
        memcpy(region.md5, profile->md5, 16);
      else
        memset( region.md5, 0, 16 );

      /* upload the new or changed region to the X server */
      error = XcolorRegionInsert( dpy, win, 0, &region, 1 );
      if(error)
          _msg( oyMSG_WARN, (oyStruct_s*)options,
                    "XcolorRegionInsert failed %d\n", error );
      XFlush( dpy );
    }
#endif
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
const char * oyX1InfoGetTextMyHandler( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "set_xcm_region")==0)
  {
         if(type == oyNAME_NICK)
      return "set_xcm_region";
    else if(type == oyNAME_NAME)
      return _("Set a X Color Management region.");
    else
      return _("The set_xcm_region takes minimal three options. The key name "
               "\"window_rectangle\" specifies in a oyRectangle_s object the "
               "requested window region in coordinates relative to the window. "
               "If its parameters are all set to zero, then the rectangle is "
               "ignored. The \"old_window_rectangle\" is similiar to the "
               "\"window_rectangle\" "
               "option but optionally specifies to remove a old rectangle. "
               "The \"window_id\" specifies a X11 window id as oyBlob_s. "
               "The \"display_id\" specifies a X11 Display struct as oyBlob_s. "
               "The " "\"icc_profile\" option of type oyProfile_s optionally "
               "provides a ICC profile to upload to the server.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"set_xcm_region\" handler lets you set "
               "X Color Management compatible client side color regions. "
               "The implementation uses libXcm and Oyranos.");
  }
  return 0;
}
const char *oyX1_texts_set_xcm_region[4] = {"can_handle","set_xcm_region","help",0};

/** @instance oyX1_api10_set_xcm_region_handler
 *  @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Management server side regions setup
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
oyCMMapi10_s_    oyX1_api10_set_xcm_region_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) &oyX1_api10_send_native_update_event_handler,

  CMMInit,
  CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "set_xcm_region._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandler,             /**< getText */
  (char**)oyX1_texts_set_xcm_region,       /**<texts; list of arguments to getText*/
 
  oyX1MOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};

