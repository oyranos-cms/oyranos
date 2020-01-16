{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#define OY_HASH_SIZE 16

#include <oyranos_object.h>

#include "oyStruct_s.h"
#include "oyOptions_s.h"
#include "oyName_s.h"


{% block GeneralPublicMethodsDeclarations %}
/* oyObject_s common object functions { */
oyObject_s   oyObject_New            ( const char        * name );
oyObject_s   oyObject_NewWithAllocators(oyAlloc_f          allocateFunc,
                                       oyDeAlloc_f         deallocateFunc,
                                       const char        * name );
oyObject_s   oyObject_NewFrom        ( oyObject_s          object,
                                       const char        * name );
oyObject_s   oyObject_Copy           ( oyObject_s          object );
int          oyObject_Release        ( oyObject_s        * oy );
/* } oyObject_s common object functions */
{% endblock %}

{% block SpecificPublicMethodsDeclarations %}
/* Include "{{ class.public_methods_declarations_h }}" { */
{% include class.public_methods_declarations_h %}
/* } Include "{{ class.public_methods_declarations_h }}" */
{% endblock %}

{% include "cpp_end.h" %}
#endif /* {{ file_name|underscores|upper|tr:". _" }} */
