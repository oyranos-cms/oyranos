{% extends "oyCMMapiFilter_s_.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyConnector_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi7.members.h" { */
{% include "CMMapi7.members.h" %}
/* } Include "CMMapi7.members.h" */
{% endblock %}
