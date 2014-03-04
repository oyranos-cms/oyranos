/** @internal
 *  @file oyranos_monitor_internal_cs.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2013  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_CS_H
#define OYRANOS_MONITOR_INTERNAL_CS_H

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"

#include "oyRectangle_s.h"

#include <Carbon/Carbon.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @internal
 *  platformdependent */
typedef struct {
  oyOBJECT_e       type_;              /**< object type oyOBJECT_MONITOR_S */
  char           * name;               /**< display name, like screen */
  char           * host;               /**< host name only - host */
  char           * identifier;         /**<  - _x_y_wxh */
  int              geo[6];             /**< display screen x y width height */
  CGDirectDisplayID id;                /**< osX monitor */
  int              screen;             /**< pos in osX CGGetActiveDisplayList */
} qarzMonitor_s;

qarzMonitor_s* qarzMonitor_newFrom_      ( const char        * display_name,
                                       int                 expensive );
int          qarzMonitor_release_      ( qarzMonitor_s      ** disp );

const char*  qarzMonitor_name_         ( qarzMonitor_s       * disp );
const char*  qarzMonitor_hostName_     ( qarzMonitor_s       * disp );
const char*  qarzMonitor_identifier_   ( qarzMonitor_s       * disp );
const char * qarzMonitor_systemPort_   ( qarzMonitor_s       * disp );
oyBlob_s   * qarzMonitor_edid_         ( qarzMonitor_s       * disp );
char*        qarzMonitor_screenNumber_ ( qarzMonitor_s       * disp );
int          qarzMonitor_deviceScreenNumber_( qarzMonitor_s  * disp );
int          qarzMonitor_number_       ( qarzMonitor_s       * disp );
int          qarzMonitor_screen_       ( qarzMonitor_s       * disp );
int          qarzMonitor_x_            ( qarzMonitor_s       * disp );
int          qarzMonitor_y_            ( qarzMonitor_s       * disp );
int          qarzMonitor_width_        ( qarzMonitor_s       * disp );
int          qarzMonitor_height_       ( qarzMonitor_s       * disp );



CGDirectDisplayID  qarzMonitor_device_ ( qarzMonitor_s       * disp );

int          qarzDeviceFillEdid      ( oyConfig_s       ** device,
                                       oyPointer           edi,
                                       size_t              edi_size,
                                       const char        * device_name,
                                       const char        * host,
                                       const char        * display_geometry,
                                       const char        * system_port,
                                       oyOptions_s       * options );
int          qarzMonitorProfileSetup ( const char        * display_name,
                                       const char        * profil_name );
int          qarzMonitorProfileUnset ( const char        * display_name );
oyRectangle_s * qarzRectangle_FromDevice (
                                       const char        * device_name );
int          qarzGetMonitorProfile   ( const char        * device_name,
                                       uint32_t            flags,
                                       char             ** block,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );
int      qarzGetAllScreenNames       ( const char        * display_name,
                                       char            *** display_names,
                                       oyAlloc_f           allocateFunc );

int   qarzGetMonitorInfo_lib      (const char* display,
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

#endif /* OYRANOS_MONITOR_INTERNAL_CS_H */
