/** Function    oyPixelAccess_Release__Members
 *  @memberof   oyPixelAccess_s
 *  @brief      Custom PixelAccess destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  pixelaccess  the PixelAccess object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyPixelAccess_Release__Members( oyPixelAccess_s_ * pixelaccess )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &pixelaccess->member );
   */
  oyArray2d_Release( &pixelaccess->array );
  oyRectangle_Release( (oyRectangle_s**)&pixelaccess->output_array_roi );
  oyImage_Release( &pixelaccess->output_image );
  oyFilterGraph_Release( (oyFilterGraph_s**)&pixelaccess->graph );
  oyOptions_Release( &pixelaccess->request_queue );

  if(pixelaccess->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = pixelaccess->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( pixelaccess->member );
     */
    if(pixelaccess->user_data && pixelaccess->user_data->release)
        pixelaccess->user_data->release( &pixelaccess->user_data );
    if(pixelaccess->array_xy)
      deallocateFunc( pixelaccess->array_xy );
    pixelaccess->array_xy = 0;
  }
}

/** Function    oyPixelAccess_Init__Members
 *  @memberof   oyPixelAccess_s
 *  @brief      Custom PixelAccess constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  pixelaccess  the PixelAccess object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPixelAccess_Init__Members( oyPixelAccess_s_ * pixelaccess )
{
  pixelaccess->output_array_roi = (oyRectangle_s_*)oyRectangle_NewFrom( 0, 0 );

  return 0;
}

/** Function    oyPixelAccess_Copy__Members
 *  @memberof   oyPixelAccess_s
 *  @brief      Custom PixelAccess copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyPixelAccess_s_ input object
 *  @param[out]  dst  the output oyPixelAccess_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPixelAccess_Copy__Members( oyPixelAccess_s_ * dst, oyPixelAccess_s_ * src)
{
  int error = 0, len;
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  dst->start_xy_old[0] = dst->start_xy[0] = src->start_xy[0];
  dst->start_xy_old[1] = dst->start_xy[1] = src->start_xy[1];
  dst->array_n = src->array_n;
  if(src->array_xy && src->array_n)
  {
    len = sizeof(int32_t) * 2 * src->array_n;
    dst->array_xy = allocateFunc_(len);
    error = !dst->array_xy;
    if(error <= 0)
      error = !memcpy(dst->array_xy, src->array_xy, len);
  }
  /* reset to properly initialise the new iterator */
  dst->index = 0;
  dst->pixels_n = src->pixels_n;
  dst->workspace_id = src->workspace_id;
  dst->output_array_roi = (oyRectangle_s_*)oyRectangle_Copy( (oyRectangle_s*)src->output_array_roi, dst->oy_ );
  dst->output_image = oyImage_Copy( src->output_image, 0 );
  dst->array = oyArray2d_Copy( src->array, 0 );
  if(src->user_data && src->user_data->copy)
    dst->user_data = src->user_data->copy( src->user_data, 0 );
  else
    dst->user_data = src->user_data;
  dst->graph = (oyFilterGraph_s_*)oyFilterGraph_Copy( (oyFilterGraph_s*)src->graph, 0 );

  return error;
}
