/** Function    oyCMMptr_Release__Members
 *  @memberof   oyCMMptr_s
 *  @brief      Custom CMMptr destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmptr  the CMMptr object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMptr_Release__Members( oyCMMptr_s_ * cmmptr )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmptr->member );
   */

  if(cmmptr->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmmptr->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmptr->member );
     */
  }
}

/** Function    oyCMMptr_Init__Members
 *  @memberof   oyCMMptr_s
 *  @brief      Custom CMMptr constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmptr  the CMMptr object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMptr_Init__Members( oyCMMptr_s_ * cmmptr )
{
  ++cmmptr->ref;

  return 0;
}

/** Function    oyCMMptr_Copy__Members
 *  @memberof   oyCMMptr_s
 *  @brief      Custom CMMptr copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMptr_s_ input object
 *  @param[out]  dst  the output oyCMMptr_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMptr_Copy__Members( oyCMMptr_s_ * dst, oyCMMptr_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  dst->ref = src-ref;

  return 0;
}
