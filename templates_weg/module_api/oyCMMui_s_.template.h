{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module_internal.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMui.members.h" { */
{% include "CMMui.members.h" %}
/* } Include "CMMui.members.h" */
{% endblock %}
