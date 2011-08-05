{% extends "Connector_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "ConnectorImaging.members.h" { */
{% include "ConnectorImaging.members.h" %}
/* } Include "ConnectorImaging.members.h" */
{% endblock %}
