/** @file oyranos_monitor_internal_cs.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2010  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_CS_H
#define OYRANOS_MONITOR_INTERNAL_CS_H

#include "config.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"

#include <Carbon/Carbon.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \internal  platformdependent */
typedef struct {
  oyOBJECT_e       type_;              /**< object type oyOBJECT_MONITOR_S */
  char           * name;               /**< display name, like screen */
  char           * host;               /**< host name only - host */
  char           * identifier;         /**<  - _x_y_wxh */
  int              geo[6];             /**< display screen x y width height */
  CGDirectDisplayID id;                /**< osX monitor */
  int              screen;             /**< pos in osX CGGetActiveDisplayList */
} oyMonitor_s;

CGDirectDisplayID  oyMonitor_device_ ( oyMonitor_s       * disp );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_INTERNAL_CS_H */
