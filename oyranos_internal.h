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
 */

/** @file @internal
 *  @brief misc internal API
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_INTERNAL_H
#define OYRANOS_INTERNAL_H

#include "config.h"
#include "oyranos.h"
#include "oyranos_i18n.h"
#include "oyranos_texts.h"
#include "oyranos_elektra.h"
#include "oyranos_version.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


/** @internal
    @brief used by sort engine */
struct oyComp_s_ {
  struct oyComp_s_ *next;   /* chain connection */
  struct oyComp_s_ *begin;  /* chain connection */
  char            *name;   /* key name */
  char            *val;    /* its value */
  int              hits;   /* weighting */
};

typedef struct oyComp_s_ oyComp_t_;

/**@internal A small search engine
 *
 * for one simple, single list, dont mix lists!!
 * name and val are not alloced or freed 
 */


oyComp_t_* oyInitComp_      (oyComp_t_ *compare, oyComp_t_ *top);
oyComp_t_* oyAppendComp_    (oyComp_t_ *list,    oyComp_t_ *new_);
void    oySetComp_         (oyComp_t_ *compare, const char* keyName,
                            const char* value, int hits );
void    oyDestroyCompList_ (oyComp_t_* list);
char*   printComp          (oyComp_t_* entry);


int     oySetProfile_Block                (const char* name, 
                                           void* mem,
                                           size_t size,
                                           oyDEFAULT_PROFILE type,
                                           const char* comnt);


/* separate from the external functions */
int   oyPathsCount_             (void);
char* oyPathName_               (int           number,
                                 oyAllocFunc_t allocate_func);
int   oyPathAdd_                (const char* pathname);
void  oyPathRemove_             (const char* pathname);
void  oyPathSleep_              (const char* pathname);
void  oyPathActivate_           (const char* pathname);
char* oyGetPathFromProfileName_ (const char*   profilename,
                                 oyAllocFunc_t allocate_func);



#ifdef oyInPlaning_
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

#endif /* oyInPlaning_ */



/* device profiles */
/** \internal enum identifying device types for distinguishing in searches */
typedef enum  {
  /*oyNOTYPE,*/
  oyDISPLAY,          /**< dynamic viewing */
  oyPRINTER,          /**< static media (dye, ink, offset, imagesetters) */
  oySCANNER,          /**< contact digitiser */
  oyCAMERA            /**< virtual or contactless image capturing */
} oyDEVICETYP;

#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_END

char* oyGetDeviceProfile                  (oyDEVICETYP typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           oyAllocFunc_t);

int	oySetDeviceProfile                    (oyDEVICETYP typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           const char* profilename,
                                           const void* mem,
                                           size_t size);
int oyEraseDeviceProfile                  (oyDEVICETYP typ,
                                           const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3);

typedef enum {
  oyDISPLAY_T
} oyOBJECT_TYPE;

#ifdef __APPLE__
#include <Carbon/Carbon.h>

int    oyGetProfileBlockOSX              ( CMProfileRef prof,
                                           char **block, size_t *size,
                                           oyAllocFunc_t allocate_func);
char * oyGetProfileNameOSX               ( CMProfileRef prof,
                                           oyAllocFunc_t allocate_func);
#endif

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_INTERNAL_H */
