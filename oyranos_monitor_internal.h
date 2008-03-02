/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2005  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 *
 * hardware API
 * 
 */

/* Date:      01. 02. 2005 */


#ifndef OYRANOS_MONITOR_INTERNAL_H
#define OYRANOS_MONITOR_INTERNAL_H

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


struct oy_display_s_ {
  oyOBJECT_TYPE type;        /**< object type */
  char         *name;        /**< traditional display name - host:0 / :0 */
  char         *host;        /**< host name only - host */
  char         *identifier;  /**<  - 0.1_x_y_wxh */
  int           geo[6];      /**< display screen x y width height */
  Display      *display;     /**< logical display */
  int           screen;      /**< external screen number to call for X */
};
/** internal only , platformdependent */
typedef struct oy_display_s_ oy_display_s;

int         oyGetDisplay_                 ( const char *display_name,
                                            oy_display_s *disp );
const char* oyDisplayName_                ( oy_display_s *disp );
const char* oyDisplayHostName_            ( oy_display_s *disp );
const char* oyDisplayIdentifier_          ( oy_display_s *disp );
char*       oyDisplayScreenNumber_        ( oy_display_s *disp );
Display*    oyDisplayDevice_              ( oy_display_s *disp );
int         oyDisplayDeviceScreenNumber_  ( oy_display_s *disp );
int         oyDisplayNumber_              ( oy_display_s *disp );
int         oyDisplayScreen_              ( oy_display_s *disp );
int         oyDisplayX_                   ( oy_display_s *disp );
int         oyDisplayY_                   ( oy_display_s *disp );
int         oyDisplayWidth_               ( oy_display_s *disp );
int         oyDisplayHeight_              ( oy_display_s *disp );


int oyFree_       (void *oy_structure);


/** an incomplete DDC struct */
struct DDC_EDID1 {
 char dummy[18];
 char major_version;
 char minor_version;
 char dummy1[58];
 char HW_ID[10];                  /**< 113 + 10 */
 char dummy2[7];
 char Mnf_Model[16];
 char dummy3[2];
 char Serial[10];                 /**< standard? */
};

void oyUnrollEdid1_               (struct DDC_EDID1 *edi,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func);


#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR__INTERNAL_H */
