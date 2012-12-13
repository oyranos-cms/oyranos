/** @internal
 *  @file oyranos_monitor_internal.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2010  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_H
#define OYRANOS_MONITOR_INTERNAL_H

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"

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
                                       double            * colours,
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

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR__INTERNAL_H */
