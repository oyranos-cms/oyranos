{% extends "Base_s.opaque.h" %}

{% block doxygenPublicClass %}
{% include "Class.dox" %}
{% endblock %}
{% block SpecificPublicMethodsDeclarations %}
{% include "Class.public_methods_declarations.h" %}
{# TODO Remove uneeded pointer in oyClass_s* #}
{% endblock SpecificPublicMethodsDeclarations %}
