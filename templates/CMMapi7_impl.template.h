{% extends "CMMapiFilter_impl.template.h" %}

{% block ChildMembers %}
{{ block.super }}
   /* CMMapi7 base class start */
   oyCMMFilterPlug_Run_f oyCMMFilterPlug_Run;
   char             context_type[8];
   oyConnector_s ** plugs;
   uint32_t         plugs_n;
   uint32_t         plugs_last_add;
   oyConnector_s ** sockets;
   uint32_t         sockets_n;
   uint32_t         sockets_last_add;
   /* CMMapi7 base class stop */
{% endblock %}
