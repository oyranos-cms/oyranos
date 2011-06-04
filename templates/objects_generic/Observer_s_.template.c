{% include "source_file_header.txt" %}

#include "oyranos_object_internal.h"

#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"

#include "oyObject_s.h"
#include "oyOption_s_.h"


/* Include "{{ class.private_methods_definitions_c }}" { */
{% include class.private_methods_definitions_c %}
/* } Include "{{ class.private_methods_definitions_c }}" */
