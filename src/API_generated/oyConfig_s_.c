/** @file oyConfig_s_.c

   [Template file inheritance graph]
   +-> oyConfig_s_.template.c
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



  
#include "oyConfig_s.h"
#include "oyConfig_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_config_init_ = 0;
static char * oy_config_msg_text_ = NULL;
static int oy_config_msg_text_n_ = 0;
static const char * oyConfig_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyConfig_s_ * s = (oyConfig_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_config_msg_text_ == NULL || oy_config_msg_text_n_ == 0 )
  {
    oy_config_msg_text_n_ = 512;
    oy_config_msg_text_ = (char*) alloc( oy_config_msg_text_n_ );
    if(oy_config_msg_text_)
      memset( oy_config_msg_text_, 0, oy_config_msg_text_n_ );
  }

  if( oy_config_msg_text_ == NULL || oy_config_msg_text_n_ == 0 )
    return "Memory problem";

  oy_config_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_config_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_config_msg_text_[strlen(oy_config_msg_text_)], "%d",
             s->id
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_config_msg_text_[strlen(oy_config_msg_text_)], "%d %s",
             s->id, s->registration
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_config_msg_text_[strlen(oy_config_msg_text_)], "%s id: %d",
             s->registration, s->id
           );


  return oy_config_msg_text_;
}

static void oyConfig_StaticFree_           ( void )
{
  if(oy_config_init_)
  {
    oy_config_init_ = 0;
    if(oy_config_msg_text_)
      oyFree_m_(oy_config_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyConfig_StaticFree_", "oyConfig_s" );
  }
}


/* Include "Config.private_custom_definitions.c" { */
/** @internal
 +  Function    oyConfig_Release__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConfig_Release__Members( oyConfig_s_ * config )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &config->member );
   */

  oyOptions_Release( &config->db );
  oyOptions_Release( &config->backend_core );
  oyOptions_Release( &config->data );

  if(config->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = config->oy_->deallocateFunc_;

    if(deallocateFunc)
    { 
      int i = 0;
      if(config->rank_map)
      {
        while(config->rank_map[i].key)
          deallocateFunc( config->rank_map[i++].key );
        deallocateFunc( config->rank_map );
      }
      if(config->registration)
        deallocateFunc( config->registration ); 
    }
  }
}

/** @internal
 +  Function    oyConfig_Init__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Init__Members( oyConfig_s_ * config )
{
  config->db = oyOptions_New( 0 );
  if(config->db)
    oyObject_SetName( config->db->oy_, "db", oyNAME_NICK );
  config->backend_core = oyOptions_New( 0 );
  if(config->backend_core)
    oyObject_SetName( config->backend_core->oy_, "backend_core", oyNAME_NICK );
  config->data = oyOptions_New( 0 );
  if(config->data)
    oyObject_SetName( config->data->oy_, "data", oyNAME_NICK );

  return 0;
}

/** @internal
 +  Function    oyConfig_Copy__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConfig_s_ input object
 *  @param[out]  dst  the output oyConfig_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Copy__Members( oyConfig_s_ * dst, oyConfig_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  dst->db = oyOptions_Copy( src->db, dst->oy_ );
  dst->backend_core = oyOptions_Copy( src->backend_core, dst->oy_ );
  dst->data = oyOptions_Copy( src->data, dst->oy_ );
  error = !memcpy( dst->version, src->version, 3*sizeof(int) );

  dst->rank_map = oyRankMapCopy( src->rank_map, allocateFunc_ );

  return error;
}

/* } Include "Config.private_custom_definitions.c" */


/** @internal
 *  Function oyConfig_New_
 *  @memberof oyConfig_s_
 *  @brief   allocate a new oyConfig_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIG_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyConfig_s" );
  oyConfig_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 2: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 3: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 4: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 5: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 6: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 7: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 8: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 9: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 10: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 11: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 12: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 13: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 14: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 15: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 16: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 17: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 18: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 19: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 20: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 21: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 22: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 23: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 24: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 25: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 26: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 27: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 28: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 29: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 30: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 31: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 32: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 33: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 34: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 35: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 36: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 37: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 38: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      case 39: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_)); break;
      default: s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_));
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

  error = !memset( s, 0, sizeof(oyConfig_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConfig_Copy;
  s->release = (oyStruct_Release_f) oyConfig_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Config constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONFIG_S, (oyPointer)s );
  /* ---- end of custom Config constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Config constructor ----- */
  error += oyConfig_Init__Members( s );
  /* ---- end of custom Config constructor ------- */
  
  
  
  

  if(!oy_config_init_)
  {
    oy_config_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyConfig_StaticMessageFunc_,
                                        oyConfig_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyConfig_Copy__
 *  @memberof oyConfig_s_
 *  @brief   real copy a Config object
 *
 *  @param[in]     config                 Config struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_Copy__ ( oyConfig_s_ *config, oyObject_s object )
{
  oyConfig_s_ *s = 0;
  int error = 0;

  if(!config || !object)
    return s;

  s = (oyConfig_s_*) oyConfig_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Config copy constructor ----- */
    error = oyConfig_Copy__Members( s, config );
    /* ---- end of custom Config copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyConfig_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfig_Copy_
 *  @memberof oyConfig_s_
 *  @brief   copy or reference a Config object
 *
 *  @param[in]     config                 Config struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_Copy_ ( oyConfig_s_ *config, oyObject_s object )
{
  oyConfig_s_ * s = config;

  if(!config)
    return 0;

  if(config && !object)
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

  s = oyConfig_Copy__( config, object );

  return s;
}
 
/** @internal
 *  Function oyConfig_Release_
 *  @memberof oyConfig_s_
 *  @brief   release and possibly deallocate a Config object
 *
 *  @param[in,out] config                 Config struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyConfig_Release_( oyConfig_s_ **config )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyConfig_s_ *s = 0;

  if(!config || !*config)
    return 0;

  s = *config;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *config = 0;

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
    WARNc2_S( "oyConfig_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Config destructor ----- */
  oyConfig_Release__Members( s );
  /* ---- end of custom Config destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyConfig_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Config.private_methods_definitions.c" { */

/* } Include "Config.private_methods_definitions.c" */

