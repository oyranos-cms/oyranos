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
    text_n = 1000;
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
             s->registration?s->registration:""
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %s",
             s->registration?s->registration:"", s->context_type
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s%s%s",
             s->registration?s->registration:"", s->context_type[0]?"\ntype: ":"", s->context_type
           );
{% endblock %}

{% block refCount %}
  {
    uint32_t ui_p = s->ui->parent ? 1 : 0;
    int r OY_UNUSED = oyObject_UnRef(s->oy_);

    /* references from members has to be substracted
     * from this objects ref count */
    if(oyObject_GetRefCount( s->oy_ ) > (int)(ui_p + observer_refs))
      return 0;
  }
{% endblock %}
