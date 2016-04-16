/** Function  oyStructList_GetRaw_
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer access
 *
 *  Unused?? Consider changing the name to oyStructList_GetArray_()??
 *  Get a pointer to the internal array of oyStruct_s objects
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyStruct_s **    oyStructList_GetRaw_( oyStructList_s_   * list )
{
  int error = 0;
  oyStructList_s_ * s = list;
  oyStruct_s ** p = 0;

  if(error <= 0)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
    p = s->ptr_;

  return p;
}

/** Function  oyStructList_Get_
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer access
 *  @internal
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_Get_   ( oyStructList_s_   * list,
                                       int                 pos )
{
  int n = 0;
  oyStructList_s_ * s = list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(error <= 0)
    if(s->type_ != oyOBJECT_STRUCT_LIST_S)
      error = 1;

  if(error <= 0)
    n = s->n_;

  if(error <= 0)
    if(pos >= 0 && n > pos && s->ptr_[pos])
      obj = s->ptr_[pos];

  return obj;
}

/** Function  oyStructList_ReferenceAt_
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer referencing
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int              oyStructList_ReferenceAt_(oyStructList_s_ * list,
                                           int               pos )
{
  int n = 0;
  int error = 0;
  oyStructList_s_ * s = list;
  oyStruct_s * p = 0;

  if(s)
    n = s->n_;
  else
    error = 1;

  if(error <= 0)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  if(pos >= 0 && n > pos && s->ptr_[pos])
  {
    p = s->ptr_[pos];
    error = !(p && p->copy);

    if(error <= 0)
    {
      p = p->copy( p, 0 );
      if(oy_debug_objects)
        oyObjectDebugMessage_( p->oy_, __func__, oyStructTypeToText(p->type_) );
    }
  }

  return !p;
}

/** Function  oyStructList_GetType_
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer access
 *  @internal
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetType_(oyStructList_s_   * list,
                                       int                 pos,
                                       oyOBJECT_e          type )
{
  oyStruct_s * obj = oyStructList_Get_( list, pos );

  if(obj && obj->type_ != type)
    obj = 0;
  return obj;
}
