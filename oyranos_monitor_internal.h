/** @file oyranos_monitor_internal.h
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

#ifndef OYRANOS_MONITOR_INTERNAL_H
#define OYRANOS_MONITOR_INTERNAL_H

#include "config.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


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
