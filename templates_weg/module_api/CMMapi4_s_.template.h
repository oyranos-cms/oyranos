{% extends "CMMapiFilter_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMapi5_s.h"
#include "oyFilterNode_s.h"
#include "oyCMMui_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi4.members.h" { */
{% include "CMMapi4.members.h" %}
/* } Include "CMMapi4.members.h" */
{% endblock %}
