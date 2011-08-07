{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_image.h"
#include "oyOptions_s.h"
#include "oyImage_s.h"
#include "oyProfile_s.h"
#include "oyFilterNode_s.h"
#include "oyPixelAccess_s.h"
{% endblock %}
