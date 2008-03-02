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
 * API
 * 
 */

/* Date:      25. 11. 2004 */


#ifndef OYRANOS_H
#define OYRANOS_H

#include "oyranos_debug.h"
#include "oyranos_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * users will usually configure their own settings, while administrators can
 * configure system wide settings
 */


/* path names */

int   oyPathsCount         (void);
char* oyPathName           (int number);
int   oyPathAdd            (const char* pathname);
void  oyPathRemove         (const char* pathname);
void  oyPathSleep          (const char* pathname);
void  oyPathActivate       (const char* pathname);


/* --- default profiles --- */

int	oySetDefaultImageProfile          (const char* name);
int	oySetDefaultImageProfileBlock     (const char* name, void* mem, int size);
int	oySetDefaultWorkspaceProfile      (const char* name);
int	oySetDefaultWorkspaceProfileBlock (const char* name, void* mem, int size);
int	oySetDefaultXYZProfile            (const char* name);
int	oySetDefaultXYZProfileBlock       (const char* name, void* mem, int size);
int	oySetDefaultLabProfile            (const char* name);
int	oySetDefaultLabProfileBlock       (const char* name, void* mem, int size);
int	oySetDefaultRGBProfile            (const char* name);
int	oySetDefaultRGBProfileBlock       (const char* name, void* mem, int size);
int	oySetDefaultCmykProfile           (const char* name);
int	oySetDefaultCmykProfileBlock      (const char* name, void* mem, int size);

char*	oyGetDefaultImageProfileName      ();
char*	oyGetDefaultWorkspaceProfileName  ();
char*	oyGetDefaultXYZProfileName        ();
char*	oyGetDefaultLabProfileName        ();
char*	oyGetDefaultRGBProfileName        ();
char*	oyGetDefaultCmykProfileName       ();

/* --- profile checking --- */

int	oyCheckProfile (const char* name, int flag);
int	oyCheckProfileMem (const void* mem, int size, int flags);


/* --- profile access through oyranos --- */

/* check for sizes before using any profiles */
int	oyGetProfileSize                  (const char* profilename);
void*	oyGetProfileBlock                 (const char* profilename, int* size);

/* device profiles */

/*
 * There different approaches to select an (mostly) fitting profile
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

typedef enum  {
  DISPLAY,                 /* dynamic viewing */
  PRINTER,                 /* static media (dye, ink, offset, imagesetters) */
  SCANNER,                 /* contact digitiser */
  CAMERA,                  /* virtual or contactless image capturing */
} DEVICETYP;

/* simply pass 0 for not specified properties
 *
 * char* profile_name = oyGetDeviceProfile ("EIZO", "LCD2100", "ID 87-135.19",
 *                                          "calc.pool", 0, "100lux", 0,
 *                                          "mga1450");
 * if (profile_name)
 * { char* ptr = (char*) malloc (oyGetProfileSize (profile_name),sizeof(int);
 *   ptr = oyGetProfileBlock (profile_name);
 *     // do something
 *   free (ptr);
 * }
 *
 */
char* oyGetDeviceProfile                  (DEVICETYP typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3);

int	oySetDeviceProfile                    (DEVICETYP typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           const char* profilename,
                                           void* mem,
                                           int   size);
#if 0
int	oySetProfileProperty                  (char* profilename,
                                           char* property,
                                           char* value);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OYRANOS_H */
