{% extends "Struct_s.template.h" %}

{% block GeneralPublicMethodsDeclarations %}
OYAPI {{ class_name }}* OYEXPORT
  oy{{ class_base_name }}_New( oyObject_s object );
OYAPI {{ class_name }}* OYEXPORT
  oy{{ class_base_name }}_Copy( {{ class_name }} *{{ class_base_name|lower }}, oyObject_s obj );
OYAPI int OYEXPORT
  oy{{ class_nase_name }}_Release( {{ class_name }} **{{ class_base_name|lower }} );
{% endblock GeneralPublicMethodsDeclarations %}

{% block SpecificPublicMethodsDeclarations %}{% endblock SpecificPublicMethodsDeclarations %}
