/** @file oyArray2d_s.h

   [Template file inheritance graph]
   +-> oyArray2d_s.template.h
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



#ifndef OY_ARRAY2D_S_H
#define OY_ARRAY2D_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyArray2d_s oyArray2d_s;



#include "oyStruct_s.h"

#include "oyranos_image.h"
#include "oyRectangle_s.h"


/* Include "Array2d.public.h" { */

/* } Include "Array2d.public.h" */


/* Include "Array2d.dox" { */
/** @struct  oyArray2d_s
 *  @ingroup objects_image
 *  @extends oyStruct_s
 *  @brief   2d data array
 *
 *  oyArray2d_s is a in memory data view. The array2d holds pointers to lines in
 *  the original memory blob. The arrays contained in array2d represent the 
 *  samples. There is no information in which order the samples appear. No pixel
 *  layout or meaning is provided. Given the coordinates x and y, a samples 
 *  memory adress can be accessed by &array2d[y][x] . This adress must be
 *  converted to the data type provided in oyArray2d_s::t.
 *
 *  The oyArray2d_s::data pointer should be observed in order to be signaled
 *  about its invalidation.
 *
  \dot
  digraph a {
  bgcolor="transparent";
  nodesep=.05;
  rankdir=LR
      node [shape=record,fontname=Helvetica, fontsize=10, width=.1,height=.1];

      e [ label="oyArray2d_s with 8 samples x 10 lines", shape=plaintext];

      y [ label="<0>0|<1>1|<2>2|<3>3|<4>4|<5>5|<6>6|<7>7|<8>8|<9>9", height=2.0 , style=filled ];
      node [width = 1.5];
      0 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      1 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      2 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      3 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      4 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      5 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      6 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      7 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      8 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      9 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      e
      y:0 -> 0:0
      y:1 -> 1:0
      y:2 -> 2:0
      y:3 -> 3:0
      y:4 -> 4:0
      y:5 -> 5:0
      y:6 -> 6:0
      y:7 -> 7:0
      y:8 -> 8:0
      y:9 -> 9:0
      0:7 -> 1:0 [arrowhead="open", style="dashed"];
      1:7 -> 2:0 [arrowhead="open", style="dashed"];
  }
  \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */

/* } Include "Array2d.dox" */

struct oyArray2d_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyArray2d_s* OYEXPORT
  oyArray2d_New( oyObject_s object );
OYAPI oyArray2d_s* OYEXPORT
  oyArray2d_Copy( oyArray2d_s *array2d, oyObject_s obj );
OYAPI int OYEXPORT
  oyArray2d_Release( oyArray2d_s **array2d );



/* Include "Array2d.public_methods_declarations.h" { */
OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        type,
                                       oyObject_s          object );
OYAPI int OYEXPORT
                 oyArray2d_Reset     ( oyArray2d_s       * array,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type );
OYAPI oyPointer  OYEXPORT
                 oyArray2d_GetData   ( oyArray2d_s       * obj );
OYAPI int  OYEXPORT
                 oyArray2d_SetData   ( oyArray2d_s       * obj,
                                       oyPointer           data );
OYAPI int  OYEXPORT
                 oyArray2d_SetRows   ( oyArray2d_s       * obj,
                                       oyPointer         * rows,
                                       int                 do_copy );
OYAPI int  OYEXPORT  oyArray2d_SetFocus (
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * rectangle );
OYAPI double  OYEXPORT
                 oyArray2d_GetDataGeo1(oyArray2d_s       * obj,
                                       int                 x_y_w_h );
OYAPI int  OYEXPORT
                 oyArray2d_GetWidth  ( oyArray2d_s       * obj );
OYAPI int  OYEXPORT
                 oyArray2d_GetHeight ( oyArray2d_s       * obj );
OYAPI oyDATATYPE_e  OYEXPORT
                 oyArray2d_GetType   ( oyArray2d_s       * array );
OYAPI const char *  OYEXPORT
                 oyArray2d_Show      ( oyArray2d_s       * array,
                                       int                 channels );

/* } Include "Array2d.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_ARRAY2D_S_H */
