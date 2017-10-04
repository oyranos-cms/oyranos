/** Function    oyOptions_Release__Members
 *  @memberof   oyOptions_s
 *  @brief      Custom Options destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  options  the Options object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyOptions_Release__Members( oyOptions_s_ * options )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &options->member );
   */

  if(options->oy_->deallocateFunc_)
  {
#if 0
    /* not used */
    oyDeAlloc_f deallocateFunc = options->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( options->member );
     */
  }
}

/** Function    oyOptions_Init__Members
 *  @memberof   oyOptions_s
 *  @brief      Custom Options constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  options  the Options object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOptions_Init__Members( oyOptions_s_ * options OY_UNUSED )
{
  return 0;
}

/** Function    oyOptions_Copy__Members
 *  @memberof   oyOptions_s
 *  @brief      Custom Options copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyOptions_s_ input object
 *  @param[out]  dst  the output oyOptions_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOptions_Copy__Members( oyOptions_s_ * dst, oyOptions_s_ * src)
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

  return error;
}
