/** @file oyConversion_s_.h

   [Template file inheritance graph]
   +-> oyConversion_s_.template.h
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


#ifndef OY_CONVERSION_S__H
#define OY_CONVERSION_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyConversionPriv_m( var ) ((oyConversion_s_*) (var))

typedef struct oyConversion_s_ oyConversion_s_;

  
#include <oyranos_object.h>


#include "oyFilterNode_s.h"
#include "oyFilterNode_s_.h"
  

#include "oyConversion_s.h"

/* Include "Conversion.private.h" { */

/* } Include "Conversion.private.h" */

/** @internal
 *  @struct   oyConversion_s_
 *  @brief      Image Manipulation by a Graph (DAG)
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyConversion_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Conversion.members.h" { */
  oyFilterNode_s_    * input;          /**< the input image filter; Most users will start logically with this pice and chain their filters to get the final result. */
  oyFilterNode_s_    * out_;           /**< @private the Oyranos output image. Oyranos will stream the filters starting from the end. This element will be asked on its first plug. */

/* } Include "Conversion.members.h" */

};


oyConversion_s_*
  oyConversion_New_( oyObject_s object );
oyConversion_s_*
  oyConversion_Copy_( oyConversion_s_ *conversion, oyObject_s object);
oyConversion_s_*
  oyConversion_Copy__( oyConversion_s_ *conversion, oyObject_s object);
int
  oyConversion_Release_( oyConversion_s_ **conversion );



/* Include "Conversion.private_methods_declarations.h" { */

/* } Include "Conversion.private_methods_declarations.h" */



void oyConversion_Release__Members( oyConversion_s_ * conversion );
int oyConversion_Init__Members( oyConversion_s_ * conversion );
int oyConversion_Copy__Members( oyConversion_s_ * dst, oyConversion_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONVERSION_S__H */
