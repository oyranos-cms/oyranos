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
int oyFilterSocket_Init__Members( oyFilterSocket_s_ * filtersocket OY_UNUSED )
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
