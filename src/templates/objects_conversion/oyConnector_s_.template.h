{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Connector.members.h" { */
{% include "Connector.members.h" %}
/* } Include "Connector.members.h" */
{% endblock %}
