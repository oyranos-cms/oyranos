/** @file oyRectangle_s_.h

   [Template file inheritance graph]
   +-> oyRectangle_s_.template.h
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


#ifndef OY_RECTANGLE_S__H
#define OY_RECTANGLE_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyRectanglePriv_m( var ) ((oyRectangle_s_*) (var))

typedef struct oyRectangle_s_ oyRectangle_s_;

  
#include <oyranos_object.h>
  

#include "oyRectangle_s.h"

/* Include "Rectangle.private.h" { */

/* } Include "Rectangle.private.h" */

/** @internal
 *  @struct   oyRectangle_s_
 *  @brief      A simple rectangle of double values
 *  @ingroup  objects_image
 *  @extends  oyStruct_s
 */
struct oyRectangle_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Rectangle.members.h" { */
  double x;
  double y;
  double width;
  double height;

/* } Include "Rectangle.members.h" */

};


oyRectangle_s_*
  oyRectangle_New_( oyObject_s object );
oyRectangle_s_*
  oyRectangle_Copy_( oyRectangle_s_ *rectangle, oyObject_s object);
oyRectangle_s_*
  oyRectangle_Copy__( oyRectangle_s_ *rectangle, oyObject_s object);
int
  oyRectangle_Release_( oyRectangle_s_ **rectangle );



/* Include "Rectangle.private_methods_declarations.h" { */

/* } Include "Rectangle.private_methods_declarations.h" */



void oyRectangle_Release__Members( oyRectangle_s_ * rectangle );
int oyRectangle_Init__Members( oyRectangle_s_ * rectangle );
int oyRectangle_Copy__Members( oyRectangle_s_ * dst, oyRectangle_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_RECTANGLE_S__H */
