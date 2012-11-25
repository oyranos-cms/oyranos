{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include "oyranos_types.h"

#include "oyFilterNode_s.h"
#include "oyPointer_s.h"

int          oyPointer_ConvertData   ( oyPointer_s       * cmm_ptr,
                                       oyPointer_s       * cmm_ptr_out,
                                       oyFilterNode_s    * node );

{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
