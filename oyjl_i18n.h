/** @file oyjl_i18n.h
 *
 *  oyjl - Basic helper C API's
 *
 *  @par Copyright:
 *            2010-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl API provides a platformindependent C interface for i18n handling.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_I18N_H
#define OYJL_I18N_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup oyjl_i18n */
/** @{ *//* oyjl_i18n */
typedef struct oyjl_val_s * oyjl_val;

/* --- i18n helpers --- */
typedef struct oyjlTranslation_s oyjlTranslation_s;
char *         oyjlLanguage          ( const char        * loc );
char *         oyjlCountry           ( const char        * loc );
const char *   oyjlLang              ( const char        * loc );
char *         oyjlTranslate         ( oyjlTranslation_s * context,
                                       const char        * string );
/** @brief custom translator, for switching the translation engine */
typedef char*(*oyjlTranslate_f)      ( oyjlTranslation_s * context,
                                       const char        * string );
#define OYJL_GETTEXT                   0x400000 /**< @brief use gettext */
/* Workaround for chaotic key order. Slow. */
#define OYJL_NO_OPTIMISE               0x800000 /**< @brief skip binary search */
oyjlTranslation_s* oyjlTranslation_New(const char        * loc,
                                       const char        * domain,
                                       oyjl_val          * catalog,
                                       oyjlTranslate_f     translator,
                                       void              * user_data,
                                       void             (* deAlloc)(void*),
                                       int                 flags );
oyjlTranslate_f oyjlTranslation_GetTranslator (
                                       oyjlTranslation_s * context );
const char * oyjlTranslation_GetLang ( oyjlTranslation_s * context );
const char * oyjlTranslation_GetDomain(oyjlTranslation_s * context );
oyjl_val oyjlTranslation_GetCatalog  ( oyjlTranslation_s * context );
void *   oyjlTranslation_GetUserData ( oyjlTranslation_s * context );
int          oyjlTranslation_GetFlags( oyjlTranslation_s * context );
void         oyjlTranslation_SetFlags( oyjlTranslation_s * context,
                                       int                 flags );
void         oyjlTranslation_SetLocale(oyjlTranslation_s * context,
                                       const char        * loc );
void         oyjlTranslation_Release ( oyjlTranslation_s** context );
oyjlTranslation_s* oyjlTranslation_Get(const char        * domain );
int          oyjlTranslation_Set     ( oyjlTranslation_s** context );
int          oyjlTranslation_Unset   ( const char        * domain );
void         oyjlTranslateJson       ( oyjl_val            root,
                                       oyjlTranslation_s * context,
                                       const char        * key_list );

/** @} *//* oyjl_i18n */


#ifdef __cplusplus
}
#endif


#endif /* OYJL_I18N_H */
