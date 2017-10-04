/** Function    oyCMMapi3_Release__Members
 *  @memberof   oyCMMapi3_s
 *  @brief      Custom CMMapi3 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi3  the CMMapi3 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi3_Release__Members( oyCMMapi3_s_ * cmmapi3 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi3->member );
   */

  if(cmmapi3->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi3->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi3->member );
     */
  }
}

/** Function    oyCMMapi3_Init__Members
 *  @memberof   oyCMMapi3_s
 *  @brief      Custom CMMapi3 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi3  the CMMapi3 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi3_Init__Members( oyCMMapi3_s_ * cmmapi3 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi3_Copy__Members
 *  @memberof   oyCMMapi3_s
 *  @brief      Custom CMMapi3 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi3_s_ input object
 *  @param[out]  dst  the output oyCMMapi3_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi3_Copy__Members( oyCMMapi3_s_ * dst, oyCMMapi3_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */

  return 0;
}
