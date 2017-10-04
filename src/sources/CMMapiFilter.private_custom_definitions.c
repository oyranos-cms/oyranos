/** Function    oyCMMapiFilter_Release__Members
 *  @memberof   oyCMMapiFilter_s
 *  @brief      Custom CMMapiFilter destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapifilter  the CMMapiFilter object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapiFilter_Release__Members( oyCMMapiFilter_s_ * cmmapifilter )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapifilter->member );
   */

  if(cmmapifilter->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapifilter->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapifilter->member );
     */
  }
}

/** Function    oyCMMapiFilter_Init__Members
 *  @memberof   oyCMMapiFilter_s
 *  @brief      Custom CMMapiFilter constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapifilter  the CMMapiFilter object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilter_Init__Members( oyCMMapiFilter_s_ * cmmapifilter OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapiFilter_Copy__Members
 *  @memberof   oyCMMapiFilter_s
 *  @brief      Custom CMMapiFilter copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapiFilter_s_ input object
 *  @param[out]  dst  the output oyCMMapiFilter_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilter_Copy__Members( oyCMMapiFilter_s_ * dst, oyCMMapiFilter_s_ * src)
{
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

  return 0;
}
