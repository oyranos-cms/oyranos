/** Function    oyNamedColors_Release__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  namedcolors  the NamedColors object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyNamedColors_Release__Members( oyNamedColors_s_ * namedcolors )
{
  oyNamedColors_s_ * s = namedcolors;
  if(s->prefix)
    oyObject_GetDeAlloc( s->oy_ )( s->prefix );
  if(s->suffix)
    oyObject_GetDeAlloc( s->oy_ )( s->suffix );
}

/** Function    oyNamedColors_Init__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  namedcolors  the NamedColors object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColors_Init__Members( oyNamedColors_s_ * namedcolors OY_UNUSED )
{
  return 0;
}

/** Function    oyNamedColors_Copy__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyNamedColors_s_ input object
 *  @param[out]  dst  the output oyNamedColors_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColors_Copy__Members( oyNamedColors_s_ * dst, oyNamedColors_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = oyObject_GetAlloc( dst->oy_ );

  /* Copy each value of src to dst here */
  if(src->prefix)
    dst->prefix = oyStringCopy_(src->prefix, allocateFunc_);

  return error;
}
