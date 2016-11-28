/**
 *  @file oyranos_monitor_hooks.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2016  Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/01
 *
 */

#ifndef OYRANOS_MONITOR_HOOKS_H
#define OYRANOS_MONITOR_HOOKS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/** @fn      oySetupMonitorProfile_f
 *  @ingroup monitor_hooks_api
 *  @brief   set all device system specific properties
 *
 *  The device type might need special setup to get a ICC profile assigned,
 *  visible in a device specific manner and transported by means of the device
 *  handling protocols and API's. This function is for the device specific
 *  setup.
 *
 *  @param[in]     monitor_name        the identifier of the device as returned
 *                                     by oyGetAllMonitorNames_f
 *  @param[in]     profle_name         the fopen()able on disk file name
 *  @param[in]     profile_data        a memory block containing a ICC profile
 *  @param[in]     profile_data_size   the size of profile_data
 *  @return                            status
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oySetupMonitorProfile_f) (
                                       const char        * monitor_name,
                                       const char        * profil_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size );

/** @fn      oyUnsetMonitorProfile_f
 *  @ingroup monitor_hooks_api
 *  @brief   unset all device system specific properties
 *
 *  The device type might need special setup to get a ICC profile assigned,
 *  visible in a device specific manner and transported by means of the device
 *  handling protocols and API's. This function is for the device specific
 *  setup cleaning.
 *
 *  @param[in]     monitor_name        the identifier of the device as returned
 *                                     by oyGetAllMonitorNames_f
 *  @return                            status
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oyUnsetMonitorProfile_f) (
                                       const char        * monitor_name );

/** @fn      oyGetRectangleFromMonitor_f
 *  @ingroup monitor_hooks_api
 *  @brief   get area dimensions in pixel
 *
 *  Display devices typical cover a certain pixel area and have offsets to
 *  neighbours.
 *
 *  @param[in]     monitor_name        the identifier of the device as returned
 *                                     by oyGetAllMonitorNames_f
 *  @param[in]     x                   x offset
 *  @param[in]     y                   y offset
 *  @param[in]     width               w dimension
 *  @param[in]     height              h dimension
 *  @return                            status
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oyGetRectangleFromMonitor_f) (
                                       const char        * monitor_name,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height );

/** @fn      oyGetMonitorProfile_f
 *  @ingroup monitor_hooks_api
 *  @brief   get a system specific monitor profile
 *
 *  @param[in]     monitor_name        the identifier of the device as returned
 *                                     by oyGetAllMonitorNames_f
 *  @param[in]     flags               profile selector
 *                                     - default: Return a target space profile.
 *                                                That will be the device
 *                                                profile or with a
 *                                                color server running typical
 *                                                sRGB.
 *                                     - 0x01: Return the real device profile.
 *                                             The requestor knows how to opt-out
 *                                             of color management and handle
 *                                             early color transforms by it own.
 *  @param[out]    size                returned data block size
 *  @return                            binary ICC profile
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef char * (*oyGetMonitorProfile_f) (
                                       const char        * monitor_name,
                                       int                 flags,
                                       size_t            * size );

/** @fn      oyGetAllMonitorNames_f
 *  @ingroup monitor_hooks_api
 *  @brief   get a list of connected monitors
 *
 *  Omit all non active monitors and non connected ports, as Oyranos
 *  has no notion of invisible devices.
 *
 *  @param[in]     display_name        the system specific display name
 *  @param[out]    monitor_names       a list of monitor identifier
 *  @return                            count of detected monitors
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oyGetAllMonitorNames_f)(const char        * display_name,
                                       char            *** monitor_names );

/** @fn      oyGetMonitorInfo_f
 *  @ingroup monitor_hooks_api
 *  @brief   get various informations about a monitor device
 *
 *  Get the informations about the device. Some values are host or system
 *  specific.
 *  If the EDID is available return that and omit all properties which
 *  can later be extracted from EDID. The from EDID values are meant to
 *  provide a way for system specific replacements in case the EDID is
 *  currently not available.
 *
 *  @param[in]     monitor_name        the system specific display name
 *  @param[out]    manufacturer        from EDID
 *  @param[out]    mnft                the three letter abbeviation from EDID
 *  @param[out]    model               from EDID
 *  @param[out]    serial              from EDID, This property is very
 *                                     specific to a given device. If nothing
 *                                     else is returned, then generic profiles
 *                                     might not work.
 *  @param[out]    vendor              from EDID
 *  @param[out]    device_geometry     widthxheight+x_offset+y_offset relative
 *                                     to the main monitor
 *  @param[out]    system_port         the system specific port name
 *  @param[out]    host                computer name
 *  @param[out]    week                from EDID
 *  @param[out]    year                from EDID
 *  @param[out]    mnft_id             from EDID
 *  @param[out]    model_id            from EDID
 *  @param[out]    colors              A list of primary color values.
 *                                     They are used to generate a fallback
 *                                     ICC profile. The values shall contain:
 *                                     - 0: red_x
 *                                     - 1: red_y
 *                                     - 2: green_x
 *                                     - 3: green_y
 *                                     - 4: blue_x
 *                                     - 5: blue_y
 *                                     - 6: white_x
 *                                     - 7: white_y
 *                                     - 8: gamma
 *                                     The values are contained in EDID, but
 *                                     could be obtained from other sources as
 *                                     well.
 *  @param[out]    edid                The EDID data blob as returned by i2c.
 *  @param[out]    edid_size           The edid data blob size.
 *  @param[in]     refresh_edid        request a edid refresh
 *  @return                            0 - success; < 0 - issue; > 0 - error
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oyGetMonitorInfo_f)   ( const char        * monitor_name,
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

/** @struct  oyMonitorHooks_s
 *  @ingroup monitor_hooks_api
 *  @brief   provide a set of hooks to enable support for a display system
 *
 *  The oyMonitorHooks_s is used by the oyranos_cmm_disp.c code in order to
 *  wire the hooks into the Oyranos device config scheme.
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef struct {
  int              type;               /**< set to 120 for ABI compatibility with the actual used header version */
  char             nick[8];            /**< four byte nick name of module */
  int              version;            /**< set to module version; Major * 10000 + Minor * 100 + Micro */
  const char *     help_system_specific; /**< System specific description for developers. E.g. how monitor_name's are build etc. */
  const char *     rank_map;           /**< optional JSON rank map */
  oySetupMonitorProfile_f     setupProfile;           /**< profile activation */
  oyUnsetMonitorProfile_f     unsetProfile;           /**< profile deactivation */
  oyGetRectangleFromMonitor_f getRectangle;           /**< get the monitor display area */
  oyGetMonitorProfile_f       getProfile;             /**< obtain a ICC profile */
  oyGetAllMonitorNames_f      getAllMonitorNames;     /**< detect all monitors to present as list to users */
  oyGetMonitorInfo_f          getInfo;                /**< obtain info to compare the device with other devices. */
} oyMonitorHooks_s;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_HOOKS_H */
