{% extends "Base_s_.c" %}

{% block customConstructor %}
  s->list_ = oyStructList_Create( s->type_, 0, 0 );
{% endblock customConstructor %}

{% block copyConstructor %}
    oyStructList_Release( &s->list_ );
    s->list_ = oyStructList_Copy( {{ class.baseName|lower }}->list_, s->oy_ );
{% endblock copyConstructor %}

{% block Container %}list{% endblock %}

{% block customDestructor %}
  oyStructList_Release( &s->list_ );
{% endblock customDestructor %}
