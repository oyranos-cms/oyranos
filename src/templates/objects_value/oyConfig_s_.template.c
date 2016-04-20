{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%d",
             s->id
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%d %s",
             s->id, s->registration
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s id: %d",
             s->registration, s->id
           );
{% endblock %}
