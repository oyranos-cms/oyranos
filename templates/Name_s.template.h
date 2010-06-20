{% extends "Base_s.h" %}

{% block doxygenPublicClass %}
{% include "Name.dox" %}
{% endblock %}
{% block SpecificPublicMethodsDeclarations %}
{% include "Name.public_methods_declarations.h" %}
{% endblock SpecificPublicMethodsDeclarations %}
