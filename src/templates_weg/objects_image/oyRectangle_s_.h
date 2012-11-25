{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Rectangle.members.h" { */
{% include "Rectangle.members.h" %}
/* } Include "Rectangle.members.h" */
{% endblock %}
