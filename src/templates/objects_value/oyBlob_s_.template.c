{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
             s->type
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s %lu",
             s->type, (long unsigned int)s->size
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "type: %s size: %lu flags: %d",
             s->type, (long unsigned int)s->size, s->flags
           );
{% endblock %}
