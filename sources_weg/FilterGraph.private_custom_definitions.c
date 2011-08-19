/** Function    oyFilterGraph_Release__Members
 *  @memberof   oyFilterGraph_s
 *  @brief      Custom FilterGraph destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filtergraph  the FilterGraph object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterGraph_Release__Members( oyFilterGraph_s_ * filtergraph )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filtergraph->member );
   */

  if(filtergraph->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filtergraph->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filtergraph->member );
     */
  }
}

/** Function    oyFilterGraph_Init__Members
 *  @memberof   oyFilterGraph_s
 *  @brief      Custom FilterGraph constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filtergraph  the FilterGraph object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterGraph_Init__Members( oyFilterGraph_s_ * filtergraph )
{
  return 0;
}

/** Function    oyFilterGraph_Copy__Members
 *  @memberof   oyFilterGraph_s
 *  @brief      Custom FilterGraph copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterGraph_s_ input object
 *  @param[out]  dst  the output oyFilterGraph_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterGraph_Copy__Members( oyFilterGraph_s_ * dst, oyFilterGraph_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return error;
}
