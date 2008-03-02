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


/* --- behaviour / policies --- */

/** enum Default Profiles
 */
typedef enum  {
  oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN,  /**< What to do if image is untagged ? */
  oyBEHAVIOUR_ACTION_OPEN_MISMATCH_RGB,/**< What to do if profiles mismatch ? */
  oyBEHAVIOUR_ACTION_OPEN_MISMATCH_CMYK,/**< What to do if profiles mismatch ?*/
  oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT,/**< Convert befor save for Print? */
  oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN,/**< Convert befor save for Screen? */
  oyBEHAVIOUR_RENDERING_INTENT,        /**< Standard colour transformations */
  oyBEHAVIOUR_RENDERING_BPC,           /**< use BlackPointCompensation */
  oyBEHAVIOUR_RENDERING_INTENT_PROOF,  /**< Proofing colour transformations */
  oyBEHAVIOUR_NUMS                     /**< just for easen Gui design */
} oyBEHAVIOUR;

enum  {
  oyNO,                                /**< dont do it */
  oyYES,                               /**< automaticaly perform action */
  oyASK,                               /**< popup dialog */
}; /**< for oyBEHAVIOUR_ACTION */

const char* oyGetBehaviourUITitle      (oyBEHAVIOUR       type,
                                        int              *choices,
                                        int               choice,
                                        const char      **category,
                                        const char      **option_string,
                                        const char      **tooltip);
int         oyGetBehaviour             (oyBEHAVIOUR       type);
int         oySetBehaviour             (oyBEHAVIOUR       type,
                                        int               choice);

/** enum Policy Groups 
 */
typedef enum  {
  oyGROUP_DEFAULT_PROFILES,   /**< Default Profiles */
  oyGROUP_RENDERING,          /**< Rendering Behaviour */
  oyGROUP_MIXED_MODE_DOCUMENTS, /**< PDF Generation Options */
  oyGROUP_MISSMATCH,          /**< Profile Missmatch Behaviour */
  oyGROUP_ALL                 /**< just for easen Gui design */
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
  oyEDITING_RGB,            /**< Rgb Editing (Workspace) Profile */
  oyEDITING_CMYK,           /**< Cmyk Editing (Workspace) Profile */
  oyEDITING_XYZ,            /**< XYZ Editing (Workspace) Profile */
  oyEDITING_LAB,            /**< Lab Editing (Workspace) Profile */
  oyASSUMED_XYZ,            /**< standard XYZ assumed source profile */
  oyASSUMED_LAB,            /**< standard Lab assumed source profile */
  oyASSUMED_RGB,            /**< standard RGB assumed source profile */
  oyASSUMED_WEB,            /**< std internet assumed source static_profile*/
  oyASSUMED_CMYK,           /**< standard Cmyk assumed source profile */
  oyDEFAULT_PROFILE_NUMS    /**< just for easen Gui design */
} oyDEFAULT_PROFILE;

int         oySetDefaultProfile        (oyDEFAULT_PROFILE type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock   (oyDEFAULT_PROFILE type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);
const char* oyGetDefaultProfileUITitle (oyDEFAULT_PROFILE type);
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


#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_H */
