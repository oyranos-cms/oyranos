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
#ifdef OYJL_HAVE_LIBINTL_H
# include <libintl.h> /* bindtextdomain() */
#endif
#ifndef OYJL_SKIP_TRANSLATE
extern oyjlTr_s**oyjl_tr_context_;
# define _(text) oyjlTranslate( oyjlTr_Get( OYJL_DOMAIN ), text )
#else
# define _(text) text
#endif

#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>           /* setlocale LC_NUMERIC */
#endif

#ifdef __cplusplus
extern "C" {
#endif
char *           oyjlLangForCatalog_ ( const char        * loc,
                                       oyjl_val            catalog,
                                       int               * cstart,
                                       int               * cend,
                                       int                 flags );
char **          oyjlCatalogGetLangs_( char             ** paths,
                                       int                 count,
                                       int               * langs_n,
                                       int              ** lang_positions_start );
int          oyjlTr_GetStart_        ( oyjlTr_s          * context );
int          oyjlTr_GetEnd_          ( oyjlTr_s          * context );

#ifdef __cplusplus
}
#endif

#endif /* OYJL_I18N_INTERNAL_H */
