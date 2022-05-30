/** @file oyLis_s_.h

   [Template file inheritance graph]
   +-> oyLis_s_.template.h
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


#ifndef OY_LIS_S__H
#define OY_LIS_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyLisPriv_m( var ) ((oyLis_s_*) (var))

typedef struct oyLis_s_ oyLis_s_;

  
#include <oyranos_object.h>
  

#include "oyLis_s.h"

/* Include "Lis.private.h" { */

/* } Include "Lis.private.h" */

/** @internal
 *  @struct   oyLis_s_
 *  @brief      a pointer list
 *  @ingroup  objects_generic
 *  @extends  oyStruct_s
 */
struct oyLis_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Lis.members.h" { */

/* } Include "Lis.members.h" */

};


oyLis_s_*
  oyLis_New_( oyObject_s object );
oyLis_s_*
  oyLis_Copy_( oyLis_s_ *lis, oyObject_s object);
oyLis_s_*
  oyLis_Copy__( oyLis_s_ *lis, oyObject_s object);
int
  oyLis_Release_( oyLis_s_ **lis );



/* Include "Lis.private_methods_declarations.h" { */

/* } Include "Lis.private_methods_declarations.h" */



void oyLis_Release__Members( oyLis_s_ * lis );
int oyLis_Init__Members( oyLis_s_ * lis );
int oyLis_Copy__Members( oyLis_s_ * dst, oyLis_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_LIS_S__H */
