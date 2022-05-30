/** @file oyFilterSocket_s.c

   [Template file inheritance graph]
   +-> oyFilterSocket_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyFilterSocket_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyFilterSocket_s_.h"

#include "oyFilterPlug_s_.h"
  


/** Function oyFilterSocket_New
 *  @memberof oyFilterSocket_s
 *  @brief   allocate a new FilterSocket object
 */
OYAPI oyFilterSocket_s * OYEXPORT
  oyFilterSocket_New( oyObject_s object )
{
  oyObject_s s = object;
  oyFilterSocket_s_ * filtersocket = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  filtersocket = oyFilterSocket_New_( s );

  return (oyFilterSocket_s*) filtersocket;
}

/** Function  oyFilterSocket_Copy
 *  @memberof oyFilterSocket_s
 *  @brief    Copy or Reference a FilterSocket object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     filtersocket                 FilterSocket struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyFilterSocket_s* OYEXPORT
  oyFilterSocket_Copy( oyFilterSocket_s *filtersocket, oyObject_s object )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*) filtersocket;

  if(s)
  {
    oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return NULL )
  }
  else
    return NULL;

  s = oyFilterSocket_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyFilterSocket_s" );

  return (oyFilterSocket_s*) s;
}
 
/** Function oyFilterSocket_Release
 *  @memberof oyFilterSocket_s
 *  @brief   release and possibly deallocate a oyFilterSocket_s object
 *
 *  @param[in,out] filtersocket                 FilterSocket struct object
 */
OYAPI int OYEXPORT
  oyFilterSocket_Release( oyFilterSocket_s **filtersocket )
{
  oyFilterSocket_s_ * s = 0;

  if(!filtersocket || !*filtersocket)
    return 0;

  s = (oyFilterSocket_s_*) *filtersocket;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 1 )

  *filtersocket = 0;

  return oyFilterSocket_Release_( &s );
}



/* Include "FilterSocket.public_methods_definitions.c" { */
#include "oyranos_module.h"

/** Function  oyFilterSocket_Callback
 *  @memberof oyFilterSocket_s
 *  @brief    Tell about a oyConversion_s event
 *
 *  @param[in,out] c                   the connector
 *  @param         e                   the event type
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/09/28
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 */
OYAPI int  OYEXPORT
                 oyFilterSocket_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e )
{
  int n, i;
  oyFilterSocket_s_ * s_, *s;
  oyFilterPlug_s * p;

  oyFilterPlug_s_ * c_ = (oyFilterPlug_s_*)c;

  if( e != oyCONNECTOR_EVENT_OK &&
      ( oy_debug_signals ||
        ( oy_debug_objects > 0 &&
          ( oy_debug_objects == oyStruct_GetId( (oyStruct_s*)c_->node ) ||
            oy_debug_objects == oyStruct_GetId( (oyStruct_s*)c_->remote_socket_ ) ||
            oy_debug_objects == oyStruct_GetId( (oyStruct_s*)c_ )
          ) ) ) )
  {
    WARNc5_S("\n  oyFilterNode_s[%d]->oyFilterSocket_s[%d]\n"
             "  event: \"%s\" plug[%d/node%d]",
            (c && c_->remote_socket_ && c_->remote_socket_->node) ?
                   oyObject_GetId(c_->remote_socket_->node->oy_) : -1,
            (c && c_->remote_socket_) ? oyObject_GetId(c_->remote_socket_->oy_)
                                     : -1,
            oyConnectorEventToText(e),
            c ? oyObject_GetId( c->oy_ ) : -1,
            c ? (c_->node ? oyObject_GetId( c_->node->oy_ ) : -1) : -1
          );
  }

  if(!c)
    return 1;

  s = s_ = (oyFilterSocket_s_*) c_->remote_socket_;

  if(!s_)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 1 )

  n = oyFilterPlugs_Count( s_->requesting_plugs_ );

  if(e == oyCONNECTOR_EVENT_RELEASED)
    for(i = 0; i < n; ++i)
    {
      p = oyFilterPlugs_Get( s_->requesting_plugs_, i );
      if(p == c)
      {
        if(s_->node != NULL)
        {
          oyFilterNode_s * n = (oyFilterNode_s *)s_->node;
          s_->node = NULL;
          oyFilterNode_Release( &n );
        }
        oyFilterPlugs_ReleaseAt( s_->requesting_plugs_, i );
        oyFilterSocket_Release( (oyFilterSocket_s**)&c_->remote_socket_ );
        break;
      }
      oyFilterPlug_Release( &p );
    }

  return 0;
}

/** Function  oyFilterSocket_SignalToGraph
 *  @memberof oyFilterSocket_s
 *  @brief    Send a signal through the graph
 *
 *  The traversal direction is defined as from the starting node to the output.
 *
 *  @return                            1 if handled or zero
 *
 *  @version Oyranos: 0.3.2
 *  @since   2009/10/27 (Oyranos: 0.1.10)
 *  @date    2011/07/10
 */
OYAPI int  OYEXPORT
                 oyFilterSocket_SignalToGraph (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e )
{
  oySIGNAL_e sig = oySIGNAL_OK;
  int n, i, j_n,j, k,k_n, handled = 0, pos;
  oyFilterPlug_s * p, * sp;
  oyFilterGraph_s * graph = 0;

  oyFilterSocket_s_ ** c_ = (oyFilterSocket_s_**)&c;
  if(!c)
  {
    WARNc_S("socket argument is missing\n");
    return 0;
  }

  switch(e)
  {
    case oyCONNECTOR_EVENT_OK:                /**< kind of ping */
    case oyCONNECTOR_EVENT_CONNECTED:         /**< connection established */
    case oyCONNECTOR_EVENT_RELEASED:          /**< released the connection */
    case oyCONNECTOR_EVENT_DATA_CHANGED:      /**< call to update image views */
    case oyCONNECTOR_EVENT_STORAGE_CHANGED:   /**< new data accessors */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_DATA: /**< can not process image */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION:/**< can not handle option */
    case oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT:/**< can not handle profile */
    case oyCONNECTOR_EVENT_INCOMPLETE_GRAPH:  /**< can not completely process */
         sig = (oySIGNAL_e) e; break;
  }

  if(*c_ && (*c_)->node)
    handled = oyStruct_ObserverSignal( (oyStruct_s*) (*c_)->node, sig, 0 );
  if(handled)
    return handled;

  if(e != oyCONNECTOR_EVENT_OK && oy_debug_signals)
  {
    WARNc4_S("oyFilterNode_s[%d]->oyFilterSocket_s[%d]\n"
             "  event: \"%s\" socket[%d]",
            (c && (*c_)->node) ? oyObject_GetId((*c_)->node->oy_) : -1,
            c ? oyObject_GetId(c->oy_) : -1,
            oyConnectorEventToText(e),
            (c) ? oyObject_GetId( c->oy_ ) : -1
          );
  }

  n = oyFilterPlugs_Count( (*c_)->requesting_plugs_ );

  for(i = 0; i < n; ++i)
  {
    p = oyFilterPlugs_Get( (*c_)->requesting_plugs_, i );
    if(oyFilterPlugPriv_m(p)->node)
      handled = oyStruct_ObserverSignal( (oyStruct_s*) oyFilterPlugPriv_m(p)->node, sig, 0 );

    /* get all nodes in the output direction */
    graph = oyFilterGraph_FromNode( (oyFilterNode_s*)oyFilterPlugPriv_m(p)->node, OY_INPUT );

    j_n = oyFilterNodes_Count( oyFilterGraphPriv_m(graph)->nodes );
    for( j = 0; j < j_n; ++j )
    {
      oyFilterNode_s * node = oyFilterNodes_Get( oyFilterGraphPriv_m(graph)->nodes, j );

      /* iterate over all node outputs */
      k_n = oyFilterNode_EdgeCount( node, 1, OY_FILTEREDGE_CONNECTED );
      for(k = 0; k < k_n; ++k)
      {
        pos = oyFilterNode_GetConnectorPos( node, 1, "///", k,
                                            OY_FILTEREDGE_CONNECTED );
        sp = oyFilterNode_GetPlug( node, pos );
        oyFilterSocket_Callback( sp, e );
        oyFilterPlug_Release( &sp );
      }

      handled = oyStruct_ObserverSignal( (oyStruct_s*) node, sig, 0 );

      oyFilterNode_Release( &node );
      if(handled)
        break;
    }

    oyFilterGraph_Release( &graph );
    oyFilterPlug_Release( &p );
    if(handled)
      break;
  }

  return handled;
}

/** Function  oyFilterSocket_GetNode
 *  @memberof oyFilterSocket_s
 *  @brief    Access oyFilterSocket_s::node
 *
 *  @param[in]     socket              the socket
 *  @return                            the node
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterSocket_GetNode  ( oyFilterSocket_s  * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  oyFilterNode_Copy( (oyFilterNode_s*)s->node, 0 );
  return (oyFilterNode_s*)s->node;
}
/** Function  oyFilterSocket_GetData
 *  @memberof oyFilterSocket_s
 *  @brief    Access oyFilterSocket_s::data
 *
 *  @param[in]     socket              the socket
 *  @return                            the data
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/11
 *  @since    2012/09/23 (Oyranos: 0.5.0)
 */
OYAPI oyStruct_s * OYEXPORT
             oyFilterSocket_GetData  ( oyFilterSocket_s  * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  if(s->data && s->data->copy)
  {
    s->data = s->data->copy(s->data, 0);
    if(oy_debug_objects >= 0 && s->data)
      oyObjectDebugMessage_( s->data->oy_, __func__,
                             oyStructTypeToText(s->data->type_) );
  }

  return s->data;
}
/** Function  oyFilterSocket_SetData
 *  @memberof oyFilterSocket_s
 *  @brief    Set oyFilterSocket_s::data
 *
 *  @param[in]     socket              the socket
 *  @param[in]     data                the data
 *  @return                            error
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/25 (Oyranos: 0.5.0)
 *  @date    2012/09/25
 */
OYAPI int OYEXPORT
             oyFilterSocket_SetData  ( oyFilterSocket_s  * socket,
                                       oyStruct_s        * data )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 1 )

  if(s->data && s->data->release)
    s->data->release( &s->data );

  if(data && data->copy)
  {
    s->data = data->copy(data, 0);
    if(oy_debug_objects >= 0 && s->data)
      oyObjectDebugMessage_( s->data->oy_, __func__,
                             oyStructTypeToText(s->data->type_) );
  }
  else
    s->data = data;

  return 0;
}
/** Function  oyFilterSocket_GetPlugs
 *  @memberof oyFilterSocket_s
 *  @brief    Access oyFilterSocket_s::requesting_plugs_
 *
 *  @param[in]     socket              the socket
 *  @return                            the requesting plugs
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyFilterPlugs_s * OYEXPORT
             oyFilterSocket_GetPlugs( oyFilterSocket_s  * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  oyFilterPlugs_Copy( s->requesting_plugs_, 0 );
  return s->requesting_plugs_;
}
/** Function  oyFilterPlug_GetPattern
 *  @memberof oyFilterPlug_s
 *  @brief    Access oyFilterPlug_s::pattern
 *
 *  @param[in]     socket              the socket
 *  @return                            capabilities pattern for this filter socket
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyConnector_s * OYEXPORT
             oyFilterSocket_GetPattern(oyFilterSocket_s  * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  oyConnector_Copy( s->pattern, 0 );
  return s->pattern;
}
/** Function  oyFilterSocket_GetRelatives
 *  @memberof oyFilterSocket_s
 *  @brief    get short description of belonging filter
 *
 *  @param[in]     socket              the socket
 *  @return                            short text
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI const char * OYEXPORT
             oyFilterSocket_GetRelatives (
                                       oyFilterSocket_s    * socket )
{
  oyFilterSocket_s_ * s = (oyFilterSocket_s_*)socket;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_SOCKET_S, return 0 )

  return s->relatives_;
}


/* } Include "FilterSocket.public_methods_definitions.c" */

