/** Function    oyCMMapi7_Release__Members
 *  @memberof   oyCMMapi7_s
 *  @brief      Custom CMMapi7 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi7  the CMMapi7 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi7_Release__Members( oyCMMapi7_s_ * cmmapi7 )
{
  uint32_t i;
  for(i = 0; i < cmmapi7->plugs_n; ++i)
  {
    if(cmmapi7->plugs[i] && cmmapi7->plugs[i]->release)
      cmmapi7->plugs[i]->release( (oyStruct_s**)&cmmapi7->plugs[i] );
  }
  for(i = 0; i < cmmapi7->sockets_n; ++i)
  {
    if(cmmapi7->sockets[i] && cmmapi7->sockets[i]->release)
      cmmapi7->sockets[i]->release( (oyStruct_s**)&cmmapi7->sockets[i] );
  }

  if(cmmapi7->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmmapi7->oy_->deallocateFunc_;

    if(cmmapi7->properties)
    {
      i = 0;
      while(cmmapi7->properties && cmmapi7->properties[i])
        deallocateFunc( cmmapi7->properties[i++] );
      deallocateFunc( cmmapi7->properties );
    }
  }
}

/** Function    oyCMMapi7_Init__Members
 *  @memberof   oyCMMapi7_s
 *  @brief      Custom CMMapi7 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi7  the CMMapi7 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi7_Init__Members( oyCMMapi7_s_ * cmmapi7 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi7_Copy__Members
 *  @memberof   oyCMMapi7_s
 *  @brief      Custom CMMapi7 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi7_s_ input object
 *  @param[out]  dst  the output oyCMMapi7_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi7_Copy__Members( oyCMMapi7_s_ * dst, oyCMMapi7_s_ * src)
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
