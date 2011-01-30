/** @file oyHash_s_.h

   [Template file inheritance graph]
   +-> Hash_s_.template.h
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
 *  @since    2011/01/30
 */


#ifndef OY_HASH_S__H
#define OY_HASH_S__H

#define oyHashPriv_m( var ) ((oyHash_s_*) (var))

  
#include <oyranos_object.h>
  

#include "oyHash_s.h"

typedef struct oyHash_s_ oyHash_s_;
/* Include "Hash.private.h" { */

/* } Include "Hash.private.h" */

/** @internal
 *  @struct   oyHash_s_
 *  @brief      A cache entry
 *  @ingroup  objects_generic
 *  @extends  oyStruct_s
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
struct oyHash_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */



/* Include "Hash.members.h" { */
  oyStruct_s         * entry;          /**< holds a pointer to something */

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


#endif /* OY_HASH_S__H */
