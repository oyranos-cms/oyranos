/** @file oyranos_i18n.c
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
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/06/28
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef USE_GETTEXT
#include <libintl.h>
#include <locale.h>
#endif

#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_texts.h"


/* --- static variables   --- */
const char *oy_domain = OY_TEXTDOMAIN;
const char *oy_domain_path = OY_LOCALEDIR;
const char *oy_domain_codeset = 0;
const char *oy_lang_ = 0;
const char *oy_language_ = 0;
const char *oy_country_ = 0;

/* --- internal API definition --- */


/** @internal
 *  @brief  initialise internationalisation 
 *
 *  @since Oyranos: version 0.x.x
 *  @date  26 november 2007 (API 0.0.1)
 */
void oyI18NInit_()
{
  DBG_PROG_START

  oy_lang_ = "C";

#ifdef USE_GETTEXT
  if(!oy_country_ || !oy_language_)
  {
    char * temp = 0;
    putenv("NLSPATH=" OY_LOCALEDIR); /* Solaris */
    bindtextdomain( oy_domain, oy_domain_path );
    DBG_NUM2_S("oy_domain_path %s %s", oy_domain, oy_domain_path)
    if(oy_domain_codeset)
      bind_textdomain_codeset(oy_domain, oy_domain_codeset);
    DBG_NUM2_S("oy_domain_codeset %s %s", oy_domain, oy_domain_codeset)

    if(getenv("LANG"))
    {
      temp = oyStringCopy_(getenv("LANG"), oyAllocateFunc_);
      oy_lang_ = (const char*) temp;
    }

    if(oy_lang_ && oyStrchr_(oy_lang_,'_'))
    {
      char * tmp = 0;
      int len = oyStrlen_(oy_lang_);

      oyAllocHelper_m_( tmp, char, len + 5, 0, return );
      oySprintf_( tmp, "%s", oyStrchr_(oy_lang_,'_')+1 );
      if(oyStrlen_(tmp) > 2)
        tmp[2] = 0;
      oy_country_ = tmp; tmp = 0;

      /*oy_country_ = oyStringCopy_(oyStrchr_(oy_lang_,'_')+1, oyAllocateFunc_);

      if(!oy_country_)
        return;*/

      tmp = oyStrchr_(oy_country_,'.');
      if(tmp)
        tmp[0] = 0;

      tmp = 0;

      oyAllocHelper_m_( tmp, char, len + 5, 0, return );
      oySprintf_( tmp, "%s", oy_lang_ );
      oy_language_ = tmp; tmp = 0;

      /*oy_language_ = oyStringCopy_(oy_lang_, oyAllocateFunc_);
      if(!oy_language_)
        return;*/

      tmp = oyStrchr_(oy_language_,'_');
      if(tmp)
        tmp[0] = 0;
    }
    /*if(temp) oyDeAllocateFunc_(temp);*/
  }
#endif
  oyTextsCheck_ ();

  DBG_PROG_ENDE
}


void
oyI18NSet_             ( int active,
                         int reserved )
{
  DBG_PROG_START

  if(active)
    oy_domain = OY_TEXTDOMAIN;
  else
    oy_domain = "";


  oyI18Nrefresh_();

  DBG_PROG_ENDE
}


void
oyI18Nrefresh_()
{
  oyTextsTranslate_ ();

  /* refresh CMM's */
  oyModulsRefreshI18N_();
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


