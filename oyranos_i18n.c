/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
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
#include "oyranos_i18n.h"
#include "oyranos_texts.h"


/* --- static variables   --- */
const char *domain = OY_TEXTDOMAIN;
const char *domain_path = LOCALEDIR;

/* --- internal API definition --- */


void oyI18NInit_()
{
#ifdef USE_GETTEXT
  {
    putenv("NLSPATH=" LOCALEDIR); // Solaris
    bindtextdomain( domain, domain_path );
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


