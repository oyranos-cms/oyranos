/** @file oyHash_s.h

   [Template file inheritance graph]
   +-> oyHash_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#ifndef OY_HASH_S_H
#define OY_HASH_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyHash_s oyHash_s;


#include "oyStruct_s.h"


/* Include "Hash.public.h" { */

/* } Include "Hash.public.h" */


/* Include "Hash.dox" { */
/** @struct  oyHash_s
 *  @ingroup objects_generic
 *  @extends oyStruct_s
 *  @brief   A cache entry
 *
 *  Combine hash, description and oyPointer to one searchable struct. The struct
 *  can be used in a oyStructList_s for a hash map or searchable cache.
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */

/* } Include "Hash.dox" */

struct oyHash_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyHash_s* OYEXPORT
  oyHash_New( oyObject_s object );
OYAPI oyHash_s* OYEXPORT
  oyHash_Copy( oyHash_s *hash, oyObject_s obj );
OYAPI int OYEXPORT
  oyHash_Release( oyHash_s **hash );



/* Include "Hash.public_methods_declarations.h" { */
OYAPI oyHash_s *  OYEXPORT
                 oyHash_Create     ( const char        * hash_text,
                                     oyObject_s          object );
OYAPI oyOBJECT_e  OYEXPORT
                 oyHash_GetType    ( oyHash_s          * hash );
OYAPI int  OYEXPORT
                 oyHash_IsOf       ( oyHash_s          * hash,
                                     oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyHash_SetPointer ( oyHash_s          * hash,
                                     oyStruct_s        * obj );
OYAPI oyStruct_s * OYEXPORT
                 oyHash_GetPointer ( oyHash_s          * hash,
                                     oyOBJECT_e          type );

/* } Include "Hash.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_HASH_S_H */
