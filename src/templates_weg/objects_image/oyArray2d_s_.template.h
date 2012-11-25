{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyRectangle_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Array2d.members.h" { */
{% include "Array2d.members.h" %}
/* } Include "Array2d.members.h" */
{% endblock %}
