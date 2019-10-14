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
