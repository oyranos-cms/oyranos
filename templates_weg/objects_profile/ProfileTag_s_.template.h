{% extends "Base_s_.h" %}

{%block LocalIncludeFiles %}
{{ block.super }}
#include <icc34.h>
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "ProfileTag.members.h" { */
{% include "ProfileTag.members.h" %}
/* } Include "ProfileTag.members.h" */
{% endblock %}
