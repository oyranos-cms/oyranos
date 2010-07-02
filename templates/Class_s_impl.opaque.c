{% extends "Base_s_impl.opaque.c" %}

{% block SpecificPrivateMethodsDefinitions %}
/* Include "Class.private_methods_definitions.c" { */
{% include "Class.private_methods_definitions.c" %}
/* } Include "Class.private_methods_definitions.c" */
{# TODO Remove uneeded pointer in oyClass_s* #}
{% endblock SpecificPrivateMethodsDefinitions %}
