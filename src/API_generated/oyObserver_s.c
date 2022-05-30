/** @file oyObserver_s.c

   [Template file inheritance graph]
   +-> oyObserver_s.template.c
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


  
#include "oyObserver_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyObserver_s_.h"
  


/** Function oyObserver_New
 *  @memberof oyObserver_s
 *  @brief   allocate a new Observer object
 */
OYAPI oyObserver_s * OYEXPORT
  oyObserver_New( oyObject_s object )
{
  oyObject_s s = object;
  oyObserver_s_ * observer = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  observer = oyObserver_New_( s );

  return (oyObserver_s*) observer;
}

/** Function  oyObserver_Copy
 *  @memberof oyObserver_s
 *  @brief    Copy or Reference a Observer object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     observer                 Observer struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyObserver_s* OYEXPORT
  oyObserver_Copy( oyObserver_s *observer, oyObject_s object )
{
  oyObserver_s_ * s = (oyObserver_s_*) observer;

  if(s)
  {
    oyCheckType__m( oyOBJECT_OBSERVER_S, return NULL )
  }
  else
    return NULL;

  s = oyObserver_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyObserver_s" );

  return (oyObserver_s*) s;
}
 
/** Function oyObserver_Release
 *  @memberof oyObserver_s
 *  @brief   release and possibly deallocate a oyObserver_s object
 *
 *  @param[in,out] observer                 Observer struct object
 */
OYAPI int OYEXPORT
  oyObserver_Release( oyObserver_s **observer )
{
  oyObserver_s_ * s = 0;

  if(!observer || !*observer)
    return 0;

  s = (oyObserver_s_*) *observer;

  oyCheckType__m( oyOBJECT_OBSERVER_S, return 1 )

  *observer = 0;

  return oyObserver_Release_( &s );
}



/* Include "Observer.public_methods_definitions.c" { */
#include "oyOption_s_.h"
#include "oyOptions_s_.h"
#include "oyStructList_s_.h"

/** Function oyObserver_SignalSend
 *  @memberof oyObserver_s
 *  @brief   send a signal to a Observer object
 *
 *  @param[in]     observer            observer
 *  @param[in]     signal_type         basic signal information
 *  @param[in]     signal_data         advanced informations
 *  @return                            0 - not matching; 1 - match, skip others
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT oyObserver_SignalSend (
                                       oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  oyObserver_s_ * s = (oyObserver_s_ *)observer;
  int result = 0;

  oyCheckType__m( oyOBJECT_OBSERVER_S, return 0 )

     /* global signal disabling */
  if(!oyToSignalBlock_m( oyObserverGetFlags() ) &&
     /* local signal disabling */
     !s->disable_ref )
    result = s->signal( observer, signal_type, signal_data ? signal_data : s->user_data );

  return result;
}

/** Function oyStruct_ObserverAdd
 *  @memberof oyObserver_s
 *  @brief   send a signal to a Observer object
 *
 *  @param[in]     model               the to be observed model
 *  @param[in]     observer            the in observation intereressted object
 *  @param[in]     user_data           additional static informations
 *  @param[in]     signalFunc          the signal handler;
 *                                     defaults to oyStructSignalForward_
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
OYAPI int  OYEXPORT oyStruct_ObserverAdd (
                                       oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc )
{
  oyObserver_s_ * s = 0,
                * obs = 0;
  int error = !model || !observer;
  oyStructList_s * list = 0;
  int n,i, found;

  if(!signalFunc)
    signalFunc = oyStructSignalForward_;

  /* add to model */
  if(!error)
    list = oyStruct_ObserverListGet_( model, OY_SIGNAL_OBSERVERS );
  if(!error && list)
  {
    found = 0;
    n = oyStructList_Count( list );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( list,
                                                  i, oyOBJECT_OBSERVER_S );
      if(observer == obs->observer && obs->signal == signalFunc)
        ++found;
    }

    /* add new oyObserver_s */
    if(found == 0)
    {
      s = (oyObserver_s_*) oyObserver_New( 0 );
      if(observer)
        s->observer = observer->copy( observer, 0 );
      s->model = model->copy( model, 0 );
      if(user_data)
        s->user_data = user_data->copy( user_data, 0 );
      s->signal = signalFunc;
      if(oy_debug_objects >= 0 || oy_debug_objects <= -2)
      {
        if(s->observer)
          oyObjectDebugMessage_( s->observer->oy_, __func__,
                                 oyStructTypeToText(s->observer->type_) );
        if(s->model)
          oyObjectDebugMessage_( s->model->oy_, __func__,
                                 oyStructTypeToText(s->model->type_) );
        if(s->user_data)
          oyObjectDebugMessage_( s->user_data->oy_, __func__,
                                 oyStructTypeToText(s->user_data->type_) );
      }

      oyStructList_MoveIn( list, (oyStruct_s**)&s, -1, 0 );
    }
  }

  /* add to observer */
  if(!error)
    list = oyStruct_ObserverListGet_( observer, OY_SIGNAL_MODELS );
  if(!error && list)
  {
    found = 0;
    n = oyStructList_Count( list );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( list,
                                                  i, oyOBJECT_OBSERVER_S );
      if(model == obs->model && obs->signal == signalFunc)
        ++found;
    }

    /* add oyObserver_s */
    if(found == 0 && !s)
    {
      s = (oyObserver_s_*)oyObserver_New( 0 );
      if(observer)
        s->observer = observer->copy( observer, 0 );
      s->model = model->copy( model, 0 );
      if(user_data)
        s->user_data = user_data->copy( user_data, 0 );
      s->signal = signalFunc;
    }

    if(s)
      oyStructList_MoveIn( list, (oyStruct_s**)&s, -1, 0 );
  }

  return error;
}

/** Function oyStruct_ObserverRemove
 *  @memberof oyObserver_s
 *  @brief   disconnect a observer from a object
 *
 *  @param[in,out] observer            the model
 *  @param[in]     model               the pattern
 *  @param[in]     signalFunc          the signal handler to remove
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT oyStruct_ObserverRemove (
                                       oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyObserver_Signal_f signalFunc )
{
  int error = !model || !observer;
  oyStructList_s * list = 0;

  if(!error && model->oy_->handles_) /* do not create a new observer list */
  {
    list = oyStruct_ObserverListGet_( model, OY_SIGNAL_OBSERVERS );
    error = oyStruct_ObserverRemove_( list, observer, 1, signalFunc );
  }
  if(!error && observer->oy_->handles_) /* do not create a new observer list */
  {
    list = oyStruct_ObserverListGet_( observer, OY_SIGNAL_MODELS );
    error = oyStruct_ObserverRemove_( list, model, 0, signalFunc );
  }

  return error;
}


/** Function oyStruct_ObserverSignal
 *  @memberof oyObserver_s
 *  @brief   send a signal to all ovservers of a model
 *
 *  @param[in]     model               the model
 *  @param[in]     signal_type         the basic signal type to emit
 *  @param[in,out] signal_data         the advanced signal information
 *  @return                            0 - no handler found; 1 - handler found;
 *                                     < 0 error or issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/27 (Oyranos: 0.1.10)
 *  @date    2009/10/27
 */
OYAPI int  OYEXPORT oyStruct_ObserverSignal (
                                       oyStruct_s        * model,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  oyObserver_s_ * obs = 0;
  int error = !model, t_err = 0;
  oyOption_s * o = 0;
  oyStructList_s * observers = 0;
  int n,i, result = 0;

  if(oyToSignalBlock_m( oyObserverGetFlags() ))
    return 0;

  if(!error)
    o = oyOptions_Find( model->oy_->handles_, OY_SIGNAL_OBSERVERS,
                        oyNAME_REGISTRATION );

  if(!o)
    return 0;

  if(!error)
  {
    observers = (oyStructList_s*)oyOption_GetStruct( o, oyOBJECT_STRUCT_LIST_S);
    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( observers,
                                                  i, oyOBJECT_OBSERVER_S );
      if(obs)
      {
        if(obs->model == model)
        {
          if(oy_debug_signals)
          {
            WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]", _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
          }
          t_err = oyObserver_SignalSend( (oyObserver_s *)obs, signal_type, signal_data );
          if(t_err)
          {
            DBG_NUM7_S( "oyObserver_SignalSend() returned %d\n\t%s %s: %s[%d]->%s[%d]",
                    t_err, _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
          }
          ++result;
        }
        else
        {
          WARNc6_S( "\n\t%s %s: %s[%d]->%s[%d]",_("found observer of wrong type"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
      } else
        WARNc3_S( "\n\t%s: oyObservers_s[%s]",_("found no observer"),
                    oySignalToString(signal_type),
                    oyStruct_TypeToText((oyStruct_s*)observers) );
    }
    oyStructList_Release( &observers );
  }

  oyOption_Release( &o );

  if(result)
    return result;
  return error;
}

/** Function oyStruct_DisableSignalSend
 *  @memberof oyObserver_s
 *  @brief   disable sending a signal to all ovservers of a model
 *
 *  @param[in]     model               the model
 *  @return                            0 - no handler found; 1 - error
 *                                     < 0 error or issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT oyStruct_DisableSignalSend (
                                       oyStruct_s        * model )
{
  oyObserver_s_ * obs = 0;
  int error = !model;
  oyOption_s * o = 0;
  oyStructList_s * observers = 0;
  int n,i;

  if(!error)
    o = oyOptions_Find( model->oy_->handles_, OY_SIGNAL_OBSERVERS,
                        oyNAME_REGISTRATION );

  if(!o)
    return 0;

  if(!error)
  {
    observers = (oyStructList_s*)oyOption_GetStruct( o, oyOBJECT_STRUCT_LIST_S );
    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( observers,
                                                  i, oyOBJECT_OBSERVER_S );
      if(obs)
        ++obs->disable_ref;
    }
  }

  return error;
}

/** Function oyStruct_EnableSignalSend
 *  @memberof oyObserver_s
 *  @brief   reenable sending a signal to all ovservers of a model
 *
 *  @param[in]     model               the model
 *  @return                            0 - no handler found; 1 - error
 *                                     < 0 error or issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT oyStruct_EnableSignalSend (
                                       oyStruct_s        * model )
{
  oyObserver_s_ * obs = 0;
  int error = !model;
  oyOption_s * o = 0;
  oyStructList_s * observers = 0;
  int n,i;

  if(!error)
    o = oyOptions_Find( model->oy_->handles_, OY_SIGNAL_OBSERVERS,
                        oyNAME_REGISTRATION );

  if(!o)
    return 0;

  if(!error)
  {
    observers = (oyStructList_s*)oyOption_GetStruct( o, oyOBJECT_STRUCT_LIST_S );

    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( observers,
                                                  i, oyOBJECT_OBSERVER_S );
      if(obs)
        --obs->disable_ref;
      if(obs && obs->disable_ref < 0)
      {
        oyOption_s_ * o_ = (oyOption_s_*)o;
        obs->disable_ref = 0;
        WARNcc3_S( model, "%s: %s[%d]", _("Already enabled"),
                   oyStruct_GetText( (oyStruct_s*)obs, oyNAME_NAME, 1),
                   oyObject_GetId( o_->value->oy_struct->oy_ ) );
      }
    }
  }

  return error;
}

/** Function oyStruct_ObserversCopy
 *  @memberof oyObserver_s
 *  @brief   copy a objects observers and models to a different object
 *
 *  oyStruct_ObserversCopy is a convenience function for
 *  oyStruct_ObserverCopyModel() and oyStruct_ObserverCopyObserver().
 *
 *  @param[in,out] object              the object
 *  @param[in]     pattern             the pattern to copy from
 *  @param[in]     flags               0x02 omit observers; 0x04 omit models
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT oyStruct_ObserversCopy (
                                       oyStruct_s        * object,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags )
{
  int error = 0;

  if(!error && !(flags & 0x04))
    error = oyStruct_ObserverCopyModel(object, pattern, flags);
  if(!error && !(flags & 0x02))
    error = oyStruct_ObserverCopyObserver(object, pattern, flags);

  return error;
}

/** Function oyStruct_ObserverCopyModel
 *  @memberof oyObserver_s
 *  @brief   copy a models observers to a different model
 *
 *  The observers are copied from pattern with object type to a model with the
 *  same object type.
 *
 *  @param[in,out] model               the model
 *  @param[in]     pattern             the pattern to copy from
 *  @param[in]     flags               0x01 omit observer idendity check
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT oyStruct_ObserverCopyModel (
                                       oyStruct_s        * model,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags )
{
  oyObserver_s_ * obs = 0;
  int error = !model;
  oyStructList_s * observers = 0;
  int n,i;

  if( !model || !pattern )
    return 0;

  observers = oyStruct_ObserverListGet_( pattern, OY_SIGNAL_OBSERVERS );

  if(!error)
  {
    n = oyStructList_Count( observers );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( observers,
                                                  i, oyOBJECT_OBSERVER_S );
      if(obs &&
         (!(flags & 0x01) || obs->model == pattern))
      {
        if(oy_debug_signals)
        {
          WARNc5_S( "\n\tCopy %s: %s[%d]->%s[%d]", _("Signal"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        error = oyStruct_ObserverAdd( (oyStruct_s*)model, obs->observer,
                                obs->user_data, obs->signal );

      }
      else
      {
        if(obs)
        {
          WARNc5_S( "\n\tCopy: %s: %s[%d]->%s[%d]",
                    _("found observer of wrong type"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        else
        {
          WARNc2_S( "\n\toyObservers_s %s ",_("found no observer"),
                    oyStruct_TypeToText((oyStruct_s*)observers) );
        }
      }
    }
  }

  return error;

}

/** Function oyStruct_ObserverCopyObserver
 *  @memberof oyObserver_s
 *  @brief   copy a observers models to a different observer
 *
 *  The models are copied from pattern with object type to a object with the
 *  same object type.
 *
 *  @param[in,out] observer            the observer
 *  @param[in]     pattern             the pattern to copy from
 *  @param[in]     flags               0x01 omit model idendity check
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/02 (Oyranos: 0.1.10)
 *  @date    2009/11/02
 */
OYAPI int  OYEXPORT oyStruct_ObserverCopyObserver (
                                       oyStruct_s        * observer,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags )
{
  oyObserver_s_ * obs = 0;
  int error = !observer;
  oyStructList_s * list = 0;
  int n,i;

  if( !observer || !pattern )
    return 0;

  list = oyStruct_ObserverListGet_( pattern, OY_SIGNAL_MODELS );

  if(!error)
  {
    n = oyStructList_Count( list );
    for(i = 0; i < n; ++i)
    {
      obs = (oyObserver_s_*) oyStructList_GetType( list,
                                                  i, oyOBJECT_OBSERVER_S );
      if(obs &&
         (!(flags & 0x01) || obs->observer == pattern))
      {
        if(oy_debug_signals)
        {
          WARNc5_S( "\n\tCopy %s: %s[%d]->%s[%d]", _("Signal"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        error = oyStruct_ObserverAdd( obs->model, observer,
                                obs->user_data, obs->signal );

      }
      else
      {
        if(obs)
        {
          WARNc5_S( "\n\tCopy: %s: %s[%d]->%s[%d]",
                    _("found observer of wrong type"),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );
        }
        else
        {
          WARNc2_S( "\n\t%s ",_("found no observer"),
                    oyStruct_TypeToText( (oyStruct_s*)list) );
        }
      }
    }
  }

  return error;

}

/**
 *  Function oyStruct_IsObserved
 *  @memberof oyObserver_s
 *  @brief   return object observation status
 *
 *  Check if a object is observed by others.
 *
 *  @param         model               model to ask
 *  @param         observer            which observes that model
 *  @return                            true or false
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/10/28
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 */
OYAPI int  OYEXPORT oyStruct_IsObserved (
                                       oyStruct_s        * model,
                                       oyStruct_s        * observer )
{
  int observed = 0;
  int i,n = 0;
  oyOption_s_ * o = 0;
  oyOptions_s_ * handles = (oyOptions_s_*) model->oy_->handles_;
  int error = 0;

  if(handles)
    n = oyStructList_Count( handles->list_ );
  for(i = 0; i < n; ++i)
  {
    o = (oyOption_s_*) oyStructList_Get_( (oyStructList_s_*)(handles->list_), i );
    if( oyStrcmp_( o->registration, OY_SIGNAL_OBSERVERS ) == 0)
    {
      if(observer)
      {
        oyStructList_s * observers = 0;
        int j_n,j;

        observers = (oyStructList_s*)oyOption_GetStruct( (oyOption_s*) o,
                                                       oyOBJECT_STRUCT_LIST_S );

        if(!error)
        {
          j_n = oyStructList_Count( observers );
          for(j = 0; j < j_n; ++j)
          {
            oyObserver_s_ * obs;
            obs = (oyObserver_s_*) oyStructList_GetType( observers,
                                                   j, oyOBJECT_OBSERVER_S );
            if(obs && obs->observer == observer)
            {
              observed = 1;
              break;
            }
          }
        }
      } else
      {
        observed = 1;
        break;
      }
    }
  }

  return observed;
}

/**
 *  Function oyStruct_ObservationCount
 *  @memberof oyObserver_s
 *  @brief   return the number of object<->model references
 *
 *  The function lists by default (0) models and observers. Both
 *  types are individually selectable in the flags.
 *
 *  @param         object              which observes a model
 *  @param         flags               select:
 *                                     - 0 : all
 *                                     - 1 : show observers count
 *                                     - 2 : show models count
 *  @return                            count
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/10/04
 *  @since   2018/09/29 (Oyranos: 0.9.7)
 */
OYAPI int  OYEXPORT oyStruct_ObservationCount (
                                       oyStruct_s        * object,
                                       uint32_t            flags )
{
  int observed = 0;
  int i,n = 0;
  oyOption_s_ * o = NULL;
  oyOptions_s_ * handles = NULL;
  int error = 0;

  if(object && object->oy_)
    handles = (oyOptions_s_*) object->oy_->handles_;

  if(handles)
    n = oyStructList_Count( handles->list_ );
  for(i = 0; i < n; ++i)
  {
    
    o = (oyOption_s_*) oyStructList_Get_( (oyStructList_s_*)(handles->list_), i );
    if( (!flags || flags & 0x02) && oyStrcmp_( o->registration, OY_SIGNAL_MODELS ) == 0)
    {
      if(object)
      {
        oyStructList_s * models = 0;
        int j_n,j;

        models = (oyStructList_s*)oyOption_GetStruct( (oyOption_s*) o,
                                                       oyOBJECT_STRUCT_LIST_S );

        if(!error)
        {
          j_n = oyStructList_Count( models );
          for(j = 0; j < j_n; ++j)
          {
            oyObserver_s_ * obs;
            obs = (oyObserver_s_*) oyStructList_GetType( models,
                                                   j, oyOBJECT_OBSERVER_S );
            if(obs && obs->observer == object)
              ++observed;
          }
        }
        if(models && models->release)
          models->release( (oyStruct_s**)&models );
      }
    }

    if( (!flags || flags & 0x01) && oyStrcmp_( o->registration, OY_SIGNAL_OBSERVERS ) == 0)
    {
      if(object)
      {
        oyStructList_s * observers = 0;
        int j_n,j;

        observers = (oyStructList_s*)oyOption_GetStruct( (oyOption_s*) o,
                                                       oyOBJECT_STRUCT_LIST_S );

        if(!error)
        {
          j_n = oyStructList_Count( observers );
          for(j = 0; j < j_n; ++j)
          {
            oyObserver_s_ * obs;
            obs = (oyObserver_s_*) oyStructList_GetType( observers,
                                                   j, oyOBJECT_OBSERVER_S );
            if(obs && obs->model == object)
              ++observed;
          }
        }
        if(observers && observers->release)
          observers->release( (oyStruct_s**)&observers );
      }
    }
  } 
  return observed;
}
  

const char *       oySignalToString  ( oySIGNAL_e          signal_type )
{
  const char * text = "unknown";
  switch(signal_type)
  {

  case oySIGNAL_OK:
       text = "oySIGNAL_OK"; break;
  case oySIGNAL_CONNECTED:             /**< connection established */
       text = "oySIGNAL_CONNECTED: connection established"; break;
  case oySIGNAL_RELEASED:              /**< released the connection */
       text = "oySIGNAL_RELEASED: released the connection"; break;
  case oySIGNAL_DATA_CHANGED:          /**< call to update image views */
       text = "oySIGNAL_DATA_CHANGED: call to update data views"; break;
  case oySIGNAL_STORAGE_CHANGED:       /**< new data accessors */
       text = "oySIGNAL_STORAGE_CHANGED: new data accessors"; break;
  case oySIGNAL_INCOMPATIBLE_DATA:     /**< can not process image */
       text = "oySIGNAL_INCOMPATIBLE_DATA: can not process data"; break;
  case oySIGNAL_INCOMPATIBLE_OPTION:   /**< can not handle option */
       text = "oySIGNAL_INCOMPATIBLE_OPTION: can not handle option"; break;
  case oySIGNAL_INCOMPATIBLE_CONTEXT:  /**< can not handle profile */
       text = "oySIGNAL_INCOMPATIBLE_CONTEXT: can not handle context"; break;
  case oySIGNAL_INCOMPLETE_GRAPH:      /**< can not completely process */
       text = "oySIGNAL_INCOMPLETE_GRAPH: can not completely process"; break;
  case oySIGNAL_VISITED:               /**< just accessed */
       text = "oySIGNAL_VISITED: just accessed"; break;
  case oySIGNAL_USER1:
       text = "oySIGNAL_USER1"; break;
  case oySIGNAL_USER2:
       text = "oySIGNAL_USER2"; break;
  case oySIGNAL_USER3:                 /**< more signal types are possible */
       text = "oySIGNAL_USER2"; break;
  }
  return text;
}


uint32_t   oy_observer_flags = 0;
/** Function oyObserverGetFlags
 *  @memberof oyObserver_s
 *  @brief   get global flags for Observation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI uint32_t OYEXPORT oyObserverGetFlags ( void )
{
  return oy_observer_flags;
}

/** Function oyObserverSetFlags
 *  @memberof oyObserver_s
 *  @brief   set global flags for Observation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI int  OYEXPORT oyObserverSetFlags (
                                       uint32_t            flags )
{
  oy_observer_flags = flags;
  return 0;
}

/* } Include "Observer.public_methods_definitions.c" */

