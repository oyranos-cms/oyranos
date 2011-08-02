/** @internal
 *  @brief   convert between oyCMMptr_s data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int          oyCMMptr_ConvertData    ( oyCMMptr_s        * cmm_ptr,
                                       oyCMMptr_s        * cmm_ptr_out,
                                       oyFilterNode_s    * node )
{
  int error = !cmm_ptr || !cmm_ptr_out;
  oyCMMapi6_s * api6 = 0;
  char * reg = 0, * tmp = 0;

  if(error <= 0)
  {
    reg = oyStringCopy_( "//", oyAllocateFunc_ );
    tmp = oyFilterRegistrationToText( oyFilterNodePriv_m(node)->core->registration_,
                                      oyFILTER_REG_TYPE,0);
    STRING_ADD( reg, tmp );
    oyFree_m_( tmp );
    STRING_ADD( reg, "/" );
    STRING_ADD( reg, oyCMMptrPriv(cmm_ptr)->resource );
    STRING_ADD( reg, "_" );
    STRING_ADD( reg, oyCMMptrPriv(cmm_ptr_out)->resource );

    api6 = (oyCMMapi6_s*) oyCMMsGetFilterApi_( 0, reg, oyOBJECT_CMM_API6_S );

    error = !api6;
  }

  if(error <= 0 && api6->oyCMMdata_Convert)
    error = api6->oyCMMdata_Convert( cmm_ptr, cmm_ptr_out, node );
  else
    error = 1;

  if(error)
    WARNc_S("Could not convert context");

  return error;
}

/** Function  oyFilterNode_Connect
 *  @memberof oyFilterNode_s
 *  @brief    Connect two nodes by a edge
 *
 *  @param         input               the node to provide a socket
 *  @param         socket_nick         name of socket
 *  @param         output              the node providing a plug
 *  @param         plug_nick           name of plug
 *  @param         flags               unused
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/26 (Oyranos: 0.1.10)
 *  @date    2009/02/26
 */
int            oyFilterNode_Connect  ( oyFilterNode_s    * input,
                                       const char        * socket_nick,
                                       oyFilterNode_s    * output,
                                       const char        * plug_nick,
                                       int                 flags )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)input;
  int error = !s;
  oyFilterPlug_s * out_plug = 0;
  oyFilterSocket_s * output_socket = 0,
                   * in_socket = 0;
  int pos, out_pos;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )
  s = (oyFilterNode_s_*)output;
  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  if(error <= 0)
  {
    if(error <= 0 &&
       (!s->core || !s->core->api4_))
    {
      WARNc2_S( "%s: %s",
      _("attempt to add a incomplete filter"), s->relatives_ );
      error = 1;
    }
    if(error <= 0 &&
       !oyFilterNode_EdgeCount( input, 0, OY_FILTEREDGE_FREE ))
    {
      WARNc2_S( "%s: %s", "input node has no free socket",
                oyFilterCore_GetName( oyFilterNodePriv_m(input)->core, oyNAME_NAME) );
      error = 1;
    }

    if(error <= 0)
    {
      if(socket_nick)
        pos = oyFilterNode_GetConnectorPos( input, 0, socket_nick, 0,
                                            0 );
      else
        pos = 0;
      if(pos >= 0)
      {
        in_socket = oyFilterNode_GetSocket( input, pos );
        in_socket = oyFilterSocket_Copy( in_socket, 0 );
      }

      if(plug_nick)
        out_pos = oyFilterNode_GetConnectorPos( output, 1, plug_nick, 0,
                                                OY_FILTEREDGE_FREE );
      else
        out_pos = 0;
      if(out_pos >= 0)
      {
        out_plug = oyFilterNode_GetPlug( output, out_pos );
        out_plug = oyFilterPlug_Copy( out_plug, 0 );
      }

      if(!out_plug)
      {
        WARNc3_S( "\n  %s: \"%s\" %s", "Could not find plug for filter",
                  oyFilterCore_GetName( oyFilterNodePriv_m(output)->core, oyNAME_NAME), socket_nick );
        error = 1;
      }

      if(error <= 0)
      {
        if(oyFilterNode_ConnectorMatch( input, pos, out_plug ))
          output_socket = oyFilterNode_GetSocket( output, 0 );
        else
        {
          WARNc3_S( "\n  %s: %s -> %s", "Filter connectors do not match",
                    oyFilterNodePriv_m(input)->relatives_, oyFilterNodePriv_m(output)->relatives_ );
          error = 1;
        }
      }

      if(error <= 0 && output_socket && !oyFilterSocketPriv_m(output_socket)->data &&
         in_socket && oyFilterSocketPriv_m(in_socket)->data)
        oyFilterSocketPriv_m(output_socket)->data = oyFilterSocketPriv_m(in_socket)->data->copy(
                                                      oyFilterSocketPriv_m(in_socket)->data,
                                                      0 );

      if(error <= 0)
        oyFilterPlug_ConnectIntoSocket( &out_plug, &in_socket );

    } else
      WARNc2_S( "%s: %d", _("?? Nothing to add ??"),
                oyObject_GetId(input->oy_));
  }

  return error;
}

/** Function  oyFilterNode_ConnectorMatch
 *  @memberof oyFilterNode_s
 *  @brief    Check if a connector match to a FilterNode
 *
 *  @param         node_first          first node
 *  @param         pos_first           position of connector from first node
 *  @param         plug                second connector
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
OYAPI int  OYEXPORT
                 oyFilterNode_ConnectorMatch (
                                       oyFilterNode_s    * node_first,
                                       int                 pos_first,
                                       oyFilterPlug_s    * plug )
{
  int match = 0;
  oyConnector_s * a = 0,  * b = oyFilterPlugPriv_m(plug)->pattern;
  char * reg = 0,
       * tmp = 0;

  if(node_first && node_first->type_ == oyOBJECT_FILTER_NODE_S &&
     oyFilterNodePriv_m(node_first)->core)
    a = oyFilterNode_ShowConnector( node_first, pos_first, 0 );

  if(a && b)
  {
    oyFilterSocket_s * sock_first = oyFilterNode_GetSocket( node_first, pos_first );
    oyConnector_s * sock_first_pattern = oyFilterSocketPriv_m(sock_first)->pattern;
    match = 1;

    if(!oyConnectorPriv_m(b)->is_plug)
      match = 0;

    if(match)
    {
      /** Check if basic types match. */
      reg = oyStringCopy_( "//", oyAllocateFunc_ );
      tmp = oyFilterRegistrationToText( oyConnector_GetReg(a),
                                        oyFILTER_REG_TYPE, 0 );
      STRING_ADD( reg, tmp );
      if(tmp) oyFree_m_( tmp );
      match = oyFilterRegistrationMatch( reg, oyConnector_GetReg(b),
                                         0 );
      if(reg) oyFree_m_(reg);
    }

    /** More detailed checking is done in oyCMMapi5_s. */
    if(match && oyConnector_GetMatch(sock_first_pattern))
      oyConnector_GetMatch(sock_first_pattern)( sock_first, plug );
  }

  oyConnector_Release( &a );

  return match;
}

/** Function  oyFilterNode_Create
 *  @memberof oyFilterNode_s
 *  @brief    Initialise a new filter node object properly
 *
 *  @param         filter              the mandatory filter
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/30
 TODO select oyCMMapi7_s over registration string */
oyFilterNode_s *   oyFilterNode_Create(oyFilterCore_s    * filter,
                                       oyObject_s          object )
{
  oyFilterNode_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  oyFilterCore_s_ ** filter_ = &(oyFilterCore_s_*)filter;
  oyFilterNode_s_ ** s_      = &(oyFilterNode_s_*)s;

  if(!filter)
    return s;

  s = oyFilterNode_New( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(error <= 0)
  {
    (*s_)->core = oyFilterCore_Copy( filter, object );
    if(!(*s_)->core)
    {
      WARNc2_S("Could not copy filter: %s %s",
               (*filter_)->registration_, (*filter_)->category_)
      error = 1;
    }

    if(error <= 0)
      (*s_)->api7_ = (oyCMMapi7_s*) oyCMMsGetFilterApi_( 0,
                                (*s_)->core->registration_, oyOBJECT_CMM_API7_S );
    if(error <= 0 && !(*s_)->api7_)
    {
      WARNc2_S("Could not obtain filter api7 for: %s %s",
               (*filter_)->registration_, (*filter_)->category_)
      error = 1;
    }

    if((*s_)->api7_)
    {
      (*s_)->plugs_n_ = (*s_)->api7_->plugs_n + (*s_)->api7_->plugs_last_add;
      if((*s_)->api7_->plugs_last_add)
        --(*s_)->plugs_n_;
      (*s_)->sockets_n_ = (*s_)->api7_->sockets_n + (*s_)->api7_->sockets_last_add;
      if((*s_)->api7_->sockets_last_add)
        --(*s_)->sockets_n_;
    }

    if((*s_)->core)
    {
      size_t len = sizeof(oyFilterSocket_s*) *
             (oyFilterNode_EdgeCount( s, 0, 0 ) + 1);
      len = len?len:sizeof(oyFilterSocket_s*);
      (*s_)->sockets = allocateFunc_( len );
      memset( (*s_)->sockets, 0, len );

      len = sizeof(oyFilterSocket_s*) * (oyFilterNode_EdgeCount( s, 1, 0 ) + 1);
      len = len?len:sizeof(oyFilterSocket_s*);
      (*s_)->plugs = allocateFunc_( len );
      memset( (*s_)->plugs, 0, len );

      (*s_)->relatives_ = allocateFunc_( oyStrlen_((*filter_)->category_) + 24 );
      oySprintf_( (*s_)->relatives_, "%d: %s", oyObject_GetId(s->oy_), (*s_)->core->category_);
    }
  }

  if(error)
    oyFilterNode_Release( &s );

  return s;
}

/** Function  oyFilterNode_DataGet
 *  @memberof oyFilterNode_s
 *  @brief    Get process data from a filter socket
 *
 *  @param[in]     node                filter node
 *  @param[in]     socket_pos          position of socket
 *  @return                            the data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
oyStruct_s *   oyFilterNode_DataGet  ( oyFilterNode_s    * node,
                                       int                 socket_pos )
{
  oyFilterNode_s * s = node;
  oyStruct_s * data = 0;
  oyFilterSocket_s * socket = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  socket = oyFilterNode_GetSocket( node, socket_pos );
  if(socket)
    data = oyFilterSocketPriv_m(socket)->data;

  return data;
}

/** Function  oyFilterNode_DataSet
 *  @memberof oyFilterNode_s
 *  @brief    Set process data to a filter socket
 *
 *  @param[in,out] node                filter node
 *  @param[in]     data                data
 *  @param[in]     socket_pos          position of socket
 *  @param[in]     object              a object to not only set a reference
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int            oyFilterNode_DataSet  ( oyFilterNode_s    * node,
                                       oyStruct_s        * data,
                                       int                 socket_pos,
                                       oyObject_s        * object )
{
  oyFilterNode_s * s = node;
  oyFilterSocket_s_ * socket = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  socket = (oyFilterSocket_s_*)oyFilterNode_GetSocket( node, socket_pos );

  if(socket)
  {
    if(socket->data && socket->data->release)
      socket->data->release( &socket->data );

    if(data && data->copy)
      socket->data = data->copy( data, object );
    else
      socket->data = data;
  }

  return 0;
}

/** Function  oyFilterNode_Disconnect
 *  @memberof oyFilterNode_s
 *  @brief    Disconnect two nodes by a edge
 *
 *  @param         edge                plug
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int            oyFilterNode_Disconnect(oyFilterPlug_s    * edge )
{
  oyFilterPlug_s * s = edge;
  oyFilterSocket_Callback( s, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( &oyFilterPlugPriv_m(s)->remote_socket_ );
  return 0;
}

/** Function  oyFilterNode_EdgeCount
 *  @memberof oyFilterNode_s
 *  @brief    Count real and potential connections to a filter node object
 *
 *  @param         node                the node
 *  @param         is_input            1 - plugs; 0 - sockets
 *  @param         flags               specify which number to return
 *                                     - oyranos::OY_FILTEREDGE_FREE: count available
 *                                     - oyranos::OY_FILTEREDGE_CONNECTED: count used
 *                                     - oyranos::OY_FILTEREDGE_LASTTYPE: account only
 *                                       for the last connector type
 *  @return                            the number of possible edges
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/27
 */
int            oyFilterNode_EdgeCount( oyFilterNode_s    * node,
                                       int                 is_input,
                                       int                 flags )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;
  int n = 0, start, i,
      possible = 0,
      connected = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  if(!s->core || !s->api7_)
    return 0;

  /* plugs */
  if(is_input)
  {
    if(oyToFilterEdge_LastType_m(flags))
    {
      possible = s->api7_->plugs_last_add + 1;
      start = s->api7_->plugs_n - 1;
    } else
    {
      possible = s->plugs_n_;
      start = 0;
    }

    if(s->plugs)
      for(i = start; i < possible; ++i)
        if(s->plugs[i] && s->plugs[i]->remote_socket_)
          ++connected;

    if(oyToFilterEdge_Free_m(flags))
      n = possible - connected;
    else if(oyToFilterEdge_Connected_m(flags))
      n = connected;
    else
      n = possible;

  } else
  /* ... sockets */
  {
    if(oyToFilterEdge_LastType_m(flags))
    {
      possible = s->api7_->sockets_last_add + 1;
      start = s->api7_->sockets_n - 1;
    } else
    {
      possible = s->sockets_n_;
      start = 0;
    }

    if(s->sockets)
      for(i = 0; i < possible; ++i)
        if(s->sockets[i])
          connected += oyFilterPlugs_Count(s->sockets[i]->requesting_plugs_);

    if(oyToFilterEdge_Free_m(flags))
      n = possible ? INT32_MAX : 0;
    else if(oyToFilterEdge_Connected_m(flags))
      n = connected;
    else
      n = possible;
  }

  return n;
}

/** Function  oyFilterNode_GetConnectorPos
 *  @memberof oyFilterNode_s
 *  @brief    Get a oyFilterSocket_s or oyFilterPlug_s position from a FilterNode
 *
 *  @param         node                filter node
 *  @param         is_input            1 - plugs; 0 - sockets
 *  @param         pattern             the pattern to be found in the
 *                                     oyConnector_s::connector_type of the
 *                                     searched plug or socket. Its a
 *                                     @ref registration string. E.g. a typical
 *                                     data connection: "//" OY_TYPE_STD "/data"
 *                                     See as well oyranos::oyCONNECTOR_e.
 *  @param         nth_of_type         the position in the group of the
 *                                     connector type for this filter; Note
 *                                     this parameter makes only sense for the
 *                                     last filter defined connector, as only
 *                                     this one can occure multiple times.
 *  @param         flags               specify which status to return
 *                                     - zero means: take all into account
 *                                     - oyranos::OY_FILTEREDGE_FREE: next free available
 *                                     - oyranos::OY_FILTEREDGE_CONNECTED: consider used
 *  @return                            the absolute position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/26 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT
               oyFilterNode_GetConnectorPos (
                                       oyFilterNode_s    * node,
                                       int                 is_input,
                                       const char        * pattern,
                                       int                 nth_of_type,
                                       int                 flags )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;
  int pos = -1,
      i, j, n, n2,
      nth = -1;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return pos )

  if(!pattern)
  {
    WARNc1_S("no ID argument given %s", s->relatives_ );
    return pos;
  }

  if(nth_of_type == -1)
    nth_of_type = 0;

  /* plugs */
  if(is_input)
  {
    n = s->api7_->plugs_n;
    for( i = 0; i < n; ++i )
    {
      if(oyFilterRegistrationMatch( oyConnector_GetReg(s->api7_->plugs[i]),
                                    pattern, 0))
      {
        if( i == n - 1 && s->api7_->plugs_last_add)
          n2 = s->api7_->plugs_last_add;
        else
          n2 = 0;

        for( j = 0; j <= n2; ++j )
        {
          if(oyToFilterEdge_Free_m(flags))
          {
            if( s->plugs[i + j] && s->plugs[i + j]->remote_socket_ )
              continue;
            else
              ++nth;

          } else
          if(oyToFilterEdge_Connected_m(flags))
          {
            if( s->plugs[i + j] && s->plugs[i + j]->remote_socket_ )
              ++nth;
            else
              continue;

          } else
            ++nth;

          if( nth == nth_of_type )
          {
            pos = i + j;
            return pos;
          }
        }
      }
    }
  } else
  /* ... or sockets */
  {
    /* 1. count possible connectors */
    n = s->api7_->sockets_n;
    for( i = 0; i < n; ++i )
    {
      /* 2. compare pattern argument with the socket type */
      if(oyFilterRegistrationMatch( oyConnector_GetReg(s->api7_->sockets[i]),
                                    pattern, 0))
      {

        /* 3. iterate through at least connectors or connectors that where added
              to the last one */
        if( i == n - 1 && s->api7_->sockets_last_add)
          n2 = s->api7_->sockets_last_add;
        else
          n2 = 0;

        for( j = 0; j <= n2; ++j )
        {
          /* 3.1 check only unused connectors */
          if(oyToFilterEdge_Free_m(flags))
          {
            if( s->sockets[i + j] &&
                oyFilterPlugs_Count( s->sockets[i + j]->requesting_plugs_ ) )
              continue;
            else
              ++nth;

          } else
          /* 3.2 check only used connectors */
          if(oyToFilterEdge_Connected_m(flags))
          {
            if( s->sockets[i + j] &&
                oyFilterPlugs_Count( s->sockets[i + j]->requesting_plugs_ ) )
              ++nth;
            else
              continue;

          } else
          /* 3.3 count all connectors */
            ++nth;

          /* 4. check the type relative positional parameter */
          if( nth == nth_of_type )
          {
            /* 4.1 return as we otherwise would need to leave two loops */
            pos = i + j;
            return pos;
          }
        }
      }
    }
  }

  return pos;
}

/** Function  oyFilterNode_GetId
 *  @memberof oyFilterNode_s
 *  @brief    Get the object Id
 *
 *  @param[in]     node                filter node
 *  @return                            the object Id
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/02/25
 */
OYAPI int  OYEXPORT
               oyFilterNode_GetId    ( oyFilterNode_s    * node )
{
  oyFilterNode_s * s = node;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return -1 )

  return oyObject_GetId( node->oy_ );
}

/** Function  oyFilterNode_GetPlug
 *  @memberof oyFilterNode_s
 *  @brief    Get a oyFilterPlug_s of type from a FilterNode
 *
 *  @param         node                filter node
 *  @param         pos                 position of connector from filter
 *  @return                            the plug - no copy
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/30
 */
OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterNode_GetPlug( oyFilterNode_s    * node,
                                       int                 pos )
{
  oyFilterNode_s_ ** node_ = &(oyFilterNode_s_*)node;
  oyFilterPlug_s_ * s = 0;

  if(node && node->type_ == oyOBJECT_FILTER_NODE_S &&
     pos < oyFilterNode_EdgeCount( node, 1, 0 ))
  {
    oyAlloc_f allocateFunc_ = node->oy_->allocateFunc_;

    if(!(*node_)->plugs)
    {
      size_t len = sizeof(oyFilterPlug_s*) *
                   (oyFilterNode_EdgeCount( node, 1, 0 ) + 1);
      (*node_)->plugs = allocateFunc_( len );
      memset( (*node_)->plugs, 0, len );
    }

    if(!(*node_)->plugs[pos])
    {
      s = (oyFilterPlug_s_*)oyFilterPlug_New( node->oy_ );
      s->pattern = oyFilterNode_ShowConnector( node, pos, 1 );
      s->node = oyFilterNode_Copy( node, 0 );
      s->relatives_ = oyStringCopy_( (*node_)->relatives_, allocateFunc_ );
      (*node_)->plugs[pos] = (oyFilterPlug_s*)s;
    }

    s = (oyFilterPlug_s_*)(*node_)->plugs[pos];
  }

  return (oyFilterPlug_s*)s;
}

/** Function  oyFilterNode_GetSocket
 *  @memberof oyFilterNode_s
 *  @brief    Get a oyFilterSocket_s of type from a FilterNode
 *
 *  @param         node                filter node
 *  @param         pos                 absolute position of connector
 *  @return                            the socket - no copy!
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/30
 */
OYAPI oyFilterSocket_s * OYEXPORT
                 oyFilterNode_GetSocket (
                                       oyFilterNode_s    * node,
                                       int                 pos )
{
  oyFilterSocket_s * s = 0;

  oyFilterNode_s_ ** node_ = &(oyFilterNode_s_*)node;
  oyFilterSocket_s_ ** s_ = &(oyFilterSocket_s_*)s;

  if(node && node->type_ == oyOBJECT_FILTER_NODE_S &&
     pos < oyFilterNode_EdgeCount( node, 0, 0 ))
  {
    oyAlloc_f allocateFunc_ = node->oy_->allocateFunc_;

    if(!(*node_)->sockets)
    {
      size_t len = sizeof(oyFilterSocket_s*) *
                   (oyFilterNode_EdgeCount( node, 0, 0 ) + 1);
      (*node_)->sockets = allocateFunc_( len );
      memset( (*node_)->sockets, 0, len );
    }

    if(!(*node_)->sockets[pos])
    {
      s = oyFilterSocket_New( node->oy_ );
      (*s_)->pattern = oyFilterNode_ShowConnector( node, pos, 0 );
      (*s_)->node = oyFilterNode_Copy( node, 0 );
      (*s_)->relatives_ = oyStringCopy_( (*node_)->relatives_, allocateFunc_ );
      (*node_)->sockets[pos] = s;
    }

    s = (*node_)->sockets[pos];
  }

  return s;
}

/** Function  oyFilterNode_GetText
 *  @memberof oyFilterNode_s
 *  @brief    Serialise filter node to text
 *
 *  Serialise into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML
 *  - oyNAME_DESCRIPTION: ??
 *
 *  This function provides a complete description of the context. It might be
 *  more adequate to use only a subset for hashing as not all data and options
 *  might have an effect to the context data result.
 *  The oyCMMapi4_s::oyCMMFilterNode_GetText() function provides a way to let a
 *  module decide about what to place into a hash text.
 *
 *  @param[in,out] node                filter node
 *  @param[out]    name_type           the type
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
const char * oyFilterNode_GetText    ( oyFilterNode_s    * node,
                                       oyNAME_e            name_type )
{
  const char * tmp = 0;
  char * hash_text = 0;
  oyFilterNode_s_ ** node_ = &(oyFilterNode_s_*)node;

  oyStructList_s * in_datas = 0,
                 * out_datas = 0;

  if(!node)
    return 0;

  /* 1. create hash text */
  hashTextAdd_m( "<oyFilterNode_s>\n  " );

  /* the filter text */
  hashTextAdd_m( oyFilterCore_GetText( (*node_)->core, oyNAME_NAME ) );

  /* pick all plug (input) data */
  in_datas = oyFilterNode_DataGet_( *node_, 1 );

  /* pick all sockets (output) data */
  out_datas = oyFilterNode_DataGet_( *node_, 0 );

  /* make a description */
  tmp = oyContextCollectData_( (oyStruct_s*)node, (*node_)->core->options_,
                               in_datas, out_datas );
  hashTextAdd_m( tmp );

  hashTextAdd_m( "</oyFilterNode_s>\n" );


  oyObject_SetName( node->oy_, hash_text, oyNAME_NICK );

  if(node->oy_->deallocateFunc_)
    node->oy_->deallocateFunc_( hash_text );
  hash_text = 0;

  hash_text = (oyChar*) oyObject_GetName( node->oy_, oyNAME_NICK );

  return hash_text;
}

/** Function  oyFilterNode_NewWith
 *  @memberof oyFilterNode_s
 *  @brief    Initialise a new filter node object properly
 *
 *  @param         registration        a registration string, @see module_api
 *  @param         options             options for the filter
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/06/24
 */
oyFilterNode_s *   oyFilterNode_NewWith (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyFilterCore_s * core = oyFilterCore_New( registration, options, object);
  oyFilterNode_s * node = oyFilterNode_Create( core, object );

  oyFilterCore_Release( &core );
  return node;
}

/** Function  oyFilterNode_OptionsGet
 *  @memberof oyFilterNode_s
 *  @brief    Get filter options
 *
 *  @param[in,out] node                filter object
 *  @param         flags               see oyOptions_s::oyOptions_ForFilter()
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/06/26
 */
oyOptions_s* oyFilterNode_OptionsGet ( oyFilterNode_s    * node,
                                       int                 flags )
{
  oyOptions_s * options = 0;
  oyFilterNode_s * s = node;
  int error = 0;

  oyFilterNode_s_ ** node_ = &(oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  if(flags || !(*node_)->core->options_)
  {
    options = oyOptions_ForFilter_( (*node_)->core, flags, (*node_)->core->oy_ );
    if(!(*node_)->core->options_)
      (*node_)->core->options_ = oyOptions_Copy( options, 0 );
    else
      error = oyOptions_Filter( &(*node_)->core->options_, 0, 0,
                                oyBOOLEAN_UNION,
                                0, options );
    if(!(*node_)->core->options_)
      (*node_)->core->options_ = oyOptions_New( 0 );
  }

  options = oyOptions_Copy( (*node_)->core->options_, 0 );

  /** Observe exported options for changes and propagate to a existing graph. */
  error = oyOptions_ObserverAdd( options, (oyStruct_s*)node,
                                 0, oyFilterNodeObserve_ );

  return options;
}

/** Function  oyFilterNode_ShowConnector
 *  @memberof oyFilterNode_s
 *  @brief    Get a connector description from a filter module
 *
 *  The path to obtain a new connector.
 *  The filter can say it has more connectors to provide for a certain kind of
 *  static connector eigther described in oyCMMapi4_s::inputs or
 *  oyCMMapi4_s::outputs.
 *
 *  @param[in]   node                the module filter node
 *  @param[in]   as_pos              the according oyConnector_s
 *  @param[in]   is_plug             select from 0 - plugs or 1 - sockets
 *  @return                          the new oyConnector_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/28 (Oyranos: 0.1.8)
 *  @date    2008/12/27
 */
OYAPI oyConnector_s * OYEXPORT
             oyFilterNode_ShowConnector (
                                       oyFilterNode_s    * node,
                                       int                 as_pos,
                                       int                 is_plug )
{
  oyConnector_s * pattern = 0;
  oyObject_s object = 0;

  oyFilterNode_s_ ** node_ = &(oyFilterNode_s_*)node;

  if(!node || !(*node_)->core || node->type_ != oyOBJECT_FILTER_NODE_S ||
     !(*node_)->api7_)
    return 0;

  object = oyObject_New ();

  if(is_plug)
  {
    if((*node_)->api7_->plugs_n <= as_pos &&
       as_pos < oyFilterNode_EdgeCount( node, 1, 0 ))
      as_pos = (*node_)->api7_->plugs_n - 1;

    if((*node_)->api7_->plugs_n > as_pos)
      pattern = oyConnector_Copy( (*node_)->api7_->plugs[as_pos], object );
  } else {
    if((*node_)->api7_->sockets_n <= as_pos &&
       as_pos < oyFilterNode_EdgeCount( node, 0, 0 ))
      as_pos = (*node_)->api7_->sockets_n - 1;

    if((*node_)->api7_->sockets_n > as_pos)
      pattern = oyConnector_Copy( (*node_)->api7_->sockets[as_pos], object );
  }

  oyObject_Release( &object );

  return pattern;
}

/** Function  oyFilterNode_UiGet
 *  @memberof oyFilterNode_s
 *  @brief    Get filter options XFORMS
 *
 *  @param[in,out] node                filter object
 *  @param[out]    ui_text             XFORMS fitting to the node Options
 *  @param[out]    namespaces          additional XML namespaces
 *  @param         allocateFunc        optional user allocator
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/29 (Oyranos: 0.1.10)
 *  @date    2009/08/31
 */
int            oyFilterNode_UiGet    ( oyFilterNode_s     * node,
                                       char              ** ui_text,
                                       char             *** namespaces,
                                       oyAlloc_f            allocateFunc )
{
  int error = 0;
  oyFilterNode_s * s = node;
  oyOptions_s * options = 0;
  char * text = 0,
       * tmp = 0;

  oyFilterNode_s_ ** node_ = &(oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(!error)
    options = oyFilterNode_OptionsGet( node, 0 );

  if(!error)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i,j = 0;
    oyCMMapi9_s * cmm_api9 = 0;
    oyCMMapi9_s_ ** cmm_api9_ = &(oyCMMapi9_s_*)cmm_api9;
    char * class, * api_reg;
    const char * reg = (*node_)->core->registration_;

    class = oyFilterRegistrationToText( reg, oyFILTER_REG_TYPE, 0 );
    api_reg = oyStringCopy_("//", oyAllocateFunc_ );
    STRING_ADD( api_reg, class );
    oyFree_m_( class );

    apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 0,0 );
    apis_n = oyCMMapiFilters_Count( apis );
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

      if(oyFilterRegistrationMatch( reg, (*cmm_api9_)->pattern, 0 ))
      {
        if((*cmm_api9_)->oyCMMuiGet)
          error = (*cmm_api9_)->oyCMMuiGet( options, &tmp, oyAllocateFunc_ );

        if(error)
        {
          WARNc2_S( "%s %s",_("error in module:"), (*cmm_api9_)->registration );
          return 1;

        } else
        if(tmp)
        {
          STRING_ADD( text, tmp );
          STRING_ADD( text, "\n" );
          oyFree_m_(tmp);

          if(namespaces && (*cmm_api9_)->xml_namespace)
          {
            if(j == 0)
            {
              size_t len = (apis_n - i + 1) * sizeof(char*);
              *namespaces = allocateFunc( len );
              memset(*namespaces, 0, len);
            }
            *namespaces[j] = oyStringCopy_( (*cmm_api9_)->xml_namespace,
                                            allocateFunc );
            ++j;
            namespaces[j] = 0;
          }
        }
      }

      if(cmm_api9->release)
        cmm_api9->release( (oyStruct_s**)&cmm_api9 );
    }
    oyCMMapiFilters_Release( &apis );
  }

  if(!error && (*node_)->core->api4_->ui->oyCMMuiGet)
  {
    /* @todo and how to mix in the values? */
    error = (*node_)->core->api4_->ui->oyCMMuiGet( options, &tmp, oyAllocateFunc_ );
    if(tmp)
    {
      STRING_ADD( text, tmp );
      oyFree_m_(tmp);
    }
  }

  oyOptions_Release( &options );

  if(error <= 0 && text)
  {
    *ui_text = oyStringCopy_( text, allocateFunc );
  }

  return error;
}
