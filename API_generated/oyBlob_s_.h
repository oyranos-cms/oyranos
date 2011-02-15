/** @file oyBlob_s_.h

   [Template file inheritance graph]
   +-> Blob_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2011/02/15
 */


#ifndef OY_BLOB_S__H
#define OY_BLOB_S__H

#define oyBlobPriv_m( var ) ((oyBlob_s_*) (var))

  
#include <oyranos_object.h>
  

#include "oyBlob_s.h"

typedef struct oyBlob_s_ oyBlob_s_;
/* Include "Blob.private.h" { */

/* } Include "Blob.private.h" */

/** @internal
 *  @struct   oyBlob_s_
 *  @brief      A data blob object
 *  @ingroup  objects_value
 *  @extends  oyStruct_s
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
struct oyBlob_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */



/* Include "Blob.members.h" { */
  size_t               size;           /**< data size */
  oyPointer            ptr;            /**< data */
  int                  flags;          /**< 0x01 - static ptr */
  char                 type[8];        /**< the type of data, e.g. oyCOLOUR_ICC_DEVICE_LINK / "oyDL" */

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


#endif /* OY_BLOB_S__H */
