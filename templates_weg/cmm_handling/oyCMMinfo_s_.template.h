{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
#include "oyranos_module.h"
#include "oyCMMapi_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMInfo.members.h" { */
{% include "CMMInfo.members.h" %}
/* } Include "CMMInfo.members.h" */
{% endblock %}
