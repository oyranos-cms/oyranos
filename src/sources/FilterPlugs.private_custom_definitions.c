/** Function    oyFilterPlugs_Release__Members
 *  @memberof   oyFilterPlugs_s
 *  @brief      Custom FilterPlugs destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filterplugs  the FilterPlugs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterPlugs_Release__Members( oyFilterPlugs_s_ * filterplugs )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filterplugs->member );
   */

  if(filterplugs->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = filterplugs->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filterplugs->member );
     */
  }
}

/** Function    oyFilterPlugs_Init__Members
 *  @memberof   oyFilterPlugs_s
 *  @brief      Custom FilterPlugs constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filterplugs  the FilterPlugs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlugs_Init__Members( oyFilterPlugs_s_ * filterplugs OY_UNUSED )
{
  return 0;
}

/** Function    oyFilterPlugs_Copy__Members
 *  @memberof   oyFilterPlugs_s
 *  @brief      Custom FilterPlugs copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterPlugs_s_ input object
 *  @param[out]  dst  the output oyFilterPlugs_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlugs_Copy__Members( oyFilterPlugs_s_ * dst, oyFilterPlugs_s_ * src)
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
