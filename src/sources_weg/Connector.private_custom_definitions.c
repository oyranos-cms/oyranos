/** Function    oyConnector_Release__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  connector  the Connector object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConnector_Release__Members( oyConnector_s_ * connector )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &connector->member );
   */

  if(connector->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = connector->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( connector->member );
     */
    if(connector->connector_type) {
      deallocateFunc( connector->connector_type );
      connector->connector_type = 0;
    }
  }
}

/** Function    oyConnector_Init__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  connector  the Connector object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnector_Init__Members( oyConnector_s_ * connector )
{
  connector->is_plug = -1;

  return 0;
}

/** Function    oyConnector_Copy__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConnector_s_ input object
 *  @param[out]  dst  the output oyConnector_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnector_Copy__Members( oyConnector_s_ * dst, oyConnector_s_ * src)
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
  error = oyObject_CopyNames( dst->oy_, src->oy_ );

  dst->connector_type = oyStringCopy_( src->connector_type, allocateFunc_ );
  dst->is_plug = src->is_plug;

  return error;
}
