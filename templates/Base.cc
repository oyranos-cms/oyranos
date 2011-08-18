{% include "source_file_header.txt" %}
{% load gsoc_extensions %}

#include "{{ class.baseName }}.hh"

using namespace oy;


{{ class.cppName }}::{{ class.cppName }}(Object object)
{
  m_oy = oy{{ class.baseName }}_New(object);
}

{{ class.cppName }}::~{{ class.cppName }}()
{
  oy{{ class.baseName }}_Release(&m_oy);
}

// Overloaded constructor methods
{% for function in class.functions %}{% if function.isConstructor %}
{{ class.cppName }}::{{ class.cppName }}({{ function.argsCPP|safe }})
{
  m_oy = oy{{ class.baseName }}_{{ function.name }}( {{ function.argNamesCPP }} );
}{% endif %}{% endfor %}

// Specific {{ class.cppName }} methods
{% for function in class.functions %}{% if not function.isStatic %}{% if not function.isConstructor %}
{{ function.returnType }}
{{ class.cppName }}::{{ function.name }}({% if function.argsCPP|safe %} {{ function.argsCPP|safe }} {% endif %})
{
  {% if not function.isVoid %}return {% endif %}oy{{ class.baseName }}_{{ function.name }}( m_oy{% if function.argNamesCPP %}, {{ function.argNamesCPP }}{% endif %} );
}{% endif %}{% endif %}
{% endfor %}

// Static functions
{% for function in class.functions %}{% if function.isStatic %}
{{ function.returnType }}
{{ class.cppName }}::{{ function.name }}({% if function.argsCPP|safe %} {{ function.argsCPP|safe }} {% endif %})
{
  {% if not function.isVoid %}return {% endif %}oy{{ class.baseName }}_{{ function.name }}( {{ function.argNamesCPP }} );
}{% endif %}
{% endfor %}
