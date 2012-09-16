{% extends "oyCMMapi_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMapiFilter_s.h"
#include "oyCMMapi_s_.h"
#include "oyFilterSocket_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi5.members.h" { */
{% include "CMMapi5.members.h" %}
/* } Include "CMMapi5.members.h" */
{% endblock %}
