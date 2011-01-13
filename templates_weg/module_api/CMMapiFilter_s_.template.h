{% extends "CMMapi_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapiFilter.members.h" { */
{% include "CMMapiFilter.members.h" %}
/* } Include "CMMapiFilter.members.h" */
{% endblock %}
