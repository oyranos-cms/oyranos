/** Function    oyLis_Release__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  lis  the Lis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyLis_Release__Members( oyLis_s_ * lis )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &lis->member );
   */

  if(lis->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = lis->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( lis->member );
     */
  }
}

/** Function    oyLis_Init__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  lis  the Lis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyLis_Init__Members( oyLis_s_ * lis )
{
  return 0;
}

/** Function    oyLis_Copy__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyLis_s_ input object
 *  @param[out]  dst  the output oyLis_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyLis_Copy__Members( oyLis_s_ * dst, oyLis_s_ * src)
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
