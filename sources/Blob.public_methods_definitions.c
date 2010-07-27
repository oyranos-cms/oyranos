/** Function oyBlob_SetFromData
 *  @memberof oyBlob_s
 *  @brief   set value from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @param[in]     ptr                 copy the data into the blob object
 *  @param[in]     size                data size; 0 means the pointer is not owned by the object.
 *  @param[in]     type                data type; assuming 8 byte with typical
 *                                     4 byte content
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/06/12
 */
int            oyBlob_SetFromData    ( oyBlob_s          * blob,
                                       oyPointer           ptr,
                                       size_t              size,
                                       const char        * type )
{
  oyBlob_s * s = blob;
  int error = !s || s->type_ != oyOBJECT_BLOB_S;

  oyCheckType__m( oyOBJECT_BLOB_S, return 1 )

  if(error <= 0)
  {
    if(s->ptr && !(s->flags & 0x01))
      s->oy_->deallocateFunc_( s->ptr );
    s->size = 0;

    if(size)
    {
      s->ptr = s->oy_->allocateFunc_( size );
      error = !s->ptr;
      s->flags = 0;
    } else
      s->flags = 0x01;
  }

  if(error <= 0)
  {
    if(size)
      error = !memcpy( s->ptr, ptr, size );
    else
      s->ptr = ptr;
  }

  if(error <= 0)
    s->size = size;

  if(error <= 0 && type)
    error = !memcpy( s->type, type, 8 );

  return error;
}

/** Function oyBlob_SetFromStatic
 *  @memberof oyBlob_s
 *  @brief   set value from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @param[in]     ptr                 move the data into the blob object
 *  @param[in]     size                data size
 *  @param[in]     type                data type; assuming 8 byte with typical
 *                                     4 byte content
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/26 (Oyranos: 0.1.10)
 *  @date    2009/07/26
 */
int            oyBlob_SetFromStatic  ( oyBlob_s          * blob,
                                       const oyPointer     ptr,
                                       size_t              size,
                                       const char        * type )
{
  oyBlob_s * s = blob;
  int error = !s || s->type_ != oyOBJECT_BLOB_S;

  oyCheckType__m( oyOBJECT_BLOB_S, return 1 )

  if(error <= 0)
  {
    if(s->ptr && !(s->flags & 0x01))
      s->oy_->deallocateFunc_( s->ptr );
    s->size = 0;

    s->ptr = ptr;
    error = !s->ptr;
    s->flags = 0x01;
  }

  if(error <= 0)
    s->size = size;

  if(error <= 0 && type)
    error = !memcpy( s->type, type, 8 );

  return error;
}
