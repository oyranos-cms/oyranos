{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%d",
             s->id
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%d %s",
             s->id, s->registration
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s id: %d",
             s->registration, s->id
           );
{% endblock %}
