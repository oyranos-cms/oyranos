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

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


 /*  @see @ref device_profiles */

/* monitor names */

int   oyGetMonitorInfo_lib        (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAlloc_f     allocate_func);
int   oyGetScreenFromPosition_lib (const char *display_name,
                                   int x,
                                   int y);
char* oyGetDisplayNameFromPosition_lib(const char *display_name,
                                   int x,
                                   int y,
                                   oyAlloc_f     allocate_func);
char* oyGetMonitorProfile_lib     (const char *display,
                                   size_t     *size,
                                   oyAlloc_f     allocate_func);
char*  oyGetMonitorProfileNameFromDB_lib ( const char        * display,
                                       oyAlloc_f           allocate_func );

/* TODO set a memory based profile */
int   oySetMonitorProfile_lib     (const char* display_name,
                                   const char* profil_name );
int      oyActivateMonitorProfiles_lib( const char        * display_name );

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_H */
