/** @file oyLis_s_.c

   [Template file inheritance graph]
   +-> oyLis_s_.template.c
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



  
#include "oyLis_s.h"
#include "oyLis_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_lis_init_ = 0;
static char * oy_lis_msg_text_ = NULL;
static int oy_lis_msg_text_n_ = 0;
static const char * oyLis_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyLis_s_ * s = (oyLis_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_lis_msg_text_ == NULL || oy_lis_msg_text_n_ == 0 )
  {
    oy_lis_msg_text_n_ = 512;
    oy_lis_msg_text_ = (char*) alloc( oy_lis_msg_text_n_ );
    if(oy_lis_msg_text_)
      memset( oy_lis_msg_text_, 0, oy_lis_msg_text_n_ );
  }

  if( oy_lis_msg_text_ == NULL || oy_lis_msg_text_n_ == 0 )
    return "Memory problem";

  oy_lis_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_lis_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_lis_msg_text_;
}

static void oyLis_StaticFree_           ( void )
{
  if(oy_lis_init_)
  {
    oy_lis_init_ = 0;
    if(oy_lis_msg_text_)
      oyFree_m_(oy_lis_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyLis_StaticFree_", "oyLis_s" );
  }
}


/* Include "Lis.private_custom_definitions.c" { */
/** Function    oyLis_Release__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  lis  the Lis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyLis_Release__Members( oyLis_s_ * lis )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &lis->member );
   */

  if(lis->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = lis->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( lis->member );
     */
  }
}

/** Function    oyLis_Init__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  lis  the Lis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyLis_Init__Members( oyLis_s_ * lis )
{
  return 0;
}

/** Function    oyLis_Copy__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyLis_s_ input object
 *  @param[out]  dst  the output oyLis_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyLis_Copy__Members( oyLis_s_ * dst, oyLis_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return error;
}

/* } Include "Lis.private_custom_definitions.c" */


/** @internal
 *  Function oyLis_New_
 *  @memberof oyLis_s_
 *  @brief   allocate a new oyLis_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyLis_s_ * oyLis_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_LIS_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyLis_s" );
  oyLis_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 2: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 3: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 4: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 5: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 6: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 7: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 8: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 9: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 10: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 11: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 12: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 13: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 14: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 15: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 16: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 17: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 18: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 19: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 20: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 21: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 22: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 23: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 24: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 25: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 26: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 27: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 28: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 29: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 30: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 31: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 32: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 33: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 34: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 35: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 36: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 37: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 38: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      case 39: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_)); break;
      default: s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_));
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

  error = !memset( s, 0, sizeof(oyLis_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyLis_Copy;
  s->release = (oyStruct_Release_f) oyLis_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Lis constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_LIS_S, (oyPointer)s );
  /* ---- end of custom Lis constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Lis constructor ----- */
  error += oyLis_Init__Members( s );
  /* ---- end of custom Lis constructor ------- */
  
  
  
  

  if(!oy_lis_init_)
  {
    oy_lis_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyLis_StaticMessageFunc_,
                                        oyLis_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyLis_Copy__
 *  @memberof oyLis_s_
 *  @brief   real copy a Lis object
 *
 *  @param[in]     lis                 Lis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyLis_s_ * oyLis_Copy__ ( oyLis_s_ *lis, oyObject_s object )
{
  oyLis_s_ *s = 0;
  int error = 0;

  if(!lis || !object)
    return s;

  s = (oyLis_s_*) oyLis_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Lis copy constructor ----- */
    error = oyLis_Copy__Members( s, lis );
    /* ---- end of custom Lis copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyLis_Release_( &s );

  return s;
}

/** @internal
 *  Function oyLis_Copy_
 *  @memberof oyLis_s_
 *  @brief   copy or reference a Lis object
 *
 *  @param[in]     lis                 Lis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyLis_s_ * oyLis_Copy_ ( oyLis_s_ *lis, oyObject_s object )
{
  oyLis_s_ * s = lis;

  if(!lis)
    return 0;

  if(lis && !object)
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

  s = oyLis_Copy__( lis, object );

  return s;
}
 
/** @internal
 *  Function oyLis_Release_
 *  @memberof oyLis_s_
 *  @brief   release and possibly deallocate a Lis object
 *
 *  @param[in,out] lis                 Lis struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyLis_Release_( oyLis_s_ **lis )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyLis_s_ *s = 0;

  if(!lis || !*lis)
    return 0;

  s = *lis;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *lis = 0;

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
    WARNc2_S( "oyLis_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Lis destructor ----- */
  oyLis_Release__Members( s );
  /* ---- end of custom Lis destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyLis_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Lis.private_methods_definitions.c" { */

/* } Include "Lis.private_methods_definitions.c" */

