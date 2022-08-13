/** @file oyranos_definitions.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2020 (C) Kai-Uwe Behrmann
 *
 *  @brief    macros and definitions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2004/11/25
 */



#ifndef OYRANOS_DEFINITIONS_H
#define OYRANOS_DEFINITIONS_H

#define OYRANOS_API 17

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
/** @brief Oyranos debug environment variable
 *
 *  Maps to ::oy_debug variable.
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.0
 */
#define OY_DEBUG                       "OY_DEBUG"
/** @brief Oyranos debug environment variable
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_WRITE                 "OY_DEBUG_WRITE"
/** @brief Oyranos debug environment variable
 *
 *  Maps to ::oy_debug_memory variable.
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.0
 */
#define OY_DEBUG_MEMORY                "OY_DEBUG_MEMORY"
/** @brief Oyranos debug environment variable
 *
 *  Maps to ::oy_debug_objects variable.
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.6
 */
#define OY_DEBUG_OBJECTS               "OY_DEBUG_OBJECTS"
/** @brief Oyranos debug environment variable
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_OBJECTS_PRINT_TREE    "OY_DEBUG_OBJECTS_PRINT_TREE"
/** @brief Oyranos debug environment variable
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_OBJECTS_SHOW_NEW      "OY_DEBUG_OBJECTS_SHOW_NEW"
/** @brief Oyranos debug environment variable
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_OBJECTS_SKIP_CMM_CACHES "OY_DEBUG_OBJECTS_SKIP_CMM_CACHES"
/** @brief Oyranos debug environment variable
 *
 *  Allows to trace option switching.
 *  Syntax: "name_fragment:value_fragment"
 *  The option is printed in case of a match or no fragment is delivered.
 *  The double point ':' is optional.
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_OPTION                "OY_DEBUG_OPTION"
/** @brief Oyranos debug environment variable
 *
 *  Integrates backtrace information into ::OY_DEBUG_OPTION
 *  initiated output.
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_OPTION_BACKTRACE      "OY_DEBUG_OPTION_BACKTRACE"
/** @brief Oyranos debug environment variable
 *
 *  Maps to ::oy_debug_signals variable.
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.0
 */
#define OY_DEBUG_SIGNALS               "OY_DEBUG_SIGNALS"
/** @brief Oyranos debug environment variable
 *
 *  Selects a prefered module to run the configuration engine with
 *  persistent DB and signaling. The text format is like for @ref registration.
 *  A default is \b "elDB" or \b "oiDB" depending on your system.
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_DB_MODULE             "OY_DEBUG_DB_MODULE"
/** @brief Oyranos debug environment variable
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.7
 */
#define OY_DEBUG_THREADS               "OY_DEBUG_THREADS"
/** @brief Oyranos debug environment variable
 *
 *  @see @ref debug_vars
 *
 *  @since 0.9.0
 */
#define OY_BACKTRACE                   "OY_BACKTRACE"
/** @brief Oyranos modules/CMM's environment variable
 *
 *  @see @ref runtime_vars
 *
 *  @since 0.1.8
 */
#define OY_MODULE_PATH                 "OY_MODULE_PATH"
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
#define OY_TOP_SHARED            "org"
/** @deprecated use this path for non standardised filters and applications */
#define OY_TOP_INTERNAL          "org"
/** Domain path for standard settings */
#define OY_DOMAIN_STD            "freedesktop"
/** Domain path for Oyranos specific settings */
#define OY_DOMAIN_INTERNAL       "oyranos"
/** Type path for standard settings */
#define OY_TYPE_STD              "openicc"

/** Oyranos general key path in the data base */
#define OY_STD     OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD
/** Oyranos internal key path in the data base */
#define OY_INTERNAL OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD
/** Type path for standard settings */
#define OY_PROFILE_STD           OY_STD OY_SLASH "profile"
/** Type path for standard settings */
#define OY_BEHAVIOUR_STD         OY_STD OY_SLASH "behaviour"
/** Type path for standard settings 
 *  @since 0.9.6
 */
#define OY_CMM_STD               OY_STD OY_SLASH "icc_color"
/** Type path for standard settings 
 *  @since 0.9.7
 */
#define OY_DISPLAY_STD           OY_STD OY_SLASH "display"
/** Type path for standard settings 
 *  @since 0.9.7
 */
#define OY_DEVICE_STD            OY_STD OY_SLASH "device"


/** Oyranos default profiles key path in the data base */
#define OY_REGISTRED_PROFILES    OY_STD OY_SLASH "device_profiles"

#ifndef MAX_PATH
/** maximal path lenght, if not allready defined elsewhere */
#define MAX_PATH 1024
#endif

#define OY_SLEEP "SLEEP"

/* --- default profiles --- */

/** key name for the default Editing profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_EDITING_RGB_PROFILE       OY_PROFILE_STD OY_SLASH "editing_rgb"
/** key name for the default Editing profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_EDITING_CMYK_PROFILE      OY_PROFILE_STD OY_SLASH "editing_cmyk"
/** key name for the default Editing profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_EDITING_XYZ_PROFILE       OY_PROFILE_STD OY_SLASH "editing_xyz"
/** key name for the default Editing profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_EDITING_LAB_PROFILE       OY_PROFILE_STD OY_SLASH "editing_lab"
/** key name for the default Editing profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_EDITING_GRAY_PROFILE      OY_PROFILE_STD OY_SLASH "editing_gray"
/** key name for the untagged data assumed source XYZ profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_ASSUMED_XYZ_PROFILE       OY_PROFILE_STD OY_SLASH "assumed_xyz"
/** key name for the untagged data assumed source Lab profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_ASSUMED_LAB_PROFILE       OY_PROFILE_STD OY_SLASH "assumed_lab"
/** key name for the untagged data assumed source RGB profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_ASSUMED_RGB_PROFILE       OY_PROFILE_STD OY_SLASH "assumed_rgb"
/** name for the untagged data assumed source profile for www content
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_ASSUMED_WEB_PROFILE       OY_PROFILE_STD OY_SLASH "assumed_web" /* static_profile */
/** key name for the untagged data assumed source Cmyk profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_ASSUMED_CMYK_PROFILE      OY_PROFILE_STD OY_SLASH "assumed_cmyk"
/** key name for the untagged data assumed source Gray profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_ASSUMED_GRAY_PROFILE      OY_PROFILE_STD OY_SLASH "assumed_gray"
/** key name for a standard proofing profile
 *
 *  type string containing a profile file name or ID */
#define OY_DEFAULT_PROOF_PROFILE             OY_PROFILE_STD OY_SLASH "proof"
/** key name for a standard effect profile
 *
 *  type string containing a profile file name or ID of a class abstract profile
 *
 *  @since 0.9.6
 */
#define OY_DEFAULT_EFFECT_PROFILE            OY_PROFILE_STD OY_SLASH "effect"


/* --- settings --- */

/** key name for the non profile tagged content action
 *
 *  type string containing a integer [ dont assign(0) | assign assumed Profile(1) | promt(2) ] */
#define OY_ACTION_UNTAGGED_ASSIGN      OY_BEHAVIOUR_STD OY_SLASH "action_untagged_assign"
/** key name for the rgb profile mismatch action
 *
 *  type string containing a integer [ dont change(0) | convert automatic(1) | promt(2) ] */
#define OY_ACTION_MISMATCH_RGB         OY_BEHAVIOUR_STD OY_SLASH "action_missmatch_rgb"
/** key name for the cmyk profile mismatch action
 *
 *  type string containing a integer [ dont change(0) | convert automatic(1) | promt(2) ] */
#define OY_ACTION_MISMATCH_CMYK        OY_BEHAVIOUR_STD OY_SLASH "action_missmatch_cmyk"
/** key name for the allow mixed color space switch
 *
 *  type string containing a integer [ dont change(0) | convert automatic to default CMYK editing space(1) | convert automatic to unspecified CMYK, keep numbers(2) | promt(3) ] */
#define OY_CONVERT_MIXED_COLOR_SPACE_PRINT_DOCUMENT OY_BEHAVIOUR_STD OY_SLASH "mixed_color_spaces_print_doc_convert"
/** key name for generating a mixed color space for internet warning
 *
 *  type string containing a integer [ dont change(0) | convert automatic to default RGB editing space(1) | convert automatic to WWW sRGB(2) | promt(3) ] */
#define OY_CONVERT_MIXED_COLOR_SPACE_SCREEN_DOCUMENT OY_BEHAVIOUR_STD OY_SLASH "mixed_color_spaces_screen_doc_convert"
/** key name for the default rendering intent
 *
 *  type string containing a integer [perceptual(0) | relative colorimetric(1) | saturation(2) | absolute colorimetric(3)] */
#define OY_DEFAULT_RENDERING_INTENT    OY_BEHAVIOUR_STD OY_SLASH "rendering_intent"
/** key name for the default rendering with BPC
 *
 *  type string containing 0 or 1 */
#define OY_DEFAULT_RENDERING_BPC       OY_BEHAVIOUR_STD OY_SLASH "rendering_bpc"
/** key name for the default proofing rendering intent
 *
 *  type string containing a integer [relative colorimetric(0) | absolute colorimetric(1)]
 * */
#define OY_DEFAULT_RENDERING_INTENT_PROOF    OY_BEHAVIOUR_STD OY_SLASH "rendering_intent_proof"
/** key name for the default gamut warning
 *
 *  type string containing 0 or 1 */
#define OY_DEFAULT_RENDERING_GAMUT_WARNING  OY_BEHAVIOUR_STD OY_SLASH "rendering_gamut_warning"
/** key name for soft proofing by default
 *
 *  type string containing 0 or 1 */
#define OY_DEFAULT_PROOF_SOFT          OY_BEHAVIOUR_STD OY_SLASH "proof_soft"
/** key name for hard proofing by default
 *
 *  type string containing 0 or 1 */
#define OY_DEFAULT_PROOF_HARD          OY_BEHAVIOUR_STD OY_SLASH "proof_hard"
/** key name for effect profile by default switch
 *
 *  type string containing 0 or 1 */
#define OY_DEFAULT_EFFECT              OY_BEHAVIOUR_STD OY_SLASH "effect_switch"
/** key name for common display white point mode
 *
 *  - 0: no white point adjustments (old behaviour)
 *  - 1: automatic - use target values from ::OY_DEFAULT_DISPLAY_WHITE_POINT_X , ::OY_DEFAULT_DISPLAY_WHITE_POINT_Y and ::OY_DEFAULT_DISPLAY_WHITE_POINT_Z
 *  - 2: D50
 *  - 3: D55
 *  - 4: D65
 *  - 5: D75
 *  - 6: D93
 *  - 7: first monitors actual media white point as target for all other monitors
 *  - 8: second monitors actual media white point as target for all other monitors
 *  - ...
 *
 *  type string containing a integer
 *
 *  @since 0.9.7
 */
#define OY_DEFAULT_DISPLAY_WHITE_POINT OY_BEHAVIOUR_STD OY_SLASH "display_white_point"
/** key name for common display white point ICC*XYZ target, the ICC*X component in 0.0 - 2.0 range
 *
 *  type string containing a double
 *
 *  @since 0.9.7
 */
#define OY_DEFAULT_DISPLAY_WHITE_POINT_X OY_DISPLAY_STD OY_SLASH "display_white_point_XYZ/[0]"
/** key name for common display white point ICC*XYZ target, the ICC*Y component in 0.0 - 2.0 range
 *
 *  type string containing a double
 *
 *  @since 0.9.7
 */
#define OY_DEFAULT_DISPLAY_WHITE_POINT_Y OY_DISPLAY_STD OY_SLASH "display_white_point_XYZ/[1]"
/** key name for common display white point ICC*XYZ target, the ICC*Z component in 0.0 - 2.0 range
 *
 *  type string containing a double
 *
 *  @since 0.9.7
 */
#define OY_DEFAULT_DISPLAY_WHITE_POINT_Z OY_DISPLAY_STD OY_SLASH "display_white_point_XYZ/[2]"
/** key name for display white point daemon
 *
 *  The setting is by default not present in the DB. Then
 *  ::OY_DEFAULT_DISPLAY_WHITE_POINT is used for manual settings. 
 *
 *  The inbuild alternative value is the "oyranos-monitor-white-point" string 
 *  for the daemon with the same name. oyranos-monitor-white-point is as well
 *  a command line tool fore more options. Unsetting involves removal of the
 *  key from the DB.
 *
 *  type string containing a service identifier
 *
 *  @since 0.9.7
 */
#define OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON OY_DISPLAY_STD OY_SLASH "display_white_point_daemon"
/** key name for a abstract effect profile
 *
 *  A type string containing a profile file name or ID.
 *  The profile is to be used in the display pipe line.
 *  It can be set automatically by a daemon and shall not be manually exposed.
 *
 *  @since 0.9.7
 */
#define OY_DEFAULT_DISPLAY_EFFECT_PROFILE OY_PROFILE_STD OY_SLASH "display_effect"

/** key name for context CMM
 *
 *  type string containing a module
 *
 *  @since 0.9.6
 */
#define OY_DEFAULT_CMM_CONTEXT         OY_CMM_STD OY_SLASH "context"
/** key name for renderer CMM 
 *
 *  type string containing a module
 *
 *  @since 0.9.6
 */
#define OY_DEFAULT_CMM_RENDERER        OY_CMM_STD OY_SLASH "renderer"
/** key name for context CMM fallback 
 *
 *  type string containing a module
 *
 *  @since 0.9.6
 */
#define OY_DEFAULT_CMM_CONTEXT_FALLBACK  OY_CMM_STD OY_SLASH "context_fallback"
/** key name for renderer CMM fallback 
 *
 *  type string containing a module
 *
 *  @since 0.9.6
 */
#define OY_DEFAULT_CMM_RENDERER_FALLBACK OY_CMM_STD OY_SLASH "renderer_fallback"


/** key name for simulation profiles in a filter node; like for proofing
 *
 *  type oyProfiles_s containing profiles
 *
 *  @since 0.9.6
 */
#define OY_PROFILES_SIMULATION               OY_PROFILE_STD OY_SLASH "profiles_simulation"
/** key name for effect profiles in a filter node; like for sepia or b/w etc.
 *
 *  type oyProfiles_s containing profiles
 *
 *  @since 0.9.6
 */
#define OY_PROFILES_EFFECT                   OY_PROFILE_STD OY_SLASH "profiles_effect"


#endif /* OYRANOS_DEFINITIONS_H */
