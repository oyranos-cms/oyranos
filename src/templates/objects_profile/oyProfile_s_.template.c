{% extends "Base_s_.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyHash_s_.h"
#include "oyProfileTag_s_.h"
#include "oyStructList_s_.h"
#include "oyranos_cache.h"
#include "oyranos_io.h"
#include "oyranos_generic_internal.h"

#include <oyjl_macros.h>
{% endblock %}

{% block GlobalIncludeFiles %}
{{ block.super }}
#include <oyranos_icc.h>
{% endblock %}

{% block customStaticMessage %}
{{ block.super }}
  /* allocate enough space */
  if(oy_{{ class.baseName|lower }}_msg_text_n_ < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_{{ class.baseName|lower }}_msg_text_ && oy_{{ class.baseName|lower }}_msg_text_n_)
      dealloc( oy_{{ class.baseName|lower }}_msg_text_ );
    oy_{{ class.baseName|lower }}_msg_text_n_ = 1024;
    oy_{{ class.baseName|lower }}_msg_text_ = alloc(oy_{{ class.baseName|lower }}_msg_text_n_);
    if(oy_{{ class.baseName|lower }}_msg_text_)
      oy_{{ class.baseName|lower }}_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_{{ class.baseName|lower }}_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if((type == oyNAME_NICK && (flags & 0x01)) ||
      type == oyNAME_NAME)
  {
    if(s->file_name_)
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s",
               s->file_name_
             );
    else if(s->use_default_)
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%d",
               s->use_default_
             );
    else
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%lu",
               (long unsigned int)s->size_
             );
  } else
  if((int)type >= oyNAME_DESCRIPTION)
  {
    uint32_t * h = NULL;
    if(s->oy_)
      h = (uint32_t*)s->oy_->hash_ptr_;
    if(s->file_name_)
      sprintf( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%s\n",
               s->file_name_
             );
    if(h)
      oySprintf_( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], "%08x%08x%08x%08x", h[0], h[1], h[2], h[3]);
    if(s->use_default_)
      oySprintf_( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], " default: %d",
                  s->use_default_
                );
    oySprintf_( &oy_{{ class.baseName|lower }}_msg_text_[strlen(oy_{{ class.baseName|lower }}_msg_text_)], " %s channels: %d modified: %d",
                oyICCColorSpaceGetName(s->sig_), s->channels_n_, s->tags_modified_
              );
  }
{% endblock %}

