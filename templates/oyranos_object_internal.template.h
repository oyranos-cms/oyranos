{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|cut:".h"|upper }}_H
#define {{ file_name|cut:".h"|upper }}_H

#include <string.h>

#include <oyranos_helper.h>
#include <oyranos_debug.h>
#include <oyranos_texts.h>

#include "oyranos_object.h"
#include "oyCMMapiFilters_s.h"

#define oyCheckType_m( typ, action ) \
  if( !s || s->type != typ) \
  { \
    WARNc3_S( "%s %s(%s)", _("Unexpected object type:"), \
              oyStructTypeToText( s ? s->type : oyOBJECT_NONE ), \
              oyStructTypeToText( typ )) \
    action; \
  }
#define oyCheckType__m( type, action ) \
  if( !s || s->type_ != type) \
  { \
    WARNc3_S( "%s %s(%s)", _("Unexpected object type:"), \
              oyStructTypeToText( s ? s->type_ : oyOBJECT_NONE ), \
              oyStructTypeToText( type )) \
    action; \
  }

oyCMMapiFilters_s * oyCMMsGetFilterApis_(const char        * cmm_required,
                                         const char        * registration,
                                         oyOBJECT_e          type,
                                         uint32_t         ** rank_list,
                                         uint32_t          * count );


#endif /* {{ file_name|cut:".h"|upper }}_H */
