{% extends "oyCMMapi_s.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyPointer_s.h"
{% endblock %}

{% block GeneralPublicMethodsDeclarations %}
{{ block.super }}
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_SetBackendContext          ( {{ class.name }}       * api,
                                       oyPointer_s       * ptr );
OYAPI oyPointer_s * OYEXPORT
           oy{{ class.baseName }}_GetBackendContext          ( {{ class.name }}       * api );
{% endblock GeneralPublicMethodsDeclarations %}

