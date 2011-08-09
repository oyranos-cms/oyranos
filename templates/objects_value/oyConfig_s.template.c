{% extends "Base_s.c" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
{% endblock %}

{% block LocalIncludeFiles %}
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
{{ block.super }}
#include "oyConfigs_s.h"
#include "oyOptions_s.h"
#ifdef UNHIDE_CMM
#include "oyCMMapiFilters_s.h"
#endif

#include "oyOption_s_.h"
#ifdef UNHIDE_CMM
#include "oyCMMapi8_s_.h"
#endif
{% endblock %}
