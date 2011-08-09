{% extends "oyConnector_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module.h"
#include "oyranos_image.h"
#include "oyCMMapi5_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "ConnectorImaging.members.h" { */
{% include "ConnectorImaging.members.h" %}
/* } Include "ConnectorImaging.members.h" */
{% endblock %}
