/** @file oyCMMapiFilters_s_.h

   [Template file inheritance graph]
   +-> oyCMMapiFilters_s_.template.h
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


#ifndef OY_CMM_API_FILTERS_S__H
#define OY_CMM_API_FILTERS_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapiFiltersPriv_m( var ) ((oyCMMapiFilters_s_*) (var))

typedef struct oyCMMapiFilters_s_ oyCMMapiFilters_s_;

  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyCMMapiFilters_s.h"

/* Include "CMMapiFilters.private.h" { */

/* } Include "CMMapiFilters.private.h" */

/** @internal
 *  @struct   oyCMMapiFilters_s_
 *  @brief      A CMMapiFilters list
 *  @ingroup  module_api
 *  @extends  oyStruct_s
 */
struct oyCMMapiFilters_s_ {

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



oyCMMapiFilters_s_*
  oyCMMapiFilters_New_( oyObject_s object );
oyCMMapiFilters_s_*
  oyCMMapiFilters_Copy_( oyCMMapiFilters_s_ *cmmapifilters, oyObject_s object);
oyCMMapiFilters_s_*
  oyCMMapiFilters_Copy__( oyCMMapiFilters_s_ *cmmapifilters, oyObject_s object);
int
  oyCMMapiFilters_Release_( oyCMMapiFilters_s_ **cmmapifilters );

#if 0
/* FIXME Are these needed at all?? */
int
           oyCMMapiFilters_MoveIn_         ( oyCMMapiFilters_s_      * list,
                                       oyCMMapiFilter_s       ** ptr,
                                       int                 pos );
int
           oyCMMapiFilters_ReleaseAt_      ( oyCMMapiFilters_s_      * list,
                                       int                 pos );
oyCMMapiFilter_s *
           oyCMMapiFilters_Get_            ( oyCMMapiFilters_s_      * list,
                                       int                 pos );
int
           oyCMMapiFilters_Count_          ( oyCMMapiFilters_s_      * list );
#endif



/* Include "CMMapiFilters.private_methods_declarations.h" { */

/* } Include "CMMapiFilters.private_methods_declarations.h" */




#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API_FILTERS_S__H */
