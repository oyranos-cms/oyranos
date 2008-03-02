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

#include <stdlib.h> // for linux size_t
#include "oyranos_definitions.h"

/** \namespace oyranos
    The Oyranos namespace.
 */
#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/** * kdb stuff */
void  oyOpen                (void);
void  oyClose               (void);

/**
 * @param[in] size the size to allocate
 *
 * Place here your allocator, like: \code new char [size]; \endcode<br>
 */
typedef void (*oyAllocFunc_t)          (size_t size);

/**
 * @param[in] data the pointer to deallocate
 *
 * Place here your deallocator, like: \code delete [] data; \endcode
 */
//typedef void (*oyDeAllocFunc_t)        (void *data);


/* path names */

int   oyPathsCount                     (void);
char* oyPathName                       (int         number,
                                        oyAllocFunc_t);
int   oyPathAdd                        (const char* pathname);
void  oyPathRemove                     (const char* pathname);
void  oyPathSleep                      (const char* pathname);
void  oyPathActivate                   (const char* pathname);
/** \return allocated by oyAllocFunc_t */
char* oyGetPathFromProfileName         (const char* profile_name,
                                        oyAllocFunc_t);

/* --- default profiles --- */

int   oySetDefaultWorkspaceProfile     (const char* name);
int   oySetDefaultWorkspaceProfileBlock(const char* name, void* mem, size_t size);
int   oySetDefaultXYZInputProfile      (const char* name);
int   oySetDefaultXYZInputProfileBlock (const char* name, void* mem, size_t size);
int   oySetDefaultLabInputProfile      (const char* name);
int   oySetDefaultLabInputProfileBlock (const char* name, void* mem, size_t size);
int   oySetDefaultRGBInputProfile      (const char* name);
int   oySetDefaultRGBInputProfileBlock (const char* name, void* mem, size_t size);
int   oySetDefaultCmykInputProfile     (const char* name);
int   oySetDefaultCmykInputProfileBlock(const char* name, void* mem, size_t size);

/** \return allocated by oyAllocFunc_t */
char* oyGetDefaultWorkspaceProfileName (oyAllocFunc_t);
/** \return allocated by oyAllocFunc_t */
char* oyGetDefaultXYZInputProfileName  (oyAllocFunc_t);
/** \return allocated by oyAllocFunc_t */
char* oyGetDefaultLabInputProfileName  (oyAllocFunc_t);
/** \return allocated by oyAllocFunc_t */
char* oyGetDefaultRGBInputProfileName  (oyAllocFunc_t);
/** \return allocated by oyAllocFunc_t */
char* oyGetDefaultCmykInputProfileName (oyAllocFunc_t);

/** \addtogroup ProfileLists
 *  Functions to handle profile name lists. Profile name lists are created
 *  recursively though all valid paths. @see PathNames
 *  @{
 */

char**oyProfileList                    (const char* colourspace, size_t * size);

/** @} */

/** --- profile checking --- */

int   oyCheckProfile                   (const char* name, int flag);
int   oyCheckProfileMem                (const void* mem, size_t size, int flags);


/** --- profile access through oyranos --- */

/** @brief obtain an memory block in the responsibility of the user */
size_t oyGetProfileSize                (const char* profilename);
/** @brief obtain an memory block in the responsibility of the user
    \return allocated by oyAllocFunc_t */
void*  oyGetProfileBlock               (const char* profilename, size_t* size,
                                        oyAllocFunc_t);


/* device profiles */

/**
 * There different approaches to select an (mostly) fitting profile
 *
 * A: search and compare all available profiles by 
 *    - ICC profile class
 *    - Manufacturer / Model (as written in profile tags)
 *    - other hints
 * B: install an profile and tell oyranos about the belonging device and the
 *    invalidating period
 * C: look for similarities of devices of allready installed profiles
 *
 */

/** enum identifying device types for distinguishing in searches */
typedef enum  {
  /*oyNOTYPE,*/
  oyDISPLAY,          /**< dynamic viewing */
  oyPRINTER,          /**< static media (dye, ink, offset, imagesetters) */
  oySCANNER,          /**< contact digitiser */
  oyCAMERA,           /**< virtual or contactless image capturing */
} oyDEVICETYP;


/** @brief ask for a profile name by specifying device attributes
 *
 *  @param typ            kind of device
 *  @param manufacturer   the device manufacturer (EIZO)
 *  @param model          the model (LCD2100)
 *  @param product_id     the ID reported during connection (ID_701200xx)
 *  @param host           useful for monitor identification (grafic:0.0)
 *  @param port           kind of connection (Matrox G650)
 *  @param attrib1        additional attribute
 * 
 *  simply pass 0 for not specified properties<br>

   \code
   char* profile_name = oyGetDeviceProfile ("EIZO", "LCD2100",
                                            "ID 87-135.19",
                                            "grafic:0.0", "Matrox G650",
                                             "100lux", 0,
                                            "");
   if (profile_name)
   { char* ptr = (char*)malloc (oyGetProfileSize (profile_name),sizeof(int);
     ptr = oyGetProfileBlock (profile_name);
       // do something
     free (ptr);
   }
   \endcode

   \return allocated by oyAllocFunc_t
 */
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
                                           void* mem,
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
#if 0
int	oySetProfileProperty                  (char* profilename,
                                           char* property,
                                           char* value);
#endif


#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_H */
