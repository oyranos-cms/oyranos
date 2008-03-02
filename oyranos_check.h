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
 *  @brief input / output methods
 *
 *  This file contains functions for handling and writing files and directories.
 */

/* Date:      17. 11. 2006 */

#ifndef OYRANOS_CHECK_H
#define OYRANOS_CHECK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- internal API definition --- */

/* separate from the external functions */

int	oyCheckProfile_                   (const char* name,const char* coloursig);
int	oyCheckProfile_Mem                (const void* mem, size_t size,
                                           const char* coloursig);

int    oyProfileGetMD5_              ( void       *buffer,
                                       size_t      size,
                                       char       *md5_return );

#endif /* OYRANOS_CHECK_H */
