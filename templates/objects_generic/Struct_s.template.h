{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S_H
#define OY_{{ class.baseName|upper }}_S_H

{% include "cpp_begin.h" %}

{% block GlobalIncludeFiles %}{% endblock %}  
#include <oyranos_object.h>
{% block LocalIncludeFiles %}{% endblock %}  

typedef struct {{ class.name }} {{ class.name }};
/* Include "{{ class.public_h }}" { */
{% include class.public_h %}
/* } Include "{{ class.public_h }}" */

{% block doxygenPublicClass %}
/* Include "{{ class.dox }}" { */
{% include class.dox %}
/* } Include "{{ class.dox }}" */
{% endblock %}
struct {{ class.name }} {
/* Include "Struct.members.h" { */
{% include "Struct.members.h" %}
/* } Include "Struct.members.h" */
};

{% block GeneralPublicMethodsDeclarations %}{% endblock %}

{% block SpecificPublicMethodsDeclarations %}
/* Include "{{ class.public_methods_declarations_h }}" { */
{% include class.public_methods_declarations_h %}
/* } Include "{{ class.public_methods_declarations_h }}" */
{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class.baseName|upper }}_S_H */
