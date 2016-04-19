{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  if(type != oyNAME_NICK || (flags & 0x01))
    sprintf( &text[strlen(text)], "%gx%g+%g+%g", s->width, s->height, s->x, s->y);
{% endblock %}
