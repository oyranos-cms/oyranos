{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyBlob_s_.h"
#include "oyFilterNode_s_.h"
#include "oyFilterPlug_s_.h"
{% endblock %}
