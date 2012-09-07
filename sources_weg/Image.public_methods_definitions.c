/** Function oyImage_GetPointContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout pixel accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyPointer oyImage_GetArray2dPointContinous (
                                         oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  int pos = (point_x * image->layout_[oyCHANS]
             + image->layout_[oyCHAN0+channel])
            * image->layout_[oyDATA_SIZE];
  if(is_allocated) *is_allocated = 0;
  return &array2d[ point_y ][ pos ]; 

}

/** Function oyImage_GetLineContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout line accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
oyPointer oyImage_GetArray2dLineContinous (
                                         oyImage_s       * image,
                                         int               point_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  if(height) *height = 1;
  if(is_allocated) *is_allocated = 0;
  if(point_y >= a->height)
    WARNc2_S("point_y < a->height failed(%d/%d)", point_y, a->height)
  return &array2d[ point_y ][ 0 ]; 
}

/** Function oyImage_SetPointContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout pixel accessor
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/22 (Oyranos: 0.1.10)
 *  @date    2009/02/22
 */
int       oyImage_SetArray2dPointContinous (
                                         oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         oyPointer         data )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  int pos = (point_x * image->layout_[oyCHANS]
             + image->layout_[oyCHAN0+channel])
            * image->layout_[oyDATA_SIZE];
  oyDATATYPE_e data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  int byteps = oySizeofDatatype( data_type );
  int channels = 1;

  if(channel < 0)
    channels = oyToChannels_m( image->layout_[oyLAYOUT] );

  memcpy( &array2d[ point_y ][ pos ], data, byteps * channels );

  return 0;

}

/** Function oyImage_SetLineContinous
 *  @memberof oyImage_s
 *  @brief   standard continous layout line accessor
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/22 (Oyranos: 0.1.10)
 *  @date    2009/02/22
 */
int       oyImage_SetArray2dLineContinous (
                                         oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               pixel_n,
                                         int               channel,
                                         oyPointer         data )
{
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  oyDATATYPE_e data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  int byteps = oySizeofDatatype( data_type );
  int channels = 1;
  int offset = point_x;

  if(pixel_n < 0)
    pixel_n = image->width - point_x;

  if(channel < 0)
  {
    channels = oyToChannels_m( image->layout_[oyLAYOUT] );
    offset *= channels;
  }

  if(&array2d[ point_y ][ offset ] != data)
    memcpy( &array2d[ point_y ][ offset ], data, pixel_n * byteps * channels );

  return 0; 
}

/** Function oyImage_GetPointPlanar
 *  @memberof oyImage_s
 *  @brief   standard planar layout pixel accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/08/24
 */
oyPointer oyImage_GetArray2dPointPlanar( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated )
{
  WARNc_S("planar pixel access not implemented")
  return 0;
#if 0
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  if(is_allocated) *is_allocated = 0;
  return &array2d[ point_y ][ (point_x + image->layout_[oyCOFF]
                               * image->layout_[oyCHAN0+channel])
                              * image->layout_[oyDATA_SIZE]       ]; 
#endif
}

/** Function oyImage_GetLinePlanar
 *  @memberof oyImage_s
 *  @brief   standard continus layout line accessor
 *
 *  We assume a channel after channel behaviour without line interweaving.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/24
 */
oyPointer oyImage_GetArray2dLinePlanar ( oyImage_s       * image,
                                         int               point_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated )
{
  if(height) *height = 1;
  WARNc_S("planar pixel access not implemented")
  return 0;
#if 0 /* SunC: warning: statement not reached */
  oyArray2d_s * a = (oyArray2d_s*) image->pixel_data;
  unsigned char ** array2d = a->array2d;
  if(is_allocated) *is_allocated = 0;
  /* it makes no sense to use more than one line */                   
  return &array2d[ 0 ][   image->width
                        * image->layout_[oyCOFF]
                        * image->layout_[oyCHAN0+channel]
                        * image->layout_[oyDATA_SIZE] ];
#endif
}


/** @brief   collect infos about a image
 *  @memberof oyImage_s
 *
 *  Create a image description and access object. The passed channels pointer
 *  remains in the responsibility of the user. The image is a in memory blob.
 *
    @param[in]    width        image width
    @param[in]    height       image height
    @param[in]    channels     pointer to the data buffer
    @param[in]    pixel_layout i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]    profile      colour space description
    @param[in]    object       the optional base
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
oyImage_s *    oyImage_Create         ( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        oyObject_s        object)
{
  oyRectangle_s * display_rectangle = 0;
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_IMAGE_S;
# define STRUCT_TYPE oyImage_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyImage_Copy;
  s->release = (oyStruct_Release_f) oyImage_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(!profile)
  {
    WARNc_S("no profile obtained");

    oyImage_Release( &s );
    return s;
  }

  s->width = width;
  s->height = height;
  {
    int channels_n = oyToChannels_m(pixel_layout);
    oyArray2d_s * a = oyArray2d_Create( channels,
                                        s->width * channels_n,
                                        s->height,
                                        oyToDataType_m(pixel_layout),
                                        s_obj );
    oyImage_SetData ( s, (oyStruct_s**) &a, 0,0,0,0,0,0 );
  }
  s->profile_ = oyProfile_Copy( profile, 0 );
  if(s->width != 0.0)
    s->viewport = oyRectangle_NewWith( 0, 0, 1.0,
                                   (double)s->height/(double)s->width, s->oy_ );

  error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout );

  if(s->pixel_data && s->layout_[oyCOFF] == 1)
    oyImage_SetData( s, 0, oyImage_GetArray2dPointContinous,
                           oyImage_GetArray2dLineContinous, 0,
                           oyImage_SetArray2dPointContinous,
                           oyImage_SetArray2dLineContinous, 0 );
  else if(s->pixel_data)
    oyImage_SetData( s, 0, oyImage_GetArray2dPointPlanar,
                           oyImage_GetArray2dLinePlanar, 0, 0,0,0 );

  if(error <= 0)
  {
    display_rectangle = oyRectangle_New_( 0 );

    error = !display_rectangle;
    if(error <= 0)
      oyOptions_MoveInStruct( &s->tags,
                              "//imaging/output/display_rectangle",
                              (oyStruct_s**)&display_rectangle, OY_CREATE_NEW );
  }

  return s;
}

/** @brief   collect infos about a image for showing one a display
 *  @memberof oyImage_s

    @param[in]     width               image width
    @param[in]     height              image height
    @param[in]     channels            pointer to the data buffer
    @param[in]     pixel_layout        i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]     display_name        display name
    @param[in]     display_pos_x       left image position on display
    @param[in]     display_pos_y       top image position on display
    @param[in]     display_width       width to show in window
    @param[in]     display_height      height to show in window
    @param[in]     object              the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_CreateForDisplay ( int              width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       const char        * display_name,
                                       int                 display_pos_x,
                                       int                 display_pos_y,
                                       int                 display_width,
                                       int                 display_height,
                                       oyObject_s          object)
{
  oyProfile_s * p = oyProfile_FromFile ("XYZ.icc",0,0);
  oyImage_s * s = oyImage_Create( width, height, channels, pixel_layout,
                                  p, object );
  int error = !s;
  oyRectangle_s * display_rectangle = 0;

  oyProfile_Release( &p );

  if(error <= 0)
  {
    if(!s->profile_)
      error = 1;

    if(error <= 0)
      error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout );

    if(error <= 0)
    {
      display_rectangle = (oyRectangle_s*) oyOptions_GetType( s->tags, -1,
                                    "display_rectangle", oyOBJECT_RECTANGLE_S );
      oyRectangle_SetGeo( display_rectangle, display_pos_x, display_pos_y,
                                             display_width, display_height );
    }
    error = !display_rectangle;
    
    if(error <= 0 && display_name)
      error = oyOptions_SetFromText( &s->tags, "//imaging/output/display_name",
                                     display_name, OY_CREATE_NEW );

    if(error > 0)
    {
      oyImage_Release( &s );
      WARNc1_S("Could not create image %d", oyObject_GetId( object ));
    }
  }

  return s;
}


/** @brief   set a image
 *  @memberof oyImage_s
 *
 *  set critical options
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/19 (Oyranos: 0.1.8)
 *  @date    2009/03/01
 */
int            oyImage_SetCritical   ( oyImage_s         * image,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyOptions_s       * tags,
                                       int                 width,
                                       int                 height )
{
  oyImage_s_ * s = (oyImage_s_*)image;
  int error = !s;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

  if(profile)
  {
    oyProfile_Release( &s->profile_ );
    s->profile_ = oyProfile_Copy( profile, 0 );
  }

  if(tags)
  {
    oyOptions_Release( &s->tags );
    s->tags = oyOptions_Copy( tags, s->oy_ );
  }

  s->width = width;
  s->height = height;

  if(pixel_layout)
    error = oyImage_CombinePixelLayout2Mask_ ( s, pixel_layout );
  else
    /* update to new ID for possible new context hashing */
    error = oyImage_CombinePixelLayout2Mask_ ( s, s->layout_[oyLAYOUT] );

  /* Not shure whether it is a good idea to have automatic image data
     allocation here. Anyway this is intented as a fallback for empty images, 
     like a unspecified output image to be catched here. */
  if((!s->setLine || !s->getLine) &&
     (!s->setPoint || !s->getPoint) &&
     s->width && s->height)
  {
    oyPixel_t pixel_layout = s->layout_[oyLAYOUT];
    oyPointer channels = 0;

    oyArray2d_s * a = oyArray2d_Create( channels,
                                        s->width * oyToChannels_m(pixel_layout),
                                        s->height,
                                        oyToDataType_m(pixel_layout),
                                        s->oy_ );
      
    oyImage_SetData( (oyImage_s*)s,    (oyStruct_s**) &a,
                           oyImage_GetArray2dPointContinous,
                           oyImage_GetArray2dLineContinous, 0,
                           oyImage_SetArray2dPointContinous,
                           oyImage_SetArray2dLineContinous, 0 );
  }

  return error;
}

/** Function oyImage_FillArray
 *  @memberof oyImage_s
 *  @brief   creata a array from a image and fill with data
 *
 *  The rectangle will be considered relative to the data.
 *  A given array will be filled. Allocation of a new array2d object happens as
 *  needed.
 *
 *  @param[in]     image               the image
 *  @param[in]     rectangle           the image rectangle in a relative unit
 *                                     a rectangle in the source image
 *  @param[in]     allocate_method
 *                                     - 0 assign the rows without copy
 *                                     - 1 do copy into the array
 *                                     - 2 allocate empty rows
 *  @param[out]    array               array to fill; If array is empty, it is
 *                                     allocated as per allocate_method
 *  @param[in]     array_rectangle     the array rectangle in samples
 *                                     For NULL the image rectangle will be
 *                                     placed to the top left corner in array.
 *                                     If array_rectangle is provided, image
 *                                     samples will be placed in array at 
 *                                     array_rectangle's position and size.
 *                                     The unit is relative to the image.
 *  @param[in]     obj                 the optional user object
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/10/02 (Oyranos: 0.1.8)
 *  @date    2011/05/02
 */
int            oyImage_FillArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       int                 allocate_method,
                                       oyArray2d_s      ** array,
                                       oyRectangle_s     * array_rectangle,
                                       oyObject_s          obj )
{
  int error;
  oyArray2d_s * a = *array;
  oyImage_s * s = image;
  oyRectangle_s image_roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0},
                r = {oyOBJECT_RECTANGLE_S,0,0,0};
  oyDATATYPE_e data_type = oyUINT8;
  int is_allocated = 0;
  int data_size, ay;
  oyRectangle_s array_roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0};
  int array_width, array_height;
  oyAlloc_f allocateFunc_ = 0;
  unsigned char * line_data = 0;
  int i,j, height;
  size_t wlen;

  if(!image)
    return 1;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

  if( allocate_method < 0 || allocate_method > 2 )
  {
    WARNc1_S("allocate_method not allowed: %d", allocate_method )
    error = 1;
  }

  data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  data_size = oySizeofDatatype( data_type );
  error = oyRectangle_SamplesFromImage( image, rectangle, &image_roi_pix );

  if(!error && array_rectangle)
    error = oyRectangle_SamplesFromImage( image, array_rectangle,
                                          &array_roi_pix );
  else
  {
    oyRectangle_SetGeo( &r, 0,0, rectangle->width, rectangle->height );
    error = oyRectangle_SamplesFromImage( image, &r, &array_roi_pix );
  }

  array_width = array_roi_pix.x + array_roi_pix.width;
  array_height = array_roi_pix.y + array_roi_pix.height;

  if(!error &&
     (!a ||
      (a && ( array_width > a->data_area.width ||
              array_height > a->data_area.height ))) &&
     array_width > 0 && array_height > 0
    )
  {
    oyArray2d_Release( array );
    if(!(array_roi_pix.width && array_roi_pix.height))
      /* array creation is not possible */
      error = -1;

    if(!error)
    {
      a = oyArray2d_Create_( array_width, array_height,
                             data_type, obj );

      if(a->oy_)
        allocateFunc_ = a->oy_->allocateFunc_;

      error = !a;
      if(!error)
      {
        /* allocate each single line */
        if(allocate_method == 1 || allocate_method == 2)
        {
          a->own_lines = 2;

          for(ay = 0; ay < array_height; ++ay)
            if(!a->array2d[ay])
              oyAllocHelper_m_( a->array2d[ay], 
                              unsigned char,
                              array_width * data_size,
                              allocateFunc_,
                              error = 1; break );
        } else if(allocate_method == 0)
        {
          for( i = 0; i < array_height; )
          {
            if(!a->array2d[i])
            {
              height = is_allocated = 0;
              line_data = image->getLine( image, i, &height, -1,
                             &is_allocated );
              for( j = 0; j < height; ++j )
              {
                if( i + j >= array_height )
                  break;

                ay = i + j;

                a->array2d[ay] = 
                    &line_data[j*data_size * array_width];
              }
            }

            i += height;

            if(error) break;
          }
        }
      }
    }
  }

  /* a array should have been created */
  if( !a && array_roi_pix.width && array_roi_pix.height )
  {
    WARNc_S("Could not create array.")
    if(error <= 0) error = -1;
  }

  if( !error && a )
  {
    /* shift array focus to requested region */
    oyArray2d_SetFocus( a, &array_roi_pix );

    /* change intermediately */
    if(a && a->width > image_roi_pix.width)
    {
      a->width = image_roi_pix.width;
    }
    if(a && a->height > image_roi_pix.height)
    {
      a->height = image_roi_pix.height;
    }
  }

  if(a && !error)
  {
  if(image->getLine)
  {
    oyPointer src, dst;

    wlen = image_roi_pix.width * data_size;

    if(allocate_method != 2)
    for( i = 0; i < image_roi_pix.height; )
    {
      height = is_allocated = 0;
      line_data = image->getLine( image, image_roi_pix.y + i, &height, -1,
                             &is_allocated );

      for( j = 0; j < height; ++j )
      {
        if( i + j >= array_roi_pix.height )
          break;

        ay = i + j;

        dst = &a->array2d[ay][0];
        src = &line_data[(j
                       * OY_ROUND(image->width * image->layout_[oyCHANS])
                       + OY_ROUND(image_roi_pix.x))
                      * data_size];

        if(dst && src && dst != src)
          error = !memcpy( dst, src, wlen );
      }

      i += height;

      if(error) break;
    }

  } else
  if(image->getPoint)
  {
    WARNc_S("image->getPoint  not yet supported")
  } else
  if(image->getTile)
  {
    WARNc_S("image->getTile  not yet supported")
    error = 1;
  }
  }

  if(error)
    oyArray2d_Release( &a );

  *array = a;

  return error;
}

/** Function oyImage_ReadArray
 *  @memberof oyImage_s
 *  @brief   read a array into a image
 *
 *  The rectangle will be considered relative to the image.
 *  The given array should match that rectangle.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
int            oyImage_ReadArray     ( oyImage_s         * image,
                                       oyRectangle_s     * image_rectangle,
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * array_rectangle )
{
  oyImage_s * s = image;
  int error = !image || !array;
  oyRectangle_s image_roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0};
  oyRectangle_s array_rect_pix = {oyOBJECT_RECTANGLE_S,0,0,0};
  oyDATATYPE_e data_type = oyUINT8;
  int bps = 0, channel_n, i, offset, width, height;

  if(error)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

  data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
  bps = oySizeofDatatype( data_type );
  channel_n = image->layout_[oyCHANS];

  error = oyRectangle_SamplesFromImage( image, image_rectangle, &image_roi_pix );
  /* We want to check if the array is big enough to hold the pixels */
  if(array->data_area.width < image_roi_pix.width ||
     array->data_area.height < image_roi_pix.height)
  {
    WARNc3_S( "array (%dx%d) is too small for rectangle %s",
               (int)array->width, (int)array->height,
               oyRectangle_Show( &image_roi_pix ) );
    error = 1;
  }

  if(!error & !image->setLine)
  {
    WARNc1_S( "only the setLine() interface is yet supported; image[%d]",
              oyObject_GetId( image->oy_ ) );
    error = 1;
  }

  if(!error)
  {
    if(array_rectangle)
    {
      oyRectangle_SetByRectangle( &array_rect_pix, array_rectangle );
      oyRectangle_Scale( &array_rect_pix, image->width );
      array_rect_pix.x *= channel_n;
      array_rect_pix.width *= channel_n;
    } else
    {
      oyRectangle_SetGeo( &array_rect_pix, 0,0, array->width, array->height );
    }
  }

  if(!error)
  {
    offset = image_roi_pix.x / channel_n * bps;
    width = OY_MIN(image_roi_pix.width, array_rect_pix.width);
    width /= channel_n;
    height = array_rect_pix.y + array_rect_pix.height;

    for(i = array_rect_pix.y; i < height; ++i)
    {
      image->setLine( image, offset, image_roi_pix.y + i, width, -1,
                      &array->array2d
                              [i][OY_ROUND(array_rect_pix.x) * bps] );
    }
  }

  return error;
}


/** Function oyImage_SetData
 *  @memberof oyImage_s
 *  @brief   set a custom image data module
 *
 *  This function allowes for exchanging of all the module components. 
 *
 *  The pixel_data structure can hold in memory or mmap representations or file
 *  pointers. The according point, line and/or tile functions shall use
 *  the oyImage_s::pixel_data member to access the data and provide in this
 *  interface.
 *
 *  @param         image               the image
 *  @param         pixel_data          data struct will be moved in
 *  @param         getPoint            interface function
 *  @param         getLine             interface function
 *  @param         getTile             interface function
 *  @param         setPoint            interface function
 *  @param         setLine             interface function
 *  @param         setTile             interface function
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
int            oyImage_SetData       ( oyImage_s         * image,
                                       oyStruct_s       ** pixel_data,
                                       oyImage_GetPoint_f  getPoint,
                                       oyImage_GetLine_f   getLine,
                                       oyImage_GetTile_f   getTile,
                                       oyImage_SetPoint_f  setPoint,
                                       oyImage_SetLine_f   setLine,
                                       oyImage_SetTile_f   setTile )
{
  oyImage_s * s = image;
  int error = 0;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 1 )

  if(pixel_data)
  {
    if(s->pixel_data && s->pixel_data->release)
      s->pixel_data->release( &s->pixel_data );
    s->pixel_data = *pixel_data;
    *pixel_data = 0;
  }

  if(getPoint)
    s->getPoint = getPoint;

  if(getLine)
    s->getLine = getLine;

  if(getTile)
    s->getTile = getTile;

  if(setPoint)
    s->setPoint = setPoint;

  if(setLine)
    s->setLine = setLine;

  if(setTile)
    s->setTile = setTile;

  return error;
}
/** Function oyImage_GetWidth
 *  @memberof oyImage_s
 *  @brief   Get the width in pixel
 *
 *  @version Oyranos: 0.5.0
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2012/06/12
 */
int            oyImage_GetWidth      ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->width;
}

/** Function oyImage_GetHeight
 *  @memberof oyImage_s
 *  @brief   Get the width in pixel
 *
 *  @version Oyranos: 0.5.0
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2012/06/12
 */
int            oyImage_GetHeight     ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->height;
}

/** Function oyImage_GetPixelLayout
 *  @memberof oyImage_s
 *  @brief   Get the pixel layout
 *
 *  @version Oyranos: 0.5.0
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2012/09/05
 */
oyPixel_t      oyImage_GePixeltLayout( oyImage_s         * image,
                                       oyLAYOUT_e          type )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->layout_[type];
}

/** Function oyImage_GetChannelType
 *  @memberof oyImage_s
 *  @brief   Get a channel type
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
oyCHANNELTYPE_e  oyImage_GetChannelType (
                                       oyImage_s         * image,
                                       int                 pos )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->channel_layout[pos];
}
/** Function oyImage_GetSubPositioning
 *  @memberof oyImage_s
 *  @brief   Get sub positioning
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
int            oyImage_GetSubPositioning (
                                       oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->sub_positioning;
}

/** Function oyImage_GetProfile
 *  @memberof oyImage_s
 *  @brief   Get the image profile
 *
 *  @version Oyranos: 0.5.0
 *  @since   2009/08/05 (Oyranos: 0.1.10)
 *  @date    2012/06/12
 */
oyProfile_s *  oyImage_GetProfile    ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return oyProfile_Copy( s->profile_, 0 );
}

/** Function oyImage_GetTags
 *  @memberof oyImage_s
 *  @brief   Get object tags
 *
 *  @version Oyranos: 0.5.0
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2012/06/12
 */
oyOptions_s *  oyImage_GetTags       ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return oyOptions_Copy( s->tags, 0 );
}
/** Function  oyImage_GetPixelData
 *  @memberof oyImage_s
 *  @brief    Get the pixel data
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
oyStruct_s *   oyImage_GetPixelData  ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->pixel_data->copy(s->pixel_data, 0 );
}
/** Function  oyImage_GetPointF
 *  @memberof oyImage_s
 *  @brief    Get the point function
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
oyImage_GetPoint_f oyImage_GetPointF ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->getPoint;
}
/** Function  oyImage_GetLineF
 *  @memberof oyImage_s
 *  @brief    Get the Line function
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
oyImage_GetLine_f  oyImage_GetLineF  ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->getLine;
}
/** Function  oyImage_GetUserData
 *  @memberof oyImage_s
 *  @brief    Get the user data
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
oyStruct_s *   oyImage_GetUserData ( oyImage_s         * image )
{
  oyImage_s * s = image;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_IMAGE_S, return 0 )

  return s->user_data->copy( s->user_data, 0 );
}
