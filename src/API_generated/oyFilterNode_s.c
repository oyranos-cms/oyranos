/** @file oyFilterNode_s.c

   [Template file inheritance graph]
   +-> oyFilterNode_s.template.c
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


  
#include "oyFilterNode_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyFilterNode_s_.h"

#include "oyranos_generic.h"
#include "oyranos_module_internal.h"
#include "oyOptions_s.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterSocket_s_.h"
#include "oyConnector_s_.h"
#include "oyStructList_s.h"
#include "oyFilterCore_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyCMMapiFilters_s.h"
#include "oyPointer_s_.h"
  


/** Function oyFilterNode_New
 *  @memberof oyFilterNode_s
 *  @brief   allocate a new FilterNode object
 */
OYAPI oyFilterNode_s * OYEXPORT
  oyFilterNode_New( oyObject_s object )
{
  oyObject_s s = object;
  oyFilterNode_s_ * filternode = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  filternode = oyFilterNode_New_( s );

  return (oyFilterNode_s*) filternode;
}

/** Function  oyFilterNode_Copy
 *  @memberof oyFilterNode_s
 *  @brief    Copy or Reference a FilterNode object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     filternode                 FilterNode struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyFilterNode_s* OYEXPORT
  oyFilterNode_Copy( oyFilterNode_s *filternode, oyObject_s object )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*) filternode;

  if(s)
  {
    oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )
  }
  else
    return NULL;

  s = oyFilterNode_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyFilterNode_s" );

  return (oyFilterNode_s*) s;
}
 
/** Function oyFilterNode_Release
 *  @memberof oyFilterNode_s
 *  @brief   release and possibly deallocate a oyFilterNode_s object
 *
 *  @param[in,out] filternode                 FilterNode struct object
 */
OYAPI int OYEXPORT
  oyFilterNode_Release( oyFilterNode_s **filternode )
{
  oyFilterNode_s_ * s = 0;

  if(!filternode || !*filternode)
    return 0;

  s = (oyFilterNode_s_*) *filternode;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  *filternode = 0;

  return oyFilterNode_Release_( &s );
}



/* Include "FilterNode.public_methods_definitions.c" { */
#include "oyFilterCore_s_.h"

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
                                       int                 flags OY_UNUSED )
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
      WARNcc2_S( s, "%s: %s",
      _("attempt to add a incomplete filter"), s->relatives_ );
      error = 1;
    }
    if(error <= 0 &&
       !oyFilterNode_EdgeCount( input, 0, OY_FILTEREDGE_FREE ))
    {
      WARNcc2_S( input, "%s: %s", "input node has no free socket",
                 oyFilterCore_GetName( (oyFilterCore_s*)oyFilterNodePriv_m(input)->core, oyNAME_NAME) );
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
        in_socket = oyFilterNode_GetSocket( input, pos );

      if(plug_nick)
        out_pos = oyFilterNode_GetConnectorPos( output, 1, plug_nick, 0,
                                                OY_FILTEREDGE_FREE );
      else
        out_pos = 0;

      if(out_pos >= 0)
        out_plug = oyFilterNode_GetPlug( output, out_pos );

      if(!out_plug)
      {
        WARNcc3_S( output, "\n  %s: \"%s\" %s", "Could not find plug for filter",
                   oyFilterCore_GetName( (oyFilterCore_s*)oyFilterNodePriv_m(output)->core, oyNAME_NAME), socket_nick );
        error = 1;
      }

      if(error <= 0)
      {
        if(oyFilterNode_ConnectorMatch( input, pos, out_plug ))
          output_socket = oyFilterNode_GetSocket( output, 0 );
        else
        {
          WARNcc3_S( input, "\n  %s: %s -> %s", "Filter connectors do not match",
                     oyFilterNodePriv_m(input)->relatives_, oyFilterNodePriv_m(output)->relatives_ );
          error = 1;
        }
      }

      if(error <= 0 && output_socket && !oyFilterSocketPriv_m(output_socket)->data &&
         in_socket && oyFilterSocketPriv_m(in_socket)->data)
      {
        oyFilterSocketPriv_m(output_socket)->data = oyFilterSocketPriv_m(in_socket)->data->copy(
                                                      oyFilterSocketPriv_m(in_socket)->data,
                                                      0 );
        if(oy_debug_objects >= 0)
          oyObjectDebugMessage_( oyFilterSocketPriv_m(output_socket)->data->oy_, __func__,
                                 oyStructTypeToText(oyFilterSocketPriv_m(output_socket)->data->type_) );
      }

      if(error <= 0)
      {
        oyFilterPlug_s_  * out_plug_ = (oyFilterPlug_s_*) out_plug;
        oyFilterSocket_s_* in_socket_ = (oyFilterSocket_s_*)in_socket;
        error = oyFilterPlug_ConnectIntoSocket( out_plug, in_socket );
        if(error <= 0)
        {
          oyFilterNode_Release( (oyFilterNode_s**) &out_plug_->node );
          out_plug_->node = (oyFilterNode_s_*) output; oyFilterNode_Copy( output, NULL );
          oyFilterNode_Release( (oyFilterNode_s**) &in_socket_->node );
          in_socket_->node = (oyFilterNode_s_*) input; oyFilterNode_Copy( input, NULL );
        }
        oyFilterPlug_Release( &out_plug );
        oyFilterSocket_Release( &in_socket );
      }

    } else
      WARNcc2_S( input, "%s: %d", _("?? Nothing to add ??"),
                 oyObject_GetId(input->oy_));
  }
  oyFilterSocket_Release( &output_socket );

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

    oyFilterSocket_Release( &sock_first );
  }

  oyConnector_Release( &a );

  return match;
}

/** Function  oyFilterNode_Create
 *  @memberof oyFilterNode_s
 *  @brief    Initialise a new filter node object properly
 *
 *  @param         registration        the processing filter @ref registration string
 *  @param         filter              the context filter
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/01
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 */
oyFilterNode_s *   oyFilterNode_Create(const char        * registration,
                                       oyFilterCore_s    * filter,
                                       oyObject_s          object )
{
  oyFilterNode_s_ * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  oyFilterCore_s_ * filter_ = (oyFilterCore_s_*)filter;

  if(!filter)
    return (oyFilterNode_s*) s;

  s = (oyFilterNode_s_*) oyFilterNode_New( object );
  error = !s;

  if(error <= 0)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    s->core = (oyFilterCore_s_*)oyFilterCore_Copy( filter, object );
    if(!s->core)
    {
      WARNc2_S("Could not copy filter: %s %s",
               filter_->registration_, filter_->category_)
      error = 1;
    }

    if(error <= 0)
      s->api7_ = (oyCMMapi7_s_*) oyCMMsGetFilterApi_(
                                           registration, oyOBJECT_CMM_API7_S );
    if(error <= 0 && !s->api7_)
    {
      WARNc2_S("Could not obtain filter api7 for: %s %s",
               filter_->registration_, filter_->category_)
      error = 1;
    }

    if(s->api7_)
    {
      s->plugs_n_ = s->api7_->plugs_n + s->api7_->plugs_last_add;
      if(s->api7_->plugs_last_add)
        --s->plugs_n_;
      s->sockets_n_ = s->api7_->sockets_n + s->api7_->sockets_last_add;
      if(s->api7_->sockets_last_add)
        --s->sockets_n_;
    }

    if(s->core)
    {
      int count = oyFilterNode_EdgeCount( (oyFilterNode_s*) s, 0, 0 );
      size_t len;
      if(count < 0) count = 0;
      len = sizeof(oyFilterSocket_s*) * (count + 1);

      s->sockets = allocateFunc_( len );
      memset( s->sockets, 0, len );

      len = sizeof(oyFilterSocket_s*) * (oyFilterNode_EdgeCount(
                                               (oyFilterNode_s*) s, 1, 0 ) + 1);
      len = len?len:sizeof(oyFilterSocket_s*);
      s->plugs = allocateFunc_( len );
      memset( s->plugs, 0, len );

      s->relatives_ = allocateFunc_( oyStrlen_(filter_->category_) + 24 );
      oySprintf_( s->relatives_, "%d: %s", oyObject_GetId(s->oy_), s->core->category_);
    }
  }

  if(error)
    oyFilterNode_Release( (oyFilterNode_s**) &s );
  else if(oy_debug)
  {
    const char * txt = oyStruct_GetInfo( (oyStruct_s*)s, oyNAME_DESCRIPTION, 0x01 );
    oyObject_SetName( s->oy_, txt, oyNAME_DESCRIPTION );
  }

  return (oyFilterNode_s*) s;
}

/** Function  oyFilterNode_GetData
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
oyStruct_s *   oyFilterNode_GetData  ( oyFilterNode_s    * node,
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

  oyFilterSocket_Release( &socket );

  if(data && data->copy)
  {
    data = data->copy( data, 0 );
    if(oy_debug_objects >= 0)
      oyObjectDebugMessage_( data->oy_, __func__, oyStructTypeToText(data->type_) );
  }

  return data;
}

/** Function  oyFilterNode_SetData
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
int            oyFilterNode_SetData  ( oyFilterNode_s    * node,
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
    {
      socket->data = data->copy( data, object );
      if(oy_debug_objects >= 0)
        oyObjectDebugMessage_( socket->data->oy_, __func__, oyStructTypeToText(socket->data->type_) );
    } else
      socket->data = data;

    oyFilterSocket_Release( (oyFilterSocket_s **) &socket );
  } else
  {
    WARNc_S("Node has no socket. Can not assign data.");
    return -1;
  }

  return 0;
}

/** Function  oyFilterNode_Disconnect
 *  @memberof oyFilterNode_s
 *  @brief    Disconnect two nodes by a edge
 *
 *  @param         node                a node
 *  @param         pos                 the plug number to release
 *  @return                            error
 *
 *  @version  Oyranos: 0.9.7
 *  @date     2018/09/03
 *  @since    2009/03/04 (Oyranos: 0.1.10)
 */
int            oyFilterNode_Disconnect(oyFilterNode_s    * node,
                                       int                 pos )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;
  oyFilterPlug_s_ * edge;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  edge = s->plugs[pos];
  if(!edge)
    return 1;

  {
    oyFilterPlug_s_ * s = edge;
    oyCheckType__m( oyOBJECT_FILTER_PLUG_S, fprintf(stderr, "id:%d\n",s->oy_->id_);return 1 );
  }

  oyFilterSocket_Callback( (oyFilterPlug_s*)edge, oyCONNECTOR_EVENT_RELEASED );
  oyFilterPlug_Callback( (oyFilterPlug_s*)edge, oyCONNECTOR_EVENT_RELEASED );
  return 0;
}

/** Function  oyFilterNode_EdgeCount
 *  @memberof oyFilterNode_s
 *  @brief    Count real and potential connections to a filter node object
 *
 *  @param         node                the node
 *  @param         is_input            1 - plugs; 0 - sockets
 *  @param         flags               specify which number to return
 *                                     - ::OY_FILTEREDGE_FREE: count available
 *                                     - ::OY_FILTEREDGE_CONNECTED: count used
 *                                     - ::OY_FILTEREDGE_LASTTYPE: account only
 *                                       for the last connector type
 *  @return                            the number of possible edges
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/01
 *  @since   2009/02/24 (Oyranos: 0.1.10)
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

    if(s->plugs && oyToFilterEdge_Connected_m(flags))
      for(i = start; i < possible; ++i)
      {
        if(s->plugs[i] && s->plugs[i]->remote_socket_)
          ++connected;
        else
          break;
      }

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

    if(s->sockets && oyToFilterEdge_Connected_m(flags))
      for(i = 0; i < possible; ++i)
      {
        if(s->sockets[i])
          connected += oyFilterPlugs_Count(s->sockets[i]->requesting_plugs_);
        else
          break;
      }

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
 *  @param         nth_of_type         the position in the group of the
 *                                     connector type for this filter; Note
 *                                     this parameter makes only sense for the
 *                                     last filter defined connector, as only
 *                                     this one can occure multiple times.
 *  @param         flags               specify which status to return
 *                                     - zero means: take all into account
 *                                     - ::OY_FILTEREDGE_FREE: next free available
 *                                     - ::OY_FILTEREDGE_CONNECTED: consider used
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
 *  @return                            the plug
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/30 (Oyranos: 0.1.8)
 *  @date    2008/07/30
 */
OYAPI oyFilterPlug_s * OYEXPORT oyFilterNode_GetPlug (
                                       oyFilterNode_s    * node,
                                       int                 pos )
{
  oyFilterNode_s_ * node_ = (oyFilterNode_s_*)node;
  oyFilterPlug_s_ * s = 0;

  if(node_ && node_->type_ == oyOBJECT_FILTER_NODE_S &&
     pos < oyFilterNode_EdgeCount( (oyFilterNode_s*)node_, 1, 0 ))
  {
    oyAlloc_f allocateFunc_ = node_->oy_->allocateFunc_;

    if(!node_->plugs)
    {
      int count = oyFilterNode_EdgeCount( (oyFilterNode_s*)node_, 1, 0 );
      size_t len;
      if(count < 0) count = 0;
      len = sizeof(oyFilterPlug_s*) * (count + 1);

      node_->plugs = allocateFunc_( len );
      memset( node_->plugs, 0, len );
    }

    if(!node_->plugs[pos])
    {
      node_->plugs[pos] = s = (oyFilterPlug_s_*)oyFilterPlug_New( node_->oy_ );
      s->pattern = oyFilterNode_ShowConnector( (oyFilterNode_s*)node_, pos, 1 );
      s->node = node_;       oyFilterNode_Copy( (oyFilterNode_s*)node_, 0);
      s->relatives_ = oyStringCopy_( node_->relatives_, allocateFunc_ );
    }

    s = node_->plugs[pos];
    oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return NULL )
  }

  return oyFilterPlug_Copy( (oyFilterPlug_s*)s, 0 ); /* reference for giving to outside */
}

/** Function  oyFilterNode_Run
 *  @memberof oyFilterNode_s
 *  @brief    Run a connected oyFilterPlug_s through the FilterNode
 *
 *  @param         node                filter node
 *  @param         plug                the plug
 *  @param         ticket              the job ticket
 *  @return                            error
 *
 *  @version  Oyranos: 0.9.7
 *  @date     2020/02/20
 *  @since    2012/09/24 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                oyFilterNode_Run     ( oyFilterNode_s    * node,
                                       oyFilterPlug_s *    plug,
                                       oyPixelAccess_s *   ticket )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;
  int error = 0;

  if(!node)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

#if DEBUG
  if(oy_debug)
  {
    const char * tags = oyOptions_GetText( s->tags, oyNAME_NICK )/*,
               * opts = oyOptions_GetText( s->api7_->options, oyNAME_NICK )*/;
    DBGs_NUM6_S( ticket, "\"%s::%s\" %d[%d]\n\t\"%s\"\n\t\"%s\"",
                 oyFilterNode_GetRegistration(node),
                 oyFilterNode_GetRendererRegistration(node),
                 s->api7_->sockets_n, oyFilterNode_CountSocketNodes(node, 0, OY_FILTEREDGE_CONNECTED),
                 oyFilterNode_GetRelatives(node),
                 tags ? tags : "----"
               );
  }
#endif

  error = oyCMMapi7_Run( (oyCMMapi7_s*) s->api7_, plug, ticket );
  oyStruct_ObserverSignal( (oyStruct_s*) node, oySIGNAL_VISITED, (oyStruct_s*) ticket );

  return error;
}

/** Function  oyFilterNode_GetSocket
 *  @memberof oyFilterNode_s
 *  @brief    Get a oyFilterSocket_s of type from a FilterNode
 *
 *  @param         node                filter node
 *  @param         pos                 absolute position of connector
 *  @return                            the socket
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
  oyFilterSocket_s_ * s = 0;
  oyFilterNode_s_ * node_ = (oyFilterNode_s_*)node;

  if(node_ && node_->type_ == oyOBJECT_FILTER_NODE_S &&
     /* do a range check */
     (0 <= pos && pos < oyFilterNode_EdgeCount( (oyFilterNode_s*)node_, 0, 0 )))
  {
    oyAlloc_f allocateFunc_ = node_->oy_->allocateFunc_;

    if(!node_->sockets)
    {
      int count = oyFilterNode_EdgeCount( (oyFilterNode_s*)node_, 0, 0 );
      size_t len;
      if(count < 0) count = 0;
      len = sizeof(oyFilterSocket_s*) * (count + 1);

      node_->sockets = allocateFunc_( len );
      memset( node_->sockets, 0, len );
    }

    if(!node_->sockets[pos])
    {
      node_->sockets[pos] = s = (oyFilterSocket_s_*)oyFilterSocket_New( node_->oy_ );
      s->pattern = oyFilterNode_ShowConnector( (oyFilterNode_s*)node_, pos, 0 );
      s->node = node_;         oyFilterNode_Copy( (oyFilterNode_s*)node_,0 );
      s->relatives_ = oyStringCopy_( node_->relatives_, allocateFunc_ );
    }

    s = node_->sockets[pos];
  }

  return oyFilterSocket_Copy( (oyFilterSocket_s*)s, 0 );
}

/** Function  oyFilterNode_GetSocketNode
 *  @memberof oyFilterNode_s
 *  @brief    Get a neighbouring oyFilterNode_s
 *
 *  @param         node                filter node
 *  @param         pos                 connector position
 *  @param         plugs_pos           position in plug list
 *  @return                            the node
 *
 *  @version  Oyranos: 0.5.0
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 *  @date     2012/10/03
 */
OYAPI oyFilterNode_s * OYEXPORT
               oyFilterNode_GetSocketNode (
                                       oyFilterNode_s    * node,
                                       int                 pos,
                                       int                 plugs_pos )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node,
                  * remote = 0;
  oyFilterPlugs_s * remote_plugs = 0;
  oyFilterPlug_s_ * remote_plug = 0;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  remote_plugs = s->sockets[pos]->requesting_plugs_;
  remote_plug = (oyFilterPlug_s_*)oyFilterPlugs_Get( remote_plugs, plugs_pos );
  remote = remote_plug->node;

  oyFilterPlug_Release( (oyFilterPlug_s**)&remote_plug );

  oyFilterNode_Copy((oyFilterNode_s*)remote,0);
  return (oyFilterNode_s*)remote;
}

/** Function  oyFilterNode_CountSocketNodes
 *  @memberof oyFilterNode_s
 *  @brief    Get a neighbouring oyFilterNode_s
 *
 *  @param         node                filter node
 *  @param         pos                 connector position
 *  @param         flags               specify which number to return
 *                                     - ::OY_FILTEREDGE_FREE: count available
 *                                     - ::OY_FILTEREDGE_CONNECTED: count used
 *                                     - ::OY_FILTEREDGE_LASTTYPE: account only
 *                                       for the last connector type
 *  @return                            the number of possible edges
 *
 *  @version  Oyranos: 0.5.0
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 *  @date     2012/10/03
 */
OYAPI int OYEXPORT
               oyFilterNode_CountSocketNodes (
                                       oyFilterNode_s    * node,
                                       int                 pos,
                                       int                 flags )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;
  int n = 0,
      possible = 0,
      connected = 0;

  if(!s || !s->core || !s->api7_)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  /* sockets */
  {
    if(oyToFilterEdge_LastType_m(flags))
    {
      possible = s->api7_->sockets_last_add + 1;
    } else
    {
      possible = s->sockets_n_;
    }

    if(s->sockets)
        if(s->sockets[pos])
          connected += oyFilterPlugs_Count(s->sockets[pos]->requesting_plugs_);

    if(oyToFilterEdge_Free_m(flags))
      n = possible ? INT32_MAX : 0;
    else if(oyToFilterEdge_Connected_m(flags))
      n = connected;
    else
      n = possible;
  }

  return n;
}

/** Function  oyFilterNode_GetPlugNode
 *  @memberof oyFilterNode_s
 *  @brief    Get a neighbouring oyFilterNode_s
 *
 *  @param         node                filter node
 *  @param         pos                 connector position
 *  @return                            the node
 *
 *  @version  Oyranos: 0.5.0
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 *  @date     2012/10/03
 */
OYAPI oyFilterNode_s * OYEXPORT
               oyFilterNode_GetPlugNode ( 
                                       oyFilterNode_s    * node,
                                       int                 pos )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node,
                  * remote = 0;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  if(s->plugs[pos] && s->plugs[pos]->remote_socket_)
  {
    oyFilterPlug_s_ * s = ((oyFilterNode_s_*)node)->plugs[pos];
    oyCheckType__m( oyOBJECT_FILTER_PLUG_S, return NULL )
    remote = s->remote_socket_->node;
  }
  else
    WARNcc3_S( node, "%s: %s  plug: %d", oyFilterNode_GetRegistration( node ),
      _("Remote filter or plug not available."), pos );

  oyFilterNode_Copy((oyFilterNode_s*)remote,0);
  return (oyFilterNode_s*)remote;
}


/** Function  oyFilterNode_GetText
 *  @memberof oyFilterNode_s
 *  @brief    Serialise filter node to text
 *
 *  Serialise into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML from module
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
 *  @version Oyranos: 0.3.3
 *  @date    2011/11/22
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 */
const char * oyFilterNode_GetText    ( oyFilterNode_s    * node,
                                       oyNAME_e            name_type )
{
  const char * tmp = 0;
  char * hash_text = 0;
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  oyStructList_s * in_datas = 0,
                 * out_datas = 0;

  if(!node)
    return 0;

  if( s->core && s->core->api4_ && s->core->api4_->oyCMMFilterNode_GetText &&
      name_type == oyNAME_NAME )
  {
    hash_text = s->core->api4_->oyCMMFilterNode_GetText( node, oyNAME_NICK,
                                                   oyAllocateFunc_ );
    if(hash_text)
    {
      oyObject_SetName( s->oy_, hash_text, oyNAME_NAME );

      oySTRUCT_FREE_m( s, hash_text );
      hash_text = 0;

      hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NAME );
      return hash_text;
    }
  }

  /* 1. create hash text */
  hashTextAdd_m( "<oyFilterNode_s>\n  " );

  /* the filter text */
  hashTextAdd_m( oyFilterCore_GetText( (oyFilterCore_s*)s->core, oyNAME_NAME ) );

  /* pick all plug (input) data */
  in_datas = oyFilterNode_GetData_( s, 1 );

  /* pick all sockets (output) data */
  out_datas = oyFilterNode_GetData_( s, 0 );

  /* make a description */
  if(s->core)
    tmp = oyContextCollectData_( (oyStruct_s*)s, s->core->options_,
                                 in_datas, out_datas );
  hashTextAdd_m( tmp );

  hashTextAdd_m( "</oyFilterNode_s>\n" );


  oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );

  if(s->oy_->deallocateFunc_)
    s->oy_->deallocateFunc_( hash_text );
  hash_text = 0;

  hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NICK );

  if(in_datas && in_datas->release)   in_datas->release ( (oyStruct_s**)&in_datas );
  if(out_datas && out_datas->release) out_datas->release( (oyStruct_s**)&out_datas );

  return hash_text;
}

/** Function  oyFilterNode_NewWith
 *  @memberof oyFilterNode_s
 *  @brief    Initialise a complete filter node
 *
 *  The function creates both the oyFilterCore_s with oyFilterCore_NewWith() and
 *  oyFilterNode_s with oyFilterNode_Create().
 *
 *  @param         registration        a @ref registration string, @see module_api
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
  oyFilterCore_s * core = oyFilterCore_NewWith( registration, options, object);
  oyFilterNode_s * node = oyFilterNode_Create( registration, core, object );

  oyFilterCore_Release( &core );
  return node;
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

  oyFilterNode_s_ ** node_ = (oyFilterNode_s_**)&node;

  if(!node || !(*node_)->core || node->type_ != oyOBJECT_FILTER_NODE_S ||
     !(*node_)->api7_)
    return 0;

  object = oyObject_New ("oyFilterNode_ShowConnector-dummy");

  if(is_plug)
  {
    if((*node_)->api7_->plugs_n <= (size_t)as_pos &&
       as_pos < oyFilterNode_EdgeCount( node, 1, 0 ))
      as_pos = (*node_)->api7_->plugs_n - 1;

    if((*node_)->api7_->plugs_n > (size_t)as_pos)
    {
      if((*node_)->api7_->plugs[as_pos] && (*node_)->api7_->plugs[as_pos]->copy)
      {
        pattern = (oyConnector_s*) (*node_)->api7_->plugs[as_pos]->copy( (oyStruct_s*) (*node_)->api7_->plugs[as_pos], object );
        if(oy_debug_objects >= 0)
          oyObjectDebugMessage_( (*node_)->api7_->plugs[as_pos]->oy_, __func__,
                                 oyStructTypeToText((*node_)->api7_->plugs[as_pos]->type_) );
      } else
      {
        pattern = oyConnector_Copy( (*node_)->api7_->plugs[as_pos], object );
      }
    }
  } else {
    if((*node_)->api7_->sockets_n <= (size_t)as_pos &&
       as_pos < oyFilterNode_EdgeCount( node, 0, 0 ))
      as_pos = (*node_)->api7_->sockets_n - 1;

    if((*node_)->api7_->sockets_n > (size_t)as_pos)
    {
      if((*node_)->api7_->sockets[as_pos] && (*node_)->api7_->sockets[as_pos]->copy)
      {
        pattern = (oyConnector_s*) (*node_)->api7_->sockets[as_pos]->copy( (oyStruct_s*) (*node_)->api7_->sockets[as_pos], object );
        if(oy_debug_objects >= 0)
          oyObjectDebugMessage_( (*node_)->api7_->sockets[as_pos]->oy_, __func__,
                                 oyStructTypeToText((*node_)->api7_->sockets[as_pos]->type_) );
      } else
      {
        pattern = oyConnector_Copy( (*node_)->api7_->sockets[as_pos], object );
      }
    }
  }

  oyObject_Release( &object );

  return pattern;
}

/** Function  oyFilterNode_GetCore
 *  @memberof oyFilterNode_s
 *  @brief    Get filter core
 *
 *  @param[in,out] node                filter object
 *  @return                            the core
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/15
 *  @since    2012/09/15 (Oyranos: 0.1.10)
 */
OYAPI oyFilterCore_s *  OYEXPORT
                 oyFilterNode_GetCore( oyFilterNode_s     * node )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  oyFilterCore_Copy( (oyFilterCore_s*)s->core, 0 );
  return (oyFilterCore_s*)s->core;
}
/** Function  oyFilterNode_GetRegistration
 *  @memberof oyFilterNode_s
 *  @brief    Get filter core registration
 *
 *  @param[in,out] node                filter object
 *  @return                            the @ref registration string
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/15
 *  @since    2012/09/15 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                 oyFilterNode_GetRegistration( oyFilterNode_s     * node )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  return ((oyFilterCore_s_*)s->core)->registration_;
}
/** Function  oyFilterNode_GetRendererRegistration
 *  @memberof oyFilterNode_s
 *  @brief    Get filters registration
 *
 *  @param[in,out] node                filter object
 *  @return                            the @ref registration string
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2014/08/04
 *  @since    2014/08/04 (Oyranos: 0.9.6)
 */
OYAPI const char *  OYEXPORT
                 oyFilterNode_GetRendererRegistration
                                     ( oyFilterNode_s     * node )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  return s->api7_->registration;
}
/** Function  oyFilterNode_GetRelatives
 *  @memberof oyFilterNode_s
 *  @brief    Get filter relatives string
 *
 *  @param[in,out] node                filter object
 *  @return                            the debug string
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/04
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                 oyFilterNode_GetRelatives
                                     ( oyFilterNode_s     * node )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  return s->relatives_;
}
/** Function  oyFilterNode_GetModuleName
 *  @memberof oyFilterNode_s
 *  @brief    Get filter core module string
 *
 *  @param[in,out] node                filter object
 *  @return                            the debug string
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/04
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                 oyFilterNode_GetModuleName
                                     ( oyFilterNode_s     * node )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  return ((oyFilterCore_s_*)s->core)->api4_->id_;
}
/**
 *  @memberof oyFilterNode_s
 *  @brief    Get module data
 *
 *  The filters private data, previously requested over 
 *  oyCMMapi4_s::oyCMMFilterNode_ContextToMem() and converted to
 *  oyCMMapi4_s::context_type
 *
 *  @param[in,out] node                filter object
 *  @return                            the data
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/12/22
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
OYAPI oyPointer_s *  OYEXPORT
                 oyFilterNode_GetContext
                                     ( oyFilterNode_s     * node )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  oyPointer_Copy( s->backend_data, 0 );
  return s->backend_data;
}
/**
 *  @memberof oyFilterNode_s
 *  @brief    Set module data
 *  @internal
 *
 *  the filters private data, requested over 
 *  oyCMMapi4_s::oyCMMFilterNode_ContextToMem() and converted to
 *  oyCMMapi4_s::context_type
 *
 *  Oyranos' core provides that data. See oyFilterNode_ToBlob()
 *
 *  @param[in,out] node                filter object
 *  @return                            the data
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/12/22
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyFilterNode_SetContext
                                     ( oyFilterNode_s     * node,
                                       oyPointer_s        * data )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  oyPointer_Release( &s->backend_data );
  s->backend_data = oyPointer_Copy( data, 0 );

  return 0;
}

/**
 *  @memberof oyFilterNode_s
 *  @brief    Node context to binary blob
 *
 *  Typical a context from a CMM will be returned. It is a intermediate
 *  context from oyCMMapi4_s backend. For a CMM with a registration string
 *  including "icc_color", the context type should be "oyDL" and this function
 *  shall return a ICC device link profile.
 *  A converted context, which is suited for precessing by oyCMMapi7_s backends,
 *  is stored inside the node and can be obtained by oyFilterNode_GetContext().
 *
 *  The function is expensive, as it requests the data from the CMM without
 *  caching.
 *
 *  @param         node                node object
 *  @param         object              the optional object
 *  @return                            the data blob
 *
 *  @version Oyranos: 0.9.5
 *  @since   2014/01/25 (Oyranos: 0.9.5)
 *  @date    2014/01/25
 */
oyBlob_s * oyFilterNode_ToBlob       ( oyFilterNode_s    * node,
                                       oyObject_s          object )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;
  oyBlob_s * blob = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  if(s)
  {
    if(((oyCMMapi4_s_*)s->core->api4_)->oyCMMFilterNode_ContextToMem &&
       strlen(((oyCMMapi4_s_*)s->core->api4_)->context_type))
    {
      blob = oyBlob_New( object );
      oyFilterNode_SetContext_( s, (oyBlob_s_*)blob );
      if(oyBlob_GetSize( blob ) == 0 && oyBlob_GetPointer( blob ) == NULL)
        oyBlob_Release( &blob );
    }
  }

  return blob;
}

/** Function  oyFilterNode_GetTags
 *  @memberof oyFilterNode_s
 *  @brief    Get filter tags
 *
 *  Tags are non persistent informations. Typical they are not directly user 
 *  visible other than filter core options, which can appear inside dialogs.
 *
 *  @param[in,out] node                filter object
 *  @return                            the tags
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/15
 *  @since    2012/09/15 (Oyranos: 0.5.0)
 */
OYAPI oyOptions_s *  OYEXPORT
                 oyFilterNode_GetTags( oyFilterNode_s     * node )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  if(!s->tags)
  {
    char * t = NULL;
    s->tags = oyOptions_New( 0 );
    if(s->tags)
    {
      oyStringAddPrintf( &t, 0,0, "%s:tags", s->core->registration_ );
      oyObject_SetName( s->tags->oy_, t, oyNAME_NICK );
    }
    oyFree_m_( t );
  }

  oyOptions_Copy( s->tags, 0 );
  return s->tags;
}

/**
 *  @memberof oyFilterNode_s
 *  @brief   set backend specific runtime data
 *
 *  Runtime data can be used as context by a backend during execution. The data
 *  is typical set during oyCMMapi7_s creation. This function provides 
 *  access for a backend inside a DAC in order to change that data during 
 *  backend lifetime.
 *
 *  That data is apart from a filter object, which can have lifetime data
 *  associated through a oyFilterNode_GetContext(). A filter connector
 *  can have its processing data associated through oyFilterNode_SetData().
 *
 *  @param[in,out] obj                 the node object
 *  @param[in]     ptr                 the data needed to run the filter type
 *  @return                            error
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/23
 *  @since   2013/12/19 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
           oyFilterNode_SetBackendContext (
                                       oyFilterNode_s    * obj,
                                       oyPointer_s       * ptr )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)obj;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  return oyCMMapi7_SetBackendContext( (oyCMMapi7_s*)s->api7_, ptr );
}

/**
 *  @memberof oyFilterNode_s
 *  @brief   get backend specific runtime data
 *
 *  Backend context is used by a filter type during execution.
 *  That is apart from a filter, which can have it's object data
 *  associated through a oyFilterNode_GetContext().
 *
 *  @param[in]     obj                 the node object
 *  @return                            the context needed to run the filter type
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/19
 *  @since   2013/12/19 (Oyranos: 0.9.5)
 */
OYAPI oyPointer_s * OYEXPORT
           oyFilterNode_GetBackendContext
                                     ( oyFilterNode_s    * obj )
{
  oyFilterNode_s_ * s = (oyFilterNode_s_*)obj;

  if(!s)
    return NULL;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return NULL )

  return oyCMMapi7_GetBackendContext( (oyCMMapi7_s*)s->api7_ );
}

/* } Include "FilterNode.public_methods_definitions.c" */

