/** Function    oyObserver_Release__Members
 *  @memberof   oyObserver_s
 *  @brief      Custom Observer destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  observer  the Observer object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyObserver_Release__Members( oyObserver_s_ * observer )
{
  int error = 0;
  /* Deallocate members here
   * E.g: oyXXX_Release( &observer->member );
   */
  if((observer->model || observer->observer))
  {
    if(observer->model == NULL || observer->model->oy_ == NULL)
      WARNcc_S(observer->model, "incomplete")
    if(observer->observer == NULL || observer->observer->oy_ == NULL)
      WARNcc_S(observer->observer, "incomplete")
  }
  error = oyStruct_ObserverRemove( observer->model && observer->model->oy_ ? observer->model : NULL, observer->observer && observer->observer->oy_ ? observer->observer : NULL, NULL );
  if(error)
  {
    fprintf( stderr, OY_DBG_FORMAT_ "!!!ERROR: Observer[%d] gives error: %d  model: %s observer: %s\n", OY_DBG_ARGS_, observer->oy_->id_, error, observer->model && observer->model->oy_ ? "yes" : "----", observer->observer && observer->observer->oy_ ? "yes" : "----" );
    if(oy_debug_objects != -1 || oy_debug)
      OY_BACKTRACE_PRINT
  }

  if(observer->observer)
  {
    oyObject_UnRef( observer->observer->oy_ );
    /*observer->observer->release( &observer->observer );*/
    if(oy_debug_objects == observer->observer->oy_->id_)
      fprintf( stderr, OY_DBG_FORMAT_ "stop observation: %d <- %d\n", OY_DBG_ARGS_, observer->observer->oy_->id_, observer->oy_->id_ );
    observer->observer = NULL;
  }
  if(observer->model)
  {
    oyObject_UnRef( observer->model->oy_ );
    /*observer->model->release( &observer->model );*/
    if(oy_debug_objects == observer->model->oy_->id_)
      fprintf( stderr, OY_DBG_FORMAT_ "stop observing model: %d <- %d\n", OY_DBG_ARGS_, observer->model->oy_->id_, observer->oy_->id_ );
    observer->model = NULL;
  }
  if(observer->user_data)
  { observer->user_data->release( &observer->user_data ); observer->user_data = 0; }

  if(oy_debug_objects == -3)
  {
    char * text = NULL;
    OY_BACKTRACE_STRING(7)
    oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ );
    fprintf( stderr, "%s\n", text ? text : __func__ );
    oyFree_m_( text )
  }

  if(observer->oy_->deallocateFunc_)
  {
    //oyDeAlloc_f deallocateFunc = observer->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( observer->member );
     */
  }
}

/** Function    oyObserver_Init__Members
 *  @memberof   oyObserver_s
 *  @brief      Custom Observer constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  observer  the Observer object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyObserver_Init__Members( oyObserver_s_ * observer OY_UNUSED )
{
  return 0;
}

/** Function    oyObserver_Copy__Members
 *  @memberof   oyObserver_s
 *  @brief      Custom Observer copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyObserver_s_ input object
 *  @param[out]  dst  the output oyObserver_s_ object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
int oyObserver_Copy__Members( oyObserver_s_ * dst, oyObserver_s_ * src)
{
  int error = 0;
  //oyAlloc_f allocateFunc_ = 0;
  //oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  //allocateFunc_ = dst->oy_->allocateFunc_;
  //deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  {
    dst->observer = src->observer->copy( src->observer, dst->oy_ );
    dst->model = src->model->copy( src->model, dst->oy_ );
    dst->user_data = src->user_data->copy( src->user_data, dst->oy_ );
    if(oy_debug_objects >= 0)
    {
      if(dst->observer)
        oyObjectDebugMessage_( dst->observer->oy_, __func__,
                               oyStructTypeToText(dst->observer->type_) );
      if(dst->model)
        oyObjectDebugMessage_( dst->model->oy_, __func__,
                               oyStructTypeToText(dst->model->type_) );
      if(dst->user_data)
        oyObjectDebugMessage_( dst->user_data->oy_, __func__,
                               oyStructTypeToText(dst->user_data->type_) );
    }
    dst->disable_ref = src->disable_ref;
  }

  return error;
}
