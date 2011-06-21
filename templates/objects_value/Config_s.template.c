{% extends "Base_s.c" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <oyranos_sentinel.h>
{% endblock %}

{% block LocalIncludeFiles %}
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
