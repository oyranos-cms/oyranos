{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Observer.members.h" { */
{% include "Observer.members.h" %}
/* } Include "Observer.members.h" */
{% endblock %}
