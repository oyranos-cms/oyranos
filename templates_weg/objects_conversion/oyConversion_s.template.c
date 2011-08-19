{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module_internal.h"
#include "oyCMMapiFilters_s.h"
#include "oyFilterGraph_s.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterNode_s_.h"
#include "oyImage_s.h"
#include "oyImage_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyPixelAccess_s_.h"
{% endblock %}
