{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_generic.h"
#include "oyranos_module_internal.h"
#include "oyOptions_s.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterSocket_s_.h"
#include "oyConnector_s_.h"
#include "oyStructList_s.h"
#include "oyFilterCore_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyCMMapiFilters_s.h"
#include "oyPointer_s_.h"
{% endblock %}
