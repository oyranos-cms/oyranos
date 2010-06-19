{% include "source_file_header.txt" %}

#include "{{ class.name }}.h"
{% block IncludePrivateHeader %}{% endblock %}

{% block GeneralPublicMethodsDefinitions %}{% endblock %}

{% block SpecificPublicMethodsDefinitions %}{% endblock %}
