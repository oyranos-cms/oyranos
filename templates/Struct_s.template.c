{% include "source_file_header.txt" %}

#include "{{ class.name }}.h"
{% block IncludePrivateHeader %}{% endblock %}

{% block GeneralPublicMethodsDefinitions %}{% endblock %}

{% block SpecificPublicMethodsDefinitions %}
{% include "Struct_s.public_methods_definitions.c" %}
{% endblock %}
