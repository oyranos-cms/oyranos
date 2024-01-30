{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
#include "oyConnector_s.h"
#include "oyFilterCore_s.h"
#include "oyFilterPlug_s.h"
#include "oyFilterSocket_s.h"
#include "oyPixelAccess_s.h"
#include "oyPointer_s.h"
{{ block.super }}
{% endblock %}
