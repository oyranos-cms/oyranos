{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "PixelAccess.members.h" { */
{% include "PixelAccess.members.h" %}
/* } Include "PixelAccess.members.h" */
{% endblock %}
