/** @file oyCMMapis_s_.h

   [Template file inheritance graph]
   +-> oyCMMapis_s_.template.h
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


#ifndef OY_CMM_APIS_S__H
#define OY_CMM_APIS_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapisPriv_m( var ) ((oyCMMapis_s_*) (var))

typedef struct oyCMMapis_s_ oyCMMapis_s_;

  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyCMMapis_s.h"

/* Include "CMMapis.private.h" { */

/* } Include "CMMapis.private.h" */

/** @internal
 *  @struct   oyCMMapis_s_
 *  @brief      A CMMapis list
 *  @ingroup  module_api
 *  @extends  oyStruct_s
 */
struct oyCMMapis_s_ {

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



oyCMMapis_s_*
  oyCMMapis_New_( oyObject_s object );
oyCMMapis_s_*
  oyCMMapis_Copy_( oyCMMapis_s_ *cmmapis, oyObject_s object);
oyCMMapis_s_*
  oyCMMapis_Copy__( oyCMMapis_s_ *cmmapis, oyObject_s object);
int
  oyCMMapis_Release_( oyCMMapis_s_ **cmmapis );

#if 0
/* FIXME Are these needed at all?? */
int
           oyCMMapis_MoveIn_         ( oyCMMapis_s_      * list,
                                       oyCMMapi_s       ** ptr,
                                       int                 pos );
int
           oyCMMapis_ReleaseAt_      ( oyCMMapis_s_      * list,
                                       int                 pos );
oyCMMapi_s *
           oyCMMapis_Get_            ( oyCMMapis_s_      * list,
                                       int                 pos );
int
           oyCMMapis_Count_          ( oyCMMapis_s_      * list );
#endif



/* Include "CMMapis.private_methods_declarations.h" { */

/* } Include "CMMapis.private_methods_declarations.h" */




#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_APIS_S__H */
