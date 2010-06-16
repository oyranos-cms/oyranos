{% include "source_file_header.txt" %}

#ifndef OY_{{ class_base_name|upper }}_S_H
#define OY_{{ class_base_name|upper }}_S_H

{% include "cpp_begin.h" %}

typedef struct {{ class_name }} {{ class_name }};

struct {{ class_name }} {
  oyOBJECT_e           type_;          /**< @private struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */
};

{% block GeneralPublicMethodsDeclaration %}{% endblock %}

{% block SpecificPublicMethodsDeclaration %}{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class_base_name|upper }}_S_H */
