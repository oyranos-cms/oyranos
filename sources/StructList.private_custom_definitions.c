/** Function    oyStructList_Release__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  structlist  the StructList object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyStructList_Release__Members( oyStructList_s_ * structlist )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &structlist->member );
   */

  if(structlist->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = structlist->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( structlist->member );
     */
  }
}

/** Function    oyStructList_Init__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  structlist  the StructList object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyStructList_Init__Members( oyStructList_s_ * structlist )
{
  return 0;
}

/** Function    oyStructList_Copy__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyStructList_s_ input object
 *  @param[out]  dst  the output oyStructList_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyStructList_Copy__Members( oyStructList_s_ * dst, oyStructList_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return 0;
}
