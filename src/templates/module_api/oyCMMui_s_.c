{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  /* allocate enough space */
  int l = 0;
  if(s->options)
    l = strlen(s->options);
  if(text_n < l+80)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_)
      dealloc = s->oy_->deallocateFunc_;
    if(text && text_n)
      dealloc( text );
    text_n = l+80;
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
             s->category
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "category: %s",
             s->category
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "category: %s options:\n%s",
             s->category,
             s->options
           );
{% endblock %}
