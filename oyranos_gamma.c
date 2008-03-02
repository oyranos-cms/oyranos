/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2005  Kai-Uwe Behrmann
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
 * gamma loader - put it somethere in Your xinitrc
 * It reads the default profile from the Oyranos CMS and recalls this profile
 * as new default profile for this screen, resulting in an curves upload to
 * the video card.
 * Currently You need xcalib installed to do the curves upload.
 * 
 */

/* Date:      03. 02. 2005 */


#include "oyranos_monitor.h"
#include <stdlib.h>

int main(void)
{
  char *display_name = getenv("DISPLAY");
  char *monitor_profile = oyGetMonitorProfileName (display_name);
  int error = 0;

  if( monitor_profile )
    error = oyActivateMonitorProfile (display_name, monitor_profile);

  return error;
}
