/** @file oyFilterNode_s_.c

   [Template file inheritance graph]
   +-> oyFilterNode_s_.template.c
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



  
#include "oyFilterNode_s.h"
#include "oyFilterNode_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"




#include "oyHash_s.h"
#include "oyranos_module_internal.h"
#include "oyranos_object_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_filternode_init_ = 0;
static char * oy_filternode_msg_text_ = NULL;
static int oy_filternode_msg_text_n_ = 0;
static const char * oyFilterNode_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_filternode_msg_text_ == NULL || oy_filternode_msg_text_n_ == 0 )
  {
    oy_filternode_msg_text_n_ = 512;
    oy_filternode_msg_text_ = (char*) alloc( oy_filternode_msg_text_n_ );
    if(oy_filternode_msg_text_)
      memset( oy_filternode_msg_text_, 0, oy_filternode_msg_text_n_ );
  }

  if( oy_filternode_msg_text_ == NULL || oy_filternode_msg_text_n_ == 0 )
    return "Memory problem";

  oy_filternode_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_filternode_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_filternode_msg_text_[strlen(oy_filternode_msg_text_)], "%s",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_)
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_filternode_msg_text_[strlen(oy_filternode_msg_text_)], "%s %d/%d",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_), s->plugs_n_, s->sockets_n_
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_filternode_msg_text_[strlen(oy_filternode_msg_text_)], "reg: %s\nrelatives: %s\nplugs: %d sockets: %d context: %s",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_), s->relatives_,
             s->plugs_n_, s->sockets_n_, oyNoEmptyString_m_(s->api7_?s->api7_->context_type:"???")
           );


  return oy_filternode_msg_text_;
}

static void oyFilterNode_StaticFree_           ( void )
{
  if(oy_filternode_init_)
  {
    oy_filternode_init_ = 0;
    if(oy_filternode_msg_text_)
      oyFree_m_(oy_filternode_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyFilterNode_StaticFree_", "oyFilterNode_s" );
  }
}


/* Include "FilterNode.private_custom_definitions.c" { */
/** Function    oyFilterNode_Release__Members
 *  @memberof   oyFilterNode_s
 *  @brief      Custom FilterNode destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filternode  the FilterNode object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterNode_Release__Members( oyFilterNode_s_ * filternode )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filternode->member );
   */
  int i = 0,n, cid = oyObject_GetId(filternode->oy_);
  static int oid[20] = {0,0,0,0,0, 0,0,0,0,0,
                        0,0,0,0,0, 0,0,0,0,0};
  int oid_n = 0, twice = 0, first_empty = -1;
  if(oy_debug_objects >= 0)
  {
    for(i = 0; i < 20; ++i)
    {
      if(oid[i]) ++oid_n;
      else if(first_empty == -1)
        first_empty = i;
      if(oid[i] == cid)
        ++twice;
    }
    if(first_empty >= 0)
      oid[first_empty] = cid;
 
    fprintf(stderr, "%s[%d](start) %d %s\n",__func__, cid, oid_n,  filternode->core->registration_);
    if(twice)
      fprintf(stderr, "\n!!!ERROR: nested releasing same object: %s %d refs: %d\n\n", oyFilterNode_StaticMessageFunc_(filternode, oyNAME_DESCRIPTION, 2), twice, oyObject_GetRefCount(filternode->oy_));
  }

  oyOptions_Release( &filternode->tags );

  if(filternode->sockets)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)filternode, 0, 0 );
    for(i = 0; i < n; ++i)
      if(filternode->sockets[i])
      {
        if(filternode->sockets[i]->requesting_plugs_)
        {
          oyFilterPlugs_s * remote_plugs = filternode->sockets[i]->requesting_plugs_;
          int count = oyFilterPlugs_Count( remote_plugs ), j;
          for(j = 0; j < count; ++j)
          {
            oyFilterPlug_s * remote_plug = oyFilterPlugs_Get( remote_plugs, j );
            oyFilterSocket_Callback( remote_plug, oyCONNECTOR_EVENT_RELEASED );
            oyFilterPlug_Callback( remote_plug, oyCONNECTOR_EVENT_RELEASED );
            oyFilterPlug_Release( &remote_plug );
          }
        }
        if((oy_debug || oy_debug_objects >= 0) &&
            oyObject_GetRefCount(filternode->sockets[i]->oy_) > 2)
        {
          fprintf(stderr, "node[%d]->socket[%d] may not be released. refs: %d\n", cid, filternode->sockets[i]->oy_->id_, oyObject_GetRefCount(filternode->sockets[i]->oy_));
        }
        oyFilterSocket_Release( (oyFilterSocket_s **)&filternode->sockets[i] );
      }
  }

  if(filternode->plugs)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)filternode, 1, 0 );
    for(i = 0; i < n; ++i)
      if(filternode->plugs[i])
      {
        oyFilterNode_Disconnect( (oyFilterNode_s*)filternode, i );
        if((oy_debug || oy_debug_objects >= 0) &&
            oyObject_GetRefCount(filternode->plugs[i]->oy_) > 2)
        {
          oyStruct_s ** parents = NULL;
          int ps = oyStruct_GetParents( (oyStruct_s*)filternode->plugs[i], &parents ), j;
          fprintf(stderr, "!!!ERROR: node[%d]->plug[%d] can not be released with refs: %d\n", cid, filternode->plugs[i]->oy_->id_, oyObject_GetRefCount(filternode->plugs[i]->oy_));
          for(j = 0; j < ps; ++j)
          {
            const char * track_name = oyStructTypeToText(parents[j]->type_);
            if(!(track_name && track_name[0]))
              track_name = oyStruct_GetInfo( parents[i], oyNAME_NICK, 0x01 );
            fprintf( stderr, "[%d]: parent[%d]: %s[%d]\n",
                     filternode->plugs[i]->oy_->id_, j,
                     track_name, parents[j]->oy_->id_ );
          }
        }
        oyFilterPlug_Release( (oyFilterPlug_s **)&filternode->plugs[i] );
      }
  }

  if( filternode->backend_data && filternode->backend_data->release )
    filternode->backend_data->release( (oyStruct_s**) & filternode->backend_data );
  filternode->backend_data = 0;

  if(filternode->api7_ && filternode->api7_->release)
    filternode->api7_->release( (oyStruct_s**) &filternode->api7_ );

  oyFilterCore_Release( (oyFilterCore_s**)&filternode->core );

  if(filternode->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filternode->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filternode->member );
     */
    if(filternode->relatives_)
      deallocateFunc( filternode->relatives_ );
    filternode->relatives_ = NULL;
    if(filternode->sockets) deallocateFunc(filternode->sockets);
    filternode->sockets = NULL;
    if(filternode->plugs) deallocateFunc(filternode->plugs);
    filternode->plugs = NULL;
  }

  if(oy_debug_objects >= 0)
  {
    if(first_empty >= 0)
      oid[first_empty] = 0;
    fprintf(stderr, "%s[%d](end)\n", __func__, cid);
  }
}

/** Function    oyFilterNode_Init__Members
 *  @memberof   oyFilterNode_s
 *  @brief      Custom FilterNode constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filternode  the FilterNode object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterNode_Init__Members( oyFilterNode_s_ * filternode OY_UNUSED )
{
  /**  Is this necessary??
   *   filternode->relatives_ = 0;
   *
   *   if( filternode->backend_data && filternode->backend_data->release )
   *     filternode->backend_data->release( (oyStruct_s**) & filternode->backend_data );
   *   filternode->backend_data = 0;
   */

  return 0;
}

/** Function    oyFilterNode_Copy__Members
 *  @memberof   oyFilterNode_s
 *  @brief      Custom FilterNode copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterNode_s_ input object
 *  @param[out]  dst  the output oyFilterNode_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterNode_Copy__Members( oyFilterNode_s_ * dst, oyFilterNode_s_ * src)
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
  if(src->backend_data && src->backend_data->copy)
    dst->backend_data = (oyPointer_s*) src->backend_data->copy( (oyStruct_s*)
                                                src->backend_data , dst->oy_ );
  if(oy_debug_objects >= 0 && dst->backend_data)
    oyObjectDebugMessage_( dst->backend_data->oy_, __func__,
                           oyStructTypeToText(dst->backend_data->type_) );

  return error;
}

/* } Include "FilterNode.private_custom_definitions.c" */


/** @internal
 *  Function oyFilterNode_New_
 *  @memberof oyFilterNode_s_
 *  @brief   allocate a new oyFilterNode_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterNode_s_ * oyFilterNode_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_NODE_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyFilterNode_s" );
  oyFilterNode_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 2: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 3: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 4: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 5: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 6: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 7: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 8: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 9: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 10: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 11: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 12: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 13: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 14: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 15: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 16: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 17: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 18: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 19: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 20: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 21: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 22: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 23: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 24: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 25: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 26: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 27: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 28: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 29: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 30: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 31: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 32: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 33: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 34: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 35: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 36: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 37: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 38: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      case 39: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_)); break;
      default: s = (oyFilterNode_s_*)s_obj->allocateFunc_(sizeof(oyFilterNode_s_));
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

  error = !memset( s, 0, sizeof(oyFilterNode_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterNode_Copy;
  s->release = (oyStruct_Release_f) oyFilterNode_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterNode constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_NODE_S, (oyPointer)s );
  /* ---- end of custom FilterNode constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom FilterNode constructor ----- */
  error += oyFilterNode_Init__Members( s );
  /* ---- end of custom FilterNode constructor ------- */
  
  
  
  

  if(!oy_filternode_init_)
  {
    oy_filternode_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyFilterNode_StaticMessageFunc_,
                                        oyFilterNode_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyFilterNode_Copy__
 *  @memberof oyFilterNode_s_
 *  @brief   real copy a FilterNode object
 *
 *  @param[in]     filternode                 FilterNode struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterNode_s_ * oyFilterNode_Copy__ ( oyFilterNode_s_ *filternode, oyObject_s object )
{
  oyFilterNode_s_ *s = 0;
  int error = 0;

  if(!filternode || !object)
    return s;

  s = (oyFilterNode_s_*)oyFilterNode_Create( filternode->api7_->registration, (oyFilterCore_s*)filternode->core, object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterNode copy constructor ----- */
    error = oyFilterNode_Copy__Members( s, filternode );
    /* ---- end of custom FilterNode copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyFilterNode_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterNode_Copy_
 *  @memberof oyFilterNode_s_
 *  @brief   copy or reference a FilterNode object
 *
 *  @param[in]     filternode                 FilterNode struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterNode_s_ * oyFilterNode_Copy_ ( oyFilterNode_s_ *filternode, oyObject_s object )
{
  oyFilterNode_s_ * s = filternode;

  if(!filternode)
    return 0;

  if(filternode && !object)
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

  s = oyFilterNode_Copy__( filternode, object );

  return s;
}
 
/** @internal
 *  Function oyFilterNode_Release_
 *  @memberof oyFilterNode_s_
 *  @brief   release and possibly deallocate a FilterNode object
 *
 *  @param[in,out] filternode                 FilterNode struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyFilterNode_Release_( oyFilterNode_s_ **filternode )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyFilterNode_s_ *s = 0;

  if(!filternode || !*filternode)
    return 0;

  s = *filternode;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *filternode = 0;

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
  uint32_t s_n = 0, p_n = 0, sn_n = 0, pn_n = 0, i, n;
  int r OY_UNUSED = oyObject_UnRef(s->oy_);

  if(s->sockets)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 0, 0 );
    for(i = 0; i < n; ++i)
      if(s->sockets[i])
      {
        if(s->sockets[i]->node)
          ++sn_n;
        ++s_n;
      }
  }

  if(s->plugs)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 1, 0 );
    for(i = 0; i < n; ++i)
      if(s->plugs[i])
      {
        if(s->plugs[i]->node)
          ++pn_n;
        ++p_n;
      }
  }

  /* referenences from members has to be substracted
   * from this objects ref count */
  if(oyObject_GetRefCount( s->oy_ ) > (int)(parent_refs + sn_n + pn_n + observer_refs*2))
    return 0;

  if(oy_debug_objects >= 0 && s->oy_)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      fprintf(stderr, "prepare destruct %s[%d] refs: %d sockets: %d|%d plugs: %d|%d\n", track_name, s->oy_->id_, s->oy_->ref_, s_n,sn_n, p_n,pn_n);
    }
  }

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
    WARNc2_S( "oyFilterNode_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom FilterNode destructor ----- */
  oyFilterNode_Release__Members( s );
  /* ---- end of custom FilterNode destructor ------- */
  
  
  
  

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
      fprintf( stderr, "!!!ERROR:%d oyFilterNode_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "FilterNode.private_methods_definitions.c" { */
#include "oyranos_conversion_internal.h"
#include "oyCMMapi9_s_.h"

/** Function  oyFilterNode_AddToAdjacencyLst_
 *  @memberof oyFilterNode_s
 *  @brief    Get a graphs adjazency list
 *  @internal
 *
 *  @param[in]     s                   the start node
 *  @param[in,out] nodes               the collected nodes
 *  @param[in,out] edges               the collected edges
 *  @param[in]     mark                a search string to get sub graphs
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int  oyFilterNode_AddToAdjacencyLst_ ( oyFilterNode_s_    * s,
                                       oyFilterNodes_s_   * nodes,
                                       oyFilterPlugs_s_   * edges,
                                       const char         * mark,
                                       int                  flags )
{
  int n, i, j, p_n;
  oyFilterPlug_s_ * p = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  /* Scan the input/plug side for unknown nodes, add these and continue in
   * the direction of previous unknown edges...
   */
  if(!(flags & OY_INPUT))
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 1, 0 );
    for( i = 0; i < n; ++i )
    {
      if( s->plugs[i] && s->plugs[i]->remote_socket_ )
        if(oyAdjacencyListAdd_( (oyFilterPlug_s*)s->plugs[i], (oyFilterNodes_s*)nodes, (oyFilterPlugs_s*)edges, mark, flags ))
          oyFilterNode_AddToAdjacencyLst_( ((oyFilterSocket_s_*)s->plugs[i]->remote_socket_)->node,
                                           nodes, edges, mark, flags );
    }
  }

  /* ... same on the output/socket side */
  if(!(flags & OY_OUTPUT))
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 0, 0 );
    for( i = 0; i < n; ++i )
    {
      if( s->sockets[i] && s->sockets[i]->requesting_plugs_ )
      {
        p_n = oyFilterPlugs_Count( s->sockets[i]->requesting_plugs_ );
        for( j = 0; j < p_n; ++j )
        {
          p = (oyFilterPlug_s_*)oyFilterPlugs_Get( s->sockets[i]->requesting_plugs_, j );

          if(oyAdjacencyListAdd_( (oyFilterPlug_s*)p, (oyFilterNodes_s*)nodes, (oyFilterPlugs_s*)edges, mark, flags ))
            oyFilterNode_AddToAdjacencyLst_( (oyFilterNode_s_*)p->node,
                                             nodes, edges, mark, flags );
          oyFilterPlug_Release( (oyFilterPlug_s**)&p );
        }
      }
    }
  }

  return 0;
}

oyHash_s *   oyFilterNode_GetHash_   ( oyFilterNode_s_   * node,
                                       int                 api )
{
  oyFilterCore_s_ * core_ = node->core;
  const char * hash_text_ = 0;
  char * hash_text = 0,
       * hash_temp = 0;
  oyHash_s * hash = 0;
  oyAlloc_f alloc = oyStruct_GetAllocator( (oyStruct_s*)  node ) ? oyStruct_GetAllocator( (oyStruct_s*)  node ) : oyAllocateFunc_;
  oyDeAlloc_f dealloc = oyStruct_GetDeAllocator( (oyStruct_s*)  node ) ? oyStruct_GetDeAllocator( (oyStruct_s*)  node ) : oyDeAllocateFunc_;

  /* create hash text */
  if(core_->api4_->oyCMMFilterNode_GetText)
  {
    hash_temp = core_->api4_->oyCMMFilterNode_GetText( (oyFilterNode_s*)node,
                                 api == 4 ? oyNAME_NICK : oyNAME_DESCRIPTION,
                                                       oyAllocateFunc_ );
    hash_text_ = hash_temp;
  } else
    hash_text_ = oyFilterNode_GetText((oyFilterNode_s*)node,oyNAME_NICK);

  if(api == 7)
    oyStringAddPrintf_( &hash_text, alloc, dealloc,
                        "%s:%s", node->api7_->context_type, hash_text_ );
  if(api == 4)
    oyStringAddPrintf_( &hash_text, alloc, dealloc,
                        "%s:%s", core_->api4_->context_type, hash_text_ );

  /* query in cache for api7 */
  hash = oyCMMCacheListGetEntry_( hash_text );

  if(oy_debug >= 2)
  {
    oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*) node,
                     OY_DBG_FORMAT_ "api: %d hash_text: \"%s\"",
                     OY_DBG_ARGS_,
                     api, hash_text );
  }

  if(hash_temp) oySTRUCT_FREE_m(core_, hash_temp);
  if(hash_text) oySTRUCT_FREE_m(core_, hash_text);

  return hash;
}

char *             oyFilterNode_GetFallback_(
                                       oyFilterNode_s_   * node,
                                       int                 select_core )
{
  char * fallback = NULL;

  oyCMMapiFilters_s * apis;
  int apis_n = 0, i;
  oyCMMapi9_s_ * cmm_api9_ = 0;
  char * class_name, * api_reg;

  oyFilterCore_s_ * core_ = node->core;
  const char * pattern = core_->registration_;

  class_name = oyFilterRegistrationToText( pattern, oyFILTER_REG_APPLICATION,0);
  api_reg = oyStringCopy_("///", oyAllocateFunc_ );
  STRING_ADD( api_reg, class_name );
  oyFree_m_( class_name );

  apis = oyCMMsGetFilterApis_( api_reg, oyOBJECT_CMM_API9_S,
                               oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                               0,0 );
  oyFree_m_( api_reg );
  apis_n = oyCMMapiFilters_Count( apis );
  for(i = 0; i < apis_n; ++i)
  {
    cmm_api9_ = (oyCMMapi9_s_*) oyCMMapiFilters_Get( apis, i );

    if(oyFilterRegistrationMatch( cmm_api9_->pattern, pattern, 0 ))
    {
      if(cmm_api9_->oyCMMGetFallback)
        fallback = cmm_api9_->oyCMMGetFallback( (oyFilterNode_s*)node, 0,
                                                select_core, oyAllocateFunc_ );
      if(!fallback)
        WARNc2_S( "%s %s",_("error in module:"), cmm_api9_->registration );
    }

    if(cmm_api9_->release)
      cmm_api9_->release( (oyStruct_s**)&cmm_api9_ );
  }
  oyCMMapiFilters_Release( &apis );

  return fallback;
}

/** Function  oyFilterNode_SetFromPattern_
 *  @memberof oyFilterNode_s
 *  @brief    Set module core in a filter
 *  @internal
 *
 *  @param[in]     node                filter
 *  @param[in]     select_core         1 - core/context, 0 - processor/renderer
 *  @param[in]     pattern             registration pattern
 *  @return                            error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/01
 *  @since   2004/06/26 (Oyranos: 0.9.6)
 */
int          oyFilterNode_SetFromPattern_ (
                                       oyFilterNode_s_   * node,
                                       int                 select_core,
                                       const char        * pattern )
{
  if(select_core)
  {
    oyFilterCore_s_ * core;

    core= (oyFilterCore_s_*)oyFilterCore_NewWith( pattern, node->core->options_,
                                                  NULL );
    if(core)
    {
      oyFilterCore_Release( (oyFilterCore_s**) &node->core );

      node->core = core;
      return 0;
    }
    else
      oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*) node,
                       OY_DBG_FORMAT_ "could not create new core: %s",
                       OY_DBG_ARGS_,
                       pattern);
  } else
  {
    oyFilterNode_s_ * node_ = (oyFilterNode_s_*) oyFilterNode_Create( pattern, (oyFilterCore_s*)node->core, NULL );

    if(node_)
    {
      if(node->api7_->release)
        node->api7_->release( (oyStruct_s**) &node->api7_ );
      node->api7_ = node_->api7_;
      return 0;
    }
    else
      oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*) node,
                       OY_DBG_FORMAT_ "could not create new node module: %s",
                       OY_DBG_ARGS_,
                       pattern);
  }

  return 1;
}

void *       oyFilterNode_ContextToMem_
                                     ( oyFilterNode_s_   * node,
                                       size_t            * size,
                                       oyAlloc_f           alloc )
{
  oyFilterCore_s_ * core_ = node->core;
  oyPointer ptr = core_->api4_->oyCMMFilterNode_ContextToMem(
                                                    (oyFilterNode_s*)node, size,
                                                              alloc );
  return ptr;
}

/** Function  oyFilterNode_SetContext_
 *  @memberof oyFilterNode_s
 *  @brief    Set module context in a filter
 *  @internal
 *
 *  The api4 data is passed to a interpolator specific transformer. The result
 *  of this transformer will on request be cached by Oyranos as well.
 *
 *  @param[in]     node                filter
 *  @param[in,out] blob                context to fill; expensive
 *  @return                            error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/26
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 */
int          oyFilterNode_SetContext_( oyFilterNode_s_    * node,
                                       oyBlob_s_          * blob  )
{
  int error = 0;
  oyFilterCore_s_ * core_ = node->core;
  oyHash_s * hash4 = 0,          /* public context provider */
           * hash7 = 0;          /* data processor part */
  oyOption_s * ct = NULL;        /* context module option */


  if(error <= 0)
  {
          size_t size = 0;
          oyPointer ptr = 0;
          oyPointer_s * cmm_ptr4 = 0,
                      * cmm_ptr7 = 0;

          /*  Cache Search
           *  1.     hash from input
           *  2.     query for hash in cache
           *  3.     check
           *  3a.       eighter take cache entry
           *  3b.       or ask CMM
           *  3b.1.                update cache entry
           */


          if(oy_debug && getenv("OY_DEBUG_WRITE"))
          {
            size = 0;
            ptr = oyFilterNode_TextToInfo_( node, &size, oyAllocateFunc_ );
            if(ptr)
              oyWriteMemToFile_( "test_dbg_color.icc", ptr, size );
          }

          /* 1. + 2. query in cache for api7 */
          hash7 = oyFilterNode_GetHash_(node, 7);

          if(error <= 0)
          {
            /* select the module by option */
            ct = oyOptions_Find( node->core->options_, "////context",
                                                       oyNAME_PATTERN );
            const char * pattern = oyOption_GetValueString( ct, 0 );
            if(pattern &&
               !oyFilterRegistrationMatch( core_->registration_, pattern, 0 ))
            {
              oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*) node,
                               OY_DBG_FORMAT_ "create core from pattern: %s",
                               OY_DBG_ARGS_,
                     oyFilterNode_GetText( (oyFilterNode_s*)node,oyNAME_NICK) );

              error = oyFilterNode_SetFromPattern_( node, 1, pattern );

              if(error)
              {
                if(oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT)
                {
                  oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*) node,
                               OY_DBG_FORMAT_ "edited pattern not available: %d %s",
                               OY_DBG_ARGS_, oyObject_GetId(ct->oy_),
                                   pattern );
                  error = 1;
                  goto clean;
                } else
                  error = 0;
              } else
                core_ = node->core;

              oyHash_Release( &hash7 );
              hash7 = oyFilterNode_GetHash_(node, 7);
            }
            oyOption_Release( &ct );

            ct = oyOptions_Find( node->core->options_,
                                                       "////renderer",
                                                       oyNAME_PATTERN );
            pattern = oyOption_GetValueString( ct, 0 );
            if(pattern &&
               !oyFilterRegistrationMatch( node->api7_->registration, pattern, 0 ))
            {
              oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*) node,
                               OY_DBG_FORMAT_ "create node from pattern: %s",
                               OY_DBG_ARGS_,
                     oyFilterNode_GetText( (oyFilterNode_s*)node,oyNAME_NICK) );

              error = oyFilterNode_SetFromPattern_( node, 0, pattern );

              if(error)
              {
                if(oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT)
                {
                  error = 1;
                  goto clean;
                } else
                  error = 0;
              } else
                core_ = node->core;

              oyHash_Release( &hash7 );
              hash7 = oyFilterNode_GetHash_(node, 7);
            }

            /* 3. check and 3.a take*/
            cmm_ptr7 = (oyPointer_s*) oyHash_GetPointer( hash7,
                                                         oyOBJECT_POINTER_S);

            if(!(cmm_ptr7 && oyPointer_GetPointer(cmm_ptr7)) || blob)
            {
              /* write the cmm4 context to memory */
              if(blob)
              {
                if(oy_debug)
                error = oyOptions_SetFromString( &node->tags, "////verbose",
                                               "true", OY_CREATE_NEW );

                /* oy_debug is used to obtain a complete data set */
                ptr = oyFilterNode_ContextToMem_( node, &size, oyAllocateFunc_);
                oyBlob_SetFromData( (oyBlob_s*)blob, ptr, size,
                                    core_->api4_->context_type );
                oyFree_m_(ptr);
                if(oy_debug)
                error = oyOptions_SetFromString( &node->tags, "////verbose",
                                               "false", 0 );

                oyPointer_Release( &cmm_ptr7 );
                goto clean;
              }

              /* 2. query in cache for api4 */
              oyHash_Release( &hash4 );
              hash4 = oyFilterNode_GetHash_(node, 4);
              cmm_ptr4 = (oyPointer_s*) oyHash_GetPointer( hash4,
                                                        oyOBJECT_POINTER_S);

              if(!cmm_ptr4)
              {
                cmm_ptr4 = oyPointer_New(0);
              }

              if(!oyPointer_GetPointer(cmm_ptr4))
              {
                size = 0;
                /* 3b. ask CMM */
                ptr = oyFilterNode_ContextToMem_( node, &size, oyAllocateFunc_);

                if(!ptr || !size)
                {
                  oyOption_s * ct = oyOptions_Find( core_->options_,
                                                    "////context",
                                                    oyNAME_PATTERN );
                  oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*) node,
                    OY_DBG_FORMAT_ "device link creation failed", OY_DBG_ARGS_);
                  if(!(oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT))
                  {
                    char * pattern = oyFilterNode_GetFallback_( node, 1 );

                    oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*) node,
                               OY_DBG_FORMAT_ "create core from fallback: %s",
                               OY_DBG_ARGS_, pattern );

                    error = oyFilterNode_SetFromPattern_( node, 1, pattern );
                    if(error)
                    {
                      error = 1;
                      oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*) node,
                      OY_DBG_FORMAT_ "no device link for caching\n%s",
                      OY_DBG_ARGS_,
                      oyFilterNode_GetText( (oyFilterNode_s*)node,oyNAME_NICK));
                      goto clean;
                    } else
                      core_ = node->core;
                    
                    ptr = oyFilterNode_ContextToMem_( node,
                                                      &size, oyAllocateFunc_ );
                    oyFree_m_( pattern );
                  }

                  if(!ptr || !size)
                  {
                    oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*) node,
                      OY_DBG_FORMAT_ "no device link for caching\n%s",
                      OY_DBG_ARGS_,
                      oyFilterNode_GetText( (oyFilterNode_s*)node,oyNAME_NICK));

                    error = 1;
                    oyPointer_Release( &cmm_ptr4 );

                  } else if(oy_debug)
                    oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*) node,
                        OY_DBG_FORMAT_ "use fallback CMM\n%s", OY_DBG_ARGS_,
                        oyFilterNode_GetText( (oyFilterNode_s*)node,
                                              oyNAME_NICK ));
                }

                if(!error)
                {
                  /* 3b.1. update the hash as the CMM can change options */
                  oyHash_Release( &hash4 );
                  hash4 = oyFilterNode_GetHash_( node, 4 );
                  oyPointer_Release( &cmm_ptr4 );
                  cmm_ptr4 = (oyPointer_s*) oyHash_GetPointer( hash4,
                                                        oyOBJECT_POINTER_S);
                  oyHash_Release( &hash7 );
                  hash7 = oyFilterNode_GetHash_( node, 7 );

                  if(!cmm_ptr4)
                    cmm_ptr4 = oyPointer_New(0);

                  error = oyPointer_Set( cmm_ptr4, core_->api4_->id_,
                                         core_->api4_->context_type,
                                    ptr, "oyPointerRelease", oyPointerRelease);
                  oyPointer_SetSize( cmm_ptr4, size );

                  /* 3b.2. update cmm4 cache entry */
                  error = oyHash_SetPointer( hash4, (oyStruct_s*) cmm_ptr4);
                }
              }


              if(error <= 0 && cmm_ptr4 && oyPointer_GetPointer(cmm_ptr4))
              {
                if(node->backend_data && node->backend_data->release)
                node->backend_data->release( (oyStruct_s**)&node->backend_data);

                if( oyStrcmp_( node->api7_->context_type,
                               core_->api4_->context_type ) != 0 )
                {
                  cmm_ptr7 = oyPointer_New(0);
                  error = oyPointer_Set( cmm_ptr7, node->api7_->id_,
                                         node->api7_->context_type, 0, 0, 0);

                  /* 3b.3. search for a convertor and convert */
                  oyPointer_ConvertData( cmm_ptr4, cmm_ptr7,
                                         (oyFilterNode_s*)node );
                  node->backend_data = cmm_ptr7;
                  /* 3b.4. update cmm7 cache entry */
                  error = oyHash_SetPointer( hash7,
                                              (oyStruct_s*) cmm_ptr7);

                } else
                  node->backend_data = oyPointer_Copy( cmm_ptr4, 0 );
              }

              if(oy_debug && getenv("OY_DEBUG_WRITE"))
              {
                int id = oyFilterNode_GetId( (oyFilterNode_s*)node );
                char * file_name = 0;
                oyAllocHelper_m_( file_name, char, 80, 0, return 1 );
                sprintf( file_name, "dbg_color_dl-node[%d].icc", id );
                if(ptr && size && node->backend_data)
                  oyWriteMemToFile_( file_name, ptr, size );
                oyFree_m_(file_name);
              }

            } else
              node->backend_data = cmm_ptr7;

          }
          oyPointer_Release( &cmm_ptr4 );
  }

  clean:
    oyHash_Release( &hash4 );
    oyHash_Release( &hash7 );
    oyOption_Release( &ct );

  return error;
}

/** Function  oyFilterNode_GetData_
 *  @memberof oyFilterNode_s
 *  @brief    Get the processing data from a filter node
 *  @internal
 *
 *  @param[in]     node                filter
 *  @param[in]     get_plug            1 get input, 0 get output data
 *  @return                            the data list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 *  @date    2008/11/04
 */
oyStructList_s * oyFilterNode_GetData_(oyFilterNode_s_    * node,
                                       int                  get_plug )
{
  int error = !node;
  oyStructList_s * datas = 0;
  oyStruct_s * data = 0;
  int i, n;

  if(error <= 0)
  {
    datas = oyStructList_Create(0, "oyFilterNode_GetData_", 0);

    if(get_plug)
    {
          /* pick all plug (input) data */
          n = oyFilterNode_EdgeCount( (oyFilterNode_s*)node, 1, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->plugs[i])
          {
            data = 0;
            if(node->plugs[i]->remote_socket_->data)
            {
              data = node->plugs[i]->remote_socket_->data->copy( node->plugs[i]->remote_socket_->data, 0 );
              if(oy_debug_objects >= 0 && data)
                oyObjectDebugMessage_( data->oy_, __func__,
                                       oyStructTypeToText(data->type_) );
            } else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
            ++i;
          }
    } else
    {
          /* pick all sockets (output) data */
          n = oyFilterNode_EdgeCount( (oyFilterNode_s*)node, 0, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->sockets[i])
          {
            data = 0;
            if(node->sockets[i]->data)
            {
              data = node->sockets[i]->data->copy( node->sockets[i]->data, 0 );
              if(oy_debug_objects >= 0 && data)
                oyObjectDebugMessage_( data->oy_, __func__,
                                       oyStructTypeToText(data->type_) );
            } else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
            ++i;
          }

    }
  }

  return datas;
}

/** Function  oyFilterNode_GetLastFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief    Get last node from a linear graph
 *  @internal
 *
 *  @param[in]     first               filter
 *  @return                            last node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/16 (Oyranos: 0.1.8)
 *  @date    2008/07/16
 */
oyFilterNode_s *   oyFilterNode_GetLastFromLinear_ (
                                       oyFilterNode_s_    * first )
{
  oyFilterNode_s * next = 0,
                 * last = 0;

      next = last = (oyFilterNode_s*)first;

      while(next)
      {
        next = oyFilterNode_GetNextFromLinear_( (oyFilterNode_s_*)next );

        if(next)
          last = next;
      }

  return last;
}

/** Function  oyFilterNode_GetNextFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief    Get next node from a linear graph
 *  @internal
 *
 *  @param[in]     first               filter
 *  @return                            next node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
oyFilterNode_s *   oyFilterNode_GetNextFromLinear_ (
                                       oyFilterNode_s_    * first )
{
  oyFilterNode_s_ * next = 0;
  oyFilterSocket_s_ * socket = 0;
  oyFilterPlug_s * plug = 0;

  {
    socket = first->sockets[0];

    if(socket)
      plug = oyFilterPlugs_Get( socket->requesting_plugs_, 0 );
    if(plug)
      next = ((oyFilterPlug_s_*)plug)->node;
    else
      next = 0;
    oyFilterPlug_Release( &plug );
  }

  return (oyFilterNode_s*)next;
}

/**
 *  @internal
 *  Info profilbody */
char info_profile_data[320] =
  {
/*0*/    0,0,1,64, 'o','y','r','a',
    2,48,0,0, 'n','o','n','e',
    'R','G','B',32, 'L','a','b',32,
    0,0,0,0,0,0,0,0,
/*32*/    0,0,0,0,97,99,115,112,
    '*','n','i','x',0,0,0,0,
    110,111,110,101,110,111,110,101,
    -64,48,11,8,-40,-41,-1,-65,
/*64*/    0,0,0,0,0,0,-10,-42,
    0,1,0,0,0,0,-45,45,
    'o','y','r','a',0,0,0,0,
    0,0,0,0,0,0,0,0,
/*96*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
/*128*/    0,0,0,3,'d','e','s','c',
    0,0,0,-88,0,0,0,33,
    'c','p','r','t',0,0,0,-52,
    0,0,0,29,'I','n','f','o',
/*160*/    0,0,0,-20,0,0,0,0,
    't','e','x','t',0,0,0,0,
    'F','i','l','t','e','r',' ','I',
    'n','f','o',' ','X','M','L',0,
/*192*/    0,0,0,0,0,0,0,0,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,110,111,116,32,
    99,111,112,121,114,105,103,104,
/*224*/    116,101,100,32,100,97,116,97,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,'s','t','a','r',
    't',0,0,0,0,0,0,0,
/*256*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
  };

/** Function  oyFilterNode_TextToInfo_
 *  @memberof oyFilterNode_s
 *  @brief    Serialise filter node to binary
 *  @internal
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" text tag.
 *  Not useable for binary contexts.
 *
 *  This function is currently a ICC only thing and yet just for debugging
 *  useful.
 *
 *  @param[in,out] node                filter node
 *  @param[out]    size                output size
 *  @param[in]     allocateFunc        memory allocator
 *  @return                            the profile container
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyPointer    oyFilterNode_TextToInfo_( oyFilterNode_s_    * node,
                                       size_t             * size,
                                       oyAlloc_f            allocateFunc )
{
  oyPointer ptr = 0;
  icHeader * header = 0;
  size_t len = 244, text_len = 0;
  char * text = 0;
  const char * temp = 0;
  uint32_t * mem = 0;

  if(!node)
    return 0;

  temp = oyFilterNode_GetText( (oyFilterNode_s*)node, oyNAME_NAME );

  text_len = strlen(temp) + 1;
  len += text_len + 1;
  len = len > 320 ? len : 320;
  ptr = allocateFunc(len);
  header = ptr;

  if(ptr)
  {
    *size = len;
    memset(ptr,0,len);
    memcpy(ptr, info_profile_data, 320);

    text = ((char*)ptr)+244;
    sprintf(text, "%s", temp);
    header->size = oyValueUInt32( len );
    mem = ptr;
    mem[41] = oyValueUInt32( text_len + 8 );
  }

  return ptr;
}

/**
 *  @internal
 *  Function oyFilterNode_Observe_
 *  @memberof oyFilterNode_s
 *  @brief   observe filter options
 *
 *  Implements oyObserver_Signal_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int      oyFilterNode_Observe_       ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data OY_UNUSED )
{
  int handled = 0;
  int i,n;
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s_ * node = 0;
  oyObserver_s_  * obs = (oyObserver_s_*)observer;

  if(obs && obs->model &&
     obs->model->type_ == oyOBJECT_OPTIONS_S &&
     obs->observer && obs->observer->type_== oyOBJECT_FILTER_NODE_S)
  {
    if(oy_debug_signals)
      WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    node = (oyFilterNode_s_*)obs->observer;

    /* invalidate the context */
    if(node->backend_data)
      node->backend_data->release( (oyStruct_s**)&node->backend_data );

    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)node, 0, 0 );
    for(i = 0; i < n; ++i)
    {
      socket = oyFilterNode_GetSocket( (oyFilterNode_s*)node, i );
      /* forward to the downward graph */
      oyFilterSocket_SignalToGraph( socket, (oyCONNECTOR_EVENT_e)signal_type );
      oyFilterSocket_Release( &socket );
    }
  }

  return handled;
}

/* } Include "FilterNode.private_methods_definitions.c" */

