{% extends "oyCMMapiFilter_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi6.members.h" { */
{% include "CMMapi6.members.h" %}
/* } Include "CMMapi6.members.h" */
{% endblock %}
