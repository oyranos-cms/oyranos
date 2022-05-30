/** @file oyStructList_s_.h

   [Template file inheritance graph]
   +-> oyStructList_s_.template.h
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


#ifndef OY_STRUCT_LIST_S__H
#define OY_STRUCT_LIST_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyStructListPriv_m( var ) ((oyStructList_s_*) (var))

typedef struct oyStructList_s_ oyStructList_s_;

  
#include <oyranos_object.h>
  

#include "oyStructList_s.h"

/* Include "StructList.private.h" { */

/* } Include "StructList.private.h" */

/** @internal
 *  @struct   oyStructList_s_
 *  @brief      a pointer list
 *  @ingroup  objects_generic
 *  @extends  oyStruct_s
 */
struct oyStructList_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "StructList.members.h" { */
oyStruct_s        ** ptr_;           /**< @private the list data */
int                  n_;             /**< @private the number of visible pointers */
int                  n_reserved_;    /**< @private the number of allocated pointers */
char               * list_name;      /**< name of list */
oyOBJECT_e           parent_type_;   /**< @private parents struct type */

/* } Include "StructList.members.h" */

};


oyStructList_s_*
  oyStructList_New_( oyObject_s object );
oyStructList_s_*
  oyStructList_Copy_( oyStructList_s_ *structlist, oyObject_s object);
oyStructList_s_*
  oyStructList_Copy__( oyStructList_s_ *structlist, oyObject_s object);
int
  oyStructList_Release_( oyStructList_s_ **structlist );



/* Include "StructList.private_methods_declarations.h" { */
oyStruct_s *     oyStructList_Get_   ( oyStructList_s_   * list,
                                       int                 pos );
int              oyStructList_ReferenceAt_(oyStructList_s_ * list,
                                           int               pos );
oyStruct_s *     oyStructList_GetType_(oyStructList_s_   * list,
                                       int                 pos,
                                       oyOBJECT_e          type );

/* } Include "StructList.private_methods_declarations.h" */



void oyStructList_Release__Members( oyStructList_s_ * structlist );
int oyStructList_Init__Members( oyStructList_s_ * structlist );
int oyStructList_Copy__Members( oyStructList_s_ * dst, oyStructList_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_STRUCT_LIST_S__H */
