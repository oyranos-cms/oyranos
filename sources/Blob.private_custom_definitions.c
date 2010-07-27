/** Function    oyBlob_Release__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  blob  the Blob object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyBlob_Release__Members( oyBlob_s_ * blob )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &blob->member );
   */

  if(blob->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = blob->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( blob->member );
     */
  }
}

/** Function    oyBlob_Init__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  blob  the Blob object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyBlob_Init__Members( oyBlob_s_ * blob )
{
  blob->ptr = NULL;

  return 0;
}

/** Function    oyBlob_Copy__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyBlob_s_ input object
 *  @param[out]  dst  the output oyBlob_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyBlob_Copy__Members( oyBlob_s_ * dst, oyBlob_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
  int error = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  if(error <= 0)
  {
    allocateFunc_ = dst->oy_->allocateFunc_;

    if(src->ptr && src->size && !(src->flags & 0x01))
    {
      dst->ptr = allocateFunc_( src->size );
      error = !dst->ptr;
      if(error <= 0)
        error = !memcpy( dst->ptr, src->ptr, src->size );
    } else
      dst->ptr = src->ptr;
  }

  if(error <= 0)
  {
    dst->size = src->size;
    dst->flags = src->flags;
    error = !memcpy( dst->type, src->type, 8 );
  }

  return error;
}
