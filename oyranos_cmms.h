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
 * internal CMM API
 * 
 */

/** @date      29. 07. 2006 */


#ifndef OYRANOS_CMMS_H
#define OYRANOS_CMMS_H

#include "oyranos_internal.h"
#include "oyranos.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


/** @brief the internal only used structure for external registred CMM functions
 */
typedef struct {
  char       *id;               /**< usually a 4 letter short name */
  char       *libname;          /**< library to search for function */
  char       *funcname;         /**< function for dlsym */
  oyOPTION    opts_start;       /**< options numbers for oyGetOptionUITitle */
  oyOPTION    opts_end;
  oyOption_t *options;          /**< the CMM options */
} oyExternFunc_t;


/** @brief the internal only used structure for external registred CMM's
 */
typedef struct {
  char  id[5];                  /**< 4 letter identifier */
  char *name;                   /**< short name */
  char *description;            /**< long description */ // TODO help license ..
  int   groups_start;
  int   groups_end;             /**< the registred layouts frames */
  oyExternFunc_t *func;         /**< the registred functions of the CMM */
  int   funcs_n;                /**< number of provided functions */
  char ***oy_groups;            /**< the oy_groups_description_ synonym */
  char *xml;                    /**< original xml text */
  const char *domain;           /**< textdomain */
  const char *domain_path;      /**< textdomain path */
} oyCMM_t;

/* singleton */
typedef struct {
  int      looked;
  oyCMM_t *cmms;
  int      n;
} oyCMM_t__;

extern oyCMM_t__ oyCMM_;

/* internal CMM API */
oyCMM_t* oyCmmGet_              (const char *id);
int   oyCmmRemove_              (const char *id);
int   oyCmmAdd_                 (oyCMM_t *cmm);
char** oyCmmGetCmmNames_        (int           *count,
                                 oyAllocFunc_t alloc_func );
oyGROUP oyRegisterGroups_       (char *cmm, char **desc);
int   oyCmmGetFromXML_          (oyGROUP           group,
                                 const char       *xml,
                                 const char       *domain,
                                 const char       *domain_path,
                                 oyCMM_t          *cmm);
int   oyCmmRegisterXML_         (oyGROUP           group,
                                 const char       *xml,
                                 const char       *domain,
                                 const char       *domain_path);


/*
 *  Sollen die oyCMSTransform_t struct opak werden?
 *  -> Typpruefung moeglich?
 *
 *  Objektartig mit begleitender Referenz auf CMM Status?
 *
 *  Wie koennen Geraete- und Bildinformationen gleich mit einfliesen?
 *
 *  Sonderfall Bildschirm - Geometrie der Daten beachten
 */
#ifdef noch_NICHT
oyCMSTransform_t oyCMSGetTransform();
int oyCMSTransform( void *out, void *in, oyCMSTransform_t t );
int oyCMSSetCMM( int oyOPTION );

// Wieviel Geraete vertraegt Oyranos?  X ist eigentlich schon ein Sonderfall
// Vielleicht ist eine abstrakte Geraetebeschreibung sinnvoll -> ICCDeviceTag
oyDevice_t


// oder erhaelt oyImage_t die oyDevice_t Infos mit? 
int oyCMSTransformScreenImage( oyDevice_t disp,
                               oyCMSTransform_t t,
                               oyImage_t data );
#endif

#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_CMMS_H */
