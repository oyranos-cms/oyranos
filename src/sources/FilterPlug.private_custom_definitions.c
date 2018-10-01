/** Function    oyFilterPlug_Release__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filterplug  the FilterPlug object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterPlug_Release__Members( oyFilterPlug_s_ * filterplug )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filterplug->member );
   */
  oyFilterNode_Release( (oyFilterNode_s**)&filterplug->node );

  oyFilterSocket_Callback( (oyFilterPlug_s*)filterplug, oyCONNECTOR_EVENT_RELEASED );

  oyConnector_Release( &filterplug->pattern );

  if(filterplug->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filterplug->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filterplug->member );
     */
    if(filterplug->relatives_)
      deallocateFunc( filterplug->relatives_ );
    filterplug->relatives_ = 0;
  }
}

/** Function    oyFilterPlug_Init__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filterplug  the FilterPlug object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlug_Init__Members( oyFilterPlug_s_ * filterplug OY_UNUSED )
{
  return 0;
}

/** Function    oyFilterPlug_Copy__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterPlug_s_ input object
 *  @param[out]  dst  the output oyFilterPlug_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlug_Copy__Members( oyFilterPlug_s_ * dst, oyFilterPlug_s_ * src)
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

  return error;
}
