{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_devices_internal.h"
#include "oyranos_cache.h"
#include "oyranos_color.h"
#include "oyranos_color_internal.h"
#include "oyranos_io.h"
#include "oyranos_icc.h"
#include "oyConfig_s_.h"
#include "oyProfileTag_s_.h"
{% endblock %}
