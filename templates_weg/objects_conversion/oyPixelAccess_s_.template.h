{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyFilterGraph_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "PixelAccess.members.h" { */
{% include "PixelAccess.members.h" %}
/* } Include "PixelAccess.members.h" */
{% endblock %}
