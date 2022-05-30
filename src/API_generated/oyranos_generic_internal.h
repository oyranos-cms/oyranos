/** @file oyranos_generic_internal.h

   [Template file inheritance graph]
   +-- oyranos_generic_internal.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_GENERIC_INTERNAL_H
#define OYRANOS_GENERIC_INTERNAL_H

#include "oyHash_s.h"
#include "oyStructList_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

oyHash_s *   oyCacheListGetEntry_    ( oyStructList_s    * cache_list,
                                       uint32_t            flags,
                                       const char        * hash_text );


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_GENERIC_INTERNAL_H */
