/** @internal
 *  @file oyranos_i18n.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2020 (C) Kai-Uwe Behrmann
 *
 *  @brief    string translation
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/06/28
 */

#include "oyranos_config_internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef USE_GETTEXT
#include <libintl.h>
#include <locale.h>
#else
#include "liboyranos.i18n.h"
#include "oyjl.h" /* oyjl_val oyjlLang() */
#endif

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"


/* --- static variables   --- */
const char *oy_domain = OY_TEXTDOMAIN;
const char *oy_domain_path = OY_LOCALEDIR;
/** \addtogroup misc
 *  @{ *//* misc */
/** \addtogroup i18n
 *  @{ *//* i18n */
const char *oy_domain_codeset = 0;
/** @} *//* i18n */
/** @} *//* misc */
char *oy_lang_ = 0;
char *oy_language_ = 0;
char *oy_country_ = 0;

/* --- internal API definition --- */


int oy_i18n_init_ = 0;
/** @internal
 *  @brief  initialise internationalisation 
 *
 *  @since Oyranos: version 0.x.x
 *  @date  26 november 2007 (API 0.0.1)
 */
void oyI18NInit_()
{
  int use_gettext = 0;
  oyjlTr_s * trc = NULL;
  char * loc = NULL;
  DBG_PROG_START

#ifdef USE_GETTEXT
  use_gettext = 1;
#endif

  if(!oy_i18n_init_)
  {
    ++oy_i18n_init_;

    /* we use the posix setlocale interface;
     * the environmental LANG variable is flacky */
    if(setlocale(LC_MESSAGES, 0))
      oy_lang_ = oyStringCopy_(setlocale(LC_MESSAGES, 0), oyAllocateFunc_);
    else
      oy_lang_ = oyStringCopy_("C", oyAllocateFunc_);

    if(oy_lang_)
    {
#if !defined( USE_GETTEXT )
      const char * t = (const char*) liboyranos_i18n_oiJS;
      int size = sizeof(liboyranos_i18n_oiJS);
      oyjl_val catalog = (oyjl_val) oyjlStringAppendN( NULL, t, size, 0 );
      trc = oyjlTr_New( oy_lang_, OY_TEXTDOMAIN, &catalog, 0,0,0,0 );
#else
      trc = oyjlTr_New( oy_lang_, OY_TEXTDOMAIN, 0, 0,0,0,0 );
#endif
      if(oyStrchr_(oy_lang_,'_'))
      {
        char * tmp = NULL;
        int len = oyStrlen_(oy_lang_);

        oyAllocHelper_m_( tmp, char, len + 5, 0, DBG_PROG_ENDE; return );
        oySprintf_( tmp, "%s", oyStrchr_(oy_lang_,'_')+1 );
        if(oyStrlen_(tmp) > 2)
          tmp[2] = 0;
        oy_country_ = tmp; tmp = NULL;

        tmp = oyStrchr_(oy_country_,'.');
        if(tmp)
          tmp[0] = 0;
        tmp = NULL;

        oyAllocHelper_m_( tmp, char, len + 5, 0, DBG_PROG_ENDE; return );
        oySprintf_( tmp, "%s", oy_lang_ );
        oy_language_ = tmp; tmp = NULL;

        tmp = oyStrchr_(oy_language_,'_');
        if(tmp)
          tmp[0] = 0;
      } else
        oy_language_ = oyStringCopy_( oy_lang_, oyAllocateFunc_);
    }
    oyjlInitLanguageDebug( "Oyranos", "OY_DEBUG", &oy_debug, use_gettext, "OY_LOCALEDIR", OY_LOCALEDIR, &trc, oyMessageFunc_p );
#ifdef USE_GETTEXT
    if(oy_domain_codeset)
    {
      if(oy_debug)
        WARNc2_S("bindtextdomain( %s, %s )", oy_domain, oy_domain_codeset );
      bind_textdomain_codeset(oy_domain, oy_domain_codeset);
    }
    DBG_NUM2_S("oy_domain_codeset %s %s", oy_domain, oyNoEmptyString_m(oy_domain_codeset))
#endif

  }

  DBG_PROG_ENDE
}






/** @internal
 *  @brief reset all variables to renew
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/05 (Oyranos: 0.1.10)
 *  @date    2009/01/05
 */
void           oyI18Nreset_          ( void )
{
  oy_i18n_init_ = 0;
  if(oy_lang_)
    oyFree_m_( oy_lang_ );
  if(oy_language_)
    oyFree_m_( oy_language_ );
  if(oy_country_)
    oyFree_m_( oy_country_ );
  oyExportResetSentinel_();
}

/** @internal
 *  @brief  get Lang code/variable
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLang_               ( void )
{
  return oy_lang_;
}

/** @internal
 *  @brief  get language code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLanguage_           ( void )
{
  return oy_language_;
}

/** @internal
 *  @brief  get country code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyCountry_            ( void )
{
  return oy_country_;
}


