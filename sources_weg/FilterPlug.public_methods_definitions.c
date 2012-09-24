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
  oyFilterPlug_s_ ** c_ = (oyFilterPlug_s_**)&c;

  if(oy_debug_signals)
    WARNc4_S("oyFilterNode_s[%d]->oyFilterPlug_s[%d]\n"
             "  event: \"%s\" socket[%d]",
            (c && (*c_)->node) ? oyObject_GetId((*c_)->node->oy_) : -1,
            c ? oyObject_GetId(c->oy_) : -1,
            oyConnectorEventToText(e),
            (c && (*c_)->remote_socket_) ?
                                   oyObject_GetId( (*c_)->remote_socket_->oy_ ) : -1
          );

  if(c && e == oyCONNECTOR_EVENT_RELEASED)
    (*c_)->remote_socket_ = 0;

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
                                       oyFilterPlug_s   ** p,
                                       oyFilterSocket_s ** s )
{
  oyFilterPlug_s_ * tp = 0;
  oyFilterSocket_s_ * ts = 0;

  if(!p || !*p || !s || !*s)
    return 1;

  tp = (oyFilterPlug_s_*)*p;
  ts = (oyFilterSocket_s_*)*s;

  if(tp->remote_socket_)
    oyFilterSocket_Callback( (oyFilterPlug_s*)tp, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( (oyFilterSocket_s**)&tp->remote_socket_ );

# if DEBUG_OBJECT
      WARNc6_S("%s Id: %d -> %s Id: %d\n  %s -> %s",
             oyStructTypeToText( (*p)->type_ ), oyObject_GetId((*p)->oy_),
             oyStructTypeToText( (*s)->type_ ), oyObject_GetId((*s)->oy_),
             (*(oyFilterPlug_s_**)p)->node->relatives_,
             (*(oyFilterSocket_s_**)s)->node->relatives_ )
#endif

  tp->remote_socket_ = (oyFilterSocket_s_*)*s; *s = 0;

  if (!ts->requesting_plugs_)
    ts->requesting_plugs_ = oyFilterPlugs_New(ts->oy_);

  return !oyFilterPlugs_MoveIn( ts->requesting_plugs_, p, -1 );
}

/** Function  oyFilterPlug_ResolveImage
 *  @memberof oyFilterPlug_s
 *  @brief    Resolve processing data during a filter run
 *
 *  The function is a convenience function to use inside a filters
 *  oyCMMFilterPlug_Run_f call. The function makes only sense for non root
 *  filters.
 *
 *  @param[in,out] plug                the filters own plug
 *  @param[in,out] socket              the filters own socket
 *  @param[in,out] ticket              the actual ticket
 *  @return                            the input image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/01 (Oyranos: 0.1.10)
 *  @date    2009/05/01
 */
OYAPI oyImage_s * OYEXPORT
             oyFilterPlug_ResolveImage(oyFilterPlug_s    * plug,
                                       oyFilterSocket_s  * socket,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterPlug_s_ ** plug_ = (oyFilterPlug_s_**)&plug;
  oyFilterSocket_s_ ** socket_ = (oyFilterSocket_s_**)&socket;
  oyPixelAccess_s_ ** ticket_ = (oyPixelAccess_s_**)&ticket;

  int error = !plug || !(*plug_)->remote_socket_ ||
              !ticket ||
              !socket || !(*socket_)->node;
  oyImage_s_ * image_input_ = 0;
  oyImage_s  * image = 0;
  oyFilterNode_s_ * input_node_ = 0;
  oyFilterNode_s_ * node = (*socket_)->node;
  oyPixel_t pixel_layout = 0;
  oyOptions_s * options = 0,
              * requests = 0,
              * ticket_orig;
  int32_t n = 0;

  if(error)
    return 0;

  image_input_ = (oyImage_s_*)oyImage_Copy( (oyImage_s*)(*plug_)->remote_socket_->data, 0 );
  input_node_ = (oyFilterNode_s_*)(*plug_)->remote_socket_->node;

  if(!image_input_)
  {
    /* get options */
    options = oyFilterNode_GetOptions( (oyFilterNode_s*)node, 0 );

    /* store original queue */
    ticket_orig = (*ticket_)->request_queue;
    (*ticket_)->request_queue = 0;

    /* select only resolve requests */
    error = oyOptions_Filter( &requests, &n, 0,
                              oyBOOLEAN_INTERSECTION,
                              "////resolve", options );
    oyOptions_Release( &options );

    /* combine old queue and requests from the current node */
    oyOptions_CopyFrom( &(*ticket_)->request_queue, requests, oyBOOLEAN_UNION, 0,0);
    oyOptions_CopyFrom( &(*ticket_)->request_queue, ticket_orig, oyBOOLEAN_UNION,
                        0, 0 );

    /* filter again, (really needed?) */
    oyOptions_Filter( &(*ticket_)->request_queue, &n, 0,
                      oyBOOLEAN_INTERSECTION, "////resolve", requests );
    oyOptions_Release( &requests );

    /* try to obtain the processing data from a generator filter */
    input_node_->api7_->oyCMMFilterPlug_Run( (oyFilterPlug_s*)(node->plugs[0]), ticket );
    image_input_ = (oyImage_s_*)oyImage_Copy( (oyImage_s*)(*plug_)->remote_socket_->data, 0 );

    /* clean up the queue */
    oyOptions_Release( &(*ticket_)->request_queue );

    /* restore old queue */
    (*ticket_)->request_queue = ticket_orig; ticket_orig = 0;

    if(!image_input_)
      return 0;
  }

  if(!(*socket_)->data)
  {
    /* Copy a root image or link to a non root image. */
    if(!(*plug_)->remote_socket_->node->api7_->plugs_n)
    {
      options = oyFilterNode_GetOptions( (oyFilterNode_s*)node, 0 );
      error = oyOptions_Filter( &requests, &n, 0,
                                oyBOOLEAN_INTERSECTION,
                                "////resolve", options );
      oyOptions_Release( &options );
      oyOptions_CopyFrom( &requests, (*ticket_)->request_queue,oyBOOLEAN_UNION,0,0);

      error = oyOptions_FindInt( requests, "pixel_layout", 0,
                                 (int32_t*)&pixel_layout );
      oyOptions_Release( &requests );

      if(error == 0)
      {
        /* possibly complete the pixel layout information */
        int n = oyToChannels_m( pixel_layout );
        int cchan_n = oyProfile_GetChannelsCount( image_input_->profile_ );
        oyPixel_t layout = oyDataType_m( oyToDataType_m(pixel_layout) ) |
                           oyChannels_m( OY_MAX(n, cchan_n) );
        /* create a new image */
        image = oyImage_Create( image_input_->width, image_input_->height,
                                0, layout,
                                image_input_->profile_, node->oy_ );

      } else
        image = oyImage_Copy( (oyImage_s*) image_input_, node->oy_ );


    } else
      image = oyImage_Copy( (oyImage_s*) image_input_, 0 );

    error = oyFilterNode_SetData( (oyFilterNode_s*)node, (oyStruct_s*)image, 0, 0 );
    oyImage_Release( &image );
  }

  if(!(*ticket_)->output_image)
    (*ticket_)->output_image = oyImage_Copy( (oyImage_s*) (*socket_)->data, 0 );

  oyOptions_Release( &requests );

  return (oyImage_s*)image_input_;
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

  return oyFilterNode_Copy( (oyFilterNode_s*)(s->node), 0 );
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

  return oyFilterSocket_Copy( (oyFilterSocket_s*)(s->remote_socket_), 0 );
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

  return oyConnector_Copy( s->pattern, 0 );
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
