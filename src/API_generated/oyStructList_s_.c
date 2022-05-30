/** @file oyStructList_s_.c

   [Template file inheritance graph]
   +-> oyStructList_s_.template.c
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



  
#include "oyStructList_s.h"
#include "oyStructList_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_structlist_init_ = 0;
static char * oy_structlist_msg_text_ = NULL;
static int oy_structlist_msg_text_n_ = 0;
static const char * oyStructList_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyStructList_s_ * s = (oyStructList_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_structlist_msg_text_ == NULL || oy_structlist_msg_text_n_ == 0 )
  {
    oy_structlist_msg_text_n_ = 512;
    oy_structlist_msg_text_ = (char*) alloc( oy_structlist_msg_text_n_ );
    if(oy_structlist_msg_text_)
      memset( oy_structlist_msg_text_, 0, oy_structlist_msg_text_n_ );
  }

  if( oy_structlist_msg_text_ == NULL || oy_structlist_msg_text_n_ == 0 )
    return "Memory problem";

  oy_structlist_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_structlist_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  /* allocate enough space */
  if(oy_structlist_msg_text_n_ < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_structlist_msg_text_ && oy_structlist_msg_text_n_)
      dealloc( oy_structlist_msg_text_ );
    oy_structlist_msg_text_n_ = 1024;
    oy_structlist_msg_text_ = alloc(oy_structlist_msg_text_n_);
    if(oy_structlist_msg_text_)
      oy_structlist_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_structlist_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
    sprintf( &oy_structlist_msg_text_[strlen(oy_structlist_msg_text_)], "%s%d",
             s->n_?"n: ":"", s->n_
           );
  else
  if(type == oyNAME_NAME)
    sprintf( &oy_structlist_msg_text_[strlen(oy_structlist_msg_text_)], "%s%s%s%d",
             s->list_name?s->list_name:"", s->list_name?"\n":"",
             s->n_?" n: ":"", s->n_
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_structlist_msg_text_[strlen(oy_structlist_msg_text_)], "%s%s%s%s%s%d",
             s->list_name?s->list_name:"", s->list_name?"\n":"",
             s->parent_type_?"parent: ":"", s->parent_type_?oyStructTypeToText(s->parent_type_):"",
             " n: ", s->n_
           );


  return oy_structlist_msg_text_;
}

static void oyStructList_StaticFree_           ( void )
{
  if(oy_structlist_init_)
  {
    oy_structlist_init_ = 0;
    if(oy_structlist_msg_text_)
      oyFree_m_(oy_structlist_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyStructList_StaticFree_", "oyStructList_s" );
  }
}


/* Include "StructList.private_custom_definitions.c" { */
/** @internal
 *  Function    oyStructList_Release__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  structlist  the StructList object
 */
void oyStructList_Release__Members( oyStructList_s_ * structlist )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &structlist->member );
   */

  oyStructList_Clear((oyStructList_s*)structlist);

  if(structlist->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = structlist->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( structlist->member );
     */
    if(structlist->ptr_)
      deallocateFunc(structlist->ptr_);

    if(structlist->list_name)
      deallocateFunc(structlist->list_name);

    memset( structlist, 0, sizeof(oyOBJECT_e) );
  }
}

/** @internal
 *  Function    oyStructList_Init__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  structlist  the StructList object
 */
int oyStructList_Init__Members( oyStructList_s_ * structlist OY_UNUSED )
{
  return 0;
}

/** @internal
 *  Function    oyStructList_Copy__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyStructList_s_ input object
 *  @param[out]  dst  the output oyStructList_s_ object
 */
int oyStructList_Copy__Members( oyStructList_s_ * dst, oyStructList_s_ * src)
{
  int error = 0, i;
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

  /* Copy each value of src to dst here */
  dst->parent_type_ = src->parent_type_;
  if(src->list_name)
    dst->list_name = oyStringAppend_(0, src->list_name, allocateFunc_);

  oyObject_Lock( dst->oy_, __FILE__, __LINE__ );

  dst->n_reserved_ = (src->n_ > 10) ? (int)(src->n_ * 1.5) : 10;
  dst->n_ = src->n_;
  dst->ptr_ = oyStruct_Allocate( (oyStruct_s*) dst, sizeof(int*) * dst->n_reserved_ );
  memset( dst->ptr_, 0, sizeof(int*) * dst->n_reserved_ );

  for(i = 0; i < src->n_; ++i)
    if(src->ptr_[i]->copy)
      dst->ptr_[i] = src->ptr_[i]->copy( src->ptr_[i], 0 );

  if(oyStruct_IsObserved( (oyStruct_s*)dst, 0) )
    error = oyStructList_ObserverAdd( (oyStructList_s*)src, 0, 0, 0 );

  oyObject_UnLock( dst->oy_, __FILE__, __LINE__ );

  return error;
}

/* } Include "StructList.private_custom_definitions.c" */


/** @internal
 *  Function oyStructList_New_
 *  @memberof oyStructList_s_
 *  @brief   allocate a new oyStructList_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_STRUCT_LIST_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyStructList_s" );
  oyStructList_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 2: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 3: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 4: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 5: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 6: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 7: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 8: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 9: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 10: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 11: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 12: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 13: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 14: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 15: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 16: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 17: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 18: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 19: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 20: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 21: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 22: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 23: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 24: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 25: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 26: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 27: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 28: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 29: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 30: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 31: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 32: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 33: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 34: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 35: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 36: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 37: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 38: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      case 39: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_)); break;
      default: s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_));
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

  error = !memset( s, 0, sizeof(oyStructList_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyStructList_Copy;
  s->release = (oyStruct_Release_f) oyStructList_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom StructList constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_STRUCT_LIST_S, (oyPointer)s );
  /* ---- end of custom StructList constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom StructList constructor ----- */
  error += oyStructList_Init__Members( s );
  /* ---- end of custom StructList constructor ------- */
  
  
  
  

  if(!oy_structlist_init_)
  {
    oy_structlist_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyStructList_StaticMessageFunc_,
                                        oyStructList_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyStructList_Copy__
 *  @memberof oyStructList_s_
 *  @brief   real copy a StructList object
 *
 *  @param[in]     structlist                 StructList struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_Copy__ ( oyStructList_s_ *structlist, oyObject_s object )
{
  oyStructList_s_ *s = 0;
  int error = 0;

  if(!structlist || !object)
    return s;

  s = (oyStructList_s_*) oyStructList_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom StructList copy constructor ----- */
    error = oyStructList_Copy__Members( s, structlist );
    /* ---- end of custom StructList copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyStructList_Release_( &s );

  return s;
}

/** @internal
 *  Function oyStructList_Copy_
 *  @memberof oyStructList_s_
 *  @brief   copy or reference a StructList object
 *
 *  @param[in]     structlist                 StructList struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_Copy_ ( oyStructList_s_ *structlist, oyObject_s object )
{
  oyStructList_s_ * s = structlist;

  if(!structlist)
    return 0;

  if(structlist && !object)
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

  s = oyStructList_Copy__( structlist, object );

  return s;
}
 
/** @internal
 *  Function oyStructList_Release_
 *  @memberof oyStructList_s_
 *  @brief   release and possibly deallocate a StructList object
 *
 *  @param[in,out] structlist                 StructList struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyStructList_Release_( oyStructList_s_ **structlist )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyStructList_s_ *s = 0;

  if(!structlist || !*structlist)
    return 0;

  s = *structlist;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *structlist = 0;

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
    WARNc2_S( "oyStructList_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom StructList destructor ----- */
  oyStructList_Release__Members( s );
  /* ---- end of custom StructList destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyStructList_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "StructList.private_methods_definitions.c" { */
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
      if(oy_debug_objects >= 0)
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

/* } Include "StructList.private_methods_definitions.c" */

