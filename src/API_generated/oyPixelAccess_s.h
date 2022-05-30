/** @file oyPixelAccess_s.h

   [Template file inheritance graph]
   +-> oyPixelAccess_s.template.h
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



#ifndef OY_PIXEL_ACCESS_S_H
#define OY_PIXEL_ACCESS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyPixelAccess_s oyPixelAccess_s;



#include "oyStruct_s.h"

#include "oyranos_image.h"
#include "oyArray2d_s.h"
#include "oyFilterPlug_s.h"
#include "oyFilterGraph_s.h"
#include "oyImage_s.h"
#include "oyOptions_s.h"
#include "oyRectangle_s.h"


/* Include "PixelAccess.public.h" { */

/* } Include "PixelAccess.public.h" */


/* Include "PixelAccess.dox" { */
/** @struct  oyPixelAccess_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   Control pixel access order
 *
 *  A struct to control pixel access. It is a kind of flexible pixel 
 *  iterator. The order or pattern of access is defined by the [array_xy and]
 *  start_[x,y] variables.
 *
 *  oyPixelAccess_s is like a job ticket. Goal is to maintain all intermediate
 *  and processing dependend memory references in this structure.
 *
 * [The index variable specifies the iterator position in the array_xy index
 *  array.]
 *
 * [pixels_n says how many pixels are to be processed for the cache.
 *  pixels_n is used to calculate the buffers located with getBuffer
 *  and freeBuffer.
 *  The amount of pixel specified in pixels_n must be processed by
 *  each filter, because other filters are relying on a properly filled cache.
 *  This variable also determins the size of the next iteration.]
 *
 * [The relation of pixels_n to array_xy and start_[x,y] is that a
 *  minimum of pixels_n must be processed by starting with start_[x,y]
 *  and processing pixels_n through array_xy. array_xy specifies
 *  the offset pixel distance to a next pixel in x and y directions. In case
 *  pixels_n is larger than array_n the array_xy has to be continued
 *  at array_xy[0,1] after reaching its end (array_n). \n
 *  \b Example: \n
 *  Thus a line iterator behaviour can be specified by simply setting 
 *  array_xy = {1,0}, for a advancement in x direction of one, array_n = 1, 
 *  as we need just this type of advancement and pixels_n = image_width, 
 *  for saying how often the pattern descibed in array_xy has to be applied.]
 *
 *  Handling of pixel access is to be supported by a filter in a function of
 *  type oyCMMFilter_GetNext_f() in oyCMMapi4_s::oyCMMConnector_GetNext().
 *
 *  Access to the buffers by concurrenting threads is handled by passing
 *  different oyPixelAccess_s objects per thread.
 *
 *  From the module point of view it is a requirement to obtain the 
 *  intermediate buffers from somewhere. These are the ones to read from and
 *  to write computed results into. \n
 *
 *  Pixel in- and output buffers separation:
 *  - Copy the output area and request to manipulate it by each filter.
 *    There is no overwriting of results.
 *    Reads a bit fixed. How can filters decide upon the input size?
 *    However, if a filter works on more than one dimension, it can
 *    opt to get its area directly from a input mediator.
 *  -[Provide a opaque output and input area and request to copy by each filter.
 *    Filters would overwrite previous manipulations or some mechanism of
 *    swapping the input with the output side is needed.]
 *  - Some filters want different input and output areas. They see the mediator
 *    as the previous, or the input, element in the graph.
 *  - Will the mediators always be visible in order to get all informations
 *    about the image? During setting up the graph this should be handled.
 *
 *  Access to input and output buffers:
 *  - The output oyArray2d_s is to be reserved only.
 *  - The input oyArray2d_s is to be provided for multi dimensional
 *    manipulators directly from the input mediator.
 *
 *  Thread synchronisation:
 *  - The oyArray2d_s is a opaque memory blob. So different filters can act upon
 *    this resource. It would be in the resposiblity of the graph to avoid
 *    conflicts like using the same output for different manipulations. Given
 *    that the output is acting actively, the potential is small anyway.
 *  - The input should be neutral and not directly manipulated. What can happen
 *    is that different threads request the same input area and the according
 *    data is to be rendered first. So this easily could end in rendering two
 *    times for the same result. Some scheduling in the mediators may help
 *    solving this and improove on performance.
 *
 *  Area dimensions:
 *  - One point is very simple to provide. It may easily require additional
 *    preparations for area manipulations like blur.
 *  - Line is the next hard. The advantage it is still simple and speed
 *    efficient. Programming is a bit more demanding.
 *  - Areas of pixel are easy to provide through oyArray2d_s. It can include the
 *    above two cases.
 *  - Pattern accessors are very flexible for manipulators. It's not clear how
 *    the resulting complexity of translating the pattern to a array with known
 *    pixel positions can be hidden from other filters, which need to know about
 *    positions. One strategy would be to use mediators. They can request the
 *    according pixels from previous filters. A function to convert the pattern
 *    to a list of positions should be provided. Very elegant, but probably
 *    better to do later after oyArray2d_s.
 *
 *  Possible strategies are (old text):
 *  - Use mediators to convert between different pixel layouts and areas.
 *    These could cache the record of a successful query. Mediators are Nodes in
 *    the graph. As the graph and thus mediators can be accessed over
 *    concurrenting entries a cache tends to be expensive.
 *  - The oyPixelAccess_s could hold caches instead of mediators. It is the
 *    structure, which is owned by a given thread anyway.
 *    oyPixelAccess_s needs two buffers one for input and one for output.
 *    As the graph is asked to provide the next pixel via a oyPixelAccess_s
 *    struct, this struct must be associated with source and destination 
 *    buffers. The mediator on output has to search through the chain for the 
 *    previous
 *    mediator and ask there for the input buffer. The ouput buffer is provided
 *    by this mediator itself. These two buffers are set as the actual ones for
 *    processing by the normal filters. It must be clear, what is a mediator,
 *    for this scheme to work. As a mediator is reached in the processing graph,
 *    its task is not only to convert between buffers but as well to update the
 *    oyPixelAccess_s struct with the next mediators and its own buffer. Thus
 *    the next inbetween filters can process on their part.
 *    One advantage is that the mediators can pass their buffers to 
 *    oyPixelAccess_s, which are independent to threads and can be shared.
 *  - Each filter obtains a buffer being asked to fill it with the pixels 
 *    positions described in oyPixelAccess_s. A filter is free to create a new
 *    oyPixelAccess_s description and obtain for instance the surounding of the
 *    requested pixels. There is no caching to be expected other than in 
 *    the oyPixelAccess_s own output buffer.
 *
 *  @todo clear about pixel buffer copying, how to reach the buffers, thread
 *        synchronisation, simple or complex pixel areas (point, line, area,
 *        pattern )
 *
 *  @verbatim
    Relation of positional parameters:

                start_xy relative to original image width
                   |
             +-----|------------------------------+
    original |     |                              |
    image ---+     |                              |
             |  ---+-----------------------+      |
             |     |                       +---------- output_array
             |     |   +---------------+   |      |
             |     |   |               |   |      |
             |     |   |               |   |      |
             |     |   +------+--------+   |      |
             |     |           \           |      |
             |     +------------\----------+      |
             |                   \                |
             +--------------------\---------------+
                                output_array_roi relative to array width/channels
    @endverbatim
 *
 *  Rectangles are passed in relative units.
 *  start_xy and output_array_roi are of type oyRectangle_s and are defined in
 *  relative units to the enclosing images/array data width. start_xy::x 0.1
 *  means 0.1 x original_image::width .
 *  output_roi::0.3333,0.0,0.333,0.333 in a array of data size 9x3 / 3 channels
 *  means the upper middle pixel.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/09/15
 *  @since   2008/07/04 (Oyranos: 0.1.8)
 */

/* } Include "PixelAccess.dox" */

struct oyPixelAccess_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyPixelAccess_s* OYEXPORT
  oyPixelAccess_New( oyObject_s object );
OYAPI oyPixelAccess_s* OYEXPORT
  oyPixelAccess_Copy( oyPixelAccess_s *pixelaccess, oyObject_s obj );
OYAPI int OYEXPORT
  oyPixelAccess_Release( oyPixelAccess_s **pixelaccess );



/* Include "PixelAccess.public_methods_declarations.h" { */
int                oyPixelAccess_SynchroniseROI (
                                       oyPixelAccess_s   * pixel_access_new,
                                       oyPixelAccess_s   * pixel_access_dst );
int                oyPixelAccess_ChangeRectangle (
                                       oyPixelAccess_s   * pixel_access,
                                       double              start_x,
                                       double              start_y,
                                       oyRectangle_s     * output_rectangle );
int                oyPixelAccess_ArrayIsFocussed (
                                       oyPixelAccess_s   * pixel_access );
oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterPlug_s    * plug,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object );
oyImage_s *        oyPixelAccess_GetOutputImage (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetOutputImage (
                                       oyPixelAccess_s   * pixel_access,
                                       oyImage_s         * image );
oyFilterGraph_s *  oyPixelAccess_GetGraph (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetFromString (
                                       oyPixelAccess_s   * ticket,
                                       const char        * key,
                                       const char        * value,
                                       int                 flags );
oyOptions_s *      oyPixelAccess_GetRequestQueue (
                                       oyPixelAccess_s   * pixel_access );
oyArray2d_s *      oyPixelAccess_GetArray (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetArray (
                                       oyPixelAccess_s   * pixel_access,
                                       oyArray2d_s       * array,
                                       int                 has_roi_focus );
int                oyPixelAccess_SetArrayFocus (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 undo );
oyRectangle_s *    oyPixelAccess_GetArrayROI (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_RoiToPixels (
                                       oyPixelAccess_s   * pixel_access,
                                       oyRectangle_s     * roi,
                                       oyRectangle_s    ** pixel_rectangle );
int                oyPixelAccess_PixelsToRoi (
                                       oyPixelAccess_s   * pixel_access,
                                       oyRectangle_s     * pixel_rectangle,
                                       oyRectangle_s    ** roi );
oyStruct_s *       oyPixelAccess_GetUserData (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetUserData (
                                       oyPixelAccess_s   * pixel_access,
                                       oyStruct_s        * user_data );
int32_t            oyPixelAccess_GetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access,
                                       int32_t             workspace_id );
double             oyPixelAccess_GetStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical );
double             oyPixelAccess_GetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical );
int                oyPixelAccess_SetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical,
                                       double              start );
const char *       oyPixelAccess_Show( oyPixelAccess_s   * pixel_access );

/* } Include "PixelAccess.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PIXEL_ACCESS_S_H */
