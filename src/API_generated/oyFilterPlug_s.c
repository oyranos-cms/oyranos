/** @file oyFilterPlug_s.c

   [Template file inheritance graph]
   +-> oyFilterPlug_s.template.c
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


  
#include "oyFilterPlug_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyFilterPlug_s_.h"

#include "oyFilterPlugs_s.h"
#include "oyImage_s_.h"
#include "oyPixelAccess_s_.h"
  


/** Function oyFilterPlug_New
 *  @memberof oyFilterPlug_s
 *  @brief   allocate a new FilterPlug object
 */
OYAPI oyFilterPlug_s * OYEXPORT
  oyFilterPlug_New( oyObject_s object )
{
  oyObject_s s = object;
  oyFilterPlug_s_ * filterplug = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  filterplug = oyFilterPlug_New_( s );

  return (oyFilterPlug_s*) filterplug;
}

/** Function  oyFilterPlug_Copy
 *  @memberof oyFilterPlug_s
 *  @brief    Copy or Reference a FilterPlug object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     filterplug                 FilterPlug struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyFilterPlug_s* OYEXPORT
  oyFilterPlug_Copy( oyFilterPlug_s *filterplug, oyObject_s object )
{
  oyFilterPlug_s_ * s = (oyFilterPlug_s_*) filterplug;

  if(s)
  {
    oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return NULL )
  }
  else
    return NULL;

  s = oyFilterPlug_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyFilterPlug_s" );

  return (oyFilterPlug_s*) s;
}
 
/** Function oyFilterPlug_Release
 *  @memberof oyFilterPlug_s
 *  @brief   release and possibly deallocate a oyFilterPlug_s object
 *
 *  @param[in,out] filterplug                 FilterPlug struct object
 */
OYAPI int OYEXPORT
  oyFilterPlug_Release( oyFilterPlug_s **filterplug )
{
  oyFilterPlug_s_ * s = 0;

  if(!filterplug || !*filterplug)
    return 0;

  s = (oyFilterPlug_s_*) *filterplug;

  oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return 1 )

  *filterplug = 0;

  return oyFilterPlug_Release_( &s );
}



/* Include "FilterPlug.public_methods_definitions.c" { */
/** Function  oyFilterPlug_Callback
 *  @memberof oyFilterPlug_s
 *  @brief    Tell about a oyConversion_s event
 *
 *  @param[in,out] c                   the connector
 *  @param         e                   the event type
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2008/07/28
 */
OYAPI int  OYEXPORT
                 oyFilterPlug_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e )
{
  oyFilterPlug_s_ * c_ = (oyFilterPlug_s_*)c;

  if(oy_debug_signals)
    WARNc4_S("oyFilterNode_s[%d]->oyFilterPlug_s[%d]\n"
             "  event: \"%s\" socket[%d]",
            (c && c_->node) ? oyObject_GetId(c_->node->oy_) : -1,
            c ? oyObject_GetId(c->oy_) : -1,
            oyConnectorEventToText(e),
            (c && c_->remote_socket_) ?
                                   oyObject_GetId( c_->remote_socket_->oy_ ) : -1
          );

  if(c && e == oyCONNECTOR_EVENT_RELEASED)
  {
    oyFilterPlug_s * s = c;
    oyFilterNode_s * n = (oyFilterNode_s *)c_->node;
    if(c_->node != NULL)
    {
      c_->node = NULL;
      oyFilterPlug_Release( &s );
    }
    oyFilterNode_Release( &n );

    s = c;
    if(c_->remote_socket_)
      oyFilterPlug_Release( &s );
    c_->remote_socket_ = NULL;
  }

  return 0;
}

/** Function  oyFilterPlug_ConnectIntoSocket
 *  @memberof oyFilterPlug_s
 *  @brief    Connect a oyFilterPlug_s with a oyFilterSocket_s
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/31
 */
OYAPI int  OYEXPORT
                 oyFilterPlug_ConnectIntoSocket (
                                       oyFilterPlug_s    * p,
                                       oyFilterSocket_s  * s )
{
  oyFilterPlug_s_ * tp = 0;
  oyFilterSocket_s_ * ts = 0;
  int error = 0;

  if(!p || !s)
    return 1;

  tp = (oyFilterPlug_s_*)p;
  ts = (oyFilterSocket_s_*)s;

  if(tp->remote_socket_)
    oyFilterSocket_Callback( (oyFilterPlug_s*)tp, oyCONNECTOR_EVENT_RELEASED );

# if DEBUG_OBJECT
      WARNc6_S("%s Id: %d -> %s Id: %d\n  %s -> %s",
             oyStructTypeToText( p->type_ ), oyObject_GetId(p->oy_),
             oyStructTypeToText( s->type_ ), oyObject_GetId(s->oy_),
             ((oyFilterPlug_s_*)p)->node->relatives_,
             ((oyFilterSocket_s_*)s)->node->relatives_ )
#endif

  tp->remote_socket_ = (oyFilterSocket_s_*)s;
  oyFilterSocket_Copy( s, NULL);

  if (!ts->requesting_plugs_)
    ts->requesting_plugs_ = oyFilterPlugs_New(ts->oy_);

  error = oyFilterPlugs_MoveIn( ts->requesting_plugs_, (oyFilterPlug_s**)&tp, -1 );
  oyFilterPlug_Copy( p, NULL);

  return error;
}

/** Function  oyFilterPlug_GetNode
 *  @memberof oyFilterPlug_s
 *  @brief    Access oyFilterPlug_s::node
 *
 *  @param[in]     plug                the plug
 *  @return                            the node
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterPlug_GetNode    ( oyFilterPlug_s    * plug )
{
  oyFilterPlug_s_ * s = (oyFilterPlug_s_*)plug;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return 0 )

  oyFilterNode_Copy( (oyFilterNode_s*)(s->node), 0 );
  return (oyFilterNode_s*)(s->node);
}
/** Function  oyFilterPlug_GetRemoteNode
 *  @memberof oyFilterPlug_s
 *  @brief    Access oyFilterPlug_s::remote_socket::node
 *
 *  @param[in]     plug                the plug
 *  @return                            the remote node
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/24
 *  @since    2012/09/24 (Oyranos: 0.5.0)
 */
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterPlug_GetRemoteNode
                                     ( oyFilterPlug_s    * plug )
{
  oyFilterPlug_s_ * s = (oyFilterPlug_s_*)plug;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return 0 )

  return oyFilterSocket_GetNode( (oyFilterSocket_s*)s->remote_socket_ ); 
}
/** Function  oyFilterPlug_GetSocket
 *  @memberof oyFilterPlug_s
 *  @brief    Access oyFilterPlug_s::remote_socket_
 *
 *  @param[in]     plug                the plug
 *  @return                            the remote socket
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyFilterSocket_s * OYEXPORT
             oyFilterPlug_GetSocket  ( oyFilterPlug_s    * plug )
{
  oyFilterPlug_s_ * s = (oyFilterPlug_s_*)plug;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return 0 )

  oyFilterSocket_Copy( (oyFilterSocket_s*)(s->remote_socket_), 0 );
  return (oyFilterSocket_s*)(s->remote_socket_);
}
/** Function  oyFilterPlug_GetPattern
 *  @memberof oyFilterPlug_s
 *  @brief    Access oyFilterPlug_s::pattern
 *
 *  @param[in]     plug                the plug
 *  @return                            capabilities pattern for this filter plug
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI oyConnector_s * OYEXPORT
             oyFilterPlug_GetPattern ( oyFilterPlug_s    * plug )
{
  oyFilterPlug_s_ * s = (oyFilterPlug_s_*)plug;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return 0 )

  oyConnector_Copy( s->pattern, 0 );
  return s->pattern;
}
/** Function  oyFilterPlug_GetRelatives
 *  @memberof oyFilterPlug_s
 *  @brief    get short description of belonging filter
 *
 *  @param[in]     plug                the plug
 *  @return                            short text
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI const char * OYEXPORT
             oyFilterPlug_GetRelatives(oyFilterPlug_s    * plug )
{
  oyFilterPlug_s_ * s = (oyFilterPlug_s_*)plug;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return 0 )

  return s->relatives_;
}

/* } Include "FilterPlug.public_methods_definitions.c" */

