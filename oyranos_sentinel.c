/** @file oyranos_sentinel.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2006-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    library sentinels
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/09/14
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"


int oyExportStart_(int export_check)
{
  static int export_setting = 1;
  static int export_path = 1;
  static int export_monitor = 1;
  static int export_cmm = 1;
  /*static int export_cmms = 1;*/
  int start = 0;

# define EXPORT_( flag, var, func ) \
  if(export_check & flag && var) \
  { \
    var = 0; \
    func; \
    start = 1; \
  }

  EXPORT_( EXPORT_SETTING, export_setting, oyOpen_() )
  /*EXPORT_( EXPORT_PATH, export_path, oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT) )*/
  /* Currently the monitor API is a link time module and outside the basic API.
     So we cant rely on it on runtime here. 
     This will change when Monitor support will be a runtime link in module.
   */
  EXPORT_( EXPORT_MONITOR, export_monitor, start = 1 )
  EXPORT_( EXPORT_CMMS, export_cmm, oyModulesScan_(0) )

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

  DBG_PROG_START

  if(initialised)
    return;
  initialised = 1;

  if(getenv("OYRANOS_DEBUG"))
    oy_debug = atoi(getenv("OYRANOS_DEBUG"));

  oyI18NInit_ ();
  DBG_PROG_ENDE
}


