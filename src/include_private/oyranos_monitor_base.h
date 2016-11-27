/**
 *  @file oyranos_monitor_base.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2016  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_BASE_H
#define OYRANOS_MONITOR_BASE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



typedef int  (*oySetupMonitorProfile_f) (
                                       const char        * display_name,
                                       const char        * profil_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size );
typedef int  (*oyUnsetMonitorProfile_f) (
                                       const char        * display_name );
typedef int  (*oyGetRectangleFromDevice_f) (
                                       const char        * device_name,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height );
typedef char * (*oyGetMonitorProfile_f) (
                                       const char        * device_name,
                                       int                 flags,
                                       size_t            * size );
typedef int  (*oyGetAllScreenNames_f)( const char        * display_name,
                                       char            *** display_names );
typedef int  (*oyGetMonitorInfo_f)   ( const char        * display_name,
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

typedef struct {
  int              type;               /**< set to 120 for ABI compatibility with the actual used header version */
  char             nick[8];            /**< four byte nick name of module */
  int              version;            /**< set to module version; Major * 10000 + Minor * 100 + Micro */
  const char *     help_system_specific;
  const char *     rank_map;           /**< optional JSON rank map */
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

#endif /* OYRANOS_MONITOR_BASE_H */
