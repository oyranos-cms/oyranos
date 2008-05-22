/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2008  Kai-Uwe Behrmann
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
 * test
 * 
 */

/*  Date:      25. 11. 2004 */

#include <kdb.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <oyranos.h>

int
main(int argc, char** argv)
{
  int i;
  uint32_t size = 0;
  char ** profiles = oyProfileListGet ( "prtr", &size, malloc );

  for( i = 0; i < (int) size; ++i )
    printf( "%d: %s\n", i, profiles[i]);

  return 0;
}


