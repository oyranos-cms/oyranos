/** Function oyFilterPlug_Callback
 *  @memberof oyFilterPlug_s
 *  @brief   tell about a oyConversion_s event
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
  if(oy_debug_signals)
    WARNc4_S("oyFilterNode_s[%d]->oyFilterPlug_s[%d]\n"
             "  event: \"%s\" socket[%d]",
            (c && c->node) ? oyObject_GetId(c->node->oy_) : -1,
            c ? oyObject_GetId(c->oy_) : -1,
            oyConnectorEventToText(e),
            (c && c->remote_socket_) ?
                                   oyObject_GetId( c->remote_socket_->oy_ ) : -1
          );

  if(c && e == oyCONNECTOR_EVENT_RELEASED)
    c->remote_socket_ = 0;

  return 0;
}

/** Function oyFilterPlug_ConnectIntoSocket
 *  @memberof oyFilterPlug_s
 *  @brief   connect a oyFilterPlug_s with a oyFilterSocket_s
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
  oyFilterPlug_s * tp = 0;
  oyFilterSocket_s * ts = 0;

  if(!p || !*p || !s || !*s)
    return 1;

  tp = *p;
  ts = *s;

  if(tp->remote_socket_)
    oyFilterSocket_Callback( tp, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( &tp->remote_socket_ );

# if DEBUG_OBJECT
      WARNc6_S("%s Id: %d -> %s Id: %d\n  %s -> %s",
             oyStructTypeToText( (*p)->type_ ), oyObject_GetId((*p)->oy_),
             oyStructTypeToText( (*s)->type_ ), oyObject_GetId((*s)->oy_),
             (*p)->node->relatives_,
             (*s)->node->relatives_ )
#endif

  tp->remote_socket_ = *s; *s = 0;
  return !(ts->requesting_plugs_ =
                          oyFilterPlugs_MoveIn( ts->requesting_plugs_, p, -1 ));
}

/** Function oyFilterPlug_ResolveImage
 *  @memberof oyFilterPlug_s
 *  @brief   resolve processing data during a filter run
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
  int error = !plug || !plug->remote_socket_ ||
              !ticket ||
              !socket || !socket->node;
  oyImage_s * image_input = 0, * image = 0;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node;
  oyPixel_t pixel_layout = 0;
  oyOptions_s * options = 0,
              * requests = 0,
              * ticket_orig;
  int32_t n = 0;

  if(error)
    return 0;

  image_input = oyImage_Copy( (oyImage_s*)plug->remote_socket_->data, 0 );
  input_node = plug->remote_socket_->node;


  if(!image_input)
  {
    /* get options */
    options = oyFilterNode_OptionsGet( node, 0 );

    /* store original queue */
    ticket_orig = ticket->request_queue;
    ticket->request_queue = 0;

    /* select only resolve requests */
    error = oyOptions_Filter( &requests, &n, 0,
                              oyBOOLEAN_INTERSECTION,
                              "////resolve", options );
    oyOptions_Release( &options );

    /* combine old queue and requests from the current node */
    oyOptions_CopyFrom( &ticket->request_queue, requests, oyBOOLEAN_UNION, 0,0);
    oyOptions_CopyFrom( &ticket->request_queue, ticket_orig, oyBOOLEAN_UNION,
                        0, 0 );

    /* filter again, (really needed?) */
    oyOptions_Filter( &ticket->request_queue, &n, 0,
                      oyBOOLEAN_INTERSECTION, "////resolve", requests );
    oyOptions_Release( &requests );

    /* try to obtain the processing data from a generator filter */
    input_node->api7_->oyCMMFilterPlug_Run( node->plugs[0], ticket );
    image_input = oyImage_Copy( (oyImage_s*)plug->remote_socket_->data, 0 );

    /* clean up the queue */
    oyOptions_Release( &ticket->request_queue );

    /* restore old queue */
    ticket->request_queue = ticket_orig; ticket_orig = 0;

    if(!image_input)
      return 0;
  }

  if(!socket->data)
  {
    /* Copy a root image or link to a non root image. */
    if(!plug->remote_socket_->node->api7_->plugs_n)
    {
      options = oyFilterNode_OptionsGet( node, 0 );
      error = oyOptions_Filter( &requests, &n, 0,
                                oyBOOLEAN_INTERSECTION,
                                "////resolve", options );
      oyOptions_Release( &options );
      oyOptions_CopyFrom( &requests, ticket->request_queue,oyBOOLEAN_UNION,0,0);

      error = oyOptions_FindInt( requests, "pixel_layout", 0,
                                 (int32_t*)&pixel_layout );
      oyOptions_Release( &requests );

      if(error == 0)
      {
        /* possibly complete the pixel layout information */
        int n = oyToChannels_m( pixel_layout );
        int cchan_n = oyProfile_GetChannelsCount( image_input->profile_ );
        oyPixel_t layout = oyDataType_m( oyToDataType_m(pixel_layout) ) |
                           oyChannels_m( OY_MAX(n, cchan_n) );
        /* create a new image */
        image = oyImage_Create( image_input->width, image_input->height,
                                0, layout,
                                image_input->profile_, node->oy_ );

      } else
        image = oyImage_Copy( (oyImage_s*) image_input, node->oy_ );


    } else
      image = oyImage_Copy( (oyImage_s*) image_input, 0 );

    error = oyFilterNode_DataSet( node, (oyStruct_s*)image, 0, 0 );
    oyImage_Release( &image );
  }

  if(!ticket->output_image)
    ticket->output_image = oyImage_Copy( (oyImage_s*) socket->data, 0 );

  oyOptions_Release( &requests );

  return image_input;
}
