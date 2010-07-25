/** Function oyOption_FromRegistration
 *  @memberof oyOption_s
 *  @brief   new option with registration and value filled from DB if available
 *
 *  FIXME value filled from DB if available???
 *
 *  @param         registration        no or full qualified registration
 *  @param         object              the optional object
 *  @return                            the option
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 *  @date    2009/01/24
 */
oyOption_s *   oyOption_FromRegistration( const char        * registration,
                                         oyObject_s          object )
{
  oyOption_s *s = oyOption_New( object );

  if(registration)
  {
    if(!registration[0] ||
       !oyStrrchr_( registration, OY_SLASH_C ))
    {
      WARNc2_S("%s: %s",
               "passed a incomplete registration string to option creation",
               registration );
      oyOption_Release( &s );
      return 0;
    } else
      oyOption_SetRegistration( s, registration );
  }

  return s;
}

/** Function oyOption_FromDB
 *  @memberof oyOption_s
 *  @brief   new option with registration and value filled from DB if available
 *
 *  @param         registration        no or full qualified registration
 *  @param         object              the optional object
 *  @return                            the option
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 *  @date    2009/01/24
 */
oyOption_s *   oyOption_FromDB       ( const char        * registration,
                                       oyObject_s          object )
{
  int error = !registration;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    /** This is merely a wrapper to oyOption_New() and
     *  oyOption_SetValueFromDB(). */
    o = oyOption_New( object );
    error = oyOption_SetValueFromDB( o );
    ((oyOption_s_*)o)->source = oyOPTIONSOURCE_DATA;
  }

  return o;
}

/** Function oyOption_GetId
 *  @memberof oyOption_s
 *  @brief   get the identification number of a option
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2008/07/10
 */
int            oyOption_GetId        ( oyOption_s        * obj )
{
  oyOption_s_ * s = (oyOption_s_*)obj;
  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTION_S, return -1 )

  return oyOption_GetId_( s );
}

/** Function oyOption_GetText
 *  @memberof oyOption_s
 *  @brief   get a text dump
 *
 *  Only oyOption_s::value is written.
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XFORMS element
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - option registration name with key and without
 *                         key attributes or value
 *
 *  @param[in,out] obj                 the option
 *  @param         type                oyNAME_NICK is equal to an ID
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 *  @date    2009/08/30
 */
const char *   oyOption_GetText      ( oyOption_s        * obj,
                                       oyNAME_e            type )
{
  int error = !obj;
  const char * erg = 0;
  oyValue_u * v = 0;
  oyOption_s_ * s = obj;

  if(s)
    oyCheckType__m( oyOBJECT_OPTION_S, return 0; )

  if(error <= 0)
    v = s->value;

  error = !v;

  /** Iterate into oyOptions_s objects. */
  if(error <= 0)
  {
    oyOption_s_ * o = s;
    if(o->value_type == oyVAL_STRUCT  && o->value->oy_struct &&
       o->value->oy_struct->type_ == oyOBJECT_OPTIONS_S)
    {
      oyOptions_GetText( (oyOptions_s*)o->value->oy_struct, type );
    }
  }

  if(error <= 0)
    if(type == oyNAME_DESCRIPTION)
    {
      char * text = oyStringCopy_(((oyOption_s_*)obj)->registration, oyAllocateFunc_),
           * tmp = oyStrrchr_(text, '/');
      if(oyStrchr_(tmp, '.'))
      {
        tmp = oyStrchr_(tmp, '.');
        *tmp = 0;
      }
      error = oyObject_SetName( obj->oy_, text, type );
      oyFree_m_(text);
    }

  if(error <= 0 &&
     ( type == oyNAME_NICK || type == oyNAME_NAME ))
  {
    int n = 1, i = 0, j;
    char * tmp = 0,
         * text = 0;
    char ** list = 0;

    if(!oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ))
      oyOption_GetText(obj, oyNAME_DESCRIPTION);


    if(type == oyNAME_NICK)
    {
      STRING_ADD ( text, oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ) );
      STRING_ADD ( text, ":" );
    } else if(type == oyNAME_NAME)
    {
      list = oyStringSplit_( oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ),
                             '/', &n, oyAllocateFunc_);
      for( i = 0; i < n; ++i )
      {
        for(j = 0; j < i; ++j)
          STRING_ADD ( text, " " );
        STRING_ADD ( text, "<" );
        STRING_ADD ( text, list[i] );
        if(i+1==n)
          STRING_ADD ( text, ">" );
        else
          STRING_ADD ( text, ">\n" );
      }

      oyStringListRelease_( &list, n, oyDeAllocateFunc_ );
    }

    tmp = oyOption_GetValueText( obj, oyAllocateFunc_ );
    STRING_ADD ( text, tmp );

    if(type == oyNAME_NAME)
    {
      list = oyStringSplit_( oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ),
                             '/', &n, oyAllocateFunc_);
      for( i = n-1; i >= 0; --i )
      {
        if(i+1 < n)
        for(j = 0; j < i; ++j)
          STRING_ADD ( text, " " );
        STRING_ADD ( text, "</" );
        STRING_ADD ( text, list[i] );
        if(i)
          STRING_ADD ( text, ">\n" );
        else
          STRING_ADD ( text, ">" );
      }

      oyStringListRelease_( &list, n, oyDeAllocateFunc_ );
    }

    error = oyObject_SetName( obj->oy_, text, type );

    oyFree_m_( tmp );
    oyFree_m_( text );
  }

  erg = oyObject_GetName( obj->oy_, type );

  return erg;
}

/** Function oyOption_SetFromText
 *  @memberof oyOption_s
 *  @brief   set a option value from a string
 *
 *  @param         obj                 the option
 *  @param         text                the text to set
 *  @param         flags               possible is OY_STRING_LIST
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/25 (Oyranos: 0.1.9)
 *  @date    2009/08/21
 */
int            oyOption_SetFromText  ( oyOption_s        * obj,
                                       const char        * text,
                                       uint32_t            flags )
{
  oyOption_s_ * s = (oyOption_s_*)obj;
  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTION_S, return -1 )

  return oyOption_SetFromText_( s, text, flags );
}

/** Function oyOption_GetValueText
 *  @memberof oyOption_s
 *  @brief   get value as a text dump
 *
 *  @param         obj                 the option
 *  @param         allocateFunc        user allocator
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/05 (Oyranos: 0.1.9)
 *  @date    2009/08/17
 */
char *         oyOption_GetValueText ( oyOption_s        * obj,
                                       oyAlloc_f           allocateFunc )
{
  oyOption_s_ * s = (oyOption_s_*)obj;
  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  return oyOption_GetValueText_( s, allocateFunc );
}

/** Function oyOption_SetFromInt
 *  @memberof oyOption_s
 *  @brief   set a integer
 *
 *  @param[in,out] obj                 the option
 *  @param         integer             the value
 *  @param         pos                 position in a list
 *  @param         flags               unused
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/05/04
 */
int            oyOption_SetFromInt   ( oyOption_s        * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags )
{
  oyOption_s_ * s = (oyOption_s_*)obj;
  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTION_S, return -1 )

  return oyOption_SetFromInt_( s, integer, pos, flags);
}

/** Function oyOption_GetValueInt
 *  @memberof oyOption_s
 *  @brief   get a integer
 *
 *  @param[in,out] obj                 the option
 *  @param         pos                 position in a list
 *  @return                            integer
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/05/04
 */
int32_t        oyOption_GetValueInt  ( oyOption_s        * obj,
                                       int                 pos )
{
  oyOption_s_ * s = (oyOption_s_*)obj;
  int error = !s;
  int32_t result = 0;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  if(error <= 0)
  {
    if( s->value_type == oyVAL_INT_LIST &&
        s->value->int32_list &&
        s->value->int32_list[0] > pos )
      result = s->value->int32_list[pos + 1];
    else if(s->value_type == oyVAL_INT)
      result = s->value->int32;
  }

  return result;
}

/** Function oyOption_SetFromDouble
 *  @memberof oyOption_s
 *  @brief   set a double value
 *
 *  @param[in,out] obj                 the option
 *  @param         floating_point      the value
 *  @param         pos                 position in a list
 *  @param         flags               unused
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/03 (Oyranos: 0.1.10)
 *  @date    2009/08/03
 */
int            oyOption_SetFromDouble( oyOption_s        * obj,
                                       double              floating_point,
                                       int                 pos,
                                       uint32_t            flags )
{
  int error = !obj;
  oyOption_s_ * s = (oyOption_s_*)obj;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  if(error <= 0)
  {
    if(s->value && 0 /*flags & OY_CLEAR*/)
    {
      oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

      if(s->value_type == oyVAL_DOUBLE && s->value)
        if(s->value->dbl == floating_point)
          return error;

      if(s->value_type == oyVAL_DOUBLE_LIST && s->value)
        if(0 <= pos && pos < s->value->dbl_list[0] &&
           s->value->dbl_list[1 + pos] == floating_point)
          return error;

      oyValueRelease( &s->value, s->value_type, deallocateFunc );
    }

    if(!s->value)
    {
      oyAllocHelper_m_( s->value, oyValue_u, 1,
                        s->oy_->allocateFunc_,
                        error = 1 );
      if(pos == 0 &&
         s->value_type != oyVAL_DOUBLE_LIST)
        s->value_type = oyVAL_DOUBLE;
      else
        s->value_type = oyVAL_DOUBLE_LIST;
    }

    if(!error && pos > 0 &&
       (s->value_type != oyVAL_DOUBLE_LIST ||
        (s->value_type == oyVAL_DOUBLE_LIST &&
         (!s->value->dbl_list || pos >= s->value->dbl_list[0]))))
    {
      double * old_list = 0,
               old_int = 0;

      if(s->value_type == oyVAL_DOUBLE_LIST)
        old_list = s->value->dbl_list;
      if(s->value_type == oyVAL_DOUBLE)
        old_int = s->value->dbl;

      s->value->dbl_list = 0;
      oyAllocHelper_m_( s->value->dbl_list, double, pos + 2,
                        s->oy_->allocateFunc_,
                        error = 1 );

      if(!error && old_list)
      {
        memcpy( s->value->dbl_list, old_list,
                (old_list[0] + 1) * sizeof(double) );
        s->oy_->deallocateFunc_( old_list ); old_list = 0;
      }

      if(!error && old_int)
        s->value->dbl_list[1] = old_int;

      s->value_type = oyVAL_DOUBLE_LIST;
      s->value->dbl_list[0] = pos + 1;
    }

    if(s->value_type == oyVAL_DOUBLE)
      s->value->dbl = floating_point;
    else
      s->value->dbl_list[pos+1] = floating_point;

    s->flags |= oyOPTIONATTRIBUTE_EDIT;
    oyStruct_ObserverSignal( (oyStruct_s*)s, oySIGNAL_DATA_CHANGED, 0 );
  }

  return error;
}

/** Function oyOption_GetValueDouble
 *  @memberof oyOption_s
 *  @brief   get a double
 *
 *  @param[in,out] obj                 the option
 *  @param         pos                 position in a list
 *  @return                            double
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/03 (Oyranos: 0.1.10)
 *  @date    2009/08/03
 */
double         oyOption_GetValueDouble(oyOption_s        * obj,
                                       int                 pos )
{
  oyOption_s_ * s = (oyOption_s_*)obj;
  int error = !s;
  double result = 0;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  if(error <= 0)
  {
    if( s->value_type == oyVAL_DOUBLE_LIST &&
        s->value->dbl_list &&
        s->value->dbl_list[0] > pos )
      result = s->value->dbl_list[pos + 1];
    else if(s->value_type == oyVAL_DOUBLE)
      result = s->value->dbl;
  }

  return result;
}

/** Function oyOption_Clear
 *  @memberof oyOption_s
 *  @brief   clear a option
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
int            oyOption_Clear        ( oyOption_s        * obj )
{
  int error = !obj;
  oyOption_s_ * s = (oyOption_s_*)obj;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return -1 )

  s->flags = 0;

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyValueRelease( &s->value, s->value_type, deallocateFunc );

    s->value_type = 0;

    if(s->registration)
      deallocateFunc( s->registration );
    s->registration = 0;
  }

  oyStruct_ObserverSignal( (oyStruct_s*)s, oySIGNAL_DATA_CHANGED, 0 );

  return 0;
}

/** Function oyOption_SetFromData
 *  @memberof oyOption_s
 *  @brief   set value from a data blob
 *
 *  @param[in]     option              the option
 *  @param[in]     ptr                 data
 *  @param[in]     size                data size
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/04 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
int            oyOption_SetFromData  ( oyOption_s        * option,
                                       oyPointer           ptr,
                                       size_t              size )
{
  oyAlloc_f allocateFunc_ = oyAllocateFunc_;
  oyDeAlloc_f deallocateFunc_ = oyDeAllocateFunc_;
  int error = !option;
  oyOption_s_ * s = (oyOption_s_*)option;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 1 )

  if(error <= 0)
  {
    if(s->oy_)
    {
      allocateFunc_ = s->oy_->allocateFunc_;
      deallocateFunc_ = s->oy_->deallocateFunc_;
    }

    if((s->value && s->value_type == oyVAL_STRUCT &&
         (((s->value->oy_struct->type_ == oyOBJECT_BLOB_S &&
           ((oyBlob_s*)(s->value->oy_struct))->ptr == ptr)) ||
          (s->value->oy_struct->type_ == oyOBJECT_CMM_POINTER_S &&
           ((oyCMMptr_s*)(s->value->oy_struct))->ptr == ptr))))
      return error;

    oyValueClear( s->value, s->value_type, deallocateFunc_ );
    if(!s->value)
      s->value = allocateFunc_(sizeof(oyValue_u));
    error = !s->value;
    if(!error)
      memset( s->value, 0, sizeof(oyValue_u) );
  }

  if(error <= 0)
  {
    s->value->oy_struct = (oyStruct_s*) oyBlob_New( 0 );
    error = !s->value->oy_struct;
    s->value_type = oyVAL_STRUCT;
  }

  if(error <= 0)
  {
    error = oyBlob_SetFromData( (oyBlob_s*) s->value->oy_struct,
                                ptr, size, 0 );
    oyStruct_ObserverSignal( (oyStruct_s*)s, oySIGNAL_DATA_CHANGED, 0 );
  }

  return error;
}

/** Function oyOption_GetData
 *  @memberof oyOption_s
 *  @brief   get the data blob
 *
 *  With data size being greater than zero, the returned pointer is owned by the
 *  caller.
 *
 *  @param[in]     option              the option
 *  @param[out]    size                data size
 *  @param[in]     allocateFunc        user allocator
 *  @return                            data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/04 (Oyranos: 0.1.10)
 *  @date    2009/07/22
 */
oyPointer      oyOption_GetData      ( oyOption_s        * option,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !option || option->type_ != oyOBJECT_OPTION_S;
  oyPointer ptr = 0;
  size_t size_ = 0;
  oyBlob_s * blob = 0;
  oyOption_s * s = option;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  if(error <= 0)
  {
    if(!allocateFunc &&
       option->oy_)
      allocateFunc = option->oy_->allocateFunc_;

    if(!(option->value && option->value_type == oyVAL_STRUCT &&
         (((option->value->oy_struct->type_ == oyOBJECT_BLOB_S &&
           ((oyBlob_s*)(option->value->oy_struct))->ptr)) ||
          option->value->oy_struct->type_ == oyOBJECT_CMM_POINTER_S)))
      error = 1;
  }

  if( error <= 0 && option->value->oy_struct->type_ == oyOBJECT_BLOB_S)
  {
    blob = (oyBlob_s*)option->value->oy_struct;
    size_ = blob->size;
    if(size_)
    {
      ptr = oyAllocateWrapFunc_( size_, allocateFunc );
      error = !ptr;

      if(error <= 0)
      {
        error = !memcpy( ptr, blob->ptr, size_ );
        if(error <= 0 && size)
          *size = size_;
      }

    } else
      ptr = blob->ptr;
  }


  if(error)
    ptr = 0;

  return ptr;
}

/** Function oyOption_SetRegistration
 *  @memberof oyOption_s
 *  @brief   set the registration
 *
 *  @param[in]     option              the option
 *  @param[in]     registration        the option's registration and key name
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 *  @date    2009/01/24
 */
int            oyOption_SetRegistration (
                                       oyOption_s        * option,
                                       const char        * registration )
{
  int error = !option;
  if(error <= 0)
    option->registration = oyStringCopy_( registration,
                                          option->oy_->allocateFunc_ );

  return error;
}

/** Function oyOption_GetRegistration
 *  @memberof oyOption_s
 *  @brief   get the registration
 *
 *  @param[in]     option              the option
 *  @return                            registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/02/26 (Oyranos: 0.1.10)
 *  @date    2010/02/26
 */
const char *   oyOption_GetRegistration (
                                       oyOption_s        * option )
{
  oyOption_s * s = option;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  return s->registration;
}

/** Function oyOption_SetValueFromDB
 *  @memberof oyOption_s
 *  @brief   value filled from DB if available
 *
 *  @param         option              the option
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 *  @date    2009/05/25
 */
int            oyOption_SetValueFromDB  ( oyOption_s        * option )
{
  int error = !option || !option->registration;
  char * text = 0;
  oyPointer ptr = 0;
  size_t size = 0;

  oyExportStart_(EXPORT_SETTING);

  if(error <= 0)
    text = oyGetKeyString_( oyOption_GetText( option, oyNAME_DESCRIPTION),
                            oyAllocateFunc_ );

  if(error <= 0)
  {
    /** Change the option value only if something was found in the DB. */
    if(text && text[0])
    {
      oyOption_SetFromText( option, text, 0 );
      option->source = oyOPTIONSOURCE_DATA;
    }
    else
    {
      ptr = oyGetKeyBinary_( option->registration, &size, oyAllocateFunc_ );
      if(ptr && size)
      {
        oyOption_SetFromData( option, ptr, size );
        option->source = oyOPTIONSOURCE_DATA;
        oyFree_m_( ptr );
      }
    }
  }

  if(text)
    oyFree_m_( text );

  oyExportEnd_();

  return error;
}

/** Function oyOption_SetStruct
 *  @memberof oyOption_s
 *  @brief   value filled by a oyStruct_s object
 *
 *  @param         option              the option
 *  @param         s                   the Oyranos style object
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 *  @date    2009/01/28
 */
int            oyOption_StructMoveIn ( oyOption_s        * option,
                                       oyStruct_s       ** s )
{
  int error = !option;

  if(error <= 0)
  {
    if(option->value)
    {
      oyDeAlloc_f deallocateFunc = option->oy_->deallocateFunc_;

      oyValueRelease( &option->value, option->value_type, deallocateFunc );

      option->value_type = 0;
    }

    oyAllocHelper_m_( option->value, oyValue_u, 1, option->oy_->allocateFunc_,
                      error = 1 );
  }

  if(error <= 0)
  {
    option->value->oy_struct = *s;
    *s = 0;
    option->value_type = oyVAL_STRUCT;
  }

  return error;
}

/** Function oyOption_GetStruct
 *  @memberof oyOption_s
 *  @brief   ask for a oyStruct_s object
 *
 *  @param         option              the option
 *  @param         type                the Oranos oyOBJECT_e object type
 *  @return                            Oyranos struct, as reference if possible
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/09 (Oyranos: 0.1.10)
 *  @date    2009/03/09
 */
oyStruct_s *   oyOption_StructGet    ( oyOption_s        * option,
                                       oyOBJECT_e          type )
{
  oyStruct_s * s = 0;
  oyOption_s * o = option;

  if( o && o->value && o->value_type == oyVAL_STRUCT && o->value->oy_struct &&
      o->value->oy_struct->type_ == type)
  {
    if(o->value->oy_struct->copy)
      s = o->value->oy_struct->copy( o->value->oy_struct, 0 );
    else
      s = o->value->oy_struct;
  }

  return s;
}
