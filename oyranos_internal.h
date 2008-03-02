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



/* device profiles */
/** \internal enum identifying device types for distinguishing in searches */
typedef enum  {
  /*oyNOTYPE,*/
  oyDISPLAY,          /**< dynamic viewing */
  oyPRINTER,          /**< static media (dye, ink, offset, imagesetters) */
  oySCANNER,          /**< contact digitiser */
  oyCAMERA,           /**< virtual or contactless image capturing */
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
  oyDISPLAY_T,
} oyOBJECT_TYPE;

#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_INTERNAL_H */
