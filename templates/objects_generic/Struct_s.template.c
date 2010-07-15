{% include "source_file_header.txt" %}

#include "{{ class.name }}.h"
{% block IncludePrivateHeader %}{% endblock %}
#include "oyranos_object_internal.h"

{% block GeneralPublicMethodsDefinitions %}{% endblock %}

{% block SpecificPublicMethodsDefinitions %}
/* Include "{{ class.public_methods_definitions_c }}" { */
{% include class.public_methods_definitions_c %}
/* } Include "{{ class.public_methods_definitions_c }}" */
{% endblock %}
