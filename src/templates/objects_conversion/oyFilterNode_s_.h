{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterNode.members.h" { */
{% include "FilterNode.members.h" %}
/* } Include "FilterNode.members.h" */
{% endblock %}
