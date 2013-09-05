{% extends "Base_s_.h" %}

{% block LocalIncludeFiles %}
#include "oyStructList_s.h"
{% endblock %}

{% block ChildMembers %}
/* Include "BaseList_s_.h" { */
  oyStructList_s * list_;              /**< the list data */
/* } Include "BaseList_s_.h" */
{% endblock %}

{% block GeneralPrivateMethodsDeclarations %}
{{ block.super }}
#if 0
/* FIXME Are these needed at all?? */
int
           oy{{ class.baseName }}_MoveIn_         ( {{ class.privName }}      * list,
                                       {{ class.listOf }}       ** ptr,
                                       int                 pos );
int
           oy{{ class.baseName }}_ReleaseAt_      ( {{ class.privName }}      * list,
                                       int                 pos );
{{ class.listOf }} *
           oy{{ class.baseName }}_Get_            ( {{ class.privName }}      * list,
                                       int                 pos );
int
           oy{{ class.baseName }}_Count_          ( {{ class.privName }}      * list );
#endif
{% endblock %}

{% block CustomPrivateMethodsDeclarations %}{% endblock %}
