/** @file oyObserver_s_.h

   [Template file inheritance graph]
   +-> oyObserver_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_OBSERVER_S__H
#define OY_OBSERVER_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyObserverPriv_m( var ) ((oyObserver_s_*) (var))

typedef struct oyObserver_s_ oyObserver_s_;

  
#include <oyranos_object.h>
  

#include "oyObserver_s.h"

/* Include "Observer.private.h" { */

/* } Include "Observer.private.h" */

/** @internal
 *  @struct   oyObserver_s_
 *  @brief       Oyranos object observers
 *  @ingroup  objects_generic
 *  @extends  oyStruct_s
 */
struct oyObserver_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Observer.members.h" { */
  /** a reference to the observing object */
  oyStruct_s         * observer;
  /** a reference to the to be observed model */
  oyStruct_s         * model;
  /** optional data; If no other user data is available this data will be
   *  passed with the signal. */
  oyStruct_s         * user_data;
  oyObserver_Signal_f  signal;         /**< observers signaling function */
  int                  disable_ref;    /**< disable signals reference counter
                                            == 0 -> enabled; otherwise not */

/* } Include "Observer.members.h" */

};


oyObserver_s_*
  oyObserver_New_( oyObject_s object );
oyObserver_s_*
  oyObserver_Copy_( oyObserver_s_ *observer, oyObject_s object);
oyObserver_s_*
  oyObserver_Copy__( oyObserver_s_ *observer, oyObject_s object);
int
  oyObserver_Release_( oyObserver_s_ **observer );



/* Include "Observer.private_methods_declarations.h" { */
#include "oyStructList_s.h"
int      oyStructSignalForward_      ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );
oyStructList_s * oyStruct_ObserverListGet_(
                                       oyStruct_s        * obj,
                                       const char        * reg );
int        oyStruct_ObserverRemove_  ( oyStructList_s    * list,
                                       oyStruct_s        * obj,
                                       int                 observer,
                                       oyObserver_Signal_f signalFunc );

/* } Include "Observer.private_methods_declarations.h" */



void oyObserver_Release__Members( oyObserver_s_ * observer );
int oyObserver_Init__Members( oyObserver_s_ * observer );
int oyObserver_Copy__Members( oyObserver_s_ * dst, oyObserver_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OBSERVER_S__H */
