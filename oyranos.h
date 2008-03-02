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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OY_DEFAULT_USER_PROFILE_PATH "~/.color/icc"
#define OY_DEFAULT_SYSTEM_PROFILE__PATH "/usr/share/color/icc"


/*
 * users will usually configure their own settings, while administrators can
 * configure system wide settings
 */

/* definitions */
#define OY_SLASH                 "/"
#define OY_KEY                   "sw/oyranos"
#define OY_SYS                   "system"
#define OY_USER                  "user"

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

#define OY_SLEEP "SLEEP"

/* path names */

#define OY_USER_PATHS            OY_USER OY_SLASH OY_KEY OY_SLASH "paths"
#define OY_USER_PATH             "path"

int	oyPathsRead          (void);
char*	oyPathName           (int number);
int	oyPathAdd            (char* pathname);
void	oyPathRemove         (char* pathname);
void	oyPathSleep          (char* pathname);
void	oyPathActivate       (char* pathname);


/* default profiles */

#define OY_DEFAULT_IMAGE_PROFILE     OY_KEY OY_SLASH "default" OY_SLASH "profile_image"
#define OY_DEFAULT_WORKSPACE_PROFILE OY_KEY OY_SLASH "default" OY_SLASH "profile_workspace"
#define OY_DEFAULT_CMYK_PROFILE      OY_KEY OY_SLASH "default" OY_SLASH "profile_cmyk"

void	oySetDefaultImageProfile          (char* name);
int	oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size);
void	oySetDefaultWorkspaceProfile      (char* name);
int	oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size);
void	oySetDefaultCmykProfile           (char* name);
int	oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size);

char*	oyGetDefaultImageProfileName      ();
char*	oyGetDefaultWorkspaceProfileName  ();
char*	oyGetDefaultCmykProfileName       ();


/* profile access through oyranos */

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
 * OY_PropClass is used as shorthand for an property key
 */
typedef enum OY_PropClass
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
} OY_PropClass;

int	oySetProfileProperty              (char* profilename,
                                           OY_PropClass property_class,
                                           char* value);

/* measure the output and check the consistency for quality ashurance */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //OYRANOS_H
