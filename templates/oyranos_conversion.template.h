{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

/** @enum    oyCONNECTOR_EVENT_e
 *  @ingroup objects_conversion
 *  @brief   Connector events types
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyCONNECTOR_EVENT_OK,                /**< kind of ping */
  oyCONNECTOR_EVENT_CONNECTED,         /**< connection established */
  oyCONNECTOR_EVENT_RELEASED,          /**< released the connection */
  oyCONNECTOR_EVENT_DATA_CHANGED,      /**< call to update image views */
  oyCONNECTOR_EVENT_STORAGE_CHANGED,   /**< new data accessors */
  oyCONNECTOR_EVENT_INCOMPATIBLE_DATA, /**< can not process image */
  oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION,/**< can not handle option */
  oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT,/**< can not handle profile */
  oyCONNECTOR_EVENT_INCOMPLETE_GRAPH   /**< can not completely process */
} oyCONNECTOR_EVENT_e;

{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
