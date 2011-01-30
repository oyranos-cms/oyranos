/** @file oyHash_s.h

   [Template file inheritance graph]
   +-> Hash_s.template.h
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
 *  @since    2011/01/30
 */



#ifndef OY_HASH_S_H
#define OY_HASH_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>


#include "oyStruct_s.h"


typedef struct oyHash_s oyHash_s;
/* Include "Hash.public.h" { */

/* } Include "Hash.public.h" */


/* Include "Hash.dox" { */
/** @struct  oyHash_s
 *  @ingroup objects_generic
 *  @extends oyStruct_s
 *  @brief   A cache entry
 *  @internal
 *  
 *  Combine hash, description and oyPointer to one searchable struct. The struct
 *  can be used in a oyStructList_s for a hash map or searchable cache.
 *  @see oyCacheListNew_ oyHashGet_
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *  @note New templates will not be created automaticly [notemplates]
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */

/* } Include "Hash.dox" */

struct oyHash_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyHash_s* OYEXPORT
  oyHash_New( oyObject_s object );
OYAPI oyHash_s* OYEXPORT
  oyHash_Copy( oyHash_s *hash, oyObject_s obj );
OYAPI int OYEXPORT
  oyHash_Release( oyHash_s **hash );



/* Include "Hash.public_methods_declarations.h" { */
oyHash_s *         oyHash_Get        ( const char        * hash_text,
                                       oyObject_s          object );
oyOBJECT_e         oyHash_GetType    ( oyHash_s          * hash );
int                oyHash_IsOf       ( oyHash_s          * hash,
                                       oyOBJECT_e          type );
int                oyHash_SetPointer ( oyHash_s          * hash,
                                       oyStruct_s        * obj );
oyStruct_s *       oyHash_GetPointer ( oyHash_s          * hash,
                                       oyOBJECT_e          type );

/* } Include "Hash.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_HASH_S_H */
