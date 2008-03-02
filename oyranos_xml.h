/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2006  Kai-Uwe Behrmann
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
 *  @brief xml handling
 */

/** @date      10. 02. 2006 */

#ifndef OYRANOS_XML_H
#define OYRANOS_XML_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


/* memory handling for text parsing and writing */
/* mem with old_leng will be stretched if add dont fits inside */

int         oyMemBlockExtent_  (char **mem, int old_len, int add);

/* gives string bordered by a xml style keyword */
char*       oyXMLgetValue_     (const char       *xml,
                                const char       *key);

/* gives the position and length of a string bordered by a xml style keyword */
char*       oyXMLgetField_     (const char       *xml,
                                const char       *key,
                                int              *len);

/* gives all strings bordered by a xml style keyword from xml */
char**      oyXMLgetArray_     (const char       *xml,
                                const char       *key,
                                int              *count);

/* write option range to mem, allocating memory on demand */
char*       oyWriteOptionToXML_(oyGROUP           group,
                                oyWIDGET          start,
                                oyWIDGET          end, 
                                char             *mem,
                                int               oytmplen);

char*       oyPolicyToXML_     (oyGROUP           group,
                                int               add_header,
                                oyAllocFunc_t     allocate_func);

int         oyReadXMLPolicy_   (oyGROUP           group, 
                                const char       *xml);

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_XML_H */

