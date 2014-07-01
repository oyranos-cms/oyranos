{% extends "BaseList_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module_internal.h"
{% endblock %}

{% block MoveIn_CheckTypes %}(*obj)->type_ >= oyOBJECT_CMM_API1_S &&
     (*obj)->type_ <= oyOBJECT_CMM_API_MAX{% endblock %}
{% block Get_RefType %}oyStructList_GetRef( s->list_, pos ){% endblock %}
