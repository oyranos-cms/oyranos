{% include "source_file_header.txt" %}

#ifndef OY_{{ class_name|upper }}_H
#define OY_{{ class_name|upper }}_H

#include "oy{{ class_name|cut:"_s" }}.h"

struct {% block ClassName %}oy{{ class_name }}{% endblock %} {
  {% block BaseMembers %}
  /* Struct base class start */
  oyOBJECT_e           type_;          /**< @private struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */
  /* Struct base class stop */
  {% endblock %}
  {% block ChildMembers %}{% endblock %}
};

#endif /* OY_{{ class_name|upper }}_H */
