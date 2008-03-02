/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
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
  oyGROUP_EXTERN = 100             /**< start of groups in a extern module */
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
  oyWIDGET_GROUP_BEHAVIOUR_RENDERING,            /**< Rendering Behaviour */
  oyWIDGET_GROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, /**< PDF Generation Options*/
  oyWIDGET_GROUP_BEHAVIOUR_MISSMATCH,     /**< Profile Missmatch Behaviour */
  oyWIDGET_GROUP_BEHAVIOUR_PROOF,         /**< Profile Missmatch Behaviour */
  oyWIDGET_GROUP_ALL,                     /**< just for easen Gui design */
  oyWIDGET_GROUP_EXTERN = 100,      /**< start of groups in a extern module */

  oyWIDGET_POLICY = 20,        /**< policy selection */

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
  oyWIDGET_EDITING_RGB,        /**< Rgb Editing (Workspace) Profile */
  oyWIDGET_EDITING_CMYK,       /**< Cmyk Editing (Workspace) Profile */
  oyWIDGET_EDITING_XYZ,        /**< XYZ Editing (Workspace) Profile */
  oyWIDGET_EDITING_LAB,        /**< Lab Editing (Workspace) Profile */
  oyWIDGET_ASSUMED_XYZ,        /**< standard XYZ assumed source profile */
  oyWIDGET_ASSUMED_LAB,        /**< standard Lab assumed source profile */
  oyWIDGET_ASSUMED_RGB,        /**< standard RGB assumed source profile */
  oyWIDGET_ASSUMED_WEB,        /**< std internet assumed source static_profile*/
  oyWIDGET_ASSUMED_CMYK,       /**< standard Cmyk assumed source profile */
  oyWIDGET_PROFILE_PROOF,      /**< standard proofing profile */
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
    oyTYPE_VOID,      /**!< data block, should not be selectable */
    oyTYPE_END
} oyWIDGET_TYPE;

oyWIDGET    * oyWidgetListGet          (oyGROUP           group,
                                        int             * count);

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
                                        const char    *** list,
                                        int               size);
# ifdef oyInPlaning_
int           oyOptionFloatIntRangeGet (oyWIDGET          option,
                                        float           * start,
                                        float           * end,
                                        float           * step_major,
                                        float           * step_minor,
                                        float           * current);
# endif

/* --- colour conversions --- */

/** @brief Option for rendering

    should be used in a list oyColourTransformOptions_s to form a options set
 */
typedef struct {
    oyWIDGET opt;                     /*!< CMM registred option */
    int    supported_by_chain;        /*!< 1 for supporting; 0 if one fails */
    double value_d;                   /*!< value of option; unset with nan; */ 
} oyOption_s;

/** @brief Options for rendering

    Options can be any flag or rendering intent and other informations needed to
    configure a process. It contains variables for colour transforms.
 */
typedef struct {
    int n;                            /*!< number of options */
    oyOption_s* opts;
} oyOptions_s;

/** @brief a profile and its attributes
 */
typedef struct {
    size_t size;                      /*!< ICC profile size */
    void *block;                      /*!< ICC profile data */
    oyDEFAULT_PROFILE use_default;    /*!< if > 0 : take from settings */
} oyProfile_s;

/** @brief tell about the conversion profiles
 */
typedef struct {
    int            n;                 /*!< number of profiles */
    oyProfile_s   *profiles;
} oyProfileList_s;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} oyRegion_s;

typedef enum {
    oyUINT8,     /*!<  8-bit integer */
    oyUINT16,    /*!< 16-bit integer */
    oyUINT32,    /*!< 32-bit integer */
    oyHALF,      /*!< 16-bit floating point number */
    oyFLOAT,     /*!< IEEE floating point number */
    oyDOUBLE     /*!< IEEE double precission floating point number */
} oyDATATYPE;

/** @brief a reference struct to gather information for image transformation

    as we dont target a complete imaging solution, only raster is supported

    oyImage_s should hold image dimensions,
    oyDisplayRegion_s information and
    a reference to the data for conversion

    As well referencing of itself would be nice.

    Should oyImage_s become internal and we provide a user interface?
 */
typedef struct {
    int          width;       /*!< data width */
    int          height;      /*!< data height */
    void        *data;        /*!< image data */
    oyDATATYPE   type;        /*!< data type */
    int          planar;      /*!< RRRGGGBBB vs RGBRGBRGB */
    oyProfile_s *profile;     /*!< image profile */
    oyRegion_s  *region;      /*!< region to render, if zero render all */
    int          screen_pos_x;/*!< upper position on screen of image */
    int          screen_pos_y;/*!< left position on screen of image */
} oyImage_s;

/** @brief clean all memory including depending structs */
int            oyImageCleanAll       ( oyImage_s *img, oyDeAllocFunc_t free );

typedef struct {
    /*int          whatch;*/      /*!< tell Oyranos to observe files */
    void*        internal;    /*!< Oyranos internal structs */
} oyColourConversion_s;

/** allocate n oyOption_s */
oyOptions_s*   oyOptionsCreate       ( int n );
/** allocate oyOption_s for a 4 char CMM identifier obtained by oyCmmGetCmms */
oyOptions_s*   oyOptionsCreateFor    ( const char *cmm );

/** free oyOption_s from the list */
void           oyOptionsFree         ( oyOptions_s *opts, oyDeAllocFunc_t free);

/** confirm if all is ok */
int            oyOptionsVerifyForCMM ( oyOptions_s *opts, char* cmm );

/** create and possibly precalculate a transform */
oyColourConversion_s* oyColourConversionCreate ( char* cmm, /*!< zero or a cmm*/
                                  oyProfileList_s *list,/*!< multi profiles */
                                  oyOptions_s *opts,   /*!< conversion opts */
                                  oyImage_s *in,       /*!< input */
                                  oyImage_s *out       /*!< zero or output */
                                  );                   /*!< return: conversion*/
int            oyColourConversionRun ( oyColourConversion_s *colour /*!< object*/
                                     );                  /*!< return: error */


/* --- CMM API --- */

int    oyModulRegisterXML            ( oyGROUP group,
                                       const char *xml );

/** obtain 4 char CMM identifiers and count of CMM's */
char** oyModulsGetNames              ( int        *count,
                                       oyAllocFunc_t alloc_func );
int    oyModulGetOptionRanges        ( const char *cmm,
                                       oyGROUP    *oy_group_start,
                                       oyGROUP    *oy_group_end,
                                       oyWIDGET   *oy_option_start,
                                       oyWIDGET   *oy_option_end );


void   oyI18NSet                     ( int active,
                                       int reserved );

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_H */
