{% include "source_file_header.txt" %}
{% load gsoc_extensions %}

#include "{{ class.baseName }}"

using namespace oy;


{{ class.cppName }}::{{ class.cppName }}(Object object)
{
  m_oy = oy{{ class.baseName }}_New(object);
}

{{ class.cppName }}::~{{ class.cppName }}()
{
  oy{{ class.baseName }}_Release(&m_oy);
}

{% for function in class.functions %}
{{ function.returnType }}
{{ class.cppName }}::{{ function.name }}({% if function.args %} {{ function.args }} {% endif %})
{
  {% if not function.isVoid %}return {% endif %}oy{{ class.baseName }}_{{ function.name }}( m_oy{% if function.argNames %}, {{ function.argNames }}{% endif %} );
}
{% endfor %}
