{% include "source_file_header.txt" %}
{% load gsoc_extensions %}
#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#define oy{{ class.baseName }}Priv_m( var ) (({{ class.privName }}*) (var))

typedef struct {{ class.privName }} {{ class.privName }};

{% block GlobalIncludeFiles %}{% endblock %}  
#include <oyranos_object.h>
{% block LocalIncludeFiles %}{% endblock %}  

#include "{{ class.name }}.h"

/* Include "{{ class.private_h }}" { */
{% include class.private_h %}
/* } Include "{{ class.private_h }}" */

/** @internal
 *  @struct   {{ class.privName }}
 *  @brief    {{ class.brief }}
 *  @ingroup  {{ class.group }}
 *  @extends  {{ class.parentName }}
 */
struct {{ class.privName }} {
{% block BaseMembers %}
/* Include "Struct.members.h" { */
{% include "Struct.members.h" %}
/* } Include "Struct.members.h" */
{% endblock %}
{% block ChildMembers %}{% endblock %}
};

{% block GeneralPrivateMethodsDeclarations %}
{{ class.privName }}*
  oy{{ class.baseName }}_New_( oyObject_s object );
{{ class.privName }}*
  oy{{ class.baseName }}_Copy_( {{ class.privName }} *{{ class.baseName|lower }}, oyObject_s object);
{{ class.privName }}*
  oy{{ class.baseName }}_Copy__( {{ class.privName }} *{{ class.baseName|lower }}, oyObject_s object);
int
  oy{{ class.baseName }}_Release_( {{ class.privName }} **{{ class.baseName|lower }} );
{% endblock %}

{% block SpecificPrivateMethodsDeclarations %}
/* Include "{{ class.private_methods_declarations_h }}" { */
{% include class.private_methods_declarations_h %}
/* } Include "{{ class.private_methods_declarations_h }}" */
{% endblock %}

{% block CustomPrivateMethodsDeclarations %}
void oy{{ class.baseName }}_Release__Members( {{ class.privName }} * {{ class.baseName|lower }} );
int oy{{ class.baseName }}_Init__Members( {{ class.privName }} * {{ class.baseName|lower }} );
int oy{{ class.baseName }}_Copy__Members( {{ class.privName }} * dst, {{ class.privName }} * src);
{% endblock CustomPrivateMethodsDeclarations %}

{% include "cpp_end.h" %}
#endif /* {{ file_name|underscores|upper|tr:". _" }} */
