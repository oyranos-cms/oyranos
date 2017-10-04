/** Function    oyCMMapis_Release__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapis  the CMMapis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapis_Release__Members( oyCMMapis_s_ * cmmapis )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapis->member );
   */

  if(cmmapis->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapis->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapis->member );
     */
  }
}

/** Function    oyCMMapis_Init__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapis  the CMMapis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapis_Init__Members( oyCMMapis_s_ * cmmapis OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapis_Copy__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapis_s_ input object
 *  @param[out]  dst  the output oyCMMapis_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapis_Copy__Members( oyCMMapis_s_ * dst, oyCMMapis_s_ * src)
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
