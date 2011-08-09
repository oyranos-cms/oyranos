{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyFilterNode_s.h"
#include "oyFilterPlugs_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterSocket.members.h" { */
{% include "FilterSocket.members.h" %}
/* } Include "FilterSocket.members.h" */
{% endblock %}
