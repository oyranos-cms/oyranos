/** @file oyranos_core.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/25
 */


#ifndef OYRANOS_CORE_H
#define OYRANOS_CORE_H

#include <stdarg.h>

#include "oyranos_version.h"
#include "oyranos_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef char oyChar;

extern int oy_debug;
extern int oy_debug_objects;

typedef enum {
  oyMSG_ERROR = 300,
  oyMSG_WARN,
  oyMSG_DBG
} oyMSG_e;
typedef int  (*oyMessage_f)          ( int/*oyMSG_e*/      code, 
                                       const void        * context,
                                       const char        * format,
                                       ... );
int            oyMessageFormat       ( char             ** message_text,
                                       int                 code,
                                       const void        * context,
                                       const char        * string );
int            oyMessageFunc         ( int                 code,
                                       const void        * context_object,
                                       const char        * format,
                                                           ... );
int            oyGuiMessageFunc      ( int                 code,
                                       const void        * c,
                                       const char        * format,
                                                           ... );
int            oyMessageFuncSet      ( oyMessage_f         message_func );
extern         oyMessage_f             oyMessageFunc_p;

/** @enum    oyNAME_e
 *  @brief   Information level
 *  @ingroup objects_generic
 *
 *  Messages consist of text, which contains information depending on the
 *  purpose. The information provider obtains with this enum a brief request,
 *  what to put into a information request.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2007/10/00
 */
typedef enum {
  oyNAME_NAME,                         /**< a short text describing the object
                                            or information in some few words;
                                            e.g. "ICC Color Profile" */
  oyNAME_NICK,                         /**< a very short text of few letters,
                                            very likely one word; e.g. "ICC" */
  oyNAME_DESCRIPTION                   /**< Some sentences as useful in help
                                            texts and other richer
                                            representations. */
} oyNAME_e;

typedef const char * (*oyStruct_RegisterStaticMessageFunc_f)
                                     ( oyPointer           context,
                                       oyNAME_e            type,
                                       int                 flags );
typedef void (*oyStruct_RegisterStaticFreeFunc_f)
                                     (  );
int            oyStruct_RegisterStaticMessageFunc (
                                       int                 type,
                                       oyStruct_RegisterStaticMessageFunc_f msg,
                                       oyStruct_RegisterStaticFreeFunc_f free );
const char *   oyStruct_GetInfo      ( oyPointer           context,
                                       oyNAME_e            type,
                                       int                 flags );

int            oyVersion             ( int                 type );
const char *   oyVersionString       ( int                 type );
char *         oyDescriptionToHTML   ( int                 group,
                                       const char       ** options,
                                       oyAlloc_f           allocateFunc );
void           oyObjectDebugMessage_ ( void              * object,
                                       const char        * function_name,
                                       const char        * struct_name );
                                       


/** @brief codeset for Oyranos
 *
 *  set here the codeset part, e.g. "UTF-8", which shall be delivered from
 *  Oyranos string translations.
 *  Set this variable before any call to Oyranos.
 *  The environment variable ::OY_LOCALEDIR overrides the static inbuild 
 *  ::OY_LOCALEDIR macro defined in oyranos_version.h . ::OY_LOCALEDIR
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


/* String Helpers */
#define oyNoEmptyString_m( text_nem ) text_nem?text_nem:"---"
int                oyStringAddPrintf ( char             ** text,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc,
                                       const char        * format,
                                                           ... );

char**             oyStringSplit     ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       oyAlloc_f           allocateFunc );
void               oyStringListRelease(char            *** l,
                                       int                 size,
                                       oyDeAlloc_f         deallocFunc );
char*              oyStringCopy      ( const char        * text,
                                       oyAlloc_f           allocateFunc );

void               oyLibCoreRelease  ( );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CORE_H */
