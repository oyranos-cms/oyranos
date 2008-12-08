/** @file oyranos_cmms.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2006-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    old CMM's framework
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/07/29
 */


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
                                      oyAlloc_f     alloc_func );
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


int              oyCMMlibMatchesCMM  ( const char        * lib_name,
                                       const char        * cmm );

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_CMMS_H */
