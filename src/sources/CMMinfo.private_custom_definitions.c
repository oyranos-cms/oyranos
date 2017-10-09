/** Function    oyCMMinfo_Release__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmminfo  the CMMinfo object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/09
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 */
void oyCMMinfo_Release__Members( oyCMMinfo_s_ * cmminfo )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmminfo->member );
   */
  oyCMMapi_s * api = NULL,
             * old_api = cmminfo->api;
  while(old_api && (api = oyCMMapi_GetNext(old_api)) != NULL)
  {
    if(old_api->release)
      old_api->release( (oyStruct_s**)&old_api );
    old_api = api;
  }

  if(cmminfo->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmminfo->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmminfo->member );
     */

    if(cmminfo->backend_version)
      deallocateFunc( cmminfo->backend_version );
  }
}

/** Function    oyCMMinfo_Init__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmminfo  the CMMinfo object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMinfo_Init__Members( oyCMMinfo_s_ * cmminfo OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMinfo_Copy__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMinfo_s_ input object
 *  @param[out]  dst  the output oyCMMinfo_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMinfo_Copy__Members( oyCMMinfo_s_ * dst, oyCMMinfo_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif
  int error = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  error = !memcpy(dst->cmm, src->cmm, 8);

  if(src->backend_version)
    dst->backend_version = oyStringCopy_( src->backend_version, allocateFunc_ );

  dst->getText = src->getText;

  dst->oy_compatibility = src->oy_compatibility;

  return error;
}
