{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Config.members.h" { */
{% include "Config.members.h" %}
/* } Include "Config.members.h" */
{% endblock %}
