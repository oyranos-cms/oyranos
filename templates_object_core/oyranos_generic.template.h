{% load gsoc_extensions %}{% include "source_file_header.txt" %}

#ifndef {{ file_name|cut:".h"|upper }}_H
#define {{ file_name|cut:".h"|upper }}_H


{% include "cpp_begin.h" %}


{% include "cpp_end.h" %}

#endif /* {{ file_name|cut:".h"|upper }}_H */
