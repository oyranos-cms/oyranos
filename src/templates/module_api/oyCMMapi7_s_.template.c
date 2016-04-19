{% extends "oyCMMapiFilter_s_.template.c" %}

{% block customStaticMessage %}
{{ block.super }}
  /* allocate enough space */
  if(text_n < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_)
      dealloc = s->oy_->deallocateFunc_;
    if(text && text_n)
      dealloc( text );
    text_n = 1024;
    text = alloc(text_n);
    if(text)
      text[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%s",
             s->registration
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %d/%d",
             s->registration, s->plugs_n, s->sockets_n
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
  {
    int i = 0;
    sprintf( &text[strlen(text)], "reg: %s\nid: %s\nplugs: %d sockets: %d context: %s",
             s->registration, s->id_,
             s->plugs_n, s->sockets_n, s->context_type
           );
    if(s->properties)
    while(s->properties[i])
    {
      if(i == 0)
        sprintf( &text[strlen(text)], "\nproperties: "
           );
      sprintf( &text[strlen(text)], "%s ",
               s->properties[i++]
           );
    }
  }
{% endblock %}
