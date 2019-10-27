{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  /* allocate enough space */
  if(oy_{{ class.baseName|lower }}_msg_text_n_ < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_)
      dealloc = s->oy_->deallocateFunc_;
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
  {
    if(s->lib_name)
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
               strchr(s->lib_name,'/') ? strchr(s->lib_name,'/') + 1: s->lib_name
         );
    else
    if(s->func_name)
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
               s->func_name
         );
    else
    if(s->id)
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
               s->id
         );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s%s%s%s%s%s%s%d",
             s->lib_name?(strchr(s->lib_name,'/') ? strchr(s->lib_name,'/') + 1: s->lib_name):"", s->lib_name?"\n":"",
             s->func_name?s->func_name:"", s->func_name?"\n":"",
             s->id?s->id:"", s->id?" ":"",
             s->size?"size: ":"", s->size?s->size:-1
         );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s%s%s%s%s%s%s%d",
             s->lib_name?s->lib_name:"", s->lib_name?"\n":"",
             s->func_name?s->func_name:"", s->func_name?"\n":"",
             s->id?s->id:"", s->id?" ":"",
             s->size?"size: ":"", s->size?s->size:-1
         );
{% endblock %}
