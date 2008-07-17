/** @file oyranos_xml.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2006-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    xml handling
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/02/10
 */


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
char*       oyWriteOptionToXML_(oyGROUP_e           group,
                                oyWIDGET_e          start,
                                oyWIDGET_e          end, 
                                char             *mem,
                                int               oytmplen);

char*       oyPolicyToXML_     (oyGROUP_e           group,
                                int               add_header,
                                oyAlloc_f         allocate_func);

int         oyReadXMLPolicy_   (oyGROUP_e           group, 
                                const char       *xml);

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_XML_H */

