{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include <oyranos_object.h>
#include <oyranos_definitions.h>

#include "oyStruct_s.h"

#define OY_SIGNAL_BLOCK                0x01 /**< do not send new signals */
#define oyToSignalBlock_m(r)           ((r)&1)
#define OY_SIGNAL_OBSERVERS OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "oyStructList_s/observers"
/*  The models list of a observing object is just a clone of the oyObserver_s
 *  object added to the model. */
#define OY_SIGNAL_MODELS OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "oyStructList_s/models"

/** @enum    oySIGNAL_e
 *  @brief   observer signals
 *  @ingroup objects_generic
 *
 *  The signal types are similiar to the graph event enum oyCONNECTOR_EVENT_e.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
typedef enum {
  oySIGNAL_OK,
  oySIGNAL_CONNECTED,                  /**< connection established */
  oySIGNAL_RELEASED,                   /**< released the connection */
  oySIGNAL_DATA_CHANGED,               /**< call to update image views */
  oySIGNAL_STORAGE_CHANGED,            /**< new data accessors */
  oySIGNAL_INCOMPATIBLE_DATA,          /**< can not process image */
  oySIGNAL_INCOMPATIBLE_OPTION,        /**< can not handle option */
  oySIGNAL_INCOMPATIBLE_CONTEXT,       /**< can not handle profile */
  oySIGNAL_USER1, 
  oySIGNAL_USER2, 
  oySIGNAL_USER3                       /**< more signal types are possible */
} oySIGNAL_e;

const char *       oySignalToString  ( oySIGNAL_e          signal_type );

typedef struct {{ class.name }} {{ class.name }};

typedef  int      (*oyObserver_Signal_f) (
                                       oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );

{% block doxygenPublicClass %}
/* Include "{{ class.dox }}" { */
{% include class.dox %}
/* } Include "{{ class.dox }}" */
{% endblock %}
struct {{ class.name }} {
  oyOBJECT_e           type_;          /**< @private internal struct type oyOBJECT_OBSERVER_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer            dummy;          /**< keep to zero */

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
};

/* {{ class.name }} common object functions { */
OYAPI oyObserver_s * OYEXPORT
           oyObserver_New            ( oyObject_s          object );
OYAPI oyObserver_s * OYEXPORT
           oyObserver_Copy           ( oyObserver_s      * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
           oyObserver_Release        ( oyObserver_s     ** obj );
/* } {{ class.name }} common object functions */

/* {{ class.name }} object specific functions { */
OYAPI int  OYEXPORT
           oyObserver_SignalSend     ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * user_data );
OYAPI int  OYEXPORT
           oyStruct_ObserverAdd      ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
           oyStruct_ObserverRemove   ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
           oyStruct_ObserverSignal   ( oyStruct_s        * model,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );
OYAPI int  OYEXPORT
           oyStruct_DisableSignalSend( oyStruct_s        * model );
OYAPI int  OYEXPORT
           oyStruct_EnableSignalSend ( oyStruct_s        * model );
OYAPI int  OYEXPORT
           oyStruct_ObserversCopy    ( oyStruct_s        * object,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyModel( oyStruct_s        * model,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyObserver (
                                       oyStruct_s        * observer,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_IsObserved       ( oyStruct_s        * model,
                                       oyStruct_s        * observer );
OYAPI uint32_t OYEXPORT
           oyObserverFlagsGet        ( void );
OYAPI int  OYEXPORT
           oyObserverFlagsSet        ( uint32_t            flags );
/* } {{ class.name }} object specific functions */

{% include "cpp_end.h" %}
#endif /* {{ file_name|underscores|upper|tr:". _" }} */
