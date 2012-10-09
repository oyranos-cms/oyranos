{% extends "oyCMMapiFilter_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi4.members.h" { */
{% include "CMMapi4.members.h" %}
/* } Include "CMMapi4.members.h" */
{% endblock %}
