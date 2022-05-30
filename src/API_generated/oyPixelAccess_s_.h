/** @file oyPixelAccess_s_.h

   [Template file inheritance graph]
   +-> oyPixelAccess_s_.template.h
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


#ifndef OY_PIXEL_ACCESS_S__H
#define OY_PIXEL_ACCESS_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyPixelAccessPriv_m( var ) ((oyPixelAccess_s_*) (var))

typedef struct oyPixelAccess_s_ oyPixelAccess_s_;

  
#include <oyranos_object.h>


#include "oyFilterGraph_s_.h"
#include "oyRectangle_s_.h"
  

#include "oyPixelAccess_s.h"

/* Include "PixelAccess.private.h" { */

/* } Include "PixelAccess.private.h" */

/** @internal
 *  @struct   oyPixelAccess_s_
 *  @brief      Control pixel access order
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyPixelAccess_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "PixelAccess.members.h" { */
  double           start_xy[2];        /**< the start point of output_image */
  double           start_xy_old[2];    /**< @deprecated the previous start point */
  int32_t        * array_xy;           /**< @deprecated array of shifts, e.g. 1,0,2,0,1,0 */
  int              array_n;            /**< @deprecated the number of points in array_xy */

  int              index;              /**< @deprecated to be advanced by the last caller */
  size_t           pixels_n;           /**< @deprecated pixels to process/cache at once; should be set to 0 or 1 */

  int32_t          workspace_id;       /**< a ID to assign distinct resources to */
  oyStruct_s     * user_data;          /**< user data, e.g. for error messages*/
  oyArray2d_s    * array;              /**< processing data. The position is in
                                            start_xy relative to the previous
                                            mediator in the graph. */
  oyRectangle_s_ * output_array_roi;   /**< rectangle of interesst; The
                                            rectangle is to be seen in relation
                                            to the tickets array::width/channels
                                            .*/
  int              output_array_is_focussed; /**< remember if
                                            oyArray2d_SetFocus() needs to be applied 
                                            from output_array_roi to array */
  oyImage_s      * output_image;       /**< the image which issued the request*/
  oyFilterGraph_s_ * graph;            /**< the graph to process */
  oyOptions_s    * request_queue;      /**< messaging; requests to resolve */

/* } Include "PixelAccess.members.h" */

};


oyPixelAccess_s_*
  oyPixelAccess_New_( oyObject_s object );
oyPixelAccess_s_*
  oyPixelAccess_Copy_( oyPixelAccess_s_ *pixelaccess, oyObject_s object);
oyPixelAccess_s_*
  oyPixelAccess_Copy__( oyPixelAccess_s_ *pixelaccess, oyObject_s object);
int
  oyPixelAccess_Release_( oyPixelAccess_s_ **pixelaccess );



/* Include "PixelAccess.private_methods_declarations.h" { */

/* } Include "PixelAccess.private_methods_declarations.h" */



void oyPixelAccess_Release__Members( oyPixelAccess_s_ * pixelaccess );
int oyPixelAccess_Init__Members( oyPixelAccess_s_ * pixelaccess );
int oyPixelAccess_Copy__Members( oyPixelAccess_s_ * dst, oyPixelAccess_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PIXEL_ACCESS_S__H */
