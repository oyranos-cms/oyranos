{% extends "BaseList_s_.h" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include "oyranos_config_internal.h"
#ifdef HAVE_LIBXML2
#include <libxml/parser.h>
#endif
{% endblock %}
