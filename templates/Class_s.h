{% extends "Base_s.h" %}

{% block doxygenPublicClass %}
{% include "Class.dox" %}
{% endblock %}
{% block SpecificPublicMethodsDeclarations %}
{% include "Class.public_methods_declarations.h" %}
{% endblock SpecificPublicMethodsDeclarations %}
