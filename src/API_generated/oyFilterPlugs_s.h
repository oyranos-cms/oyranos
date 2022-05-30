/** @file oyFilterPlugs_s.h

   [Template file inheritance graph]
   +-> oyFilterPlugs_s.template.h
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



#ifndef OY_FILTER_PLUGS_S_H
#define OY_FILTER_PLUGS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyFilterPlugs_s oyFilterPlugs_s;



#include "oyStruct_s.h"

#include "oyFilterPlug_s.h"


/* Include "FilterPlugs.public.h" { */

/* } Include "FilterPlugs.public.h" */


/* Include "FilterPlugs.dox" { */
/** @struct  oyFilterPlugs_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   A FilterPlugs list
 *
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */

/* } Include "FilterPlugs.dox" */

struct oyFilterPlugs_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyFilterPlugs_s* OYEXPORT
  oyFilterPlugs_New( oyObject_s object );
OYAPI oyFilterPlugs_s* OYEXPORT
  oyFilterPlugs_Copy( oyFilterPlugs_s *filterplugs, oyObject_s obj );
OYAPI int OYEXPORT
  oyFilterPlugs_Release( oyFilterPlugs_s **filterplugs );

OYAPI int  OYEXPORT
           oyFilterPlugs_MoveIn          ( oyFilterPlugs_s       * list,
                                       oyFilterPlug_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyFilterPlugs_ReleaseAt       ( oyFilterPlugs_s       * list,
                                       int                 pos );
OYAPI oyFilterPlug_s * OYEXPORT
           oyFilterPlugs_Get             ( oyFilterPlugs_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyFilterPlugs_Count           ( oyFilterPlugs_s       * list );
OYAPI int  OYEXPORT
           oyFilterPlugs_Clear           ( oyFilterPlugs_s       * list );
OYAPI int  OYEXPORT
           oyFilterPlugs_Sort            ( oyFilterPlugs_s       * list,
                                       int32_t           * rank_list );



/* Include "FilterPlugs.public_methods_declarations.h" { */

/* } Include "FilterPlugs.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_PLUGS_S_H */
