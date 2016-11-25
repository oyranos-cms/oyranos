/** @internal
 *  @file oyranos_monitor_internal.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2010  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_H
#define OYRANOS_MONITOR_INTERNAL_H

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"
#include "oyCMMapi_s.h"

#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}
#define catCMMfunc(nick,func) nick ## func
#define catCMMstruct(nick, func) nick ## MonitorHooks -> func

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int          oyUnrollEdid1_          ( void              * edid,
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
int            oyDeviceFillInfos     ( const char        * registration,
                                       oyConfig_s       ** device,
                                       const char        * device_name,
                                       const char        * host,
                                       const char        * display_geometry,
                                       const char        * system_port,
                                       const char        * EDID_manufacturer,
                                       const char        * EDID_mnft,
                                       const char        * EDID_model,
                                       const char        * EDID_serial,
                                       const char        * EDID_vendor,
                                       int                 week,
                                       int                 year,
                                       int                 EDID_mnft_id,
                                       int                 EDID_model_id,
                                       double            * colors,
                                       oyOptions_s       * options );
int            oyDeviceFillEdid      ( const char        * registration,
                                       oyConfig_s       ** device,
                                       oyPointer           edi,
                                       size_t              edi_size,
                                       const char        * device_name,
                                       const char        * host,
                                       const char        * display_geometry,
                                       const char        * system_port,
                                       oyOptions_s       * options );



typedef int  (*oySetupMonitorProfile_f) (
                                       const char        * display_name,
                                       const char        * profil_name );
typedef int  (*oyUnsetMonitorProfile_f) (
                                       const char        * display_name );
typedef oyRectangle_s * (*oyGetRectangleFromDevice_f) (
                                       const char        * device_name );
typedef char * (*oyGetMonitorProfile_f) (
                                       const char        * device_name,
                                       uint32_t            flags,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );
typedef int  (*oyGetAllScreenNames_f)( const char        * display_name,
                                       char            *** display_names,
                                       oyAlloc_f           allocateFunc );
typedef int  (*oyGetMonitorInfo_f)   ( const char        * display,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
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
                                       oyAlloc_f           allocate_func,
                                       oyStruct_s        * user_data );

typedef struct {
  uint32_t         type;               /**< set to 120 for ABI compatibility with the actual used header version */
  char             nick[8];            /**< four byte nick name of module */
  uint32_t         version;            /**< set to module version; Major * 10000 + Minor * 100 + Micro */
  const char *     help_system_specific;
  oyRankMap *      rank_map;
  oySetupMonitorProfile_f setupProfile;
  oyUnsetMonitorProfile_f unsetProfile;
  oyGetRectangleFromDevice_f getRectangle;
  oyGetMonitorProfile_f getProfile;
  oyGetAllScreenNames_f getAllScreenNames;
  oyGetMonitorInfo_f getInfo;
} oyMonitorDeviceHooks_s;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR__INTERNAL_H */
