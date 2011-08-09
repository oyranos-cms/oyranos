{% include "source_file_header.txt" %}
{% load gsoc_extensions %}

#include "{{ class.baseName }}"

using namespace oy;


{{ class.cppName }}::{{ class.cppName }}(Object object = NULL)
{
  m_oy = oy{{ class.baseName }}_New(object);
}

{{ class.cppName }}::~{{ class.cppName }}()
{
  oy{{ class.baseName }}_Release(&m_oy);
}

{% for function in class.functions %}
{{ class.cppName }}::{{ function.returnType }}
{{ function.name }}({{ function.args }})
{
  oy{{ class.baseName }}_{{ function }}( m_oy, {{ function.argNames }} );
}
{% endfor %}
