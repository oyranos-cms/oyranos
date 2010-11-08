/** @file oyranos_monitor_internal_x11.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2010  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_X11_H
#define OYRANOS_MONITOR_INTERNAL_X11_H

#include "config.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"
# if HAVE_XRANDR
#  include <X11/extensions/Xrandr.h>
# endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
# include <X11/Xcm/XcmEdidParse.h>
#if HAVE_XIN
# include <X11/extensions/Xinerama.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum {
  oyX11INFO_SOURCE_SCREEN,
  oyX11INFO_SOURCE_XINERAMA,
  oyX11INFO_SOURCE_XRANDR
} oyX11INFO_SOURCE_e;

/** \internal  platformdependent */
typedef struct {
  oyOBJECT_e       type_;              /**< object type oyOBJECT_MONITOR_S */
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
# endif
  char               * system_port;    /**< the operating systems port name */
  oyBlob_s           * edid;           /**< edid for the device */
  oyX11INFO_SOURCE_e   info_source; /**< */
} oyMonitor_s;

Display*     oyMonitor_device_       ( oyMonitor_s       * disp );
oyX11INFO_SOURCE_e 
             oyMonitor_infoSource_   ( oyMonitor_s       * disp );
# ifdef HAVE_XRANDR
XRRScreenResources *
             oyMonitor_xrrResource_  ( oyMonitor_s       * disp );
RROutput     oyMonitor_xrrOutput_    ( oyMonitor_s       * disp );
XRROutputInfo *
             oyMonitor_xrrOutputInfo_( oyMonitor_s       * disp );
int          oyMonitor_activeOutputs_( oyMonitor_s       * disp );
# endif


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_INTERNAL_X11_H */
