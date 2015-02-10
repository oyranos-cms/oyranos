/** @file oyranos.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#ifndef OYRANOS_H
#define OYRANOS_H

#include "oyranos_definitions.h"
#include "oyranos_core.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief Widget Groups 
 */
typedef enum  {
  oyGROUP_START = 0,
  oyGROUP_POLICY,                  /**< Policies */
  oyGROUP_PATHS,                   /**< Paths */
  oyGROUP_DEFAULT_PROFILES,        /**< Default Profiles */
  oyGROUP_DEFAULT_PROFILES_EDIT,   /**< Default Editing Profiles */
  oyGROUP_DEFAULT_PROFILES_ASSUMED,/**< Default Assumed Profiles */
  oyGROUP_DEFAULT_PROFILES_PROOF,  /**< Default Proofing Profiles */
  oyGROUP_BEHAVIOUR,               /**< Behaviour */
  oyGROUP_BEHAVIOUR_RENDERING,            /**< Rendering Behaviour */
  oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, /**< PDF Generation Options */
  oyGROUP_BEHAVIOUR_MISSMATCH,            /**< Profile Missmatch Behaviour */
  oyGROUP_BEHAVIOUR_PROOF,                /**< Profile Missmatch Behaviour */
  oyGROUP_CMM,                     /**< CMM selection */
  oyGROUP_ALL,                     /**< just for easen Gui design */
  oyGROUP_EXTERN = 200             /**< start of groups in a extern module */
} oyGROUP_e;


/* --- behaviour --- */

/** @brief Behaviour settings
 *
 *  possibly include the default profiles here
 */
typedef enum  {
  oyBEHAVIOUR_START = 30,
  oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN,  /**< What to do if image is untagged ? */
  oyBEHAVIOUR_ACTION_OPEN_MISMATCH_RGB,/**< What to do if profiles mismatch ? */
  oyBEHAVIOUR_ACTION_OPEN_MISMATCH_CMYK,/**< What to do if profiles mismatch ?*/
  oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT,/**< Convert befor save for Print? */
  oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN,/**< Convert befor save for Screen? */
  oyBEHAVIOUR_RENDERING_INTENT,        /**< Standard color transformations */
  oyBEHAVIOUR_RENDERING_BPC,           /**< use BlackPointCompensation */
  oyBEHAVIOUR_RENDERING_INTENT_PROOF,  /**< Proofing color transformations */
  oyBEHAVIOUR_PROOF_SOFT,              /**< Proofing by default for screen */
  oyBEHAVIOUR_PROOF_HARD,              /**< Proofing by default for printing */
  oyBEHAVIOUR_RENDERING_GAMUT_WARNING, /**< mark out of gamut colors */
  oyBEHAVIOUR_END                      /**< just for easen Gui design */
} oyBEHAVIOUR_e;

enum  {
  oyNO,                                /**< dont do it */
  oyYES,                               /**< automaticaly perform action */
  oyASK                                /**< popup dialog */
}; /**< for oyBEHAVIOUR_ACTION */

#define oyALL oyASK

int         oyGetBehaviour             (oyBEHAVIOUR_e     type);
int          oySetBehaviour          ( oyBEHAVIOUR_e       type,
                                       oySCOPE_e           scope,
                                       int                 choice);

/* --- CMM --- */

/** \addtogroup cmm_handling
 *  @{ */
/** @brief CMM Types
 */
typedef enum {
  oyCMM_START = 300,
  oyCMM_CONTEXT,                       /**< parse profiles, options and create a device link */
  oyCMM_RENDERER,                      /**< take a device link and color convert */
  oyCMM_CONTEXT_FALLBACK,              /**< parse profiles, options and create a device link */
  oyCMM_RENDERER_FALLBACK,             /**< take a device link and color convert */
  oyCMM_END                            /**< just for easen Gui design */
} oyCMM_e;
/** @} */ /* cmm_handling */

char *       oyGetCMMPattern         ( oyCMM_e             type,
                                       uint32_t            flags,
                                       oyAlloc_f           allocate_func );
int          oySetCMMPattern         ( oyCMM_e             type,
                                       uint32_t            flags,
                                       oySCOPE_e           scope,
                                       const char        * name );
/** \addtogroup cmm_handling
 *  @{ */
/** @brief the system specific module name; e.g. a library name */
#define oyNAME_MODULE (oyNAME_DESCRIPTION + 2)
/** @brief the logical name for selection */
#define oyNAME_REGISTRATION (oyNAME_DESCRIPTION + 3)
/** @brief a logical name for registration search */
#define oyNAME_PATTERN (oyNAME_DESCRIPTION + 4)
/** @brief use inbuild values */
#define oySOURCE_FILTER 0x02
/** @brief use persistenly stored DB values */
#define oySOURCE_DATA 0x04
/** @} */ /* cmm_handling */
char **      oyGetCMMs               ( oyCMM_e             type,
                                       int                 name_type,
                                       uint32_t            flags,
                                       oyAlloc_f           allocate_func );
char *       oyCMMRegistrationToName ( const char        * registration,
                                       oyCMM_e             type,
                                       int                 name_type,
                                       uint32_t            flags,
                                       oyAlloc_f           allocate_func );
char *       oyCMMNameToRegistration ( const char        * name,
                                       oyCMM_e             type,
                                       int                 name_type,
                                       uint32_t            flags,
                                       oyAlloc_f           allocate_func );

/* --- policies --- */

int         oyPolicySaveActual        ( oyGROUP_e         group,
                                        const char      * name );
char*       oyPolicyToXML              (oyGROUP_e         group,
                                        int               add_header,
                                        oyAlloc_f         alloc_func);
int         oyReadXMLPolicy            (oyGROUP_e         group,
                                        const char       *xml);
int         oyPolicySet                (const char      * policy,
                                        const char      * full_name );
int        oyPolicyFileNameGet       ( const char        * policy_name,
                                       char             ** full_name,
                                       oyAlloc_f           allocateFunc );


/** \addtogroup path_names Path Names
 *  @brief   Access to data path names for Installation
 *
 *  The obtained informations can be used outside Oyranos, to do system
 *  specific operations, which do not easily fit into the library.
 *  @{ */
/**
 *  @brief   data type selectors
 */
typedef enum {
  oyPATH_NON_SPECIFIC = 0x00,          /**< */
  oyPATH_ICC       = 0x01,             /**< ICC profile path */
  oyPATH_POLICY    = 0x02,             /**< policy data */
  oyPATH_MODULE    = 0x04,             /**< module data */
  oyPATH_SCRIPT    = 0x08
} oyPATH_TYPE_e;

/** @brief Select Scope for Installation or Configurations
 */
typedef enum {
  oySCOPE_ALL = 0x0,                   /**< do not limit search to one scope */
  oySCOPE_USER = 0x01,                 /**< use user data scope */
  oySCOPE_SYSTEM = 0x02,               /**< use system data scope */
  oySCOPE_OYRANOS = 0x04,              /**< use Oyranos installation data scope */
  oySCOPE_MACHINE = 0x08               /**< use machine data scope */
} oySCOPE_e;

char *       oyGetInstallPath        ( oyPATH_TYPE_e       type,
                                       oySCOPE_e           scope,
                                       oyAllocateFunc      allocFunc );
/** @} */ /* path_names */

char* oyGetPathFromProfileName         (const char* profile_name,
                                        oyAlloc_f    );

/* --- default profiles --- */

/** @brief Default Profiles
 */
typedef enum  {
  oyDEFAULT_PROFILE_START = 100,
  oyEDITING_XYZ,            /**< XYZ Editing (Workspace) Profile */
  oyEDITING_LAB,            /**< Lab Editing (Workspace) Profile */
  oyEDITING_RGB,            /**< Rgb Editing (Workspace) Profile */
  oyEDITING_CMYK,           /**< Cmyk Editing (Workspace) Profile */
  oyEDITING_GRAY,           /**< Gray Editing (Workspace) Profile */

  oyASSUMED_XYZ = 110,      /**< standard XYZ assumed source profile */
  oyASSUMED_LAB,            /**< standard Lab assumed source profile */
  oyASSUMED_RGB,            /**< standard RGB assumed source profile */
  oyASSUMED_WEB,            /**< std internet assumed source static_profile*/
  oyASSUMED_CMYK,           /**< standard Cmyk assumed source profile */
  oyASSUMED_GRAY,           /**< standard Gray assumed source Profile */

  oyPROFILE_PROOF = 120,    /**< standard proofing profile */
  oyDEFAULT_PROFILE_END     /**< just for easen Gui design */
} oyPROFILE_e;

/** @deprecated 
    provide oyDEFAULT_PROFILE for compatibility
 */
#if !defined(OY_IGNORE_DEPRECATED) || (OY_IGNORE_DEPRECATED < OYRANOS_VERSION)
  /*#warning oyDEFAULT_PROFILE is deprecated*/
# define oyDEFAULT_PROFILE oyPROFILE_e
#endif

int      oySetDefaultProfile         ( oyPROFILE_e         type,
                                       oySCOPE_e           scope,
                                       const char        * file_name );
char*       oyGetDefaultProfileName    (oyPROFILE_e       type,
                                        oyAlloc_f         alloc_func);


/* --- profile lists --- */

char **  oyProfileListGet            ( const char        * colorsig,
                                       uint32_t          * size,
                                       oyAlloc_f           allocateFunc );
char **  oyProfilePathsGet           ( int               * size,
                                       oyAlloc_f           allocateFunc );


/* --- profile checking --- */

int   oyCheckProfile                   (const char* name,
                                        const char* unused);
int   oyCheckProfileMem                (const void* mem, size_t size,
                                        const char* unused);


/* --- profile access through oyranos --- */

size_t oyGetProfileSize                (const char * profilename);
void*  oyGetProfileBlock               (const char * profilename, size_t * size,
                                        oyAlloc_f     alloc_func);


/* --- options / GUI layout --- */
/** @brief options for UI presentation, covering all kind of settings */
typedef enum  {
  oyWIDGET_GROUP_START = 0,
  oyWIDGET_GROUP_POLICY,                  /**< Policies */
  oyWIDGET_GROUP_PATHS,                   /**< Paths */
  oyWIDGET_GROUP_DEFAULT_PROFILES,        /**< Default Profiles */
  oyWIDGET_GROUP_DEFAULT_PROFILES_EDIT,   /**< Default Editing Profiles */
  oyWIDGET_GROUP_DEFAULT_PROFILES_ASSUMED,/**< Default Assumed Profiles */
  oyWIDGET_GROUP_DEFAULT_PROFILES_PROOF,  /**< Default Proofing Profiles */
  oyWIDGET_GROUP_BEHAVIOUR,               /**< Behaviour */
  oyWIDGET_GROUP_BEHAVIOUR_RENDERING,      /**< Rendering Behaviour */
  oyWIDGET_GROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, /**< PDF Generation Options*/
  oyWIDGET_GROUP_BEHAVIOUR_MISSMATCH,     /**< Profile Missmatch Behaviour */
  oyWIDGET_GROUP_BEHAVIOUR_PROOF,         /**< Profile Missmatch Behaviour */
  oyWIDGET_GROUP_CMM,                     /**< Color Matching Modules */
  oyWIDGET_GROUP_ALL,                     /**< just for easen Gui design */
  oyWIDGET_GROUP_DEVICES,                 /**< just for easen Gui design */
  oyWIDGET_GROUP_DEVICES_PROFILES_TAXI,   /**< just for easen Gui design */
  oyWIDGET_GROUP_DEVICES_PROFILES,        /**< just for easen Gui design */
  oyWIDGET_GROUP_INFORMATION,             /**< just for easen Gui design */
  oyWIDGET_GROUP_EXTERN = 200,      /**< start of groups in a extern module */

  oyWIDGET_POLICY = 20,                   /**< policy selection */
  oyWIDGET_PATHS,                         /**< path selection */

  oyWIDGET_BEHAVIOUR_START = 30,
  oyWIDGET_ACTION_UNTAGGED_ASSIGN,    /**< What to do if image is untagged ? */
  oyWIDGET_ACTION_OPEN_MISMATCH_RGB,  /**< What to do if profiles mismatch ? */
  oyWIDGET_ACTION_OPEN_MISMATCH_CMYK, /**< What to do if profiles mismatch ?*/
  oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT, /**< Convert befor save for Print? */
  oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN,/**< Convert befor save for Screen? */
  oyWIDGET_RENDERING_INTENT,          /**< Standard color transformations */
  oyWIDGET_RENDERING_BPC,             /**< use BlackPointCompensation */
  oyWIDGET_RENDERING_INTENT_PROOF,    /**< Proofing color transformations */
  oyWIDGET_PROOF_SOFT,                /**< Proofing by default for screen */
  oyWIDGET_PROOF_HARD,                /**< Proofing by default for printing */
  oyWIDGET_RENDERING_GAMUT_WARNING,   /**< mark out of gamut colors */
  oyWIDGET_BEHAVIOUR_END,             /**< just for easen Gui design */

  oyWIDGET_DEFAULT_PROFILE_START = 100,
  oyWIDGET_EDITING_XYZ,        /**< XYZ Editing (Workspace) Profile */
  oyWIDGET_EDITING_LAB,        /**< Lab Editing (Workspace) Profile */
  oyWIDGET_EDITING_RGB,        /**< Rgb Editing (Workspace) Profile */
  oyWIDGET_EDITING_CMYK,       /**< Cmyk Editing (Workspace) Profile */
  oyWIDGET_EDITING_GRAY,       /**< Gray Editing (Workspace) Profile */

  oyWIDGET_ASSUMED_XYZ=110,    /**< standard XYZ assumed source profile */
  oyWIDGET_ASSUMED_LAB,        /**< standard Lab assumed source profile */
  oyWIDGET_ASSUMED_RGB,        /**< standard RGB assumed source profile */
  oyWIDGET_ASSUMED_WEB,        /**< std internet assumed source static_profile*/
  oyWIDGET_ASSUMED_CMYK,       /**< standard Cmyk assumed source profile */
  oyWIDGET_ASSUMED_GRAY,       /**< standard Gray assumed source profile */

  oyWIDGET_PROFILE_PROOF = 120,/**< standard proofing profile */
  oyWIDGET_DEFAULT_PROFILE_END,/**< just for easen Gui design */

  oyWIDGET_DEVICES_RELATED = 200,      /**< select only device related profiles [checkbox] */
  oyWIDGET_POLICY_ACTIVE,              /**< active policy [choice] */
  oyWIDGET_TAXI_PROFILE_INSTALL,       /**< Install selected profile [button] */

  oyWIDGET_CMM_START = 300,            /**< CMM options */
  oyWIDGET_CMM_CONTEXT,                /**< CMM core selection */
  oyWIDGET_CMM_RENDERER,               /**< take a device link and color convert */
  oyWIDGET_CMM_CONTEXT_FALLBACK,       /**< CMM core fallback selection */
  oyWIDGET_CMM_RENDERER_FALLBACK,      /**< take a device link and color convert */
  oyWIDGET_CMM_END                     /**< just for easen Gui design */
} oyWIDGET_e;


/** @brief type of widget */
typedef enum {
    oyWIDGETTYPE_START,
    oyWIDGETTYPE_GROUP_TREE,/**< group creating a new visibility (tab, leave) */
    oyWIDGETTYPE_BEHAVIOUR,
    oyWIDGETTYPE_DEFAULT_PROFILE,
    oyWIDGETTYPE_PROFILE,   /**< select a profile through a list widget */
    oyWIDGETTYPE_INT,       /**< TODO @todo value range */
    oyWIDGETTYPE_FLOAT,     /**< IEEE floating point number */
    oyWIDGETTYPE_CHOICE,    /**< option from a array of choices */
    oyWIDGETTYPE_LIST,      /**< list widget, like for paths */
    oyWIDGETTYPE_VOID,      /**< data block, should not be selectable */
    oyWIDGETTYPE_END
} oyWIDGET_TYPE_e;

/** @brief layout flags for widgets */
#define OY_LAYOUT_NO_CHOICES    0x01  /**< show informational */
#define OY_LAYOUT_MIDDLE        0x02  /**< Arrange in the middle. */
#define OY_LAYOUT_PATH_SELECTOR 0x04  /**< add a path selector to each entry */ 

oyWIDGET_e  * oyWidgetListGet          (oyGROUP_e         group,
                                        int             * count,
                                        oyAlloc_f         allocate_func );

oyWIDGET_TYPE_e oyWidgetTitleGet       (oyWIDGET_e        option,
                                        const oyGROUP_e** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags );
oyWIDGET_TYPE_e  oyWidgetDescriptionGet (
                                       oyWIDGET_e          option,
                                       const char       ** description,
                                       int                 choice );

int           oyOptionChoicesGet     ( oyWIDGET_e          option,
                                       int               * choices,
                                       const char      *** choices_string_list,
                                       int               * current);
uint32_t oyICCProfileSelectionFlagsFromRegistration (
                                       const char        * registration );
int           oyOptionChoicesGet2    ( oyWIDGET_e          option,
                                       uint32_t            flags,
                                       int                 name_type,
                                       int               * choices,
                                       const char      *** choices_string_list,
                                       int               * current );
void          oyOptionChoicesFree      (oyWIDGET_e        option,
                                        char          *** list,
                                        int               size);
# ifdef oyInPlaning_
int           oyOptionFloatIntRangeGet (oyWIDGET_e        option,
                                        float           * start,
                                        float           * end,
                                        float           * step_major,
                                        float           * step_minor,
                                        float           * current);
# endif



int    oyProfileGetMD5               ( void       *buffer,
                                       size_t      size,
                                       unsigned char *md5_return );




char *   oyGetDisplayNameFromPosition2(const char        * device_type,
                                       const char        * device_class,
                                       const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAlloc_f           allocateFunc );
/* deprecated monitor functions */
char *   oyGetDisplayNameFromPosition( const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAlloc_f           allocate_func );
int      oyGetScreenFromPosition     ( const char        * display_name,
                                       int                 x,
                                       int                 y );
char *   oyGetMonitorProfile         ( const char        * display,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );
char *   oyGetMonitorProfileNameFromDB(const char        * display,
                                       oyAlloc_f           allocate_func );
int      oySetMonitorProfile         ( const char        * display_name,
                                       const char        * profil_name );
int      oyActivateMonitorProfiles   ( const char        * display_name );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_H */
