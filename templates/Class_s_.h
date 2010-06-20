{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
  /* {{ class.privName }} members start */
{% include "Class.members.h" %}
  /* {{ class.privName }} members stop */
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}
{% include "Class.private_methods_declarations.h" %}
{% endblock %}
