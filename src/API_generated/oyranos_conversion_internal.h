
/** @file oyranos_conversion_internal.h

   [Template file inheritance graph]
   +-- oyranos_conversion_internal.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_CONVERSION_INTERNAL_H
#define OYRANOS_CONVERSION_INTERNAL_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#include "oyranos_types.h"

#include "oyFilterNode_s.h"
#include "oyPointer_s.h"

int          oyPointer_ConvertData   ( oyPointer_s       * cmm_ptr,
                                       oyPointer_s       * cmm_ptr_out,
                                       oyFilterNode_s    * node );

#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */


#endif /* OYRANOS_CONVERSION_INTERNAL_H */
