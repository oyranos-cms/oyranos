{% extends "oyCMMapiFilter_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi9.members.h" { */
{% include "CMMapi9.members.h" %}
/* } Include "CMMapi9.members.h" */
{% endblock %}
