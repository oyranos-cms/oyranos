/** @file oyImage_s_.h

   [Template file inheritance graph]
   +-> oyImage_s_.template.h
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


#ifndef OY_IMAGE_S__H
#define OY_IMAGE_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyImagePriv_m( var ) ((oyImage_s_*) (var))

typedef struct oyImage_s_ oyImage_s_;

  
#include <oyranos_object.h>


#include "oyranos_image.h"
#include "oyRectangle_s.h"
#include "oyProfile_s.h"
  

#include "oyImage_s.h"

/* Include "Image.private.h" { */

/* } Include "Image.private.h" */

/** @internal
 *  @struct   oyImage_s_
 *  @brief      A reference struct to gather information for image transformation
 *  @ingroup  objects_image
 *  @extends  oyStruct_s
 */
struct oyImage_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Image.members.h" { */
  oyRectangle_s      * viewport;       /**< intented viewing area, normalised to the pixel width == 1.0 */
  double               resolution_x;   /**< resolution in horizontal direction*/
  double               resolution_y;   /**< resolution in vertical direction */

  oyPixel_t          * layout_;        /**< @private samples mask;
                                            the oyPixel_t pixel_layout variable
                                            passed during oyImage_Create is
                                            stored in position 0 */
  oyCHANNELTYPE_e    * channel_layout; /**< non profile described channels */
  int                  width;          /*!< data width */
  int                  height;         /*!< data height */
  oyOptions_s        * tags;           /**< display_rectangle, display_name ... */
  oyProfile_s        * profile_;       /*!< @private image profile */

  oyStruct_s         * pixel_data;     /**< struct used by each subsequent call of g/set* pixel acessors */
  oyImage_GetPoint_f   getPoint;       /**< the point interface */
  oyImage_GetLine_f    getLine;        /**< the line interface */
  oyImage_GetTile_f    getTile;        /**< the tile interface */
  oyImage_SetPoint_f   setPoint;       /**< the point interface */
  oyImage_SetLine_f    setLine;        /**< the line interface */
  oyImage_SetTile_f    setTile;        /**< the tile interface */
  int                  tile_width;     /**< needed by the tile interface */
  int                  tile_height;    /**< needed by the tile interface */
  uint16_t             subsampling[2]; /**< 1, 2 or 4 */
  int                  sub_positioning;/**< 0 None, 1 Postscript, 2 CCIR 601-1*/
  oyStruct_s         * user_data;      /**< user provided pointer */

/* } Include "Image.members.h" */

};


oyImage_s_*
  oyImage_New_( oyObject_s object );
oyImage_s_*
  oyImage_Copy_( oyImage_s_ *image, oyObject_s object);
oyImage_s_*
  oyImage_Copy__( oyImage_s_ *image, oyObject_s object);
int
  oyImage_Release_( oyImage_s_ **image );



/* Include "Image.private_methods_declarations.h" { */
int oyImage_CombinePixelLayout2Mask_ ( oyImage_s_        * image,
                                       oyPixel_t           pixel_layout );

/* } Include "Image.private_methods_declarations.h" */



void oyImage_Release__Members( oyImage_s_ * image );
int oyImage_Init__Members( oyImage_s_ * image );
int oyImage_Copy__Members( oyImage_s_ * dst, oyImage_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_IMAGE_S__H */
