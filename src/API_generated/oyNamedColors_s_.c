/** @file oyNamedColors_s_.c

   [Template file inheritance graph]
   +-> oyNamedColors_s_.template.c
   |
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



  
#include "oyNamedColors_s.h"
#include "oyNamedColors_s_.h"


#include "oyList_s_.h"




#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_namedcolors_init_ = 0;
static char * oy_namedcolors_msg_text_ = NULL;
static int oy_namedcolors_msg_text_n_ = 0;
static const char * oyNamedColors_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_namedcolors_msg_text_ == NULL || oy_namedcolors_msg_text_n_ == 0 )
  {
    oy_namedcolors_msg_text_n_ = 512;
    oy_namedcolors_msg_text_ = (char*) alloc( oy_namedcolors_msg_text_n_ );
    if(oy_namedcolors_msg_text_)
      memset( oy_namedcolors_msg_text_, 0, oy_namedcolors_msg_text_n_ );
  }

  if( oy_namedcolors_msg_text_ == NULL || oy_namedcolors_msg_text_n_ == 0 )
    return "Memory problem";

  oy_namedcolors_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_namedcolors_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_namedcolors_msg_text_;
}

static void oyNamedColors_StaticFree_           ( void )
{
  if(oy_namedcolors_init_)
  {
    oy_namedcolors_init_ = 0;
    if(oy_namedcolors_msg_text_)
      oyFree_m_(oy_namedcolors_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyNamedColors_StaticFree_", "oyNamedColors_s" );
  }
}


/* Include "NamedColors.private_custom_definitions.c" { */
/** Function    oyNamedColors_Release__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  namedcolors  the NamedColors object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyNamedColors_Release__Members( oyNamedColors_s_ * namedcolors )
{
  oyNamedColors_s_ * s = namedcolors;
  if(s->prefix)
    oyObject_GetDeAlloc( s->oy_ )( s->prefix );
  if(s->suffix)
    oyObject_GetDeAlloc( s->oy_ )( s->suffix );
}

/** Function    oyNamedColors_Init__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  namedcolors  the NamedColors object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColors_Init__Members( oyNamedColors_s_ * namedcolors OY_UNUSED )
{
  return 0;
}

/** Function    oyNamedColors_Copy__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyNamedColors_s_ input object
 *  @param[out]  dst  the output oyNamedColors_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColors_Copy__Members( oyNamedColors_s_ * dst, oyNamedColors_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = oyObject_GetAlloc( dst->oy_ );

  /* Copy each value of src to dst here */
  if(src->prefix)
    dst->prefix = oyStringCopy_(src->prefix, allocateFunc_);

  return error;
}

/* } Include "NamedColors.private_custom_definitions.c" */


/** @internal
 *  Function oyNamedColors_New_
 *  @memberof oyNamedColors_s_
 *  @brief   allocate a new oyNamedColors_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColors_s_ * oyNamedColors_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAMED_COLORS_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyNamedColors_s" );
  oyNamedColors_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 2: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 3: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 4: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 5: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 6: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 7: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 8: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 9: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 10: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 11: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 12: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 13: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 14: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 15: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 16: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 17: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 18: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 19: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 20: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 21: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 22: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 23: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 24: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 25: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 26: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 27: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 28: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 29: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 30: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 31: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 32: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 33: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 34: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 35: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 36: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 37: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 38: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      case 39: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_)); break;
      default: s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_));
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

  error = !memset( s, 0, sizeof(oyNamedColors_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyNamedColors_Copy;
  s->release = (oyStruct_Release_f) oyNamedColors_Release;

  s->oy_ = s_obj;

  
  
  /* ---- start of custom List constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_LIST_S, (oyPointer)s );
  /* ---- end of custom List constructor ------- */
  /* ---- start of custom NamedColors constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_NAMED_COLORS_S, (oyPointer)s );
  /* ---- end of custom NamedColors constructor ------- */
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  
  /* ---- start of custom List constructor ----- */
  error += oyList_Init__Members( (oyList_s_*)s );
  /* ---- end of custom List constructor ------- */
  /* ---- start of custom NamedColors constructor ----- */
  error += oyNamedColors_Init__Members( s );
  /* ---- end of custom NamedColors constructor ------- */
  
  
  

  if(!oy_namedcolors_init_)
  {
    oy_namedcolors_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyNamedColors_StaticMessageFunc_,
                                        oyNamedColors_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyNamedColors_Copy__
 *  @memberof oyNamedColors_s_
 *  @brief   real copy a NamedColors object
 *
 *  @param[in]     namedcolors                 NamedColors struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColors_s_ * oyNamedColors_Copy__ ( oyNamedColors_s_ *namedcolors, oyObject_s object )
{
  oyNamedColors_s_ *s = 0;
  int error = 0;

  if(!namedcolors || !object)
    return s;

  s = (oyNamedColors_s_*) oyNamedColors_New( object );
  error = !s;

  if(!error) {
    
    
    /* ---- start of custom List copy constructor ----- */
    error = oyList_Copy__Members( (oyList_s_*)s, (oyList_s_*)namedcolors );
    /* ---- end of custom List copy constructor ------- */
    /* ---- start of custom NamedColors copy constructor ----- */
    error = oyNamedColors_Copy__Members( s, namedcolors );
    /* ---- end of custom NamedColors copy constructor ------- */
    
    
    
    
    s->list_ = oyStructList_Copy( namedcolors->list_, s->oy_ );

  }

  if(error)
    oyNamedColors_Release_( &s );

  return s;
}

/** @internal
 *  Function oyNamedColors_Copy_
 *  @memberof oyNamedColors_s_
 *  @brief   copy or reference a NamedColors object
 *
 *  @param[in]     namedcolors                 NamedColors struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColors_s_ * oyNamedColors_Copy_ ( oyNamedColors_s_ *namedcolors, oyObject_s object )
{
  oyNamedColors_s_ * s = namedcolors;

  if(!namedcolors)
    return 0;

  if(namedcolors && !object)
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

  s = oyNamedColors_Copy__( namedcolors, object );

  return s;
}
 
/** @internal
 *  Function oyNamedColors_Release_
 *  @memberof oyNamedColors_s_
 *  @brief   release and possibly deallocate a NamedColors list
 *
 *  @param[in,out] namedcolors                 NamedColors struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyNamedColors_Release_( oyNamedColors_s_ **namedcolors )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyNamedColors_s_ *s = 0;

  if(!namedcolors || !*namedcolors)
    return 0;

  s = *namedcolors;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *namedcolors = 0;

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
    WARNc2_S( "oyNamedColors_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  
  /* ---- start of custom List destructor ----- */
  oyList_Release__Members( (oyList_s_*)s );
  /* ---- end of custom List destructor ------- */
  /* ---- start of custom NamedColors destructor ----- */
  oyNamedColors_Release__Members( s );
  /* ---- end of custom NamedColors destructor ------- */
  
  
  

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
      fprintf( stderr, "!!!ERROR:%d oyNamedColors_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "NamedColors.private_methods_definitions.c" { */

/* } Include "NamedColors.private_methods_definitions.c" */

