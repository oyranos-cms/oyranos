{% extends "BaseList_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_sentinel.h"
{% endblock %}

    {% block MoveIn_Observed %}{
      if(oyStruct_IsObserved((oyStruct_s*)s, 0))
        oyStruct_ObserverAdd( (oyStruct_s*)*obj, (oyStruct_s*)s->list_, 0, 0 );
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos,
                                   OY_OBSERVE_AS_WELL );
    }{% endblock %}
