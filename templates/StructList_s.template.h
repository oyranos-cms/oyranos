{% extends "Base_s.h" %}

{% block ChildMembers %}
  oyStruct_s        ** ptr_;           /**< @private the list data */
  int                  n_;             /**< @private the number of visible pointers */
  int                  n_reserved_;    /**< @private the number of allocated pointers */
  char               * list_name;      /**< name of list */
  oyOBJECT_e           parent_type_;   /**< @private parents struct type */
{% endblock %}
