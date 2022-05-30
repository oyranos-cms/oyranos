/** @file oyCMMapi3_s.h

   [Template file inheritance graph]
   +-> oyCMMapi3_s.template.h
   |
   +-> oyCMMapi_s.template.h
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



#ifndef OY_CMM_API3_S_H
#define OY_CMM_API3_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi3_s oyCMMapi3_s;


#include "oyStruct_s.h"


/* Include "CMMapi3.public.h" { */
#include "oyCMMapi_s.h"

/* } Include "CMMapi3.public.h" */


/* Include "CMMapi3.dox" { */
/** @struct  oyCMMapi3_s
 *  @ingroup module_api
 *  @extends oyCMMapi_s
 *  @brief   The API 3 to implement and set to provide low level ICC profile support
 *  @internal
 *  
 *  This API provides weak interface compile time checking.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */

/* } Include "CMMapi3.dox" */

struct oyCMMapi3_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMapi3_s* OYEXPORT
  oyCMMapi3_New( oyObject_s object );
OYAPI oyCMMapi3_s* OYEXPORT
  oyCMMapi3_Copy( oyCMMapi3_s *cmmapi3, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi3_Release( oyCMMapi3_s **cmmapi3 );



/* Include "CMMapi3.public_methods_declarations.h" { */
oyCMMCanHandle_f   oyCMMapi3_GetCanHandleF
                                     ( oyCMMapi3_s       * api );


/* } Include "CMMapi3.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API3_S_H */
