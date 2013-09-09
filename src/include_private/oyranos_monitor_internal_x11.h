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
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_X11_H
#define OYRANOS_MONITOR_INTERNAL_X11_H

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"

#include "oyRectangle_s.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
# include <X11/Xcm/XcmEdidParse.h>
#if defined(HAVE_XRANDR)
# include <X11/extensions/Xrandr.h>
#endif
#if defined(HAVE_XINERAMA)
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
  int                  rr_version;     /**< XRandR version */
  int                  rr_screen;      /**< nth XRandR output */
# endif
  char               * system_port;    /**< the operating systems port name */
  oyBlob_s           * edid;           /**< edid for the device */
  oyX11INFO_SOURCE_e   info_source; /**< */
} oyX1Monitor_s;

oyX1Monitor_s* oyX1Monitor_newFrom_      ( const char        * display_name,
                                       int                 expensive );
int          oyX1Monitor_release_      ( oyX1Monitor_s      ** disp );

const char*  oyX1Monitor_name_         ( oyX1Monitor_s       * disp );
const char*  oyX1Monitor_hostName_     ( oyX1Monitor_s       * disp );
const char*  oyX1Monitor_identifier_   ( oyX1Monitor_s       * disp );
const char * oyX1Monitor_systemPort_   ( oyX1Monitor_s       * disp );
oyBlob_s   * oyX1Monitor_edid_         ( oyX1Monitor_s       * disp );
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
                                       const char        * profil_name );
int          oyX1MonitorProfileUnset ( const char        * display_name );
oyRectangle_s * oyX1Rectangle_FromDevice (
                                       const char        * device_name );
char *       oyX1GetMonitorProfile   ( const char        * device_name,
                                       uint32_t            flags,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );
int      oyX1GetAllScreenNames       ( const char        * display_name,
                                       char            *** display_names,
                                       oyAlloc_f           allocateFunc );


int   oyX1GetMonitorInfo_lib      (const char* display,
                                   char**      manufacturer,
                                       char             ** mnft,
                                   char**      model,
                                   char**      serial,
                                       char             ** vendor,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       char             ** host,
                                       uint32_t          * week,
                                       uint32_t          * year,
                                       uint32_t          * mnft_id,
                                       uint32_t          * model_id,
                                       double            * colors,
                                       oyBlob_s         ** edid,
                                   oyAlloc_f     allocate_func,
                                       oyStruct_s        * user_data );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_INTERNAL_X11_H */
