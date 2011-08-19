{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterGraph.members.h" { */
{% include "FilterGraph.members.h" %}
/* } Include "FilterGraph.members.h" */
{% endblock %}
