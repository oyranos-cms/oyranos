/** Function  oyHash_CopyRef_
 *  @memberof oyHash_s
 *  @brief    Copy a Oyranos cache entry
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s_ *   oyHash_CopyRef_         ( oyHash_s_         * entry,
                                        oyObject_s          object )
{
  oyHash_s_ * s = entry;
  int error = !s;

  if(error <= 0)
    if(s->type_ != oyOBJECT_HASH_S)
      error = 1;

  if(error <= 0)
    oyObject_Copy( s->oy_ );

  return s;
}

/** @internal
 *  @memberof oyHash_s
 *  @brief   get a new Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s *         oyHash_Get_       ( const char        * hash_text,
                                       oyObject_s          object )
{
  oyHash_s * s = 0;
  int error = !hash_text;
  uint32_t * val = 0;

  if(error <= 0)
  {
    s = oyHash_New_(object);
    error = !s;
  }

  if(error <= 0)
  {
    val = (uint32_t*) s->oy_->hash_ptr_;

    if(oyStrlen_(hash_text) < OY_HASH_SIZE*2-1)
      memcpy(s->oy_->hash_ptr_, hash_text, oyStrlen_(hash_text)+1);
    else
#if 0
      error = oyMiscBlobGetMD5_( (void*)hash_text, oyStrlen_(hash_text),
                                 s->oy_->hash_ );
#else
      (*val) = oyMiscBlobGetL3_( (void*)hash_text, oyStrlen_(hash_text) );
#endif
  }

  if(error <= 0)
    error = oyObject_SetName(s->oy_, hash_text, oyNAME_NAME);

  return s;
}

/** @internal
 *  @memberof oyHash_s
 *  @brief hash is of type
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_IsOf_      ( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  return (hash && hash->entry && hash->entry->type_ == type);
}

/** @internal
 *  @memberof oyHash_s
 *
 *  @param[in,out] hash                the to be set hash
 *  @param[in,out] obj                 the to be referenced object
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/03 (Oyranos: 0.1.8)
 *  @date    2009/11/05
 */
int                oyHash_SetPointer_( oyHash_s          * hash,
                                       oyStruct_s        * obj )
{
  if(hash)
  {
    if(obj && obj->copy)
      hash->entry = obj->copy( obj, 0 );
    else
      hash->entry = obj;
    return 0;
  } else
    return 1;
}

/** @internal
 *  @memberof oyHash_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
oyStruct_s *       oyHash_GetPointer_( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  if(oyHash_IsOf_( hash, type))
    return hash->entry;
  else
    return 0;
}
