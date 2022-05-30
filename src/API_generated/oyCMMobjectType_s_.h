/** @file oyCMMobjectType_s_.h

   [Template file inheritance graph]
   +-> oyCMMobjectType_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_CMM_OBJECT_TYPE_S__H
#define OY_CMM_OBJECT_TYPE_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMobjectTypePriv_m( var ) ((oyCMMobjectType_s_*) (var))

typedef struct oyCMMobjectType_s_ oyCMMobjectType_s_;

  
#include <oyranos_object.h>
  

#include "oyCMMobjectType_s.h"

/* Include "CMMobjectType.private.h" { */
/** typedef oyCMMobjectLoadFromMem_f
 *  @brief   load a filter object from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMobjectType_s
 *
 *  @param[in]     buf_size            data size
 *  @param[in]     buf                 data blob
 *  @param[in]     flags               for future use
 *  @param[in]     object              the optional base
 *  @return                            the object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */
typedef oyStruct_s * (*oyCMMobjectLoadFromMem_f) (
                                       size_t              buf_size,
                                       const oyPointer     buf,
                                       uint32_t            flags,
                                       oyObject_s          object);

/** typedef oyCMMobjectScan_f
 *  @brief   load a filter object from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMobjectType_s
 *
 *  @param[in]     data                data blob
 *  @param[in]     size                data size
 *  @param[out]    registration        filter registration string
 *  @param[out]    name                filter name
 *  @param[in]     allocateFunc        e.g. malloc
 *  @return                            0 on success; error >= 1; unknown < 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */
typedef int          (*oyCMMobjectScan_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       char             ** registration,
                                       char             ** name,
                                       oyAlloc_f           allocateFunc );

/* } Include "CMMobjectType.private.h" */

/** @internal
 *  @struct   oyCMMobjectType_s_
 *  @brief      Custom object handler
 *  @ingroup  module_api
 *  @extends  oyStruct_s
 */
struct oyCMMobjectType_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "CMMobjectType.members.h" { */
  /** object type id,
   *  set to a object type known to Oyranos, or
   *  set as a unique four byte signature, like 'myID' just more unique
   *  to avoid collisions. The id shall match a the oyStruct_s::type_ member
   *  generated through oyCMMobjectLoadFromMem_f. */
  oyOBJECT_e       id;
  /** a colon separated list of sub paths to expect the data in,
      e.g. "color/icc" */
  const char     * paths;
  const char     * (*pathsGet)();      /**< e.g. non XDG colon separated paths*/
  const char     * exts;               /**< file extensions, e.g. "icc:icm" */
  const char     * element_name;       /**< XML element name, e.g. "profile" */
  oyCMMobjectGetText_f             oyCMMobjectGetText; /**< */
  oyCMMobjectLoadFromMem_f         oyCMMobjectLoadFromMem; /**< */
  oyCMMobjectScan_f                oyCMMobjectScan; /**< */

/* } Include "CMMobjectType.members.h" */

};


oyCMMobjectType_s_*
  oyCMMobjectType_New_( oyObject_s object );
oyCMMobjectType_s_*
  oyCMMobjectType_Copy_( oyCMMobjectType_s_ *cmmobjecttype, oyObject_s object);
oyCMMobjectType_s_*
  oyCMMobjectType_Copy__( oyCMMobjectType_s_ *cmmobjecttype, oyObject_s object);
int
  oyCMMobjectType_Release_( oyCMMobjectType_s_ **cmmobjecttype );



/* Include "CMMobjectType.private_methods_declarations.h" { */

/* } Include "CMMobjectType.private_methods_declarations.h" */



void oyCMMobjectType_Release__Members( oyCMMobjectType_s_ * cmmobjecttype );
int oyCMMobjectType_Init__Members( oyCMMobjectType_s_ * cmmobjecttype );
int oyCMMobjectType_Copy__Members( oyCMMobjectType_s_ * dst, oyCMMobjectType_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_OBJECT_TYPE_S__H */
