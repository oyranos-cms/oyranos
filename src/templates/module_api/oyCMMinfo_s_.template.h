{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
#include "oyranos_module.h"
#include "oyCMMapi_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMinfo.members.h" { */
{% include "CMMinfo.members.h" %}
/* } Include "CMMinfo.members.h" */
{% endblock %}
