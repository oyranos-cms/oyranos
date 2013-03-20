{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|cut:".h"|upper }}_H
#define {{ file_name|cut:".h"|upper }}_H

#include <string.h>
#include <math.h>

#include <oyranos_object.h>
#include <oyranos_helper.h>
#include <oyranos_debug.h>
#include <oyranos_i18n.h>
#include <oyranos_string.h>

#include "oyranos_object.h"
/*#include "oyCMMapiFilters_s.h"*/

#define hashTextAdd_m( text_ ) \
  oyStringAdd_( &hash_text, text_, s->oy_->allocateFunc_, \
                            s->oy_->deallocateFunc_ );

int oyCheckType_( oyOBJECT_e type1, oyOBJECT_e type2 );

#define oyCheckType_m( typ, action ) \
  if( !s || oyCheckType_(s->type_, typ)) \
  { \
    WARNc3_S( "%s %s(%s)", _("Unexpected object type:"), \
              oyStructTypeToText( s ? s->type : oyOBJECT_NONE ), \
              oyStructTypeToText( typ )) \
    action; \
  }
#define oyCheckType__m( type, action ) \
  if( !s || oyCheckType_( s->type_, type )) \
  { \
    WARNc3_S( "%s %s(%s)", _("Unexpected object type:"), \
              oyStructTypeToText( s ? s->type_ : oyOBJECT_NONE ), \
              oyStructTypeToText( type )) \
    action; \
  }

#define OY_ERR if(l_error != 0) error = l_error;

int    oyTextIccDictMatch            ( const char        * text,
                                       const char        * pattern,
                                       double              delta );
int    oyPointerRelease              ( oyPointer         * ptr );

#endif /* {{ file_name|cut:".h"|upper }}_H */
