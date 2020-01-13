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
  if(oy_{{ class.baseName|lower }}_msg_text_n_ < l)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_{{ class.baseName|lower }}_msg_text_ && oy_{{ class.baseName|lower }}_msg_text_n_)
      dealloc( oy_{{ class.baseName|lower }}_msg_text_ );
    oy_{{ class.baseName|lower }}_msg_text_n_ = l;
    oy_{{ class.baseName|lower }}_msg_text_ = alloc(oy_{{ class.baseName|lower }}_msg_text_n_);
    if(oy_{{ class.baseName|lower }}_msg_text_)
      oy_{{ class.baseName|lower }}_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_{{ class.baseName|lower }}_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%d",
             l
           );
  } else
  if(type == oyNAME_NAME ||
     (int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
             hash_text?hash_text:"----"
           );
{% endblock %}
