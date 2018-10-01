/** @file oyConnectorImaging_s_.c

   [Template file inheritance graph]
   +-> oyConnectorImaging_s_.template.c
   |
   +-> oyConnector_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2018 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyConnectorImaging_s.h"
#include "oyConnectorImaging_s_.h"


#include "oyConnector_s_.h"




#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


static int oy_connectorimaging_init_ = 0;
static const char * oyConnectorImaging_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*) obj;
  static char * text = 0;
  static int text_n = 0;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if(s->oy_ && s->oy_->allocateFunc_)
    alloc = s->oy_->allocateFunc_;

  if( text == NULL || text_n == 0 )
  {
    text_n = 512;
    text = (char*) alloc( text_n );
    if(text)
      memset( text, 0, text_n );
  }

  if( text == NULL || text_n == 0 )
    return "Memory problem";

  text[0] = '\000';

  if(!(flags & 0x01))
    sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return text;
}


/* Include "ConnectorImaging.private_custom_definitions.c" { */
/** Function    oyConnectorImaging_Release__Members
 *  @memberof   oyConnectorImaging_s
 *  @brief      Custom ConnectorImaging destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  connectorimaging  the ConnectorImaging object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConnectorImaging_Release__Members( oyConnectorImaging_s_ * connectorimaging )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &connectorimaging->member );
   */

  if(connectorimaging->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = connectorimaging->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( connectorimaging->member );
     */
    if(connectorimaging->data_types)
      deallocateFunc( connectorimaging->data_types );
    connectorimaging->data_types = 0;
    connectorimaging->data_types_n = 0;

    if(connectorimaging->channel_types)
      deallocateFunc( connectorimaging->channel_types );
    connectorimaging->channel_types = 0;
    connectorimaging->channel_types_n = 0;
  }
}

/** Function    oyConnectorImaging_Init__Members
 *  @memberof   oyConnectorImaging_s
 *  @brief      Custom ConnectorImaging constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  connectorimaging  the ConnectorImaging object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnectorImaging_Init__Members( oyConnectorImaging_s_ * connectorimaging )
{
  connectorimaging->max_color_offset = -1;
  connectorimaging->min_channels_count = -1;
  connectorimaging->max_channels_count = -1;
  connectorimaging->min_color_count = -1;
  connectorimaging->max_color_count = -1;
  connectorimaging->can_planar = -1;
  connectorimaging->can_interwoven = -1;
  connectorimaging->can_swap = -1;
  connectorimaging->can_swap_bytes = -1;
  connectorimaging->can_revert = -1;
  connectorimaging->can_premultiplied_alpha = -1;
  connectorimaging->can_nonpremultiplied_alpha = -1;
  connectorimaging->can_subpixel = -1;

  return 0;
}

/** Function    oyConnectorImaging_Copy__Members
 *  @memberof   oyConnectorImaging_s
 *  @brief      Custom ConnectorImaging copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConnectorImaging_s_ input object
 *  @param[out]  dst  the output oyConnectorImaging_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnectorImaging_Copy__Members( oyConnectorImaging_s_ * dst, oyConnectorImaging_s_ * src)
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
  if(src->data_types_n)
  {
    dst->data_types = allocateFunc_( src->data_types_n * sizeof(oyDATATYPE_e) );
    error = !dst->data_types;
    if(!error)
      error = !memcpy( dst->data_types, src->data_types,
                       src->data_types_n * sizeof(oyDATATYPE_e) );
    if(error <= 0)
      dst->data_types_n = src->data_types_n;
  }

  dst->max_color_offset = src->max_color_offset;
  dst->min_channels_count = src->min_channels_count;
  dst->max_channels_count = src->max_channels_count;
  dst->min_color_count = src->min_color_count;
  dst->max_color_count = src->max_color_count;
  dst->can_planar = src->can_planar;
  dst->can_interwoven = src->can_interwoven;
  dst->can_swap = src->can_swap;
  dst->can_swap_bytes = src->can_swap_bytes;
  dst->can_revert = src->can_revert;
  dst->can_premultiplied_alpha = src->can_premultiplied_alpha;
  dst->can_nonpremultiplied_alpha = src->can_nonpremultiplied_alpha;
  dst->can_subpixel = src->can_subpixel;

  if(src->channel_types_n)
  {
    int n = src->channel_types_n;

    dst->channel_types = allocateFunc_( n * sizeof(oyCHANNELTYPE_e) );
    error = !dst->channel_types;
    if(!error)
      error = !memcpy( dst->channel_types, src->channel_types,
                       n * sizeof(oyCHANNELTYPE_e) );
    if(error <= 0)
      dst->channel_types_n = n;
  }

  dst->is_mandatory = src->is_mandatory;

  return error;
}

/* } Include "ConnectorImaging.private_custom_definitions.c" */


/** @internal
 *  Function oyConnectorImaging_New_
 *  @memberof oyConnectorImaging_s_
 *  @brief   allocate a new oyConnectorImaging_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnectorImaging_s_ * oyConnectorImaging_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONNECTOR_IMAGING_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyConnectorImaging_s_ * s = 0;

  if(s_obj)
    s = (oyConnectorImaging_s_*)s_obj->allocateFunc_(sizeof(oyConnectorImaging_s_));
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

  error = !memset( s, 0, sizeof(oyConnectorImaging_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConnectorImaging_Copy;
  s->release = (oyStruct_Release_f) oyConnectorImaging_Release;

  s->oy_ = s_obj;

  
  
  /* ---- start of custom Connector constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONNECTOR_S, (oyPointer)s );
  /* ---- end of custom Connector constructor ------- */
  /* ---- start of custom ConnectorImaging constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONNECTOR_IMAGING_S, (oyPointer)s );
  /* ---- end of custom ConnectorImaging constructor ------- */
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  /* ---- start of custom Connector constructor ----- */
  error += oyConnector_Init__Members( (oyConnector_s_*)s );
  /* ---- end of custom Connector constructor ------- */
  /* ---- start of custom ConnectorImaging constructor ----- */
  error += oyConnectorImaging_Init__Members( s );
  /* ---- end of custom ConnectorImaging constructor ------- */
  
  
  

  if(!oy_connectorimaging_init_)
  {
    oy_connectorimaging_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyConnectorImaging_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyConnectorImaging_Copy__
 *  @memberof oyConnectorImaging_s_
 *  @brief   real copy a ConnectorImaging object
 *
 *  @param[in]     connectorimaging                 ConnectorImaging struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnectorImaging_s_ * oyConnectorImaging_Copy__ ( oyConnectorImaging_s_ *connectorimaging, oyObject_s object )
{
  oyConnectorImaging_s_ *s = 0;
  int error = 0;

  if(!connectorimaging || !object)
    return s;

  s = (oyConnectorImaging_s_*) oyConnectorImaging_New( object );
  error = !s;

  if(!error) {
    
    
    /* ---- start of custom Connector copy constructor ----- */
    error = oyConnector_Copy__Members( (oyConnector_s_*)s, (oyConnector_s_*)connectorimaging );
    /* ---- end of custom Connector copy constructor ------- */
    /* ---- start of custom ConnectorImaging copy constructor ----- */
    error = oyConnectorImaging_Copy__Members( s, connectorimaging );
    /* ---- end of custom ConnectorImaging copy constructor ------- */
    
    
    
    
    
  }

  if(error)
    oyConnectorImaging_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConnectorImaging_Copy_
 *  @memberof oyConnectorImaging_s_
 *  @brief   copy or reference a ConnectorImaging object
 *
 *  @param[in]     connectorimaging                 ConnectorImaging struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnectorImaging_s_ * oyConnectorImaging_Copy_ ( oyConnectorImaging_s_ *connectorimaging, oyObject_s object )
{
  oyConnectorImaging_s_ *s = connectorimaging;

  if(!connectorimaging)
    return 0;

  if(connectorimaging && !object)
  {
    s = connectorimaging;
    
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
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
        if(n != s->oy_->ref_)
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
          fprintf( stderr, "%s[%d] tracking refs: %d parents: %d\n",
                   track_name, s->oy_->id_, s->oy_->ref_, n );
          for(i = 0; i < n; ++i)
          {
            track_name = oyStructTypeToText(parents[i]->type_);
            fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                     track_name, parents[i]->oy_->id_ );
          }
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConnectorImaging_Copy__( connectorimaging, object );

  return s;
}
 
/** @internal
 *  Function oyConnectorImaging_Release_
 *  @memberof oyConnectorImaging_s_
 *  @brief   release and possibly deallocate a ConnectorImaging object
 *
 *  @param[in,out] connectorimaging                 ConnectorImaging struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyConnectorImaging_Release_( oyConnectorImaging_s_ **connectorimaging )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyConnectorImaging_s_ *s = 0;

  if(!connectorimaging || !*connectorimaging)
    return 0;

  s = *connectorimaging;

  *connectorimaging = 0;

  observer_refs = oyStruct_ObservedModelCount( (oyStruct_s*)s );

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
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      if(n != s->oy_->ref_)
      {
        int i;
        track_name = oyStructTypeToText(s->type_);
        fprintf( stderr, "%s[%d] unref with refs: %d observers: %d parents: %d\n",
                 track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  
  if(oyObject_UnRef(s->oy_))
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
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      fprintf( stderr, "%s[%d] destruct\n", track_name, s->oy_->id_);
    }
  }

  
  
  /* ---- start of custom Connector destructor ----- */
  oyConnector_Release__Members( (oyConnector_s_*)s );
  /* ---- end of custom Connector destructor ------- */
  /* ---- start of custom ConnectorImaging destructor ----- */
  oyConnectorImaging_Release__Members( s );
  /* ---- end of custom ConnectorImaging destructor ------- */
  
  
  



  /* model and observer reference each other. So release the object two times.
   * The models and and observers are released later inside the
   * oyObject_s::handles. */
  for(i = 0; i < observer_refs; ++i)
  {
    oyObject_UnRef(s->oy_);
    oyObject_UnRef(s->oy_);
  }

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int id = s->oy_->id_;
    int refs = s->oy_->ref_;

    if(refs > 1)
      fprintf( stderr, "!!!ERROR: node[%d]->object can not be untracked with refs: %d\n", id, refs);

    oyObject_Release( &s->oy_ );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "ConnectorImaging.private_methods_definitions.c" { */

/* } Include "ConnectorImaging.private_methods_definitions.c" */

