{% extends "Base_s.c" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <oyranos_sentinel.h>
{% endblock %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyConfigs_s.h"
#include "oyCMMapiFilters_s.h"

#include "oyOption_s_.h"
#include "oyCMMapi8_s_.h"
{% endblock %}
