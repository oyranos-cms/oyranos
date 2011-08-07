/** Function  oyRectangle_CountPoints
 *  @memberof oyRectangle_s
 *  @brief    Count number of points covered by this rectangle
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2009/02/23
 */
double         oyRectangle_CountPoints(oyRectangle_s     * rectangle )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)rectangle;
  oyRectangle_s_ * r = s;

  if(!s)
    return FALSE;

  return r->width * r->height;
}

/** Function  oyRectangle_Index
 *  @memberof oyRectangle_s
 *  @brief    Return position inside rectangle, assuming rectangle size
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRectangle_Index     ( oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)rectangle;
  oyRectangle_s_ * r = s;

  if(!s)
    return FALSE;

  return OY_ROUND((y - r->y) * r->width + (x - r->x));
}

/**
 *  @brief   compare
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRectangle_IsEqual   ( oyRectangle_s     * rectangle1,
                                       oyRectangle_s     * rectangle2 )
{
  int gleich = TRUE;
  oyRectangle_s_ * r1 = (oyRectangle_s_*)rectangle1;
  oyRectangle_s_ * r2 = (oyRectangle_s_*)rectangle2;

  if(!r1 || !r2)
    return FALSE;

  if (r1->x != r2->x) gleich = FALSE;
  if (r1->y != r2->y) gleich = FALSE;
  if (r1->width != r2->width) gleich = FALSE;
  if (r1->height != r2->height) gleich = FALSE;
  return gleich;
}

/** Function  oyRectangle_IsInside
 *  @memberof oyRectangle_s
 *  @brief    Compare
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/02/23
 */
int            oyRectangle_IsInside  ( oyRectangle_s     * test,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s_ * test_ = (oyRectangle_s_*)test;

  return oyRectangle_PointIsInside( ref, test_->x, test_->y ) &&
         oyRectangle_PointIsInside( ref, test_->x + test_->width - 1, test_->y ) &&
         oyRectangle_PointIsInside( ref, test_->x + test_->width - 1,
                                      test_->y + test_->height - 1) &&
         oyRectangle_PointIsInside( ref, test_->x, test_->y + test_->height - 1 );
}

/** Function  oyRectangle_MoveInside
 *  @memberof oyRectangle_s
 *  @brief    Trim edit_rectangle to ref extents
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_MoveInside( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)edit_rectangle;
  oyRectangle_s_ * a = (oyRectangle_s_*)ref;

  if(!s)
    return;

  oyRectangle_Normalise( edit_rectangle );

  if (s->x < a->x)
    s->x = a->x;
  if (s->x+s->width > a->x+a->width)
  { if (s->width > a->width)
      ; /* Lassen */
    else
      s->x = a->x+a->width - s->width;
  }
  if (s->y < a->y)
    s->y = a->y;
  if (s->y+s->height  > a->y+a->height)
  { if (s->height > a->height)
      ; /* Lassen */
    else
      s->y = a->y+a->height - s->height;
  }
}

/**
 *  @brief   new from other rectangle
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_NewFrom   ( oyRectangle_s     * ref,
                                       oyObject_s          object )
{
  oyRectangle_s * s = oyRectangle_New_( object );
  if(s)
    oyRectangle_SetByRectangle(s, ref);
  return s;
}

/**
 *  @brief   new with geometry
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_NewWith   ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object )
{
  oyRectangle_s * s = oyRectangle_New_( object );
  if(s)
    oyRectangle_SetGeo( s, x, y, width, height );
  return s;
}

/**
 *  @brief   normalise swapped values for width and height
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Normalise ( oyRectangle_s     * edit_rectangle )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;

  if(!s)
    return;

  if(r->width < 0) {
    r->x += r->width;
    r->width = fabs(r->width);
  }
  if(r->height < 0) {
    r->y += r->height;
    r->height = fabs(r->height);
  }
}

/**
 *  @brief   compare
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2009/02/23
 */
int            oyRectangle_PointIsInside (
                                       oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y )
{
  oyRectangle_s * s = rectangle;
  oyRectangle_s * r = s;
  int in = TRUE;

  if(!s)
    return FALSE;

  if (x < r->x) return FALSE;
  if (y < r->y) return FALSE;
  if (x >= (r->x + r->width)) return FALSE;
  if (y >= (r->y + r->height)) return FALSE;
  return in;
}

/**
 *  @brief   scale with origin in the top left corner
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Round     ( oyRectangle_s     * edit_rectangle )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;

  if(!s)
    return;

  r->x = OY_ROUND(r->x);
  r->y = OY_ROUND(r->y);
  r->width = OY_ROUND(r->width);
  r->height = OY_ROUND(r->height);
}

/** Function oyRectangle_SamplesFromImage
 *  @memberof oyRectangle_s
 *  @brief   new from image
 *
 *  @param[in]     image               a image
 *  @param[in]     image_rectangle     optional rectangle from image
 *  @param[in,out] pixel_rectangle     mandatory rectangle for pixel results
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/13 (Oyranos: 0.1.10)
 *  @date    2009/06/08
 */
int            oyRectangle_SamplesFromImage (
                                       oyImage_s         * image,
                                       oyRectangle_s     * image_rectangle,
                                       oyRectangle_s     * pixel_rectangle )
{
  int error = !image,
      channel_n = 0;

  if(!error && image->type_ != oyOBJECT_IMAGE_S)
    return 0;

  if(!error)
  {
    channel_n = image->layout_[oyCHANS];

    if(!image_rectangle)
    {
      oyRectangle_SetGeo( pixel_rectangle, 0,0, image->width, image->height );
      pixel_rectangle->width *= channel_n;

    } else
    {
      oyRectangle_SetByRectangle( pixel_rectangle, image_rectangle );
      oyRectangle_Scale( pixel_rectangle, image->width );
      pixel_rectangle->x *= channel_n;
      pixel_rectangle->width *= channel_n;
      oyRectangle_Round( pixel_rectangle );
    }
  }

  return error;
}

/**
 *  @brief   scale with origin in the top left corner
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Scale     ( oyRectangle_s     * edit_rectangle,
                                       double              factor )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;

  if(!s)
    return;

  r->x *= factor;
  r->y *= factor;
  r->width *= factor;
  r->height *= factor;
}

/**
 *  @brief   copy values
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_SetByRectangle (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s * s = edit_rectangle;
  if(!s || !ref)
    return;

  oyRectangle_SetGeo( s, ref->x, ref->y, ref->width, ref->height );
}

/**
 *  @brief   set geometry
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_SetGeo    ( oyRectangle_s     * edit_rectangle,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height )
{
  oyRectangle_s * s = edit_rectangle;
  if(!s)
    return;

  s->x = x;
  s->y = y;
  s->width = width;
  s->height = height;
}

/**
 *  @memberof oyRectangle_s
 *  @brief   debug text
 *  not so threadsafe
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
const char*    oyRectangle_Show      ( oyRectangle_s     * r )
{
  static oyChar *text = 0;

  if(!text)
    text = oyAllocateFunc_(sizeof(char) * 512);

  if(r)
    oySprintf_(text, "%.02fx%.02f%s%.02f%s%.02f", r->width,r->height,
                     r->x<0?"":"+", r->x, r->y<0?"":"+", r->y);
  else
    oySprintf_(text, "no rectangle");

  return text;

}

/**
 *  @brief   trim edit_rectangle to ref extents
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Trim      ( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s * r = s;
  if(!s)
    return;

  if (r->x < ref->x)
  {
    r->width -= ref->x - r->x;
    r->x = ref->x;
  }
  if (r->x + r->width > ref->x + ref->width)
    r->width -= (r->x + r->width) - (ref->x + ref->width);
  if( r->width < 0 )
    r->width = 0;

  if (r->y < ref->y)
  {
    r->height -= ref->y - r->y;
    r->y = ref->y;
  }
  if (r->y + r->height > ref->y + ref->height)
    r->height -= (r->y + r->height) - (ref->y + ref->height);
  if( r->height < 0 )
    r->height = 0;

  oyRectangle_Normalise( r );
}
