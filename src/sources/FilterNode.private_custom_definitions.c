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
          fprintf(stderr, "!!!ERROR: node[%d]->plug[%d] can not be released with refs: %d\n", cid, filternode->plugs[i]->oy_->id_, oyObject_GetRefCount(filternode->plugs[i]->oy_));
        oyFilterPlug_Release( (oyFilterPlug_s **)&filternode->plugs[i] );
      }
  }

  if( filternode->backend_data && filternode->backend_data->release )
    filternode->backend_data->release( (oyStruct_s**) & filternode->backend_data );
  filternode->backend_data = 0;
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
