/** @file oyranos_monitor.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */


#ifndef OYRANOS_MONITOR_H
#define OYRANOS_MONITOR_H

#include "oyranos.h"
#include "oyranos_alpha.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


 /*  @see @ref device_profiles */

/* monitor names */

int   oyGetMonitorInfo_lib        (const char* display,
                                   char**      manufacturer,
                                       char             ** mnft,
                                   char**      model,
                                   char**      serial,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       char             ** host,
                                       double            * colours,
                                       oyBlob_s         ** edid,
                                   oyAlloc_f     allocate_func,
                                       oyStruct_s        * user_data );

int      oyGetAllScreenNames         ( const char        * display_name,
                                       char            *** display_names,
                                       oyAlloc_f           allocateFunc );

int          oyX1MonitorProfileSetup ( const char        * display_name,
                                       const char        * profil_name );
int          oyX1MonitorProfileUnset ( const char        * display_name );
oyRectangle_s * oyX1Rectangle_FromDevice (
                                       const char        * device_name );
char *       oyX1GetMonitorProfile   ( const char        * device_name,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );


#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_H */
