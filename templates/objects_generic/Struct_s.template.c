{% include "source_file_header.txt" %}

{% block GlobalIncludeFiles %}{% endblock %}  
#include "{{ class.name }}.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"
{% block LocalIncludeFiles %}{% endblock %}  

{% block GeneralPublicMethodsDefinitions %}{% endblock %}

{% block SpecificPublicMethodsDefinitions %}
/* Include "{{ class.public_methods_definitions_c }}" { */
{% include class.public_methods_definitions_c %}
/* } Include "{{ class.public_methods_definitions_c }}" */
{% endblock %}
