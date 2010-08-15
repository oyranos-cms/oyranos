{% extends "Base_s.h" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <icc34.h>
{% endblock %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_profile.h"
{% endblock %}
