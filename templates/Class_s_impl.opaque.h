{% extends "Base_s_impl.opaque.h" %}

{% block ChildMembers %}
{{ block.super }}
{% include "Class.members.h" %}
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}
{% include "Class.private_methods_declarations.h" %}
{# TODO Remove uneeded pointer in oyClass_s* #}
{% endblock %}
