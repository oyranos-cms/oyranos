/** @file oyPixelAccess_s.c

   [Template file inheritance graph]
   +-> oyPixelAccess_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyPixelAccess_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyPixelAccess_s_.h"

#include "oyRectangle_s_.h"
#include "oyFilterSocket_s_.h"
#include "oyFilterPlug_s_.h"
  


/** Function oyPixelAccess_New
 *  @memberof oyPixelAccess_s
 *  @brief   allocate a new PixelAccess object
 */
OYAPI oyPixelAccess_s * OYEXPORT
  oyPixelAccess_New( oyObject_s object )
{
  oyObject_s s = object;
  oyPixelAccess_s_ * pixelaccess = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  pixelaccess = oyPixelAccess_New_( s );

  return (oyPixelAccess_s*) pixelaccess;
}

/** Function  oyPixelAccess_Copy
 *  @memberof oyPixelAccess_s
 *  @brief    Copy or Reference a PixelAccess object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     pixelaccess                 PixelAccess struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyPixelAccess_s* OYEXPORT
  oyPixelAccess_Copy( oyPixelAccess_s *pixelaccess, oyObject_s object )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*) pixelaccess;

  if(s)
  {
    oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return NULL )
  }
  else
    return NULL;

  s = oyPixelAccess_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyPixelAccess_s" );

  return (oyPixelAccess_s*) s;
}
 
/** Function oyPixelAccess_Release
 *  @memberof oyPixelAccess_s
 *  @brief   release and possibly deallocate a oyPixelAccess_s object
 *
 *  @param[in,out] pixelaccess                 PixelAccess struct object
 */
OYAPI int OYEXPORT
  oyPixelAccess_Release( oyPixelAccess_s **pixelaccess )
{
  oyPixelAccess_s_ * s = 0;

  if(!pixelaccess || !*pixelaccess)
    return 0;

  s = (oyPixelAccess_s_*) *pixelaccess;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  *pixelaccess = 0;

  return oyPixelAccess_Release_( &s );
}



/* Include "PixelAccess.public_methods_definitions.c" { */
/** Function  oyPixelAccess_ChangeRectangle
 *  @memberof oyPixelAccess_s
 *  @brief    Change the ticket for a conversion graph
 *
 *  @param[in,out] pixel_access        optional pixel iterator configuration
 *  @param[in]     start_x             x position relative to virtual source
 *                                     image
 *  @param[in]     start_y             y position relative to virtual source
 *                                     image
 *  @param[in]     output_rectangle    the region in the output image; optional
 *  @return                            0 on success, else error
 *
 *  The output_rectangle is absolute to oyPixelAccess_GetArray().
 *  A passed in output_rectangle will call
 *  oyPixelAccess_SetArrayFocus( pixel_access(::array), undo=1 ).
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/09/26
 *  @since   2011/04/17 (Oyranos: 0.3.0)
 */
int                oyPixelAccess_ChangeRectangle (
                                       oyPixelAccess_s   * pixel_access,
                                       double              start_x,
                                       double              start_y,
                                       oyRectangle_s     * output_rectangle )
{
  oyPixelAccess_s_ * pixel_access_ = (oyPixelAccess_s_*)pixel_access;
  int error = 0;

  if(!pixel_access)
    error = 1;

  if(error <= 0 && output_rectangle)
  {
    oyRectangle_SetByRectangle( (oyRectangle_s*)pixel_access_->output_array_roi,
                                output_rectangle );
    error = oyPixelAccess_SetArrayFocus( pixel_access, 1 );
  }

  if(error <= 0)
  {
    {
      oyImage_s * image = pixel_access_->output_image;
      int channels = oyImage_GetPixelLayout( image, oyCHANS );
      double pixels = start_x * oyImage_GetWidth( image );
      int val = OY_ROUNDf( pixels );
      double diff = (pixels - val) * channels;
      if(diff > 0.5)
      {
        error = -1;
        oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)pixel_access,
                         OY_DBG_FORMAT_ "sub pixel access is pretty uncommon: %s x:%d/%g diff:%g %dc", OY_DBG_ARGS_,
                         oyPixelAccess_Show(pixel_access), val, pixels, diff, channels );
      }
    }

    pixel_access_->start_xy[0] = start_x;
    pixel_access_->start_xy[1] = start_y;
  }

  return error;
}

/** Function  oyPixelAccess_ArrayIsFocussed
 *  @memberof oyPixelAccess_s
 *  @brief    Tell if array has changed focus as of changed output_rectangle
 *
 *  @param[in]     pixel_access        pixel iterator configuration
 *  @return                            0 - array needs focusing
 *                                     1 - array has focus set from array_roi
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/09/14
 *  @since   2016/09/14 (Oyranos: 0.9.6)
 */
int                oyPixelAccess_ArrayIsFocussed (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * pixel_access_ = (oyPixelAccess_s_*)pixel_access;

  if(!pixel_access)
    return 0;

  return pixel_access_->output_array_is_focussed;
}


/** Function  oyPixelAccess_Create
 *  @memberof oyPixelAccess_s
 *  @brief    Allocate and initialise a basic oyPixelAccess_s object
 *
 *  @code
  // conversion->out_ has to be linear, as we access only the first plug
  node = oyConversion_GetNode( conversion, OY_OUTPUT );
  plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterNode_Release( &node );

  // create a very simple pixel iterator
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                         oyPIXEL_ACCESS_IMAGE, 0 );
    @endcode
 *
 *  @see oyPixelAccess_SetOutputImage() and oyPixelAccess_SetArray()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2009/06/10
 */
oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterPlug_s    * plug,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)oyPixelAccess_New( object );
  oyFilterSocket_s_ * sock = 0;
  oyFilterPlug_s_ ** plug_ = (oyFilterPlug_s_**)&plug;
  int error = !s || !plug || !(*plug_)->remote_socket_;
  int w = 0;
  oyImage_s * image = 0;
  int32_t n = 0;

  if(error <= 0)
  {
    sock = (*plug_)->remote_socket_;
    image = (oyImage_s*)sock->data;

    s->start_xy[0] = s->start_xy_old[0] = start_x;
    s->start_xy[1] = s->start_xy_old[1] = start_y;

    /* make shure the filter->image_ is set, e.g.
       error = oyFilterCore_ImageSet ( filter, image );

    s->data_in = filter->image_->data; */
    if(image)
      w = oyImage_GetWidth( image );

    /** The filters have no obligation to pass end to end informations.
        The ticket must hold all pices of interesst.
     */
    s->output_array_roi->width = 1.0;
    if(image && w)
      s->output_array_roi->height = oyImage_GetHeight( image ) / (double)w;
    s->output_image = oyImage_Copy( image, 0 );
    s->graph = (oyFilterGraph_s_*)oyFilterGraph_FromNode( (oyFilterNode_s*)sock->node, 0 );

    if(type == oyPIXEL_ACCESS_POINT)
    {
      s->array_xy = s->oy_->allocateFunc_(sizeof(int32_t) * 2);
      s->array_xy[0] = s->array_xy[1] = 0;
      s->array_n = 1;
      s->pixels_n = 1;
    } else
    if(type == oyPIXEL_ACCESS_LINE)
    {
      s->array_xy = s->oy_->allocateFunc_(sizeof(int32_t) * 2);
      /* set relative advancements from one pixel to the next */
      s->array_xy[0] = 1;
      s->array_xy[1] = 0;
      s->array_n = 1;
      s->pixels_n = w;       /* the total we want */
    } else
    /* if(type == oyPIXEL_ACCESS_IMAGE) */
    {
      /** @todo how can we know about the various module capabilities
       *  - back report the processed number of pixels in the passed pointer
       *  - restrict for a line interface only, would fit to oyArray2d_s
       *  - + handle inside an to be created function oyConversion_RunPixels()
       */
    }

    /* Copy requests, which where attached to the node, to the ticket. */
    if((*plug_)->node->core->options_)
      error = oyOptions_Filter( &s->request_queue, &n, 0,
                                oyBOOLEAN_INTERSECTION,
                                "////resolve", (*plug_)->node->core->options_ );
  }

  if(error)
    oyPixelAccess_Release ( (oyPixelAccess_s**)&s );

  return (oyPixelAccess_s*)s;
}

/** Function  oyPixelAccess_GetOutputImage
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::output_image
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyImage_s *        oyPixelAccess_GetOutputImage (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  oyImage_Copy( s->output_image, 0 );
  return s->output_image;
}
/** Function  oyPixelAccess_SetOutputImage
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::output_image
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/04
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetOutputImage (
                                       oyPixelAccess_s   * pixel_access,
                                       oyImage_s         * image )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  oyImage_Release( &s->output_image );
  s->output_image = oyImage_Copy( image, 0 );

  return 0;
}
/** Function  oyPixelAccess_SynchroniseROI
 *  @memberof oyPixelAccess_s
 *  @brief    Set a new ticket according to old ticket geometries
 *
 *  After a new ticket is generated, the old geometries might become invalid,
 *  as channel counts change or the array sizes becomes different. This
 *  function convinently changes the ROI and start_xy dimensions after such
 *  critical changes.
 *
 *  @param[in,out] pixel_access_new    to be modified pixel iterator configuration
 *  @param[in]     pixel_access_src    pixel iterator configuration pattern
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/03
 *  @since   2016/03/29 (Oyranos: 0.9.6)
 */
int                oyPixelAccess_SynchroniseROI (
                                       oyPixelAccess_s   * pixel_access_new,
                                       oyPixelAccess_s   * pixel_access_src )
{
  int error = 0;

  if(!pixel_access_src || !pixel_access_new)
    error = 1;

  if(!error)
  {
    oyImage_s * image_src = oyPixelAccess_GetOutputImage( pixel_access_src ),
              * image_dst = oyPixelAccess_GetOutputImage( pixel_access_new );
    int image_width_src = oyImage_GetWidth( image_src ),
        image_width_dst = oyImage_GetWidth( image_dst );

    /* start_xy is defined relative to the tickets output image width */
    double start_x_pixel = oyPixelAccess_GetStart( pixel_access_src, 0 ) * image_width_src,
           start_y_pixel = oyPixelAccess_GetStart( pixel_access_src, 1 ) * image_width_src;

    oyRectangle_s * roi_pix = NULL, * roi = NULL;

    if(!image_width_dst) image_width_dst = 1; /* avoid division by zero */

    oyPixelAccess_RoiToPixels( pixel_access_src, NULL, &roi_pix );
    oyPixelAccess_PixelsToRoi( pixel_access_new, roi_pix, &roi );
    
    if(oy_debug)
      oyMessageFunc_p( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)pixel_access_src, OY_DBG_FORMAT_
                       "pixel_access_src: %s",OY_DBG_ARGS_,
                       oyPixelAccess_Show(pixel_access_src));

    oyPixelAccess_ChangeRectangle( pixel_access_new,
                          start_x_pixel / image_width_dst,
                          start_y_pixel / image_width_dst, roi );


    if(oy_debug)
      oyMessageFunc_p( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)pixel_access_src, OY_DBG_FORMAT_
                       "pixel_access_new: %s",OY_DBG_ARGS_,
                       oyPixelAccess_Show(pixel_access_new));

    oyRectangle_Release( &roi ); oyRectangle_Release( &roi_pix );
    oyImage_Release( &image_src ); oyImage_Release( &image_dst );
  }

  return error;
}

/** Function  oyPixelAccess_SetArrayFocus
 *  @memberof oyPixelAccess_s
 *  @brief    Ensure that the array is in output ROI focus
 *
 *  The output_array_roi is a absolute rectangle. The unit is relative
 *  to a existing oyPixelAccess_GetArray() or to the output image.
 *  Undoing will change the oyPixelAccess_GetArray() back to it's data
 *  area (oyArray2d_GetDataGeo1()).
 *
 *  After calling this function the oyPixelAccess_GetArray() obtained
 *  array might be changed through oyArray2d_SetFocus().
 *
 *  @param[in,out] pixel_access        pixel iterator configuration
 *  @param         undo                undo a previously possibly set focus
 *  @return                            0 - no action, -1 - focus changed, 1 - error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/09/14
 *  @since   2016/03/14 (Oyranos: 0.9.6)
 */
int                oyPixelAccess_SetArrayFocus (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 undo )
{
  int error = 0;

  if(pixel_access)
  {
    oyArray2d_s * array = oyPixelAccess_GetArray( pixel_access );

    if(array)
    {
      oyRectangle_s_ r_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
      oyRectangle_s * r = (oyRectangle_s *) &r_,
                    * r_samples = oyRectangle_New(0);

      if(undo == 0 && !oyPixelAccess_ArrayIsFocussed(pixel_access))
      {
        /* set array focus for simple plug-ins */
        oyImage_s * image = oyPixelAccess_GetOutputImage( pixel_access );
        int channels = oyImage_GetPixelLayout( image, oyCHANS );
        if(!channels) channels = 1; /* avoid division by zero */

        /* convert roi to channel units */
        oyPixelAccess_RoiToPixels( pixel_access, 0, &r );
        /* scale horizontal for pixel -> channels */
        oyImage_PixelsToSamples( image, r, r_samples );
        /* finally set the focus for simple plug-ins */
        error = oyArray2d_SetFocus( array, r_samples );
        ((oyPixelAccess_s_*)pixel_access)->output_array_is_focussed = 1;

        if(oy_debug >=3 || error > 0)
        {
          char *  t = oyStringCopy( oyRectangle_Show( r ), oyAllocateFunc_ );
          oyMessageFunc_p( error ? oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)pixel_access,
                           OY_DBG_FORMAT_ "%cset focus: %s %s", OY_DBG_ARGS_,
                           error == -1?'*':' ', t, oyArray2d_Show(array,channels) );
          oyFree_m_(t);
        }

        {
          double pixels = r_.x;
          int val = pixels;
          double diff = (pixels - val) * channels;
          if(diff > 0.5)
          {
            error = -1;
        oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)pixel_access,
                         OY_DBG_FORMAT_ "sub pixel access is pretty uncommon: %s x:%d/%g diff:%g %dc", OY_DBG_ARGS_,
                         oyPixelAccess_Show(pixel_access), val, pixels, diff, channels );
          }
        }

        oyImage_Release( &image );

      } else
      if(undo && oyPixelAccess_ArrayIsFocussed(pixel_access))
      {
        r_.width = oyArray2d_GetDataGeo1( array, 2 );
        r_.height = oyArray2d_GetDataGeo1( array, 3 );
        error = oyArray2d_SetFocus( array, r );
        ((oyPixelAccess_s_*)pixel_access)->output_array_is_focussed = 0;

        if(oy_debug >=3 || error > 0)
          oyMessageFunc_p( error ? oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)pixel_access,
                           OY_DBG_FORMAT_ "%cunset focus to: %s", OY_DBG_ARGS_,
                           error == -1?'*':' ', oyRectangle_Show(r) );
      }
      oyRectangle_Release( &r_samples );
    }

    oyArray2d_Release( &array );
  }

  return error;
}

/** Function  oyPixelAccess_Show
 *  @memberof oyPixelAccess_s
 *  @brief    Print ticket geometries
 *
 *  This function is not thread safe.
 *
 *  @param[in]     pixel_access        pixel iterator configuration
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/04
 *  @since   2016/04/04 (Oyranos: 0.9.6)
 */
const char *       oyPixelAccess_Show( oyPixelAccess_s   * pixel_access )
{
  char * t = NULL;
  const char * text = NULL;
  oyAlloc_f alloc;
  oyDeAlloc_f dealloc;

  if(!pixel_access)
    return "";

  alloc = oyObject_GetAlloc( pixel_access->oy_ );
  dealloc = oyObject_GetDeAlloc( pixel_access->oy_ );

  {
    oyPixelAccess_s * ticket = pixel_access;
    oyFilterGraph_s * ticket_graph = oyPixelAccess_GetGraph( ticket );
    oyOptions_s * ticket_graph_opts = oyFilterGraph_GetOptions( ticket_graph );
    oyImage_s * image = oyPixelAccess_GetOutputImage( ticket );
    int image_width = oyImage_GetWidth( image );
    oyRectangle_s * ticket_array_roi = oyPixelAccess_GetArrayROI( ticket );
    oyArray2d_s * a = oyPixelAccess_GetArray( ticket );
    int a_is_focussed = oyPixelAccess_ArrayIsFocussed( ticket );
    oyRectangle_s_  r = {oyOBJECT_RECTANGLE_S, 0,0,0, 0,0,0,0};
    oyRectangle_s * roi = (oyRectangle_s*)&r;

    int dirty = oyOptions_FindString( ticket_graph_opts, "dirty", "true")  ? 1 : 0;
    oyFilterGraph_Release( &ticket_graph );
    oyOptions_Release( &ticket_graph_opts );

    /* start_xy is defined relative to the tickets output image width */
    double start_x_pixel = oyPixelAccess_GetStart( ticket, 0 ) * (double)image_width,
           start_y_pixel = oyPixelAccess_GetStart( ticket, 1 ) * (double)image_width;
    int layout = oyImage_GetPixelLayout( image, oyLAYOUT );
    int channels = oyToChannels_m( layout );
    int a_width;

    if(!channels) channels = 1; /* avoid division by zero */
    a_width = oyArray2d_GetDataGeo1( a, 2 ) / channels;

    oyRectangle_SetByRectangle( roi, ticket_array_roi );
    oyRectangle_Scale( roi, a_width?a_width:image_width );
    oyStringAddPrintf( &t, alloc,dealloc,
                "ticket[%d] start_xy %g|%g %s[%d](%dx%d)%dc ROI: %s %c%s dirty: %d",
                oyStruct_GetId((oyStruct_s*)ticket),
                start_x_pixel, start_y_pixel, _("Image"),
                oyStruct_GetId((oyStruct_s*)image),image_width,oyImage_GetHeight(image),channels,
                oyRectangle_Show( roi ),
                a_is_focussed?' ':'~', oyArray2d_Show( a, channels ), dirty );

    oyObject_SetName( pixel_access->oy_, t, oyNAME_NAME );
    if(t) dealloc(t);
    text = oyObject_GetName( pixel_access->oy_, oyNAME_NAME );

    oyImage_Release( &image );
    oyArray2d_Release( &a );
    oyRectangle_Release( &ticket_array_roi );
  }

  return (text&&text[0])?text:"----";
}

/** Function  oyPixelAccess_GetArrayROI
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::output_array_roi
 *
 *  The rectangle unit is normalised to array::width/channels.
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/03/26
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyRectangle_s *    oyPixelAccess_GetArrayROI (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  oyRectangle_Copy( (oyRectangle_s*)s->output_array_roi, 0 );
  return (oyRectangle_s*)s->output_array_roi;
}

/** Function  oyPixelAccess_RoiToPixels
 *  @memberof oyPixelAccess_s
 *  @brief    Calculate pixel rectangle from ROI
 *
 *  @see oyPixelAccess_PixelsToRoi()
 *
 *  @param[in]     pixel_access        pixel iterator configuration
 *  @param[in]     roi                 alternative ROI in 
 *                                     array::width/channels == 1.0 unit;
 *                                     optional,
 *                                     default: is pixel_access::output_array_roi
 *  @param[in,out] pixel_rectangle     rectangle for pixel results in pixels
 *  @return                            error
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/09/27
 *  @since    2016/09/27 (Oyranos: 0.9.6)
 */
int                oyPixelAccess_RoiToPixels (
                                       oyPixelAccess_s   * pixel_access,
                                       oyRectangle_s     * roi,
                                       oyRectangle_s    ** pixel_rectangle )
{
  int error = !pixel_access;
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!error && pixel_access->type_ != oyOBJECT_PIXEL_ACCESS_S)
    return 0;

  if(!error)
  {
    if(!*pixel_rectangle)
      *pixel_rectangle = oyRectangle_New(0);

    if(!roi)
      roi = (oyRectangle_s*)s->output_array_roi;

    if(oyRectangle_CountPoints( roi ) == 0 )
    {
      oyRectangle_SetGeo( *pixel_rectangle, 0,0, oyImage_GetWidth(s->output_image),
                                                oyImage_GetHeight(s->output_image) );

    } else
    {
      int pixel_width;
      int channels = oyImage_GetPixelLayout( s->output_image, oyCHANS );
      if(!channels) channels = 1; /* avoid division by zero */

      oyRectangle_SetByRectangle( *pixel_rectangle, roi );

      if(s->array)
        pixel_width = oyArray2d_GetDataGeo1( s->array, 2 ) / channels;
      else
        pixel_width = oyImage_GetWidth( s->output_image );

      oyRectangle_Scale( *pixel_rectangle, pixel_width );
      oyRectangle_Round( *pixel_rectangle );
    }
  }

  return error;
}

/** Function  oyPixelAccess_PixelsToRoi
 *  @memberof oyPixelAccess_s
 *  @brief    Calculate ROI rectangle from pixel rectangle
 *
 *  For obtaining the oyPixelAccess_s::output_array_roi see
 *  oyPixelAccess_GetArrayROI().
 *
 *  @see oyPixelAccess_RoiToPixels()
 *
 *  @param[in]     pixel_access        pixel iterator configuration
 *  @param[in]     pixel_rectangle     pixel rectangle; optional 
 *                                     - scale pixel_rectangle -> roi or
 *                                     - use output_array_roi > 0 or
 *                                     - fall back to scaled
 *                                       (array::data_area -> pixel)
 *                                     - fall back to scaled output_image size
 *  @param[in,out] roi                 returns a rectangle with ROI 
 *  @return                            error
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/09/27
 *  @since    2016/09/27 (Oyranos: 0.9.6)
 */
int                oyPixelAccess_PixelsToRoi (
                                       oyPixelAccess_s   * pixel_access,
                                       oyRectangle_s     * pixel_rectangle,
                                       oyRectangle_s    ** roi )
{
  int error = !pixel_access;
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!error && pixel_access->type_ != oyOBJECT_PIXEL_ACCESS_S)
    return 0;

  if(!error)
  {
    int pixel_width;
    int channels = oyImage_GetPixelLayout( s->output_image, oyCHANS );
    if(!channels) channels = 1; /* avoid division by zero */

    if(s->array)
      pixel_width = oyArray2d_GetDataGeo1( s->array, 2 ) / channels;
    else
      pixel_width = oyImage_GetWidth( s->output_image );

    if(!*roi)
      *roi = oyRectangle_New(0);

    if( pixel_rectangle == NULL )
    {
      if(s->output_array_roi &&
         oyRectangle_CountPoints( (oyRectangle_s*)s->output_array_roi ) > 0)
        oyRectangle_SetByRectangle( *roi, (oyRectangle_s*)s->output_array_roi );
      else
      if(s->array)
        oyRectangle_SetGeo( *roi,
                            oyArray2d_GetDataGeo1( s->array, 0 ) / channels,
                            oyArray2d_GetDataGeo1( s->array, 1 ),
                            oyArray2d_GetDataGeo1( s->array, 2 ) / channels,
                            oyArray2d_GetDataGeo1( s->array, 3 ) );
      else
        oyRectangle_SetGeo( *roi, 0,0, oyImage_GetWidth(s->output_image),
                                      oyImage_GetHeight(s->output_image) );

    } else
    {
      oyRectangle_SetByRectangle( *roi, pixel_rectangle );
      oyRectangle_Round( *roi );

      if(pixel_width) /* avoid division by zero */
        oyRectangle_Scale( *roi, 1.0/pixel_width );
    }
  }

  return error;
}

/** Function  oyPixelAccess_GetGraph
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::graph
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyFilterGraph_s *  oyPixelAccess_GetGraph (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  oyFilterGraph_Copy( (oyFilterGraph_s*)s->graph, 0 );
  return (oyFilterGraph_s*)s->graph;
}
/** @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::graph::options
 *  @see      oyOptions_SetFromString()
 *
 *  @version  Oyranos: 0.9.7
 *  @date     2018/05/28
 *  @since    2018/05/28 (Oyranos: 0.9.7)
 */
int      oyPixelAccess_SetFromString ( oyPixelAccess_s   * ticket,
                                       const char        * key,
                                       const char        * value,
                                       int                 flags )
{
  oyFilterGraph_s * ticket_graph = oyPixelAccess_GetGraph( ticket );
  oyOptions_s * ticket_graph_opts = oyFilterGraph_GetOptions( ticket_graph );
  int error = oyOptions_SetFromString( &ticket_graph_opts, key, value, flags );
  oyFilterGraph_Release( &ticket_graph );
  oyOptions_Release( &ticket_graph_opts );
  return error;
}

/** Function  oyPixelAccess_GetRequestQueue
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::request_queue
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyOptions_s *      oyPixelAccess_GetRequestQueue (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  oyOptions_Copy( s->request_queue, 0 );
  return s->request_queue;
}
/** Function  oyPixelAccess_GetArray
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::array
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyArray2d_s *      oyPixelAccess_GetArray (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  oyArray2d_Copy( s->array, 0 );
  return s->array;
}
/** Function  oyPixelAccess_SetArray
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::array
 *
 *  @param[in,out] pixel_access        pixel iterator configuration; optional
 *  @param[in]     array               channel data
 *  @param[in]     has_roi_focus       0 - the supplied array is not yet focused for the pixel_access::output_array_roi
 *                                     1 - the array is already in focus for pixel_access::output_array_roi
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/10/01
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetArray (
                                       oyPixelAccess_s   * pixel_access,
                                       oyArray2d_s       * array,
                                       int                 has_roi_focus )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;
  double old_array_pix_width,
         array_pix_width;
  int channels;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )


  /* adapt to eventually new ROI base unit, which is
   * s->array::data:width/channels */
  channels = oyImage_GetPixelLayout( s->output_image, oyCHANS );
  if(!channels) channels = 1; /* avoid division by zero */

  if(s->array)
    old_array_pix_width = oyArray2d_GetDataGeo1( s->array, 2 ) / channels;
  else
    old_array_pix_width = oyImage_GetWidth( s->output_image );

  if(array)
    array_pix_width = oyArray2d_GetDataGeo1( array, 2 ) / channels;
  else
    array_pix_width = oyImage_GetWidth( s->output_image );

  if(array_pix_width && old_array_pix_width != array_pix_width)
  {
    oyRectangle_Scale( (oyRectangle_s*)s->output_array_roi, old_array_pix_width );
    oyRectangle_Round( (oyRectangle_s*)s->output_array_roi );
    oyRectangle_Scale( (oyRectangle_s*)s->output_array_roi, 1.0/array_pix_width );
  }

  if(s->array != array)
  {
    if(s->array)
      oyArray2d_Release( &s->array );
    s->array = oyArray2d_Copy( array, 0 );
  }

  if(has_roi_focus)
    s->output_array_is_focussed = has_roi_focus;
  else
    oyPixelAccess_SetArrayFocus( pixel_access, has_roi_focus );

  return 0;
}
/** Function  oyPixelAccess_GetUserData
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::user_data
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyStruct_s *       oyPixelAccess_GetUserData (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  if(s->user_data && s->user_data->copy)
  {
    s->user_data = s->user_data->copy( s->user_data, 0 );
    if(oy_debug_objects >= 0 && s->user_data)
      oyObjectDebugMessage_( s->user_data->oy_, __func__,
                             oyStructTypeToText(s->user_data->type_) );
  }

  return s->user_data;
}
/** Function  oyPixelAccess_SetUserData
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::user_data
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetUserData (
                                       oyPixelAccess_s   * pixel_access,
                                       oyStruct_s        * user_data )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  if(s->user_data && s->user_data->release)
    s->user_data->release( &s->user_data );

  if(user_data && user_data->copy)
  {
    s->user_data = user_data->copy( user_data, 0 );
    if(oy_debug_objects >= 0 && s->user_data)
      oyObjectDebugMessage_( s->user_data->oy_, __func__,
                             oyStructTypeToText(s->user_data->type_) );
  } else 
    s->user_data = user_data;

  return 0;
}
/** Function  oyPixelAccess_GetWorkspaceID
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::workspace_id
 *
 *  A ID for distinct resources.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int32_t            oyPixelAccess_GetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  return s->workspace_id;
}
/** Function  oyPixelAccess_SWorkspaceID
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::workspace_id
 *
 *  A ID for distinct resources.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access,
                                       int32_t             workspace_id )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  return s->workspace_id = workspace_id;
}
/** Function  oyPixelAccess_GetStart
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::start_xy
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
double             oyPixelAccess_GetStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  if(vertical == 0)
    return s->start_xy[0];
  else
    return s->start_xy[1];
}
/** Function  oyPixelAccess_GetOldStart
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::start_xy_old
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/10
 *  @since    2012/09/10 (Oyranos: 0.5.0)
 */
double             oyPixelAccess_GetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  if(vertical == 0)
    return s->start_xy_old[0];
  else
    return s->start_xy_old[1];
}
/** Function  oyPixelAccess_SetOldStart
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::start_xy_old
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/10
 *  @since    2012/09/10 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical,
                                       double              start )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  if(vertical == 0)
    s->start_xy_old[0] = start;
  else
    s->start_xy_old[1] = start;
  return 0;
}

/* } Include "PixelAccess.public_methods_definitions.c" */

