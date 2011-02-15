{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

#include <oyranos_object.h>

#include "{{ class.name }}.h"

/** @internal
 *  @struct   {{ class.Name }}
 *  @brief    {{ class.brief }}
 *  @ingroup  {{ class.group }}
 *  @extends  {{ class.parentName }}
 */
struct {{ class.name }} {
{% block BaseMembers %}
/* Include "Struct.members.h" { */
{% include "Struct.members.h" %}
/* } Include "Struct.members.h" */
{% endblock %}
{% block ChildMembers %}{% endblock %}
};

{% block GeneralPrivateMethodsDeclarations %}
{{ class.name }}
  oy{{ class.baseName }}_New_( oyObject_s object );
{{ class.name }}
  oy{{ class.baseName }}_Copy_( {{ class.name }} {{ class.baseName|lower }}, oyObject_s object);
{{ class.name }}
  oy{{ class.baseName }}_Copy__( {{ class.name }} {{ class.baseName|lower }}, oyObject_s object);
int
  oy{{ class.baseName }}_Release_( {{ class.name }}* {{ class.baseName|lower }} );
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}
/* Include "{{ class.private_methods_declarations_h }}" { */
{% include class.private_methods_declarations_h %}
/* } Include "{{ class.private_methods_declarations_h }}" */
{% endblock %}
{# TODO Remove uneeded pointer in oyClass_s* #}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
