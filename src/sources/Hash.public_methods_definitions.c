/** Function  oyHash_Create
 *  @memberof oyHash_s
 *  @brief    Create a new Oyranos cache entry
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/17
 *  @since   2007/11/24 (Oyranos: 0.1.8)
 */
oyHash_s *         oyHash_Create      ( const char        * hash_text,
                                        oyObject_s          object )
{
  return (oyHash_s*) oyHash_Get_( hash_text, object );
}

/** Function  oyHash_GetType
 *  @memberof oyHash_s
 *  @brief    get Hash entries type
 *
 *  @since    Oyranos: version 0.3.0
 *  @version  (API 0.3.0)
 */
oyOBJECT_e         oyHash_GetType    ( oyHash_s          * hash )
{
  oyHash_s_ * s = (oyHash_s_*)hash;
  if(s && s->oy_struct)
    return s->oy_struct->type_;
  else
    return oyOBJECT_NONE;
}

/** Function  oyHash_IsOf
 *  @memberof oyHash_s
 *  @brief    Hash is of type
 *
 *  @since Oyranos: version 0.3.0
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_IsOf       ( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  return oyHash_IsOf_((oyHash_s_*)hash, type);
}

/** Function  oyHash_SetPointer
 *  @memberof oyHash_s
 *
 *  @param[in,out] hash                the to be set hash
 *  @param[in,out] obj                 the to be referenced object
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/12/03 (Oyranos: 0.3.0)
 *  @date    2011/01/05
 */
int                oyHash_SetPointer ( oyHash_s          * hash,
                                       oyStruct_s        * obj )
{
  return oyHash_SetPointer_((oyHash_s_*)hash, obj);
}

/** Function  oyHash_GetPointer
 *  @memberof oyHash_s
 *
 *  @version   Oyranos: 0.3.0
 *  @since     2011/01/05 (Oyranos: 0.3.0)
 */
oyStruct_s *       oyHash_GetPointer ( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  return oyHash_GetPointer_((oyHash_s_*)hash, type);
}

