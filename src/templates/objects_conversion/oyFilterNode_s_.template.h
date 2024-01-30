{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyCMMapi7_s_.h"
#include "oyBlob_s_.h"
#include "oyFilterNodes_s_.h"
#include "oyFilterPlugs_s_.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterCore_s_.h"
#include "oyFilterSocket_s.h"
#include "oyObserver_s_.h"
{% endblock %}

{% block ChildMembers %}
{{ block.super }}
/* Include "FilterNode.members.h" { */
{% include "FilterNode.members.h" %}
/* } Include "FilterNode.members.h" */
{% endblock %}
