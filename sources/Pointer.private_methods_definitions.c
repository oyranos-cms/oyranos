/** @internal
 *  @brief set oyPointer_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2011/02/13
 */
int                oyPointer_Set_    ( oyPointer_s_      * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  oyPointer_s_ * s = cmm_ptr;
  int error = !s;

  if(error <= 0 && lib_name)
    s->lib_name = oyStringCopy_( lib_name, oyAllocateFunc_ );

  if(error <= 0 && func_name)
    if(oyStrlen_(func_name) < 32)
      oySprintf_(s->func_name, "%s", func_name); 

  if(error <= 0 && ptr)
  {
    if(s->ptrRelease && s->ptr)
      s->ptrRelease( &ptr );
    s->ptr = ptr;
  }

  if(error <= 0 && resource)
    if(oyStrlen_(resource) < 5)
      oySprintf_(s->resource, "%s", resource); 

  if(error <= 0 && ptrRelease)
    s->ptrRelease = ptrRelease;

  return error;
}
