{% extends "CMMapiFilter_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMobjectType_s.h"
#include "oyCMMui_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi9.members.h" { */
{% include "CMMapi9.members.h" %}
/* } Include "CMMapi9.members.h" */
{% endblock %}
