/** @file oyStructList_s_.h

   [Template file inheritance graph]
   +-> StructList_s_.template.h
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
 *  @since    2011/01/27
 */


#ifndef OY_STRUCT_LIST_S__H
#define OY_STRUCT_LIST_S__H

#define oyStructListPriv_m( var ) ((oyStructList_s_*) (var))

  
#include <oyranos_object.h>
  

#include "oyStructList_s.h"

typedef struct oyStructList_s_ oyStructList_s_;
/* Include "StructList.private.h" { */

/* } Include "StructList.private.h" */

/** @internal
 *  @struct   oyStructList_s_
 *  @brief      a pointer list
 *  @ingroup  objects_generic
 *  @extends  oyStruct_s
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
struct oyStructList_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

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
/*oyStruct_s **    oyStructList_GetRaw_( oyStructList_s    * list );*/
oyStruct_s *     oyStructList_Get_   ( oyStructList_s    * list,
                                       int                 pos );
int              oyStructList_ReferenceAt_(oyStructList_s* list,
                                       int                 pos );


/* } Include "StructList.private_methods_declarations.h" */



void oyStructList_Release__Members( oyStructList_s_ * structlist );
int oyStructList_Init__Members( oyStructList_s_ * structlist );
int oyStructList_Copy__Members( oyStructList_s_ * dst, oyStructList_s_ * src);


#endif /* OY_STRUCT_LIST_S__H */
