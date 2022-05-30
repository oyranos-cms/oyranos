/** @file oyList_s_.c

   [Template file inheritance graph]
   +-> oyList_s_.template.c
   |
   +-> BaseList_s_.c
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



  
#include "oyList_s.h"
#include "oyList_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_list_init_ = 0;
static char * oy_list_msg_text_ = NULL;
static int oy_list_msg_text_n_ = 0;
static const char * oyList_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyList_s_ * s = (oyList_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_list_msg_text_ == NULL || oy_list_msg_text_n_ == 0 )
  {
    oy_list_msg_text_n_ = 512;
    oy_list_msg_text_ = (char*) alloc( oy_list_msg_text_n_ );
    if(oy_list_msg_text_)
      memset( oy_list_msg_text_, 0, oy_list_msg_text_n_ );
  }

  if( oy_list_msg_text_ == NULL || oy_list_msg_text_n_ == 0 )
    return "Memory problem";

  oy_list_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_list_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_list_msg_text_;
}

static void oyList_StaticFree_           ( void )
{
  if(oy_list_init_)
  {
    oy_list_init_ = 0;
    if(oy_list_msg_text_)
      oyFree_m_(oy_list_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyList_StaticFree_", "oyList_s" );
  }
}


/* Include "List.private_custom_definitions.c" { */
/** Function    oyList_Release__Members
 *  @memberof   oyList_s
 *  @brief      Custom List destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  list  the List object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyList_Release__Members( oyList_s_ * list OY_UNUSED )
{
}

/** Function    oyList_Init__Members
 *  @memberof   oyList_s
 *  @brief      Custom List constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  list  the List object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyList_Init__Members( oyList_s_ * list OY_UNUSED )
{
  return 0;
}

/** Function    oyList_Copy__Members
 *  @memberof   oyList_s
 *  @brief      Custom List copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyList_s_ input object
 *  @param[out]  dst  the output oyList_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyList_Copy__Members( oyList_s_ * dst OY_UNUSED, oyList_s_ * src OY_UNUSED)
{
  return 0;
}

/* } Include "List.private_custom_definitions.c" */


/** @internal
 *  Function oyList_New_
 *  @memberof oyList_s_
 *  @brief   allocate a new oyList_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyList_s_ * oyList_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_LIST_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyList_s" );
  oyList_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 2: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 3: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 4: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 5: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 6: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 7: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 8: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 9: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 10: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 11: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 12: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 13: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 14: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 15: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 16: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 17: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 18: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 19: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 20: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 21: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 22: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 23: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 24: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 25: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 26: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 27: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 28: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 29: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 30: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 31: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 32: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 33: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 34: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 35: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 36: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 37: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 38: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      case 39: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_)); break;
      default: s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_));
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

  error = !memset( s, 0, sizeof(oyList_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyList_Copy;
  s->release = (oyStruct_Release_f) oyList_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom List constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_LIST_S, (oyPointer)s );
  /* ---- end of custom List constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom List constructor ----- */
  error += oyList_Init__Members( s );
  /* ---- end of custom List constructor ------- */
  
  
  
  

  if(!oy_list_init_)
  {
    oy_list_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyList_StaticMessageFunc_,
                                        oyList_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyList_Copy__
 *  @memberof oyList_s_
 *  @brief   real copy a List object
 *
 *  @param[in]     list                 List struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyList_s_ * oyList_Copy__ ( oyList_s_ *list, oyObject_s object )
{
  oyList_s_ *s = 0;
  int error = 0;

  if(!list || !object)
    return s;

  s = (oyList_s_*) oyList_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom List copy constructor ----- */
    error = oyList_Copy__Members( s, list );
    /* ---- end of custom List copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( list->list_, s->oy_ );

  }

  if(error)
    oyList_Release_( &s );

  return s;
}

/** @internal
 *  Function oyList_Copy_
 *  @memberof oyList_s_
 *  @brief   copy or reference a List object
 *
 *  @param[in]     list                 List struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyList_s_ * oyList_Copy_ ( oyList_s_ *list, oyObject_s object )
{
  oyList_s_ * s = list;

  if(!list)
    return 0;

  if(list && !object)
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

  s = oyList_Copy__( list, object );

  return s;
}
 
/** @internal
 *  Function oyList_Release_
 *  @memberof oyList_s_
 *  @brief   release and possibly deallocate a List list
 *
 *  @param[in,out] list                 List struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyList_Release_( oyList_s_ **list )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyList_s_ *s = 0;

  if(!list || !*list)
    return 0;

  s = *list;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *list = 0;

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
    WARNc2_S( "oyList_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom List destructor ----- */
  oyList_Release__Members( s );
  /* ---- end of custom List destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


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
      fprintf( stderr, "!!!ERROR:%d oyList_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "List.private_methods_definitions.c" { */

/* } Include "List.private_methods_definitions.c" */

