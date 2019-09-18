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
   */
  cmmui->parent = NULL;

  if(cmmui->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmui->oy_->deallocateFunc_;
#endif

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
 *  @version Oyranos: 0.9.7
 *  @since   2017/10/26 (Oyranos: 0.9.7)
 *  @date    2017/10/26
 */
int oyCMMui_Init__Members( oyCMMui_s_ * cmmui )
{
  /* set basic versions, as is checked during module load */
  cmmui->module_api[0] = cmmui->version[0] = OYRANOS_VERSION_A;
  cmmui->module_api[1] = cmmui->version[1] = OYRANOS_VERSION_B;
  cmmui->module_api[2] = cmmui->version[2] = OYRANOS_VERSION_C;

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
  if(src->parent && src->parent->copy)
  {
    dst->parent = (oyCMMapiFilter_s*) src->parent->copy( (oyStruct_s*) src->parent, src->oy_ );
    if(oy_debug_objects >= 0 && dst->parent)
        oyObjectDebugMessage_( dst->parent->oy_, __func__,
                               oyStructTypeToText(dst->parent->type_) );
  } else
    dst->parent = src->parent;

  return 0;
}
