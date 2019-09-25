/** Function    oyFilterCore_Release__Members
 *  @memberof   oyFilterCore_s
 *  @brief      Custom FilterCore destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filtercore  the FilterCore object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterCore_Release__Members( oyFilterCore_s_ * filtercore )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filtercore->member );
   */

  if(filtercore->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filtercore->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filtercore->member );
     */
    if(filtercore->registration_)
      deallocateFunc( filtercore->registration_ );

    if(filtercore->category_) {
      deallocateFunc( filtercore->category_ );
      filtercore->category_ = 0;
    }

    if(filtercore->api4_ && filtercore->api4_->release)
      filtercore->api4_->release( (oyStruct_s**)&filtercore->api4_ );
  }
}

/** Function    oyFilterCore_Init__Members
 *  @memberof   oyFilterCore_s
 *  @brief      Custom FilterCore constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filtercore  the FilterCore object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterCore_Init__Members( oyFilterCore_s_ * filtercore OY_UNUSED )
{
  /* No members seem to need initialization for now */
  return 0;
}

/** Function    oyFilterCore_Copy__Members
 *  @memberof   oyFilterCore_s
 *  @brief      Custom FilterCore copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterCore_s_ input object
 *  @param[out]  dst  the output oyFilterCore_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterCore_Copy__Members( oyFilterCore_s_ * dst, oyFilterCore_s_ * src)
{
  int error = 0;
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
  dst->registration_ = oyStringCopy_( src->registration_, allocateFunc_ );
  dst->category_ = oyStringCopy_( src->category_, allocateFunc_ );
  dst->options_ = oyOptions_Copy( src->options_, dst->oy_ );
  dst->api4_ = src->api4_;

  return error;
}
