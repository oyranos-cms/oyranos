/** @file oyList_s_.h

   [Template file inheritance graph]
   +-> oyList_s_.template.h
   |
   +-> BaseList_s_.h
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


#ifndef OY_LIST_S__H
#define OY_LIST_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyListPriv_m( var ) ((oyList_s_*) (var))

typedef struct oyList_s_ oyList_s_;



  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyList_s.h"

/* Include "List.private.h" { */

/* } Include "List.private.h" */

/** @internal
 *  @struct   oyList_s_
 *  @brief      a pointer list
 *  @ingroup  objects_generic
 *  @extends  oyStruct_s
 */
struct oyList_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "BaseList_s_.h" { */
  oyStructList_s * list_;              /**< the list data */
/* } Include "BaseList_s_.h" */

/* Include "List.members.h" { */

/* } Include "List.members.h" */

};



oyList_s_*
  oyList_New_( oyObject_s object );
oyList_s_*
  oyList_Copy_( oyList_s_ *list, oyObject_s object);
oyList_s_*
  oyList_Copy__( oyList_s_ *list, oyObject_s object);
int
  oyList_Release_( oyList_s_ **list );

#if 0
/* FIXME Are these needed at all?? */
int
           oyList_MoveIn_         ( oyList_s_      * list,
                                              ** ptr,
                                       int                 pos );
int
           oyList_ReleaseAt_      ( oyList_s_      * list,
                                       int                 pos );
 *
           oyList_Get_            ( oyList_s_      * list,
                                       int                 pos );
int
           oyList_Count_          ( oyList_s_      * list );
#endif



/* Include "List.private_methods_declarations.h" { */

/* } Include "List.private_methods_declarations.h" */



void oyList_Release__Members( oyList_s_ * list );
int oyList_Init__Members( oyList_s_ * list );
int oyList_Copy__Members( oyList_s_ * dst, oyList_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_LIST_S__H */
