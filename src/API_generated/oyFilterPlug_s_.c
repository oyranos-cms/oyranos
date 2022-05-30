/** @file oyFilterPlug_s_.c

   [Template file inheritance graph]
   +-> oyFilterPlug_s_.template.c
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



  
#include "oyFilterPlug_s.h"
#include "oyFilterPlug_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_filterplug_init_ = 0;
static char * oy_filterplug_msg_text_ = NULL;
static int oy_filterplug_msg_text_n_ = 0;
static const char * oyFilterPlug_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyFilterPlug_s_ * s = (oyFilterPlug_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_filterplug_msg_text_ == NULL || oy_filterplug_msg_text_n_ == 0 )
  {
    oy_filterplug_msg_text_n_ = 512;
    oy_filterplug_msg_text_ = (char*) alloc( oy_filterplug_msg_text_n_ );
    if(oy_filterplug_msg_text_)
      memset( oy_filterplug_msg_text_, 0, oy_filterplug_msg_text_n_ );
  }

  if( oy_filterplug_msg_text_ == NULL || oy_filterplug_msg_text_n_ == 0 )
    return "Memory problem";

  oy_filterplug_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_filterplug_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type == oyNAME_DESCRIPTION)
    sprintf( &oy_filterplug_msg_text_[strlen(oy_filterplug_msg_text_)], "%s", s->relatives_?s->relatives_:"" );


  return oy_filterplug_msg_text_;
}

static void oyFilterPlug_StaticFree_           ( void )
{
  if(oy_filterplug_init_)
  {
    oy_filterplug_init_ = 0;
    if(oy_filterplug_msg_text_)
      oyFree_m_(oy_filterplug_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyFilterPlug_StaticFree_", "oyFilterPlug_s" );
  }
}


/* Include "FilterPlug.private_custom_definitions.c" { */
/** Function    oyFilterPlug_Release__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filterplug  the FilterPlug object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterPlug_Release__Members( oyFilterPlug_s_ * filterplug )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filterplug->member );
   */
  oyFilterNode_Release( (oyFilterNode_s**)&filterplug->node );

  oyFilterSocket_Callback( (oyFilterPlug_s*)filterplug, oyCONNECTOR_EVENT_RELEASED );

  oyConnector_Release( &filterplug->pattern );

  if(filterplug->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filterplug->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filterplug->member );
     */
    if(filterplug->relatives_)
      deallocateFunc( filterplug->relatives_ );
    filterplug->relatives_ = 0;
  }
}

/** Function    oyFilterPlug_Init__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filterplug  the FilterPlug object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlug_Init__Members( oyFilterPlug_s_ * filterplug OY_UNUSED )
{
  return 0;
}

/** Function    oyFilterPlug_Copy__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterPlug_s_ input object
 *  @param[out]  dst  the output oyFilterPlug_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlug_Copy__Members( oyFilterPlug_s_ * dst, oyFilterPlug_s_ * src)
{
  int error = 0;
#if 0
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

#if 0
  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  dst->pattern = oyConnector_Copy( src->pattern, dst->oy_ );
  dst->node = (oyFilterNode_s_*)oyFilterNode_Copy( (oyFilterNode_s*)src->node, 0 );

  return error;
}

/* } Include "FilterPlug.private_custom_definitions.c" */


/** @internal
 *  Function oyFilterPlug_New_
 *  @memberof oyFilterPlug_s_
 *  @brief   allocate a new oyFilterPlug_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlug_s_ * oyFilterPlug_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_PLUG_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyFilterPlug_s" );
  oyFilterPlug_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 2: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 3: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 4: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 5: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 6: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 7: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 8: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 9: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 10: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 11: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 12: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 13: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 14: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 15: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 16: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 17: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 18: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 19: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 20: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 21: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 22: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 23: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 24: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 25: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 26: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 27: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 28: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 29: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 30: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 31: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 32: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 33: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 34: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 35: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 36: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 37: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 38: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      case 39: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_)); break;
      default: s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_));
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

  error = !memset( s, 0, sizeof(oyFilterPlug_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterPlug_Copy;
  s->release = (oyStruct_Release_f) oyFilterPlug_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterPlug constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_PLUG_S, (oyPointer)s );
  /* ---- end of custom FilterPlug constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom FilterPlug constructor ----- */
  error += oyFilterPlug_Init__Members( s );
  /* ---- end of custom FilterPlug constructor ------- */
  
  
  
  

  if(!oy_filterplug_init_)
  {
    oy_filterplug_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyFilterPlug_StaticMessageFunc_,
                                        oyFilterPlug_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyFilterPlug_Copy__
 *  @memberof oyFilterPlug_s_
 *  @brief   real copy a FilterPlug object
 *
 *  @param[in]     filterplug                 FilterPlug struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlug_s_ * oyFilterPlug_Copy__ ( oyFilterPlug_s_ *filterplug, oyObject_s object )
{
  oyFilterPlug_s_ *s = 0;
  int error = 0;

  if(!filterplug || !object)
    return s;

  s = (oyFilterPlug_s_*) oyFilterPlug_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterPlug copy constructor ----- */
    error = oyFilterPlug_Copy__Members( s, filterplug );
    /* ---- end of custom FilterPlug copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyFilterPlug_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterPlug_Copy_
 *  @memberof oyFilterPlug_s_
 *  @brief   copy or reference a FilterPlug object
 *
 *  @param[in]     filterplug                 FilterPlug struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlug_s_ * oyFilterPlug_Copy_ ( oyFilterPlug_s_ *filterplug, oyObject_s object )
{
  oyFilterPlug_s_ * s = filterplug;

  if(!filterplug)
    return 0;

  if(filterplug && !object)
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

  s = oyFilterPlug_Copy__( filterplug, object );

  return s;
}
 
/** @internal
 *  Function oyFilterPlug_Release_
 *  @memberof oyFilterPlug_s_
 *  @brief   release and possibly deallocate a FilterPlug object
 *
 *  @param[in,out] filterplug                 FilterPlug struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyFilterPlug_Release_( oyFilterPlug_s_ **filterplug )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyFilterPlug_s_ *s = 0;

  if(!filterplug || !*filterplug)
    return 0;

  s = *filterplug;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *filterplug = 0;

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

  
  {
  uint32_t n = 0;
  int r OY_UNUSED = oyObject_UnRef(s->oy_);

  if(s->node)
    ++n;

  if(s->remote_socket_)
    ++n;

  if( r+1 < (int)n )
    WARNcc2_S( s, "reference count below internal references to other object(s): %s %s",
               s->node?"node":"", s->remote_socket_?"remote_socket_":"" );

  /* referenences from members has to be substracted
   * from this objects ref count */
  if(oyObject_GetRefCount( s->oy_ ) >= (int)(parent_refs + n + observer_refs*2) && r > 0)
     return 0;

  /* ref before oyXXX_Release__Members(), so the
   * oyXXX_Release() is not called twice */
  oyObject_Ref(s->oy_);
  }

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
    WARNc2_S( "oyFilterPlug_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom FilterPlug destructor ----- */
  oyFilterPlug_Release__Members( s );
  /* ---- end of custom FilterPlug destructor ------- */
  
  
  
  

  /* unref after oyXXX_Release__Members() */
  oyObject_UnRef(s->oy_);


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
      fprintf( stderr, "!!!ERROR:%d oyFilterPlug_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "FilterPlug.private_methods_definitions.c" { */

/* } Include "FilterPlug.private_methods_definitions.c" */

