{% extends "CMMapiFilter_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMui_s.h"
#include "oyCMMapi5_s.h"
#include "oyFilterNode_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi4.members.h" { */
{% include "CMMapi4.members.h" %}
/* } Include "CMMapi4.members.h" */
{% endblock %}
