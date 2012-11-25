{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMinfo.members.h" { */
{% include "CMMinfo.members.h" %}
/* } Include "CMMinfo.members.h" */
{% endblock %}
