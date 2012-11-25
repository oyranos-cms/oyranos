{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_image.h"
#include "oyArray2d_s.h"
#include "oyFilterPlug_s.h"
#include "oyFilterGraph_s.h"
#include "oyImage_s.h"
#include "oyOptions_s.h"
#include "oyRectangle_s.h"
{% endblock %}
