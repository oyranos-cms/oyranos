/** Function    oyConversion_Release__Members
 *  @memberof   oyConversion_s
 *  @brief      Custom Conversion destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  conversion  the Conversion object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConversion_Release__Members( oyConversion_s_ * conversion )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &conversion->member );
   */
  oyFilterNode_Release( (oyFilterNode_s**)&conversion->input );
  oyFilterNode_Release( (oyFilterNode_s**)&conversion->out_ );

  if(conversion->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = conversion->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( conversion->member );
     */
  }
}

/** Function    oyConversion_Init__Members
 *  @memberof   oyConversion_s
 *  @brief      Custom Conversion constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  conversion  the Conversion object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConversion_Init__Members( oyConversion_s_ * conversion OY_UNUSED )
{
  return 0;
}

/** Function    oyConversion_Copy__Members
 *  @memberof   oyConversion_s
 *  @brief      Custom Conversion copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConversion_s_ input object
 *  @param[out]  dst  the output oyConversion_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConversion_Copy__Members( oyConversion_s_ * dst, oyConversion_s_ * src)
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
  dst->input = (oyFilterNode_s_*)oyFilterNode_Copy( (oyFilterNode_s*)src->input, dst->oy_ );

  return error;
}
