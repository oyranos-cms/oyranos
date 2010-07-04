{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S__H
#define OY_{{ class.baseName|upper }}_S__H

#include <oyranos_object.h>

#include "{{ class.name }}.h"

/** @internal
 *  @struct   {{ class.Name }}
 *  @brief    {{ class.brief }}
 *  @ingroup  {{ class.group }}
 *  @extends  {{ class.parentName }}
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
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

#endif /* OY_{{ class.baseName|upper }}_S__H */
