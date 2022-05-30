/** @file oyOption_s.c

   [Template file inheritance graph]
   +-> oyOption_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyOption_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyOption_s_.h"

#include "oyranos_sentinel.h"
#include "oyObserver_s.h"
#include "oyBlob_s_.h"
  


/** Function oyOption_New
 *  @memberof oyOption_s
 *  @brief   allocate a new Option object
 */
OYAPI oyOption_s * OYEXPORT
  oyOption_New( oyObject_s object )
{
  oyObject_s s = object;
  oyOption_s_ * option = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  option = oyOption_New_( s );

  return (oyOption_s*) option;
}

/** Function  oyOption_Copy
 *  @memberof oyOption_s
 *  @brief    Copy or Reference a Option object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     option                 Option struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyOption_s* OYEXPORT
  oyOption_Copy( oyOption_s *option, oyObject_s object )
{
  oyOption_s_ * s = (oyOption_s_*) option;

  if(s)
  {
    oyCheckType__m( oyOBJECT_OPTION_S, return NULL )
  }
  else
    return NULL;

  s = oyOption_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyOption_s" );

  return (oyOption_s*) s;
}
 
/** Function oyOption_Release
 *  @memberof oyOption_s
 *  @brief   release and possibly deallocate a oyOption_s object
 *
 *  @param[in,out] option                 Option struct object
 */
OYAPI int OYEXPORT
  oyOption_Release( oyOption_s **option )
{
  oyOption_s_ * s = 0;

  if(!option || !*option)
    return 0;

  s = (oyOption_s_*) *option;

  oyCheckType__m( oyOBJECT_OPTION_S, return 1 )

  *option = 0;

  return oyOption_Release_( &s );
}



/* Include "Option.public_methods_definitions.c" { */
#include "oyPointer_s.h"

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

/** @memberof oyOption_s
 *  @brief   get a text dump
 *
 *  Only oyOption_s::value is written.
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XFORMS element
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - option registration name with key and without
 *                         key attributes or value
 *  - oyNAME_JSON - JSON formatted string supporting nested JSON
 *
 *  @param[in,out] obj                 the option
 *  @param         type                oyNAME_NICK is equal to an ID
 *  @return                            the text
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/03/20
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 */
const char *   oyOption_GetText      ( oyOption_s        * obj,
                                       oyNAME_e            type )
{
  int error = !obj;
  const char * erg = 0;
  oyValue_u * v = 0;
  oyOption_s_ * s = (oyOption_s_*)obj;

  if(s)
    oyCheckType__m( oyOBJECT_OPTION_S, return 0; )

  if(error <= 0)
  {
    v = s->value;
    if(!v)
      error = -1;
  }

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
      const char * txt = ((oyOption_s_*)obj)->registration;
      char * t = oyjlJsonEscape( txt, OYJL_REGEXP | OYJL_REVERSE );
      char * text = oyStringCopy_(t, oyAllocateFunc_),
           * tmp = text ? oyStrrchr_(text, '/') : NULL;
      if(t) { free(t); t = NULL; }
      if(tmp && oyStrchr_(tmp, '.'))
      {
        tmp = oyStrchr_(tmp, '.');
        *tmp = 0;
      }
      if(text)
      {
        error = oyObject_SetName( obj->oy_, text, type );
        oyFree_m_(text);
      }
    }

  if(error <= 0 &&
     ( type == oyNAME_NICK || type == oyNAME_NAME  || type == oyNAME_JSON))
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
    } else if(type == oyNAME_JSON)
    {
      list = oyStringSplit_( oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ),
                             '/', &n, oyAllocateFunc_);
      for( i = 0; i < n; ++i )
      {
        for(j = 0; j < i; ++j)
          STRING_ADD ( text, " " );
        if(!text) /* opening bracket */
          STRING_ADD ( text, "{ \"" );
        else
          STRING_ADD ( text, " \"" );
        STRING_ADD ( text, list[i] );
        STRING_ADD ( text, "\": {" );
      }

      oyStringListRelease_( &list, n, oyDeAllocateFunc_ );
    }

    if( s->value_type == oyVAL_STRUCT &&
        s->value->oy_struct)
    {
      const char * t = oyStruct_GetText( s->value->oy_struct, type, 0 );
      int pos = 0;
      while(t && isspace(t[pos])) ++pos;
      if(t && t[pos] == '{')
      {
        t = &t[pos+1];
        STRING_ADD ( text, t );
        text[strlen(text)-1] = '\000';
      }
      else
        STRING_ADD ( text, t );
    }
    else
    {
      tmp = oyOption_GetValueText( obj, oyAllocateFunc_ );
      STRING_ADD ( text, tmp );
    }

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
    } else
    if(type == oyNAME_JSON)
    {
      list = oyStringSplit_( oyObject_GetName( obj->oy_, oyNAME_DESCRIPTION ),
                             '/', &n, oyAllocateFunc_);
      for( i = n-1; i >= 0; --i )
      {
        if(i+1 < n)
        {
          for(j = 0; j < i; ++j)
            STRING_ADD ( text, " " );
        }
        STRING_ADD ( text, "}" );
      }
      STRING_ADD ( text, "}" ); /* closing bracket */

      oyStringListRelease_( &list, n, oyDeAllocateFunc_ );
    }

    error = oyObject_SetName( obj->oy_, text, type );

    if(tmp) oyFree_m_( tmp );
    if(text) oyFree_m_( text );
  }

  if(error <= 1 && obj)
    erg = oyObject_GetName( obj->oy_, type );

  return erg;
}

/** Function oyOption_SetFromString
 *  @memberof oyOption_s
 *  @brief   set a option value from a string
 *
 *  Update the flags if necessary.
 *
 *  @param         obj                 the option
 *  @param         text                the text to set
 *  @param         flags               possible is ::OY_STRING_LIST with newline '\n' delimiter
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.2.1
 *  @since   2008/11/25 (Oyranos: 0.1.9)
 *  @date    2011/01/21
 */
int            oyOption_SetFromString( oyOption_s        * obj,
                                       const char        * text,
                                       uint32_t            flags )
{
  int error = 0;
  oyOption_s_ * s = (oyOption_s_*)obj;
  const char * dbg = getenv(OY_DEBUG_OPTION);
  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTION_S, return -1 )

  if(dbg)
  {
    int n = 0;
    char ** list = oyStringSplit_( dbg, ':', &n, oyAllocateFunc_ );
    if( n >= 1 && (strstr(s->registration, list[0] ) || strlen(list[0]) == 0) &&
        (n == 1 || (text && strstr(text, list[1]))) )
    {
      char * t = NULL;
      if(getenv(OY_DEBUG_OPTION_BACKTRACE))
        t = oyBT(0);
      oyMessageFunc_p( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)obj,
                       OY_DBG_FORMAT_ "%s %s", OY_DBG_ARGS_,
                       oyNoEmptyName_m_(text), t?t:"" );
      if(t) oyDeAllocateFunc_(t);
    }
    oyStringListRelease_( &list, n, oyDeAllocateFunc_ );
  }

  error = oyOption_SetFromString_( s, text, flags );
  if(!error)
    oyOption_UpdateFlags_(s);

  return error;
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

/** Function oyOption_GetValueString
 *  @memberof oyOption_s
 *  @brief   get a string
 *
 *  @param         obj                 the option
 *  @param         pos                 position in a list
 *  @return                            the text
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
const char *   oyOption_GetValueString(oyOption_s        * obj,
                                       int                 pos )
{
  oyOption_s_ * s = (oyOption_s_*)obj;
  int error = !s,
      n = 0;
  const char * result = 0;

  if(!s)
    return NULL;

  oyCheckType__m( oyOBJECT_OPTION_S, return NULL )

  if(error <= 0)
  {
    if( s->value_type == oyVAL_STRING_LIST &&
        s->value->string_list)
    {
      while(s->value->string_list[n]) ++n;
      if( n > pos )
        result = s->value->string_list[pos];
    }
    else if(s->value_type == oyVAL_STRING)
      result = s->value->string;
  }

  return result;
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
  int error = 0;
  oyOption_s_ * s = (oyOption_s_*)obj;
  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTION_S, return -1 )

  error = oyOption_SetFromInt_( s, integer, pos, flags);
  if(!error)
    oyOption_UpdateFlags_(s);

  return error;
}

/** Function oyOption_GetValueInt
 *  @memberof oyOption_s
 *  @brief   get a integer
 *
 *  @param[in,out] obj                 the option
 *  @param         pos                 position in a list
 *                                     - -1: get the count
 *  @return                            integer
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/05/24
 *  @since   2009/05/04 (Oyranos: 0.1.10)
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
        s->value->int32_list )
    {
      if( pos == -1 )
        result = s->value->int32_list[0];
      else if( s->value->int32_list[0] > pos )
        result = s->value->int32_list[pos + 1];
    }
    else if( s->value_type == oyVAL_INT )
    {
      if( pos == -1 )
        result = 1;
      else
        result = s->value->int32;
    }
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
                                       uint32_t            flags OY_UNUSED )
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
      oyStruct_AllocHelper_m_( s->value, oyValue_u, 1,
                               s, error = 1 );
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
      oyStruct_AllocHelper_m_( s->value->dbl_list, double, pos + 2,
                               s, error = 1 );

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

    oyOption_UpdateFlags_(s);
  }

  return error;
}

/** Function oyOption_GetValueDouble
 *  @memberof oyOption_s
 *  @brief   get a double
 *
 *  @param[in,out] obj                 the option
 *  @param         pos                 position in a list;
 *                                     - -1: get the count
 *  @return                            double
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/05/24
 *  @since   2009/08/03 (Oyranos: 0.1.10)
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
        s->value->dbl_list )
    {
      if(pos == -1)
        result = s->value->dbl_list[0];
      else if( s->value->dbl_list[0] > pos )
        result = s->value->dbl_list[pos + 1];
    }
    else if(s->value_type == oyVAL_DOUBLE)
    {
      if(pos == -1)
        result = 1;
      else
        result = s->value->dbl;
    }
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

    s->value_type = oyVAL_NONE;

    if(s->registration)
      deallocateFunc( s->registration );
    s->registration = NULL;
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
           ((oyBlob_s_*)(s->value->oy_struct))->ptr == ptr)) ||
          (s->value->oy_struct->type_ == oyOBJECT_POINTER_S &&
           oyPointer_GetPointer((oyPointer_s*)(s->value->oy_struct)) == ptr))))
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

    oyOption_UpdateFlags_(s);
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
  oyPointer ptr = 0;
  size_t size_ = 0;
  oyBlob_s_ * blob = 0;
  int error = !option;
  oyOption_s_ * s = (oyOption_s_*)option;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  if(error <= 0)
  {
    if(!allocateFunc &&
       s->oy_)
      allocateFunc = s->oy_->allocateFunc_;

    if(!(s->value && s->value_type == oyVAL_STRUCT &&
         (((s->value->oy_struct->type_ == oyOBJECT_BLOB_S &&
           ((oyBlob_s_*)(s->value->oy_struct))->ptr)) ||
          s->value->oy_struct->type_ == oyOBJECT_POINTER_S)))
      error = 1;
  }

  if( error <= 0 && s->value->oy_struct->type_ == oyOBJECT_BLOB_S)
  {
    blob = (oyBlob_s_*)s->value->oy_struct;
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
 *  @date    2019/09/20
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 */
int            oyOption_SetRegistration (
                                       oyOption_s        * option,
                                       const char        * registration )
{
  int error = !option;
  oyOption_s_ * s = (oyOption_s_*)option;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 1 )

  if(s->registration)
  {
    oyDeAlloc_f dealloc = oyStruct_GetDeAllocator( (oyStruct_s*) option );
    if(!dealloc) dealloc = oyDeAllocateFunc_;
    dealloc( s->registration );
    s->registration = NULL;
  }

  if(registration)
    s->registration = oyStringCopy_( registration,
                                     s->oy_->allocateFunc_ );

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
  oyOption_s_ * s = (oyOption_s_*)option;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  return s->registration;
}

/** Function oyOption_MoveInStruct
 *  @memberof oyOption_s
 *  @brief   value filled by a oyStruct_s object
 *
 *  @param         option              the option
 *  @param         oystruct            the Oyranos style object
 *  @return                            error
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/08/13
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 */
int            oyOption_MoveInStruct ( oyOption_s        * option,
                                       oyStruct_s       ** oystruct )
{
  int error = !option;
  oyOption_s_ * s = (oyOption_s_*)option;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 1 )

  return oyOption_MoveInStruct_( s, oystruct );
}

/** Function oyOption_GetStruct
 *  @memberof oyOption_s
 *  @brief   ask for a oyStruct_s object
 *
 *  @param         option              the option
 *  @param         type                the Oranos oyOBJECT_e object type
 *  @return                            Oyranos struct, as reference if possible
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/05
 *  @since    2009/03/09 (Oyranos: 0.1.10)
 */
oyStruct_s *   oyOption_GetStruct    ( oyOption_s        * option,
                                       oyOBJECT_e          type )
{
  oyStruct_s * s = 0;
  oyOption_s_ * o = (oyOption_s_*)option;

  if( o && o->value && o->value_type == oyVAL_STRUCT && o->value->oy_struct &&
      o->value->oy_struct->type_ == type)
  {
    if(o->value->oy_struct->copy)
    {
      s = o->value->oy_struct->copy( o->value->oy_struct, 0 );
      if(oy_debug_objects >= 0 && s)
        oyObjectDebugMessage_( s->oy_, __func__,
                               oyStructTypeToText(s->type_) );
    } else
      s = o->value->oy_struct;
  }

  return s;
}

void           oyOption_SetSource    ( oyOption_s        * option,
                                       oyOPTIONSOURCE_e    source )
{
  oyOption_s_ * s = (oyOption_s_*)option;

  if(!s)
    return;

  oyCheckType__m( oyOBJECT_OPTION_S, return )

  s->source = source;
}
oyOPTIONSOURCE_e oyOption_GetSource  ( oyOption_s        * option )
{
  oyOption_s_ * s = (oyOption_s_*)option;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0 )

  return s->source;
}

int            oyOption_GetFlags     ( oyOption_s        * object )
{
  oyOption_s_ * s = (oyOption_s_*)object;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTION_S, return 0)

  return s->flags;
}
int            oyOption_SetFlags     ( oyOption_s        * object,
                                       uint32_t            flags )
{
  oyOption_s_ * s = (oyOption_s_*)object;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTION_S, return 1)

  s->flags = flags;
  return 0;
}

/** Function oyOption_GetValueType
 *  @memberof oyOption_s
 *  @brief   Ask for the type of the value
 *
 *  @param         option              the option
 *  @return                            value type enum
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/05/04
 *  @since    2016/05/04 (Oyranos: 0.9.6)
 */
oyVALUETYPE_e  oyOption_GetValueType ( oyOption_s        * option )
{
  oyOption_s_ * o = (oyOption_s_*)option;

  if( o )
    return o->value_type;

  return oyVAL_NONE;
}


/** Function oyValueCopy
 *  @memberof oyValue_u
 *  @brief   copy a oyValue_u union
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
void           oyValueCopy           ( oyValue_u         * to,
                                       oyValue_u         * from,
                                       oyVALUETYPE_e       type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int n = 0, i;

  if(!from || !to)
    return;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_; 

  switch(type)
  {
  case oyVAL_INT: to->int32 = from->int32; break;
  case oyVAL_INT_LIST:
       if(to->int32_list && deallocateFunc)
       {
         deallocateFunc(to->int32_list);
         to->int32_list = 0;
       }

       if(!from->int32_list)
         return;

       n = from->int32_list[0];

       to->int32_list = allocateFunc( (n+1) * sizeof(int32_t) );
       to->int32_list[0] = n;
       for(i = 1; i <= n; ++i)
         to->int32_list[i] = from->int32_list[i];
       break;
  case oyVAL_DOUBLE: to->dbl = from->dbl; break;
  case oyVAL_DOUBLE_LIST:
       if(to->dbl_list && deallocateFunc)
       {
         deallocateFunc(to->dbl_list);
         to->dbl_list = 0;
       }

       if(!from->dbl_list)
         return;

       n = from->dbl_list[0];

       to->dbl_list = allocateFunc( (n+1) * sizeof(double));

       to->dbl_list[0] = n;
       for(i = 1; i <= n; ++i)
         to->dbl_list[i] = from->dbl_list[i];

       break;
  case oyVAL_STRING:
       if(to->string && deallocateFunc)
       {
         deallocateFunc(to->string);
         to->string = 0;
       }

       to->string = oyStringCopy_(from->string, allocateFunc);
       break;
  case oyVAL_STRING_LIST:
       if(to->string_list && deallocateFunc)
       {
         i = 0;
         while(to->string_list[i])
           deallocateFunc(to->string_list[i++]);
         deallocateFunc(to->string_list);
         to->string_list = 0;
       }

       if(!from->string_list)
         return;

       i = 0;
       n = 0;
       while((size_t)from->string_list[i])
         ++n;

       to->string_list = allocateFunc( n * sizeof(char*));
       memset( to->string_list, 0, n * sizeof(char*) );
       i = 0;
       while(from->string_list[i])
       {
         to->string_list[i] = oyStringCopy_(from->string_list[i], allocateFunc);
         ++i;
       }
       to->string_list[n] = 0;

       break;
  case oyVAL_STRUCT:
       if(to->oy_struct && deallocateFunc)
       {
         if(to->oy_struct->release)
           to->oy_struct->release( &to->oy_struct );
         to->oy_struct = 0;
       }

       if(!from->oy_struct)
         return;
       if(from->oy_struct->copy)
         to->oy_struct = from->oy_struct->copy( from->oy_struct,
                                                from->oy_struct->oy_ );
       break;
  case oyVAL_NONE:
  case oyVAL_MAX: break;
  }
}

/** Function  oyValueEqual
 *  @memberof oyValue_u
 *  @brief    compare a oyValue_u union
 *
 *  @param         a                   value a
 *  @param         b                   value b
 *  @param         type                the value type to comare in a and b
 *  @param         pos                 position in list; -1 compare all
 *  return                             0 - not equal; 1 - equal
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/12/07
 *  @since    2010/04/11 (Oyranos: 0.1.10)
 */
int            oyValueEqual          ( oyValue_u         * a,
                                       oyValue_u         * b,
                                       oyVALUETYPE_e       type,
                                       int                 pos )
{
  int n = 0, n2 = 0, i;
  int equal = 0;

  if(!a && !b)
    return 1;
  else if(!a || !b)
    return equal;
  else if(a == b)
    return 1;

  switch(type)
  {
  case oyVAL_INT:
      if(a->int32 == b->int32)
          return 1;
      break;
  case oyVAL_INT_LIST:

      {
        if(!a->int32_list && !b->int32_list)
          return 1;
        if(!a->int32_list || !b->int32_list)
          return 0;
        if(a->int32_list == b->int32_list)
          return 1;

        if(0 <= pos && pos < a->int32_list[0] &&
           pos < b->int32_list[0])
          if(a->int32_list[1 + pos] == b->int32_list[1 + pos])
            return 1;
        if(pos < 0)
        {
          if(a->int32_list[0] != b->int32_list[0])
            return 0;
          n = a->int32_list[0];
          for(i = 0; i < n; ++i)
            if(a->int32_list[1 + i] != b->int32_list[1 + i])
              return 0;
          return 1;
        }
      }
      break;
  case oyVAL_DOUBLE: if(a->dbl == b->dbl) return 1; break;
  case oyVAL_DOUBLE_LIST:
        if(!a->dbl_list && !b->dbl_list)
          return 1;
        if(!a->dbl_list || !b->dbl_list)
          return 0;
        if(a->dbl_list == b->dbl_list)
          return 1;

        if(0 <= pos && pos < a->dbl_list[0] &&
           pos < b->dbl_list[0])
          if(a->dbl_list[1 + pos] == b->dbl_list[1 + pos])
            return 1;
        if(pos < 0)
        {
          if(a->dbl_list[0] != b->dbl_list[0])
            return 0;
          n = a->dbl_list[0];
          for(i = 0; i < n; ++i)
            if(a->dbl_list[1 + i] != b->dbl_list[1 + i])
              return 0;
          return 1;
        }
      break;
  case oyVAL_STRING:
        if(!a->string && !b->string)
          return 1;
        if(a->string && b->string && oyStrcmp_(a->string,b->string) == 0)
          return 1;
      break;
  case oyVAL_STRING_LIST:
        if(!a->string_list && !b->string_list)
          return 1;
        if(!a->string_list || !b->string_list)
          return 0;
        if(a->string_list == b->string_list)
          return 1;

        i = 0;
        n = 0;
        while((size_t)a->string_list[i])
          ++n;
        i = 0;
        n2 = 0;
        while((size_t)b->string_list[i])
          ++n2;
        if(0 <= pos && pos < n &&
           pos < n2)
        {
          if(a->string_list[pos] && b->dbl_list[1 + pos] &&
             oyStrcmp_(a->string_list[pos], b->string_list[pos]) == 0)
            return 1;
          else
            return 0;
        }

        if(pos < 0)
        {
          if(n != n2)
            return 0;
          for(i = 0; i < n; ++i)
            if(a->string_list[i] && b->string_list[i] &&
               oyStrcmp_(a->string_list[i], b->string_list[i]) == 0)
              return 0;
          return 1;
        }

      break;
  case oyVAL_STRUCT:
        if(!a->oy_struct && !b->oy_struct)
          return 1;
        if(!a->oy_struct || !b->oy_struct)
          return 0;
        if(a->oy_struct == b->oy_struct)
          return 1;

        if(a->oy_struct->type_ == oyOBJECT_BLOB_S &&
           b->oy_struct->type_ == oyOBJECT_BLOB_S &&
           oyBlob_GetPointer((oyBlob_s*)(a->oy_struct)) == oyBlob_GetPointer((oyBlob_s*)(b->oy_struct)) )
          return 1;
        if(a->oy_struct->type_ == oyOBJECT_POINTER_S &&
           b->oy_struct->type_ == oyOBJECT_POINTER_S &&
           oyPointer_GetPointer((oyPointer_s*)(a->oy_struct)) == oyPointer_GetPointer((oyPointer_s*)(b->oy_struct)))
          return 1;
      break;
  case oyVAL_NONE:
  case oyVAL_MAX: break;
  }

  return equal;
}

/** Function oyValueClear
 *  @memberof oyValue_u
 *  @brief   clear a oyValue_u union
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
void           oyValueClear          ( oyValue_u         * v,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc )
{
  int i;

  if(!v)
    return;

  if(!deallocateFunc)
    return; 

  if(v->int32)
  switch(type)
  {
  case oyVAL_INT:
  case oyVAL_DOUBLE:
       break;
  case oyVAL_INT_LIST:
  case oyVAL_DOUBLE_LIST:
       if(!v->int32_list)
         break;

       if(v->int32_list)
         deallocateFunc(v->int32_list);

       break;
  case oyVAL_STRING:
       deallocateFunc( v->string );
       break;
  case oyVAL_STRING_LIST:
       if(!v->string_list)
         break;

       if(v->string_list)
       {
         i = 0;
         while(v->string_list[i])
           deallocateFunc(v->string_list[i++]);
         deallocateFunc(v->string_list);
       }

       break;
  case oyVAL_STRUCT:
       if(v->oy_struct->release)
         v->oy_struct->release( &v->oy_struct );
       break;
  case oyVAL_NONE:
  case oyVAL_MAX: break;
  }
  memset( v, 0, sizeof(oyValue_u) );
}
/** Function oyValueRelease
 *  @memberof oyValue_u
 *  @brief   release a oyValue_u union
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
void           oyValueRelease        ( oyValue_u        ** v,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc )
{
  if(!v || !*v)
    return;

  if(!deallocateFunc)
    return; 

  oyValueClear( *v, type, deallocateFunc );

  deallocateFunc(*v);
  *v = 0;
}

/** Function oyValueTypeText
 *  @memberof oyValue_u
 *  @brief   obtain a short string about union type
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
const char *   oyValueTypeText       ( oyVALUETYPE_e       type )
{
  switch(type)
  {
  case oyVAL_INT:         return "xs:integer";
  case oyVAL_DOUBLE:      return "xs:double";
  case oyVAL_INT_LIST:    return "xs:integer";
  case oyVAL_DOUBLE_LIST: return "xs:double";
  case oyVAL_STRING:      return "xs:string";
  case oyVAL_STRING_LIST: return "xs:string";
  case oyVAL_STRUCT:      return "struct";
  case oyVAL_NONE:        return "none";
  case oyVAL_MAX:         return "max";
  }
  return 0;
}







/* } Include "Option.public_methods_definitions.c" */

