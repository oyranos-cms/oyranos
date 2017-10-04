/** Function    oyCMMapi9_Release__Members
 *  @memberof   oyCMMapi9_s
 *  @brief      Custom CMMapi9 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi9  the CMMapi9 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi9_Release__Members( oyCMMapi9_s_ * cmmapi9 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi9->member );
   */

  if(cmmapi9->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi9->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi9->member );
     */
  }
}

/** Function    oyCMMapi9_Init__Members
 *  @memberof   oyCMMapi9_s
 *  @brief      Custom CMMapi9 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi9  the CMMapi9 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi9_Init__Members( oyCMMapi9_s_ * cmmapi9 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi9_Copy__Members
 *  @memberof   oyCMMapi9_s
 *  @brief      Custom CMMapi9 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi9_s_ input object
 *  @param[out]  dst  the output oyCMMapi9_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi9_Copy__Members( oyCMMapi9_s_ * dst, oyCMMapi9_s_ * src)
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
