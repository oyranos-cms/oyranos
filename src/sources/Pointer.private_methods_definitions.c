/** @internal
 *  @brief set oyPointer_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2011/02/15
 */
int                oyPointer_Set_    ( oyPointer_s_      * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       const char        * id,
                                       oyPointer_release_f ptrRelease )
{
  oyPointer_s_ * s = cmm_ptr;
  int error = !s;
  oyAlloc_f alloc_func = oyStruct_GetAllocator( (oyStruct_s*) s );
  oyDeAlloc_f dealloc_func = oyStruct_GetDeAllocator( (oyStruct_s*) s );

    
  if(error <= 0 && lib_name)
  {
    if(s->lib_name)
      oyStringFree_( &s->lib_name, dealloc_func );
    s->lib_name = oyStringCopy_( lib_name, alloc_func );
  }

  if(error <= 0 && func_name)
  {
    if(s->func_name)
      oyStringFree_( &s->func_name, dealloc_func );
    s->func_name = oyStringCopy_( func_name, alloc_func );
  }

  if(error <= 0 && resource)
  {
    if(s->resource)
      oyStringFree_( &s->resource, dealloc_func );
    s->resource = oyStringCopy_( resource, alloc_func );
  }

  if(error <= 0 && id)
  {
    if(s->id)
      oyStringFree_( &s->id, dealloc_func );
    s->id = oyStringCopy_( id, alloc_func );
  }

  if(error <= 0 && ptr)
  {
    if(s->ptrRelease && s->ptr)
      s->ptrRelease( &s->ptr );
    s->ptr = ptr;
  }

  if(error <= 0 && ptrRelease)
    s->ptrRelease = ptrRelease;

  return error;
}
