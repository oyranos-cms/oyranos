{% extends "oyStruct_s.template.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyStruct_s.h"
{% endblock %}

{% block GeneralPublicMethodsDeclarations %}
OYAPI {{ class.name }}* OYEXPORT
  oy{{ class.baseName }}_New( oyObject_s object );
/** @memberof {{ class.name }}
 *  @brief    Copy or Reference a {{ class.baseName }} object
 *
 *  The macro is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     {{ class.baseName|lower }}                 {{ class.baseName }} struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
#define oy{{ class.baseName }}_Copy({{ class.baseName|lower }},object) oy{{ class.baseName }}_Copy_x({{ class.baseName|lower }},object); if(oy_debug_objects) oyObjectDebugMessage_( {{ class.baseName|lower }}?{{ class.baseName|lower }}->oy_:NULL, __func__, "{{ class.name }}" );
OYAPI {{ class.name }}* OYEXPORT
  oy{{ class.baseName }}_Copy_x( {{ class.name }} *{{ class.baseName|lower }}, oyObject_s obj );
OYAPI int OYEXPORT
  oy{{ class.baseName }}_Release( {{ class.name }} **{{ class.baseName|lower }} );
{% endblock GeneralPublicMethodsDeclarations %}
