{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S_H
#define OY_{{ class.baseName|upper }}_S_H

{% include "cpp_begin.h" %}

#include <oyranos_object.h>

typedef struct {{ class.name }} {{ class.name }};
{% include class.public_h %}

{% block doxygenPublicClass %}
/** @brief Oyranos base structure
 *  @ingroup objects_generic
 *
 *  All Oyranos structures should be castable to this basic one.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
{% endblock %}
struct {{ class.name }} {
{% include "Struct_s.members.h" %}
};

{% block GeneralPublicMethodsDeclarations %}{% endblock %}

{% block SpecificPublicMethodsDeclarations %}
{% include "Struct_s.public_methods_declarations.h" %}
{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class.baseName|upper }}_S_H */
