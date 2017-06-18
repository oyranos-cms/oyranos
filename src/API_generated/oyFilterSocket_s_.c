/** @file oyFilterSocket_s_.c

   [Template file inheritance graph]
   +-> oyFilterSocket_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2017 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyFilterSocket_s.h"
#include "oyFilterSocket_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "FilterSocket.private_custom_definitions.c" { */
/** Function    oyFilterSocket_Release__Members
 *  @memberof   oyFilterSocket_s
 *  @brief      Custom FilterSocket destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filtersocket  the FilterSocket object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterSocket_Release__Members( oyFilterSocket_s_ * filtersocket )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filtersocket->member );
   */
  oyObject_Ref(filtersocket->oy_);

  oyFilterNode_Release( (oyFilterNode_s**)&filtersocket->node );

  {
    int count = 0,
        i;
    oyFilterPlug_s * c = 0;

    count = oyFilterPlugs_Count( filtersocket->requesting_plugs_ );
    for(i = 0; i < count; ++i)
    {
      c = oyFilterPlugs_Get( filtersocket->requesting_plugs_, i );
      oyFilterPlug_Callback( c, oyCONNECTOR_EVENT_RELEASED );
      oyFilterPlug_Release( &c );
    }
  }

  oyObject_UnRef(filtersocket->oy_);
  oyConnector_Release( &filtersocket->pattern );

  if(filtersocket->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filtersocket->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filtersocket->member );
     */
    if(filtersocket->relatives_)
      deallocateFunc( filtersocket->relatives_ );
    filtersocket->relatives_ = 0;
  }
}

/** Function    oyFilterSocket_Init__Members
 *  @memberof   oyFilterSocket_s
 *  @brief      Custom FilterSocket constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filtersocket  the FilterSocket object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterSocket_Init__Members( oyFilterSocket_s_ * filtersocket )
{
  return 0;
}

/** Function    oyFilterSocket_Copy__Members
 *  @memberof   oyFilterSocket_s
 *  @brief      Custom FilterSocket copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterSocket_s_ input object
 *  @param[out]  dst  the output oyFilterSocket_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterSocket_Copy__Members( oyFilterSocket_s_ * dst, oyFilterSocket_s_ * src)
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
  dst->pattern = oyConnector_Copy( src->pattern, dst->oy_ );
  dst->node = (oyFilterNode_s_*)oyFilterNode_Copy( (oyFilterNode_s*)src->node, 0 );
  if(src->data && src->data->copy)
  {
    dst->data = src->data->copy( src->data, dst->oy_ );
    if(oy_debug_objects >= 0 && dst->data)
      oyObjectDebugMessage_( dst->data->oy_, __func__,
                             oyStructTypeToText(dst->data->type_) );
  }

  return error;
}

/* } Include "FilterSocket.private_custom_definitions.c" */



static int oy_filtersocket_init_ = 0;
static const char * oyFilterSocket_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*) obj;
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
    text_n = 128;
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
/** @internal
 *  Function oyFilterSocket_New_
 *  @memberof oyFilterSocket_s_
 *  @brief   allocate a new oyFilterSocket_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterSocket_s_ * oyFilterSocket_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_SOCKET_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyFilterSocket_s_ * s = 0;

  if(s_obj)
    s = (oyFilterSocket_s_*)s_obj->allocateFunc_(sizeof(oyFilterSocket_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyFilterSocket_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterSocket_Copy;
  s->release = (oyStruct_Release_f) oyFilterSocket_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterSocket constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_SOCKET_S, (oyPointer)s );
  /* ---- end of custom FilterSocket constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom FilterSocket constructor ----- */
  error += oyFilterSocket_Init__Members( s );
  /* ---- end of custom FilterSocket constructor ------- */
  
  
  
  

  if(!oy_filtersocket_init_)
  {
    oy_filtersocket_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyFilterSocket_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyFilterSocket_Copy__
 *  @memberof oyFilterSocket_s_
 *  @brief   real copy a FilterSocket object
 *
 *  @param[in]     filtersocket                 FilterSocket struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterSocket_s_ * oyFilterSocket_Copy__ ( oyFilterSocket_s_ *filtersocket, oyObject_s object )
{
  oyFilterSocket_s_ *s = 0;
  int error = 0;

  if(!filtersocket || !object)
    return s;

  s = (oyFilterSocket_s_*) oyFilterSocket_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterSocket copy constructor ----- */
    error = oyFilterSocket_Copy__Members( s, filtersocket );
    /* ---- end of custom FilterSocket copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyFilterSocket_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterSocket_Copy_
 *  @memberof oyFilterSocket_s_
 *  @brief   copy or reference a FilterSocket object
 *
 *  @param[in]     filtersocket                 FilterSocket struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterSocket_s_ * oyFilterSocket_Copy_ ( oyFilterSocket_s_ *filtersocket, oyObject_s object )
{
  oyFilterSocket_s_ *s = filtersocket;

  if(!filtersocket)
    return 0;

  if(filtersocket && !object)
  {
    s = filtersocket;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterSocket_Copy__( filtersocket, object );

  return s;
}
 
/** @internal
 *  Function oyFilterSocket_Release_
 *  @memberof oyFilterSocket_s_
 *  @brief   release and possibly deallocate a FilterSocket object
 *
 *  @param[in,out] filtersocket                 FilterSocket struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyFilterSocket_Release_( oyFilterSocket_s_ **filtersocket )
{
  /* ---- start of common object destructor ----- */
  oyFilterSocket_s_ *s = 0;

  if(!filtersocket || !*filtersocket)
    return 0;

  s = *filtersocket;

  *filtersocket = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom FilterSocket destructor ----- */
  oyFilterSocket_Release__Members( s );
  /* ---- end of custom FilterSocket destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "FilterSocket.private_methods_definitions.c" { */

/* } Include "FilterSocket.private_methods_definitions.c" */

