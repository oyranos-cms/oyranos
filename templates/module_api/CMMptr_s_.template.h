{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMptr.members.h" { */
{% include "CMMptr.members.h" %}
/* } Include "CMMptr.members.h" */
{% endblock %}
