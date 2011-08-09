{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

#include <oyranos_object.h>

#include "{{ class.name }}.h"
#include "oyStructList_s.h"

/* Include "{{ class.private_methods_declarations_h }}" { */
{% include class.private_methods_declarations_h %}
/* } Include "{{ class.private_methods_declarations_h }}" */

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
