/** @file oyranos_cs.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    ColorSync internal API
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/24
 */


#ifndef OYRANOS_CS_H
#define OYRANOS_CS_H

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_version.h"
#include "oyranos_alpha.h"
#include "oyranos_i18n.h"
#include "oyranos_texts.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef __APPLE__
#include <Carbon/Carbon.h>

int    oyGetProfileBlockOSX              ( CMProfileRef prof,
                                           char **block, size_t *size,
                                           oyAlloc_f     allocate_func);
char * oyGetProfileNameOSX               ( CMProfileRef prof,
                                           oyAlloc_f     allocate_func);
char * oyGetDefaultProfileNameOSX_   ( oyPROFILE_e         type );

int    oyOSxVersionAtRuntime         ( void );

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CS_H */
