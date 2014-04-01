/** @file oyranos_check.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    input / output  methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/11/17
 *
 *  This file contains functions for handling and writing files and directories.
 */


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

int	oyCheckProfile_                   (const char* name,const char* colorsig, int flags);
int	oyCheckProfileMem_                (const void* mem, size_t size,
                                           const char* colorsig, int flags);

int     oyProfileGetMD5_             ( void       * buffer,
                                       size_t       size,
                                       unsigned char     * md5_return );

int	oyCheckPolicy_               ( const char * name );

#endif /* OYRANOS_CHECK_H */
