/** @file oyBlob_s.h

   [Template file inheritance graph]
   +-> oyBlob_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2016 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#ifndef OY_BLOB_S_H
#define OY_BLOB_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyBlob_s oyBlob_s;


#include "oyStruct_s.h"


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
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyBlob_s* OYEXPORT
  oyBlob_New( oyObject_s object );
/** @memberof oyBlob_s
 *  @brief    Copy or Reference a Blob object
 *
 *  The macro is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     blob                 Blob struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
#define oyBlob_Copy(blob,object) oyBlob_Copy_x(blob,object); if(oy_debug_objects) oyObjectDebugMessage_( blob?blob->oy_:NULL, __func__, "oyBlob_s" );
OYAPI oyBlob_s* OYEXPORT
  oyBlob_Copy_x( oyBlob_s *blob, oyObject_s obj );
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
OYAPI oyPointer  OYEXPORT
                 oyBlob_GetPointer ( oyBlob_s          * blob );
OYAPI size_t  OYEXPORT
                 oyBlob_GetSize    ( oyBlob_s          * blob );
OYAPI const char *  OYEXPORT
                 oyBlob_GetType    ( oyBlob_s          * blob );

/* } Include "Blob.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_BLOB_S_H */
