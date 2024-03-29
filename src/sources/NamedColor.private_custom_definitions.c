/** Function    oyNamedColor_Release__Members
 *  @memberof   oyNamedColor_s
 *  @brief      Custom NamedColor destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  namedcolor  the NamedColor object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyNamedColor_Release__Members( oyNamedColor_s_ * namedcolor )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &namedcolor->member );
   */
  oyNamedColor_s_ * s = namedcolor;

  oyProfile_Release( &s->profile_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->channels_)
    { deallocateFunc( s->channels_ ); s->channels_ = 0; }

    if(s->blob_) /* s->blob_len */
    { deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0; }
  }
}

/** Function    oyNamedColor_Init__Members
 *  @memberof   oyNamedColor_s
 *  @brief      Custom NamedColor constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  namedcolor  the NamedColor object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColor_Init__Members( oyNamedColor_s_ * namedcolor OY_UNUSED )
{
  return 0;
}

/** Function    oyNamedColor_Copy__Members
 *  @memberof   oyNamedColor_s
 *  @brief      Custom NamedColor copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyNamedColor_s_ input object
 *  @param[out]  dst  the output oyNamedColor_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColor_Copy__Members( oyNamedColor_s_ * dst, oyNamedColor_s_ * src)
{
  int error = 0;
  oyNamedColor_s_ * s = dst;
  oyAlloc_f allocateFunc_ = 0;
  const double    * chan;
  const char      * blob;
  int               blob_len;
  oyProfile_s     * ref;
  int n = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

  chan = src->channels_;
  blob = src->blob_;
  blob_len = src->blob_len_;
  ref = src->profile_;

  /* Copy each value of src to dst here */
  if(error <= 0)
  {
    s->profile_  = oyProfile_Copy( ref, 0 );
  }

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(n)
    s->channels_ = allocateFunc_( n * sizeof(double) );
  oyCopyColor( chan, &s->channels_[0], 1, ref, n );
  oyCopyColor( 0, &s->XYZ_[0], 1, 0, 0 );

  if(error <= 0 && blob && blob_len)
  {
    s->blob_ = allocateFunc_( blob_len );
    if(!s->blob_) error = 1;

    if(error <= 0)
      error = !memcpy( s->blob_, blob, blob_len );

    if(error <= 0)
      s->blob_len_ = blob_len;
  }

  return error;
}
