/** @file oyList_s.h

   [Template file inheritance graph]
   +-> oyList_s.template.h
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



#ifndef OY_LIST_S_H
#define OY_LIST_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyList_s oyList_s;



#include "oyStruct_s.h"

#include "oyLis_s.h"


/* Include "List.public.h" { */

/* } Include "List.public.h" */


/* Include "List.dox" { */
/** @struct oyList_s
 *  @ingroup objects_generic
 *  @extends oyStruct_s
 *  @brief   a pointer list
 *  @internal
 *  
 *  @version Oyranos: x.x.x
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */

/* } Include "List.dox" */

struct oyList_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyList_s* OYEXPORT
  oyList_New( oyObject_s object );
OYAPI oyList_s* OYEXPORT
  oyList_Copy( oyList_s *list, oyObject_s obj );
OYAPI int OYEXPORT
  oyList_Release( oyList_s **list );

OYAPI int  OYEXPORT
           oyList_MoveIn          ( oyList_s       * list,
                                       oyLis_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyList_ReleaseAt       ( oyList_s       * list,
                                       int                 pos );
OYAPI oyLis_s * OYEXPORT
           oyList_Get             ( oyList_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyList_Count           ( oyList_s       * list );
OYAPI int  OYEXPORT
           oyList_Clear           ( oyList_s       * list );
OYAPI int  OYEXPORT
           oyList_Sort            ( oyList_s       * list,
                                       int32_t           * rank_list );



/* Include "List.public_methods_declarations.h" { */

/* } Include "List.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_LIST_S_H */
