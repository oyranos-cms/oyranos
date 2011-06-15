/** Function oyPointer_Set
 *  @brief   set a oyPointer_s
 *  @ingroup module_api
 *  @memberof oyPointer_s
 *
 *  Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/22 (Oyranos: 0.1.10)
 *  @date    2009/08/16
 */
int          oyPointer_Set           ( oyPointer_s       * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  return oyPointer_Set_( (oyPointer_s_*)cmm_ptr, lib_name, resource, ptr, func_name, ptrRelease);
}

const char * oyPointer_GetFuncName    ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->func_name;
  else
    return NULL;
}
const char * oyPointer_GetLibName     ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->lib_name;
  else
    return NULL;
}
const char * oyPointer_GetResourceName( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->resource;
  else
    return NULL;
}
int          oyPointer_GetSize        ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->size;
  else
    return 0;
}
int          oyPointer_SetSize        ( oyPointer_s        * cmm_ptr,
                                       int                 size )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    c->size = size;
  return 0;
}
oyPointer    oyPointer_GetPointer     ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->ptr;
  else
    return NULL;
}

/** Function oyPointerLookUpFromObject
 *  @brief   Get a module specific pointer from cache
 *  @memberof oyPointer_s
 *
 *  The returned oyPointer_s has to be released after using by the module with
 *  oyPointer_Release().
 *  In case the the oyPointer_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see oyPointerLookUpFromText()
 *
 *  @param[in]     data                 object to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyPointer_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyPointer_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyPointer_s  * oyPointerLookUpFromObject (
                                       oyStruct_s        * data,
                                       const char        * data_type )
{
  oyStruct_s * s = data;
  int error = !s;
  oyPointer_s * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    const char * tmp = 0;
    tmp = oyObject_GetName( s->oy_, oyNAME_NICK );
    cmm_ptr = oyPointerLookUpFromText( tmp, data_type );
  }

  return cmm_ptr;
}

/** Function oyPointerLookUpFromText
 *  @brief   get a module specific pointer from cache
 *  @memberof oyPointer_s
 *
 *  The returned oyPointer_s has to be released after using by the module with
 *  oyPointer_Release().
 *  In case the the oyPointer_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see e.g. lcmsCMMData_Open()
 *
 *  @param[in]     text                 hash text to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyPointer_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyPointer_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/05 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyPointer_s * oyPointerLookUpFromText( const char        * text,
                                       const char        * data_type )
{
  int error = !text;
  oyPointer_s * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    /*oyPointer_s *cmm_ptr = 0;*/
    const char * tmp = 0;

    oyHash_s * entry = 0;
    oyChar * hash_text = 0;

    /** Cache Search \n
     *  1.     hash from input \n
     *  2.     query for hash in cache \n
     *  3.     check \n
     *  3a.       eighter take cache entry or \n
     *  3b.       update cache entry
     */

    /* 1. create hash text */
    STRING_ADD( hash_text, data_type );
    STRING_ADD( hash_text, ":" );
    tmp = text;
    STRING_ADD( hash_text, tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );

    if(error <= 0)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyPointer_s*) oyHash_GetPointer( entry,
                                                  oyOBJECT_POINTER_S);

      if(!cmm_ptr)
      {
        cmm_ptr = oyPointer_New( 0 );
        error = !cmm_ptr;

        if(error <= 0)
          error = oyPointer_Set( cmm_ptr, 0,
                                 data_type, 0, 0, 0 );

        error = !cmm_ptr;

        if(error <= 0 && cmm_ptr)
          /* 3b.1. update cache entry */
          error = oyHash_SetPointer( entry,
                                     (oyStruct_s*) cmm_ptr );
      }
    }

    oyHash_Release( &entry );
  }

  return cmm_ptr;
}
