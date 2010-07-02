{% extends "Base_s.h" %}

{% block doxygenPublicClass %}
/* Include "Class.dox" { */
{% include "Class.dox" %}
/* } Include "Class.dox" */
{% endblock %}
{% block SpecificPublicMethodsDeclarations %}
/* Include "Class.public_methods_declarations.h" { */
{% include "Class.public_methods_declarations.h" %}
/* } Include "Class.public_methods_declarations.h" */
{% endblock SpecificPublicMethodsDeclarations %}
