/** Function    oyCMMapi_Release__Members
 *  @memberof   oyCMMapi_s
 *  @brief      Custom CMMapi destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi  the CMMapi object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi_Release__Members( oyCMMapi_s_ * cmmapi )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi->member );
   */

  if(cmmapi->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmmapi->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi->member );
     */
    if(cmmapi->id_)
      deallocateFunc( cmmapi->id_ );
    cmmapi->id_ = NULL;
    if(cmmapi->registration)
      deallocateFunc( cmmapi->registration );
    cmmapi->registration = NULL;
    cmmapi->next = NULL;
  }
}

/** Function    oyCMMapi_Init__Members
 *  @memberof   oyCMMapi_s
 *  @brief      Custom CMMapi constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi  the CMMapi object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi_Init__Members( oyCMMapi_s_ * cmmapi OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi_Copy__Members
 *  @memberof   oyCMMapi_s
 *  @brief      Custom CMMapi copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi_s_ input object
 *  @param[out]  dst  the output oyCMMapi_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi_Copy__Members( oyCMMapi_s_ * dst, oyCMMapi_s_ * src)
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
