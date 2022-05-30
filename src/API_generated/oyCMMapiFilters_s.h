/** @file oyCMMapiFilters_s.h

   [Template file inheritance graph]
   +-> oyCMMapiFilters_s.template.h
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



#ifndef OY_CMM_API_FILTERS_S_H
#define OY_CMM_API_FILTERS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapiFilters_s oyCMMapiFilters_s;



#include "oyStruct_s.h"

#include "oyCMMapiFilter_s.h"


/* Include "CMMapiFilters.public.h" { */

/* } Include "CMMapiFilters.public.h" */


/* Include "CMMapiFilters.dox" { */
/** @struct  oyCMMapiFilters_s
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @brief   A CMMapiFilters list
 *  @internal
 *  
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */

/* } Include "CMMapiFilters.dox" */

struct oyCMMapiFilters_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyCMMapiFilters_s* OYEXPORT
  oyCMMapiFilters_New( oyObject_s object );
OYAPI oyCMMapiFilters_s* OYEXPORT
  oyCMMapiFilters_Copy( oyCMMapiFilters_s *cmmapifilters, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapiFilters_Release( oyCMMapiFilters_s **cmmapifilters );

OYAPI int  OYEXPORT
           oyCMMapiFilters_MoveIn          ( oyCMMapiFilters_s       * list,
                                       oyCMMapiFilter_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyCMMapiFilters_ReleaseAt       ( oyCMMapiFilters_s       * list,
                                       int                 pos );
OYAPI oyCMMapiFilter_s * OYEXPORT
           oyCMMapiFilters_Get             ( oyCMMapiFilters_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyCMMapiFilters_Count           ( oyCMMapiFilters_s       * list );
OYAPI int  OYEXPORT
           oyCMMapiFilters_Clear           ( oyCMMapiFilters_s       * list );
OYAPI int  OYEXPORT
           oyCMMapiFilters_Sort            ( oyCMMapiFilters_s       * list,
                                       int32_t           * rank_list );



/* Include "CMMapiFilters.public_methods_declarations.h" { */

/* } Include "CMMapiFilters.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API_FILTERS_S_H */
