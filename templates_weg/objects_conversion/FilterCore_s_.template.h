{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterCore.members.h" { */
{% include "FilterCore.members.h" %}
/* } Include "FilterCore.members.h" */
{% endblock %}
