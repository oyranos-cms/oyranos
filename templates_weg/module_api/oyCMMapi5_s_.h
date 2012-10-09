{% extends "oyCMMapi_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi5.members.h" { */
{% include "CMMapi5.members.h" %}
/* } Include "CMMapi5.members.h" */
{% endblock %}
