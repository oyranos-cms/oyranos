/** @file oyHash_s_.c

   [Template file inheritance graph]
   +-> oyHash_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyHash_s.h"
#include "oyHash_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_hash_init_ = 0;
static char * oy_hash_msg_text_ = NULL;
static int oy_hash_msg_text_n_ = 0;
static const char * oyHash_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyHash_s_ * s = (oyHash_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_hash_msg_text_ == NULL || oy_hash_msg_text_n_ == 0 )
  {
    oy_hash_msg_text_n_ = 512;
    oy_hash_msg_text_ = (char*) alloc( oy_hash_msg_text_n_ );
    if(oy_hash_msg_text_)
      memset( oy_hash_msg_text_, 0, oy_hash_msg_text_n_ );
  }

  if( oy_hash_msg_text_ == NULL || oy_hash_msg_text_n_ == 0 )
    return "Memory problem";

  oy_hash_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_hash_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  const char * hash_text = oyObject_GetName(s->oy_, oyNAME_NAME);
  int l = 0;
  if(hash_text)
    l = strlen(hash_text);

  /* allocate enough space */
  if(oy_hash_msg_text_n_ < l)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_hash_msg_text_ && oy_hash_msg_text_n_)
      dealloc( oy_hash_msg_text_ );
    oy_hash_msg_text_n_ = l;
    oy_hash_msg_text_ = alloc(oy_hash_msg_text_n_);
    if(oy_hash_msg_text_)
      oy_hash_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_hash_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_hash_msg_text_[strlen(oy_hash_msg_text_)], "%d",
             l
           );
  } else
  if(type == oyNAME_NAME ||
     (int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_hash_msg_text_[strlen(oy_hash_msg_text_)], "%s",
             hash_text?hash_text:"----"
           );


  return oy_hash_msg_text_;
}

static void oyHash_StaticFree_           ( void )
{
  if(oy_hash_init_)
  {
    oy_hash_init_ = 0;
    if(oy_hash_msg_text_)
      oyFree_m_(oy_hash_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyHash_StaticFree_", "oyHash_s" );
  }
}


/* Include "Hash.private_custom_definitions.c" { */
/** @internal
 *  Function    oyHash_Release__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  hash  the Hash object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyHash_Release__Members( oyHash_s_ * hash )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &hash->member );
   */

  /* should not happen */
  if(hash->oy_struct && hash->oy_struct->release)
    hash->oy_struct->release( &hash->oy_struct );

  if(hash->oy_->deallocateFunc_)
  {
    /*oyDeAlloc_f deallocateFunc = hash->oy_->deallocateFunc_;*/

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( hash->member );
     */
  }
}

/** @internal
 *  Function    oyHash_Init__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  hash  the Hash object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyHash_Init__Members( oyHash_s_ * hash )
{
  oyStruct_AllocHelper_m_( hash->oy_->hash_ptr_,
                    unsigned char,
                    OY_HASH_SIZE*2,
                    hash,
                    oyHash_Release_( &hash ));

  return 0;
}

/** @internal
 *  Function    oyHash_Copy__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyHash_s_ input object
 *  @param[out]  dst  the output oyHash_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyHash_Copy__Members( oyHash_s_ * dst, oyHash_s_ * src)
{
  int error = 0;

  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */

  return error;
}

/* } Include "Hash.private_custom_definitions.c" */


/** @internal
 *  Function oyHash_New_
 *  @memberof oyHash_s_
 *  @brief   allocate a new oyHash_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyHash_s_ * oyHash_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_HASH_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyHash_s" );
  oyHash_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 2: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 3: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 4: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 5: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 6: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 7: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 8: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 9: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 10: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 11: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 12: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 13: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 14: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 15: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 16: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 17: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 18: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 19: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 20: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 21: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 22: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 23: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 24: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 25: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 26: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 27: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 28: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 29: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 30: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 31: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 32: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 33: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 34: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 35: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 36: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 37: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 38: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      case 39: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_)); break;
      default: s = (oyHash_s_*)s_obj->allocateFunc_(sizeof(oyHash_s_));
    }
  }
  else
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  if(!s)
  {
    if(s_obj)
      oyObject_Release( &s_obj );
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyHash_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyHash_Copy;
  s->release = (oyStruct_Release_f) oyHash_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Hash constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_HASH_S, (oyPointer)s );
  /* ---- end of custom Hash constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Hash constructor ----- */
  error += oyHash_Init__Members( s );
  /* ---- end of custom Hash constructor ------- */
  
  
  
  

  if(!oy_hash_init_)
  {
    oy_hash_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyHash_StaticMessageFunc_,
                                        oyHash_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyHash_Copy__
 *  @memberof oyHash_s_
 *  @brief   real copy a Hash object
 *
 *  @param[in]     hash                 Hash struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyHash_s_ * oyHash_Copy__ ( oyHash_s_ *hash, oyObject_s object )
{
  oyHash_s_ *s = 0;
  int error = 0;

  if(!hash || !object)
    return s;

  s = (oyHash_s_*) oyHash_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Hash copy constructor ----- */
    error = oyHash_Copy__Members( s, hash );
    /* ---- end of custom Hash copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyHash_Release_( &s );

  return s;
}

/** @internal
 *  Function oyHash_Copy_
 *  @memberof oyHash_s_
 *  @brief   copy or reference a Hash object
 *
 *  @param[in]     hash                 Hash struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyHash_s_ * oyHash_Copy_ ( oyHash_s_ *hash, oyObject_s object )
{
  oyHash_s_ * s = hash;

  if(!hash)
    return 0;

  if(hash && !object)
  {
    if(oy_debug_objects >= 0 && s->oy_)
    {
      const char * t = getenv(OY_DEBUG_OBJECTS);
      int id_ = -1;

      if(t)
        id_ = atoi(t);
      else
        id_ = oy_debug_objects;

      if((id_ >= 0 && s->oy_->id_ == id_) ||
         (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
         id_ == 1)
      {
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents ),
            observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 ),
            i;
        const char * track_name = oyStructTypeToText(s->type_);
        if(!(track_name && track_name[0]))
          track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
        if(s->oy_->id_ == id_)
          for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
            fprintf( stderr, "  " );
        fprintf( stderr, "%s[%d] tracking refs: %d++ observers: %d parents: %d\n",
                 (s->oy_->id_ == id_)?oyjlTermColor(oyjlGREEN, track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          if(!(track_name && track_name[0]))
            track_name = oyStruct_GetInfo( parents[i], oyNAME_NICK, 0x01 );
          if(s->oy_->id_ == id_)
          {
            int i;
            for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
              fprintf( stderr, "  " );
          }
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyHash_Copy__( hash, object );

  return s;
}
 
/** @internal
 *  Function oyHash_Release_
 *  @memberof oyHash_s_
 *  @brief   release and possibly deallocate a Hash object
 *
 *  @param[in,out] hash                 Hash struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyHash_Release_( oyHash_s_ **hash )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyHash_s_ *s = 0;

  if(!hash || !*hash)
    return 0;

  s = *hash;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *hash = 0;

  observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 );

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      {
        int i;
        track_name = oyStructTypeToText(s->type_);
        if(!(track_name && track_name[0]))
          track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
        if(s->oy_->id_ == id_)
          for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
            fprintf( stderr, "  " );
        fprintf( stderr, "%s[%d] unref with refs: %d-- observers: %d parents: %d\n",
                 (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          if(!(track_name && track_name[0]))
            track_name = oyStruct_GetInfo( parents[i], oyNAME_NICK, 0x01 );
          if(s->oy_->id_ == id_)
          {
            int i;
            for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
              fprintf( stderr, "  " );
          }
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  
  if((oyObject_UnRef(s->oy_) - parent_refs - 2*observer_refs) > 0)
    return 0;
  /* ---- end of common object destructor ------- */

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      if(!(track_name && track_name[0]))
        track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
      fprintf( stderr, "%s[%d] destruct\n", (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_);
    }
  }

  refs = s->oy_->ref_;
  if(refs < 0)
  {
    WARNc2_S( "oyHash_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Hash destructor ----- */
  oyHash_Release__Members( s );
  /* ---- end of custom Hash destructor ------- */
  
  
  
  



  /* remove observer edges */
  oyOptions_Release( &s->oy_->handles_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyObject_s oy = s->oy_;

    refs = s->oy_->ref_;

    if(track_name)
      fprintf( stderr, "%s[%d] destructing\n", (s->oy_->id_ == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    if(refs > 1)
      fprintf( stderr, "!!!ERROR:%d oyHash_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Hash.private_methods_definitions.c" { */
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

/* } Include "Hash.private_methods_definitions.c" */

