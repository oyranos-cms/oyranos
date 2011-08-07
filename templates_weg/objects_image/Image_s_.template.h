{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_image.h"
#include "oyRectangle_s.h"
#include "oyProfile_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Image.members.h" { */
{% include "Image.members.h" %}
/* } Include "Image.members.h" */
{% endblock %}
