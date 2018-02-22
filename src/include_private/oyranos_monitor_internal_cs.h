/** @internal
 *  @file oyranos_monitor_internal_cs.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2018  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_CS_H
#define OYRANOS_MONITOR_INTERNAL_CS_H

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_monitor_hooks.h"

#include "oyRectangle_s.h"

#include <Carbon/Carbon.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CMM_NICK "qarz"

extern oyMonitorHooks2_s * qarzMonitorHooks2;
#define next_api (oyCMMapi_s*) NULL

#define CMMInit                 catCMMfunc( qarz, CMMInit )
#define _initialised            catCMMfunc( qarz, _initialised )
#define CMMMessageFuncSet       catCMMfunc( qarz, CMMMessageFuncSet )
#define _msg                    catCMMfunc( qarz, _msg )
#define DeviceFromName_         catCMMfunc( qarz, DeviceFromName_ )
#define Configs_Modify          catCMMfunc( qarz, Configs_Modify )
#define GetText                 catCMMfunc( qarz, GetText )
#define Api8UiGetText           catCMMfunc( qarz, Api8UiGetText )
#define hooks2                  catCMMfunc( qarz, MonitorHooks2 )
#define _help_list              catCMMfunc( qarz, _help_list )
#define _help_properties        catCMMfunc( qarz, _help_properties )
#define _help_system_specific   catCMMstruct( qarz, help_system_specific )
#define _help_setup             catCMMfunc( qarz, _help_setup )
#define _help_unset             catCMMfunc( qarz, _help_unset )
#define _help_add_edid_to_icc   catCMMfunc( qarz, _help_add_edid_to_icc )
#define _help                   catCMMfunc( qarz, _help )
#define ConfigsUsage            catCMMfunc( qarz, ConfigsUsage )
#define Configs_FromPattern     catCMMfunc( qarz, Configs_FromPattern )
#define Config_Rank             catCMMfunc( qarz, Config_Rank )
#define _api8_ui_texts          catCMMfunc( qarz, _api8_ui_texts )
#define _api8_ui                catCMMfunc( qarz, _api8_ui )
#define _api8_icon              catCMMfunc( qarz, _api8_icon )
#define _api8                   catCMMfunc( qarz, _api8 )
#define _cmm_module             catCMMfunc( qarz, _cmm_module )
#define _texts                  catCMMfunc( qarz, _texts )
#define GetMonitorInfo_lib      catCMMstruct( qarz, getInfo )
#define GetAllScreenNames       catCMMstruct( qarz, getAllMonitorNames )
#define SetupMonitorCalibration catCMMstruct( qarz, setupCalibration )
#define SetupMonitorProfile     catCMMstruct( qarz, setupProfile )
#define UnsetMonitorProfile     catCMMstruct( qarz, unsetProfile )
#define GetRectangleFromMonitor catCMMstruct( qarz, getRectangle )
#define GetMonitorProfile       catCMMstruct( qarz, getProfile )

int                CMMInit           ( );
int                CMMMessageFuncSet ( oyMessage_f         message_func );


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

int          qarzMonitorProfileSetup ( const char        * monitor_name,
                                       const char        * profil_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size );
int          qarzMonitorProfileUnset ( const char        * monitor_name );
int          qarzRectangle_FromDevice( const char        * monitor_name,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height );
char *       qarzGetMonitorProfile   ( const char        * monitor_name,
                                       int                 flags,
                                       size_t            * size );
int      qarzGetAllScreenNames       ( const char        * display_name,
                                       char            *** monitor_names );

int   qarzGetMonitorInfo_lib         ( const char        * monitor_name,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** vendor,
                                       char             ** device_geometry,
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


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_INTERNAL_CS_H */
