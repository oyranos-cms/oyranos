/** @file oyArray2d_s_.h

   [Template file inheritance graph]
   +-> oyArray2d_s_.template.h
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


#ifndef OY_ARRAY2D_S__H
#define OY_ARRAY2D_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyArray2dPriv_m( var ) ((oyArray2d_s_*) (var))

typedef struct oyArray2d_s_ oyArray2d_s_;

  
#include <oyranos_object.h>


#include "oyRectangle_s_.h"
  

#include "oyArray2d_s.h"

/* Include "Array2d.private.h" { */

/* } Include "Array2d.private.h" */

/** @internal
 *  @struct   oyArray2d_s_
 *  @brief      2d data array
 *  @ingroup  objects_image
 *  @extends  oyStruct_s
 */
struct oyArray2d_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Array2d.members.h" { */
  oyDATATYPE_e         t;              /**< data type */
  int                  width;          /**< width of actual data view */
  int                  height;         /**< height of actual data view */
  oyRectangle_s_       data_area;      /**< size of reserve pixels, x,y <= 0, width,height >= data view width,height */

  unsigned char     ** array2d;        /**< sorted data, pointer is always owned
                                            by the object */
  int                  own_lines;      /**< Are *array2d rows owned by object?
                                            - 0 not owned by the object
                                            - 1 one own monolithic memory block
                                                starting in array2d[0]
                                            - 2 several owned memory blocks */
  oyStructList_s     * refs_;          /**< references of other arrays to this*/
  oyArray2d_s        * refered_;       /**< array this one refers to */

/* } Include "Array2d.members.h" */

};


oyArray2d_s_*
  oyArray2d_New_( oyObject_s object );
oyArray2d_s_*
  oyArray2d_Copy_( oyArray2d_s_ *array2d, oyObject_s object);
oyArray2d_s_*
  oyArray2d_Copy__( oyArray2d_s_ *array2d, oyObject_s object);
int
  oyArray2d_Release_( oyArray2d_s_ **array2d );



/* Include "Array2d.private_methods_declarations.h" { */
int                oyArray2d_Init_   ( oyArray2d_s_      * s,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type );
oyArray2d_s_ *
                   oyArray2d_Create_ ( int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object );
int
             oyArray2d_ReleaseArray_ ( oyArray2d_s       * obj );
int              oyArray2d_ToPPM_    ( oyArray2d_s_      * array,
                                       const char        * file_name );

/* } Include "Array2d.private_methods_declarations.h" */



void oyArray2d_Release__Members( oyArray2d_s_ * array2d );
int oyArray2d_Init__Members( oyArray2d_s_ * array2d );
int oyArray2d_Copy__Members( oyArray2d_s_ * dst, oyArray2d_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_ARRAY2D_S__H */
