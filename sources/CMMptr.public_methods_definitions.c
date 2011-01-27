/** Function oyCMMptr_Set
 *  @brief   set a oyCMMptr_s
 *  @ingroup module_api
 *  @memberof oyCMMptr_s
 *
 *  Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/22 (Oyranos: 0.1.10)
 *  @date    2009/08/16
 */
int          oyCMMptr_Set            ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  return oyCMMptr_Set_( (oyCMMptr_s_*)cmm_ptr, lib_name, resource, ptr, func_name, ptrRelease);
}

const char * oyCMMptr_GetFuncName    ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->func_name;
  else
    return NULL;
}
const char * oyCMMptr_GetLibName     ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->lib_name;
  else
    return NULL;
}
const char * oyCMMptr_GetResourceName( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->resource;
  else
    return NULL;
}
int          oyCMMptr_GetSize        ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->size;
  else
    return 0;
}
oyPointer    oyCMMptr_GetPointer     ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->ptr;
  else
    return NULL;
}

