
/** @file oyObserver_s_.h

   [Template file inheritance graph]
   +-- oyObserver_s_.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2014 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2014/01/11
 */


#ifndef OY_OBSERVER_S__H
#define OY_OBSERVER_S__H

#include <oyranos_object.h>

#include "oyObserver_s.h"
#include "oyStructList_s.h"

/* Include "Observer.private_methods_declarations.h" { */
int      oyStructSignalForward_      ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );
oyObserver_s * oyObserver_Copy_      ( oyObserver_s      * obj,
                                       oyObject_s          object );
oyStructList_s * oyStruct_ObserverListGet_(
                                       oyStruct_s        * obj,
                                       const char        * reg );
int        oyStruct_ObserverRemove_  ( oyStructList_s    * list,
                                       oyStruct_s        * obj,
                                       int                 observer,
                                       oyObserver_Signal_f signalFunc );

/* } Include "Observer.private_methods_declarations.h" */

#endif /* OY_OBSERVER_S__H */
