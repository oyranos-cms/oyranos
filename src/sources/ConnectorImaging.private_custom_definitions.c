/** Function    oyConnectorImaging_Release__Members
 *  @memberof   oyConnectorImaging_s
 *  @brief      Custom ConnectorImaging destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  connectorimaging  the ConnectorImaging object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConnectorImaging_Release__Members( oyConnectorImaging_s_ * connectorimaging )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &connectorimaging->member );
   */

  if(connectorimaging->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = connectorimaging->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( connectorimaging->member );
     */
    if(connectorimaging->data_types)
      deallocateFunc( connectorimaging->data_types );
    connectorimaging->data_types = 0;
    connectorimaging->data_types_n = 0;

    if(connectorimaging->channel_types)
      deallocateFunc( connectorimaging->channel_types );
    connectorimaging->channel_types = 0;
    connectorimaging->channel_types_n = 0;
  }
}

/** Function    oyConnectorImaging_Init__Members
 *  @memberof   oyConnectorImaging_s
 *  @brief      Custom ConnectorImaging constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  connectorimaging  the ConnectorImaging object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnectorImaging_Init__Members( oyConnectorImaging_s_ * connectorimaging )
{
  connectorimaging->max_color_offset = -1;
  connectorimaging->min_channels_count = -1;
  connectorimaging->max_channels_count = -1;
  connectorimaging->min_color_count = -1;
  connectorimaging->max_color_count = -1;
  connectorimaging->can_planar = -1;
  connectorimaging->can_interwoven = -1;
  connectorimaging->can_swap = -1;
  connectorimaging->can_swap_bytes = -1;
  connectorimaging->can_revert = -1;
  connectorimaging->can_premultiplied_alpha = -1;
  connectorimaging->can_nonpremultiplied_alpha = -1;
  connectorimaging->can_subpixel = -1;

  return 0;
}

/** Function    oyConnectorImaging_Copy__Members
 *  @memberof   oyConnectorImaging_s
 *  @brief      Custom ConnectorImaging copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConnectorImaging_s_ input object
 *  @param[out]  dst  the output oyConnectorImaging_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnectorImaging_Copy__Members( oyConnectorImaging_s_ * dst, oyConnectorImaging_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  if(src->data_types_n)
  {
    dst->data_types = allocateFunc_( src->data_types_n * sizeof(oyDATATYPE_e) );
    error = !dst->data_types;
    if(!error)
      error = !memcpy( dst->data_types, src->data_types,
                       src->data_types_n * sizeof(oyDATATYPE_e) );
    if(error <= 0)
      dst->data_types_n = src->data_types_n;
  }

  dst->max_color_offset = src->max_color_offset;
  dst->min_channels_count = src->min_channels_count;
  dst->max_channels_count = src->max_channels_count;
  dst->min_color_count = src->min_color_count;
  dst->max_color_count = src->max_color_count;
  dst->can_planar = src->can_planar;
  dst->can_interwoven = src->can_interwoven;
  dst->can_swap = src->can_swap;
  dst->can_swap_bytes = src->can_swap_bytes;
  dst->can_revert = src->can_revert;
  dst->can_premultiplied_alpha = src->can_premultiplied_alpha;
  dst->can_nonpremultiplied_alpha = src->can_nonpremultiplied_alpha;
  dst->can_subpixel = src->can_subpixel;

  if(src->channel_types_n)
  {
    int n = src->channel_types_n;

    dst->channel_types = allocateFunc_( n * sizeof(oyCHANNELTYPE_e) );
    error = !dst->channel_types;
    if(!error)
      error = !memcpy( dst->channel_types, src->channel_types,
                       n * sizeof(oyCHANNELTYPE_e) );
    if(error <= 0)
      dst->channel_types_n = n;
  }

  dst->is_mandatory = src->is_mandatory;

  return error;
}
