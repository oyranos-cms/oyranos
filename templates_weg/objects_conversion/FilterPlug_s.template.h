{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_conversion.h"
#include "oyFilterSocket_s.h"
#include "oyPixelAccess_s.h"
#include "oyImage_s.h"
{% endblock %}
