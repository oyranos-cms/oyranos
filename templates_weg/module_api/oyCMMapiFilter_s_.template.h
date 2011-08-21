{% extends "oyCMMapi_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMapi5_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapiFilter.members.h" { */
{% include "CMMapiFilter.members.h" %}
/* } Include "CMMapiFilter.members.h" */
{% endblock %}
