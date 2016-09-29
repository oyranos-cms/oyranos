{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
    sprintf( &text[strlen(text)], "%dx%d", s->width, s->height);
  else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "(%dx%d)%dc", s->width, s->height,
             s->layout_[oyCHANS]);
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "(%dx%d)%dc%s", s->width, s->height,
             s->layout_[oyCHANS],
             oyDataTypeToText( oyToDataType_m( s->layout_[oyLAYOUT] )));
{% endblock %}
