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

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
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
int         oySetBehaviour             (oyBEHAVIOUR_e     type,
                                        int               choice);

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


/* path names */

typedef enum {
  oyPATH_NON_SPECIFIC = 0x00,
  oyPATH_ICC       = 0x01,
  oyPATH_POLICY    = 0x02,
  oyPATH_MODULE    = 0x04,
  oyPATH_SCRIPT    = 0x08
} oyPATH_TYPE_e;

#if 0
int   oyPathsCount                     (void);
char* oyPathName                       (int         number,
                                        oyAlloc_f    );
#endif
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

int         oySetDefaultProfile        (oyPROFILE_e       type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock   (oyPROFILE_e       type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);
char*       oyGetDefaultProfileName    (oyPROFILE_e       type,
                                        oyAlloc_f         alloc_func);


/* --- profile lists --- */

char **  oyProfileListGet            ( const char        * colorsig,
                                       uint32_t          * size,
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
  oyWIDGET_GROUP_ALL,                     /**< just for easen Gui design */
  oyWIDGET_GROUP_DEVICES,                 /**< just for easen Gui design */
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

  oyWIDGET_CMM_START = 300,        /**< CMM options */
  oyWIDGET_CMM_SELECT,             /**< CMM selection */
  oyWIDGET_CMM_INTENT,             /**< CMM rendering intent */
  oyWIDGET_CMM_BPC,                /**< black point compensation switch */
  oyWIDGET_CMM_INTENT_PROOF        /**< Proofing color transformations */
} oyWIDGET_e;


/** @brief type of widget */
typedef enum {
    oyWIDGETTYPE_START,
    oyWIDGETTYPE_GROUP_TREE,/**!< group creating a new visibility (tab, leave) */
    oyWIDGETTYPE_BEHAVIOUR,
    oyWIDGETTYPE_DEFAULT_PROFILE,
    oyWIDGETTYPE_PROFILE,   /**!< select a profile through a list widget */
    oyWIDGETTYPE_INT,       /**!< TODO @todo value range */
    oyWIDGETTYPE_FLOAT,     /**!< IEEE floating point number */
    oyWIDGETTYPE_CHOICE,    /**!< option from a array of choices */
    oyWIDGETTYPE_LIST,      /**!< list widget, like for paths */
    oyWIDGETTYPE_VOID,      /**!< data block, should not be selectable */
    oyWIDGETTYPE_END
} oyWIDGET_TYPE_e;

/** @brief layout flags for widgets */
#define OY_LAYOUT_NO_CHOICES    0x01  /**!< show informational */
#define OY_LAYOUT_MIDDLE        0x02  /**!< Arrange in the middle. */
#define OY_LAYOUT_PATH_SELECTOR 0x04  /**!< add a path selector tp each entry */ 

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

int           oyOptionChoicesGet       (oyWIDGET_e        option,
                                        int             * choices,
                                        const char    *** choices_string_list,
                                        int             * current);
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
