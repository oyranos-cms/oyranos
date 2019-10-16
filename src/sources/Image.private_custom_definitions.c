/** Function    oyImage_Release__Members
 *  @memberof   oyImage_s
 *  @brief      Custom Image destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  image  the Image object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyImage_Release__Members( oyImage_s_ * image )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &image->member );
   */
  image->width = 0;
  image->height = 0;
  if(image->pixel_data && image->pixel_data->release)
    image->pixel_data->release( &image->pixel_data );

  if(image->user_data && image->user_data->release)
    image->user_data->release( &image->user_data );

  oyProfile_Release( &image->profile_ );
  oyOptions_Release( &image->tags );
  oyRectangle_Release( &image->viewport );


  if(image->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = image->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( image->member );
     */
    if(image->layout_)
    { deallocateFunc( image->layout_ ); image->layout_ = 0; }

    if(image->channel_layout)
    { deallocateFunc( image->channel_layout ); image->channel_layout = 0; }
  }
}

/** Function    oyImage_Init__Members
 *  @memberof   oyImage_s
 *  @brief      Custom Image constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  image  the Image object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyImage_Init__Members( oyImage_s_ * image )
{
  image->tags = oyOptions_New(0);
  image->layout_ = image->oy_->allocateFunc_( sizeof(oyPixel_t) * (oyCHAN0 + 
                                             4 + 1));
  memset(image->layout_,0,sizeof(oyPixel_t) * (oyCHAN0 + 4 + 1));
  return 0;
}

/** Function    oyImage_Copy__Members
 *  @memberof   oyImage_s
 *  @brief      Custom Image copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyImage_s_ input object
 *  @param[out]  dst  the output oyImage_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyImage_Copy__Members( oyImage_s_ * dst, oyImage_s_ * src)
{
  int error = 0;
#if 0
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

#if 0
  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */

  return error;
}
