{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S_H
#define OY_{{ class.baseName|upper }}_S_H

{% include "cpp_begin.h" %}

typedef struct {{ class.name }} {{ class.name }};

{% block doxygenPublicClass %}{% endblock %}
struct {{ class.name }} {
  oyOBJECT_e           type_;          /**< @private struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */
};

{% block GeneralPublicMethodsDeclarations %}{% endblock %}

{% block SpecificPublicMethodsDeclarations %}{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class.baseName|upper }}_S_H */
