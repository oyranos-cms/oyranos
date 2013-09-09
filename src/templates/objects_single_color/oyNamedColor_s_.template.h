{% extends "Base_s_.h" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include "oyranos_color.h"
#include "oyranos_image.h"

#include "oyProfile_s.h"
#include "oyImage_s.h"
#include "oyConversion_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "NamedColor.members.h" { */
{% include "NamedColor.members.h" %}
/* } Include "NamedColor.members.h" */
{% endblock %}
