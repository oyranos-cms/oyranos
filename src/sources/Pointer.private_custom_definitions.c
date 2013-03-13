/** @internal
 *  Function    oyPointer_Release__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer destructor
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
  oyPointer_s_ * s = cmmptr;
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmptr->member );
   */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmptr->member );
     */
    if(--s->ref)
      return;

    s->type_ = 0;

    if(s->ptr)
    {
      if(s->ptrRelease)
        s->ptrRelease( &s->ptr );
      else
      {
        oyDeAllocateFunc_( s->ptr );
        s->ptr = 0;
      }

      if(s->lib_name)
      deallocateFunc( s->lib_name ); s->lib_name = 0;
      if(s->func_name)
      deallocateFunc( s->func_name ); s->func_name = 0;
      if(s->resource)
      deallocateFunc( s->resource ); s->resource = 0;
      if(s->id)
      deallocateFunc( s->id ); s->id = 0;

      /*oyCMMdsoRelease_( cmmptr->lib_name );*/
    }
  }
}

/** @internal
 *  Function    oyPointer_Init__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer constructor 
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

/** @internal
 *  Function    oyPointer_Copy__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer copy constructor
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

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

#define COPY_MEMBER_STRING(variable_name) { \
  if(allocateFunc_) \
    dst->variable_name = oyStringCopy_( src->variable_name, allocateFunc_ ); \
  else \
    dst->variable_name = src->variable_name; }

  /* Copy each value of src to dst here */
  dst->ref = src->ref;
  COPY_MEMBER_STRING( lib_name )
  COPY_MEMBER_STRING( func_name )
  COPY_MEMBER_STRING( resource )
  COPY_MEMBER_STRING( id )

  return 0;
}
