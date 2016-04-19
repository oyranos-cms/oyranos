{% extends "Base_s_.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyHash_s_.h"
#include "oyProfileTag_s_.h"
#include "oyStructList_s_.h"
#include "oyranos_io.h"
#include "oyranos_generic_internal.h"
{% endblock %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <oyranos_icc.h>
{% endblock %}

{% block customStaticMessage %}
{{ block.super }}
  /* allocate enough space */
  if(text_n < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_)
      dealloc = s->oy_->deallocateFunc_;
    if(text && text_n)
      dealloc( text );
    text_n = 1024;
    text = alloc(text_n);
    if(text)
      text[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type != oyNAME_NICK || (flags & 0x01))
    sprintf( &text[strlen(text)], "%s", oyProfile_GetText( (oyProfile_s*)s, type ));
{% endblock %}

