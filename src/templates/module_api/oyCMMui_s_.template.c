{% extends "Base_s_.c" %}

{% block refCount %}
  {
    uint32_t ui_p = s->parent ? 1 : 0;
    int r OY_UNUSED = oyObject_UnRef(s->oy_);

    /* references from members has to be substracted
     * from this objects ref count */
    if(oyObject_GetRefCount( s->oy_ ) > (int)(ui_p + observer_refs))
      return 0;
  }
{% endblock %}
