{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Pointer.members.h" { */
{% include "Pointer.members.h" %}
/* } Include "Pointer.members.h" */
{% endblock %}
