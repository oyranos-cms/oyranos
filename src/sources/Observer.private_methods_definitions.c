#include "oyOption_s_.h"

/** Function  oyStructSignalForward_
 *  @memberof oyObserver_s
 *  @brief    Observe all list members
 *  @internal
 *
 *
 *  This function is useful to forward signals and fill holes in a chain.
 *  Implements oyObserver_Signal_f.
 *
 *  @code
    error = oyStruct_ObserverAdd( (oyStruct_s*)model, (oyStruct_s*)observer,
                                  0, oyStructSignalForward_ );
    @endcode
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
int      oyStructSignalForward_      ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  oyObserver_s_ * obs = (oyObserver_s_ *)observer;

  if(oy_debug_signals)
    WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

  if(obs && obs->model &&
     obs->observer && obs->observer->type_ > oyOBJECT_NONE)
    handled = oyStruct_ObserverSignal( obs->observer,
                                       signal_type, signal_data );

  return handled;
}

/** Function  oyStruct_ObserverListGet_
 *  @memberof oyObserver_s
 *  @brief    Get the desired list of oyObserver_s'
 *  @internal
 *
 *  @param[in]     obj                 the object to look in for the list
 *  @param[in]     reg                 the selector for the list
 *  @return                            the observers
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
oyStructList_s * oyStruct_ObserverListGet_(
                                       oyStruct_s        * obj,
                                       const char        * reg )
{
  oyOption_s * o = 0;
  oyOption_s_ * o_ = 0;
  int error = 0;
  oyStructList_s * list = 0;

  o = oyOptions_Find( obj->oy_->handles_, reg, oyNAME_PATTERN );

  if(!o)
  {
    list = oyStructList_Create( 0, oyStruct_GetText(obj, oyNAME_NAME, 0), 0 );
    o = oyOption_FromRegistration( reg, obj->oy_ );
    error = oyOption_MoveInStruct( o, (oyStruct_s**)&list );
    if(!error)
    {
      if(!obj->oy_->handles_)
        obj->oy_->handles_ = oyOptions_New( 0 );
      error = oyOptions_MoveIn( obj->oy_->handles_, &o, -1);
      o = oyOptions_Find( obj->oy_->handles_, reg, oyNAME_PATTERN );
    }
  }

  o_ = oyOptionPriv_m(o);

  if(!error && o_ && o_->value_type == oyVAL_STRUCT && o_->value)
  {
    if(o_->value->oy_struct &&
       o_->value->oy_struct->type_ == oyOBJECT_STRUCT_LIST_S)
      list = (oyStructList_s*)o_->value->oy_struct;
    else
    {
      WARNcc3_S( obj, "%s: %s %s", _("found list of wrong type"),
                 reg,
                 oyStruct_TypeToText( o_->value->oy_struct ) );
    }

    oyOption_Release( &o );
  }
  return list;
}

/** Function  oyStruct_ObserverRemove_
 *  @memberof oyObserver_s
 *  @brief    Remove a observer from the observer or model internal list
 *  @internal
 *
 *  @param[in,out] list                the reference list
 *  @param[in]     obj                 comparision object
 *  @param[in]     observer            1 - remove observer; 0 - remove model
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
int        oyStruct_ObserverRemove_  ( oyStructList_s    * list,
                                       oyStruct_s        * obj,
                                       int                 observer,
                                       oyObserver_Signal_f signalFunc )
{
  int error = 0;
  oyObserver_s_ * obs = 0;
  int n,i;
  if(list)
  {
    n = oyStructList_Count( list );
    for(i = n-1; i >= 0; --i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( list,
                                                  i, oyOBJECT_OBSERVER_S );

      if(obs &&
         ((observer && obj == obs->observer) ||
          (!observer && obj == obs->model)) &&
          (!signalFunc || obs->signal == signalFunc))
        oyStructList_ReleaseAt( list, i );
    }
  }
  return error;
}
