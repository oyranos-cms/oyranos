/** Function    oy{{ class.baseName }}_Release__Members
 *  @memberof   {{ class.name }}
 *  @brief      Custom {{ class.baseName }} destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  {{ class.baseName|lower }}  the {{ class.baseName }} object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oy{{ class.baseName }}_Release__Members( {{ class.privName }} * {{ class.baseName|lower }} )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &{{ class.baseName|lower }}->member );
   */

  if({{ class.baseName|lower }}->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = {{ class.baseName|lower }}->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( {{ class.baseName|lower }}->member );
     */
  }
}

/** Function    oy{{ class.baseName }}_Init__Members
 *  @memberof   {{ class.name }}
 *  @brief      Custom {{ class.baseName }} constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  {{ class.baseName|lower }}  the {{ class.baseName }} object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oy{{ class.baseName }}_Init__Members( {{ class.privName }} * {{ class.baseName|lower }} )
{
  return 0;
}

/** Function    oy{{ class.baseName }}_Copy__Members
 *  @memberof   {{ class.name }}
 *  @brief      Custom {{ class.baseName }} copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the {{ class.privName }} input object
 *  @param[out]  dst  the output {{ class.privName }} object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oy{{ class.baseName }}_Copy__Members( {{ class.privName }} * dst, {{ class.privName }} * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src || dst == src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return error;
}
