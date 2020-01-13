{% extends "oyCMMapiFilter_s_.template.c" %}

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
  {
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
             s->registration
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s %d/%d",
             s->registration?s->registration:_("Start"), s->plugs_n, s->sockets_n
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
  {
    int i = 0;
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "reg: %s\nplugs: %d sockets: %d context: %s",
             s->registration,
             s->plugs_n, s->sockets_n, s->context_type
           );
    if(s->properties)
    while(s->properties[i])
    {
      if(i == 0)
        sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "\nproperties: "
           );
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s ",
               s->properties[i++]
           );
    }
  }
{% endblock %}
