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
  connectorimaging->max_colour_offset = -1;
  connectorimaging->min_channels_count = -1;
  connectorimaging->max_channels_count = -1;
  connectorimaging->min_colour_count = -1;
  connectorimaging->max_colour_count = -1;
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
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return error;
}
