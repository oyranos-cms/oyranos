{% include "source_file_header.txt" %}

#include "{{ file_name|cut:".c" }}.h"

{% block GeneralPublicMethodsDefinitions %}{% endblock %}

{% block SpecificPublicMethodsDefinitions %}{% endblock %}
