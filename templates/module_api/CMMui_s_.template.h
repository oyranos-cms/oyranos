{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMui.members.h" { */
{% include "CMMui.members.h" %}
/* } Include "CMMui.members.h" */
{% endblock %}
