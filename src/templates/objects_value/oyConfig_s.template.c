{% extends "Base_s.c" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
{% endblock %}

{% block LocalIncludeFiles %}
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
{{ block.super }}
#include "oyConfigs_s.h"
#include "oyOptions_s.h"
#include "oyCMMapiFilters_s.h"

#include "oyOption_s_.h"
#include "oyCMMapi8_s_.h"
{% endblock %}
