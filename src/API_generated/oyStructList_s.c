/** @file oyStructList_s.c

   [Template file inheritance graph]
   +-> oyStructList_s.template.c
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




#include <stdio.h>
  
#include "oyStructList_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyStructList_s_.h"
  


/** Function oyStructList_New
 *  @memberof oyStructList_s
 *  @brief   allocate a new StructList object
 */
OYAPI oyStructList_s * OYEXPORT
  oyStructList_New( oyObject_s object )
{
  oyObject_s s = object;
  oyStructList_s_ * structlist = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  structlist = oyStructList_New_( s );

  return (oyStructList_s*) structlist;
}

/** Function  oyStructList_Copy
 *  @memberof oyStructList_s
 *  @brief    Copy or Reference a StructList object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     structlist                 StructList struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyStructList_s* OYEXPORT
  oyStructList_Copy( oyStructList_s *structlist, oyObject_s object )
{
  oyStructList_s_ * s = (oyStructList_s_*) structlist;

  if(s)
  {
    oyCheckType__m( oyOBJECT_STRUCT_LIST_S, return NULL )
  }
  else
    return NULL;

  s = oyStructList_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyStructList_s" );

  return (oyStructList_s*) s;
}
 
/** Function oyStructList_Release
 *  @memberof oyStructList_s
 *  @brief   release and possibly deallocate a oyStructList_s object
 *
 *  @param[in,out] structlist                 StructList struct object
 */
OYAPI int OYEXPORT
  oyStructList_Release( oyStructList_s **structlist )
{
  oyStructList_s_ * s = 0;

  if(!structlist || !*structlist)
    return 0;

  s = (oyStructList_s_*) *structlist;

  oyCheckType__m( oyOBJECT_STRUCT_LIST_S, return 1 )

  *structlist = 0;

  return oyStructList_Release_( &s );
}



/* Include "StructList.public_methods_definitions.c" { */
#include "oyOption_s.h"
#include "oyranos_generic_internal.h"
/** Function  oyStructList_MoveIn
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer add
 *
 *  If the list was observed, the new elements are not automatically observed.
 *  The caller can select OY_OBSERVE_AS_WELL in the flags argument if he
 *  wishes to observe a newly added element as well. The attached observation
 *  function will be oyStructSignalForward_.
 *
 *  @param[in]     list                the list
 *  @param[in]     ptr                 the handle this function takes ownership
 *                                     of
 *  @param[in]     pos                 the prefered position in the list. This
 *                                     option has affect of the order in list.
 *  @param[in]     flags               OY_OBSERVE_AS_WELL
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/30 (Oyranos: 0.1.8)
 *  @date    2009/10/28
 */
int              oyStructList_MoveIn ( oyStructList_s    * list,
                                       oyStruct_s       ** ptr,
                                       int                 pos,
                                       uint32_t            flags )
{
  oyStructList_s_ * s = (oyStructList_s_*)list;
  int error = 0;
  int i;
  int set = 0;

  error = !s;

  if(error <= 0)
    if(s->type_ != oyOBJECT_STRUCT_LIST_S)
      error = 1;

  if(error <= 0)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );
  else
    return error;

  if(error <= 0)
    error = !(ptr && *ptr && (*ptr)->type_);

  if(error <= 0)
    /* search for a empty pointer and set */
    if(0 > pos || pos >= s->n_)
    {
      for(i = 0; i < s->n_; ++i)
        if(s->ptr_[i] == 0)
        {
          s->ptr_[i] = *ptr;
          set = 1;
        }
    }

  if(error <= 0 && !set)
  {
    int mult = (s->n_<7) ? 10 : (int)(s->n_ * 1.5);
    size_t len = 0;
    oyStruct_s ** tmp = 0;
    int real_copy = 0;

    /* reserve new memory */
    if(s->n_ >= s->n_reserved_)
    {
      len =  sizeof(oyPointer) * mult;
      s->n_reserved_ = mult;
      tmp = oySTRUCT_ALLOC_m(s, len);

      error = !tmp;

      if(error <= 0)
        error = !memset( tmp, 0, len );
      real_copy = 1;

    } else {

      tmp = s->ptr_;

    }

    /* position the new */
    if(pos < 0 || pos >= s->n_)
      pos = s->n_;

    /* sort the old to the new */
    {
      int j; 

      if(error <= 0)
      if(pos < s->n_ || real_copy)
      for(i = s->n_; i >= 0; --i)
      {
        j = (i >= pos) ? i-1 : i;
        if(i != pos)
          /*if(real_copy)
            tmp[i] = oyHandle_copy_( s->ptr_[j], 0 );
          else*/
            tmp[i] = s->ptr_[j];
      }

      if(error <= 0)
      {
        tmp[pos] = /*oyHandle_copy_(*/ *ptr/*, 0 )*/;
      }

      /* release old data */
      if(error <= 0)
      {
        if(real_copy)
        {
          if(s->ptr_)
            oySTRUCT_FREE_m(s, s->ptr_);
          s->ptr_ = tmp;
        }
      }

      /* set the final count */
      if(error <= 0)
        ++s->n_;
    }

    if(flags & OY_OBSERVE_AS_WELL && oyStruct_IsObserved((oyStruct_s*)s, 0))
      oyStruct_ObserverAdd( (oyStruct_s*)*ptr, (oyStruct_s*)s, 0,0 );
    *ptr = 0;
  }

  oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function  oyStructList_GetRef
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s referenced pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetRef ( oyStructList_s    * list,
                                       int                 pos )
{
  oyStructList_s_ * s = (oyStructList_s_*)list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  obj = oyStructList_Get_(s, pos);
  error = !obj;

  if(error <= 0)
    error = oyStructList_ReferenceAt_(s, pos);

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return obj;
}

/** Function  oyStructList_GetRefType
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetRefType( oyStructList_s * list,
                                          int              pos,
                                          oyOBJECT_e       type )
{
  oyStruct_s * obj = oyStructList_GetRef( list, pos );

  if(obj && obj->type_ != type)
  {
    if(obj->oy_ && obj->release)
      obj->release( &obj );
    obj = 0;
  }

  return obj;
}

/** Function  oyStructList_ReleaseAt
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer release
 *
 *  release and shrink
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/21
 *  @since   2007/11/21 (Oyranos: 0.1.8)
 */
int            oyStructList_ReleaseAt( oyStructList_s    * list,
                                       int                 pos )
{
  oyStructList_s_ * s = (oyStructList_s_*)list;
  int error = 0;

  if(!s) return -1;

  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    return 1;

  oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(0 <= pos && pos < s->n_)
  {
    oyStruct_s * entry = s->ptr_[pos];

    --s->n_;

    if(pos < s->n_)
      error = !memmove( &s->ptr_[pos], &s->ptr_[pos+1],
                        sizeof(oyStruct_s*) * (s->n_ - pos));

    if(entry && entry->release)
      entry->release( &entry );
  }

  oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function  oyStructList_Count
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s count
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
int              oyStructList_Count ( oyStructList_s   * list )
{
  int n = 0;
  oyStructList_s_ * s = (oyStructList_s_*)list;
  int error = 0;

  if(!(s && s->type_ == oyOBJECT_STRUCT_LIST_S))
    error = 1;

  if(error <= 0)
    n = s->n_;

  return n;
}

/** Function  oyStructList_GetText
 *  @memberof oyStructList_s
 *  @brief    Build and obtain the lists member names
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/11/04
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 */
const char * oyStructList_GetText    ( oyStructList_s    * list,
                                       oyNAME_e            name_type,
                                       int                 intent_spaces,
                                       uint32_t            flags OY_UNUSED )
{
  oyStructList_s_ * s = (oyStructList_s_*)list;
  int error = !s, i, n;
  char * hash_text = 0;
  char * text = 0;
  oyStruct_s * oy_struct = 0;

  if(error <= 0)
  {
    oyAllocHelper_m_( text, char, intent_spaces + 1, 0, return 0 );
    for(i = 0; i < intent_spaces; ++i)
      text[i] = ' ';
    text[i] = 0;
    n = oyStructList_Count( list );
    for(i = 0; i < n; ++i)
    {
      oy_struct = oyStructList_Get_( s, i );
      hashTextAdd_m( text );
      /*hashTextAdd_m( text );*/
      hashTextAdd_m( oyObject_GetName( oy_struct->oy_, name_type ) );
      hashTextAdd_m( text );
    }

    oyObject_SetName( s->oy_, hash_text, name_type );

    if(hash_text && s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );
    hash_text = 0;
    oyFree_m_( text );

    hash_text = (oyChar*) oyObject_GetName( s->oy_, name_type );
  }

  return hash_text;
}

/**
 *  Function  oyStructList_GetID
 *  @memberof oyStructList_s
 *  @brief    Eventually build and obtain the lists member names
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/11/04
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 */
const char *     oyStructList_GetID  ( oyStructList_s    * list,
                                       int                 intent_spaces,
                                       uint32_t            flags )
{
  int error = !list;
  const char * text = 0;

  if(error <= 0)
  {
    text = oyObject_GetName( list->oy_, oyNAME_NICK );
    if(!text)
      text = oyStructList_GetText( list, oyNAME_NICK, intent_spaces, flags );
  }

  return text;
}

/**
 *  Function  oyStructList_Clear
 *  @memberof oyStructList_s
 *  @brief    Release all listed objects
 *
 *  @version Oyranos: 0.1.9
 *  @date    2008/11/27
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 */
int              oyStructList_Clear  ( oyStructList_s    * list )
{
  oyStructList_s_ * s = (oyStructList_s_*)list;
  int error = !(s && s->type_ == oyOBJECT_STRUCT_LIST_S), i;

  if(error <= 0)
    for(i = s->n_ - 1; i >= 0; --i)
      oyStructList_ReleaseAt( list, i );
  return error;
}

/**
 *  Function  oyStructList_CopyFrom
 *  @memberof oyStructList_s
 *  @brief    Clean "list" and copy all listed objects from "from" to "list".
 *
 *  If the list was observed, the new elements are observed by the list through
 *  the standard signal forwarding function (oyStructSignalForward_).
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2009/10/28
 */
int              oyStructList_CopyFrom(oyStructList_s    * list,
                                       oyStructList_s    * from,
                                       oyObject_s          object )
{
  oyStructList_s * s = list;
  int error = !(s && s->type_ == oyOBJECT_STRUCT_LIST_S && 
                from && from->type_ == oyOBJECT_STRUCT_LIST_S),
      i;
  int from_n;
  oyStruct_s * o;

  if(error <= 0)
  {
    error = oyStructList_Clear( s );

    from_n = ((oyStructList_s_*)from)->n_;
    for(i = 0; i < from_n && error <= 0; ++i)
    {
      o = oyStructList_Get_( (oyStructList_s_*)from, i );
      o = o->copy( o, object );
      if(oy_debug_objects >= 0 && o)
        oyObjectDebugMessage_( o->oy_, __func__,
                               oyStructTypeToText(o->type_) );
      error = !o;
      if(!error)
        error = oyStructList_MoveIn( s, &o, -1, 0 );
    }

    if(error <= 0 && oyStruct_IsObserved( (oyStruct_s*)s, 0) )
      error = oyStructList_ObserverAdd( s, 0, 0, 0 );
  }


  return error;
}

/** Function  oyStructList_MoveTo
 *  @memberof oyStructList_s
 *  @brief    Move a list element to a new position
 *
 *  real used?
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/05/22
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 */
int              oyStructList_MoveTo ( oyStructList_s    * s,
                                       int                 pos,
                                       int                 new_pos )
{
  int error = 0,
      n;
  oyStruct_s * e = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_STRUCT_LIST_S, return 0 )

  n = oyStructList_Count( s );

  if(pos >= n || new_pos >= n || pos < 0 || new_pos < 0)
    error = 1;

  if(!error && pos != new_pos)
  {
    e = oyStructList_GetRef( s, pos );

    oyStructList_ReleaseAt( s, pos );
    oyStructList_MoveIn( s, &e, new_pos, 0 );
  } 

  return error;
}

/**
 *  Function  oyStructList_Sort
 *  @memberof oyStructList_s
 *  @brief    Sort a list according to a rank_list
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/05/23
 *  @since   2009/05/23 (Oyranos: 0.1.10)
 */
int              oyStructList_Sort   ( oyStructList_s    * list,
                                       int32_t           * rank_list )
{
  int error = !rank_list,
      n, i, j = 0;
  int32_t * rank_copy = 0;
  int32_t max = INT32_MIN;
  oyPointer * ptr = 0;
  int last = 0, pos;
  oyStructList_s_ * s = (oyStructList_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_STRUCT_LIST_S, return 0 )

  n = oyStructList_Count( (oyStructList_s*)s );

  if(!error && n)
  {
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

    ptr = oyAllocateFunc_( sizeof(int*) * n );
    if(!ptr) return 0;
    memset( ptr, 0, sizeof(int*) * n );

    rank_copy = oyAllocateFunc_( n * sizeof(int32_t) );
    if(!rank_copy) { free(ptr); return 0; }
    memset( rank_copy, 0, n * sizeof(int32_t) );

    for( j = 0; j < n; ++j )
    {
      max = INT32_MIN;
      pos = -1;

      /* find maximum  */
      for( i = 0; i < n; ++i )
        if(s->ptr_[i] && rank_list[i] > max)
        {
          max = rank_list[i];
          pos = i;
        }

      /* nothing to do? -> skip */
      if(pos < 0)
        break;

      /* move maximum to list copy */
      for(i = 0; i < n; ++i)
        if(s->ptr_[i] && rank_list[i] == max)
        {
          rank_copy[last] = max;
          ptr[last++] = s->ptr_[i];
          s->ptr_[i] = 0;
        }
    }

    /* move back the sorted data */
    error = !memmove( s->ptr_, ptr, n * sizeof(oyPointer) );
    if(!error)
      error = !memmove( rank_list, rank_copy, n * sizeof(int32_t) );

    oyFree_m_( ptr );
    oyFree_m_( rank_copy );

    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
  }

  return error;
}

/**
 *  Function  oyStructList_ObserverAdd
 *  @memberof oyStructList_s
 *  @brief    Add a observer to the each list member
 *
 *  Members are further observed by the list object.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/10/28
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 */
int              oyStructList_ObserverAdd (
                                       oyStructList_s    * list,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc )
{
  int error = 0;
  oyStructList_s * s = list;
  int i,n;
  oyStruct_s * o = 0;

  if(!list)
    return 0;

  oyCheckType__m( oyOBJECT_STRUCT_LIST_S, return 1 )

  if(observer)
    error = oyStruct_ObserverAdd( (oyStruct_s*)s, observer,
                                  user_data, signalFunc );

  n = oyStructList_Count( list );
  for(i = 0; i < n; ++i)
  {
    o = oyStructList_Get_( (oyStructList_s_*)list, i );
    oyStruct_ObserverAdd( o, (oyStruct_s*)s, 0, 0 );
  }

  return error;
}

/** Function  oyStructList_GetType
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer access
 *
 *  non thread save; better dont use
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetType( oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_e          type )
{
  oyStruct_s * obj = oyStructList_Get_( (oyStructList_s_*)list, pos );

  if(obj && obj->type_ != type)
    obj = NULL;
  return obj;
}

/** Function  oyStructList_GetParentObjType
 *  @memberof oyStructList_s
 *
 */
oyOBJECT_e       oyStructList_GetParentObjType (
                                       oyStructList_s    * list )
{
  oyStructList_s_ * s = (oyStructList_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_STRUCT_LIST_S, return 0 )

  return s->parent_type_;
}

/** Function  oyStructList_Create
 *  @memberof oyStructList_s
 *  @brief    Create a new oyStruct_s list
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
  {
    s->list_name = oyStringAppend_(0, list_name, s->oy_->allocateFunc_);
    oyObject_SetNames( s->oy_, list_name,list_name,list_name );
  }

  return (oyStructList_s*)s;
}

/**
 *  Function  oyStructList_MoveInName
 *  @memberof oyStructList_s
 *  @brief    Move a name into a list
 *
 *  The text is released inside.
 *
 *  @see oyStructList_AddName()
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/24
 *  @since   2008/10/07 (Oyranos: 0.1.13)
 */
int oyStructList_MoveInName( oyStructList_s * texts, char ** text, int pos, oyNAME_e type )
{
  int error = !texts || !text;
  if(!error)
  {
     oyStructList_AddName( texts, *text, pos, type );
     oyFree_m_(*text);
     *text = NULL;
  }
  return error;
}
/**
 *  Function  oyStructList_AddName
 *  @memberof oyStructList_s
 *  @brief    Add a name to a list
 *
 *  The text can be get back with oyStructList_GetName().
 *
 *  @param         texts               the texts list
 *  @param         text                the string to add
 *  @param         pos                 the position of the string in list
 *                                     or -1 for the end
 *  @param         type                supported are:
 *                                     - ::oyNAME_NAME
 *                                     - ::oyNAME_NICK
 *                                     - ::oyNAME_DESCRIPTION
 *                                     - ::oyNAME_LC
 *                                     language_country code in text with up to 5 bytes
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/10
 *  @since   2008/10/07 (Oyranos: 0.1.13)
 */
OYAPI int OYEXPORT oyStructList_AddName (
                                       oyStructList_s    * texts,
                                       const char        * text,
                                       int                 pos,
                                       oyNAME_e            type )
{
  int error = !texts;
  if(!error)
  {
    oyOption_s * name = (oyOption_s*) oyStructList_GetRefType( texts, pos, oyOBJECT_OPTION_S),
	       * tname = name;
    if(!name) name = oyOption_FromRegistration( OY_INTERNAL, 0 );
    if(!name) return 1;

    if(text)
      error = oyObject_SetName( name->oy_, text, type );

    if(tname)
      oyOption_Release( &tname );
    else
    {
      oyStruct_s * oy_struct = (oyStruct_s*) name;
      oyStructList_MoveIn( texts, &oy_struct, pos, 0 );
    }
  }
  return error;
}
/**
 *  Function  oyStructList_GetName
 *  @memberof oyStructList_s
 *  @brief    Add a name to a list
 *
 *  The text must have added with oyStructList_AddName().
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/24
 *  @since   2011/05/18 (Oyranos: 0.3.1)
 */
OYAPI const char *  OYEXPORT oyStructList_GetName (
                                       oyStructList_s    * texts,
                                       int                 pos,
                                       oyNAME_e            type )
{
  int error = !texts;
  const char * text = NULL;
  if(!error)
  {
    oyOption_s * name = (oyOption_s*)oyStructList_GetRefType(texts, pos, oyOBJECT_OPTION_S);
    if(!name || !name->oy_) return text;
    text = oyObject_GetName( name->oy_, type );
    oyOption_Release( &name );
  }
  return text;
}

/**
 *  Function  oyStructList_GetHash
 *  @memberof oyStructList_s
 *  @brief    Get a hash from a cache
 *
 *  Get always a Oyranos cache entry from a cache list.
 *
 *  @param[in]     cache_list          the list to search in
 *  @param[in]     flags               - 0 - assume text, length is strlen()
 *                                     - 1 - assume 16 byte hash instead of text
 *  @param[in]     hash_text           the text to search for in the cache_list
 *  @return                            the cache entry may not have a entry
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/17
 *  @since   2012/10/17 (Oyranos: 0.9.0)
 */
oyHash_s *       oyStructList_GetHash( oyStructList_s    * cache_list,
                                       uint32_t            flags,
                                       const char        * hash_text )
{
  return oyCacheListGetEntry_( cache_list, flags, hash_text );
}

/**
 *  Function  oyStructList_GetHashStruct
 *  @memberof oyStructList_s
 *  @brief    Get a hash from a cache
 *
 *  Get always a Oyranos cache entry from a cache list.
 *
 *  @param[in]     cache_list          the list to search in
 *  @param[in]     flags               - 0 - assume text, length is strlen()
 *                                     - 1 - assume 16 byte hash instead of text
 *  @param[in]     hash_text           the text to search for in the cache_list
 *  @param[in]     type                thre requested object type
 *  @return                            the cache entry may not have a entry
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/17
 *  @since   2012/10/17 (Oyranos: 0.9.0)
 */
oyStruct_s *     oyStructList_GetHashStruct (
                                       oyStructList_s    * cache_list,
                                       uint32_t            flags,
                                       const char        * hash_text,
                                       oyOBJECT_e          type )
{
  oyHash_s * hash = oyCacheListGetEntry_( cache_list, flags, hash_text );
  oyStruct_s * object = oyHash_GetPointer( hash, type );
  oyHash_Release( &hash );
  return object;
}


/* } Include "StructList.public_methods_definitions.c" */

