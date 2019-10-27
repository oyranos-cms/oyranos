{% extends "Base_s_.c" %}

{% block altConstructor %}(oyArray2d_s_*)oyArray2d_Create( 0, array2d->height, 0, array2d->t, object );{% endblock %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%dx%d", s->width, s->height);
  else
  if(type == oyNAME_NAME)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "(%dx%d)%dt", s->width, s->height, s->t);
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "(%dx%d)%s", s->width, s->height, oyDataTypeToText(s->t));
{% endblock %}
