/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2007  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 *  @brief string translation
 * 
 */

/* Date:      28. 06. 2006 */

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
const char *domain = OY_TEXTDOMAIN;
const char *domain_path = OY_LOCALEDIR;
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
  oy_lang_ = "C";

#ifdef USE_GETTEXT
  if(!oy_country_ || !oy_language_)
  {
    char * temp = 0;
    putenv("NLSPATH=" OY_LOCALEDIR); /* Solaris */
    bindtextdomain( domain, domain_path );

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
    if(temp) oyDeAllocateFunc_(temp);
  }
#endif
  oyTextsCheck_ ();
}


void
oyI18NSet_             ( int active,
                         int reserved )
{
  DBG_PROG_START

  if(active)
    domain = OY_TEXTDOMAIN;
  else
    domain = "";


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


