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
 * hardware API
 * 
 */

/* Date:      01. 02. 2005 */


#ifndef OYRANOS_MONITOR_H
#define OYRANOS_MONITOR_H

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


/*
 * hardware detection with X
 * should be independent of the huge windowing system
 */


/* monitor names */

int
oyGetMonitorInfo                  (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial);
char*
oyGetMonitorProfileName           (const char* display);

int
oySetMonitorProfile               (const char* display_name,
                                   const char* profil_name );

#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_H */
