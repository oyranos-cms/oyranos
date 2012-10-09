{% extends "oyCMMapiFilter_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi7.members.h" { */
{% include "CMMapi7.members.h" %}
/* } Include "CMMapi7.members.h" */
{% endblock %}
