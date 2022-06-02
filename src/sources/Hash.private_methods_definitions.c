/** @internal
 *  Function  oyHash_Get_
 *  @memberof oyHash_s
 *  @brief    Get a new Oyranos cache entry
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/03/13
 *  @since   2007/11/24 (Oyranos: 0.1.8)
 */
oyHash_s_ *         oyHash_Get_       ( const char        * hash_text,
                                        oyObject_s          object )
{
  oyHash_s_ * s = 0;
  int error = !hash_text;

  if(error <= 0)
  {
    s = oyHash_New_(object);
    error = !s;
  }

  if(error <= 0)
  {
    if(oyStrlen_(hash_text) < OY_HASH_SIZE*2-1)
      memcpy(s->oy_->hash_ptr_, hash_text, oyStrlen_(hash_text)+1);
    else
      error = oyMiscBlobGetHash_( (void*)hash_text, oyStrlen_(hash_text), 0,
                                  s->oy_->hash_ptr_ );
  }

  if(error <= 0)
    error = oyObject_SetName(s->oy_, hash_text, oyNAME_NAME);

  return s;
}

/** @internal
 *  Function  oyHash_IsOf_
 *  @memberof oyHash_s
 *  @brief    Hash is of type
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_IsOf_      ( oyHash_s_         * hash,
                                       oyOBJECT_e          type )
{
  return (hash && hash->oy_struct && hash->oy_struct->type_ == type);
}

/** @internal
 *  Function  oyHash_SetPointer_
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
int                oyHash_SetPointer_( oyHash_s_         * hash,
                                       oyStruct_s        * obj )
{
  if(hash)
  {
    if(hash->oy_struct && hash->oy_struct->release)
      hash->oy_struct->release( &hash->oy_struct );
    if(obj && obj->copy)
    {
      hash->oy_struct = obj->copy( obj, 0 );
      if(oy_debug_objects >= 0)
        oyObjectDebugMessage_( obj->oy_, __func__, oyStructTypeToText(obj->type_) );
    } else
      hash->oy_struct = obj;
    return 0;
  } else
    return 1;
}

/** @internal
 *  Function  oyHash_GetPointer_
 *  @memberof oyHash_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
oyStruct_s *       oyHash_GetPointer_( oyHash_s_         * hash,
                                       oyOBJECT_e          type )
{
  if(oyHash_IsOf_( hash, type))
  {
    if(hash->oy_struct && hash->oy_struct->copy)
      hash->oy_struct = hash->oy_struct->copy( hash->oy_struct, 0 );
    return hash->oy_struct;
  }
  else
    return 0;
}
