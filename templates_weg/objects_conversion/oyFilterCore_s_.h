{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterCore.members.h" { */
{% include "FilterCore.members.h" %}
/* } Include "FilterCore.members.h" */
{% endblock %}
