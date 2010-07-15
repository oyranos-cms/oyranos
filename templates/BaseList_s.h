{% extends "Base_s.h" %}

{% block ProjectIncludes %}
{{ block.super }}
#include "{{ class.content.name }}.h"
{% endblock %}

{% block GeneralPublicMethodsDeclarations %}
{{ block.super }}
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_MoveIn          ( {{ class.name }}       * list,
                                       {{ class.listOf }}       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_ReleaseAt       ( {{ class.name }}       * list,
                                       int                 pos );
OYAPI {{ class.listOf }} * OYEXPORT
           oy{{ class.baseName }}_Get             ( {{ class.name }}       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_Count           ( {{ class.name }}       * list );
{% endblock GeneralPublicMethodsDeclarations %}
