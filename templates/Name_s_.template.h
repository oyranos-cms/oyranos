{% extends "Base_s_.h" %}

{% block ChildMembers %}
{{ block.super }}
  /* {{ class.privName }} members start */
{% include "Name.members.h" %}
  /* {{ class.privName }} members stop */
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}
{% include "Name.private_methods_declarations.h" %}
{% endblock %}
