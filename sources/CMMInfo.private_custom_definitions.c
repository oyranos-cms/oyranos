/** Function    oyCMMInfo_Release__Members
 *  @memberof   oyCMMInfo_s
 *  @brief      Custom CMMInfo destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmminfo  the CMMInfo object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMInfo_Release__Members( oyCMMInfo_s_ * cmminfo )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmminfo->member );
   */

  if(cmminfo->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmminfo->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmminfo->member );
     */
  }
}

/** Function    oyCMMInfo_Init__Members
 *  @memberof   oyCMMInfo_s
 *  @brief      Custom CMMInfo constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmminfo  the CMMInfo object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMInfo_Init__Members( oyCMMInfo_s_ * cmminfo )
{
  return 0;
}

/** Function    oyCMMInfo_Copy__Members
 *  @memberof   oyCMMInfo_s
 *  @brief      Custom CMMInfo copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMInfo_s_ input object
 *  @param[out]  dst  the output oyCMMInfo_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMInfo_Copy__Members( oyCMMInfo_s_ * dst, oyCMMInfo_s_ * src)
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
