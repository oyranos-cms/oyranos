{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%s",
             s->type
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %lu",
             s->type, s->size
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "type: %s size: %lu flags: %d",
             s->type, s->size, s->flags
           );
{% endblock %}
