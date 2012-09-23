{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyFilterGraph_s_.h"
#include "oyFilterPlugs_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterNode_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterSocket.members.h" { */
{% include "FilterSocket.members.h" %}
/* } Include "FilterSocket.members.h" */
{% endblock %}
