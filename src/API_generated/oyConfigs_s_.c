/** @file oyConfigs_s_.c

   [Template file inheritance graph]
   +-> oyConfigs_s_.template.c
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



  
#include "oyConfigs_s.h"
#include "oyConfigs_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_configs_init_ = 0;
static char * oy_configs_msg_text_ = NULL;
static int oy_configs_msg_text_n_ = 0;
static const char * oyConfigs_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyConfigs_s_ * s = (oyConfigs_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_configs_msg_text_ == NULL || oy_configs_msg_text_n_ == 0 )
  {
    oy_configs_msg_text_n_ = 512;
    oy_configs_msg_text_ = (char*) alloc( oy_configs_msg_text_n_ );
    if(oy_configs_msg_text_)
      memset( oy_configs_msg_text_, 0, oy_configs_msg_text_n_ );
  }

  if( oy_configs_msg_text_ == NULL || oy_configs_msg_text_n_ == 0 )
    return "Memory problem";

  oy_configs_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_configs_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_configs_msg_text_;
}

static void oyConfigs_StaticFree_           ( void )
{
  if(oy_configs_init_)
  {
    oy_configs_init_ = 0;
    if(oy_configs_msg_text_)
      oyFree_m_(oy_configs_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyConfigs_StaticFree_", "oyConfigs_s" );
  }
}


/* Include "Configs.private_custom_definitions.c" { */
/** Function    oyConfigs_Release__Members
 *  @memberof   oyConfigs_s
 *  @brief      Custom Configs destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  configs  the Configs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConfigs_Release__Members( oyConfigs_s_ * configs )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &configs->member );
   */

  if(configs->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = configs->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( configs->member );
     */
  }
}

/** Function    oyConfigs_Init__Members
 *  @memberof   oyConfigs_s
 *  @brief      Custom Configs constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  configs  the Configs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfigs_Init__Members( oyConfigs_s_ * configs OY_UNUSED )
{
  return 0;
}

/** Function    oyConfigs_Copy__Members
 *  @memberof   oyConfigs_s
 *  @brief      Custom Configs copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConfigs_s_ input object
 *  @param[out]  dst  the output oyConfigs_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfigs_Copy__Members( oyConfigs_s_ * dst, oyConfigs_s_ * src)
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

  return error;
}

/* } Include "Configs.private_custom_definitions.c" */


/** @internal
 *  Function oyConfigs_New_
 *  @memberof oyConfigs_s_
 *  @brief   allocate a new oyConfigs_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfigs_s_ * oyConfigs_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIGS_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyConfigs_s" );
  oyConfigs_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 2: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 3: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 4: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 5: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 6: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 7: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 8: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 9: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 10: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 11: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 12: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 13: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 14: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 15: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 16: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 17: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 18: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 19: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 20: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 21: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 22: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 23: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 24: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 25: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 26: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 27: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 28: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 29: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 30: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 31: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 32: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 33: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 34: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 35: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 36: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 37: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 38: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      case 39: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_)); break;
      default: s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_));
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

  error = !memset( s, 0, sizeof(oyConfigs_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConfigs_Copy;
  s->release = (oyStruct_Release_f) oyConfigs_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Configs constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONFIGS_S, (oyPointer)s );
  /* ---- end of custom Configs constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom Configs constructor ----- */
  error += oyConfigs_Init__Members( s );
  /* ---- end of custom Configs constructor ------- */
  
  
  
  

  if(!oy_configs_init_)
  {
    oy_configs_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyConfigs_StaticMessageFunc_,
                                        oyConfigs_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyConfigs_Copy__
 *  @memberof oyConfigs_s_
 *  @brief   real copy a Configs object
 *
 *  @param[in]     configs                 Configs struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfigs_s_ * oyConfigs_Copy__ ( oyConfigs_s_ *configs, oyObject_s object )
{
  oyConfigs_s_ *s = 0;
  int error = 0;

  if(!configs || !object)
    return s;

  s = (oyConfigs_s_*) oyConfigs_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Configs copy constructor ----- */
    error = oyConfigs_Copy__Members( s, configs );
    /* ---- end of custom Configs copy constructor ------- */
    
    
    
    
    
    oyStructList_Release( &s->list_ );
    s->list_ = oyStructList_Copy( configs->list_, s->oy_ );

  }

  if(error)
    oyConfigs_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfigs_Copy_
 *  @memberof oyConfigs_s_
 *  @brief   copy or reference a Configs object
 *
 *  @param[in]     configs                 Configs struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfigs_s_ * oyConfigs_Copy_ ( oyConfigs_s_ *configs, oyObject_s object )
{
  oyConfigs_s_ * s = configs;

  if(!configs)
    return 0;

  if(configs && !object)
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

  s = oyConfigs_Copy__( configs, object );

  return s;
}
 
/** @internal
 *  Function oyConfigs_Release_
 *  @memberof oyConfigs_s_
 *  @brief   release and possibly deallocate a Configs list
 *
 *  @param[in,out] configs                 Configs struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyConfigs_Release_( oyConfigs_s_ **configs )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyConfigs_s_ *s = 0;

  if(!configs || !*configs)
    return 0;

  s = *configs;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *configs = 0;

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
    WARNc2_S( "oyConfigs_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Configs destructor ----- */
  oyConfigs_Release__Members( s );
  /* ---- end of custom Configs destructor ------- */
  
  
  
  

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
      fprintf( stderr, "!!!ERROR:%d oyConfigs_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Configs.private_methods_definitions.c" { */

/* } Include "Configs.private_methods_definitions.c" */

