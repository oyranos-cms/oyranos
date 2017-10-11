/** @internal
 +  Function    oyConfig_Release__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConfig_Release__Members( oyConfig_s_ * config )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &config->member );
   */

  oyOptions_Release( &config->db );
  oyOptions_Release( &config->backend_core );
  oyOptions_Release( &config->data );

  if(config->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = config->oy_->deallocateFunc_;

    if(deallocateFunc)
    { 
      int i = 0;
      if(config->rank_map)
      {
        while(config->rank_map[i].key)
          deallocateFunc( config->rank_map[i++].key );
        deallocateFunc( config->rank_map );
      }
      if(config->registration)
        deallocateFunc( config->registration ); 
    }
  }
}

/** @internal
 +  Function    oyConfig_Init__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Init__Members( oyConfig_s_ * config )
{
  config->db = oyOptions_New( 0 );
  if(config->db)
    oyObject_SetName( config->db->oy_, "db", oyNAME_NICK );
  config->backend_core = oyOptions_New( 0 );
  if(config->backend_core)
    oyObject_SetName( config->backend_core->oy_, "backend_core", oyNAME_NICK );
  config->data = oyOptions_New( 0 );
  if(config->data)
    oyObject_SetName( config->data->oy_, "data", oyNAME_NICK );

  return 0;
}

/** @internal
 +  Function    oyConfig_Copy__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConfig_s_ input object
 *  @param[out]  dst  the output oyConfig_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Copy__Members( oyConfig_s_ * dst, oyConfig_s_ * src)
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
  dst->db = oyOptions_Copy( src->db, dst->oy_ );
  dst->backend_core = oyOptions_Copy( src->backend_core, dst->oy_ );
  dst->data = oyOptions_Copy( src->data, dst->oy_ );
  error = !memcpy( dst->version, src->version, 3*sizeof(int) );

  dst->rank_map = oyRankMapCopy( src->rank_map, allocateFunc_ );

  return error;
}
