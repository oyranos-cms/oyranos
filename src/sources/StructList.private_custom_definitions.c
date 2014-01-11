/** @internal
 *  Function    oyStructList_Release__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  structlist  the StructList object
 */
void oyStructList_Release__Members( oyStructList_s_ * structlist )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &structlist->member );
   */

  oyStructList_Clear((oyStructList_s*)structlist);

  if(structlist->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = structlist->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( structlist->member );
     */
    if(structlist->ptr_)
      deallocateFunc(structlist->ptr_);

    if(structlist->list_name)
      deallocateFunc(structlist->list_name);

    memset( structlist, 0, sizeof(oyOBJECT_e) );
  }
}

/** @internal
 *  Function    oyStructList_Init__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  structlist  the StructList object
 */
int oyStructList_Init__Members( oyStructList_s_ * structlist )
{
  return 0;
}

/** @internal
 *  Function    oyStructList_Copy__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyStructList_s_ input object
 *  @param[out]  dst  the output oyStructList_s_ object
 */
int oyStructList_Copy__Members( oyStructList_s_ * dst, oyStructList_s_ * src)
{
  int error = 0, i;
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

  /* Copy each value of src to dst here */
  dst->parent_type_ = src->parent_type_;
  if(src->list_name)
    dst->list_name = oyStringAppend_(0, src->list_name, allocateFunc_);

  oyObject_Lock( dst->oy_, __FILE__, __LINE__ );

  dst->n_reserved_ = (src->n_ > 10) ? (int)(src->n_ * 1.5) : 10;
  dst->n_ = src->n_;
  dst->ptr_ = oyAllocateFunc_( sizeof(int*) * dst->n_reserved_ );
  memset( dst->ptr_, 0, sizeof(int*) * dst->n_reserved_ );

  for(i = 0; i < src->n_; ++i)
    if(src->ptr_[i]->copy)
      dst->ptr_[i] = src->ptr_[i]->copy( src->ptr_[i], 0 );

  if(oyStruct_IsObserved( (oyStruct_s*)dst, 0) )
    error = oyStructList_ObserverAdd( (oyStructList_s*)src, 0, 0, 0 );

  oyObject_UnLock( dst->oy_, __FILE__, __LINE__ );

  return error;
}
