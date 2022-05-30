/** @file oyPointer_s_.c

   [Template file inheritance graph]
   +-> oyPointer_s_.template.c
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



  
#include "oyPointer_s.h"
#include "oyPointer_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_pointer_init_ = 0;
static char * oy_pointer_msg_text_ = NULL;
static int oy_pointer_msg_text_n_ = 0;
static const char * oyPointer_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyPointer_s_ * s = (oyPointer_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_pointer_msg_text_ == NULL || oy_pointer_msg_text_n_ == 0 )
  {
    oy_pointer_msg_text_n_ = 512;
    oy_pointer_msg_text_ = (char*) alloc( oy_pointer_msg_text_n_ );
    if(oy_pointer_msg_text_)
      memset( oy_pointer_msg_text_, 0, oy_pointer_msg_text_n_ );
  }

  if( oy_pointer_msg_text_ == NULL || oy_pointer_msg_text_n_ == 0 )
    return "Memory problem";

  oy_pointer_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_pointer_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  /* allocate enough space */
  if(oy_pointer_msg_text_n_ < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_pointer_msg_text_ && oy_pointer_msg_text_n_)
      dealloc( oy_pointer_msg_text_ );
    oy_pointer_msg_text_n_ = 1024;
    oy_pointer_msg_text_ = alloc(oy_pointer_msg_text_n_);
    if(oy_pointer_msg_text_)
      oy_pointer_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_pointer_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    if(s->lib_name)
      sprintf( &oy_pointer_msg_text_[strlen(oy_pointer_msg_text_)], "%s",
               strchr(s->lib_name,'/') ? strchr(s->lib_name,'/') + 1: s->lib_name
         );
    else
    if(s->func_name)
      sprintf( &oy_pointer_msg_text_[strlen(oy_pointer_msg_text_)], "%s",
               s->func_name
         );
    else
    if(s->id)
      sprintf( &oy_pointer_msg_text_[strlen(oy_pointer_msg_text_)], "%s",
               s->id
         );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_pointer_msg_text_[strlen(oy_pointer_msg_text_)], "%s%s%s%s%s%s%s%d",
             s->lib_name?(strchr(s->lib_name,'/') ? strchr(s->lib_name,'/') + 1: s->lib_name):"", s->lib_name?"\n":"",
             s->func_name?s->func_name:"", s->func_name?"\n":"",
             s->id?s->id:"", s->id?" ":"",
             s->size?"size: ":"", s->size?s->size:-1
         );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_pointer_msg_text_[strlen(oy_pointer_msg_text_)], "%s%s%s%s%s%s%s%d",
             s->lib_name?s->lib_name:"", s->lib_name?"\n":"",
             s->func_name?s->func_name:"", s->func_name?"\n":"",
             s->id?s->id:"", s->id?" ":"",
             s->size?"size: ":"", s->size?s->size:-1
         );


  return oy_pointer_msg_text_;
}

static void oyPointer_StaticFree_           ( void )
{
  if(oy_pointer_init_)
  {
    oy_pointer_init_ = 0;
    if(oy_pointer_msg_text_)
      oyFree_m_(oy_pointer_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyPointer_StaticFree_", "oyPointer_s" );
  }
}


/* Include "Pointer.private_custom_definitions.c" { */
/** @internal
 *  Function    oyPointer_Release__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmptr  the Pointer object
 *
 *  @version Oyranos: 0.3.0
 *  @since   2010/08/00 (Oyranos: 0.3.0)
 *  @date    2011/02/13
 */
void oyPointer_Release__Members( oyPointer_s_ * cmmptr )
{
  oyPointer_s_ * s = cmmptr;
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmptr->member );
   */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmptr->member );
     */
    if(--s->ref)
      return;

    memset( s, 0, sizeof(oyOBJECT_e) );

    if(s->ptr)
    {
      if(s->ptrRelease)
        s->ptrRelease( &s->ptr );
      else
      {
        oyDeAllocateFunc_( s->ptr );
        s->ptr = 0;
      }
    }

    if(s->lib_name)
    { deallocateFunc( s->lib_name ); s->lib_name = 0; }
    if(s->func_name)
    { deallocateFunc( s->func_name ); s->func_name = 0; }
    if(s->resource)
    { deallocateFunc( s->resource ); s->resource = 0; }
    if(s->id)
    { deallocateFunc( s->id ); s->id = 0; }
  }
}

/** @internal
 *  Function    oyPointer_Init__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmptr  the Pointer object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPointer_Init__Members( oyPointer_s_ * cmmptr )
{
  ++cmmptr->ref;

  return 0;
}

/** @internal
 *  Function    oyPointer_Copy__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyPointer_s_ input object
 *  @param[out]  dst  the output oyPointer_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPointer_Copy__Members( oyPointer_s_ * dst, oyPointer_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

#define COPY_MEMBER_STRING(variable_name) { \
  if(allocateFunc_) \
    dst->variable_name = oyStringCopy_( src->variable_name, allocateFunc_ ); \
  else \
    dst->variable_name = src->variable_name; }

  /* Copy each value of src to dst here */
  dst->ref = src->ref;
  COPY_MEMBER_STRING( lib_name )
  COPY_MEMBER_STRING( func_name )
  COPY_MEMBER_STRING( resource )
  COPY_MEMBER_STRING( id )

  return 0;
}

/* } Include "Pointer.private_custom_definitions.c" */


/** @internal
 *  Function oyPointer_New_
 *  @memberof oyPointer_s_
 *  @brief   allocate a new oyPointer_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPointer_s_ * oyPointer_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_POINTER_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyPointer_s" );
  oyPointer_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 2: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 3: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 4: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 5: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 6: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 7: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 8: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 9: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 10: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 11: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 12: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 13: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 14: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 15: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 16: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 17: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 18: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 19: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 20: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 21: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 22: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 23: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 24: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 25: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 26: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 27: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 28: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 29: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 30: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 31: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 32: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 33: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 34: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 35: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 36: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 37: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 38: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      case 39: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_)); break;
      default: s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_));
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

  error = !memset( s, 0, sizeof(oyPointer_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyPointer_Copy;
  s->release = (oyStruct_Release_f) oyPointer_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Pointer constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_POINTER_S, (oyPointer)s );
  /* ---- end of custom Pointer constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Pointer constructor ----- */
  error += oyPointer_Init__Members( s );
  /* ---- end of custom Pointer constructor ------- */
  
  
  
  

  if(!oy_pointer_init_)
  {
    oy_pointer_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyPointer_StaticMessageFunc_,
                                        oyPointer_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyPointer_Copy__
 *  @memberof oyPointer_s_
 *  @brief   real copy a Pointer object
 *
 *  @param[in]     pointer                 Pointer struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPointer_s_ * oyPointer_Copy__ ( oyPointer_s_ *pointer, oyObject_s object )
{
  oyPointer_s_ *s = 0;
  int error = 0;

  if(!pointer || !object)
    return s;

  s = (oyPointer_s_*) oyPointer_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Pointer copy constructor ----- */
    error = oyPointer_Copy__Members( s, pointer );
    /* ---- end of custom Pointer copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyPointer_Release_( &s );

  return s;
}

/** @internal
 *  Function oyPointer_Copy_
 *  @memberof oyPointer_s_
 *  @brief   copy or reference a Pointer object
 *
 *  @param[in]     pointer                 Pointer struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPointer_s_ * oyPointer_Copy_ ( oyPointer_s_ *pointer, oyObject_s object )
{
  oyPointer_s_ * s = pointer;

  if(!pointer)
    return 0;

  if(pointer && !object)
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

  s = oyPointer_Copy__( pointer, object );

  return s;
}
 
/** @internal
 *  Function oyPointer_Release_
 *  @memberof oyPointer_s_
 *  @brief   release and possibly deallocate a Pointer object
 *
 *  @param[in,out] pointer                 Pointer struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyPointer_Release_( oyPointer_s_ **pointer )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyPointer_s_ *s = 0;

  if(!pointer || !*pointer)
    return 0;

  s = *pointer;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *pointer = 0;

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
    WARNc2_S( "oyPointer_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Pointer destructor ----- */
  oyPointer_Release__Members( s );
  /* ---- end of custom Pointer destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyPointer_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Pointer.private_methods_definitions.c" { */
/** @internal
 *  @brief set oyPointer_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2011/02/15
 */
int                oyPointer_Set_    ( oyPointer_s_      * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       const char        * id,
                                       oyPointer_release_f ptrRelease )
{
  oyPointer_s_ * s = cmm_ptr;
  int error = !s;
  oyAlloc_f alloc_func = oyStruct_GetAllocator( (oyStruct_s*) s );
  oyDeAlloc_f dealloc_func = oyStruct_GetDeAllocator( (oyStruct_s*) s );

    
  if(error <= 0 && lib_name)
  {
    if(s->lib_name)
      oyStringFree_( &s->lib_name, dealloc_func );
    s->lib_name = oyStringCopy_( lib_name, alloc_func );
  }

  if(error <= 0 && func_name)
  {
    if(s->func_name)
      oyStringFree_( &s->func_name, dealloc_func );
    s->func_name = oyStringCopy_( func_name, alloc_func );
  }

  if(error <= 0 && resource)
  {
    if(s->resource)
      oyStringFree_( &s->resource, dealloc_func );
    s->resource = oyStringCopy_( resource, alloc_func );
  }

  if(error <= 0 && id)
  {
    if(s->id)
      oyStringFree_( &s->id, dealloc_func );
    s->id = oyStringCopy_( id, alloc_func );
  }

  if(error <= 0 && ptr)
  {
    if(s->ptrRelease && s->ptr)
      s->ptrRelease( &s->ptr );
    s->ptr = ptr;
  }

  if(error <= 0 && ptrRelease)
    s->ptrRelease = ptrRelease;

  return error;
}

/* } Include "Pointer.private_methods_definitions.c" */

