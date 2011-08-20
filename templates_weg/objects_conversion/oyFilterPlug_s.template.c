{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyFilterPlugs_s.h"
#include "oyImage_s_.h"
#include "oyPixelAccess_s_.h"
{% endblock %}
