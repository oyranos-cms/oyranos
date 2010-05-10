#ifndef OY_{{ class_name|upper }}_H
#define OY_{{ class_name|upper }}_H

typedef struct {
  {% block BaseMembers %}
  /* Struct base class start */
  oyOBJECT_e           type_;          /**< @private struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */
  /* Struct base class stop */
  {% endblock %}
  {% block ChildMembers %}{% endblock %}
} {% block ClassName %}oy{{ class_name }}_s{% endblock %};

{% block ClassMethods %}
const char * oy{{ class_name }}_GetText( oyStruct_s * obj, oyNAME_e name_type, uint32_t flags );
{% endblock %}

#endif /* OY_{{ class_name|upper }}_H */
