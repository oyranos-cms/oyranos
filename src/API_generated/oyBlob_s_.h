/** @file oyBlob_s_.h

   [Template file inheritance graph]
   +-> oyBlob_s_.template.h
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


#ifndef OY_BLOB_S__H
#define OY_BLOB_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyBlobPriv_m( var ) ((oyBlob_s_*) (var))

typedef struct oyBlob_s_ oyBlob_s_;

  
#include <oyranos_object.h>
  

#include "oyBlob_s.h"

/* Include "Blob.private.h" { */

/* } Include "Blob.private.h" */

/** @internal
 *  @struct   oyBlob_s_
 *  @brief      A data blob object
 *  @ingroup  objects_value
 *  @extends  oyStruct_s
 */
struct oyBlob_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Blob.members.h" { */
  size_t               size;           /**< data size */
  oyPointer            ptr;            /**< data */
  int                  flags;          /**< 0x01 - static ptr */
  char                 type[8];        /**< the type of data, e.g. oyCOLOR_ICC_DEVICE_LINK / "oyDL" */

/* } Include "Blob.members.h" */

};


oyBlob_s_*
  oyBlob_New_( oyObject_s object );
oyBlob_s_*
  oyBlob_Copy_( oyBlob_s_ *blob, oyObject_s object);
oyBlob_s_*
  oyBlob_Copy__( oyBlob_s_ *blob, oyObject_s object);
int
  oyBlob_Release_( oyBlob_s_ **blob );



/* Include "Blob.private_methods_declarations.h" { */

/* } Include "Blob.private_methods_declarations.h" */



void oyBlob_Release__Members( oyBlob_s_ * blob );
int oyBlob_Init__Members( oyBlob_s_ * blob );
int oyBlob_Copy__Members( oyBlob_s_ * dst, oyBlob_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_BLOB_S__H */
