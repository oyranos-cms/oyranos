/** @file oyImage_s.h

   [Template file inheritance graph]
   +-> oyImage_s.template.h
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



#ifndef OY_IMAGE_S_H
#define OY_IMAGE_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyImage_s oyImage_s;



#include "oyStruct_s.h"

#include "oyranos_image.h"
#include "oyProfile_s.h"
#include "oyRectangle_s.h"
#include "oyArray2d_s.h"


/* Include "Image.public.h" { */
/**
 *  Typedef   oyImage_GetPoint_f
 *  @memberof oyImage_s
 *  @brief    pixel accessor
 *
 *  @param[in,out] image                 the image object
 *  @param[in]     point_x               the data location in x direction
 *  @param[in]     point_y               the data location in y direction
 *  @param[in]     channel               the nth channel will is requested
 *                                       -1 means to request all channels, note
 *                                       that can be an expensive call for non 
 *                                       continous pixel layouts
 *  @param[out]    is_allocated          are the points always newly allocated?
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
typedef oyPointer (*oyImage_GetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the lines always newly allocated? */
typedef oyPointer (*oyImage_GetLine_f) ( oyImage_s       * image,
                                         int               line_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the tiles always newly allocated? */
typedef oyPointer*(*oyImage_GetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         int             * is_allocated );
/**
 *  Typedef   oyImage_SetPoint_f
 *  @memberof oyImage_s
 *  @brief    pixel setter
 *
 *  @param[in,out] image                 the image object
 *  @param[in]     point_x               the data location in x direction
 *  @param[in]     point_y               the data location in y direction
 *  @param[in]     channel               the nth channel will is requested
 *                                       -1 means to request all channels, note
 *                                       that can be an expensive call for non 
 *                                       continous pixel layouts
 *  @param[out]    data                  the data to be copied into the image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
typedef int       (*oyImage_SetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetLine_f) ( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               pixel_n,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         oyPointer         data );

/* } Include "Image.public.h" */


/* Include "Image.dox" { */
/** @struct  oyImage_s
 *  @ingroup objects_image
 *  @extends oyStruct_s
 *  @brief   A reference struct to gather information for image transformation
 *
 *  as we don't target a complete imaging solution, only raster is supported
 *
 *  Resolution is in pixel per centimeter.
 *
 *  Requirements: \n
 *  - to provide a view on the image data we look at per line arrays
 *  - it should be possible to echange the to be processed data without altering
 *    the context
 *  - oyImage_s should hold image dimensions,
 *  - display rectangle information and
 *  - a reference to the data for conversion
 *
 *  Create a basic image with the pixel owned by the caller use oyImage_Create().
 *
 *  To set a image data module use oyImage_SetData().
 *  \dot
 digraph oyImage_s {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record,fontname=Helvetica, fontsize=10, width=.1];

  subgraph cluster_3 {
    label="oyImage_s data modules";
    color=white;
    clusterrank=global;

      i [ label="... | <0>oyStruct_s * pixel | <1> oyImage_GetPoint_f getPoint | <2>oyImage_GetLine_f getLine | <3>oyImage_GetTile_f getTile | ..."];

      node [width = 2.5, style=filled];
      pixel_A [label="oyArray2d_s arrayA"];
      gp_p_A [label="Array2d_GetPointA"];
      gp_l_A [label="Array2d_GetLineA"];
      gp_t_A [label="Array2d_GetTileA"];

      pixel_B [label="mmap arrayB"];
      gp_p_B [label="mmap_GetPointB"];
      gp_l_B [label="mmap_GetLineB"];
      gp_t_B [label="mmap_GetTileB"];

      subgraph cluster_0 {
        rank=max;
        color=red;
        style=dashed;
        node [style="filled"];
        pixel_A; gp_p_A; gp_l_A; gp_t_A;
        //pixel_A -> gp_p_A -> gp_l_A -> gp_t_A [color=white, arrowhead=none, dirtype=none];
        label="module A";
      }

      subgraph cluster_1 {
        color=blue;
        style=dashed;
        node [style="filled"];
        pixel_B; gp_p_B; gp_l_B; gp_t_B;
        label="module B";
      }

      subgraph cluster_2 {
        color=gray;
        node [style="filled"];
        i;
        label="oyImage_s";
        URL="structoyImage__s.html";
      }

      i:0 -> pixel_A [arrowhead="open", color=red];
      i:1 -> gp_p_A [arrowhead="open", color=red];
      i:2 -> gp_l_A [arrowhead="open", color=red];
      i:3 -> gp_t_A [arrowhead="open", color=red];
      i:0 -> pixel_B [arrowhead="open", color=blue];
      i:1 -> gp_p_B [arrowhead="open", color=blue];
      i:2 -> gp_l_B [arrowhead="open", color=blue];
      i:3 -> gp_t_B [arrowhead="open", color=blue];
  }
 }
 \enddot
 *
 *  Should oyImage_s become internal and we provide a user interface?
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */

/* } Include "Image.dox" */

struct oyImage_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyImage_s* OYEXPORT
  oyImage_New( oyObject_s object );
OYAPI oyImage_s* OYEXPORT
  oyImage_Copy( oyImage_s *image, oyObject_s obj );
OYAPI int OYEXPORT
  oyImage_Release( oyImage_s **image );



/* Include "Image.public_methods_declarations.h" { */
int            oyImage_GetWidth      ( oyImage_s         * image );
int            oyImage_GetHeight     ( oyImage_s         * image );
int            oyImage_GetPixelLayout( oyImage_s         * image,
                                       oyLAYOUT_e          type );
oyCHANNELTYPE_e  oyImage_GetChannelType (
                                       oyImage_s         * image,
                                       int                 pos );
int            oyImage_GetSubPositioning (
                                       oyImage_s         * image );
oyOptions_s *  oyImage_GetTags       ( oyImage_s         * image );
oyProfile_s *  oyImage_GetProfile    ( oyImage_s         * image );
oyStruct_s *   oyImage_GetPixelData  ( oyImage_s         * image );
oyImage_GetPoint_f oyImage_GetPointF ( oyImage_s         * image );
oyImage_GetLine_f oyImage_GetLineF   ( oyImage_s         * image );
oyImage_SetPoint_f oyImage_GetSetPointF (
                                       oyImage_s         * image );
oyImage_SetLine_f oyImage_GetSetLineF( oyImage_s         * image );
oyStruct_s *   oyImage_GetUserData   ( oyImage_s         * image );
oyImage_s *    oyImage_Create        ( int                 width,
                                       int                 height,
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyObject_s          object);
oyImage_s *    oyImage_CreateForDisplay ( int              width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       const char        * display_name,
                                       int                 window_pos_x,
                                       int                 window_pos_y,
                                       int                 window_width,
                                       int                 window_height,
                                       int                 icc_profile_flags,
                                       oyObject_s          object);
int            oyImage_FromFile      ( const char        * file_name,
                                       int                 icc_profile_flags,
                                       oyImage_s        ** image,
                                       oyObject_s          object );
int            oyImage_ToFile        ( oyImage_s         * image,
                                       const char        * file_name,
                                       oyOptions_s       * opts );
int            oyImage_SetCritical   ( oyImage_s         * image,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyOptions_s       * tags,
                                       int                 width,
                                       int                 height );
int            oyImage_SetData       ( oyImage_s         * image,
                                       oyStruct_s       ** pixel_data,
                                       oyImage_GetPoint_f  getPoint,
                                       oyImage_GetLine_f   getLine,
                                       oyImage_GetTile_f   getTile,
                                       oyImage_SetPoint_f  setPoint,
                                       oyImage_SetLine_f   setLine,
                                       oyImage_SetTile_f   setTile );
int            oyImage_FillArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       int                 do_copy,
                                       oyArray2d_s      ** array,
                                       oyRectangle_s     * array_rectangle,
                                       oyObject_s          obj );
int            oyImage_ReadArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * array_rectangle );
int            oyImage_RoiToSamples  ( oyImage_s         * image,
                                       oyRectangle_s     * roi,
                                       oyRectangle_s    ** sample_rectangle );
int            oyImage_SamplesToRoi  ( oyImage_s         * image,
                                       oyRectangle_s     * sample_rectangle,
                                       oyRectangle_s    ** roi );
int            oyImage_SamplesToPixels(oyImage_s         * image,
                                       oyRectangle_s     * sample_rectangle,
                                       oyRectangle_s     * pixel_rectangle );
int            oyImage_PixelsToSamples(oyImage_s         * image,
                                       oyRectangle_s     * pixel_rectangle,
                                       oyRectangle_s     * sample_rectangle );
int            oyImage_WritePPM      ( oyImage_s         * image,
                                       const char        * file_name,
                                       const char        * comment );

/* } Include "Image.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_IMAGE_S_H */
