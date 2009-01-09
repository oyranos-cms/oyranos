/** @file oyranos_monitor_internal.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2008  Kai-Uwe Behrmann
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
namespace oyranos
{
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
  XRROutputInfo      * output;         /**< XRandR output */
  int                  active_outputs; /**< outputs with crtc and gamma size */
# endif
  char               * system_port;    /**< the operating systems port name */
  oyBlob_s           * edid;           /**< edid for the device */
  oyX11INFO_SOURCE_e   info_source; /**< */
} oyMonitor_s;

oyMonitor_s* oyMonitor_newFrom_      ( const char        * display_name );
int          oyMonitor_release_      ( oyMonitor_s      ** disp );

const char*  oyMonitor_name_         ( oyMonitor_s       * disp );
const char*  oyMonitor_hostName_     ( oyMonitor_s       * disp );
const char*  oyMonitor_identifier_   ( oyMonitor_s       * disp );
const char * oyMonitor_systemPort_   ( oyMonitor_s       * disp );
oyBlob_s   * oyMonitor_edid_         ( oyMonitor_s       * disp );
char*        oyMonitor_screenNumber_ ( oyMonitor_s       * disp );
Display*     oyMonitor_device_       ( oyMonitor_s       * disp );
int          oyMonitor_deviceScreenNumber_( oyMonitor_s  * disp );
int          oyMonitor_number_       ( oyMonitor_s       * disp );
int          oyMonitor_screen_       ( oyMonitor_s       * disp );
int          oyMonitor_x_            ( oyMonitor_s       * disp );
int          oyMonitor_y_            ( oyMonitor_s       * disp );
int          oyMonitor_width_        ( oyMonitor_s       * disp );
int          oyMonitor_height_       ( oyMonitor_s       * disp );

#endif

int oyFree_       (void *oy_structure);


/** @brief \internal DDC struct */
struct oyDDC_EDID1_s_ {
 unsigned char sig[8];
 unsigned char MNF_ID[2];              /* [8] manufaturer ID */
 unsigned char MON_ID[2];              /* [10] model ID */
 unsigned char SER_ID[2];              /* [12] serial ID */
 unsigned char dummy_li[2];
 unsigned char WEEK;                   /* [16] Week */
 unsigned char YEAR;                   /* [17] + 1990 => Year */
 unsigned char major_version;          /* [18] */
 unsigned char minor_version;          /* [19] */
 unsigned char video_input_type;       /* [20] */
 unsigned char width;                  /* [21] */
 unsigned char height;                 /* [22] */
 unsigned char gamma_factor;           /* [23] */
 unsigned char DPMS;                   /* [24] */
 unsigned char rg;                     /* [25] colour information */
 unsigned char wb;                     /* [26] */
 unsigned char rY;                     /* [27] */
 unsigned char rX;                     /* [28] */
 unsigned char gY;                     /* [29] */
 unsigned char gX;                     /* [30] */
 unsigned char bY;                     /* [31] */
 unsigned char bX;                     /* [32] */
 unsigned char wY;                     /* [33] */
 unsigned char wX;                     /* [34] */
 unsigned char etiming1;               /* [35] */
 unsigned char etiming2;               /* [36] */
 unsigned char mtiming;                /* [37] */
 unsigned char stdtiming[16];          /* [38] */
 unsigned char text1[18];              /* [54] Product string */
 unsigned char text2[18];              /* [72] text 2 */
 unsigned char text3[18];              /* [90] text 3 */
 unsigned char text4[18];             /* [108] text 4 */
 unsigned char dummy_lii;
 unsigned char checksum;              /* [127] */
};

void oyUnrollEdid1_               (struct oyDDC_EDID1_s_ *edi,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAlloc_f     allocate_func);


#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR__INTERNAL_H */
