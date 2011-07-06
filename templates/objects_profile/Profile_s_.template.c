{% extends "Base_s_.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyHash_s_.h"
#include "oyProfileTag_s_.h"
#include "oyStructList_s_.h"
#include "oyranos_io.h"
#include "oyranos_generic_internal.h"
{% endblock %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <oyranos_icc.h>
{% endblock %}
