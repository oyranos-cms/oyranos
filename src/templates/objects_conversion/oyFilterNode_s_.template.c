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
  uint32_t s_n = 0, p_n = 0, sn_n = 0, pn_n = 0, i, n;
  int r OY_UNUSED = oyObject_UnRef(s->oy_);

  if(s->sockets)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 0, 0 );
    for(i = 0; i < n; ++i)
      if(s->sockets[i])
      {
        if(s->sockets[i]->node)
          ++sn_n;
        ++s_n;
      }
  }

  if(s->plugs)
  {
    n = oyFilterNode_EdgeCount( (oyFilterNode_s*)s, 1, 0 );
    for(i = 0; i < n; ++i)
      if(s->plugs[i])
      {
        if(s->plugs[i]->node)
          ++pn_n;
        ++p_n;
      }
  }

  /* referenences from members has to be substracted
   * from this objects ref count */
  if(oyObject_GetRefCount( s->oy_ ) > (int)(parent_refs + sn_n + pn_n + observer_refs*2))
    return 0;

  if(oy_debug_objects >= 0 && s->oy_)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      fprintf(stderr, "prepare destruct %s[%d] refs: %d sockets: %d|%d plugs: %d|%d\n", track_name, s->oy_->id_, s->oy_->ref_, s_n,sn_n, p_n,pn_n);
    }
  }

  /* ref before oyXXX_Release__Members(), so the
   * oyXXX_Release() is not called twice */
  oyObject_Ref(s->oy_);
  }
{% endblock %}

{% block customDestructor %}
  /* unref after oyXXX_Release__Members() */
  oyObject_UnRef(s->oy_);
{% endblock customDestructor %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_)
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s %d/%d",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_), s->plugs_n_, s->sockets_n_
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "reg: %s\nrelatives: %s\nplugs: %d sockets: %d context: %s",
             oyNoEmptyString_m_(s->api7_?s->api7_->registration:s->relatives_), s->relatives_,
             s->plugs_n_, s->sockets_n_, oyNoEmptyString_m_(s->api7_?s->api7_->context_type:"???")
           );
{% endblock %}
