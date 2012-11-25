{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "StructList.members.h" { */
{% include "StructList.members.h" %}
/* } Include "StructList.members.h" */
{% endblock %}
