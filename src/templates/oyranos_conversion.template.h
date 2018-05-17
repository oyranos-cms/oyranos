{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include "oyranos_types.h"
#include "oyOptions_s.h"

/** \addtogroup objects_conversion
 *  @brief Directed Acyclic Graph handling, caching and data processing
 *
 *  @{ *//* objects_conversion */

/** @} *//* objects_conversion */ 

uint32_t     oyICCProfileSelectionFlagsFromOptions (
                                       const char        * db_base_key,
                                       const char        * base_pattern,
                                       oyOptions_s       * options,
                                       int                 select_core );

{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
