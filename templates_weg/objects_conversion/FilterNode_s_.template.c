{% extends "Base_s_.c" %}

{% block altConstructor %}oyFilterNode_Create( {{ class.baseName|lower }}->core, object );{% endblock %}
