{% extends "Base_s_impl.opaque.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Class.members.h" { */
{% include "Class.members.h" %}
/* } Include "Class.members.h" */
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}
/* Include "Class.private_methods_declarations.h" { */
{% include "Class.private_methods_declarations.h" %}
/* } Include "Class.private_methods_declarations.h" */
{# TODO Remove uneeded pointer in oyClass_s* #}
{% endblock %}
