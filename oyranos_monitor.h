/** @file oyranos_monitor.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
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


/** \addtogroup monitor_api Monitor API
 *  @brief Hardware detection and profile selection for monitors

 *  The notation of screens for the X Window system is equal for traditional 
 *  and Xinerama screens.

 *  @{
 */

 /*  @see @ref device_profiles */

/* monitor names */

int   oyGetMonitorInfo            (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func);
int   oyGetScreenFromPosition     (const char *display_name,
                                   int x,
                                   int y);
char* oyGetDisplayNameFromPosition(const char *display_name,
                                   int x,
                                   int y,
                                   oyAllocFunc_t allocate_func);
char* oyGetMonitorProfile         (const char *display,
                                   size_t     *size,
                                   oyAllocFunc_t allocate_func);
char* oyGetMonitorProfileName     (const char *display,
                                   oyAllocFunc_t allocate_func);

/* TODO set a memory based profile */
int   oySetMonitorProfile         (const char* display_name,
                                   const char* profil_name );
int   oyActivateMonitorProfiles   (const char* display_name);
/* @} */

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_H */
