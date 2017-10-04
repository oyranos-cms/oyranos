/** Function    oyCMMapi6_Release__Members
 *  @memberof   oyCMMapi6_s
 *  @brief      Custom CMMapi6 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi6  the CMMapi6 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi6_Release__Members( oyCMMapi6_s_ * cmmapi6 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi6->member );
   */

  if(cmmapi6->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi6->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi6->member );
     */
  }
}

/** Function    oyCMMapi6_Init__Members
 *  @memberof   oyCMMapi6_s
 *  @brief      Custom CMMapi6 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi6  the CMMapi6 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi6_Init__Members( oyCMMapi6_s_ * cmmapi6 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi6_Copy__Members
 *  @memberof   oyCMMapi6_s
 *  @brief      Custom CMMapi6 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi6_s_ input object
 *  @param[out]  dst  the output oyCMMapi6_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi6_Copy__Members( oyCMMapi6_s_ * dst, oyCMMapi6_s_ * src)
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
