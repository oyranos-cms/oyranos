
/** @file oyranos_conversion.h

   [Template file inheritance graph]
   +-- oyranos_conversion.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_CONVERSION_H
#define OYRANOS_CONVERSION_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#include "oyranos_types.h"
#include "oyOptions_s.h"

/** \addtogroup objects_conversion
 *  @brief Directed Acyclic Graph handling, caching and data processing
 *
 *  @{ *//* objects_conversion */

/** @} *//* objects_conversion */ 

uint32_t     oyICCProfileSelectionFlagsFromOptions (
                                       const char        * db_base_key,
                                       const char        * base_pattern,
                                       oyOptions_s       * options,
                                       int                 select_core );

#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */


#endif /* OYRANOS_CONVERSION_H */
