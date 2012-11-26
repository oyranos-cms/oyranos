{% extends "oyCMMapiFilter_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi10.members.h" { */
{% include "CMMapi10.members.h" %}
/* } Include "CMMapi10.members.h" */
{% endblock %}
