{% extends "oyList_s_.template.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "NamedColors.members.h" { */
{% include "NamedColors.members.h" %}
/* } Include "NamedColors.members.h" */
{% endblock %}
