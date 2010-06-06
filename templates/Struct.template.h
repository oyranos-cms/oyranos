{% include "source_file_header.txt" %}

#ifndef OY_{{ class_name|upper }}_H
#define OY_{{ class_name|upper }}_H

typedef struct {% block ClassName %}oy{{ class_name }}_s* oy{{ class_name }}{% endblock %};

{% block ClassMethods %}
const char * oy{{ class_name }}_GetText( oyStruct_s * obj, oyNAME_e name_type, uint32_t flags );
{% endblock %}

#endif /* OY_{{ class_name|upper }}_H */
