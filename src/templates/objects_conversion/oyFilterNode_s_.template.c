{% extends "Base_s_.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyHash_s.h"
#include "oyranos_module_internal.h"
#include "oyranos_object_internal.h"
{% endblock %}

{% block altConstructor %}(oyFilterNode_s_*)oyFilterNode_Create( filternode->api7_->registration, (oyFilterCore_s*){{ class.baseName|lower }}->core, object );{% endblock %}

{% block refCount %}
  {
  uint32_t s_n = 0, p_n = 0, i, n;
  int r OY_UNUSED = oyObject_UnRef(s->oy_);

  if(s->sockets)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 0, 0 );
    for(i = 0; i < n; ++i)
      if(s->sockets[i]) ++s_n;
  }

  if(s->plugs)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 1, 0 );
    for(i = 0; i < n; ++i)
      if(s->plugs[i]) ++p_n;
  }

  if(oyObject_GetRefCount( s->oy_ ) > (int)(s_n + p_n))
    return 0;

  oyObject_Ref(s->oy_);
  }
{% endblock %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%s",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_)
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %d/%d",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_), s->plugs_n_, s->sockets_n_
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "reg: %s\nrelatives: %s\nplugs: %d sockets: %d context: %s",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_), s->relatives_,
             s->plugs_n_, s->sockets_n_, oyNoEmptyString_m_(s->api7_?s->api7_->context_type:"???")
           );
{% endblock %}
