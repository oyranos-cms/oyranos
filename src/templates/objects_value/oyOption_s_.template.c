{% extends "Base_s_.c" %}

{% block customStaticMessage %}
{{ block.super }}
  {
    const char * t = "";
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_) dealloc = s->oy_->deallocateFunc_;
#define AD alloc,dealloc
    if(!(flags & 0x01))
      oyStringAddPrintf( &oy_{{ class.baseName|lower }}_msg_text_, AD, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");


    if(type == oyNAME_NICK && (flags & 0x01))
    {
      t = oyOption_GetText((oyOption_s*)s,type);
      oyStringAddPrintf( &oy_{{ class.baseName|lower }}_msg_text_, AD, "%s",
             oyNoEmptyString_m_(t)
             );
    } else
    if(type == oyNAME_NAME)
    {
      t = oyOption_GetText((oyOption_s*)s, oyNAME_NICK);
      oyStringAddPrintf( &oy_{{ class.baseName|lower }}_msg_text_, AD, "id:%d %s",
             s->id, oyNoEmptyString_m_(t)
           );
    } else
    if((int)type >= oyNAME_DESCRIPTION)
    {
      t = oyOption_GetText((oyOption_s*)s, oyNAME_NICK);
      oyStringAddPrintf( &oy_{{ class.baseName|lower }}_msg_text_, AD, "%s\nid: %d type: %s source: %d flags: %d",
             oyNoEmptyString_m_(t),
             s->id,
             oyValueTypeText(s->value_type),
             s->source,
             s->flags
           );
    }
  }
{% endblock %}
