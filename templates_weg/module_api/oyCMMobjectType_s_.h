{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMobjectType.members.h" { */
{% include "CMMobjectType.members.h" %}
/* } Include "CMMobjectType.members.h" */
{% endblock %}
