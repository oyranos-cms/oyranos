/** @file oyranos_monitor_internal_x11.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2010  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_BASE_X11_X11_H
#define OYRANOS_MONITOR_BASE_X11_X11_H

#include "oyranos_monitor_base.h"


/* shared platform API's */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
# include <X11/Xcm/XcmEdidParse.h>
#if defined(HAVE_XCM)
# include <X11/Xcm/Xcm.h>
# if defined(XCM_HAVE_X11)
#  include <X11/Xcm/XcmEvents.h>
# endif
#endif
#if defined(HAVE_XRANDR)
# include <X11/extensions/Xrandr.h>
#endif
#if defined(HAVE_XINERAMA)
# include <X11/extensions/Xinerama.h>
#endif
# ifdef HAVE_XXF86VM
#  include <X11/extensions/xf86vmode.h>
# endif
#include <X11/extensions/Xfixes.h>

typedef enum {
  oyX11INFO_SOURCE_SCREEN,
  oyX11INFO_SOURCE_XINERAMA,
  oyX11INFO_SOURCE_XRANDR
} oyX11INFO_SOURCE_e;

/** \internal  platformdependent */
typedef struct {
  int          type_;                  /**< object type oyOBJECT_MONITOR_S */
  char         *name;        /**< traditional display name - host:0 / :0 */
  char         *host;        /**< host name only - host */
  char         *identifier;  /**<  - x_y_wxh */
  int           geo[6];      /**< display screen x y width height */
  Display      *display;     /**< logical display */
  int           screen;      /**< external screen number to call for X */
# ifdef HAVE_XRANDR
  XRRScreenResources * res;            /**< XRandR root window struct */
  RROutput             output;         /**< XRandR output */
  XRROutputInfo      * output_info;    /**< XRandR output info */
  int                  active_outputs; /**< outputs with crtc and gamma size */
  int                  rr_version;     /**< XRandR version */
  int                  rr_screen;      /**< nth XRandR output */
  int                  mm_width;       /**< screen's milli meter dimension */
  int                  mm_height;      /**< screen's milli meter dimension */
# endif
  char               * system_port;    /**< the operating systems port name */
  oyX11INFO_SOURCE_e   info_source; /**< */
} oyX1Monitor_s;

oyX1Monitor_s* oyX1Monitor_newFrom_      ( const char        * display_name,
                                       int                 expensive );
int          oyX1Monitor_release_      ( oyX1Monitor_s      ** disp );

const char*  oyX1Monitor_name_         ( oyX1Monitor_s       * disp );
const char*  oyX1Monitor_hostName_     ( oyX1Monitor_s       * disp );
const char*  oyX1Monitor_identifier_   ( oyX1Monitor_s       * disp );
const char * oyX1Monitor_systemPort_   ( oyX1Monitor_s       * disp );
char*        oyX1Monitor_screenNumber_ ( oyX1Monitor_s       * disp );
int          oyX1Monitor_deviceScreenNumber_( oyX1Monitor_s  * disp );
int          oyX1Monitor_number_       ( oyX1Monitor_s       * disp );
int          oyX1Monitor_screen_       ( oyX1Monitor_s       * disp );
int          oyX1Monitor_x_            ( oyX1Monitor_s       * disp );
int          oyX1Monitor_y_            ( oyX1Monitor_s       * disp );
int          oyX1Monitor_width_        ( oyX1Monitor_s       * disp );
int          oyX1Monitor_height_       ( oyX1Monitor_s       * disp );


Display*     oyX1Monitor_device_       ( oyX1Monitor_s       * disp );
oyX11INFO_SOURCE_e 
             oyX1Monitor_infoSource_   ( oyX1Monitor_s       * disp );
# ifdef HAVE_XRANDR
XRRScreenResources *
             oyX1Monitor_xrrResource_  ( oyX1Monitor_s       * disp );
RROutput     oyX1Monitor_xrrOutput_    ( oyX1Monitor_s       * disp );
XRROutputInfo *
             oyX1Monitor_xrrOutputInfo_( oyX1Monitor_s       * disp );
int          oyX1Monitor_activeOutputs_( oyX1Monitor_s       * disp );
int          oyX1Monitor_rrVersion_    ( oyX1Monitor_s       * disp );
int          oyX1Monitor_rrScreen_     ( oyX1Monitor_s       * disp );
# endif

int          oyX1MonitorProfileSetup ( const char        * display_name,
                                       const char        * profil_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size );
int          oyX1MonitorProfileUnset ( const char        * display_name );
int          oyX1Rectangle_FromDevice( const char        * device_name,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height );
char *       oyX1GetMonitorProfile   ( const char        * device_name,
                                       int                 flags,
                                       size_t            * size );
int      oyX1GetAllScreenNames       ( const char        * display_name,
                                       char            *** display_names );
int      oyX1MoveColorServerProfiles ( const char        * display_name,
                                       int                 screen,
                                       int                 setup );


int      oyX1GetMonitorInfo_         ( const char        * display,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** vendor,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       char             ** host,
                                       int               * week,
                                       int               * year,
                                       int               * mnft_id,
                                       int               * model_id,
                                       double            * colors,
                                       char             ** edid,
                                       size_t            * edid_size,
                                       int                 refresh_edid );


int   oyX1Monitor_getScreenFromDisplayName_( oyX1Monitor_s   * disp );
char**oyX1GetAllScreenNames_        (const char *display_name, int *n_scr );
int   oyX1Monitor_getScreenGeometry_   ( oyX1Monitor_s       * disp );

#endif /* OYRANOS_MONITOR_BASE_X11_X11_H */
