/** @file oyBlob_s.h

   [Template file inheritance graph]
   +-> Blob_s.template.h
   |
   +-> Base_s.h
   |
   +-- Struct_s.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/01/28
 */



#ifndef OY_BLOB_S_H
#define OY_BLOB_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>


#include "oyStruct_s.h"


typedef struct oyBlob_s oyBlob_s;
/* Include "Blob.public.h" { */

/* } Include "Blob.public.h" */


/* Include "Blob.dox" { */
/** @struct  oyBlob_s
 *  @ingroup objects_value
 *  @extends oyStruct_s
 *  @brief   A data blob object
 *  
 *
 *  @version Oyranos: 0.2.1
 *  @since   2009/01/06 (Oyranos: 0.1.9)
 *  @date    2011/01/19
 */

/* } Include "Blob.dox" */

struct oyBlob_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyBlob_s* OYEXPORT
  oyBlob_New( oyObject_s object );
OYAPI oyBlob_s* OYEXPORT
  oyBlob_Copy( oyBlob_s *blob, oyObject_s obj );
OYAPI int OYEXPORT
  oyBlob_Release( oyBlob_s **blob );



/* Include "Blob.public_methods_declarations.h" { */
OYAPI int  OYEXPORT
                 oyBlob_SetFromData  ( oyBlob_s          * obj,
                                       oyPointer           ptr,
                                       size_t              size,
                                       const char        * type );
OYAPI int  OYEXPORT
                 oyBlob_SetFromStatic( oyBlob_s          * blob,
                                       const oyPointer     ptr,
                                       size_t              size,
                                       const char        * type );
oyPointer          oyBlob_GetPointer ( oyBlob_s          * blob );
size_t             oyBlob_GetSize    ( oyBlob_s          * blob );
const char *       oyBlob_GetType    ( oyBlob_s          * blob );

/* } Include "Blob.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_BLOB_S_H */
