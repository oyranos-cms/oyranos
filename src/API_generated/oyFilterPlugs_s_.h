/** @file oyFilterPlugs_s_.h

   [Template file inheritance graph]
   +-> oyFilterPlugs_s_.template.h
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


#ifndef OY_FILTER_PLUGS_S__H
#define OY_FILTER_PLUGS_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyFilterPlugsPriv_m( var ) ((oyFilterPlugs_s_*) (var))

typedef struct oyFilterPlugs_s_ oyFilterPlugs_s_;

  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyFilterPlugs_s.h"

/* Include "FilterPlugs.private.h" { */

/* } Include "FilterPlugs.private.h" */

/** @internal
 *  @struct   oyFilterPlugs_s_
 *  @brief      A FilterPlugs list
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyFilterPlugs_s_ {

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



oyFilterPlugs_s_*
  oyFilterPlugs_New_( oyObject_s object );
oyFilterPlugs_s_*
  oyFilterPlugs_Copy_( oyFilterPlugs_s_ *filterplugs, oyObject_s object);
oyFilterPlugs_s_*
  oyFilterPlugs_Copy__( oyFilterPlugs_s_ *filterplugs, oyObject_s object);
int
  oyFilterPlugs_Release_( oyFilterPlugs_s_ **filterplugs );

#if 0
/* FIXME Are these needed at all?? */
int
           oyFilterPlugs_MoveIn_         ( oyFilterPlugs_s_      * list,
                                       oyFilterPlug_s       ** ptr,
                                       int                 pos );
int
           oyFilterPlugs_ReleaseAt_      ( oyFilterPlugs_s_      * list,
                                       int                 pos );
oyFilterPlug_s *
           oyFilterPlugs_Get_            ( oyFilterPlugs_s_      * list,
                                       int                 pos );
int
           oyFilterPlugs_Count_          ( oyFilterPlugs_s_      * list );
#endif



/* Include "FilterPlugs.private_methods_declarations.h" { */

/* } Include "FilterPlugs.private_methods_declarations.h" */




#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_PLUGS_S__H */
