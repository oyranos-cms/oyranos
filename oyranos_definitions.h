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
#define OY_PROFILE_PATH_USER_DEFAULT    "~/.color/icc"
/** default global search path */
#define OY_PROFILE_PATH_SYSTEM_DEFAULT  "/usr/share/color/icc"


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

/** elektra keys for configured paths */
#define OY_PATHS              OY_KEY OY_SLASH "paths"
/** elektra key base name for configured paths */
#define OY_PATH               "path"


/* --- default profiles --- */

/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_RGB_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_editing_rgb"
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_CMYK_PROFILE      OY_KEY OY_SLASH "default" OY_SLASH "profile_editing_cmyk"
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_XYZ_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_editing_xyz"
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_LAB_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_editing_lab"
/** key name for the untagged data assumed source XYZ profile */
#define OY_DEFAULT_ASSUMED_XYZ_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_xyz"
/** key name for the untagged data assumed source Lab profile */
#define OY_DEFAULT_ASSUMED_LAB_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_lab"
/** key name for the untagged data assumed source RGB profile */
#define OY_DEFAULT_ASSUMED_RGB_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_rgb"
/** name for the untagged data assumed source profile for www content */
#define OY_DEFAULT_ASSUMED_WEB_PROFILE       "sRGB.icc" /* static_profile */
/** key name for the untagged data assumed source Cmyk profile */
#define OY_DEFAULT_ASSUMED_CMYK_PROFILE      OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_cmyk"


/* --- settings --- */

/** key name for the non profile tagged content action [ dont assign | assign assumed Profile | promt ] */
#define OY_ACTION_UNTAGGED_ASSIGN            OY_KEY OY_SLASH "behaviour" OY_SLASH "action_untagged_assign"
/** key name for the rgb profile mismatch action */
#define OY_ACTION_MISMATCH_RGB          OY_KEY OY_SLASH "behaviour" OY_SLASH "action_missmatch_rgb"
/** key name for the cmyk profile mismatch action */
#define OY_ACTION_MISMATCH_CMYK         OY_KEY OY_SLASH "behaviour" OY_SLASH "action_missmatch_cmyk"
/** key name for the allow mixed colour space switch */
#define OY_CONVERT_MIXED_COLOUR_SPACE_PRINT_DOCUMENT OY_KEY OY_SLASH "behaviour" OY_SLASH "mixed_colour_spaces_print_doc_convert"
/** key name for generating a mixed colour space for internet warning */
#define OY_CONVERT_MIXED_COLOUR_SPACE_SCREEN_DOCUMENT OY_KEY OY_SLASH "behaviour" OY_SLASH "mixed_colour_spaces_screen_doc_convert"
/** key name for the default rendering intent */
#define OY_DEFAULT_RENDERING_INTENT          OY_KEY OY_SLASH "behaviour" OY_SLASH "default_rendering_intent"
/** key name for the default prooing rendering intent */
#define OY_DEFAULT_RENDERING_INTENT_PROOF    OY_KEY OY_SLASH "behaviour" OY_SLASH "default_rendering_intent_proof"


#endif /* OYRANOS_DEFINITIONS_H */
