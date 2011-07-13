{% extends "BaseList_s.c" %}

{% block MoveIn_Observed %}error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, OY_OBSERVE_AS_WELL );{% endblock %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_devices_internal.h"
{% endblock %}
