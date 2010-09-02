{% extends "CMMapi_s_.template.h" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <oyranos_icc.h>
{% endblock %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyStructList_s.h"
#include "oyProfileTag_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "CMMapi3.members.h" { */
{% include "CMMapi3.members.h" %}
/* } Include "CMMapi3.members.h" */
{% endblock %}
