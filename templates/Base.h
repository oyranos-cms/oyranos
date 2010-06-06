{% extends "Struct.template.h" %}

{% block ClassMethods %}
oy{{ class_name }} oy{{ class_name }}_New( oyObject_s object );
oy{{ class_name }} oy{{ class_name }}_Copy( oy{{ class_name }} {{ class_name|lower }}, oyObject_s obj );
int oy{{ class_name }}_Release( oy{{ class_name }}* {{ class_name|lower }} );
{% endblock %}
