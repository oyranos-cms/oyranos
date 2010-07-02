{% extends "Base_s.opaque.h" %}

{% block doxygenPublicClass %}
/* Include "Class.dox" { */
{% include "Class.dox" %}
/* } Include "Class.dox" */
{% endblock %}
{% block SpecificPublicMethodsDeclarations %}
/* Include "Class.public_methods_declarations.h" { */
{% include "Class.public_methods_declarations.h" %}
/* } Include "Class.public_methods_declarations.h" */
{# TODO Remove uneeded pointer in oyClass_s* #}
{% endblock SpecificPublicMethodsDeclarations %}
