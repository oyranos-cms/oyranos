{% extends "BaseList_s_.h" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "List.members.h" { */
{% include "List.members.h" %}
/* } Include "List.members.h" */
{% endblock %}

{% block CustomPrivateMethodsDeclarations %}
void oy{{ class.baseName }}_Release__Members( {{ class.privName }} * {{ class.baseName|lower }} );
int oy{{ class.baseName }}_Init__Members( {{ class.privName }} * {{ class.baseName|lower }} );
int oy{{ class.baseName }}_Copy__Members( {{ class.privName }} * dst, {{ class.privName }} * src);
{% endblock CustomPrivateMethodsDeclarations %}

