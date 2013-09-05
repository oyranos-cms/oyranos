{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oy{{ class.listBaseName }}_s.h"
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
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_Clear           ( {{ class.name }}       * list );
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_Sort            ( {{ class.name }}       * list,
                                       int32_t           * rank_list );
{% endblock GeneralPublicMethodsDeclarations %}

