/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004  Kai-Uwe Behrmann 
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
 * sorting
 * 
 */

/* Date:      25. 11. 2004 */


#ifndef OYRANOS_H
#define OYRANOS_H

#include "oyranos_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * users will usually configure their own settings, while administrators can
 * configure system wide settings
 */


/* path names */

int	oyPathsCount         (void);
char*	oyPathName           (int number);
int	oyPathAdd            (char* pathname);
void	oyPathRemove         (char* pathname);
void	oyPathSleep          (char* pathname);
void	oyPathActivate       (char* pathname);


/* --- default profiles --- */

int	oySetDefaultImageProfile          (char* name);
int	oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size);
int	oySetDefaultWorkspaceProfile      (char* name);
int	oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size);
int	oySetDefaultCmykProfile           (char* name);
int	oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size);

char*	oyGetDefaultImageProfileName      ();
char*	oyGetDefaultWorkspaceProfileName  ();
char*	oyGetDefaultCmykProfileName       ();

/* --- profile checking --- */

int	oyCheckProfile (char* name);
int	oyCheckProfileMem (void* mem, size_t size);


/* --- profile access through oyranos --- */

/* check for sizes before using any profiles */
size_t	oyGetProfileSize                  (char* profilename);
void*	oyGetProfileBlock                 (char* profilename);

/* device profiles */

/*
 * There may be 3 approaches to find an (mostly) fitting profile by no measuring
 *
 * A: search and compare all available profiles by 
 *    - ICC profile class
 *    - Manufacturer / Model (as written in profile tags)
 *    - other hints
 * B: install an profile and tell oyranos about the belonging device and the
 *    invalidating period
 * C: look for similarities of devices of allready installed profiles
 *
 */

/* simply pass 0 for char* or -1 for ints for not specified properties
 *
 * char* profile_name = oyGetDeviceProfile ("EIZO", "CG2100",
 *                                         "calc.pool", 0, "D50", "100lux", 0);
 * if (profile_name)
 * { char* ptr = (char*) malloc (oyGetProfileSize (profile_name),sizeof(size_t);
 *   ptr = oyGetProfileBlock (profile_name);
 *     // do something
 *   free (ptr);
 * }
 *
 */
char*	oyGetDeviceProfile                (char* manufacturer,
                                           char* model,
                                           char* host,
                                           char* port,
                                           char* attrib1,
                                           char* attrib2,
                                           char* attrib3);

int	oySetDeviceProfile                (char* manufacturer,
                                           char* model,
                                           char* host,
                                           char* port,
                                           char* profilename,
                                           void* mem,
                                           size_t size);

/*
 * OY_Prop is used as shorthand for an property key
 */
typedef enum OY_Prop
{
  MANUFACTURER,
  MODEL,
  HOST,		/* The computer the device is primarily connected to */
  PORT,		/* The port (to describe the 1. Monitor on the thierd vga card:
		*   "vga:2 connector:0" )     */
  RESOLUTION,	/* mainly cameras and scanners ("high", "1200dpi") */
  BITDEPTH,	/* cameras / scanners  1/4/8/12/14/16 bit per colour channel */
  EXTENDED	/* use this to describe an more complex scenario like printer
		   settings, value should contain corresponding key set name */
} OY_Prop;

int	oySetProfileProperty              (char* profilename,
                                           OY_Prop property_class,
                                           char* value);

/* measure the output and check the consistency for quality ashurance */

// debugging variable - set 0 to off (default), set 1 to switch debugging on
extern int oy_debug;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //OYRANOS_H
