{% extends "Base_s_.c" %}

{% block altConstructor %}oyFilterNode_Create( {{ class.baseName|lower }}->core, object );{% endblock %}

{% block refCount %}
  {
  uint32_t s_n = 0, p_n = 0, i, n;
  oyObject_UnRef(s->oy_);

  if(s->sockets)
  {
    n = oyFilterNode_EdgeCount( s, 0, 0 );
    for(i = 0; i < n; ++i)
      if(s->sockets[i]) ++s_n;
  }

  if(s->plugs)
  {
    n = oyFilterNode_EdgeCount( s, 1, 0 );
    for(i = 0; i < n; ++i)
      if(s->plugs[i]) ++p_n;
  }

  if(oyObject_GetRefCount( s->oy_ ) > s_n + p_n)
    return 0;

  oyObject_Ref(s->oy_);
  }
{% endblock %}
