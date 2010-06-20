{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S__H
#define OY_{{ class.baseName|upper }}_S__H

typedef struct {{ class.privName }} {{ class.privName }};

/** @internal
 *  @struct   {{ class.privName }}
 *  @brief    {% block doxy_brief %}Oyranos {{ class.baseName|lower }} structure{% endblock %}
 *  @ingroup  {{ class.group }}
 *  @extends  {{ class.parentName }}
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
struct {{ class.privName }} {
  /* Struct base class start */
{% include "Struct_s.members.h" %}
  /* Struct base class stop */
  {% block ChildMembers %}{% endblock %}
};

{% block GeneralPrivateMethodsDeclarations %}
{{ class.privName }}*
  oy{{ class.baseName }}_New_( oyObject_s_ object );
{{ class.privName }}*
  oy{{ class.baseName }}_Copy_( {{ class.privName }} *obj, oyObject_s_ object);
int
  oy{{ class.baseName }}_Release_( {{ class.privName }} **obj );
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}{% endblock %}

#endif /* OY_{{ class.baseName|upper }}_S__H */
