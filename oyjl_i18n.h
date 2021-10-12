/**
 *  @internal
 *  @file     oyjl_i18n.h
 *
 *  oyjl - convinient tree JSON APIs
 *
 *  @par Copyright:
 *            2019-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    translations
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2019/01/14
 */

#ifndef OYJL_I18N_INTERNAL_H
#define OYJL_I18N_INTERNAL_H 1

#include "oyjl_version.h"
#ifdef OYJL_USE_GETTEXT
# ifdef OYJL_HAVE_LIBINTL_H
#  include <libintl.h> /* bindtextdomain() */
# endif
# define _(text) dgettext( OYJL_DOMAIN, text )
#else
# ifndef OYJL_SKIP_TRANSLATE
#  define _(text) oyjlTranslate( oyjl_lang_, oyjl_catalog_, text, 0 )
# else
#  define _(text) text
# endif
#endif

#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>           /* setlocale LC_NUMERIC */
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern char * oyjl_lang_;
extern oyjl_val oyjl_catalog_;


#ifdef __cplusplus
}
#endif

#endif /* OYJL_I18N_INTERNAL_H */
