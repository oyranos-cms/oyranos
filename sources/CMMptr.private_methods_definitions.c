/** @internal
 *  @brief set oyCMMptr_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/12/27
 */
int                oyCMMptr_Set_     ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  oyCMMptr_s * s = cmm_ptr;
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
