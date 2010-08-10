{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
#include "oyCMMapi_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMInfo.members.h" { */
{% include "CMMInfo.members.h" %}
/* } Include "CMMInfo.members.h" */
{% endblock %}
