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
 * sorting
 * 
 */

/* Date:      02. 09. 2005 */

#include "oyranos_helper.h"
#include "oyranos.h"

#include <stdio.h>

/* --- internal API definition --- */

/* internal memory handling */
void* oyAllocateFunc_           (size_t        size)
{
  /* we have most often to work with text arrays, so initialise with 0 */
  void *ptr = calloc (sizeof (char), size);

  if( !ptr )
    WARN_S(( "can not allocate %d byte", (int)size ));

  return ptr;
}

void  oyDeAllocateFunc_           (void*       block)
{
  if( !block ) {
    WARN_S(( "emory block is empty" ))
  } else
    free( block );
}


const char *domain = "oyranos";
