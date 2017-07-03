{% include "source_file_header.txt" %}

#include <math.h>

#include <oyranos_cache.h>
#include <oyranos_object_internal.h>

#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"

{% block SpecificPrivateMethodsDefinitions %}
/* Include "{{ class.private_methods_definitions_c }}" { */
{% include class.private_methods_definitions_c %}
/* } Include "{{ class.private_methods_definitions_c }}" */
{% endblock SpecificPrivateMethodsDefinitions %}
