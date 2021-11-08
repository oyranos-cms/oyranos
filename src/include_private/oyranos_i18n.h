/** @internal
 *  @file oyranos_i18n.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    string translation
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/07/29
 */


#ifndef OYRANOS_I18N_H
#define OYRANOS_I18N_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "oyranos_config_internal.h"

#if defined(HAVE_LOCALE_H)
#include <locale.h>           /* setlocale LC_NUMERIC */
#endif

#ifdef HAVE_LIBINTL_H
# include <libintl.h>
#endif
#include "oyjl.h"
#define OY_TEXTDOMAIN "oyranos"
#define _(text) (char*)oyjlTranslate( oyjlTr_Get(OY_TEXTDOMAIN), text )
extern const char *oy_domain;
extern const char *oy_domain_path;


void   oyI18NInit_                   ( void );
void   oyI18Nreset_                  ( void );
void   oyI18NSet_                    ( int active,
                                       int reserved );

const char *   oyLang_               ( void );
const char *   oyLanguage_           ( void );
const char *   oyCountry_            ( void );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_I18N_H */
