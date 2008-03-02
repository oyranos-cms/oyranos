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
 *  @brief library sentinels
 */

/* Date:      28. 06. 2006 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"


int oyExportStart_(int export_check)
{
  static int export_path = 1;
  static int export_setting = 1;
  int start = 0;

# define EXPORT_( flag, var, func ) \
  if(export_check & flag && var) \
  { \
    var = 0; \
    func; \
    start = 1; \
  }

  EXPORT_( EXPORT_SETTING, export_setting, oyOpen_() )
  EXPORT_( EXPORT_PATH, export_path, oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT) )
  EXPORT_( EXPORT_MONITOR, export_setting, start = 1 )

  oyInit_();
  return start;
}

int oyExportEnd_()
{
  static int start = 1;
  if(start == 1)
  {
    start = 0;
    return 1;
  }
  return start;
}

void oyInit_()
{
  static int initialised = 0;

  if(initialised)
    return;
  initialised = 1;

  if(getenv("OYRANOS_DEBUG"))
    oy_debug = atoi(getenv("OYRANOS_DEBUG"));

  oyI18NInit_ ();
}


