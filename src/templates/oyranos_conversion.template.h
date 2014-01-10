{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include "oyranos_types.h"

/** \addtogroup objects_conversion DAG and Image processing API
 *  @brief Directed Acyclic Graph handling, caching and data processing
 *
 *  @{
 */

/** @} *//* objects_conversion */ 

{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
