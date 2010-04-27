{% extends "CMMapi_impl.template.h" %}

{% block ChildMembers %}
{{ block.super }}
   /* CMMapiFilter base class start */
   oyCMMapi5_s    * api5_;
   /* CMMapiFilter base class stop */
{% endblock %}
