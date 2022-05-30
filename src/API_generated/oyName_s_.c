/** @file oyName_s_.c

   [Template file inheritance graph]
   +-- oyName_s_.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#include "oyranos_object_internal.h"

#include "oyName_s.h"
#include "oyName_s_.h"

#include "oyObject_s.h"

/** @brief oyName_s new
 *  @internal
 *  @memberof oyName_s
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/22
 *  @since   2008/01/08 (Oyranos: 0.1.8)
 */
oyName_s *   oyName_newWith          ( oyAlloc_f           allocateFunc )
{
  /* ---- start of object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAME_S;
# define STRUCT_TYPE oyName_s
  int error = 0;
  STRUCT_TYPE * s = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  s = (STRUCT_TYPE*)allocateFunc(sizeof(STRUCT_TYPE));

  if(!s)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  if(error)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  s->type = type;

  s->copy = (oyStruct_Copy_f) oyName_copy;
  s->release = (oyStruct_Release_f) oyName_release;
# undef STRUCT_TYPE
  /* ---- end of object constructor ------- */

  return s;
}

/** @brief oyName_s new
 *  @internal
 *  @memberof oyName_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_new              ( oyObject_s          object )
{
  oyAlloc_f allocateFunc = oyAllocateFunc_;
  if(object)
    allocateFunc = object->allocateFunc_;

  return oyName_newWith( allocateFunc );
}

/** @brief oyName_s copy
 *  @internal
 *  @memberof oyName_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_copy               ( oyName_s        * obj,
                                         oyObject_s        object )
{
  int error = 0;
  oyName_s * s = 0;

  if(!obj)
    return s;

  s = oyName_new( object );

  error = oyName_copy_( s, obj, object );

  if(error)
    oyName_release( &s );

  return s;
}

/** @brief oyName_s deallocation
 *  @internal
 *  @memberof oyName_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
int          oyName_release          ( oyName_s         ** obj )
{
  int error = 0;

  if(!obj || !*obj)
    return 0;

  error = oyName_release_(obj, oyDeAllocateFunc_);

  *obj = 0;

  return error;
}
/** @brief oyName_s copy
 *  @internal
 *  @memberof oyName_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/22 (Oyranos: 0.1.10)
 *  @date    2008/12/22
 */
int          oyName_copy_            ( oyName_s          * dest,
                                       oyName_s          * src,
                                       oyObject_s          object )
{
  int error = 0;
  oyName_s * s = dest;
  oyAlloc_f   allocateFunc   = oyAllocateFunc_;
  oyDeAlloc_f deallocateFunc = oyDeAllocateFunc_;

  if(!src || !dest)
    return 0;

  if(object)
  {
    allocateFunc = object->allocateFunc_;
    deallocateFunc = object->deallocateFunc_;
  }

  if(src->name)
    s = oyName_set_ ( s, src->name, oyNAME_NAME, allocateFunc, deallocateFunc );
  if(src->nick)
    s = oyName_set_ ( s, src->nick, oyNAME_NICK, allocateFunc, deallocateFunc );
  if(src->description)
    s = oyName_set_ ( s, src->description, oyNAME_DESCRIPTION, allocateFunc, deallocateFunc );

  if(!s)
    s = oyName_new( object );

  if(error <= 0)
    error = !memcpy( s->lang, src->lang, 8 );

  return error;
}

/**
 *  @internal
 *  @memberof oyName_s
 *  @brief oyName_s deallocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int          oyName_release_         ( oyName_s         ** obj,
                                       oyDeAlloc_f         deallocateFunc )
{
  /* ---- start of common object destructor ----- */
  oyName_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( s->type != oyOBJECT_NAME_S)
  {
    WARNc_S(("Attempt to release a non oyName_s object."))
    return 1;
  }
  /* ---- end of common object destructor ------- */

  *obj = 0;

  if(!deallocateFunc)
    return 0;

  oyName_releaseMembers( s, deallocateFunc );

  deallocateFunc( s );

  return 0;
}

/**
 *  @internal
 *  @memberof oyName_s
 *  @brief naming plus automatic allocation
 *
 *  @param[in]    obj            the oyName_s struct
 *  @param[in]    text           the name should fit into usual labels
 *  @param[in]    type           the kind of name
 *  @param[in]    allocateFunc   memory management (defaults to oyAllocateFunc_)
 *  @param[in]    deallocateFunc optional memory management 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyName_s *   oyName_set_             ( oyName_s          * obj,
                                       const char        * text,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int error = 0;
  oyName_s * s = obj;

  if(obj && obj->type != oyOBJECT_NAME_S)
  {
    WARNc_S(("Attempt to edit a non oyName_s object."))
    return 0;
  }

  if(!s)
    s = oyName_newWith( allocateFunc );

  if(!s) return s;

  s->type = oyOBJECT_NAME_S;

  {
#define oySetString_m(n_type)\
    if(error <= 0) { \
      if(s->n_type && deallocateFunc) \
        deallocateFunc( s->n_type ); \
      s->n_type = oyStringCopy_( text, allocateFunc ); \
      if( !s->n_type ) error = 1; \
    } 
    switch (type) {
    case oyNAME_NICK:
         oySetString_m(nick) break;
    case oyNAME_NAME:
         oySetString_m(name) break;
    case oyNAME_DESCRIPTION:
         oySetString_m(description) break;
    default: break;
    }
#undef oySetString_
  }

  return s;
}

/**
 *  Function oyName_get_
 *  @internal
 *  @memberof oyName_s
 *  @brief   get name
 *
 *  @param[in,out] obj                 name object
 *  @param         type                type of name
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
const char * oyName_get_             ( const oyName_s    * obj,
                                       oyNAME_e            type )
{
  const char * text = 0;
  const oyName_s * name = obj;
  if(!obj)
    return 0;

  switch(type)
  {
    case oyNAME_NICK:
         text = name->nick; break;
    case oyNAME_DESCRIPTION:
         text = name->description; break;
    case oyNAME_NAME:
    default:
         text = name->name; break;
  }

  return text;
}

/** Function oyName_releaseMembers
 *  @internal
 *  @memberof oyName_s
 *  @brief   release only members
 *
 *  Useful to release the member strings but not the struct itself, which can
 *  in this case be static. Deallocation uses oyDeAllocateFunc_().
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
int          oyName_releaseMembers   ( oyName_s          * obj,
                                       oyDeAlloc_f         deallocateFunc )
{
  int error = 0;
  oyName_s * s = 0;

  if(!obj)
    return 0;

  if(!deallocateFunc)
    deallocateFunc = oyDeAllocateFunc_;

  s = obj;

  if(s->nick)
  { deallocateFunc(s->nick); s->nick = 0; }

  if(s->name)
  { deallocateFunc(s->name); s->name = 0; }

  if(s->description)
  { deallocateFunc(s->description); s->description = 0; }

  return error;
}
