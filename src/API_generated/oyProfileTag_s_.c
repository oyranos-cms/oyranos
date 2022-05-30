/** @file oyProfileTag_s_.c

   [Template file inheritance graph]
   +-> oyProfileTag_s_.template.c
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



  
#include "oyProfileTag_s.h"
#include "oyProfileTag_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_profiletag_init_ = 0;
static char * oy_profiletag_msg_text_ = NULL;
static int oy_profiletag_msg_text_n_ = 0;
static const char * oyProfileTag_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_profiletag_msg_text_ == NULL || oy_profiletag_msg_text_n_ == 0 )
  {
    oy_profiletag_msg_text_n_ = 512;
    oy_profiletag_msg_text_ = (char*) alloc( oy_profiletag_msg_text_n_ );
    if(oy_profiletag_msg_text_)
      memset( oy_profiletag_msg_text_, 0, oy_profiletag_msg_text_n_ );
  }

  if( oy_profiletag_msg_text_ == NULL || oy_profiletag_msg_text_n_ == 0 )
    return "Memory problem";

  oy_profiletag_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_profiletag_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  /* allocate enough space */
  if(oy_profiletag_msg_text_n_ < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_profiletag_msg_text_ && oy_profiletag_msg_text_n_)
      dealloc( oy_profiletag_msg_text_ );
    oy_profiletag_msg_text_n_ = 1000;
    oy_profiletag_msg_text_ = alloc(oy_profiletag_msg_text_n_);
    if(oy_profiletag_msg_text_)
      oy_profiletag_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_profiletag_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_profiletag_msg_text_[strlen(oy_profiletag_msg_text_)], "%s",
             oyICCTagName(s->use)
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_profiletag_msg_text_[strlen(oy_profiletag_msg_text_)], "%s %s",
             oyICCTagName(s->use), oyICCTagTypeName(s->tag_type_)
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_profiletag_msg_text_[strlen(oy_profiletag_msg_text_)], "%s %s\noffset: %lu size: %lu/%lu",
             oyICCTagDescription(s->use), oyICCTagTypeName(s->tag_type_),
             (long unsigned int)s->offset_orig, (long unsigned int)s->size_, (long unsigned int)s->size_check_
           );


  return oy_profiletag_msg_text_;
}

static void oyProfileTag_StaticFree_           ( void )
{
  if(oy_profiletag_init_)
  {
    oy_profiletag_init_ = 0;
    if(oy_profiletag_msg_text_)
      oyFree_m_(oy_profiletag_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyProfileTag_StaticFree_", "oyProfileTag_s" );
  }
}


/* Include "ProfileTag.private_custom_definitions.c" { */
/** @internal
 *  Function    oyProfileTag_Release__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  profiletag  the ProfileTag object
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/10/07 (Oyranos: 0.9.7)
 *  @date    2017/10/07
 */
void oyProfileTag_Release__Members( oyProfileTag_s_ * profiletag OY_UNUSED )
{
  if(profiletag->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = profiletag->oy_->deallocateFunc_;

    if(profiletag->block_)
    { deallocateFunc( profiletag->block_ ); profiletag->block_ = 0; profiletag->size_ = 0; }
  }
}

/** @internal
 *  Function    oyProfileTag_Init__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  profiletag  the ProfileTag object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfileTag_Init__Members( oyProfileTag_s_ * profiletag OY_UNUSED )
{
  return 0;
}

/** @internal
 *  Function    oyProfileTag_Copy__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyProfileTag_s_ input object
 *  @param[out]  dst  the output oyProfileTag_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfileTag_Copy__Members( oyProfileTag_s_ * dst, oyProfileTag_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */
  /* Copy the whole struct mem block as is - is this safe? */
  memcpy(dst, src, sizeof(oyProfileTag_s_));

  return 0;
}

/* } Include "ProfileTag.private_custom_definitions.c" */


/** @internal
 *  Function oyProfileTag_New_
 *  @memberof oyProfileTag_s_
 *  @brief   allocate a new oyProfileTag_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfileTag_s_ * oyProfileTag_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PROFILE_TAG_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyProfileTag_s" );
  oyProfileTag_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 2: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 3: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 4: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 5: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 6: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 7: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 8: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 9: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 10: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 11: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 12: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 13: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 14: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 15: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 16: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 17: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 18: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 19: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 20: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 21: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 22: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 23: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 24: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 25: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 26: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 27: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 28: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 29: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 30: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 31: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 32: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 33: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 34: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 35: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 36: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 37: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 38: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      case 39: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_)); break;
      default: s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_));
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

  error = !memset( s, 0, sizeof(oyProfileTag_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyProfileTag_Copy;
  s->release = (oyStruct_Release_f) oyProfileTag_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom ProfileTag constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_PROFILE_TAG_S, (oyPointer)s );
  /* ---- end of custom ProfileTag constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom ProfileTag constructor ----- */
  error += oyProfileTag_Init__Members( s );
  /* ---- end of custom ProfileTag constructor ------- */
  
  
  
  

  if(!oy_profiletag_init_)
  {
    oy_profiletag_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyProfileTag_StaticMessageFunc_,
                                        oyProfileTag_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyProfileTag_Copy__
 *  @memberof oyProfileTag_s_
 *  @brief   real copy a ProfileTag object
 *
 *  @param[in]     profiletag                 ProfileTag struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfileTag_s_ * oyProfileTag_Copy__ ( oyProfileTag_s_ *profiletag, oyObject_s object )
{
  oyProfileTag_s_ *s = 0;
  int error = 0;

  if(!profiletag || !object)
    return s;

  s = (oyProfileTag_s_*) oyProfileTag_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom ProfileTag copy constructor ----- */
    error = oyProfileTag_Copy__Members( s, profiletag );
    /* ---- end of custom ProfileTag copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyProfileTag_Release_( &s );

  return s;
}

/** @internal
 *  Function oyProfileTag_Copy_
 *  @memberof oyProfileTag_s_
 *  @brief   copy or reference a ProfileTag object
 *
 *  @param[in]     profiletag                 ProfileTag struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfileTag_s_ * oyProfileTag_Copy_ ( oyProfileTag_s_ *profiletag, oyObject_s object )
{
  oyProfileTag_s_ * s = profiletag;

  if(!profiletag)
    return 0;

  if(profiletag && !object)
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

  s = oyProfileTag_Copy__( profiletag, object );

  return s;
}
 
/** @internal
 *  Function oyProfileTag_Release_
 *  @memberof oyProfileTag_s_
 *  @brief   release and possibly deallocate a ProfileTag object
 *
 *  @param[in,out] profiletag                 ProfileTag struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyProfileTag_Release_( oyProfileTag_s_ **profiletag )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyProfileTag_s_ *s = 0;

  if(!profiletag || !*profiletag)
    return 0;

  s = *profiletag;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *profiletag = 0;

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
    WARNc2_S( "oyProfileTag_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom ProfileTag destructor ----- */
  oyProfileTag_Release__Members( s );
  /* ---- end of custom ProfileTag destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyProfileTag_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "ProfileTag.private_methods_definitions.c" { */

/* } Include "ProfileTag.private_methods_definitions.c" */

