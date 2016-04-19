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
    if(s->lib_name)
      sprintf( &text[strlen(text)], "%s",
               strchr(s->lib_name,'/') ? strchr(s->lib_name,'/') + 1: s->lib_name
         );
    else
    if(s->func_name)
      sprintf( &text[strlen(text)], "%s",
               s->func_name
         );
    else
    if(s->id)
      sprintf( &text[strlen(text)], "%s",
               s->id
         );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s%s%s%s%s%s%s%d",
             s->lib_name?(strchr(s->lib_name,'/') ? strchr(s->lib_name,'/') + 1: s->lib_name):"", s->lib_name?"\n":"",
             s->func_name?s->func_name:"", s->func_name?"\n":"",
             s->id?s->id:"", s->id?" ":"",
             s->size?"size: ":"", s->size?s->size:-1
         );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s%s%s%s%s%s%s%d",
             s->lib_name?s->lib_name:"", s->lib_name?"\n":"",
             s->func_name?s->func_name:"", s->func_name?"\n":"",
             s->id?s->id:"", s->id?" ":"",
             s->size?"size: ":"", s->size?s->size:-1
         );
{% endblock %}
