{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Hash.members.h" { */
{% include "Hash.members.h" %}
/* } Include "Hash.members.h" */
{% endblock %}
