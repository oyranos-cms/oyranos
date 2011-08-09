{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMui.members.h" { */
{% include "CMMui.members.h" %}
/* } Include "CMMui.members.h" */
{% endblock %}
