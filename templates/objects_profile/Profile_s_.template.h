{% extends "Base_s_.h" %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <icc34.h>
{% endblock %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyStructList_s.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "Profile.members.h" { */
{% include "Profile.members.h" %}
/* } Include "Profile.members.h" */
{% endblock %}
