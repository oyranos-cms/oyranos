/** @file oyFilterNodes_s.h

   [Template file inheritance graph]
   +-> oyFilterNodes_s.template.h
   |
   +-> BaseList_s.h
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



#ifndef OY_FILTER_NODES_S_H
#define OY_FILTER_NODES_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyFilterNodes_s oyFilterNodes_s;



#include "oyStruct_s.h"

#include "oyFilterNode_s.h"


/* Include "FilterNodes.public.h" { */

/* } Include "FilterNodes.public.h" */


/* Include "FilterNodes.dox" { */
/** @struct  oyFilterNodes_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   A FilterNode list
 *  @internal
 *
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */

/* } Include "FilterNodes.dox" */

struct oyFilterNodes_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyFilterNodes_s* OYEXPORT
  oyFilterNodes_New( oyObject_s object );
OYAPI oyFilterNodes_s* OYEXPORT
  oyFilterNodes_Copy( oyFilterNodes_s *filternodes, oyObject_s obj );
OYAPI int OYEXPORT
  oyFilterNodes_Release( oyFilterNodes_s **filternodes );

OYAPI int  OYEXPORT
           oyFilterNodes_MoveIn          ( oyFilterNodes_s       * list,
                                       oyFilterNode_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyFilterNodes_ReleaseAt       ( oyFilterNodes_s       * list,
                                       int                 pos );
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterNodes_Get             ( oyFilterNodes_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyFilterNodes_Count           ( oyFilterNodes_s       * list );
OYAPI int  OYEXPORT
           oyFilterNodes_Clear           ( oyFilterNodes_s       * list );
OYAPI int  OYEXPORT
           oyFilterNodes_Sort            ( oyFilterNodes_s       * list,
                                       int32_t           * rank_list );



/* Include "FilterNodes.public_methods_declarations.h" { */

/* } Include "FilterNodes.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_NODES_S_H */
