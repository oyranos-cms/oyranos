{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
