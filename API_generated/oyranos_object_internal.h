
/** @file oyranos_object_internal.h

   [Template file inheritance graph]
   +-- oyranos_object_internal.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2011/02/15
 */


#ifndef OYRANOS_OBJECT_INTERNAL_H
#define OYRANOS_OBJECT_INTERNAL_H

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


#endif /* OYRANOS_OBJECT_INTERNAL_H */
