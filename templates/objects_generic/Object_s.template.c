{% include "source_file_header.txt" %}

#include "oyranos_object_internal.h"

#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"

{% block SpecificPublicMethodsDefinitions %}
/* Include "{{ class.public_methods_definitions_c }}" { */
{% include class.public_methods_definitions_c %}
/* } Include "{{ class.public_methods_definitions_c }}" */
{% endblock %}
