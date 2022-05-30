/** @file oyFilterNodes_s_.h

   [Template file inheritance graph]
   +-> oyFilterNodes_s_.template.h
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


#ifndef OY_FILTER_NODES_S__H
#define OY_FILTER_NODES_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyFilterNodesPriv_m( var ) ((oyFilterNodes_s_*) (var))

typedef struct oyFilterNodes_s_ oyFilterNodes_s_;

  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyFilterNodes_s.h"

/* Include "FilterNodes.private.h" { */

/* } Include "FilterNodes.private.h" */

/** @internal
 *  @struct   oyFilterNodes_s_
 *  @brief      A FilterNode list
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyFilterNodes_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */


/* Include "BaseList_s_.h" { */
  oyStructList_s * list_;              /**< the list data */
/* } Include "BaseList_s_.h" */

};



oyFilterNodes_s_*
  oyFilterNodes_New_( oyObject_s object );
oyFilterNodes_s_*
  oyFilterNodes_Copy_( oyFilterNodes_s_ *filternodes, oyObject_s object);
oyFilterNodes_s_*
  oyFilterNodes_Copy__( oyFilterNodes_s_ *filternodes, oyObject_s object);
int
  oyFilterNodes_Release_( oyFilterNodes_s_ **filternodes );

#if 0
/* FIXME Are these needed at all?? */
int
           oyFilterNodes_MoveIn_         ( oyFilterNodes_s_      * list,
                                       oyFilterNode_s       ** ptr,
                                       int                 pos );
int
           oyFilterNodes_ReleaseAt_      ( oyFilterNodes_s_      * list,
                                       int                 pos );
oyFilterNode_s *
           oyFilterNodes_Get_            ( oyFilterNodes_s_      * list,
                                       int                 pos );
int
           oyFilterNodes_Count_          ( oyFilterNodes_s_      * list );
#endif



/* Include "FilterNodes.private_methods_declarations.h" { */

/* } Include "FilterNodes.private_methods_declarations.h" */




#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_NODES_S__H */
