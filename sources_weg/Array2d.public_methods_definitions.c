/** Function oyArray2d_Create
 *  @memberof oyArray2d_s
 *  @brief   allocate and initialise a oyArray2d_s object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object )
{
  oyArray2d_s * s = 0;
  int error = 0;

  if(!width || !height)
    return s;

  s = oyArray2d_Create_( width, height, data_type, object );
  error = !s;

  if(error <= 0)
  {
    if(data)
      error = oyArray2d_DataSet( s, data );
    else
    {
      data = s->oy_->allocateFunc_( width * height *
                                    oySizeofDatatype( data_type ) );
      error = oyArray2d_DataSet( s, data );
      s->own_lines = oyYES;
    }
  }

  return s;
}

#if 0
/**
 *  Function oyArray2d_DataCopy
 *  @memberof oyArray2d_s
 *  @brief   copy data
 *
 *  @todo just refere the other arrays, with refs_ and refered_ members,
 *        reuse memory
 *
 *  @param[in,out] obj                 the array to fill in
 *  @param[in]     roi_obj             rectangle of interesst in samples
 *  @param[in]     source              the source data
 *  @param[in]     roi_source          rectangle of interesst in samples
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/12 (Oyranos: 0.1.10)
 *  @date    2009/03/12
 */
OYAPI int  OYEXPORT
                 oyArray2d_DataCopy  ( oyArray2d_s      ** obj,
                                       oyRectangle_s     * obj_source,
                                       oyArray2d_s       * source,
                                       oyRectangle_s     * roi_source )
{
  oyArray2d_s * s = *obj,
              * src = source;
  int error = 0;
  int new_roi = !roi;

  if(!src || !s)
    return 1;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  if(error <= 0)
  {
    if(!roi)
      roi = oyRectangle_NewWith( 0,0, s->width, s->height, s->oy_ );
    error = !roi;
  }

  /* possibly expensive hack */
  if(*obj)
    oyArray2d_Release( obj );

  if(!(*obj))
  {
    *obj = oyArray2d_Create( 0, roi->height, roi->width, src->t, src->oy_ );
    error = !*obj;
  }

  if(error <= 0)
  {
    oyAlloc_f allocateFunc_ = s->oy_->allocateFunc_;
    int i, size;

    s->own_lines = 2;
    for(i = 0; i < roi->height; ++i)
    {
      size = roi->width * oySizeofDatatype( s->t );
      if(!s->array2d[i])
        oyAllocHelper_m_( s->array2d[i], unsigned char, size, allocateFunc_,
                          error = 1; break );
      error = !memcpy( s->array2d[i], src->array2d[i], size );
    }
  }

  if(error)
    oyArray2d_Release( obj );

  if(new_roi)
    oyRectangle_Release( &roi );

  return error;
}
#endif

/**
 *  @internal
 *  Function oyArray2d_DataSet
 *  @memberof oyArray2d_s
 *  @brief   set the data blob and (re-)initialise the object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     data                the data, remains in the property of the
 *                                     caller
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI int  OYEXPORT
                 oyArray2d_DataSet   ( oyArray2d_s       * obj,
                                       oyPointer           data )
{
  oyArray2d_s * s = 0;
  int error = 0;

  if(!data)
    return 1;

  if(!obj || obj->type_ != oyOBJECT_ARRAY2D_S)
    return 1;

  s = obj;

  {
    int y_len = sizeof(unsigned char *) * (s->height + 1),
        y;
    uint8_t * u8 = data;

    error = !s->array2d;

    if(error <= 0)
      error = !memset( s->array2d, 0, y_len );

    s->own_lines = oyNO;

    if(error <= 0)
      for( y = 0; y < s->height; ++y )
        s->array2d[y] = &u8[oySizeofDatatype( s->t ) * s->width * y];
  }

  return error;
}

/** Function oyArray2d_ReleaseArray
 *  @memberof oyArray2d_s
 *  @brief   release Array2d::array member
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/07 (Oyranos: 0.1.11)
 *  @date    2010/09/07
 */
int            oyArray2d_ReleaseArray( oyArray2d_s       * obj )
{
  int error = 0;
  oyArray2d_s * s = obj;
  if(s && s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int y, y_max = s->data_area.height + s->data_area.y;
    size_t dsize = oySizeofDatatype( s->t );

    if(oy_debug > 3)
      oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*)s,
                       OY_DBG_FORMAT_ "s->data_area: %s", OY_DBG_ARGS_,
                       oyRectangle_Show(&s->data_area) );

    for( y = s->data_area.y; y < y_max; ++y )
    {
      if((s->own_lines == 1 && y == s->data_area.y) ||
         s->own_lines == 2)
        deallocateFunc( &s->array2d[y][dsize * (int)s->data_area.x] );
      s->array2d[y] = 0;
    }
    deallocateFunc( s->array2d + (size_t)s->data_area.y );
    s->array2d = 0;
  }
  return error;
}
 
/**
 *  @internal
 *  Function oyArray2d_RowsSet
 *  @memberof oyArray2d_s
 *  @brief   set the data and (re-)initialise the object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     data                the data
 *  @param[in]     do_copy             - 0 : take the memory as is
 *                                     - 1 : copy the memory monolithic
 *                                     - 2 : copy the memory chunky
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/07 (Oyranos: 0.1.11)
 *  @date    2010/09/07
 */
OYAPI int  OYEXPORT
                 oyArray2d_RowsSet   ( oyArray2d_s       * obj,
                                       oyPointer         * rows,
                                       int                 do_copy )
{
  oyArray2d_s * s = obj;
  int error = 0;

  if(!rows)
    return 1;

  if(!obj || obj->type_ != oyOBJECT_ARRAY2D_S)
    return 1;

  {
    int y_len = sizeof(unsigned char *) * (s->height + 1),
        y;
    size_t size = 0;
    oyAlloc_f allocateFunc_ = s->oy_->allocateFunc_;

    error = !s->array2d;

    size = s->width * oySizeofDatatype( s->t );

    oyArray2d_ReleaseArray( s );

    /* allocate the base array */
    oyAllocHelper_m_( s->array2d, unsigned char *, s->height+1, allocateFunc_,
                      error = 1; return 1 );
    if(error <= 0)
      error = !memset( s->array2d, 0, y_len );

    s->own_lines = do_copy;

    if(error <= 0 && s->own_lines == 2)
    {
      /* allocate each line separately */
      for(y = 0; y < s->height; ++y)
      {
        oyAllocHelper_m_( s->array2d[y], unsigned char, size, allocateFunc_,
                          error = 1; break );
        error = !memcpy( s->array2d[y], rows[y], size );
      }

    } else
    if(error <= 0 && s->own_lines == 1)
    {
      /* allocate all lines at once */
      unsigned char * u8 = 0;
      oyAllocHelper_m_( u8, unsigned char, size * s->height, allocateFunc_,
                        error = 1; return 1 );

      s->own_lines = do_copy;
      if(error <= 0)
      for( y = 0; y < s->height; ++y )
      {
        s->array2d[y] = &u8[size * y];
        error = !memcpy( s->array2d[y], rows[y], size );
      }

    } else
    if(error <= 0 && s->own_lines == 0)
    {
      /* just assign */
      for( y = 0; y < s->height; ++y )
        s->array2d[y] = rows[y];
    }
  }

  return error;
}

/** Function oyArray2d_SetFocus
 *  @memberof oyArray2d_s
 *  @brief   move a arrays active area to a given rectangle
 *
 *  @param[in,out] array               the pixel array
 *  @param[in]     rectangle           the new region in the array's wholes data
 *  @return                            error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/04/19 (Oyranos: 0.3.0)
 *  @date    2011/04/19
 */
int          oyArray2d_SetFocus      ( oyArray2d_s       * array,
                                       oyRectangle_s     * rectangle )
{
  oyRectangle_s * array_roi_pix = rectangle;
  oyArray2d_s * a = array;
  int error = 0;
  int y;

  if(array && rectangle)
  {
    /* shift array focus to requested region */
    int bps = oySizeofDatatype( array->t );
    if(a->data_area.x != OY_ROUND(array_roi_pix->x))
    {
      int height = a->data_area.height + a->data_area.y;
      int shift = (OY_ROUND(array_roi_pix->x) + OY_ROUND(a->data_area.x)) * bps;
      for(y = a->data_area.y; y < height; ++y)
        a->array2d[y] += shift;
      a->data_area.x = -OY_ROUND(array_roi_pix->x);
    }
    if(a->data_area.y != OY_ROUND(array_roi_pix->y))
    {
      a->array2d += OY_ROUND(array_roi_pix->y + a->data_area.y);
      a->data_area.y = -array_roi_pix->y;
    }
    a->width = array_roi_pix->width;
    a->height = array_roi_pix->height;
  } else
    error = 1;

  return error;
}
