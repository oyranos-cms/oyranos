{% extends "Base_s.opaque.c" %}

{% block SpecificPublicMethodsDefinitions %}
/* Include "Class.public_methods_definitions.c" { */
{% include "Class.public_methods_definitions.c" %}
/* } Include "Class.public_methods_definitions.c" */
{# TODO Remove uneeded pointer in oyClass_s* #}
{% endblock %}
