/** @file oyranos_i18n.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2006-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    string translation
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/07/29
 */


#ifndef OYRANOS_I18N_H
#define OYRANOS_I18N_H

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

#ifdef USE_GETTEXT
# include <libintl.h>
# include <locale.h>
# define _(text) dgettext( domain, text )
#else
# define _(text) text
#endif
extern const char *domain;
extern const char *domain_path;

#define OY_TEXTDOMAIN "oyranos"

/* library sentinels */
void   oyI18NInit_                   ( void);
void   oyI18Nrefresh_();
void   oyI18NSet_                    ( int active,
                                       int reserved );

const char *   oyLang_               ( void );
const char *   oyLanguage_           ( void );
const char *   oyCountry_            ( void );


#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_I18N_H */
