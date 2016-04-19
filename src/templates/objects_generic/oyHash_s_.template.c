{% extends "Base_s_.c" %}

{% block oyClass_Copy_notObject %}
    if(s->entry && s->entry->copy)
      s->entry = s->entry->copy( s->entry, 0 );
{% endblock %}

{% block customStaticMessage %}
{{ block.super }}
  const char * hash_text = oyObject_GetName(s->oy_, oyNAME_NAME);
  int l = 0;
  if(hash_text)
    l = strlen(hash_text);

  /* allocate enough space */
  if(text_n < l)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_)
      dealloc = s->oy_->deallocateFunc_;
    if(text && text_n)
      dealloc( text );
    text_n = l;
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
    sprintf( &text[strlen(text)], "%d",
             l
           );
  } else
  if(type == oyNAME_NAME ||
     (int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s",
             hash_text?hash_text:"----"
           );
{% endblock %}
