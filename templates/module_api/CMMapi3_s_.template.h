{% extends "CMMapi_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi3.members.h" { */
{% include "CMMapi3.members.h" %}
/* } Include "CMMapi3.members.h" */
{% endblock %}
