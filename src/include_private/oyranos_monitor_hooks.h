/**
 *  @file oyranos_monitor_hooks.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2018  Kai-Uwe Behrmann
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

typedef struct oyMonitor_s oyMonitor_s;
/**
 *  @ingroup monitor_hooks_api
 *  @brief   create a monitor object
 *
 *  Almost any other calls of the hook need this object.
 *
 *  @param[in]     monitor_name        the identifier of the device as returned
 *                                     by oyGetAllMonitorNames_f
 *  @return                            object
 *
 *  @version Oyranos Monitor: 0.9.7
 *  @date    2018/07/22
 *  @since   2018/07/22 (Oyranos Monitor: 0.9.7)
 */
typedef oyMonitor_s * (*oyGetMonitorFromName_f) (
                                       const char        * monitor_name );
/**
 *  @ingroup monitor_hooks_api
 *  @brief   delete a monitor object
 *
 *  Release all resources of the monitor object.
 *
 *  @param[in]     monitor             object
 *  @return                            0 - success; < 0 - issue; > 0 - error
 *
 *  @version Oyranos Monitor: 0.9.7
 *  @date    2018/07/22
 *  @since   2018/07/22 (Oyranos Monitor: 0.9.7)
 */
typedef int  (*oyFreeMonitor_f)      ( oyMonitor_s      ** monitor );

#define OY_CALIB_ERROR 0x01                 /**< system error, e.g. xcalib not installed */
#define OY_CALIB_VCGT_NOT_CONTAINED 0x04    /**< vcgt not found inside ICC profile */
#define OY_CALIB_DEVICE_NOT_SUPPORTED 0x08  /**< device can not be calibrated */
#define OY_CALIB_NOT_IMPLEMENTED 0x10       /**< function has no effect */
#define OY_CALIB_WITH_PROFILE_SETUP 0x20    /**< handled in oySetupMonitorProfile_f */
/** Function oySetupMonitorCalibration_f
 *  @ingroup monitor_hooks_api
 *  @brief   set all device system specific properties
 *
 *  The device type might need special setup to make it calibrated.
 *  The calibration data is inside the non standard vcgt tag. This
 *  VideCardGammaTable (vcgt) is uploaded into the devices gamma ramps.
 *
 *  @param[in]     monitor             object
 *  @param[in]     profle_name         the fopen()able on disk file name
 *  @param[in]     profile_data        a memory block containing a ICC profile
 *  @param[in]     profile_data_size   the size of profile_data
 *  @return                            status flags:
 *                                     - 0 : success, plain vgct load
 *                                     - OY_CALIB_ERROR : error, something went wrong with system;
 *                                       That can be authorisation problems, API or tool misses ...
 *                                     - OY_CALIB_VCGT_NOT_CONTAINED : no vcgt found
 *                                     - OY_CALIB_DEVICE_NOT_SUPPORTED : device supports no gamma setup
 *                                     - OY_CALIB_NOT_IMPLEMENTED : does nothing, not implemented
 *                                     - OY_CALIB_WITH_PROFILE_SETUP : combined in profile load; e.g. on OS X
 *
 *  @version Oyranos Monitor: 0.9.7
 *  @date    2018/02/22
 *  @since   2018/02/22 (Oyranos Monitor: 0.9.7)
 */
typedef int  (*oySetupMonitorCalibration_f) (
                                       oyMonitor_s       * monitor,
                                       const char        * profil_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size );

/** Function oySetupMonitorProfile_f
 *  @ingroup monitor_hooks_api
 *  @brief   set all device system specific properties
 *
 *  The device type might need special setup to get a ICC profile assigned,
 *  visible in a device specific manner and transported by means of the device
 *  handling protocols and API's. This function is for the device specific
 *  setup part two.
 *
 *  @param[in]     monitor             object
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
                                       oyMonitor_s       * monitor,
                                       const char        * profil_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size );

/** Function oyUnsetMonitorProfile_f
 *  @ingroup monitor_hooks_api
 *  @brief   unset all device system specific properties
 *
 *  The device type might need special setup to get a ICC profile assigned,
 *  visible in a device specific manner and transported by means of the device
 *  handling protocols and API's. This function is for the device specific
 *  setup cleaning.
 *
 *  @param[in]     monitor             object
 *  @return                            status
 *
 *  @version Oyranos Monitor: 0.9.6
 *  @date    2016/11/27
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oyUnsetMonitorProfile_f) (
                                       oyMonitor_s       * monitor );

/** Function oyGetRectangleFromMonitor_f
 *  @ingroup monitor_hooks_api
 *  @brief   get area dimensions in pixel
 *
 *  Display devices typical cover a certain pixel area and have offsets to
 *  neighbours.
 *
 *  @param[in]     monitor             object
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
                                       oyMonitor_s       * monitor,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height );

/** Function oyGetMonitorProfile_f
 *  @ingroup monitor_hooks_api
 *  @brief   get a system specific monitor profile
 *
 *  @param[in]     monitor             object
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
                                       oyMonitor_s       * monitor,
                                       int                 flags,
                                       size_t            * size );

/** Function oyGetAllMonitorNames_f
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
 *  @version Oyranos Monitor: 0.9.7
 *  @date    2021/11/24
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oyGetAllMonitorNames_f)(const char        * display_name,
                                       char            *** monitor_names );

/** Function oyGetMonitorInfo_f
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
 *  @param[in]     monitor             object
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
 *  @param[in,out] debug_info          additional text infos
 *  @return                            0 - success; < 0 - issue; > 0 - error
 *
 *  @version Oyranos Monitor: 0.9.7
 *  @date    2021/11/24
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef int  (*oyGetMonitorInfo_f)   ( oyMonitor_s       * monitor,
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
                                       int                 refresh_edid,
                                       char             ** debug_info );

/** @struct  oyMonitorHooks3_s
 *  @ingroup monitor_hooks_api
 *  @brief   provide a set of hooks to enable support for a display system
 *
 *  The oyMonitorHooks3_s is used by the oyranos_cmm_disp.c code in order to
 *  wire the hooks into the Oyranos device config scheme.
 *
 *  @version Oyranos Monitor: 0.9.7
 *  @date    2018/07/22
 *  @since   2016/11/27 (Oyranos Monitor: 0.9.6)
 */
typedef struct {
  int              type;               /**< set to 123 for ABI compatibility with the actual used header version */
  char             nick[8];            /**< four byte nick name of module + terminating zero */
  int              version;            /**< set to module version; Major * 10000 + Minor * 100 + Micro */
  const char *     help_system_specific; /**< System specific description for developers. E.g. how monitor_name's are build etc. */
  const char *     rank_map;           /**< optional JSON rank map */
  oyGetMonitorFromName_f      getMonitor;             /**< obtain a monitor object */
  oyFreeMonitor_f             freeMonitor;            /**< release a monitor object */
  oySetupMonitorCalibration_f setupCalibration;       /**< vcgt activation */
  oySetupMonitorProfile_f     setupProfile;           /**< ICC profile activation */
  oyUnsetMonitorProfile_f     unsetProfile;           /**< ICC profile deactivation */
  oyGetRectangleFromMonitor_f getRectangle;           /**< get the monitor display area */
  oyGetMonitorProfile_f       getProfile;             /**< obtain a ICC profile */
  oyGetAllMonitorNames_f      getAllMonitorNames;     /**< detect all monitors to present as list to users */
  oyGetMonitorInfo_f          getInfo;                /**< obtain info to compare the device with other devices. */
} oyMonitorHooks3_s;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_HOOKS_H */
