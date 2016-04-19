{% extends "Base_s_.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyranos_module_internal.h"
#include "oyCMMapiFilters_s.h"
#include "oyCMMapi9_s_.h"
{% endblock %}

{% block customStaticMessage %}
{{ block.super }}
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%s",
             s->category_
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %s",
             s->category_, s->registration_
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "category: %s\nreg: %s",
             s->category_, s->registration_
           );
{% endblock %}
