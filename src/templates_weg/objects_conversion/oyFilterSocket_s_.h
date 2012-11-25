{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterSocket.members.h" { */
{% include "FilterSocket.members.h" %}
/* } Include "FilterSocket.members.h" */
{% endblock %}
