{% extends "Base_s_impl.opaque.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Class.members.h" { */
{% include "Class.members.h" %}
/* } Include "Class.members.h" */
{% endblock %}
