{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module_internal.h"
#include "oyCMMapi3_s_.h"
#include "oyBlob_s.h"
{% endblock %}
