{% extends "Base_s_.c" %}

{% block refCount %}
  {
  uint32_t n = 0;
  int r OY_UNUSED = oyObject_UnRef(s->oy_);

  if(s->node)
    ++n;

  if(s->remote_socket_)
    ++n;

  if( r+1 < (int)n )
    WARNcc2_S( s, "reference count below internal references to other object(s): %s %s",
               s->node?"node":"", s->remote_socket_?"remote_socket_":"" );

  /* referenences from members has to be substracted
   * from this objects ref count */
  if(oyObject_GetRefCount( s->oy_ ) >= (int)(parent_refs + n + observer_refs*2) && r > 0)
     return 0;

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
  if(type == oyNAME_DESCRIPTION)
    sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s", s->relatives_?s->relatives_:"" );
{% endblock %}
