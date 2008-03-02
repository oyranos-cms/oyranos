/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004  Kai-Uwe Behrmann 
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
 * helpers
 * 
 */

/* Date:      August 2004 */

#ifndef OYRANOS_HELFER_H
#define OYRANOS_HELFER_H

#define _(text) text


/* mathematical helpers */

#define MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define HYP(a,b)    pow((a)*(a) + (b)*(b),1.0/2.0)
#define HYP3(a,b,c) pow( (a)*(a) + (b)*(b) + (c)*(c) , 1.0/2.0)
#define RUND(a)     ((a) + 0.5)


#endif /* OYRANOS_HELFER_H */
