#include <locale.h>  /* setlocale */

#include "oyStructList_s_.h" /* not nice, but direct access is probably faster */
#include "oyranos_i18n.h"

/**
 *  @internal
 *  Function oyOption_Match_
 *  @memberof oyOption_s
 *  @brief   two option key name matches
 *
 *  A registration name match is not required.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
int            oyOption_Match_       ( oyOption_s_        * option_a,
                                       oyOption_s_        * option_b )
{
  int erg = 0;

  if( option_a && option_b )
  {
    char * a =  oyFilterRegistrationToText( option_a->registration,
                                            oyFILTER_REG_TYPE, 0 );
    char * b =  oyFilterRegistrationToText( option_b->registration,
                                            oyFILTER_REG_TYPE, 0 );

    if( oyStrcmp_( a, b ) == 0 )
      erg = 1;

    oyDeAllocateFunc_(a);
    oyDeAllocateFunc_(b);
  }

  return erg;
}

/**
 *  @internal
 *  Function oyOption_UpdateFlags_
 *  @memberof oyOption_s
 *  @brief   set the ::flags member
 *
 *  @date    2015/05/13
 *  @version Oyranos: 0.9.6
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 */
void         oyOption_UpdateFlags_   ( oyOption_s_        * o )
{
  const char * tmp = 0;

  if(o && o->registration && oyStrrchr_( o->registration, '/' ))
  {
      tmp = oyStrrchr_( o->registration, '/' );
      if(oyStrstr_( tmp, "front" ))
         o->flags |= oyOPTIONATTRIBUTE_FRONT;
      if(oyStrstr_( tmp, "advanced" ))
         o->flags |= oyOPTIONATTRIBUTE_ADVANCED;
      tmp = 0;

      o->flags |= oyOPTIONATTRIBUTE_EDIT;
  }
}

/**
 *  @internal
 *  Function oyOption_GetId_
 *  @memberof oyOption_s
 *  @brief   get the identification number of a option
 *
 *  @version Oyranos: 0.1.8
 */
int            oyOption_GetId_       ( oyOption_s_       * obj )
{
  if(obj)
    return obj->id;

  return -1;
}

/**
 *  @internal
 *  Function oyOption_SetFromText_
 *  @memberof oyOption_s
 *  @brief   set a option value from a string
 *
 *  @param         obj                 the option
 *  @param         text                the text to set
 *  @param         flags               possible is OY_STRING_LIST
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.9
 */
int            oyOption_SetFromText_ ( oyOption_s_       * obj,
                                       const char        * text,
                                       uint32_t            flags )
{
  int error = !obj;
  char ** list = 0;
  int n = 0;

  if(error <= 0)
  {
    /* ignore the special case of assigning the same string twice. */
    if(obj->value && obj->value_type == oyVAL_STRING &&
       obj->value->string == text)
      return error;

    if(obj->value)
    {
      oyDeAlloc_f deallocateFunc = obj->oy_->deallocateFunc_;

      if( text )
      {
        int j = 0;
        if( obj->value_type == oyVAL_STRING && obj->value->string )
        {
          if(oyStrcmp_(text, obj->value->string) == 0)
            return error;
        }
        if( obj->value_type == oyVAL_STRING_LIST && obj->value->string_list )
        while(obj->value->string_list[j])
        {
          const char * value = obj->value->string_list[j];

          if(value && oyStrcmp_(value, text))
            return error;
          ++j;
        }

      }
      oyValueRelease( &obj->value, obj->value_type, deallocateFunc );
    }

    obj->value = obj->oy_->allocateFunc_(sizeof(oyValue_u));
    memset( obj->value, 0, sizeof(oyValue_u) );

    if(oyToStringList_m(flags))
    {
      /** Split for flags & OY_STRING_LIST at newline. */
      list = oyStringSplit_( text, '\n', &n, obj->oy_->allocateFunc_ );
      obj->value->string_list = list; list = 0;
      obj->value_type = oyVAL_STRING_LIST;
    } else
    {
      if(text)
        obj->value->string = oyStringCopy_( text, obj->oy_->allocateFunc_ );
      obj->value_type = oyVAL_STRING;
    }
    obj->flags |= oyOPTIONATTRIBUTE_EDIT;
    oyStruct_ObserverSignal( (oyStruct_s*)obj, oySIGNAL_DATA_CHANGED, 0 );
  }

  return error;
}

/**
 *  @internal
 *  Function oyOption_GetValueText_
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
char *         oyOption_GetValueText_( oyOption_s_       * obj,
                                       oyAlloc_f           allocateFunc )
{
  int error = !obj;
  char * erg = 0;
  oyValue_u * v = 0;
  oyStructList_s * oy_struct_list = 0;
  char * text = 0;
  char * save_locale = 0;

  if(error <= 0)
    v = obj->value;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  error = !v;

  save_locale = oyStringCopy_( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_ );
  setlocale(LC_NUMERIC, "C");

  if(error <= 0)
  {
    int n = 1, i = 0;
    char * tmp = oyAllocateFunc_(1024);
    const char * ct = 0;

    switch(obj->value_type)
    {
    case oyVAL_INT_LIST:    n = v->int32_list[0]; break;
    case oyVAL_DOUBLE_LIST: n = (int)v->dbl_list[0]; break;
    case oyVAL_STRING_LIST: n = 0; while( v->string_list[n] ) ++n; break;
    case oyVAL_INT:
    case oyVAL_DOUBLE:
    case oyVAL_STRING:
    case oyVAL_STRUCT:
         n = 1; break;
    }

    if(obj->value_type == oyVAL_STRUCT)
    {
      oy_struct_list = (oyStructList_s*) v->oy_struct;
      if(oy_struct_list)
      {
        if(oy_struct_list->type_ == oyOBJECT_STRUCT_LIST_S)
          n = oyStructList_Count( oy_struct_list );
      } else
        WARNc2_S( "missed \"oy_struct\" member of \"%s\" [%d]",
                  obj->registration, oyObject_GetId(obj->oy_) );
    }

    for(i = 0; i < n; ++i)
    {
      if(obj->value_type == oyVAL_INT)
        oySprintf_(tmp, "%d", v->int32);
      if(obj->value_type == oyVAL_DOUBLE)
        oySprintf_(tmp, "%g", v->dbl);
      if(obj->value_type == oyVAL_INT_LIST)
        oySprintf_(tmp, "%d", v->int32_list[i+1]);
      if(obj->value_type == oyVAL_DOUBLE_LIST)
        oySprintf_(tmp, "%g", v->dbl_list[i+1]);

      if((obj->value_type == oyVAL_INT_LIST ||
          obj->value_type == oyVAL_DOUBLE_LIST) && i)
        STRING_ADD( text, "," );

      switch(obj->value_type)
      {
      case oyVAL_INT:
      case oyVAL_DOUBLE:
      case oyVAL_INT_LIST:
      case oyVAL_DOUBLE_LIST: STRING_ADD( text, tmp ); break;
      case oyVAL_STRING:      STRING_ADD( text, v->string ); break;
      case oyVAL_STRING_LIST: STRING_ADD( text, v->string_list[i] ); break;
      case oyVAL_STRUCT:      break;
      }
      if(obj->value_type == oyVAL_STRUCT)
      {
        oyStruct_s * oy_struct = 0;

        if(oy_struct_list && oy_struct_list->type_ == oyOBJECT_STRUCT_LIST_S)
          oy_struct = oyStructList_Get_( ( oyStructList_s_*)oy_struct_list, i );
        else if(v->oy_struct)
          oy_struct = v->oy_struct;

        if(oy_struct)
        {
          ct = 0;
          /* get explicite name */
          if(oy_struct->oy_)
            ct = oyObject_GetName( oy_struct->oy_, oyNAME_NICK );
          if(!ct)
          /* fall back to oyCMMapi9_s object type lookup */
            ct = oyStruct_GetText( oy_struct, oyNAME_NICK, 0 );
          if(ct)
            STRING_ADD( text, ct );
          if(!ct)
          /* fall back to plain struct type name, if known */
            STRING_ADD ( text, oyStructTypeToText(oy_struct->type_) );
        }
      }
    }

    if(text)
      erg = oyStringCopy_( text, allocateFunc );

    oyFree_m_( tmp );
    if(!text)
    {
      DBG_NUM2_S( "missed value in \"%s\" [%d]", obj->registration,
                    oyObject_GetId(obj->oy_) );
    } else
      oyFree_m_( text );
  }
  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );

  return erg;
}

/**
 *  @internal
 *  Function oyOption_SetFromInt_
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
int            oyOption_SetFromInt_  ( oyOption_s_       * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags )
{
  int error = !obj;
  oyOption_s_ * s = obj;

  if(!obj)
    return error;

  if(error <= 0)
  {
    if(s->value && 0 /*flags & OY_CLEAR*/)
    {
      oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

      if(s->value_type == oyVAL_INT && obj->value)
        if(s->value->int32 == integer)
          return error;

      if(s->value_type == oyVAL_INT_LIST && s->value)
        if(0 <= pos && pos < s->value->int32_list[0] &&
           s->value->int32_list[1 + pos] == integer)
          return error;

      oyValueRelease( &s->value, s->value_type, deallocateFunc );
    }

    if(!s->value)
    {
      oyAllocHelper_m_( s->value, oyValue_u, 1,
                        s->oy_->allocateFunc_,
                        error = 1 );
      if(pos == 0 &&
         s->value_type != oyVAL_INT_LIST)
        s->value_type = oyVAL_INT;
      else
        s->value_type = oyVAL_INT_LIST;
    }

    if(!error && pos > 0 &&
       (s->value_type != oyVAL_INT_LIST ||
        (s->value_type == oyVAL_INT_LIST &&
         (!s->value->int32_list || pos >= s->value->int32_list[0]))))
    {
      int32_t * old_list = 0,
                old_int = 0;

      if(s->value_type == oyVAL_INT_LIST)
        old_list = s->value->int32_list;
      if(s->value_type == oyVAL_INT)
        old_int = s->value->int32;

      s->value->int32_list = 0;
      oyAllocHelper_m_( s->value->int32_list, int32_t, pos + 2,
                        s->oy_->allocateFunc_,
                        error = 1 );

      if(!error && old_list)
      {
        memcpy( s->value->int32_list, old_list,
                (old_list[0] + 1) * sizeof(uint32_t) );
        s->oy_->deallocateFunc_( old_list ); old_list = 0;
      }

      if(!error && old_int)
        s->value->int32_list[1] = old_int;

      s->value_type = oyVAL_INT_LIST;
      s->value->int32_list[0] = pos + 1;
    }

    if(s->value_type == oyVAL_INT)
      s->value->int32 = integer;
    else
      s->value->int32_list[pos+1] = integer;

    s->flags |= oyOPTIONATTRIBUTE_EDIT;
    oyStruct_ObserverSignal( (oyStruct_s*)obj, oySIGNAL_DATA_CHANGED, 0 );
  }

  return error;
}

/**
 *  @internal
 *  Function oyOption_MoveInStruct_
 *  @memberof oyOption_s
 *  @brief   value filled by a oyStruct_s object
 *
 *  @param         option              the option
 *  @param         s                   the Oyranos style object
 *  @return                            error
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/08/13
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 */
int            oyOption_MoveInStruct_( oyOption_s_       * option,
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
