{% extends "oyCMMapiFilter_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi8.members.h" { */
{% include "CMMapi8.members.h" %}
/* } Include "CMMapi8.members.h" */
{% endblock %}
