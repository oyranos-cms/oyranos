{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  /* allocate enough space */
  if(oy_{{ class.baseName|lower }}_msg_text_n_ < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_{{ class.baseName|lower }}_msg_text_ && oy_{{ class.baseName|lower }}_msg_text_n_)
      dealloc( oy_{{ class.baseName|lower }}_msg_text_ );
    oy_{{ class.baseName|lower }}_msg_text_n_ = 1024;
    oy_{{ class.baseName|lower }}_msg_text_ = alloc(oy_{{ class.baseName|lower }}_msg_text_n_);
    if(oy_{{ class.baseName|lower }}_msg_text_)
      oy_{{ class.baseName|lower }}_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_{{ class.baseName|lower }}_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s%d",
             s->n_?"n: ":"", s->n_
           );
  else
  if(type == oyNAME_NAME)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s%s%s%d",
             s->list_name?s->list_name:"", s->list_name?"\n":"",
             s->n_?" n: ":"", s->n_
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s%s%s%s%s%d",
             s->list_name?s->list_name:"", s->list_name?"\n":"",
             s->parent_type_?"parent: ":"", s->parent_type_?oyStructTypeToText(s->parent_type_):"",
             " n: ", s->n_
           );
{% endblock %}
