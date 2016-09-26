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
    oyPixelAccess_SetArrayFocus( pixel_access, 1 );
  }

  if(error <= 0)
  {
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
 *  @brief    Allocate iand initialise a basic oyPixelAccess_s object
 *
 *  @code
  // conversion->out_ has to be linear, so we access only the first plug
  node = oyConversion_GetNode( conversion, OY_OUTPUT );
  plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterNode_Release( &node );

  // create a very simple pixel iterator
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                         oyPIXEL_ACCESS_IMAGE, 0 );
@endcode
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
    if(image)
      s->output_array_roi->height = oyImage_GetHeight( image ) / (double)oyImage_GetWidth( image );
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
 *  @date    2016/03/29
 *  @since   2016/03/29 (Oyranos: 0.9.6)
 */
int                oyPixelAccess_SynchroniseROI (
                                       oyPixelAccess_s   * pixel_access_new,
                                       oyPixelAccess_s   * pixel_access_dst )
{
  int error = 0;

  if(!pixel_access_dst || !pixel_access_new)
    error = 1;

  if(!error)
  {
    oyPixelAccess_s * ticket = pixel_access_dst,
                    * new_ticket = pixel_access_new;
    oyImage_s * image_src = oyPixelAccess_GetOutputImage( new_ticket ),
              * image_dst = oyPixelAccess_GetOutputImage( ticket );
    int image_width_src = oyImage_GetWidth( image_src ),
        image_width_dst = oyImage_GetWidth( image_dst );
    oyRectangle_s * ticket_array_roi_src = oyPixelAccess_GetArrayROI( new_ticket ),
                  * ticket_array_roi_dst = oyPixelAccess_GetArrayROI( ticket );
    oyArray2d_s * a_dst = oyPixelAccess_GetArray( ticket ),
                * a_src = oyPixelAccess_GetArray( new_ticket );

    /* start_xy is defined relative to the tickets output image width */
    double start_x_dst_pixel = oyPixelAccess_GetStart( ticket, 0 ) * image_width_dst,
           start_y_dst_pixel = oyPixelAccess_GetStart( ticket, 1 ) * image_width_dst;
    int layout_src = oyImage_GetPixelLayout( image_src, oyLAYOUT ),
        layout_dst = oyImage_GetPixelLayout( image_dst, oyLAYOUT );
    int channels_src = oyToChannels_m( layout_src );
    int channels_dst = oyToChannels_m( layout_dst );
    int a_width_dst = 0, a_width_src = 0;

    if(channels_dst)
      a_width_dst = oyArray2d_GetDataGeo1( a_dst, 2 ) / channels_dst;
    if(channels_src)
      a_width_src = oyArray2d_GetDataGeo1( a_src,2 ) / channels_src;

    /** 1. Ignore any changes from previous edits of the new pixel access ticket. */
    oyRectangle_SetByRectangle( ticket_array_roi_src, ticket_array_roi_dst );

    /** 2. Adapt the access start and write relative to new tickets image width. */
    if(image_width_src)
      oyPixelAccess_ChangeRectangle( new_ticket,
                          start_x_dst_pixel / image_width_src,
                          start_y_dst_pixel / image_width_src, 0 );

    /** 3. And use the available source image area */
      /** 3.1. Convert ROI to old array pixel. */
    oyRectangle_Scale( ticket_array_roi_src, a_width_dst );

    if(oy_debug)
    {
      oyRectangle_s_  r = {oyOBJECT_RECTANGLE_S, 0,0,0, 0,0,0,0};
      oyRectangle_s * roi = (oyRectangle_s*)&r;
      char * t;
      oyRectangle_SetByRectangle( roi, ticket_array_roi_dst );
      oyRectangle_Scale( roi, a_width_dst );
      t = oyStringCopy( oyRectangle_Show( roi ), oyAllocateFunc_ );
      oyMessageFunc_p( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_
              "new_ticket[%d] start_xy %f|%f ROI: image[%d](%s)%dc a[%d](%dx%d) <- [%d](%s)%dc a[%d](%dx%d)\n",OY_DBG_ARGS_,
                   oyStruct_GetId((oyStruct_s*)new_ticket),
                   start_x_dst_pixel, start_y_dst_pixel,
                   oyStruct_GetId((oyStruct_s*)image_dst),t,channels_dst,
                   oyStruct_GetId((oyStruct_s*)a_dst),oyArray2d_GetWidth(a_dst),oyArray2d_GetHeight(a_dst),
                   oyStruct_GetId((oyStruct_s*)image_src),
                   oyRectangle_Show(roi),channels_src,
                   oyStruct_GetId((oyStruct_s*)a_src),oyArray2d_GetWidth(a_src),oyArray2d_GetHeight(a_src) );
      oyFree_m_(t);
    }

      /** 3.2. Divide ROI by new array size. */
    if(a_width_src)
      oyRectangle_Scale( ticket_array_roi_src, 1.0/a_width_src );

    oyImage_Release( &image_src );
    oyImage_Release( &image_dst );
    oyArray2d_Release( &a_src );
    oyArray2d_Release( &a_dst );
    oyRectangle_Release( &ticket_array_roi_dst );
    oyRectangle_Release( &ticket_array_roi_src );
  }

  return error;
}

/** Function  oyPixelAccess_ChannelRectFromROI
 *  @memberof oyPixelAccess_s
 *  @brief    Obtain channel geometry from output ROI
 *
 *  @param[in]     pixel_access        pixel iterator configuration
 *  @return                            rectangle
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/09/14
 *  @since   2016/03/14 (Oyranos: 0.9.6)
 */
oyRectangle_s *    oyPixelAccess_ChannelRectFromROI (
                                       oyPixelAccess_s   * pixel_access )
{
  int error = 0;
  oyRectangle_s * output_channel_roi = NULL;

  if(!pixel_access)
    error = 1;

  if(!error)
  {
    oyPixelAccess_s * ticket = pixel_access;
    oyImage_s * image = oyPixelAccess_GetOutputImage( ticket );
    int image_width = oyImage_GetWidth( image );
    oyRectangle_s * ticket_array_roi = oyPixelAccess_GetArrayROI( ticket );
    oyArray2d_s * a = oyPixelAccess_GetArray( ticket );
    oyRectangle_s_  r = {oyOBJECT_RECTANGLE_S, 0,0,0, 0,0,0,0};
    oyRectangle_s * roi = (oyRectangle_s*)&r;

    int layout = oyImage_GetPixelLayout( image, oyLAYOUT );
    int channels = oyToChannels_m( layout );
    int a_width = oyArray2d_GetDataGeo1( a, 2 ) / channels;

    oyRectangle_SetByRectangle( roi, ticket_array_roi );
    oyRectangle_Scale( roi, a_width?a_width:image_width );

    output_channel_roi = oyRectangle_NewFrom( roi, NULL );

    oyImage_Release( &image );
    oyArray2d_Release( &a );
    oyRectangle_Release( &ticket_array_roi );
  }

  return output_channel_roi;
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
      oyRectangle_s * r = (oyRectangle_s *) &r_;

      if(undo == 0 && !oyPixelAccess_ArrayIsFocussed(pixel_access))
      {
        /* set array focus for simple plug-ins */
        oyImage_s * image = oyPixelAccess_GetOutputImage( pixel_access );
        oyRectangle_s * ticket_array_roi = oyPixelAccess_GetArrayROI(
                                                          pixel_access );

        int layout = oyImage_GetPixelLayout( image, oyLAYOUT );
        int channels = oyToChannels_m( layout );

        int array_pix_width = oyArray2d_GetDataGeo1( array, 2 ) / channels;

        /* convert roi to channel units */
        oyRectangle_SetByRectangle( r, ticket_array_roi );
        oyRectangle_Scale( r, array_pix_width );
        /* scale horicontal for pixel -> channels */
        *oyRectangle_SetGeo1( r, 0 ) *= channels;
        *oyRectangle_SetGeo1( r, 2 ) *= channels;
        /* finally set the focus for simple plug-ins */
        error = oyArray2d_SetFocus( array, r );
        ((oyPixelAccess_s_*)pixel_access)->output_array_is_focussed = 1;

        DBGs_PROG2_S( pixel_access, "%cset focus: %s", error == -1?'*':' ',
                      oyRectangle_Show(r));

        oyImage_Release( &image );
        oyRectangle_Release( &ticket_array_roi );

      } else
      if(undo && oyPixelAccess_ArrayIsFocussed(pixel_access))
      {
        r_.width = oyArray2d_GetDataGeo1( array, 2 );
        r_.height = oyArray2d_GetDataGeo1( array, 3 );
        error = oyArray2d_SetFocus( array, r );
        ((oyPixelAccess_s_*)pixel_access)->output_array_is_focussed = 0;

        DBGs_PROG2_S( pixel_access, "%cunset focus to: %s", error == -1?'*':' ',
                      oyRectangle_Show(r));
      }
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
  int error = 0;
  static char * t = NULL;

  if(!t) t = malloc(1024);

  if(!pixel_access || !t)
    error = 1;

  if(!error)
  {
    oyPixelAccess_s * ticket = pixel_access;
    oyImage_s * image = oyPixelAccess_GetOutputImage( ticket );
    int image_width = oyImage_GetWidth( image );
    oyRectangle_s * ticket_array_roi = oyPixelAccess_GetArrayROI( ticket );
    oyArray2d_s * a = oyPixelAccess_GetArray( ticket );
    oyRectangle_s_  r = {oyOBJECT_RECTANGLE_S, 0,0,0, 0,0,0,0};
    oyRectangle_s * roi = (oyRectangle_s*)&r;

    /* start_xy is defined relative to the tickets output image width */
    double start_x_pixel = oyPixelAccess_GetStart( ticket, 0 ) * image_width,
           start_y_pixel = oyPixelAccess_GetStart( ticket, 1 ) * image_width;
    int layout = oyImage_GetPixelLayout( image, oyLAYOUT );
    int channels = oyToChannels_m( layout );
    int a_width = oyArray2d_GetDataGeo1( a, 2 ) / channels;

    oyRectangle_SetByRectangle( roi, ticket_array_roi );
    oyRectangle_Scale( roi, a_width?a_width:image_width );
    oySprintf_( t,
                "ticket[%d] start_xy %g|%g %s[%d](%dx%d)%dc ROI: %s a[%d](%dx%d)",
                oyStruct_GetId((oyStruct_s*)ticket),
                start_x_pixel, start_y_pixel, _("Image"),
                oyStruct_GetId((oyStruct_s*)image),image_width,oyImage_GetHeight(image),channels,
                oyRectangle_Show( roi ),
                oyStruct_GetId((oyStruct_s*)a),oyArray2d_GetWidth(a),oyArray2d_GetHeight(a));

    oyImage_Release( &image );
    oyArray2d_Release( &a );
    oyRectangle_Release( &ticket_array_roi );
  }

  return t?t:"----";
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
 *  @date     2016/09/14
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetArray (
                                       oyPixelAccess_s   * pixel_access,
                                       oyArray2d_s       * array,
                                       int                 has_roi_focus )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  if(s->array != array)
  {
    if(s->array)
      oyArray2d_Release( &s->array );
    s->array = oyArray2d_Copy( array, 0 );
  }

  s->output_array_is_focussed = has_roi_focus;

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
