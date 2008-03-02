/** @file oyranos_misc.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2006-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    misc
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/07/29
 */

#include <stdio.h>

#include "oyranos_helper.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_sentinel.h"

/** @internal
 *  @brief  read in the declarations of available options and functions 
 *
 *  @param  group        the policy group
 *  @param  xml          xml configuration string
 *  @return              errors
 */
int
oyModulRegisterXML       (oyGROUP_e           group,
                          const char       *xml)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CMMS);

  n = oyModulRegisterXML_(group, xml);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}


/* --- internal API definition --- */


