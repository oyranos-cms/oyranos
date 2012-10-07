{% extends "BaseList_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module_internal.h"
{% endblock %}

{% block MoveIn_CheckTypes %}oyIsOfTypeCMMapiFilter( (*obj)->type_ ){% endblock %}
{% block Get_RefType %}oyStructList_GetRef( s->list_, pos ){% endblock %}
