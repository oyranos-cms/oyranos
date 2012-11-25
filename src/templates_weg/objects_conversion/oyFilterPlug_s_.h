{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterPlug.members.h" { */
{% include "FilterPlug.members.h" %}
/* } Include "FilterPlug.members.h" */
{% endblock %}
