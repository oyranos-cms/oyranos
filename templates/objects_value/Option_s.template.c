{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_sentinel.h"
#include "oyObserver_s.h"
{% endblock %}
