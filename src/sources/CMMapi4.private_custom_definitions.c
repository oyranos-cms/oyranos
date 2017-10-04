/** Function    oyCMMapi4_Release__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi4  the CMMapi4 object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
void oyCMMapi4_Release__Members( oyCMMapi4_s_ * cmmapi4 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi4->member );
   */

  if(cmmapi4 && cmmapi4->oy_ && cmmapi4->oy_->deallocateFunc_)
  {
#   if 0
    oyDeAlloc_f deallocateFunc = cmmapi4->oy_->deallocateFunc_;
#   endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi4->member );
     */

    oyCMMui_Release( (oyCMMui_s**) &cmmapi4->ui );
    oyCMMapiFilter_Release( (oyCMMapiFilter_s**)&cmmapi4 );
  }
}

/** Function    oyCMMapi4_Init__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi4  the CMMapi4 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi4_Init__Members( oyCMMapi4_s_ * cmmapi4 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi4_Copy__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi4_s_ input object
 *  @param[out]  dst  the output oyCMMapi4_s_ object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
int oyCMMapi4_Copy__Members( oyCMMapi4_s_ * dst, oyCMMapi4_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */

  memcpy( dst->context_type, src->context_type, 8 );
  dst->oyCMMFilterNode_ContextToMem = src->oyCMMFilterNode_ContextToMem;
  dst->oyCMMFilterNode_GetText = src->oyCMMFilterNode_GetText;
  dst->ui = (oyCMMui_s_*) oyCMMui_Copy( (oyCMMui_s*)src->ui, src->oy_ );

  return 0;
}
