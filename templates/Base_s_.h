{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S__H
#define OY_{{ class.baseName|upper }}_S__H

typedef struct {{ class.name }} {{ class.name }};

/** @struct   {{ class.name }}
 *  @brief    {% block doxy_brief %}Oyranos {{ class.baseName|lower }} structure{% endblock %}
 *  @ingroup  {% block doxy_group %}objects_generic{% endblock %}
 *  @extends  {% block doxy_extends %}oyStruct_s{% endblock %}
 *
 *  {% block doxy_details %}{% endblock %}
 *
 *   @version Oyranos: 0.1.10
 *   @since   2007/10/00 (Oyranos: 0.1.8)
 *   @date    2009/03/01
 */
struct {{ class.name }} {
  /* Struct base class start */
{% include Struct_s.members.h %}
  /* Struct base class stop */
  {% block ChildMembers %}{% endblock %}
};

{% block GeneralPrivateMethodsDeclarations %}
{{ class.name }}*
  oy{{ class.baseName }}_New_( oyObject_s_ object );
{{ class.name }}*
  oy{{ class.baseName }}_Copy_( {{ class.name }} *obj, oyObject_s_ object);
int
  oy{{ class.baseName }}_Release_( {{ class.name }} **obj );
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}{% endblock %}

#endif /* OY_{{ class.baseName|upper }}_S__H */
