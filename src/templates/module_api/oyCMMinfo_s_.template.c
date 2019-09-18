{% extends "Base_s_.c" %}

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
             s->cmm
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %s",
             s->cmm, s->backend_version
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
  {
    sprintf( &text[strlen(text)], "%s %s %d",
             s->cmm, s->backend_version, s->oy_compatibility
           );
  }
{% endblock %}
