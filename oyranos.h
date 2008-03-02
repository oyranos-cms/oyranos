/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2005  Kai-Uwe Behrmann
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


/* --- behaviour --- */

/** enum Behaviour Options
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
  oyASK,                               /**< popup dialog */
}; /**< for oyBEHAVIOUR_ACTION */

int         oyGetOption                (oyBEHAVIOUR       type);
int         oySetOption                (oyBEHAVIOUR       type,
                                        int               choice);

/* --- policies --- */

/** enum Policy Groups 
 */
typedef enum  {
  oyGROUP_START = 0,
  oyGROUP_DEFAULT_PROFILES,        /**< Default Profiles */
  oyGROUP_DEFAULT_PROFILES_EDIT,   /**< Default Editing Profiles */
  oyGROUP_DEFAULT_PROFILES_ASSUMED,/**< Default Assumed Profiles */
  oyGROUP_DEFAULT_PROFILES_PROOF,  /**< Default Proofing Profiles */
  oyGROUP_PATHS,                   /**< Paths */
  oyGROUP_BEHAVIOUR,               /**< Behaviour */
  oyGROUP_BEHAVIOUR_RENDERING,            /**< Rendering Behaviour */
  oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, /**< PDF Generation Options */
  oyGROUP_BEHAVIOUR_MISSMATCH,            /**< Profile Missmatch Behaviour */
  oyGROUP_BEHAVIOUR_PROOF,                /**< Profile Missmatch Behaviour */
  oyGROUP_ALL                      /**< just for easen Gui design */
} oyGROUP;

char*       oyPolicyToXML              (oyGROUP           group,
                                        int               add_header,
                                        oyAllocFunc_t     alloc_func);

int         oyReadXMLPolicy            (oyGROUP           group,
                                        const char       *xml);

/*
 * @param[in] data the pointer to deallocate
 *
 * Place here your deallocator, like: \code delete [] data; \endcode
 */
//typedef void (*oyDeAllocFunc_t)        (void *data);


/* path names */

int   oyPathsCount                     (void);
char* oyPathName                       (int         number,
                                        oyAllocFunc_t);
char* oyGetPathFromProfileName         (const char* profile_name,
                                        oyAllocFunc_t);

/* --- default profiles --- */

/** enum Default Profiles
 */
typedef enum  {
  oyDEFAULT_PROFILE_START = 100,
  oyEDITING_RGB,            /**< Rgb Editing (Workspace) Profile */
  oyEDITING_CMYK,           /**< Cmyk Editing (Workspace) Profile */
  oyEDITING_XYZ,            /**< XYZ Editing (Workspace) Profile */
  oyEDITING_LAB,            /**< Lab Editing (Workspace) Profile */
  oyASSUMED_XYZ,            /**< standard XYZ assumed source profile */
  oyASSUMED_LAB,            /**< standard Lab assumed source profile */
  oyASSUMED_RGB,            /**< standard RGB assumed source profile */
  oyASSUMED_WEB,            /**< std internet assumed source static_profile*/
  oyASSUMED_CMYK,           /**< standard Cmyk assumed source profile */
  oyPROFILE_PROOF,          /**< standard proofing profile */
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

char**oyProfileList                    (const char* coloursig, int * size);
void  oyProfileListFree                (char** list, int size);

/* --- profile checking --- */

int   oyCheckProfile                   (const char* name,
                                        const char* coloursig);
int   oyCheckProfileMem                (const void* mem, size_t size,
                                        const char* coloursig);


/* --- profile access through oyranos --- */

size_t oyGetProfileSize                (const char* profilename);
void*  oyGetProfileBlock               (const char* profilename, size_t* size,
                                        oyAllocFunc_t);


/* --- options / GUI layout --- */

typedef enum  {
  oyOPTION_BEHAVIOUR_START = 30,
  oyOPTION_ACTION_UNTAGGED_ASSIGN,  /**< What to do if image is untagged ? */
  oyOPTION_ACTION_OPEN_MISMATCH_RGB,/**< What to do if profiles mismatch ? */
  oyOPTION_ACTION_OPEN_MISMATCH_CMYK,/**< What to do if profiles mismatch ?*/
  oyOPTION_MIXED_MOD_DOCUMENTS_PRINT,/**< Convert befor save for Print? */
  oyOPTION_MIXED_MOD_DOCUMENTS_SCREEN,/**< Convert befor save for Screen? */
  oyOPTION_RENDERING_INTENT,        /**< Standard colour transformations */
  oyOPTION_RENDERING_BPC,           /**< use BlackPointCompensation */
  oyOPTION_RENDERING_INTENT_PROOF,  /**< Proofing colour transformations */
  oyOPTION_PROOF_SOFT,              /**< Proofing by default for screen */
  oyOPTION_PROOF_HARD,              /**< Proofing by default for printing */
  oyOPTION_BEHAVIOUR_END,           /**< just for easen Gui design */

  oyOPTION_DEFAULT_PROFILE_START = 100,
  oyOPTION_EDITING_RGB,            /**< Rgb Editing (Workspace) Profile */
  oyOPTION_EDITING_CMYK,           /**< Cmyk Editing (Workspace) Profile */
  oyOPTION_EDITING_XYZ,            /**< XYZ Editing (Workspace) Profile */
  oyOPTION_EDITING_LAB,            /**< Lab Editing (Workspace) Profile */
  oyOPTION_ASSUMED_XYZ,            /**< standard XYZ assumed source profile */
  oyOPTION_ASSUMED_LAB,            /**< standard Lab assumed source profile */
  oyOPTION_ASSUMED_RGB,            /**< standard RGB assumed source profile */
  oyOPTION_ASSUMED_WEB,            /**< std internet assumed source static_profile*/
  oyOPTION_ASSUMED_CMYK,           /**< standard Cmyk assumed source profile */
  oyOPTION_PROFILE_PROOF,          /**< standard proofing profile */
  oyOPTION_DEFAULT_PROFILE_END     /**< just for easen Gui design */
} oyOPTION;

const char* oyGetOptionUITitles        (oyOPTION          type,
                                        oyGROUP         **categories,
                                        const char     ***category_string_list,
                                        int              *choices,
                                        const char     ***choices_string_list,
                                        const char      **tooltip );
const char* oyGetDefaultProfileUITitle (oyDEFAULT_PROFILE type);


#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_H */
