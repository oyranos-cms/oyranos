{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
{% include "Class.members.h" %}
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}
{% include "Class.private_methods_declarations.h" %}
{% endblock %}
