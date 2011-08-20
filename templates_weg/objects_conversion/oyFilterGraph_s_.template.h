{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyFilterNodes_s.h"
#include "oyFilterPlugs_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterGraph.members.h" { */
{% include "FilterGraph.members.h" %}
/* } Include "FilterGraph.members.h" */
{% endblock %}
