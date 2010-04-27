{% extends "Struct.template.h" %}

{% block ChildMembers %}
   /* CMMapi base class start */
   oyCMMInit_f      oyCMMInit;
   oyCMMMessageFuncSet_f oyCMMMessageFuncSet;
   const char     * registration;
   int32_t          version[3];
   int32_t          module_api[3];
   char           * id_;
   /* CMMapi base class stop */
{% endblock %}
