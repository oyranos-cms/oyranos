{% include "source_file_header.txt" %}

#ifndef OY_{{ class_base_name|upper }}_S__H
#define OY_{{ class_base_name|upper }}_S__H

typedef struct {{ class_name }} {{ class_name }};

struct {{ class_name }} {
  /* Struct base class start */
  oyOBJECT_e           type_;          /**< @private struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */
  /* Struct base class stop */
  {% block ChildMembers %}{% endblock %}
};

{% block GeneralPrivateMethodsDeclaration %}
{{ class_name }}*
  oy{{ class_base_name }}_New_( oyObject_s_ object );
{{ class_name }}*
  oy{{ class_base_name }}_Copy_( {{ class_name }} *obj, oyObject_s_ object);
int
  oy{{ class_base_name }}_Release_( {{ class_name }} **obj );
{% endblock %}

{% block SpecificPrivateMethodsDeclaration %}{% endblock %}

#endif /* OY_{{ class_base_name|upper }}_S__H */
