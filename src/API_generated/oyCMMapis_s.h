/** @file oyCMMapis_s.h

   [Template file inheritance graph]
   +-> oyCMMapis_s.template.h
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



#ifndef OY_CMM_APIS_S_H
#define OY_CMM_APIS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapis_s oyCMMapis_s;



#include "oyStruct_s.h"

#include "oyCMMapi_s.h"


/* Include "CMMapis.public.h" { */

/* } Include "CMMapis.public.h" */


/* Include "CMMapis.dox" { */
/** @struct  oyCMMapis_s
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @brief   A CMMapis list
 *
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */

/* } Include "CMMapis.dox" */

struct oyCMMapis_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyCMMapis_s* OYEXPORT
  oyCMMapis_New( oyObject_s object );
OYAPI oyCMMapis_s* OYEXPORT
  oyCMMapis_Copy( oyCMMapis_s *cmmapis, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapis_Release( oyCMMapis_s **cmmapis );

OYAPI int  OYEXPORT
           oyCMMapis_MoveIn          ( oyCMMapis_s       * list,
                                       oyCMMapi_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyCMMapis_ReleaseAt       ( oyCMMapis_s       * list,
                                       int                 pos );
OYAPI oyCMMapi_s * OYEXPORT
           oyCMMapis_Get             ( oyCMMapis_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyCMMapis_Count           ( oyCMMapis_s       * list );
OYAPI int  OYEXPORT
           oyCMMapis_Clear           ( oyCMMapis_s       * list );
OYAPI int  OYEXPORT
           oyCMMapis_Sort            ( oyCMMapis_s       * list,
                                       int32_t           * rank_list );



/* Include "CMMapis.public_methods_declarations.h" { */

/* } Include "CMMapis.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_APIS_S_H */
