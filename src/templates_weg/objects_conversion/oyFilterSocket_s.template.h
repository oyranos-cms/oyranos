{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_conversion.h"
#include "oyranos_module.h"
#include "oyConnector_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterPlug_s.h"
#include "oyFilterPlugs_s.h"
{% endblock %}
