/** Function    oyPointer_Release__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmptr  the Pointer object
 *
 *  @version Oyranos: 0.3.0
 *  @since   2010/08/00 (Oyranos: 0.3.0)
 *  @date    2011/02/13
 */
void oyPointer_Release__Members( oyPointer_s_ * cmmptr )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmptr->member );
   */

  if(cmmptr->oy_->deallocateFunc_)
  {
    /*oyDeAlloc_f deallocateFunc = cmmptr->oy_->deallocateFunc_;*/

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmptr->member );
     */
    if(--cmmptr->ref)
      return;

    cmmptr->type_ = 0;

    if(cmmptr->ptr)
    {
      if(cmmptr->ptrRelease)
        cmmptr->ptrRelease( &cmmptr->ptr );
      else
      {
        oyDeAllocateFunc_( cmmptr->ptr );
        cmmptr->ptr = 0;
      }

      /*oyCMMdsoRelease_( cmmptr->lib_name );*/
    }
  }
}

/** Function    oyPointer_Init__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmptr  the Pointer object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPointer_Init__Members( oyPointer_s_ * cmmptr )
{
  ++cmmptr->ref;

  return 0;
}

/** Function    oyPointer_Copy__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyPointer_s_ input object
 *  @param[out]  dst  the output oyPointer_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPointer_Copy__Members( oyPointer_s_ * dst, oyPointer_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  dst->ref = src->ref;

  return 0;
}
