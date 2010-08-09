/** Function    oyCMMui_Release__Members
 *  @memberof   oyCMMui_s
 *  @brief      Custom CMMui destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmui  the CMMui object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMui_Release__Members( oyCMMui_s_ * cmmui )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmui->member );
   */

  if(cmmui->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmmui->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmui->member );
     */
  }
}

/** Function    oyCMMui_Init__Members
 *  @memberof   oyCMMui_s
 *  @brief      Custom CMMui constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmui  the CMMui object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMui_Init__Members( oyCMMui_s_ * cmmui )
{
  return 0;
}

/** Function    oyCMMui_Copy__Members
 *  @memberof   oyCMMui_s
 *  @brief      Custom CMMui copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMui_s_ input object
 *  @param[out]  dst  the output oyCMMui_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMui_Copy__Members( oyCMMui_s_ * dst, oyCMMui_s_ * src)
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
