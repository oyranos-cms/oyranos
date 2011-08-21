{% extends "oyCMMapi_s.template.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_image_internal.h"
#include "oyConnectorImaging_s_.h"
#include "oyFilterPlug_s_.h"
#include "oyImage_s_.h"
{% endblock %}
