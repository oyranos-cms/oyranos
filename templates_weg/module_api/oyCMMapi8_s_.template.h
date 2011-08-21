{% extends "oyCMMapiFilter_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyConfigs_s.h"
#include "oyCMMapi5_s.h"
#include "oyCMMui_s.h"

#include "oyCMMapi_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi8.members.h" { */
{% include "CMMapi8.members.h" %}
/* } Include "CMMapi8.members.h" */
{% endblock %}
