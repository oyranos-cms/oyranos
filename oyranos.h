/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2007  Kai-Uwe Behrmann
 *
 * @autor: Kai-Uwe Behrmann <ku.b@gmx.de>
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

/** @date      25. 11. 2004 */


#ifndef OYRANOS_H
#define OYRANOS_H

#include <stdlib.h> /* for linux size_t */
#include "oyranos_definitions.h"
#include "oyranos_version.h"

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/**
 * @param[in] size the size to allocate
 *
 * Place here your allocator, like: \code
   void* myAllocFunc (size_t size)
   { return malloc (size);
   }
 * \endcode<br>
 */
typedef void* (*oyAllocFunc_t)         (size_t size);

/**
 * @param[in] data the pointer to free
 *
 * Place here your allocator, like: \code
   void* myDeAllocFunc (void *data)
   { return free (data);
   }
 * \endcode<br>
 */
typedef void (*oyDeAllocFunc_t)       (void *data);


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
} oyGROUP;


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
  oyBEHAVIOUR_RENDERING_INTENT,        /**< Standard colour transformations */
  oyBEHAVIOUR_RENDERING_BPC,           /**< use BlackPointCompensation */
  oyBEHAVIOUR_RENDERING_INTENT_PROOF,  /**< Proofing colour transformations */
  oyBEHAVIOUR_PROOF_SOFT,              /**< Proofing by default for screen */
  oyBEHAVIOUR_PROOF_HARD,              /**< Proofing by default for printing */
  oyBEHAVIOUR_END                      /**< just for easen Gui design */
} oyBEHAVIOUR;

enum  {
  oyNO,                                /**< dont do it */
  oyYES,                               /**< automaticaly perform action */
  oyASK                                /**< popup dialog */
}; /**< for oyBEHAVIOUR_ACTION */

int         oyGetBehaviour             (oyBEHAVIOUR       type);
int         oySetBehaviour             (oyBEHAVIOUR       type,
                                        int               choice);

/* --- policies --- */

char*       oyPolicyToXML              (oyGROUP           group,
                                        int               add_header,
                                        oyAllocFunc_t     alloc_func);
int         oyReadXMLPolicy            (oyGROUP           group,
                                        const char       *xml);
int         oyPolicySet                (const char      * policy,
                                        const char      * full_name );


/* path names */

int   oyPathsCount                     (void);
char* oyPathName                       (int         number,
                                        oyAllocFunc_t);
char* oyGetPathFromProfileName         (const char* profile_name,
                                        oyAllocFunc_t);

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
} oyDEFAULT_PROFILE;

int         oySetDefaultProfile        (oyDEFAULT_PROFILE type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock   (oyDEFAULT_PROFILE type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);
char*       oyGetDefaultProfileName    (oyDEFAULT_PROFILE type,
                                        oyAllocFunc_t     alloc_func);


/* --- profile lists --- */

char**oyProfileListGet                 (const char* coloursig, int * size);


/* --- profile checking --- */

int   oyCheckProfile                   (const char* name,
                                        const char* coloursig);
int   oyCheckProfileMem                (const void* mem, size_t size,
                                        const char* coloursig);


/* --- profile access through oyranos --- */

size_t oyGetProfileSize                (const char * profilename);
void*  oyGetProfileBlock               (const char * profilename, size_t * size,
                                        oyAllocFunc_t alloc_func);


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
  oyWIDGET_GROUP_EXTERN = 200,      /**< start of groups in a extern module */

  oyWIDGET_POLICY = 20,                   /**< policy selection */
  oyWIDGET_PATHS,                         /**< path selection */

  oyWIDGET_BEHAVIOUR_START = 30,
  oyWIDGET_ACTION_UNTAGGED_ASSIGN,    /**< What to do if image is untagged ? */
  oyWIDGET_ACTION_OPEN_MISMATCH_RGB,  /**< What to do if profiles mismatch ? */
  oyWIDGET_ACTION_OPEN_MISMATCH_CMYK, /**< What to do if profiles mismatch ?*/
  oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT, /**< Convert befor save for Print? */
  oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN,/**< Convert befor save for Screen? */
  oyWIDGET_RENDERING_INTENT,          /**< Standard colour transformations */
  oyWIDGET_RENDERING_BPC,             /**< use BlackPointCompensation */
  oyWIDGET_RENDERING_INTENT_PROOF,    /**< Proofing colour transformations */
  oyWIDGET_PROOF_SOFT,                /**< Proofing by default for screen */
  oyWIDGET_PROOF_HARD,                /**< Proofing by default for printing */
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
  oyWIDGET_CMM_INTENT_PROOF        /**< Proofing colour transformations */
} oyWIDGET;


/** @brief type of widget */
typedef enum {
    oyTYPE_START,
    oyTYPE_GROUP_TREE,/**!< group creating a new visibility (tab, leave) */
    oyTYPE_BEHAVIOUR,
    oyTYPE_DEFAULT_PROFILE,
    oyTYPE_PROFILE,   /**!< select a profile through a list widget */
    oyTYPE_INT,       /**!< TODO @todo value range */
    oyTYPE_FLOAT,     /**!< IEEE floating point number */
    oyTYPE_CHOICE,    /**!< option from a array of choices */
    oyTYPE_LIST,      /**!< list widget, like for paths */
    oyTYPE_VOID,      /**!< data block, should not be selectable */
    oyTYPE_END
} oyWIDGET_TYPE;

/** @brief layout flags for widgets */
#define oyLAYOUT_NO_CHOICES    0x01  /**!< show informational */
#define oyLAYOUT_MIDDLE        0x02  /**!< Arrange in the middle. */
#define oyLAYOUT_PATH_SELECTOR 0x04  /**!< add a path selector tp each entry */ 

oyWIDGET    * oyWidgetListGet          (oyGROUP           group,
                                        int             * count,
                                        oyAllocFunc_t     allocate_func );

oyWIDGET_TYPE oyWidgetTitleGet         (oyWIDGET          option,
                                        const oyGROUP  ** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags );

int           oyOptionChoicesGet       (oyWIDGET          option,
                                        int             * choices,
                                        const char    *** choices_string_list,
                                        int             * current);
void          oyOptionChoicesFree      (oyWIDGET_TYPE     option,
                                        char          *** list,
                                        int               size);
# ifdef oyInPlaning_
int           oyOptionFloatIntRangeGet (oyWIDGET          option,
                                        float           * start,
                                        float           * end,
                                        float           * step_major,
                                        float           * step_minor,
                                        float           * current);
# endif


int oyVersion( int type ); /**!< itype is for further extension */ 

int    oyProfileGetMD5               ( void       *buffer,
                                       size_t      size,
                                       char       *md5_return );

void   oyI18NSet                     ( int active,
                                       int reserved );

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_H */
