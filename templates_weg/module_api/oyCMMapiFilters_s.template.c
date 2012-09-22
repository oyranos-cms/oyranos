{% extends "BaseList_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_alpha_internal.h"
{% endblock %}

{% block MoveIn_CheckTypes %}oyIsOfTypeCMMapiFilter( (*obj)->type_ ){% endblock %}
