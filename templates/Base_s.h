{% extends "oyStruct_s.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyStruct_s.h"
{% endblock %}

{% block GeneralPublicMethodsDeclarations %}
OYAPI {{ class.name }}* OYEXPORT
  oy{{ class.baseName }}_New( oyObject_s object );
OYAPI {{ class.name }}* OYEXPORT
  oy{{ class.baseName }}_Copy( {{ class.name }} *{{ class.baseName|lower }}, oyObject_s obj );
OYAPI int OYEXPORT
  oy{{ class.baseName }}_Release( {{ class.name }} **{{ class.baseName|lower }} );
{% endblock GeneralPublicMethodsDeclarations %}
