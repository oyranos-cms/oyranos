{% include "source_file_header.txt" %}
{% load gsoc_extensions %}
#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% block GlobalIncludeFiles %}{% endblock %}  
{% block LocalIncludeFiles %}{% endblock %}  
#include "{{ class.name }}.h"

namespace oy {

class {{ class.cppName }} {
  private:
    {{ class.privName }} * m_oy;

  public:
    {{ class.cppName }}(Object object = NULL);
    //{{ class.cppName }}(const {{ class.name }}* c);
    //{{ class.cppName }}(const {{ class.privName }}* c);
    //{{ class.cppName }}(const {{ class.cppName }}& c);
    ~{{ class.cppName }}();

    // Member functions
    {% for function in class.functions %}
    {{ function.returnType }}
    {{ function.name }}({{ function.args }});
    {% endfor %}
} // class {{ class.cppName }}

} // namespace oy
#endif // {{ file_name|underscores|upper|tr:". _" }}
