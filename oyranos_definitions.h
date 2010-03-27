/** @file oyranos_definitions.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    macros and definitions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
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
#define OY_MODULE_NICK                 "oyIM"

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
/** Top path for standard settings */
#define OY_TOP_SHARED            "shared"
/** @deprecated use this path for non standardised filters and applications */
#define OY_TOP_INTERNAL          "sw"
/** Domain path for standard settings */
#define OY_DOMAIN_STD            "freedesktop.org"
/** Domain path for Oyranos specific settings */
#define OY_DOMAIN_INTERNAL       "oyranos.org"
/** Type path for standard settings */
#define OY_TYPE_STD              "imaging"

/** Oyranos general key path in the elektra data base */
#define OY_STD     OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD
/** Type path for standard settings */
#define OY_PROFILE_STD           OY_STD OY_SLASH "profile" OY_SLASH
/** Type path for standard settings */
#define OY_BEHAVIOUR_STD         OY_STD OY_SLASH "behaviour" OY_SLASH

/** Oyranos default profiles key path in the elektra data base */
#define OY_REGISTRED_PROFILES    OY_STD OY_SLASH "device_profiles"
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
#define OY_PATHS              OY_STD OY_SLASH "paths"
/** elektra key base name for configured paths */
#define OY_PATH               "path"


/* --- default profiles --- */

/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_RGB_PROFILE       OY_PROFILE_STD "editing_rgb"
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_CMYK_PROFILE      OY_PROFILE_STD "editing_cmyk"
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_XYZ_PROFILE       OY_PROFILE_STD "editing_xyz"
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_LAB_PROFILE       OY_PROFILE_STD "editing_lab"
/** key name for the default Editing profile */
#define OY_DEFAULT_EDITING_GRAY_PROFILE      OY_PROFILE_STD "editing_gray"
/** key name for the untagged data assumed source XYZ profile */
#define OY_DEFAULT_ASSUMED_XYZ_PROFILE       OY_PROFILE_STD "assumed_xyz"
/** key name for the untagged data assumed source Lab profile */
#define OY_DEFAULT_ASSUMED_LAB_PROFILE       OY_PROFILE_STD "assumed_lab"
/** key name for the untagged data assumed source RGB profile */
#define OY_DEFAULT_ASSUMED_RGB_PROFILE       OY_PROFILE_STD "assumed_rgb"
/** name for the untagged data assumed source profile for www content */
#define OY_DEFAULT_ASSUMED_WEB_PROFILE       OY_PROFILE_STD "assumed_web" /* static_profile */
/** key name for the untagged data assumed source Cmyk profile */
#define OY_DEFAULT_ASSUMED_CMYK_PROFILE      OY_PROFILE_STD "assumed_cmyk"
/** key name for the untagged data assumed source Gray profile */
#define OY_DEFAULT_ASSUMED_GRAY_PROFILE      OY_PROFILE_STD "assumed_gray"
/** key name for a standard proofing profile */
#define OY_DEFAULT_PROOF_PROFILE             OY_PROFILE_STD "proof"


/* --- settings --- */

/** key name for the non profile tagged content action [ dont assign | assign assumed Profile | promt ] */
#define OY_ACTION_UNTAGGED_ASSIGN      OY_BEHAVIOUR_STD "action_untagged_assign"
/** key name for the rgb profile mismatch action */
#define OY_ACTION_MISMATCH_RGB         OY_BEHAVIOUR_STD "action_missmatch_rgb"
/** key name for the cmyk profile mismatch action */
#define OY_ACTION_MISMATCH_CMYK        OY_BEHAVIOUR_STD "action_missmatch_cmyk"
/** key name for the allow mixed colour space switch */
#define OY_CONVERT_MIXED_COLOUR_SPACE_PRINT_DOCUMENT OY_BEHAVIOUR_STD "mixed_colour_spaces_print_doc_convert"
/** key name for generating a mixed colour space for internet warning */
#define OY_CONVERT_MIXED_COLOUR_SPACE_SCREEN_DOCUMENT OY_BEHAVIOUR_STD "mixed_colour_spaces_screen_doc_convert"
/** key name for the default rendering intent */
#define OY_DEFAULT_RENDERING_INTENT    OY_BEHAVIOUR_STD "rendering_intent"
/** key name for the default rendering with BPC */
#define OY_DEFAULT_RENDERING_BPC       OY_BEHAVIOUR_STD "rendering_bpc"
/** key name for the default proofing rendering intent */
#define OY_DEFAULT_RENDERING_INTENT_PROOF    OY_BEHAVIOUR_STD "rendering_intent_proof"
/** key name for the default gamut warning */
#define OY_DEFAULT_RENDERING_GAMUT_WARNING  OY_BEHAVIOUR_STD "rendering_gamut_warning"
/** key name for the default rendering precission */
#define OY_DEFAULT_RENDERING_HIGH_PRECISSION  OY_BEHAVIOUR_STD "rendering_high_precission"
/** key name for soft proofing by default */
#define OY_DEFAULT_PROOF_SOFT          OY_BEHAVIOUR_STD "proof_soft"
/** key name for hard proofing by default */
#define OY_DEFAULT_PROOF_HARD          OY_BEHAVIOUR_STD "proof_hard"


/** see http://www.freedesktop.org/wiki/Specifications/icc_profiles_in_x_spec */
#define OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE "_ICC_PROFILE"

#endif /* OYRANOS_DEFINITIONS_H */
