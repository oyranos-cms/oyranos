{% extends "oyCMMapiFilter_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyPointer_s.h"
#include "oyFilterNode_s.h"
#include "oyCMMapi6_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi6.members.h" { */
{% include "CMMapi6.members.h" %}
/* } Include "CMMapi6.members.h" */
{% endblock %}
