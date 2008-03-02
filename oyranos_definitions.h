/** @file oyranos_definitions.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    macros and definitions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2004/11/25
 */



#ifndef OYRANOS_DEFINITIONS_H
#define OYRANOS_DEFINITIONS_H

#define OYRANOS_API 15

/** @deprecated default local search path */
#define OY_PROFILE_PATH_USER_DEFAULT    "~/.color/icc"
/** @brief from OpenICC directory proposal
 *  needs additional 'color' and the usage 'icc/cmms/...'
 *
 *  @since 0.1.8
 */
#define OY_USER_PATH                    "~/.local"
/** default global search path @todo add installation dir */
#define OY_PROFILE_PATH_SYSTEM_DEFAULT  "/usr/share/color/icc"
/** OpenICC directory proposal
 *  needs additional 'share,lib' and 'color' and purpose 'icc,cmms,...'
 *
 *  @since 0.1.8
 */
#define OY_SYS_PATH1                    "/usr"
#define OY_SYS_PATH2                    "/usr/local"
/** @brief Oyranos modules/CMM's environment variable
 *
 *  @since 0.1.8
 */
#define OY_MODULE_PATHS                 "OY_MODULE_PATHS"
/** @brief Oyranos modules/CMM's suffix after the four byte CMM ID
 *
 *  for instance LittleCMS has ID lcms, thus we get lcms_cmm_module
 *
 *  @since 0.1.8
 */
#define OY_MODULE_NAME                 "_cmm_module"
/** @brief Oyranos CMM ID
 *
 *  It is the id of the internal module and the creator id of profiles
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 */
#define OY_MODULE_NICK                 "oyra"

#define OY_POLICY_HEADER "<!--?xml version=\"1.0\" encoding=\"UTF-8\"? -->\n\
<!-- Oyranos policy format 1.0 -->"

/** @brief none profile
 *
 *  The name of a non existent profile.
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/06
 *  @since   2008/02/06 (Oyranos: 0.1.8)
 */
#define OY_PROFILE_NONE                _("[none]")

/*
 * users will usually configure their own settings, while administrators can
 * provide system wide default settings
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
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_GRAY_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_editing_gray"
/** key name for the untagged data assumed source XYZ profile */
#define OY_DEFAULT_ASSUMED_XYZ_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_xyz"
/** key name for the untagged data assumed source Lab profile */
#define OY_DEFAULT_ASSUMED_LAB_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_lab"
/** key name for the untagged data assumed source RGB profile */
#define OY_DEFAULT_ASSUMED_RGB_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_rgb"
/** name for the untagged data assumed source profile for www content */
#define OY_DEFAULT_ASSUMED_WEB_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_web_rgb" /* static_profile */
/** key name for the untagged data assumed source Cmyk profile */
#define OY_DEFAULT_ASSUMED_CMYK_PROFILE      OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_cmyk"
/** key name for the untagged data assumed source Gray profile */
#define OY_DEFAULT_ASSUMED_GRAY_PROFILE       OY_KEY OY_SLASH "default" OY_SLASH "profile_assumed_gray"
/** key name for a standard proofing profile */
#define OY_DEFAULT_PROOF_PROFILE      OY_KEY OY_SLASH "default" OY_SLASH "profile_proof"


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
/** key name for the default rendering with BPC */
#define OY_DEFAULT_RENDERING_BPC             OY_KEY OY_SLASH "behaviour" OY_SLASH "default_rendering_bpc"
/** key name for the default proofing rendering intent */
#define OY_DEFAULT_RENDERING_INTENT_PROOF    OY_KEY OY_SLASH "behaviour" OY_SLASH "default_rendering_intent_proof"
/** key name for soft proofing by default */
#define OY_DEFAULT_PROOF_SOFT    OY_KEY OY_SLASH "behaviour" OY_SLASH "default_proof_soft"
/** key name for hard proofing by default */
#define OY_DEFAULT_PROOF_HARD    OY_KEY OY_SLASH "behaviour" OY_SLASH "default_proof_hard"


#endif /* OYRANOS_DEFINITIONS_H */
