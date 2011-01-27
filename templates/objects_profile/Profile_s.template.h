{% extends "Base_s.h" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <icc34.h>
{% endblock %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos.h"
#include "oyranos_profile.h"
#include "oyProfileTag_s.h"
#include "oyConfig_s.h"
{% endblock %}
