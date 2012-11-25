{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Blob.members.h" { */
{% include "Blob.members.h" %}
/* } Include "Blob.members.h" */
{% endblock %}
