{% extends "Base_s_.h" %}

{% block ChildMembers %}
  oyStructList_s     * list_;          /**< the list data */
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
