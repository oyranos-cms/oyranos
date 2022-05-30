/** @file oyObserver_s_.c

   [Template file inheritance graph]
   +-> oyObserver_s_.template.c
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



  
#include "oyObserver_s.h"
#include "oyObserver_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_observer_init_ = 0;
static char * oy_observer_msg_text_ = NULL;
static int oy_observer_msg_text_n_ = 0;
static const char * oyObserver_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyObserver_s_ * s = (oyObserver_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_observer_msg_text_ == NULL || oy_observer_msg_text_n_ == 0 )
  {
    oy_observer_msg_text_n_ = 512;
    oy_observer_msg_text_ = (char*) alloc( oy_observer_msg_text_n_ );
    if(oy_observer_msg_text_)
      memset( oy_observer_msg_text_, 0, oy_observer_msg_text_n_ );
  }

  if( oy_observer_msg_text_ == NULL || oy_observer_msg_text_n_ == 0 )
    return "Memory problem";

  oy_observer_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_observer_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_observer_msg_text_;
}

static void oyObserver_StaticFree_           ( void )
{
  if(oy_observer_init_)
  {
    oy_observer_init_ = 0;
    if(oy_observer_msg_text_)
      oyFree_m_(oy_observer_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyObserver_StaticFree_", "oyObserver_s" );
  }
}


/* Include "Observer.private_custom_definitions.c" { */
/** Function    oyObserver_Release__Members
 *  @memberof   oyObserver_s
 *  @brief      Custom Observer destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  observer  the Observer object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyObserver_Release__Members( oyObserver_s_ * observer )
{
  int error = 0;
  int id = 0;
  /* Deallocate members here
   * E.g: oyXXX_Release( &observer->member );
   */
  if((observer->model || observer->observer))
  {
    if(observer->model == NULL || observer->model->oy_ == NULL)
      WARNcc_S(observer->model, "incomplete")
    if(observer->observer == NULL || observer->observer->oy_ == NULL)
      WARNcc_S(observer->observer, "incomplete")
  }
  error = oyStruct_ObserverRemove( observer->model && observer->model->oy_ ? observer->model : NULL, observer->observer && observer->observer->oy_ ? observer->observer : NULL, NULL );
  if(error)
  {
    fprintf( stderr, OY_DBG_FORMAT_ "!!!ERROR: Observer[%d] gives error: %d  model: %s observer: %s\n", OY_DBG_ARGS_, observer->oy_->id_, error, observer->model && observer->model->oy_ ? "yes" : "----", observer->observer && observer->observer->oy_ ? "yes" : "----" );
    if(oy_debug_objects != -1 || oy_debug)
      OY_BACKTRACE_PRINT
  }

  if(observer->observer)
  {
    oyObject_UnRef( observer->observer->oy_ );
    /*observer->observer->release( &observer->observer );*/
    if(oy_debug_objects == observer->observer->oy_->id_)
      fprintf( stderr, OY_DBG_FORMAT_ "stop observation: %d <- %d\n", OY_DBG_ARGS_, observer->observer->oy_->id_, observer->oy_->id_ );
    observer->observer = NULL;
  }
  if(observer->model)
  {
    id = observer->model->oy_->id_;
    oyObject_UnRef( observer->model->oy_ );
    /*observer->model->release( &observer->model );*/
    if(oy_debug_objects == observer->model->oy_->id_)
      fprintf( stderr, OY_DBG_FORMAT_ "stop observing model: %d <- %d\n", OY_DBG_ARGS_, observer->model->oy_->id_, observer->oy_->id_ );
    observer->model = NULL;
  }
  if(observer->user_data)
  { observer->user_data->release( &observer->user_data ); observer->user_data = 0; }

  if(oy_debug_objects == -3 || (oy_debug_objects == id && getenv(OY_DEBUG_OBJECTS_PRINT_TREE))) /* animate the history of object releases */
  {
    char * text = NULL;
    OY_BACKTRACE_STRING(7)
    oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ );
    fprintf( stderr, "%s\n", text ? text : __func__ );
    oyFree_m_( text )
  }

  if(observer->oy_->deallocateFunc_)
  {
    //oyDeAlloc_f deallocateFunc = observer->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( observer->member );
     */
  }
}

/** Function    oyObserver_Init__Members
 *  @memberof   oyObserver_s
 *  @brief      Custom Observer constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  observer  the Observer object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyObserver_Init__Members( oyObserver_s_ * observer OY_UNUSED )
{
  return 0;
}

/** Function    oyObserver_Copy__Members
 *  @memberof   oyObserver_s
 *  @brief      Custom Observer copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyObserver_s_ input object
 *  @param[out]  dst  the output oyObserver_s_ object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
int oyObserver_Copy__Members( oyObserver_s_ * dst, oyObserver_s_ * src)
{
  int error = 0;
  //oyAlloc_f allocateFunc_ = 0;
  //oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  //allocateFunc_ = dst->oy_->allocateFunc_;
  //deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  {
    dst->observer = src->observer->copy( src->observer, dst->oy_ );
    dst->model = src->model->copy( src->model, dst->oy_ );
    dst->user_data = src->user_data->copy( src->user_data, dst->oy_ );
    if(oy_debug_objects >= 0)
    {
      if(dst->observer)
        oyObjectDebugMessage_( dst->observer->oy_, __func__,
                               oyStructTypeToText(dst->observer->type_) );
      if(dst->model)
        oyObjectDebugMessage_( dst->model->oy_, __func__,
                               oyStructTypeToText(dst->model->type_) );
      if(dst->user_data)
        oyObjectDebugMessage_( dst->user_data->oy_, __func__,
                               oyStructTypeToText(dst->user_data->type_) );
    }
    dst->disable_ref = src->disable_ref;
  }

  return error;
}

/* } Include "Observer.private_custom_definitions.c" */


/** @internal
 *  Function oyObserver_New_
 *  @memberof oyObserver_s_
 *  @brief   allocate a new oyObserver_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyObserver_s_ * oyObserver_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OBSERVER_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyObserver_s" );
  oyObserver_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 2: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 3: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 4: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 5: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 6: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 7: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 8: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 9: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 10: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 11: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 12: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 13: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 14: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 15: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 16: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 17: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 18: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 19: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 20: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 21: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 22: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 23: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 24: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 25: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 26: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 27: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 28: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 29: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 30: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 31: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 32: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 33: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 34: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 35: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 36: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 37: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 38: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      case 39: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_)); break;
      default: s = (oyObserver_s_*)s_obj->allocateFunc_(sizeof(oyObserver_s_));
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

  error = !memset( s, 0, sizeof(oyObserver_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyObserver_Copy;
  s->release = (oyStruct_Release_f) oyObserver_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Observer constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_OBSERVER_S, (oyPointer)s );
  /* ---- end of custom Observer constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Observer constructor ----- */
  error += oyObserver_Init__Members( s );
  /* ---- end of custom Observer constructor ------- */
  
  
  
  

  if(!oy_observer_init_)
  {
    oy_observer_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyObserver_StaticMessageFunc_,
                                        oyObserver_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyObserver_Copy__
 *  @memberof oyObserver_s_
 *  @brief   real copy a Observer object
 *
 *  @param[in]     observer                 Observer struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyObserver_s_ * oyObserver_Copy__ ( oyObserver_s_ *observer, oyObject_s object )
{
  oyObserver_s_ *s = 0;
  int error = 0;

  if(!observer || !object)
    return s;

  s = (oyObserver_s_*) oyObserver_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Observer copy constructor ----- */
    error = oyObserver_Copy__Members( s, observer );
    /* ---- end of custom Observer copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyObserver_Release_( &s );

  return s;
}

/** @internal
 *  Function oyObserver_Copy_
 *  @memberof oyObserver_s_
 *  @brief   copy or reference a Observer object
 *
 *  @param[in]     observer                 Observer struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyObserver_s_ * oyObserver_Copy_ ( oyObserver_s_ *observer, oyObject_s object )
{
  oyObserver_s_ * s = observer;

  if(!observer)
    return 0;

  if(observer && !object)
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

  s = oyObserver_Copy__( observer, object );

  return s;
}
 
/** @internal
 *  Function oyObserver_Release_
 *  @memberof oyObserver_s_
 *  @brief   release and possibly deallocate a Observer object
 *
 *  @param[in,out] observer                 Observer struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyObserver_Release_( oyObserver_s_ **observer )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyObserver_s_ *s = 0;

  if(!observer || !*observer)
    return 0;

  s = *observer;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *observer = 0;

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
    WARNc2_S( "oyObserver_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Observer destructor ----- */
  oyObserver_Release__Members( s );
  /* ---- end of custom Observer destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyObserver_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Observer.private_methods_definitions.c" { */
#include "oyOption_s_.h"
#include "oyStructList_s_.h"

/** Function  oyStructSignalForward_
 *  @memberof oyObserver_s
 *  @brief    Observe all list members
 *  @internal
 *
 *
 *  This function is useful to forward signals and fill holes in a chain.
 *  Implements oyObserver_Signal_f.
 *
 *  @code
    error = oyStruct_ObserverAdd( (oyStruct_s*)model, (oyStruct_s*)observer,
                                  0, oyStructSignalForward_ );
    @endcode
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int      oyStructSignalForward_      ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  oyObserver_s_ * obs = (oyObserver_s_ *)observer;

  if(oy_debug_signals)
    WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

  if(obs && obs->model &&
     obs->observer && obs->observer->type_ > oyOBJECT_NONE)
    handled = oyStruct_ObserverSignal( obs->observer,
                                       signal_type, signal_data );

  return handled;
}

/** Function  oyStruct_ObserverListGet_
 *  @memberof oyObserver_s
 *  @brief    Get the desired list of oyObserver_s'
 *  @internal
 *
 *  @param[in]     obj                 the object to look in for the list
 *  @param[in]     reg                 the selector for the list
 *  @return                            the observers
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
oyStructList_s * oyStruct_ObserverListGet_(
                                       oyStruct_s        * obj,
                                       const char        * reg )
{
  oyOption_s * o = 0;
  oyOption_s_ * o_ = 0;
  int error = 0;
  oyStructList_s * list = 0;

  o = oyOptions_Find( obj->oy_->handles_, reg, oyNAME_PATTERN );

  if(!o)
  {
    list = oyStructList_Create( 0, oyStruct_GetText(obj, oyNAME_NAME, 0), 0 );
    o = oyOption_FromRegistration( reg, obj->oy_ );
    error = oyOption_MoveInStruct( o, (oyStruct_s**)&list );
    if(!error)
    {
      if(!obj->oy_->handles_)
        obj->oy_->handles_ = oyOptions_New( 0 );
      error = oyOptions_MoveIn( obj->oy_->handles_, &o, -1);
      o = oyOptions_Find( obj->oy_->handles_, reg, oyNAME_PATTERN );
    }
  }

  o_ = oyOptionPriv_m(o);

  if(!error && o_ && o_->value_type == oyVAL_STRUCT && o_->value)
  {
    if(o_->value->oy_struct &&
       o_->value->oy_struct->type_ == oyOBJECT_STRUCT_LIST_S)
      list = (oyStructList_s*)o_->value->oy_struct;
    else
    {
      WARNcc3_S( obj, "%s: %s %s", _("found list of wrong type"),
                 reg,
                 oyStruct_TypeToText( o_->value->oy_struct ) );
    }

    oyOption_Release( &o );
  }
  return list;
}


/** Function  oyStruct_ObserverRemove_
 *  @memberof oyObserver_s
 *  @brief    Remove a observer from the observer or model internal list
 *  @internal
 *
 *  @param[in,out] list                the reference list
 *  @param[in]     obj                 comparision object
 *  @param[in]     observer            1 - remove observer; 0 - remove model
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
int        oyStruct_ObserverRemove_  ( oyStructList_s    * list,
                                       oyStruct_s        * obj,
                                       int                 observer,
                                       oyObserver_Signal_f signalFunc )
{
  int error = 0;
  oyObserver_s_ * obs = 0;
  int n,i;
  if(list)
  {
    n = oyStructList_Count( list );
    for(i = n-1; i >= 0; --i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( list,
                                                   i, oyOBJECT_OBSERVER_S );

      if(obs &&
         ((observer && obj == obs->observer) ||
          (!observer && obj == obs->model)) &&
          (!signalFunc || obs->signal == signalFunc))
        oyStructList_ReleaseAt( /*(oyStructList_s_*)*/list, i );//, OY_STRUCT_LIST_UNREF_AND_SHRINK );
    }
  }
  return error;
}

/* } Include "Observer.private_methods_definitions.c" */

