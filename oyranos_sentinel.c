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
 *
 * library sentinels
 * 
 */

/* Date:      28. 06. 2006 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

/* --- Helpers  --- */

/* --- static variables   --- */
char *old_td = NULL, *old_bdtd = NULL;
char *domain_path = LOCALEDIR;

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */



void oyExportStart_()
{
  oyInit_();
#ifdef USE_GETTEXT_
  WARN_S((_("Yes")))
  {

    {
    char *t2=0;
      //old_td = textdomain( 0 );
      //old_bdtd = bindtextdomain( old_td, 0);

      if((old_td && (strcmp(old_td, domain) != 0)) ||
         !old_td)
      {
        setlocale(LC_MESSAGES, "");
        domain_path = bindtextdomain (domain, LOCALEDIR);
        if(!domain_path)
          domain_path = bindtextdomain (domain, SRC_LOCALEDIR);
        t2 = textdomain( domain );
        WARN_S(("Setting textdomain from %s in %s\n to %s in %s", old_td, old_bdtd, t2, domain_path ))
      } else
      {
        WARN_S(("no textdomain changed %s %s", old_td, domain))
        WARN_S((" from %s in %s\n to %s in %s", textdomain(old_td), bindtextdomain(old_td,old_bdtd), t2, domain_path ))
      }
    }
  }
#endif
}

void oyExportEnd_()
{
#ifdef USE_GETTEXT_
  {
    char *t1=0, *t2=0;
    if(old_td && (strcmp(old_td, domain) != 0))
    {
      setlocale(LC_MESSAGES, "");
      if(old_bdtd) {
        t1 = bindtextdomain( old_td, old_bdtd );
        WARN_S(("bindtextdomain done"))
      }
      t2 = textdomain( old_td );
      WARN_S(("Setting back to old textdomain: %s in %s\n%s in %s", old_td, old_bdtd, t2, t1 ))
    } else
      WARN_S(("no textdomain changed %s %s", old_td, domain))
  }
#endif
}

void oyInit_()
{
  static int initialised = 0;

  if(initialised)
    return;
  initialised = 1;

#ifdef USE_GETTEXT
  {
    setlocale(LC_MESSAGES, "");
    putenv("NLSPATH=" LOCALEDIR); // Solaris
    bindtextdomain( "oyranos", LOCALEDIR );
  }
#endif
}


