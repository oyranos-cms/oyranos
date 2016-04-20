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

  if((type == oyNAME_NICK && (flags & 0x01)) ||
      type == oyNAME_NAME)
  {
    if(s->file_name_)
      sprintf( &text[strlen(text)], "%s",
               s->file_name_
             );
    else if(s->use_default_)
      sprintf( &text[strlen(text)], "%d",
               s->use_default_
             );
    else
      sprintf( &text[strlen(text)], "%lu",
               s->size_
             );
  } else
  if((int)type >= oyNAME_DESCRIPTION)
  {
    uint32_t * h = (uint32_t*)s->oy_->hash_ptr_;
    if(s->file_name_)
      sprintf( &text[strlen(text)], "%s\n",
               s->file_name_
             );
    if(h)
      oySprintf_( &text[strlen(text)], "%08x%08x%08x%08x", h[0], h[1], h[2], h[3]);
    if(s->use_default_)
      oySprintf_( &text[strlen(text)], " default: %d",
                  s->use_default_
                );
    oySprintf_( &text[strlen(text)], " %s channels: %d modified: %d",
                oyICCColorSpaceGetName(s->sig_), s->channels_n_, s->tags_modified_
              );
  }
{% endblock %}

