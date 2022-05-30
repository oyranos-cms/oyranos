/** @file oyHash_s_.h

   [Template file inheritance graph]
   +-> oyHash_s_.template.h
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


#ifndef OY_HASH_S__H
#define OY_HASH_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyHashPriv_m( var ) ((oyHash_s_*) (var))

typedef struct oyHash_s_ oyHash_s_;

  
#include <oyranos_object.h>
  

#include "oyHash_s.h"

/* Include "Hash.private.h" { */

/* } Include "Hash.private.h" */

/** @internal
 *  @struct   oyHash_s_
 *  @brief      A cache entry
 *  @ingroup  objects_generic
 *  @extends  oyStruct_s
 */
struct oyHash_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Hash.members.h" { */
  oyStruct_s         * oy_struct;      /**< holds a pointer to something */

/* } Include "Hash.members.h" */

};


oyHash_s_*
  oyHash_New_( oyObject_s object );
oyHash_s_*
  oyHash_Copy_( oyHash_s_ *hash, oyObject_s object);
oyHash_s_*
  oyHash_Copy__( oyHash_s_ *hash, oyObject_s object);
int
  oyHash_Release_( oyHash_s_ **hash );



/* Include "Hash.private_methods_declarations.h" { */
oyHash_s_ *         oyHash_Get_       ( const char        * hash_text,
                                        oyObject_s          object );
int                oyHash_IsOf_      ( oyHash_s_         * hash,
                                       oyOBJECT_e          type );
int                oyHash_SetPointer_( oyHash_s_         * hash,
                                       oyStruct_s        * obj );
oyStruct_s *       oyHash_GetPointer_( oyHash_s_         * hash,
                                       oyOBJECT_e          type );

/* } Include "Hash.private_methods_declarations.h" */



void oyHash_Release__Members( oyHash_s_ * hash );
int oyHash_Init__Members( oyHash_s_ * hash );
int oyHash_Copy__Members( oyHash_s_ * dst, oyHash_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_HASH_S__H */
