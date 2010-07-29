/** Function oyCMMptrLookUpFromText
 *  @brief   get a module specific pointer from cache
 *  @memberof oyCMMptr_s
 *
 *  The returned oyCMMptr_s has to be released after using by the module with
 *  oyCMMptr_Release().
 *  In case the the oyCMMptr_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see e.g. lcmsCMMData_Open()
 *
 *  @param[in]     text                 hash text to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyCMMptr_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyCMMptr_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/05 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyCMMptr_s * oyCMMptrLookUpFromText  ( const char        * text,
                                       const char        * data_type )
{
  int error = !text;
  oyCMMptr_s_ * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    /*oyCMMptr_s *cmm_ptr = 0;*/
    const char * tmp = 0;

    oyHash_s_ * entry = 0;
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
      cmm_ptr = (oyCMMptr_s_*) oyHash_GetPointer_( entry,
                                                  oyOBJECT_CMM_POINTER_S);

      if(!cmm_ptr)
      {
        cmm_ptr = oyCMMptr_New_( 0 );
        error = !cmm_ptr;

        if(error <= 0)
          error = oyCMMptr_Set_( cmm_ptr, 0,
                                 data_type, 0, 0, 0 );

        error = !cmm_ptr;

        if(error <= 0 && cmm_ptr)
          /* 3b.1. update cache entry */
          error = oyHash_SetPointer_( entry,
                                     (oyStruct_s*) cmm_ptr );
      }
    }

    oyHash_Release_( &entry );
  }

  return (oyCMMptr_s*)cmm_ptr;
}

/** Function oyCMMptrLookUpFromObject
 *  @brief   get a module specific pointer from cache
 *  @memberof oyCMMptr_s
 *
 *  The returned oyCMMptr_s has to be released after using by the module with
 *  oyCMMptr_Release().
 *  In case the the oyCMMptr_s::ptr member is empty, it should be set by the
 *  requesting module.
 *
 *  @see oyCMMptrLookUpFromText()
 *
 *  @param[in]     data                 object to look up
 *  @param[in]     data_type            four byte module type for this object
 *                                      type; The data_type shall enshure the
 *                                      returned oyCMMptr_s is specific to the
 *                                      calling module.
 *  @return                             the CMM specific oyCMMptr_s; It is owned
 *                                      by the CMM.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2009/11/05
 */
oyCMMptr_s * oyCMMptrLookUpFromObject( oyStruct_s        * data,
                                       const char        * data_type )
{
  oyStruct_s * s = data;
  int error = !s;
  oyCMMptr_s * cmm_ptr = 0;

  if(error <= 0 && !data_type)
    error = !data_type;

  if(error <= 0)
  {
    const char * tmp = 0;
    tmp = oyObject_GetName( s->oy_, oyNAME_NICK );
    cmm_ptr = oyCMMptrLookUpFromText( tmp, data_type );
  }

  return cmm_ptr;
}

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

/** @internal
 *  @brief   convert between oyCMMptr_s data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int          oyCMMptr_ConvertData    ( oyCMMptr_s        * cmm_ptr,
                                       oyCMMptr_s        * cmm_ptr_out,
                                       oyFilterNode_s    * node )
{
  int error = !cmm_ptr || !cmm_ptr_out;
  oyCMMapi6_s * api6 = 0;
  char * reg = 0, * tmp = 0;

  if(error <= 0)
  {
    reg = oyStringCopy_( "//", oyAllocateFunc_ );
    tmp = oyFilterRegistrationToText( node->core->registration_,
                                      oyFILTER_REG_TYPE,0);
    STRING_ADD( reg, tmp );
    oyFree_m_( tmp );
    STRING_ADD( reg, "/" );
    STRING_ADD( reg, cmm_ptr->resource );
    STRING_ADD( reg, "_" );
    STRING_ADD( reg, cmm_ptr_out->resource );

    api6 = (oyCMMapi6_s*) oyCMMsGetFilterApi_( 0, reg, oyOBJECT_CMM_API6_S );

    error = !api6;
  }

  if(error <= 0 && api6->oyCMMdata_Convert)
    error = api6->oyCMMdata_Convert( cmm_ptr, cmm_ptr_out, node );
  else
    error = 1;

  if(error)
    WARNc_S("Could not convert context");

  return error;
}
