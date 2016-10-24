{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Config.members.h" { */
{% include "Config.members.h" %}
/* } Include "Config.members.h" */
{% endblock %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMapi8_s_.h"
#include "oyCMMapiFilter_s_.h"
#include "oyCMMapiFilters_s.h"
#include "oyOptions_s_.h"

#include "oyranos_module_internal.h"
{% endblock %}
