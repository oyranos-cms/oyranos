{% extends "BaseList_s.c" %}

    {% block MoveIn_Observed %}{
      if(oyStruct_IsObserved((oyStruct_s*)s, 0))
        oyStruct_ObserverAdd( (oyStruct_s*)*obj, (oyStruct_s*)s->list, 0, 0 );
      error = oyStructList_MoveIn( s->list, (oyStruct_s**)obj, pos,
                                   OY_OBSERVE_AS_WELL );
    }{% endblock %}
