{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_conversion.h"
#include "oyConnector_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterPlugs_s.h"
#include "oyFilterSocket_s.h"
#include "oyPixelAccess_s.h"
#include "oyImage_s.h"
{% endblock %}
