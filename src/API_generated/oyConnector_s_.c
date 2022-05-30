/** @file oyConnector_s_.c

   [Template file inheritance graph]
   +-> oyConnector_s_.template.c
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



  
#include "oyConnector_s.h"
#include "oyConnector_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_connector_init_ = 0;
static char * oy_connector_msg_text_ = NULL;
static int oy_connector_msg_text_n_ = 0;
static const char * oyConnector_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyConnector_s_ * s = (oyConnector_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_connector_msg_text_ == NULL || oy_connector_msg_text_n_ == 0 )
  {
    oy_connector_msg_text_n_ = 512;
    oy_connector_msg_text_ = (char*) alloc( oy_connector_msg_text_n_ );
    if(oy_connector_msg_text_)
      memset( oy_connector_msg_text_, 0, oy_connector_msg_text_n_ );
  }

  if( oy_connector_msg_text_ == NULL || oy_connector_msg_text_n_ == 0 )
    return "Memory problem";

  oy_connector_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_connector_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_connector_msg_text_;
}

static void oyConnector_StaticFree_           ( void )
{
  if(oy_connector_init_)
  {
    oy_connector_init_ = 0;
    if(oy_connector_msg_text_)
      oyFree_m_(oy_connector_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyConnector_StaticFree_", "oyConnector_s" );
  }
}


/* Include "Connector.private_custom_definitions.c" { */
/** Function    oyConnector_Release__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  connector  the Connector object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConnector_Release__Members( oyConnector_s_ * connector )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &connector->member );
   */

  if(connector->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = connector->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( connector->member );
     */
    if(connector->connector_type) {
      deallocateFunc( connector->connector_type );
      connector->connector_type = 0;
    }
  }
}

/** Function    oyConnector_Init__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  connector  the Connector object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnector_Init__Members( oyConnector_s_ * connector )
{
  connector->is_plug = -1;

  return 0;
}

/** Function    oyConnector_Copy__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConnector_s_ input object
 *  @param[out]  dst  the output oyConnector_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnector_Copy__Members( oyConnector_s_ * dst, oyConnector_s_ * src)
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
  error = oyObject_CopyNames( dst->oy_, src->oy_ );

  dst->connector_type = oyStringCopy_( src->connector_type, allocateFunc_ );
  dst->is_plug = src->is_plug;

  return error;
}

/* } Include "Connector.private_custom_definitions.c" */


/** @internal
 *  Function oyConnector_New_
 *  @memberof oyConnector_s_
 *  @brief   allocate a new oyConnector_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnector_s_ * oyConnector_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONNECTOR_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyConnector_s" );
  oyConnector_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 2: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 3: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 4: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 5: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 6: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 7: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 8: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 9: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 10: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 11: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 12: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 13: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 14: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 15: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 16: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 17: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 18: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 19: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 20: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 21: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 22: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 23: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 24: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 25: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 26: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 27: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 28: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 29: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 30: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 31: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 32: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 33: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 34: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 35: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 36: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 37: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 38: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      case 39: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_)); break;
      default: s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_));
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

  error = !memset( s, 0, sizeof(oyConnector_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConnector_Copy;
  s->release = (oyStruct_Release_f) oyConnector_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Connector constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONNECTOR_S, (oyPointer)s );
  /* ---- end of custom Connector constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Connector constructor ----- */
  error += oyConnector_Init__Members( s );
  /* ---- end of custom Connector constructor ------- */
  
  
  
  

  if(!oy_connector_init_)
  {
    oy_connector_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyConnector_StaticMessageFunc_,
                                        oyConnector_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyConnector_Copy__
 *  @memberof oyConnector_s_
 *  @brief   real copy a Connector object
 *
 *  @param[in]     connector                 Connector struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnector_s_ * oyConnector_Copy__ ( oyConnector_s_ *connector, oyObject_s object )
{
  oyConnector_s_ *s = 0;
  int error = 0;

  if(!connector || !object)
    return s;

  s = (oyConnector_s_*) oyConnector_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Connector copy constructor ----- */
    error = oyConnector_Copy__Members( s, connector );
    /* ---- end of custom Connector copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyConnector_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConnector_Copy_
 *  @memberof oyConnector_s_
 *  @brief   copy or reference a Connector object
 *
 *  @param[in]     connector                 Connector struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnector_s_ * oyConnector_Copy_ ( oyConnector_s_ *connector, oyObject_s object )
{
  oyConnector_s_ * s = connector;

  if(!connector)
    return 0;

  if(connector && !object)
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

  s = oyConnector_Copy__( connector, object );

  return s;
}
 
/** @internal
 *  Function oyConnector_Release_
 *  @memberof oyConnector_s_
 *  @brief   release and possibly deallocate a Connector object
 *
 *  @param[in,out] connector                 Connector struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyConnector_Release_( oyConnector_s_ **connector )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyConnector_s_ *s = 0;

  if(!connector || !*connector)
    return 0;

  s = *connector;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *connector = 0;

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
    WARNc2_S( "oyConnector_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Connector destructor ----- */
  oyConnector_Release__Members( s );
  /* ---- end of custom Connector destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyConnector_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Connector.private_methods_definitions.c" { */

/* } Include "Connector.private_methods_definitions.c" */

