/** @file oyranos_monitor_internal.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2009  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_H
#define OYRANOS_MONITOR_INTERNAL_H

#include "config.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"
# if HAVE_XRANDR
#  include <X11/extensions/Xrandr.h>
# endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef HAVE_X

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

#elif defined(__APPLE__)
/** \internal  platformdependent */
typedef struct {
  oyOBJECT_e       type_;              /**< object type oyOBJECT_MONITOR_S */
  char           * name;               /**< display name, like screen */
  char           * host;               /**< host name only - host */
  char           * identifier;         /**<  - _x_y_wxh */
  int              geo[6];             /**< display screen x y width height */
  CGDirectDisplayID id;                /**< osX monitor */
  int              screen;             /**< pos in osX CGGetActiveDisplayList */
} oyMonitor_s;

CGDirectDisplayID  oyMonitor_device_ ( oyMonitor_s       * disp );
#endif

oyMonitor_s* oyMonitor_newFrom_      ( const char        * display_name,
                                       int                 expensive );
int          oyMonitor_release_      ( oyMonitor_s      ** disp );

const char*  oyMonitor_name_         ( oyMonitor_s       * disp );
const char*  oyMonitor_hostName_     ( oyMonitor_s       * disp );
const char*  oyMonitor_identifier_   ( oyMonitor_s       * disp );
const char * oyMonitor_systemPort_   ( oyMonitor_s       * disp );
oyBlob_s   * oyMonitor_edid_         ( oyMonitor_s       * disp );
char*        oyMonitor_screenNumber_ ( oyMonitor_s       * disp );
int          oyMonitor_deviceScreenNumber_( oyMonitor_s  * disp );
int          oyMonitor_number_       ( oyMonitor_s       * disp );
int          oyMonitor_screen_       ( oyMonitor_s       * disp );
int          oyMonitor_x_            ( oyMonitor_s       * disp );
int          oyMonitor_y_            ( oyMonitor_s       * disp );
int          oyMonitor_width_        ( oyMonitor_s       * disp );
int          oyMonitor_height_       ( oyMonitor_s       * disp );


void         oyUnrollEdid1_          ( void              * edid,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** vendor,
                                       uint32_t          * week,
                                       uint32_t          * year,
                                       uint32_t          * mnft_id,
                                       uint32_t          * model_id,
                                       double            * c,
                                       oyAlloc_f           allocate_func);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR__INTERNAL_H */
