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
 *  @brief internal CMM API
 */

/** @date      29. 07. 2006 */


#ifndef OYRANOS_CMMS_H
#define OYRANOS_CMMS_H

#include "oyranos_internal.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */



int          oyModulRemove_          (const char *id);
char**       oyModulsGetNames_       (int           *count,
                                      oyAllocFunc_t alloc_func );
oyGROUP_e    oyRegisterGroups_       (char *cmm, char *id,
                                      char *name, char *tooltip);
int          oyModulRegisterXML_     (oyGROUP_e           group,
                                      const char       *xml);
int          oyModulesScan_          ( int                 flags );


int              oyCMMapi1_Check_    ( oyCMMapi1_s       * api1 );


/** \internal
 *  build a oyCMM_t__ API
 */
oyOption_t_* oyModulsUIOptionSearch_ (oyWIDGET_e    id);
const char*  oyModulGetName_         (const char *cmm);
const char*  oyModulGetDescription_  (const char *cmm);
const char*  oyModulGetXml_          (const char *cmm);
const char*  oyModulGetFunc_         ( const char        * cmm,
                                       const char        * id,
                                       const char       ** lib_name );
void         oyModulReference_       ( const char        * cmm );
void         oyModulRelease_         ( const char        * cmm );
/*const char*  oyModulGetDomain_       (const char *cmm);
const char*  oyModulGetDomainPath_   (const char *cmm);*/
void         oyModulGetGroups_       (const char *cmm, int *start, int *count);
/*const char*  oyModulGetGroupUITitle_ (oyGROUP_e   group, const char **tooltip,
                                      const char**xml);*/

char*        oyModulPrint_           (const char *cmm);
void         oyModulRefreshI18N_     (const char *cmm);
void         oyModulsRefreshI18N_    (void);


#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_CMMS_H */
