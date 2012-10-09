{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi.members.h" { */
{% include "CMMapi.members.h" %}
/* } Include "CMMapi.members.h" */
{% endblock %}
