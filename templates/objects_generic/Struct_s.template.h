{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S_H
#define OY_{{ class.baseName|upper }}_S_H

{% include "cpp_begin.h" %}

#include <oyranos_object.h>

typedef struct {{ class.name }} {{ class.name }};
{% include class.public_h %}

{% block doxygenPublicClass %}
{% include "Struct.dox" %}
{% endblock %}
struct {{ class.name }} {
{% include "Struct.members.h" %}
};

{% block GeneralPublicMethodsDeclarations %}{% endblock %}

{% block SpecificPublicMethodsDeclarations %}
{% include "Struct.public_methods_declarations.h" %}
{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class.baseName|upper }}_S_H */
