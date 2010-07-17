{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S__H
#define OY_{{ class.baseName|upper }}_S__H

#include <oyranos_object.h>

#include "{{ class.name }}.h"
#include "oyStructList_s.h"

int      oyStructSignalForward_      ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );
oyObserver_s * oyObserver_Copy_
                                     ( oyObserver_s      * obj,
                                       oyObject_s          object );
oyStructList_s * oyStruct_ObserverListGet_(
                                       oyStruct_s        * obj,
                                       const char        * reg );
int        oyStruct_ObserverRemove_  ( oyStructList_s    * list,
                                       oyStruct_s        * obj,
                                       int                 observer,
                                       oyObserver_Signal_f signalFunc );
#endif /* OY_{{ class.baseName|upper }}_S__H */
