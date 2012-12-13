/** @file oyranos_core.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#ifndef OYRANOS_CORE_H
#define OYRANOS_CORE_H

#include <stdarg.h>

#include "oyranos_version.h"
#include "oyranos_types.h"

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef char oyChar;

extern int oy_debug;

typedef enum {
  oyMSG_ERROR = 300,
  oyMSG_WARN,
  oyMSG_DBG
} oyMSG_e;
typedef int  (*oyMessage_f)          ( int/*oyMSG_e*/      code, 
                                       const oyPointer     context,
                                       const char        * format,
                                       ... );
int            oyMessageFormat       ( char             ** message_text,
                                       int                 code,
                                       const oyPointer     context,
                                       const char        * string );
int            oyMessageFunc         ( int                 code,
                                       const oyPointer     context_object,
                                       const char        * format,
                                                           ... );
int            oyMessageFuncSet      ( oyMessage_f         message_func );
extern         oyMessage_f             oyMessageFunc_p;

typedef const char * (*oyStruct_RegisterStaticMessageFunc_f)
                                     ( oyPointer           context,
                                       int                 flags );
int            oyStruct_RegisterStaticMessageFunc (
                                       int                 type,
                                       oyStruct_RegisterStaticMessageFunc_f f);
const char *   oyStruct_GetInfo      ( oyPointer           context,
                                       int                 flags );

int            oyVersion             ( int                 type );
char *         oyVersionString       ( int                 type,
                                       oyAlloc_f           allocateFunc );
char *         oyDescriptionToHTML   ( int                 group,
                                       const char       ** options,
                                       oyAlloc_f           allocateFunc );



/** @brief codeset for Oyranos
 *
 *  set here the codeset part, e.g. "UTF-8", which shall be delivered from
 *  Oyranos string translations.
 *  Set this variable before any call to Oyranos.
 *  The environment variable OY_LOCALEDIR overrides the static inbuild 
 *  OY_LOCALEDIR macro defined in oyranos_config_internal.h . OY_LOCALEDIR
 *  should match a corresponding $prefix/share/locale path.
 */
extern const char *oy_domain_codeset;

void           oyI18NSet             ( int active,
                                       int reserved );
const char *   oyLanguage            ( void );
const char *   oyCountry             ( void );
const char *   oyLang                ( void );
void           oyI18Nreset           ( void );


int                oyBigEndian       ( void );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CORE_H */
