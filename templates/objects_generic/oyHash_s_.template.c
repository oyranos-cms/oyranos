{% extends "Base_s_.c" %}

{% block oyClass_Copy_notObject %}
    if(s->entry && s->entry->copy)
      s->entry = s->entry->copy( s->entry, 0 );
{% endblock %}
