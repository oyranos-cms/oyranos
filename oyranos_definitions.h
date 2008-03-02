/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2005  Kai-Uwe Behrmann 
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * -----------------------------------------------------------------------------
 *
 * macros and definitions
 * 
 */

/* Date:      25. 11. 2004 */


#ifndef OYRANOS_DEFINITIONS_H
#define OYRANOS_DEFINITIONS_H


/** default local search path */
#define OY_DEFAULT_USER_PROFILE_PATH    "~/.color/icc"
/** default global search path */
#define OY_DEFAULT_SYSTEM_PROFILE_PATH "/usr/share/color/icc"


/*
 * users will usually configure their own settings, while administrators can
 * configure system wide settings
 */

/* --- definitions --- */
/** directory separator */
#define OY_SLASH                 "/"
/** directory separator */
#define OY_SLASH_C               '/'
/** Oyranos general key path in the elektra data base */
#define OY_KEY                   "sw/oyranos"
/** Oyranos default profiles key path in the elektra data base */
#define OY_REGISTRED_PROFILES    OY_KEY "/device_profiles"
/** elektra system keys */
#define OY_SYS                   "system/"
/** elektra user keys */
#define OY_USER                  "user/"

#ifndef MAX_PATH
/** maximal path lenght, if not allready defined elsewhere */
#define MAX_PATH 1024
#endif

#define OY_SLEEP "SLEEP"

/* path names */

/** elektra keys for system wide configured paths */
#define OY_SYSTEM_PATHS              OY_SYS OY_KEY OY_SLASH "paths"
/** elektra key name for system wide configured paths */
#define OY_SYSTEM_PATH               "path"
/** elektra keys for user configured paths */
#define OY_USER_PATHS                OY_USER OY_KEY OY_SLASH "paths"
/** elektra key name for user configured paths */
#define OY_USER_PATH                 "path"


/* --- default profiles --- */

/** key name for the default Workspace profile */
#define OY_DEFAULT_WORKSPACE_PROFILE OY_USER OY_KEY OY_SLASH "default" OY_SLASH "profile_workspace"
/** key name for the default XYZ Input profile */
#define OY_DEFAULT_XYZ_INPUT_PROFILE       OY_USER OY_KEY OY_SLASH "default" OY_SLASH "profile_xyz"
/** key name for the default Lab Input profile */
#define OY_DEFAULT_LAB_INPUT_PROFILE       OY_USER OY_KEY OY_SLASH "default" OY_SLASH "profile_lab"
/** key name for the default RGB Input profile */
#define OY_DEFAULT_RGB_INPUT_PROFILE       OY_USER OY_KEY OY_SLASH "default" OY_SLASH "profile_rgb"
/** key name for the default Cmyk Input profile */
#define OY_DEFAULT_CMYK_INPUT_PROFILE      OY_USER OY_KEY OY_SLASH "default" OY_SLASH "profile_cmyk"


#endif //OYRANOS_DEFINITIONS_H
