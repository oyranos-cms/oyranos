/** Function    oyOption_Release__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  option  the Option object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyOption_Release__Members( oyOption_s_ * option )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &option->member );
   */

  if(option->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = option->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( option->member );
     */
  }
}

static int oy_option_id_ = 0;

/** Function    oyOption_Init__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  option  the Option object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOption_Init__Members( oyOption_s_ * option )
{
  option->id = oy_option_id_++;

  return 0;
}

/** Function    oyOption_Copy__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyOption_s_ input object
 *  @param[out]  dst  the output oyOption_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOption_Copy__Members( oyOption_s_ * dst, oyOption_s_ * src)
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
