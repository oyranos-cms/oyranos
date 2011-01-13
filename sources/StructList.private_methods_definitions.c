/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
/*oyHandle_s **    oyStructList_GetRaw_( oyStructList_s    * list )
{
  int error = 0;
  oyStructList_s_ * s = (oyStructList_s_*)list;
  oyHandle_s ** p = 0;

  if(error <= 0)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
    p = s->ptr_;

  return p;
}*/

/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_Get_   ( oyStructList_s    * list,
                                       int                 pos )
{
  int n = 0;
  oyStructList_s_ * s = (oyStructList_s_*)list;
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

/** @internal
 *  @brief oyStructList_s pointer referencing
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int              oyStructList_ReferenceAt_( oyStructList_s * list,
                                       int                 pos )
{
  int n = 0;
  int error = 0;
  oyStructList_s_ * s = (oyStructList_s_*)list;
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
      p = p->copy( p, 0 );
  }

  return !p;
}

/** @internal
 *  Function oyStructList_Create
 *  @brief   create a new oyStruct_s list
 *
 *  @param         parent_type         type of parent object
 *  @param         list_name           optional list name
 *  @param         object              the optional object
 *  @return                            a empty list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
oyStructList_s * oyStructList_Create ( oyOBJECT_e          parent_type,
                                       const char        * list_name,
                                       oyObject_s          object )
{
  oyStructList_s_ * s = (oyStructList_s_*)oyStructList_New(object);

  if(!s)
    return (oyStructList_s*)s;

  s->parent_type_ = parent_type;
  if(list_name)
    s->list_name = oyStringAppend_(0, list_name, s->oy_->allocateFunc_);

  return (oyStructList_s*)s;
}
