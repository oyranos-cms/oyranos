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
/* Include "Struct.members.h" { */
{% include "Struct.members.h" %}
/* } Include "Struct.members.h" */
{% block ChildMembers %}{% endblock %}
};

{% block GeneralPrivateMethodsDeclarations %}
{{ class.name }}
  oy{{ class.baseName }}_New_( oyObject_s_ object );
{{ class.name }}
  oy{{ class.baseName }}_Copy_( {{ class.name }} {{ class.baseName|lower }}, oyObject_s_ object);
{{ class.name }}
  oy{{ class.baseName }}_Copy__( {{ class.name }} {{ class.baseName|lower }}, oyObject_s_ object);
int
  oy{{ class.baseName }}_Release_( {{ class.name }}* {{ class.baseName|lower }} );
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}{% endblock %}

#endif /* OY_{{ class.baseName|upper }}_S__H */
