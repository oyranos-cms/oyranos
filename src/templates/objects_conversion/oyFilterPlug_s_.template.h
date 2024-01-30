{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyFilterNode_s.h"
#include "oyFilterSocket_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterPlug.members.h" { */
{% include "FilterPlug.members.h" %}
/* } Include "FilterPlug.members.h" */
{% endblock %}
