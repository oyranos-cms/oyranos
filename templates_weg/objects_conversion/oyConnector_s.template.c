{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#ifdef oyCheckCType__m
#undef oyCheckCType__m
#endif
#define oyCheckCType__m(a,b) ;
{% endblock %}

{% block Copy_CheckType %}oyCheckCType__m{% endblock %}
{% block Release_CheckType %}oyCheckCType__m{% endblock %}
