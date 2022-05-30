/** @file oyRectangle_s_.c

   [Template file inheritance graph]
   +-> oyRectangle_s_.template.c
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



  
#include "oyRectangle_s.h"
#include "oyRectangle_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_rectangle_init_ = 0;
static char * oy_rectangle_msg_text_ = NULL;
static int oy_rectangle_msg_text_n_ = 0;
static const char * oyRectangle_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyRectangle_s_ * s = (oyRectangle_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_rectangle_msg_text_ == NULL || oy_rectangle_msg_text_n_ == 0 )
  {
    oy_rectangle_msg_text_n_ = 512;
    oy_rectangle_msg_text_ = (char*) alloc( oy_rectangle_msg_text_n_ );
    if(oy_rectangle_msg_text_)
      memset( oy_rectangle_msg_text_, 0, oy_rectangle_msg_text_n_ );
  }

  if( oy_rectangle_msg_text_ == NULL || oy_rectangle_msg_text_n_ == 0 )
    return "Memory problem";

  oy_rectangle_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_rectangle_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type != oyNAME_NICK || (flags & 0x01))
    sprintf( &oy_rectangle_msg_text_[strlen(oy_rectangle_msg_text_)], "%gx%g+%g+%g", s->width, s->height, s->x, s->y);


  return oy_rectangle_msg_text_;
}

static void oyRectangle_StaticFree_           ( void )
{
  if(oy_rectangle_init_)
  {
    oy_rectangle_init_ = 0;
    if(oy_rectangle_msg_text_)
      oyFree_m_(oy_rectangle_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyRectangle_StaticFree_", "oyRectangle_s" );
  }
}


/* Include "Rectangle.private_custom_definitions.c" { */
/** @internal
 *  Function    oyRectangle_Release__Members
 *  @memberof   oyRectangle_s
 *  @brief      Custom Rectangle destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  rectangle  the Rectangle object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyRectangle_Release__Members( oyRectangle_s_ * rectangle )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &rectangle->member );
   */

  if(rectangle->oy_->deallocateFunc_)
  {
    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( rectangle->member );
     */
  }
}

/** @internal
 *  Function    oyRectangle_Init__Members
 *  @memberof   oyRectangle_s
 *  @brief      Custom Rectangle constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  rectangle  the Rectangle object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyRectangle_Init__Members( oyRectangle_s_ * rectangle OY_UNUSED )
{
  return 0;
}

/** @internal
 *  Function    oyRectangle_Copy__Members
 *  @memberof   oyRectangle_s
 *  @brief      Custom Rectangle copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyRectangle_s_ input object
 *  @param[out]  dst  the output oyRectangle_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyRectangle_Copy__Members( oyRectangle_s_ * dst, oyRectangle_s_ * src)
{
  int error = 0;

  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */
  dst->x = src->x;
  dst->y = src->y;
  dst->width = src->width;
  dst->height = src->height;

  return error;
}

/* } Include "Rectangle.private_custom_definitions.c" */


/** @internal
 *  Function oyRectangle_New_
 *  @memberof oyRectangle_s_
 *  @brief   allocate a new oyRectangle_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyRectangle_s_ * oyRectangle_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_RECTANGLE_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyRectangle_s" );
  oyRectangle_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 2: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 3: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 4: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 5: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 6: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 7: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 8: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 9: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 10: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 11: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 12: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 13: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 14: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 15: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 16: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 17: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 18: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 19: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 20: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 21: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 22: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 23: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 24: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 25: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 26: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 27: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 28: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 29: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 30: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 31: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 32: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 33: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 34: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 35: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 36: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 37: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 38: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      case 39: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_)); break;
      default: s = (oyRectangle_s_*)s_obj->allocateFunc_(sizeof(oyRectangle_s_));
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

  error = !memset( s, 0, sizeof(oyRectangle_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyRectangle_Copy;
  s->release = (oyStruct_Release_f) oyRectangle_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Rectangle constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_RECTANGLE_S, (oyPointer)s );
  /* ---- end of custom Rectangle constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Rectangle constructor ----- */
  error += oyRectangle_Init__Members( s );
  /* ---- end of custom Rectangle constructor ------- */
  
  
  
  

  if(!oy_rectangle_init_)
  {
    oy_rectangle_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyRectangle_StaticMessageFunc_,
                                        oyRectangle_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyRectangle_Copy__
 *  @memberof oyRectangle_s_
 *  @brief   real copy a Rectangle object
 *
 *  @param[in]     rectangle                 Rectangle struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyRectangle_s_ * oyRectangle_Copy__ ( oyRectangle_s_ *rectangle, oyObject_s object )
{
  oyRectangle_s_ *s = 0;
  int error = 0;

  if(!rectangle || !object)
    return s;

  s = (oyRectangle_s_*) oyRectangle_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Rectangle copy constructor ----- */
    error = oyRectangle_Copy__Members( s, rectangle );
    /* ---- end of custom Rectangle copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyRectangle_Release_( &s );

  return s;
}

/** @internal
 *  Function oyRectangle_Copy_
 *  @memberof oyRectangle_s_
 *  @brief   copy or reference a Rectangle object
 *
 *  @param[in]     rectangle                 Rectangle struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyRectangle_s_ * oyRectangle_Copy_ ( oyRectangle_s_ *rectangle, oyObject_s object )
{
  oyRectangle_s_ * s = rectangle;

  if(!rectangle)
    return 0;

  if(rectangle && !object)
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

  s = oyRectangle_Copy__( rectangle, object );

  return s;
}
 
/** @internal
 *  Function oyRectangle_Release_
 *  @memberof oyRectangle_s_
 *  @brief   release and possibly deallocate a Rectangle object
 *
 *  @param[in,out] rectangle                 Rectangle struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyRectangle_Release_( oyRectangle_s_ **rectangle )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyRectangle_s_ *s = 0;

  if(!rectangle || !*rectangle)
    return 0;

  s = *rectangle;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *rectangle = 0;

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
    WARNc2_S( "oyRectangle_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Rectangle destructor ----- */
  oyRectangle_Release__Members( s );
  /* ---- end of custom Rectangle destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyRectangle_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Rectangle.private_methods_definitions.c" { */

/* } Include "Rectangle.private_methods_definitions.c" */

