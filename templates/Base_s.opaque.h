{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S_H
#define OY_{{ class.baseName|upper }}_S_H

{% include "cpp_begin.h" %}

#include <oyranos_object.h>

{% block doxygenPublicClass %}
/* Include "{{ class.dox }}" { */
{% include class.dox %}
/* } Include "{{ class.dox }}" */
{% endblock %}
struct {{ class.baseName }};

typedef struct {{ class.baseName }}* {{ class.name }};

{% block GeneralPublicMethodsDeclarations %}
OYAPI {{ class.name }} OYEXPORT
  oy{{ class.baseName }}_New( oyObject_s object );
OYAPI {{ class.name }} OYEXPORT
  oy{{ class.baseName }}_Copy( {{ class.name }} {{ class.baseName|lower }}, oyObject_s obj );
OYAPI int OYEXPORT
  oy{{ class.baseName }}_Release( {{ class.name }} *{{ class.baseName|lower }} );
{% endblock %}

{% block SpecificPublicMethodsDeclarations %}
/* Include "{{ class.public_methods_declarations_h }}" { */
{% include class.public_methods_declarations_h %}
/* } Include "{{ class.public_methods_declarations_h }}" */
{# -->TODO<-- Remove uneeded pointer in oyClass_s* #}
{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class.baseName|upper }}_S_H */
